/*
 * mode-syspopup
 *
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <efl_assist.h>

#include <appcore-efl.h>
#include <aul.h>
#include <pkgmgr-info.h>
/*#include <package-manager.h> */

#include <device.h>
#include <dd-display.h>
#include <dd-deviced.h>



#include <syspopup.h>
#include <mode-syspopup.h>
#include <mode-syspopup-alarmmgr.h>
#include <dd-haptic.h>
#include <dd-led.h>
#include <tapi_common.h>
#include <ITapiSim.h>
#include <ITapiModem.h>
#include <TapiUtility.h>
#include <TelPower.h>
#include <sound_manager.h>
#include <app_preference.h>
#include <bundle_internal.h>
#include <notification_text_domain.h>
#include <notification_internal.h>
#include <setting-debug.h>
#include <setting-common-data-error.h>

#include "setting-common-general-func.h"


void play_text(struct appdata *ad, const char *text);

static bool is_recovery = false;
static int normal_settings[22] = { -1, -1, -1, -1, -1, -1, -1, -3, -1, -1, -1, -1, -1 /*and pws profile*/, -1, -1, -1, -1, -1, -1, -1, 0, 0};
static int powerful_settings_def[22] = {0, 0, 0, 20, 0, 15, 0, 0, 0, 1, 1, -1, -1 /*and pws profile*/, -1, -1, -1, -1, -1, -1, -1, 0, 0};
static int powerful_settings[22] = { -2, -2, -2, -2, -2, -2, -2, -4, -2, -2, -2, -2, -2 /*and pws profile*/, -1, -1, -1, -1, -1, -1, -1, 0, 0};
static int emergency_settings[22] = {0, 0, 0, 20, 0, 15, 0, 0, 1, 1, 1, 1, 1 /*and pws profile*/, 0, 0, 0, 0, 0, 0, 0, 1, 1};
/*brightness default as 20 changed 20131127 */
/*lcd freq default as 1 in setting spec */

/* default value for power saving
* Power saving mode default
Haptic feedback : Off
Apps opening effect : Off
Use GPS satellites : Off
----------------------------------------
Brightness : 30%
Screen rotation: Off
Screen timeout : 15sec
Smart stay : off
Touch key light duration : Always off
Greyscale mode: Off
----------------------------------------
CPU max frequency limit : On
Screen output : On
Eco wallpaper : Off
*/


static Eina_List *mini_list;
static int __mode_reboot_noti_method();
static int __mode_syspopup_app_terminate(void *data);


/*dbus for CPU freq control*/
static int __append_param(DBusMessage *msg, const char *sig, char *param[])
{
	DBusMessageIter iter;
	DBusMessageIter cont_iter;
	char *ch;
	char *str_type;
	int int_type;
	int i;
	if (sig == NULL || param == NULL)
		return 0;

	dbus_message_iter_init_append(msg, &iter);
	for (ch = (char *)sig, i = 0; *ch != '\0'; ++i, ++ch) {
		_DBG("sig : %c", *ch);
		switch (*ch) {
			case 'i':
				int_type = atoi(param[i]);
				_DBG("param[%2d] : %d", i, int_type);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &int_type);
				break;
			case 's':
				str_type = param[i];
				_DBG("param[%2d] : %s", i, str_type);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str_type);
				break;
			default:
				break;
		}
	}
	return 0;
}

static DBusMessage *__invoke_dbus_method(const char *dest, const char *path,
                                         const char *interface, const char *method,
                                         const char *sig, char *param[])
{
	DBusError err;
	DBusConnection *conn;
	DBusMessage *reply;
	DBusMessage *msg;
	int r;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	if (conn == NULL) {
		_DBG("dbus_bus_get error");
		return NULL;
	}
	msg = dbus_message_new_method_call(dest, path, interface, method);
	if (msg == NULL) {
		_DBG("dbus_message_new_method_call error");
		return NULL;
	}
	r = __append_param(msg, sig, param);
	if (r < 0) {
		_DBG("append_param error");
		return NULL;
	}
	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_REPLY_TIMEOUT, &err);
	dbus_message_unref(msg);
	if (dbus_error_is_set(&err)) {
		_DBG("dbus_connection_send_with_reply_and_block error");
		_DBG("error [%s:%s]", err.name, err.message);
		dbus_error_free(&err);
		return NULL;
	}
	return reply;
}

int __set_LCD_freq_control(int value)
{
	int ret = 0;
	int state;
	DBusMessage *msg;
	DBusError err;
	char *arr[4] = {0,};
	char temp1[2] = {0,};
	char temp[3] = {0,};

	e_dbus_init();
	dbus_error_init(&err);

	snprintf(temp1, 2, "%1d", 0);	/*kernal parsing setting with value 0 */
	temp1[1] = '\0';
	_DBG("set as [%s]", temp1);

	snprintf(temp, 3, "%1d", value);
	temp[2] = '\0';
	_DBG("set as [%s]", temp);
	arr[0] = temp1;
	arr[1] = temp;

	msg = __invoke_dbus_method(BUS_NAME, DEVICED_PATH_DISPLAY, DEVICED_INTERFACE_DISPLAY, METHOD_NAME, "ii", arr);
	if (msg) {
		if (!dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &state, DBUS_TYPE_INVALID)) {
			_DBG("no message : [%s:%s]", err.name, err.message);
			ret = -1;	/*RETURN FAIL */
		} else {
			_DBG("%s-%s : %d", DEVICED_INTERFACE_DISPLAY, METHOD_NAME, state);
		}
		dbus_message_unref(msg);
	}
	e_dbus_shutdown();

	return ret;	/*succuess */
}

static Eina_Bool _LCD_freq_ctrl(void *data)
{
	struct appdata *ad = data;
	_DBG("_LCD_freq_ctrl");

	/*lcd freq ctrl, it effect flickering while progressing on change home */
	/*get master key psmode */
	int m_key;
	vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);

	_DBG("ad->type: %s, vconfkey : %d", ad->type, m_key);
	if (!strcmp(ad->type, MODE_SYSTEM_POPUP_EMERGENCY) || !strcmp(ad->type, MODE_SYSTEM_POPUP_EMERGENCY_SETTING)) {
		if (ad->is_lcd_ctrl) {
			/*normal -> emergency */
			_DBG("set LCD FRQ ON, 40");
			__set_LCD_freq_control(LCD_FRQ_CONTROL_ON);
		}
	} else if (!strcmp(ad->type, MODE_SYSTEM_POPUP_NORMAL) || !strcmp(ad->type, MODE_SYSTEM_POPUP_NORMAL_SETTING)) {
		/*except powerful -> normal */
		if (!ad->is_lcd_ctrl) {
			/*emergency -> normal */
			_DBG("set LCD FRQ OFF, 60");
			__set_LCD_freq_control(LCD_FRQ_CONTROL_OFF);
		}
	}
	return FALSE;
}

static Eina_Bool _LCD_freq_ctrl_handler(void *data)
{
	struct appdata *ad = data;
	_DBG("_LCD_freq_ctrl_handler");

	_LCD_freq_ctrl(ad);

	ad->lcd_freq_timer = NULL;
	return FALSE;
}

/*mini app term start*/
typedef struct {
	char *appid;
	int pid;
} pid_s;

void list_destroy_running_pid(Evas_Object *win)
{
	ret_if(NULL == win);

	Eina_List *list = evas_object_data_del(win, DATA_KEY_RUNNING_LIST);
	if (NULL == list) return;

	pid_s *p = NULL;
	EINA_LIST_FREE(list, p) {
		if (NULL == p) continue;
		if (p->appid) free(p->appid);
		free(p);
	}

	eina_list_free(list);
}

static int _running_app_info_cb(const aul_app_info *ainfo, void *data)
{
	retv_if(NULL == ainfo, 0);
	retv_if(NULL == ainfo->pkg_name, 0);
	retv_if(NULL == data, 0);

	Evas_Object *win = data;
	pid_s *p = calloc(1, sizeof(pid_s));
	retv_if(NULL == p, 0);

	char *appid = strdup(ainfo->pkg_name);
	goto_if(NULL == appid, ERROR);

	p->appid = appid;
	p->pid = ainfo->pid;

	Eina_List *rlist = evas_object_data_get(win, DATA_KEY_RUNNING_LIST);
	rlist = eina_list_append(rlist, p);
	evas_object_data_set(win, DATA_KEY_RUNNING_LIST, rlist);

	return 1;

ERROR:
	if (p) free(p);
	return 0;
}

int list_create_running_pid(Evas_Object *win)
{
	_DBG("list_create_running_pid");
	list_destroy_running_pid(win);
#if 0 /* Proc */
	if (AUL_R_OK != aul_app_get_running_app_info(_running_app_info_cb, win)) {
#else /* DB */
	if (AUL_R_OK != aul_get_running_app_info_from_memory(_running_app_info_cb, win)) {
#endif
		_DBG("Cannot get the running apps' info");
		return -1;
	}
	return 0;
}

int list_get_running_pid(Evas_Object *win, const char *appid)
{
	Eina_List *list = NULL;
	const Eina_List *l = NULL;
	const Eina_List *ln = NULL;
	pid_s *p = NULL;

	retv_if(NULL == win, -1);
	retv_if(NULL == appid, -1);

	list = evas_object_data_get(win, DATA_KEY_RUNNING_LIST);
	if (!list) {
		retv_if(0 != list_create_running_pid(win), -1);
		list = evas_object_data_get(win, DATA_KEY_RUNNING_LIST);
		/* It's possible to be no running apps */
		if (NULL == list) return -1;
	}

	EINA_LIST_FOREACH_SAFE(list, l, ln, p) {
		if (NULL == p) continue;
		if (NULL == p->appid) continue;
		if (!strcmp(p->appid, appid)) return p->pid;
	}

	return -1;
}

#define APPID_PHONE "org.tizen.phone"
#define APPID_CONTACTS "org.tizen.contacts"	/*it is different appid in 'Phone' mini app P140623-06536 */
int _mini_apps_cb(pkgmgrinfo_appinfo_h handle, void *user_data)
{
	retv_if(NULL == handle, 0);

	char *appid = NULL;
	pkgmgrinfo_appinfo_get_appid(handle, &appid);
	retv_if(NULL == appid, 0);

	_DBG("appid[%s]", appid);
	if (appid && !strcmp(appid, APPID_PHONE)) {
		mini_list = eina_list_append(mini_list, strdup(APPID_CONTACTS));
	} else {
		mini_list = eina_list_append(mini_list, strdup(appid));
	}
	return 0;
}

#define MINIAPP_CATEGORY "http://samsung.com/category/floatingapp"
Eina_List *_miniapp_create(void)
{
	/* Get info from AIL */
	pkgmgrinfo_appinfo_filter_h filter = NULL;
	int ret = 0;
	int count = -1;

	ret = pkgmgrinfo_appinfo_filter_create(&filter);
	if (ret > 0) {
		_DBG("pkgmgrinfo_appinfo_filter_create() failed");
	}
	ret = pkgmgrinfo_appinfo_filter_add_string(filter, PMINFO_APPINFO_PROP_APP_CATEGORY, MINIAPP_CATEGORY);
	ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(filter, _mini_apps_cb, NULL);
	ret = pkgmgrinfo_appinfo_filter_count(filter, &count);

	if (count <= 0) {
		pkgmgrinfo_appinfo_filter_destroy(filter);
		filter = NULL;
	}

	free(filter);
	return mini_list;
}

void _mini_terminate(int pid)
{
	_DBG("Terminate: (%d)", pid);
	if (0 < pid) {
		if (aul_terminate_pid(pid) < 0) {
			_ERR("aul_terminate_pid fail");
		}
	}
}

void _mini_destroy(void)
{
	mini_list = eina_list_free(mini_list);
}

void _terminate_miniapp(void *data)
{
	struct appdata *ad = data;
	mini_list = _miniapp_create();	/*list create */
	ret_if(NULL == mini_list);

	Eina_List *l = NULL;
	char *appid = NULL;
	EINA_LIST_FOREACH(mini_list, l, appid) {
		_DBG("terminate miniapp [%s]", appid);
		if (!appid) continue;
		int pid = list_get_running_pid(ad->win, appid);
		if (pid) _mini_terminate(pid);
	}
}
/*mini app term end*/


/*help api */
static void __help_popup_language_change_cb(void *data, Evas_Object *obj, void *source)
{
	/*char *string_id = (char *)data;
	char *message = _(string_id);
	helpui_set_popup_text(obj, message);*/
}

static void syspop_help_popup_circle_block_create(Evas_Object *win, Evas_Object *parent, Evas_Object **circle,
                                                  Evas_Object **popup, char *string_id, int x, int y, Evas_Coord_Rectangle *rect)
{
}

int launching_effect_set(Evas_Object *win, int flag)
{
	Ecore_X_Window xwin, root;
	Ecore_X_Atom effect_enable_atom;
	int is_enable = flag;
	root = ecore_x_window_root_first_get();
	effect_enable_atom = ecore_x_atom_get("_NET_CM_EFFECT_ENABLE");
	if (!effect_enable_atom) {
		_DBG("Cannot create _NET_CM_EFFECT_ENABLE atom... \n\n");
		return -1;
	}

	/* is_enable  (0: effect disable, 1: effect enable) */
	ecore_x_window_prop_card32_set(root, effect_enable_atom, &is_enable, 1);

	return 0;
}

int launching_effect_get(Evas_Object *win, int *flag)
{
	Ecore_X_Window xwin, root;
	Ecore_X_Atom effect_enable_atom;
	int is_enable = 0;
	root = ecore_x_window_root_first_get();
	effect_enable_atom = ecore_x_atom_get("_NET_CM_EFFECT_ENABLE");
	if (!effect_enable_atom) {
		_DBG("Cannot create _NET_CM_EFFECT_ENABLE atom... \n\n");
		return -1;
	}

	/* is_enable  (0: effect disable, 1: effect enable) */
	(void)ecore_x_window_prop_card32_get(root, effect_enable_atom, &is_enable, 1);
	*flag = is_enable;

	return 0;
}

static void mode_remove_noti(void *data, char *package)
{
	struct appdata *ad = data;
	notification_error_e ret = NOTIFICATION_ERROR_NONE;
	ret = notification_delete_all_by_type(package, NOTIFICATION_TYPE_ONGOING);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_delete_all_by_type [%d]", ret);
		return;
	}
}

static void mode_create_noti(void *data, const char *key_str, const char *package, const char *title)
{
	struct appdata *ad = data;
	notification_h noti = NULL;
	notification_error_e ret = NOTIFICATION_ERROR_NONE;

	/* remove noti */
	mode_remove_noti(ad, package);

	noti = notification_new(NOTIFICATION_TYPE_ONGOING,
	                        NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
	if (!noti) {
		_DBG("Fail to notification_new [%d]", ret);
		return;
	}

	ret = notification_set_layout(noti, NOTIFICATION_LY_ONGOING_EVENT);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_layout [%d]", ret);
		goto FAIL;
	}

	if (!strcmp("setting-firewall-efl", package)) {
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, IMG_Firewall);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON_FOR_INDICATOR, INDI_IMG_Firewall);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_TITLE,
	                            title,
	                            title, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_CONTENT,
	                            key_str,
	                            key_str, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text_domain(noti, SETTING_PACKAGE, SETTING_LOCALEDIR);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_pkgname(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_pkgname [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_application(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_application [%d]", ret);
		goto FAIL;
	}

	ret = notification_insert(noti, &ad->noti_id);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_insert [%d]", ret);
		goto FAIL;
	}

	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
		goto FAIL;
	}

	return;

FAIL:
	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
	}
	return;
}

static void mode_create_noti_with_no_msg(void *data, const char *key_str, const char *package, const char *title)
{
	struct appdata *ad = data;
	notification_h noti = NULL;
	notification_error_e ret = NOTIFICATION_ERROR_NONE;

	/* remove noti */
	mode_remove_noti(ad, package);

	noti = notification_new(NOTIFICATION_TYPE_ONGOING,
	                        NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
	if (!noti) {
		_DBG("Fail to notification_new [%d]", ret);
		return;
	}

	ret = notification_set_layout(noti, NOTIFICATION_LY_NOTI_THUMBNAIL);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_layout [%d]", ret);
		goto FAIL;
	}

	if (!strcmp(PKG_BLOCKING_MODE, package)) {
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, IMG_BlockingMode);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON_FOR_INDICATOR, INDI_IMG_BlockingMode);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_TITLE,
	                            title,
	                            title, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_CONTENT,
	                            key_str,
	                            key_str, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text_domain(noti, SETTING_PACKAGE, SETTING_LOCALEDIR);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_pkgname(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_pkgname [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_application(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_application [%d]", ret);
		goto FAIL;
	}

	ret = notification_insert(noti, &ad->noti_id);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_insert [%d]", ret);
		goto FAIL;
	}

	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
		goto FAIL;
	}

	return;

FAIL:
	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
	}
	return;
}

void _mode_backup_settings(void *data)
{

	struct appdata *ad = data;

	int val, ret;
	char *str1;
	char *str2;

	/*PSMODE #0 */
	vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &val);
	_DBG("VCONFKEY_SETAPPL_PSMODE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_PSMODE, val);
	_DBG("PREFKEY_SAVINGPOWER_BACKUP_PSMODE [%d]", ret);

	/*CPU */
	vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, &val);
	_DBG("VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_CPU_PERF, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_CPU_PERF [%d]", ret);

	/*Wallpaper change */
	str1 = vconf_get_str(VCONF_HOMESCREEN_PATH);
	ret = preference_set_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HOMESCREEN_PATH, str1);
	free(str1);
	str2 = vconf_get_str(VCONF_LOCKSCREEN_PATH);
	ret = preference_set_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LOCKSCREEN_PATH, str2);
	free(str2);

	/*time out */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, &val);
	_DBG("VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TIMEOUT, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TIMEOUT [%d]", ret);
	/*Auto Brightness : ON/OFF */
	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &val);
	_DBG("VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_BRIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_BRIGHT [%d]", ret);
	/*Brightness */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &val);
	_DBG("VCONFKEY_SETAPPL_LCD_BRIGHTNESS [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BRIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BRIGHT [%d]", ret);
	/*Auto rotate screen */
	vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &val);
	_DBG("VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_ROTATE, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_ROTATE [%d]", ret);

	/*Haptic feedback */
	vconf_get_bool(VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL, &val);
	_DBG("VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HAPTIC_FEEDBACK, val);	/*on/off */
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HAPTIC_FEEDBACK [%d]", ret);
	/*Launching effect */
	launching_effect_get(ad->win, &val);
	_DBG("launching_effect_get [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LAUNCH_EFFECT, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LAUNCH_EFFECT [%d]", ret);

	/*pkg backup : cluster or easy */
	char *pkg_home = NULL;
	pkg_home = vconf_get_str(VCONF_PKG_PATH);
	if (pkg_home) {
		_DBG("pkghome [%s]", pkg_home);
		ret = preference_set_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_PKG_HOME, pkg_home);
		_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_PKG_HOME [%d]", ret);
	}

	/*vconf_get_int(VCONF_FAKE_KEY, &val); */
	/*ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FAKE_EFFECT, val); */
	/*setting_retm_if(ret != PREFERENCE_ERROR_NONE, "*** [ERR] PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FAKE_EFFECT ***"); */

	/*GREYSCALE TONE */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_GRAYTONE, &val);
	_DBG("VCONFKEY_SETAPPL_WINDOW_GRAYTONE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GRAYTONE, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GRAYTONE [%d]", ret);

	/*SCREEN OUTPUT */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_FPS, &val);
	_DBG("VCONFKEY_SETAPPL_WINDOW_FPS [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FPS, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FPS [%d]", ret);

	/*GPS */
	vconf_get_int(VCONFKEY_LOCATION_ENABLED, &val);
	_DBG("VCONFKEY_LOCATION_ENABLED [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GPS, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GPS [%d]", ret);

	/*touch key ligh duration */
	vconf_get_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, &val);
	_DBG("VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TOUCH_KEY_LIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TOUCH_KEY_LIGHT [%d]", ret);

	/*end for power saving profile */

	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LED_INDICATOR_NOTI, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LED_INDICATOR_NOTI [%d]", ret);

	/*Net-restriction mode - syspopup #20 */
	vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &val);
	_DBG("VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_NET_RESRICT_MODE, val);
	_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_NET_RESRICT_MODE [%d]", ret);

	/*default power saving need 1 time */
	ret = preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, &val);
	_DBG("PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR : ret [%d], val [%d]", ret, val);
	if (ret != PREFERENCE_ERROR_NONE) {
		preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, 1);
	}
}

void _mode_power_backup_settings(void *data)
{

	struct appdata *ad = data;

	int val, ret;
	char *str1;
	char *str2;

	/*PSMODE #0 */
	vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &val);
	_DBG("VCONFKEY_SETAPPL_PSMODE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_PSMODE, val);
	_DBG("PREFKEY_SAVINGPOWER_BACKUP_PSMODE [%d]", ret);

	/*CPU */
	vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, &val);
	_DBG("VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_CPU_PERF, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_CPU_PERF [%d]", ret);

	/*Wallpaper change */
	str1 = vconf_get_str(VCONF_HOMESCREEN_PATH);
	ret = preference_set_string(PREFKEY_SAVINGPOWER_POWER_BACKUP_HOMESCREEN_PATH, str1);
	/*setting_retm_if(ret != PREFERENCE_ERROR_NONE, "*** [ERR] PREFKEY_SAVINGPOWER_POWER_BACKUP_HOMESCREEN_PATH ***"); */
	free(str1);
	str2 = vconf_get_str(VCONF_LOCKSCREEN_PATH);
	ret = preference_set_string(PREFKEY_SAVINGPOWER_POWER_BACKUP_LOCKSCREEN_PATH, str2);
	/*setting_retm_if(ret != PREFERENCE_ERROR_NONE, "*** [ERR] PREFKEY_SAVINGPOWER_POWER_BACKUP_LOCKSCREEN_PATH ***"); */
	free(str2);

	/*time out */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, &val);
	_DBG("VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_TIMEOUT, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_TIMEOUT [%d]", ret);
	/*Auto Brightness : ON/OFF */
	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &val);
	_DBG("VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_BRIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_BRIGHT [%d]", ret);
	/*Brightness */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &val);
	_DBG("VCONFKEY_SETAPPL_LCD_BRIGHTNESS [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_BRIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_BRIGHT [%d]", ret);
	/*Auto rotate screen */
	vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &val);
	_DBG("VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_ROTATE, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_ROTATE [%d]", ret);

	/*Haptic feedback */
	vconf_get_bool(VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL, &val);
	_DBG("VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_HAPTIC_FEEDBACK, val);	/*on/off */
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_HAPTIC_FEEDBACK [%d]", ret);

	/*Launching effect */
	launching_effect_get(ad->win, &val);
	_DBG("launching_effect_get [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_LAUNCH_EFFECT, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_LAUNCH_EFFECT [%d]", ret);

	/*pkg backup : cluster or easy */
	char *pkg_home = NULL;
	pkg_home = vconf_get_str(VCONF_PKG_PATH);
	if (pkg_home) {
		_DBG("pkghome [%s]", pkg_home);
		ret = preference_set_string(PREFKEY_SAVINGPOWER_POWER_BACKUP_PKG_HOME, pkg_home);
		_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_PKG_HOME [%d]", ret);
	}

	/*GREYSCALE TONE */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_GRAYTONE, &val);
	_DBG("VCONFKEY_SETAPPL_WINDOW_GRAYTONE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_GRAYTONE, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_GRAYTONE [%d]", ret);

	/*SCREEN OUTPUT */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_FPS, &val);
	_DBG("VCONFKEY_SETAPPL_WINDOW_FPS [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_FPS, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_FPS [%d]", ret);

	/*GPS */
	vconf_get_int(VCONFKEY_LOCATION_ENABLED, &val);
	_DBG("VCONFKEY_LOCATION_ENABLED [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_GPS, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_GPS [%d]", ret);

	/*touch key ligh duration */
	vconf_get_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, &val);
	_DBG("VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT [%d]", ret);

	/*end for power saving profile */

	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_LED_INDICATOR_NOTI, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_LED_INDICATOR_NOTI [%d]", ret);

	/*Net-restriction mode - syspopup #20 */
	vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &val);
	_DBG("VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE [%d]", val);
	ret = preference_set_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_NET_RESRICT_MODE, val);
	_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_NET_RESRICT_MODE [%d]", ret);
}


void _mode_apply_settings(void *data, int *set)
{

	struct appdata *ad = data;
	int val, crt_val, ret;


	/*/////////////////////////////// */
	/* power saving mode content order */
	/*/////////////////////////////// */

	/*Haptic feedback #1 */
	vconf_get_bool(VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL, &crt_val);
	val = set[0];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HAPTIC_FEEDBACK, &val);
	} else if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_HAPTIC_FEEDBACK, &val);
	}
	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_HAPTIC_FEEDBACK_STATUS_BOOL, val);
		_DBG("set haptic  feedback : %d", val);
	}

	/*Launching effect #2 */
	launching_effect_get(ad->win, &crt_val);
	val = set[1];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LAUNCH_EFFECT, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_LAUNCH_EFFECT, &val);
	}
	if (val != crt_val) {
		launching_effect_set(ad->win, val);
		_DBG("set launching effect %d", val);
	}

	/*GPS #3 */
	vconf_get_int(VCONFKEY_LOCATION_ENABLED, &crt_val);
	_DBG("get GPS %d", crt_val);
	val = set[2];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GPS, &val);
		_DBG("get GPS %d", val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_GPS, &val);
		_DBG("get GPS %d", val);
	}
	if (val != crt_val) {
		vconf_set_int(VCONFKEY_LOCATION_ENABLED, val);
		_DBG("set GPS %d", val);
	}

	/*Brightness #4 */
	/*add check auto brightness routine */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &crt_val);
	val = set[3];
	if (val == -1) {
		int auto_br = 0;
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_BRIGHT, &auto_br);
		/* * 0 : Off */
		/* * 1 : On */
		/* * 2 : Pause */
		_DBG("check1 auto_br [%d]", auto_br);
		if (auto_br == SETTING_BRIGHTNESS_AUTOMATIC_ON) {
			/*prev : auto on, fix to auto status last */
			ret = vconf_set_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, SETTING_BRIGHTNESS_AUTOMATIC_ON);

		} else if (auto_br == SETTING_BRIGHTNESS_AUTOMATIC_OFF) {
			/*auto to off and set bright */
			ret = vconf_set_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, SETTING_BRIGHTNESS_AUTOMATIC_OFF);
			ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BRIGHT, &val);
		}
	} else if (val == -2) {
		int auto_br = 0;
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_BRIGHT, &auto_br);
		/* * 0 : Off */
		/* * 1 : On */
		/* * 2 : Pause */
		_DBG("check1 auto_br [%d]", auto_br);
		if (auto_br == SETTING_BRIGHTNESS_AUTOMATIC_ON) {
			/*prev : auto on, fix to auto status last */
			ret = vconf_set_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, SETTING_BRIGHTNESS_AUTOMATIC_ON);

		} else if (auto_br == SETTING_BRIGHTNESS_AUTOMATIC_OFF) {
			/*auto to off and set bright */
			ret = vconf_set_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, SETTING_BRIGHTNESS_AUTOMATIC_OFF);
			ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_BRIGHT, &val);
		}
	} else {
		/*normal to power saving or emergency(val != -1), get auto brightness off */
		ret = vconf_set_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, SETTING_BRIGHTNESS_AUTOMATIC_OFF);
	}

	if (val != -1 && val != -2) {
		ret = display_set_brightness_with_setting(val);
		_DBG("set Brightness %d, ret = %d", val, ret);
		ret = vconf_set_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, val);
	}

	/*Auto rotate screen #5 */
	vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &crt_val);
	val = set[4];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_ROTATE, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_ROTATE, &val);
	}
	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, val);
		_DBG("set Auto rotate screen  %d", val);
	}

	/*time out #6 */
	vconf_get_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, &crt_val);
	val = set[5];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TIMEOUT, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_TIMEOUT, &val);
	}
	if (val != crt_val) {
		vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, val);
		_DBG("set time out %d", val);
	}

	/*Touch key light duration #8 */
	vconf_get_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, &crt_val);
	_DBG("VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION %d", crt_val);
	val = set[7];
	if (val == -3) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TOUCH_KEY_LIGHT, &val);
		_DBG("PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TOUCH_KEY_LIGHT %d", val);
	}
	if (val == -4) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT, &val);
		_DBG("PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT %d", val);
	}
	if (val != crt_val) {
		vconf_set_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, val);
		_DBG("set Touch key light duration %d", val);
	}

	/*gray scale tone #9 */
	/*gray scale tone for power saving mode! */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_GRAYTONE, &crt_val);
	val = set[8];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GRAYTONE, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_GRAYTONE, &val);
	}
	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_WINDOW_GRAYTONE, val);
		_DBG("set gray scale tone %d", val);
	}

	/*CPU max freq #10 */
	/*cpu limit on for power saving mode! */
	vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, &crt_val);
	val = set[9];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_CPU_PERF, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_CPU_PERF, &val);
	}

	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, val);
		_DBG("set CPU frequency limit %d", val);
	}

	/*Screen Output (D.FPS) #11 */
	/*screen output for power saving mode! */
	vconf_get_bool(VCONFKEY_SETAPPL_WINDOW_FPS, &crt_val);
	val = set[10];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FPS, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_FPS, &val);
	}
	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_WINDOW_FPS, val);
		_DBG("set FPS %d", val);
	}

	/*Eco. wallpaper (Wallpaper change) #12 */
	char *str1;
	char *str2;
	char *crt_str1;
	char *crt_str2;
	val = set[11];
	crt_str1 = vconf_get_str(VCONF_HOMESCREEN_PATH);
	crt_str2 = vconf_get_str(VCONF_LOCKSCREEN_PATH);
	if (val == -1) {
		ret = preference_get_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HOMESCREEN_PATH, &str1);
		ret = preference_get_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LOCKSCREEN_PATH, &str2);
	} else if (val == -2) {
		ret = preference_get_string(PREFKEY_SAVINGPOWER_POWER_BACKUP_HOMESCREEN_PATH, &str1);
		ret = preference_get_string(PREFKEY_SAVINGPOWER_POWER_BACKUP_LOCKSCREEN_PATH, &str2);
	} else { /* value ==1 and other cases -> set to default value */
		str1 = strdup(HOMESCREEN_FILE);
		str2 = strdup(LOCKSCREEN_FILE);
	}

	if (str1) {
		if (strcmp(str1, crt_str1) != 0) {
			vconf_set_str(VCONF_HOMESCREEN_PATH, str1);
			_DBG("set eco home screen wallpaper");
		}
	}
	if (str2) {
		if (strcmp(str2, crt_str2) != 0) {
			vconf_set_str(VCONF_LOCKSCREEN_PATH, str2);
			_DBG("set eco lock screen wallpaper");
		}
	}
	free(str1);
	free(str2);
	free(crt_str1);
	free(crt_str2);

	/*END of the Power saving profile set */

	/*pkghome -for HOMESCREEN #13 */
	char *str = NULL;
	char *crt_str = NULL;
	val = set[12];
	crt_str = vconf_get_str(VCONF_PKG_PATH);
	if (val == -1) {
		int chk_easy = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_HOMESCREEN_EASYMODE_BOOL, &chk_easy);
		if (chk_easy) {
			/*easy-mode :on -> off mode did not go back to current, just keep easy home state */
			str = strdup(PKG_EASYHOME);	/*get curren pkg home path -> easy-mode */
		} else {
			/*off on easy-mode */
			/*ret = preference_get_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_PKG_HOME, &str); */
			str = strdup(PKG_CLUSTERHOME);
		}
	} else if (val == -2) {
		int chk_easy = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_HOMESCREEN_EASYMODE_BOOL, &chk_easy);
		if (chk_easy) {
			/*easy-mode :on -> off mode did not go back to current, just keep easy home state */
			str = strdup(PKG_EASYHOME);	/*get curren pkg home path -> easy-mode */
		} else {
			/*off on easy-mode */
			/*ret = preference_get_string(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_PKG_HOME, &str); */
			str = strdup(PKG_CLUSTERHOME);
		}
	} else if (val == 1) {
		str = strdup(PKG_EMERGENCY);
	}

	if (crt_str) {
		if (str != NULL && strcmp(str, crt_str) != 0) {
			vconf_set_str(VCONF_PKG_PATH, str);
		}
	}
	free(str);
	free(crt_str);

	/*ENDOF POWER SAVING PROFILES */

	/*OTHERS */
	val = set[17];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HANDS_FREE_MODE, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_HANDS_FREE_MODE, &val);
	}

	/*Net-restriction mode - syspopup #20 */
	vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &crt_val);
	val = set[19];
	if (val == -1) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_NORMAL_BACKUP_NET_RESRICT_MODE, &val);
	}
	if (val == -2) {
		ret = preference_get_int(PREFKEY_SAVINGPOWER_POWER_BACKUP_NET_RESRICT_MODE, &val);
	}

	if (val != crt_val) {
		vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, val);
		/*todo mode-syspopup */
		_DBG("set Net-restriction mode %d", val);
	}

	/*mini app terminate - syspopup #21 */
	val = set[20];
	if (val == 1) {
		_DBG("emergency terminate mini");
		_terminate_miniapp(ad);
	}
}

/*///////////////// */
/*/ blocking mode /// */
/*///////////////// */

/* rotation policy with lock screen */
static void lcd_state_changed(keynode_t *key, void *data)
{
	int state;
	struct appdata *ad = data;

	if (!key || !ad)
		return;

	state = vconf_keynode_get_int(key);
	if (state != VCONFKEY_PM_STATE_LCDOFF)
		return;

	if (vconf_ignore_key_changed(VCONFKEY_PM_STATE, lcd_state_changed) != 0)
		_DBG("vconf key ignore failed");

	elm_exit();
}

/*popup handling */
static void __def_response_cb(void *data, Evas_Object *obj,
                              void *event_info)
{
	_DBG(" REMOVE THE POPUP OBJECT BY THIS FUNCTION ");
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
}

void __popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	_DBG("__popup_del_cb");
#if 0
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get(obj);
	int ret = utilx_ungrab_key(disp, xwin, KEY_HOME);
	if (ret) {
		_DBG("KEY_HOME ungrab error ret[%d]", ret);
	}

	ret = utilx_ungrab_key(disp, xwin, KEY_BACK);
	if (ret) {
		_DBG("KEY_BACK ungrab error ret[%d]", ret);
	}

	ret = utilx_ungrab_key(disp, xwin, KEY_POWER);
	if (ret) {
		_DBG("KEY_POWER ungrab error ret[%d]", ret);
	}

	ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEUP);
	if (ret) {
		_DBG("KEY_VOLUMEUP ungrab error ret[%d]", ret);
	}

	ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEDOWN);
	if (ret) {
		_DBG("KEY_VOLUMEDOWN ungrab error ret[%d]", ret);
	}
#endif
}

static void __popup_event_set(Evas_Object *popup, void *data,
                              _cb_func response_cb,
                              int timeout,/*to control the timeout time */
                              bool blocked_flag,/*to control whether to block the screen */
                              bool keygrab_flag/*to control whether to block the 'Home key' */
                             )
{
	_DBG("__popup_event_set");
	if (timeout > 0) {
		if (response_cb) {
			evas_object_smart_callback_add(popup, "timeout", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "timeout", __def_response_cb, data);
		}
	}

	if (!blocked_flag) {/*not blocked_flag == TRUE !!! */
		if (response_cb) {
			evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "block,clicked", __def_response_cb, data);
		}
	}

	if (keygrab_flag) {
#if 0
		Ecore_X_Display *disp = ecore_x_display_get();
		Ecore_X_Window xwin = elm_win_xwindow_get(popup);
		int ret = utilx_grab_key(disp, xwin, KEY_HOME, TOP_POSITION_GRAB);
		if (ret) {
			_DBG("KEY_HOME grab error ret[%d]", ret);
		}

		ret = utilx_grab_key(disp, xwin, KEY_BACK, TOP_POSITION_GRAB);
		if (ret) {
			_DBG("KEY_BACK grab error ret[%d]", ret);
		}

		ret = utilx_grab_key(disp, xwin, KEY_POWER, TOP_POSITION_GRAB);
		if (ret) {
			_DBG("KEY_POWER grab error ret[%d]", ret);
		}

		ret = utilx_grab_key(disp, xwin, KEY_VOLUMEUP, TOP_POSITION_GRAB);
		if (ret) {
			_DBG("KEY_VOLUMEUP grab error ret[%d]", ret);
		}

		ret = utilx_grab_key(disp, xwin, KEY_VOLUMEDOWN, TOP_POSITION_GRAB);
		if (ret) {
			_DBG("KEY_VOLUMEDOWN grab error ret[%d]", ret);
		}
#endif
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb, NULL);
	}
}

static Evas_Object *__add_progressbar(void *data, Evas_Object *parent,
                                      char *progressbar_style,
                                      char *progressbar_title,
                                      char *progressbar_lable)
{
	_DBG("__add_progressbar");
#if 0
	Evas_Object *popup;
	Evas_Object *progressbar;
	struct appdata *ad;
	Evas_Object *layout;

	ad = (struct appdata *) data;
	popup = ea_center_popup_add(parent);

	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, POPUP_EDJ, "popup_processingview_1button");	/*u can use add button or not */
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* [UI] progress icon */
	progressbar = elm_progressbar_add(popup);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_style_set(progressbar, progressbar_style);
	elm_progressbar_horizontal_set(progressbar, EINA_TRUE);
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(progressbar);

	elm_object_part_content_set(layout, "elm.swallow.content", progressbar);
	/* [UI] text */
	elm_object_part_text_set(layout, "elm.text", progressbar_lable);

	elm_object_content_set(popup, layout);
	return popup;
#endif
	Evas_Object *popup;
	Evas_Object *progressbar;
	Evas_Object *layout;
	Evas_Object *label;
	int change_ang = 0;

	popup = elm_popup_add(parent);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object *box = elm_box_add(popup);
	elm_box_horizontal_set(box, EINA_TRUE);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_align_set(box, 0.5, 0.5);
	elm_box_padding_set(box, 16, 0);

	progressbar = elm_progressbar_add(popup);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_style_set(progressbar, "process_large");
	evas_object_show(progressbar);
	elm_box_pack_end(box, progressbar);

	label = elm_label_add(popup);
	elm_object_text_set(label, progressbar_lable);
	evas_object_show(label);
	elm_box_pack_end(box, label);

	Evas_Object *table = elm_table_add(popup);
	evas_object_show(table);
	elm_table_homogeneous_set(table, EINA_FALSE);

	Evas_Object *rect_up;
	rect_up = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_up, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(32));

	Evas_Object *rect_down;
	rect_down = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_down, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(37));

	/* box */
	elm_table_pack(table, rect_up, 0, 0, 1, 1);/* rect as a padding */
	elm_table_pack(table, box, 0, 1, 1, 1);
	elm_table_pack(table, rect_down, 0, 2, 1, 1);/* rect as a padding */
	evas_object_show(box);
	elm_object_content_set(popup, table);

	evas_object_show(popup);
	return popup;
}

static Evas_Object *__add_double_line_progressbar(void *data, Evas_Object *parent,
                                                  char *progressbar_style,
                                                  char *progressbar_title,
                                                  char *progressbar_lable)
{
	_DBG("__add_double_line_progressbar");
	Evas_Object *popup;
	Evas_Object *label;
	Evas_Object *layout;
	Evas_Object *progressbar = NULL;
	popup = ea_center_popup_add(parent);
	/* [UI] add lable */
	if (progressbar_lable) {
		label = elm_label_add(popup);
		elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
		elm_object_text_set(label, _(progressbar_lable));
		evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(label);
	}
	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, POPUP_EDJ, "popup_processingview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* [UI] progress icon */
	if (progressbar_style) {
		progressbar = elm_progressbar_add(popup);
		elm_progressbar_pulse(progressbar, EINA_TRUE);
		elm_object_style_set(progressbar, progressbar_style);
		elm_progressbar_horizontal_set(progressbar, EINA_TRUE);
		evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_show(progressbar);
	}

	elm_object_part_content_set(layout, "elm.swallow.content", progressbar);
	elm_object_part_content_set(layout, "elm.swallow.text", label);

	elm_object_content_set(popup, layout);
	return popup;
}


Evas_Object *__popup_with_progressbar(void *data, Evas_Object *popup,
                                      Evas_Object *parent,
                                      char *progressbar_style,
                                      char *title,
                                      char *text,
                                      _cb_func response_cb,
                                      int timeout,
                                      bool blocked_flag,
                                      bool keygrab_flag,
                                      bool dobule_line_flag)
{
	if (dobule_line_flag) {
		popup = __add_double_line_progressbar(data, parent, progressbar_style,
		                                      title, text);
	} else {
		popup = __add_progressbar(data, parent, progressbar_style,
		                          title, text);
		elm_object_style_set(popup, "no_effect");
	}

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	__popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);
	evas_object_show(popup);
	return popup;
}

Evas_Object *_create_bg(Evas_Object *parent, Evas_Object *win, const char *bg_style)
{
	char *path = NULL;
	if (parent == NULL) {
		return "NULL";
	}
	Evas_Object *bg = elm_bg_add(parent);

	if (bg == NULL) {
		return "NULL";
	}
	elm_object_style_set(bg, bg_style);
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	elm_win_resize_object_add(parent, bg);	/*regarding window resize */
	/*elm_bg_file_set(bg, DEFAULT_BG_PATH, NULL); */
	evas_object_show(bg);
	return bg;
}

char *mode_syspopup_gettext(const char *s)
{
	/* fisrt find in app pg */
	if (s == NULL) {
		return "NULL";
	}

	char *p = dgettext(PACKAGE, s);

	if (p && !strcmp(s, p)) {        /* not found */
		/* find in system pkg */
		p = dgettext("sys_string", s);
	}

	if (p && !strcmp(s, p)) {        /* not found */
		/* find in system pkg */
		p = dgettext("setting", s);
	}
	return p;
}

int myterm(bundle *b, void *data)
{
	return 0;
}

int mytimeout(bundle *b, void *data)
{
	return 0;
}

syspopup_handler handler = {
	.def_term_fn = myterm,
	.def_timeout_fn = mytimeout
};

/* Release evas object */
void release_evas_object(Evas_Object **obj)
{
	if (!obj || !(*obj))
		return;
	evas_object_del(*obj);
	*obj = NULL;
}


static void ea_popup_back_cb_overide(void *data, Evas_Object *obj, void *event_info)
{
	ea_popup_back_cb(data, obj, event_info);

	if (NULL == data) return;
	struct appdata *ad = data;
	__mode_syspopup_app_terminate(ad);
}

static void timeout_response_cb(void *data, Evas_Object *obj, void *event_info)
{
	ea_popup_back_cb(data, obj, event_info);

	if (NULL == data) return;
	struct appdata *ad = data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	__mode_syspopup_app_terminate(ad);
}

static void __progress_popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (NULL == data) return;
	struct appdata *ad = data;
	_DBG("progress popup cb");

	/* have operating delay in emergency to cluster, exit point was after progress */
	if (vconf_ignore_key_changed(VCONFKEY_PM_STATE, lcd_state_changed) != 0)
		_DBG("vconf key ignore failed");

	is_recovery = false;

	/*refresh indicator */
	__mode_reboot_noti_method();

	elm_exit();
}

Eina_Bool
play_timeout_update_cb(void *data)
{
	if (NULL == data) return EINA_FALSE;
	struct appdata *ad = data;
	play_text(ad, _("IDS_ST_BODY_A_PERSON_YOU_HAVE_GRANTED_AUTHORITY_TO_IS_TRYING_TO_REMOTELY_ENABLE_EMERGENCY_MODE_ON_THIS_DEVICE_MSG"));
	ad->play_timer = NULL;
	return EINA_FALSE;
}


static void __mode_syspopup_emergency_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	/*delete old help */
	if (ad->help_type != HELP_NONE) {
		if (ad->help_circle) {
			evas_object_del(ad->help_circle);
			ad->help_circle = NULL;
		}
		if (ad->help_popup) {
			evas_object_del(ad->help_popup);
			ad->help_popup = NULL;
		}
	}

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK")) || !strcmp(btn_str, _("IDS_ST_BUTTON_ENABLE"))) {
		/*make complete flag */
		int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check export xml");

		int m_key;
		vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		} else if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}
		release_evas_object(&(ad->popup));
		ad->bg = _create_bg(ad->win, NULL, "group_list");
		/*progress bar */
		ad->enable_popup = __popup_with_progressbar(ad, ad->enable_popup, ad->win, PROGRESSBAR_STYLE,
		                                            NULL, _("IDS_ST_POP_ENABLING_EMERGENCY_MODE_ING"), __progress_popup_cb, 3/*0*/, TRUE, TRUE, TRUE);	/* 2 seconds to wait in maximum */

		/*assistive light off, P140422-06240 */
		ret = led_set_brightness_with_noti(0, TRUE);
		if (0 != ret) {
			_DBG(">>>led SET TO ZERO err [%d]", ret);
		} else {
			vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to disable popup off reminder %d***", ret);
			}
		}

		/*set emergency*/
		_mode_apply_settings(ad, emergency_settings);
		/*set gray tone */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_EMERGENCY) == 0) {
			_DBG("set on SETTING_PSMODE_EMERGENCY");
		}
		free(btn_str);
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
		free(btn_str);
		elm_exit();
	} else {
		free(btn_str);
	}
	/*evas_object_del(ad->win); */
}

static void __mode_syspopup_powerful_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	/*delete old help */
	if (ad->help_type != HELP_NONE) {
		if (ad->help_circle) {
			evas_object_del(ad->help_circle);
			ad->help_circle = NULL;
		}
		if (ad->help_popup) {
			evas_object_del(ad->help_popup);
			ad->help_popup = NULL;
		}
	}

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		int m_key, first;
		vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		}
		_DBG("OK OK");
		/*set normal sub*/
		int ret = preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, &first);

		if (first) {
			_mode_apply_settings(ad, powerful_settings_def);
			preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, 0);
		} else {
			_mode_apply_settings(ad, powerful_settings);
			_DBG("right to set powerful");
		}
		/* set to default */
		int opposite;
		ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
		if (ret) _DBG("vconf_get_bool failed");
		/*emergency mode -> normal */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_POWERFUL) == 0) {
			_DBG("set on SETTING_PSMODE_POWERFUL");
		}
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
	}
	/*evas_object_del(ad->win); */
	free(btn_str);
	elm_exit();
}

static void __mode_syspopup_netrestrictionmode_change_on_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");

	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		_DBG("OK button");

		if (vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, 1) == 0) {
			_DBG("network_restrict_mode vconf set 1");
		} else {
			_DBG("network_restrict_mode vconf set failed");
		}
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("Cancel button");
	}

	free(btn_str);

	elm_exit();
}

static void __mode_syspopup_netrestrictionmode_change_off_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");

	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		_DBG("OK button");

		if (vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, 0) == 0) {
			_DBG("network_restrict_mode vconf set 0");
		} else {
			_DBG("network_restrict_mode vconf set failed");
		}
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("Cancel button");
	}

	free(btn_str);

	elm_exit();
}

static void __mode_syspopup_normal_cb(void *data, Evas_Object *obj, void *event_info)
{
	_DBG("__mode_syspopup_normal_cb");
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK")) || !strcmp(btn_str, _("IDS_ST_BUTTON_DISABLE"))) {
		/*using normal cb as 1) emergency -> normal or 2) emergency -> powerful , after[K] concept back_mode needed */
		int back_mode;
		preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_PSMODE, &back_mode);

		int m_key;
		vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
		if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}
		/*evas_object_hide(ad->popup); */
		release_evas_object(&(ad->popup));
		ad->bg = _create_bg(ad->win, NULL, "group_list");
		/*progress bar */
		ad->disable_popup = __popup_with_progressbar(ad, ad->disable_popup, ad->win, PROGRESSBAR_STYLE,
		                                             NULL, _(KeyStr_Loading), __progress_popup_cb, 3/*0*/, TRUE, TRUE, FALSE);	/* 2 seconds to wait in maximum */


		/*set normal sub*/
		if (back_mode == SETTING_PSMODE_NORMAL) {
			/*make complete flag */
			int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
			if (ret) _ERR("check export xml");
			/*torch light off with diable emergency/ups mode, P140507-03841 */
			int t_ret = led_set_brightness_with_noti(0, TRUE);
			if (0 != t_ret) {
				_DBG(">>>led SET TO ZERO err [%d]", t_ret);
			} else {
				vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
				if (t_ret != 0) {
					_DBG("*** [ERR] Failed to disable popup off reminder %d***", t_ret);
				}
			}
			int b_network_lcd_off;
			vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, &b_network_lcd_off);
			if (b_network_lcd_off) {
				vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, EINA_FALSE);
			}
			_mode_apply_settings(ad, normal_settings);
		}

		/*set power sub*/
		if (back_mode == SETTING_PSMODE_POWERFUL) {
			int first;
			/*set normal sub*/
			int ret = preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, &first);
			if (first) {
				_mode_apply_settings(ad, powerful_settings_def);
				preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, 0);
			} else {
				_mode_apply_settings(ad, powerful_settings);
				_DBG("right to set powerful");
			}
		}

		/* set to default */
		int opposite;
		int ret = 0;
		ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
		if (ret) _DBG("vconf_get_bool failed");
		if (back_mode == SETTING_PSMODE_NORMAL) {
			/*emergency mode -> normal */
			if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_NORMAL) == 0) {
				_DBG("set on SETTING_PSMODE_NORMAL");
			}
		} else if (back_mode == SETTING_PSMODE_POWERFUL) {
			/*emergency mode -> power saving */
			if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_POWERFUL) == 0) {
				_DBG("set on SETTING_PSMODE_POWERFUL");
			}
		}
		free(btn_str);

		/* */
#if LCD_FREQ_CTRL
		if (ad->lcd_freq_timer) {
			ecore_timer_del(ad->lcd_freq_timer);
			ad->lcd_freq_timer = NULL;
		}
		ad->is_lcd_ctrl = EINA_FALSE;
		ad->lcd_freq_timer = ecore_timer_add(2.7, (Ecore_Task_Cb)_LCD_freq_ctrl_handler, ad);
#endif

		_DBG("OK end");
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
		free(btn_str);
		elm_exit();
	} else {
		free(btn_str);
	}
	/*evas_object_del(ad->win); */
}

static void __mode_blockingmode_popup_cb(app_control_h request, app_control_h reply, app_control_result_e result, void *data)
{
	struct appdata *ad = data;
	_DBG("__mode_blockingmode_popup_cb");
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

static void __mode_syspopup_blockingmode_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		_DBG("OK OK");

		app_control_h app_control = NULL;
		app_control_create(&app_control);
		if (app_control == NULL) {
			_DBG("the app_control is NULL");
			free(btn_str);
			return;
		}
		int ret;
		ret = app_control_set_app_id(app_control, "setting-blockingmode-efl");
		if (ret != APP_CONTROL_ERROR_NONE) {
			_DBG("app_control_set_app_id fail : %d", ret);
		}
		app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
		ret = app_control_send_launch_request(app_control, NULL, NULL);
		if (ret != APP_CONTROL_ERROR_NONE) {
			_DBG("app_control_send_launch_request fail : %d", ret);
		}
		app_control_destroy(app_control);
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
	}
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	/*evas_object_del(ad->win); */
	free(btn_str);
	elm_exit();
}

static void __mode_syspopup_drivingmode_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		_DBG("OK OK");

		app_control_h app_control = NULL;
		app_control_create(&app_control);
		if (app_control == NULL) {
			_DBG("the app_control is NULL");
			free(btn_str);
			return;
		}
		int ret;
		ret = app_control_set_app_id(app_control, "setting-drivingmode-efl");
		if (ret != APP_CONTROL_ERROR_NONE) {
			_DBG("app_control_set_app_id fail : %d", ret);
		}
		app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
		ret = app_control_send_launch_request(app_control, NULL, NULL);
		if (ret != APP_CONTROL_ERROR_NONE) {
			_DBG("app_control_send_launch_request fail : %d", ret);
		}
		app_control_destroy(app_control);
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
	}
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	/*evas_object_del(ad->win); */
	free(btn_str);
	elm_exit();
}


static void __mode_syspopup_mobiledata_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
		_DBG("OK OK");

		int ret;
		if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_MOBILEDATA_ON)) {
			/*enable mobile data */
			ret = vconf_set_bool(VCONFKEY_3G_ENABLE, 1);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to enable mobile data with error code %d***", ret);
			}
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				ret = vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, 0);
				if (ret != 0) {
					_DBG("*** [ERR] Failed to disable popup on reminder %d***", ret);
				}
			}

		} else {
			/*disable mobile data*/
			ret = vconf_set_bool(VCONFKEY_3G_ENABLE, 0);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to enable mobile data with error code %d***", ret);
			}
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				ret = vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, 0);
				if (ret != 0) {
					_DBG("*** [ERR] Failed to disable popup off reminder %d***", ret);
				}
			}
		}

	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");

	}
	free(btn_str);
	elm_exit();
}

static void __mode_syspopup_flightmode_tapi_event_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	_DBG("flight mode result:%d", result);
	elm_exit();
}

static void __mode_syspopup_flightmode_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK")) || !strcmp(btn_str, _("IDS_ST_BUTTON_ENABLE"))) {
		_DBG("OK OK");

		int ret;
		TapiHandle *tapi_handle = tel_init(NULL);
		if (!tapi_handle) {
			_DBG("*** [ERR] tel_init failed ***");
			free(btn_str);
			elm_exit();
			return;
		}

		int firewall_state;
		vconf_get_bool(VCONFKEY_SETAPPL_FIREWALL_KEY, &firewall_state);
		if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_FLIGHTMODE_ON)) {
			/*enable flight mode on*/
			ret = tel_set_flight_mode(tapi_handle, TAPI_POWER_FLIGHT_MODE_ENTER, __mode_syspopup_flightmode_tapi_event_cb, ad);
			if (ret != TAPI_API_SUCCESS) {
				_DBG("*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_ENTER) ***");
			}


			if (firewall_state) {
				mode_remove_noti(ad, "setting-firewall-efl");
			}
		} else {
			/*disable flight mode*/
			ret = tel_set_flight_mode(tapi_handle, TAPI_POWER_FLIGHT_MODE_LEAVE, __mode_syspopup_flightmode_tapi_event_cb, ad);
			if (ret != TAPI_API_SUCCESS) {
				_DBG("*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_LEAVE) ***");
			}
			if (firewall_state) {
				mode_create_noti(ad, "IDS_ST_BODY_CONFIGURE_FIREWALL_SETTINGS", "setting-firewall-efl", "IDS_ST_BODY_FIREWALL_ENABLED_ABB");
			}
		}

		if ((ret = tel_deinit(tapi_handle)) != TAPI_API_SUCCESS) {
			_DBG("failed to deinitialized tapi handler:%d", ret);
		}

	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");

	}
	/*evas_object_del(ad->win); */
	free(btn_str);
	elm_exit();
}


static void _text_rotation_changed(void *data, Evas_Object *obj, void *event_info)
{
	int rot = -1;
	Evas_Object *win = obj;
	Evas_Object *layout = data;

	rot = elm_win_rotation_get(win);
	if (rot == 90 || rot == 270)
		elm_layout_file_set(layout, POPUP_EDJ, "label_layout_landscape");
	else
		elm_layout_file_set(layout, POPUP_EDJ, "label_layout");
}

static Eina_Bool _help_handler(void *data)
{
	struct appdata *ad = data;
	Evas_Coord_Rectangle rect = {0,};
	evas_object_geometry_get(ad->help_eo, &rect.x, &rect.y, &rect.w, &rect.h);
	if (ad->help_type != HELP_NONE) {
		syspop_help_popup_circle_block_create(ad->win, ad->popup,
		                                      &ad->help_circle, &ad->help_popup,
		                                      _(MODE_SYSTEM_POPUP_MSG),
		                                      rect.x + rect.w / 2, rect.y + rect.h / 2, &rect);
	}
	ad->help_timer = NULL;
	return FALSE;
}

static void _help_rotation_changed(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = (struct appdata *)data;
	/*delete old help */
	if (ad->help_circle) {
		evas_object_del(ad->help_circle);
		ad->help_circle = NULL;
	}
	if (ad->help_popup) {
		evas_object_del(ad->help_popup);
		ad->help_popup = NULL;
	}
	/*update help */
	if (ad->help_type != HELP_NONE) {
		if (ad->help_timer) {
			ecore_timer_del(ad->help_timer);
			ad->help_timer = NULL;
		}
		ad->help_timer = ecore_timer_add(0.5, (Ecore_Task_Cb)_help_handler, ad);
	}
}

static void _ultra_popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	struct appdata *ad = data;

	if (!ad)
		return;

	/*delete old help */
	if (ad->help_type != HELP_NONE) {
		if (ad->help_circle) {
			evas_object_del(ad->help_circle);
			ad->help_circle = NULL;
		}
		if (ad->help_popup) {
			evas_object_del(ad->help_popup);
			ad->help_popup = NULL;
		}
	}

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	setting_retm_if(NULL == btn_str, "btn_str is NULL");
	if (!strcmp(btn_str, _("IDS_ST_BUTTON_OK")) || !strcmp(btn_str, _("IDS_ST_BUTTON_ENABLE"))) {
		/*make complete flag */
		int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check export xml");
		int m_key;
		vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		} else if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}
		_DBG("OK OK");
		release_evas_object(&(ad->popup));
		ad->bg = _create_bg(ad->win, NULL, "group_list");
		/*progress bar */
		ad->enable_popup = __popup_with_progressbar(ad, ad->enable_popup, ad->win, PROGRESSBAR_STYLE,
		                                            NULL, _("IDS_ST_TPOP_ENABLING_ULTRA_POWER_SAVING_MODE_ING"), __progress_popup_cb, 3/*0*/, TRUE, TRUE, TRUE);	/* 2 seconds to wait in maximum */

		/*assistive light off, P140422-06240 */
		ret = led_set_brightness_with_noti(0, TRUE);
		if (0 != ret) {
			_DBG(">>>led SET TO ZERO err [%d]", ret);
		} else {
			vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to disable popup off reminder %d***", ret);
			}
		}

		/*set ultra == emergency*/
		_mode_apply_settings(ad, emergency_settings);
		/*set gray tone */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_EMERGENCY) == 0) {
			_DBG("set upsm, in indicator");
		}
		free(btn_str);
	} else if (!strcmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB"))) {
		_DBG("CANCEL");
		free(btn_str);
		elm_exit();
	} else {
		free(btn_str);
	}
}

void _set_battery(Evas_Object *battery, int percent)
{
	int stage = ((double) percent) / 100 * 35;
	stage = stage == 0 ? 1 : stage;

	char stage_txt[16] = { '\0' };
	snprintf(stage_txt, sizeof(stage_txt), "%02d", stage);

	char percent_txt[16] = { '\0' };
	snprintf(percent_txt, sizeof(percent_txt), "%d%%", percent);

	/*batt level image set */
	elm_object_signal_emit(battery, stage_txt, "batt_image");

	/*batt percent text */
	elm_object_part_text_set(battery, "text2", percent_txt);
}

void _set_stand_by_time(Evas_Object *stand_by_time, int percent)
{
	int stage = ((double) percent) / 100 * 36;	/*battery percent value */
	double sbt_val = ((double) percent) * 2.80f;	/*estimated time value */

	stage = stage == 0 ? 1 : stage;

	char stage_txt[16] = { '\0' };
	snprintf(stage_txt, sizeof(stage_txt), "%02d", stage);

	char percent_txt[16] = { '\0' };
	if (sbt_val > 24) {
		double decimal = sbt_val / 24.0f - (int)sbt_val / 24.0f;
		if (decimal == 0.0) {
			snprintf(percent_txt, sizeof(percent_txt), "%d%s", (int)(sbt_val / 24.0f), _("IDS_ST_MBODY_DAYS_M_WEEKDAY"));
		} else {
			snprintf(percent_txt, sizeof(percent_txt), "%.1f%s", sbt_val / 24.0f, _("IDS_ST_MBODY_DAYS_M_WEEKDAY"));
		}
	} else if (sbt_val > 1) {
		snprintf(percent_txt, sizeof(percent_txt), "%dh%dm", (int)sbt_val, (int)(sbt_val / 0.6f));
	} else {
		snprintf(percent_txt, sizeof(percent_txt), "%dm", (int)(sbt_val / 0.6f));
	}

	elm_object_signal_emit(stand_by_time, stage_txt, "standby_lev_img");
	elm_object_part_text_set(stand_by_time, "text_r3", percent_txt);
}

Evas_Object *_ultra_popup_with_btn(void *data,
                                   Evas_Object *parent,
                                   char *title,
                                   char *text,
                                   _cb_func response_cb,
                                   int timeout, int btn_num)
{
	_DBG("_ultra_popup_with_btn");
	struct appdata *ad = data;
	if (!ad)
		return NULL;

	Evas_Object *popup = ad->popup;

	if (text) {
		Evas_Object *layout = elm_layout_add(popup);
		int rotation = elm_win_rotation_get(parent);
		if (rotation == 90 || rotation == 270)
			elm_layout_file_set(layout, UPMS_EDJ, "label_layout_landscape");
		else
			elm_layout_file_set(layout, UPMS_EDJ, "label_layout");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		Evas_Object *inner_layout = elm_layout_add(popup);
		elm_layout_file_set(inner_layout, UPMS_EDJ, "inner_layout");
		evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		Evas_Object *scroller = elm_scroller_add(layout);
		elm_object_style_set(scroller, "list_effect");
		elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
		elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		evas_object_show(scroller);
		elm_object_content_set(scroller, inner_layout);

#if 0
		/*1) battery image, text */
		/*text */
		elm_object_part_text_set(inner_layout, "text1" , _("IDS_ST_BODY_BATTERY_PERCENTAGE_ABB"));
		/*image */
		Evas_Object *img = NULL;
		img = elm_image_add(inner_layout);
		elm_image_file_set(img, "/usr/apps/org.tizen.setting/res/icons/savingpower/popup/battery/A01-11_popup_battery_bg.png", NULL);
		elm_object_part_content_set(inner_layout, "def_image", img);
		/*batt level image, text */
		int percent;
		if (vconf_get_int(VCONFKEY_SYSMAN_BATTERY_CAPACITY, &percent))
			_DBG("Cannot get the vconf value for VCONFKEY_SYSMAN_BATTERY_CAPACITY");
		_set_battery(inner_layout, percent);
#endif
		/*2) standby time image, text */
		/*text r1, r2 -> text r1 */
		elm_object_part_text_set(inner_layout, "text_r1" , _("IDS_ST_BODY_ESTIMATED_MAX_STANDBY_TIME_ABB"));
		/*elm_object_part_text_set(inner_layout, "text_r2" ,"standby time"); */
		/*image */
		Evas_Object *standby_img = NULL;
		standby_img = elm_image_add(inner_layout);
		elm_image_file_set(standby_img, "/usr/apps/org.tizen.setting/res/icons/savingpower/popup/standby_time/A01-11_popup_time_bg.png", NULL);
		elm_object_part_content_set(inner_layout, "standby_image", standby_img);
		/*standby time image, text */
		/*_set_stand_by_time(inner_layout, percent);*/

		/* label for long typing popup */
		Evas_Object *label = elm_label_add(popup);
		elm_object_style_set(label, "popup/default");
		elm_label_line_wrap_set(label, ELM_WRAP_MIXED);

		char ultra_desc[1024] = {0, };
		snprintf(ultra_desc, sizeof(ultra_desc), "%s", text);
		elm_object_text_set(label, ultra_desc);
		evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(label);

		elm_object_part_content_set(inner_layout, "guide_text", label);

		elm_object_part_content_set(layout, "elm.swallow.content", scroller);

		elm_object_content_set(popup, layout);
		evas_object_smart_callback_add(elm_object_top_widget_get(popup), "rotation,changed", _text_rotation_changed, layout);
	}
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	if (btn_num > 0) { /*the case popup has button or buttons */
		Evas_Object *btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
		elm_object_part_content_set(popup, "button1", btn1);	/*right */
		evas_object_smart_callback_add(btn1, "clicked", response_cb, ad);
		Evas_Object *btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _("IDS_ST_BUTTON_ENABLE"));
		elm_object_part_content_set(popup, "button2", btn2);	/*left */
		evas_object_smart_callback_add(btn2, "clicked", response_cb, ad);
		evas_object_show(popup);
	}
	evas_object_show(popup);
	return popup;
}


static Evas_Object *mode_syspopup_create_popup_with_label_check(void *data,
                                                                Evas_Object *parent,
                                                                char *title,
                                                                char *content,
                                                                char *check_str,
                                                                Evas_Object **check,
                                                                Evas_Smart_Cb response_cb,
                                                                int timeout,
                                                                const char *btn_left, const char *btn_right)
{
	_DBG("ENTER mode_syspopup_create_popup_with_label_check");
	struct appdata *ad = data;
	if (!ad) return NULL;
	Evas_Object *popup = elm_popup_add(parent);
#if 0
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (btn_left) {
		Evas_Object *btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _(btn_left));
		elm_object_part_content_set(popup, "button1", btn1);
		if (response_cb) {
			evas_object_smart_callback_add(btn1, "clicked", response_cb, ad);
		}
	}
	if (btn_right) {
		Evas_Object *btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _(btn_right));
		elm_object_part_content_set(popup, "button2", btn2);
		if (response_cb) {
			evas_object_smart_callback_add(btn2, "clicked", response_cb, ad);
		}
	}
	evas_object_show(popup);
	return popup;
#endif
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb, NULL);
	/*setting_add_hardkey_features(popup, data); */
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	Evas_Object *label = NULL;
	label = elm_label_add(popup);
	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
#if 1
	char arr[2048] = {'\0',};
	snprintf(arr, 2048, "<color=#000000>%s</color>", _(content));
	elm_object_text_set(label, arr);
#else
	evas_object_color_set(label, 0, 0, 0, 255);
	evas_object_show(label);
#endif
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);

	Evas_Object *layout = elm_layout_add(popup);
	elm_layout_file_set(layout, MODE_POPUP_EDJ, "popup_checkview_setting");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (check) {
		*check = elm_check_add(popup);
#if 1
		elm_object_style_set(*check, "multiline");
		if (check_str) {
			elm_object_text_set(*check, _(check_str));
		}
#else
		elm_object_style_set(*check, "multiline");
		elm_object_text_set(*check, "Don't ask again Don't ask again  Don't ask again  Don't ask again  Don't ask again	Don't ask again  ");
#endif
		evas_object_size_hint_align_set(*check, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(*check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_show(*check);

		Evas_Object *box = elm_box_add(popup);
		elm_box_horizontal_set(box, EINA_TRUE);
		elm_box_pack_end(box, *check);

		/* add text */
		Evas_Object *lbl = elm_label_add(popup);
		elm_object_text_set(lbl, _(check_str));
		evas_object_show(lbl);
		elm_box_pack_end(box, lbl);

		elm_box_padding_set(box, 30, 0);
		evas_object_show(box);

		elm_object_part_content_set(layout, "elm.swallow.end", box);
	}

	elm_object_part_content_set(layout, "elm.swallow.content", label);


	elm_object_content_set(popup, layout);
	evas_object_data_set(popup, "check_str", check_str);
	Evas_Object *btn1;
	Evas_Object *btn2;

	if (btn_left) {
		btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _(btn_left));
		elm_object_part_content_set(popup, "button1", btn1);
		if (response_cb) {
			evas_object_smart_callback_add(btn1, "clicked", response_cb, ad);
		}
	}
	if (btn_right) {
		btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _(btn_right));
		elm_object_part_content_set(popup, "button2", btn2);
		if (response_cb) {
			evas_object_smart_callback_add(btn2, "clicked", response_cb, ad);
		}
	}
	evas_object_show(popup);
	return popup;
}

void fini_tts(struct appdata *ad)
{
	_INFO("fini tts");
	if (ad->tts) {
		int result = 0;
		tts_state_e current_state;
		tts_get_state(ad->tts, &current_state);

		if (TTS_STATE_READY == current_state) {
			result = tts_unprepare(ad->tts);
			if (TTS_ERROR_NONE != result) {
				_ERR("Fail to prepare function : ret(%d)\n", result);
			}
		}

		result = tts_destroy(ad->tts);
		if (TTS_ERROR_NONE != result) {
			_ERR("Fail to destroy handle : result(%d)", result);
		}
		ad->tts = NULL;
	}
}

void __tts_completed_cb(tts_h tts, int utt_id, void *user_data)
{
	fini_tts(user_data);
}

void init_tts(struct appdata *ad)
{
	_INFO("init tts");
	int result = tts_create(&(ad->tts));
	if (TTS_ERROR_NONE != result) {
		_ERR("Fail to get handle : result(%d)", result);
		ad->tts = NULL;
	} else {
		result = tts_set_utterance_completed_cb(ad->tts, __tts_completed_cb, ad);
		if (TTS_ERROR_NONE != result) {
			_ERR("Fail to set callback : ret(%d)\n", result);
		}
		result = tts_set_mode(ad->tts, TTS_MODE_DEFAULT);
		if (TTS_ERROR_NONE != result) {
			_ERR("Fail to set tts mode : ret(%d)\n", result);
		}
		result = tts_prepare(ad->tts);/*async API,need to wait until TTS_STATE_READY */
		if (TTS_ERROR_NONE != result) {
			_ERR("Fail to prepare function : ret(%d)\n", result);
		}
	}
}

void play_text(struct appdata *ad, const char *text)
{
	/*paly prompts */
	_INFO("play text");
	if (!ad || !ad->tts) {
		_ERR("ad or tts == NULL");
		return;
	}
	int ret = 0;
	setting_retm_if(NULL == ad->tts, "ad->tts is NULL");
	ret = tts_stop(ad->tts);
	if (TTS_ERROR_NONE != ret) {
		_ERR("failed to call tts_stop: %d", ret);
	}
	int utt_id;
	setting_retm_if(NULL == text, "text is NULL");
	ret = tts_add_text(ad->tts, text, NULL,
	                   TTS_VOICE_TYPE_AUTO, TTS_SPEED_AUTO, &utt_id);
	if (TTS_ERROR_NONE != ret) {
		_ERR("failed to call tts_add_text: %d", ret);
	}

	ret = tts_play(ad->tts);
	if (TTS_ERROR_NONE != ret) {
		_ERR("failed to call tts_play: %d", ret);
	}
}


static void __mode_syspopup_add(struct appdata *ad)
{
	if (!ad)
		return;

	Evas_Object *popup = NULL;
	Evas_Object *btn1;
	Evas_Object *btn2 = NULL;

	/*evas_object_show(ad->win); */
	if (ad->win) {
		popup = elm_popup_add(ad->win);
		ad->popup = popup;

		if (popup == NULL) {
			_ERR("elm_popup_add failed..!!");
			return;
		}
	}
	int m_key;
	vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
	if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_EMERGENCY)) {
		vconf_set_int(VCONFKEY_SETAPPL_EMERGENCY_UI_MODE, EMERGENCY_UI_MODE_EMERGENCY);
		elm_object_part_text_set(popup, "title,text", _(KeyStr_Enable_emergency_Mode));
		Evas_Object *layout = elm_layout_add(popup);
		int rotation = elm_win_rotation_get(ad->win);
		if (rotation == 90 || rotation == 270)
			elm_layout_file_set(layout, POPUP_EDJ, "label_layout_landscape");
		else
			elm_layout_file_set(layout, POPUP_EDJ, "label_layout");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		/* label for long typing popup */
		Evas_Object *label = elm_label_add(popup);
		elm_object_style_set(label, "popup/default");
		elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
		elm_object_text_set(label, _(KeyStr_Survivor_Mode_POP_DESC));
		evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_show(label);

		Evas_Object *scroller = elm_scroller_add(layout);
		elm_object_style_set(scroller, "list_effect");
		elm_scroller_bounce_set(scroller, EINA_TRUE, EINA_TRUE);
		elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		evas_object_show(scroller);
		elm_object_content_set(scroller, label);

		elm_object_part_content_set(layout, "elm.swallow.content", scroller);

		elm_object_content_set(popup, layout);
		evas_object_smart_callback_add(elm_object_top_widget_get(popup), "rotation,changed", _text_rotation_changed, layout);

		btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
		elm_object_part_content_set(popup, "button1", btn1);	/*right */
		evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_emergency_cb, ad);
		btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _("IDS_ST_BUTTON_ENABLE"));
		elm_object_part_content_set(popup, "button2", btn2);	/*left */
		evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_emergency_cb, ad);
		evas_object_show(popup);

		/*help */
		if (ad->help_type != HELP_NONE) {
			if (ad->help_timer) {
				ecore_timer_del(ad->help_timer);
				ad->help_timer = NULL;
			}
			ad->help_eo = btn2;
			ad->help_timer = ecore_timer_add(0.5, (Ecore_Task_Cb)_help_handler, ad);
		}
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_ULTRA)) {
		vconf_set_int(VCONFKEY_SETAPPL_EMERGENCY_UI_MODE, EMERGENCY_UI_MODE_ULTRA_POWER_SAVING);
		char ultra_desc[1024] = {0, };
		snprintf(ultra_desc, sizeof(ultra_desc), "- %s<br>- %s<br>- %s<br>- %s",
		         _(KeyStr_Survivor_Mode_POP_DESC1),
		         _(KeyStr_Survivor_Mode_POP_DESC2),
		         _(KeyStr_Survivor_Mode_POP_DESC3),
		         _(KeyStr_Survivor_Mode_POP_DESC4));
		_ultra_popup_with_btn(ad, ad->win, "IDS_ST_HEADER_ULTRA_POWER_SAVING_MODE",
		                      ultra_desc,
		                      _ultra_popup_cb, 0,
		                      2);
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_EMERGENCY_SETTING)) {
		/*upsm in setting */
		/*make complete flag */
		int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check export xml");
		/*ultra power saving mode !! */
		if (popup) {
			evas_object_del(popup);
			popup = NULL;
		}

		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		} else if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}
#if LCD_FREQ_CTRL
		ad->is_lcd_ctrl = EINA_TRUE;
		_LCD_freq_ctrl(ad);
#endif
		/*assistive light off, P140422-06240 */
		ret = led_set_brightness_with_noti(0, TRUE);
		if (0 != ret) {
			_DBG(">>>led SET TO ZERO err [%d]", ret);
		} else {
			vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to disable popup off reminder %d***", ret);
			}
		}
		_mode_apply_settings(ad, emergency_settings);
#if 1 /*---------following two invoking will cause KERNAL PANNIC */
		{
			_DBG("set ultra power saving mode in setting");
		}
#endif
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_EMERGENCY_SAFETY)) {
		vconf_set_int(VCONFKEY_SETAPPL_EMERGENCY_UI_MODE, EMERGENCY_UI_MODE_EMERGENCY);

		/*make complete flag */
		int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check export xml");
		if (popup) {
			evas_object_del(popup);
			popup = NULL;
		}

		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		} else if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}

		/*assistive light off, P140422-06240 */
		ret = led_set_brightness_with_noti(0, TRUE);
		if (0 != ret) {
			_DBG(">>>led SET TO ZERO err [%d]", ret);
		} else {
			vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
			if (ret != 0) {
				_DBG("*** [ERR] Failed to disable popup off reminder %d***", ret);
			}
		}

		_mode_apply_settings(ad, emergency_settings);
		/*set gray tone */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_EMERGENCY) == 0) {
			_DBG("set on SETTING_PSMODE_EMERGENCY");
		}
		elm_exit();
	}
	if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_NORMAL)) {
		elm_object_style_set(popup, "transparent");
		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		int ultra_emergency;
		vconf_get_int(VCONFKEY_SETAPPL_EMERGENCY_UI_MODE, &ultra_emergency);

		if (ultra_emergency == 1) {
			elm_object_text_set(popup, _(KeyStr_Emergency_off_Mode_POP_DESC));
			elm_object_part_text_set(popup, "title,text", _("IDS_ST_HEADER_DISABLE_EMERGENCY_MODE_ABB"));
		} else if (ultra_emergency == 2) {
			elm_object_text_set(popup, _(KeyStr_Ultra_off_Mode_POP_DESC));
			elm_object_part_text_set(popup, "title,text", _("IDS_ST_HEADER_ULTRA_POWER_SAVING_MODE"));
		}

		if (is_recovery) {
			release_evas_object(&(ad->popup));
			ad->bg = _create_bg(ad->win, NULL, "group_list");

			/*progress bar */
			ad->disable_popup = __popup_with_progressbar(ad, ad->disable_popup, ad->win, PROGRESSBAR_STYLE,
			                                             NULL, _(KeyStr_Loading), __progress_popup_cb, 3/*0*/, TRUE, TRUE, FALSE);	/* 2 seconds to wait in maximum */

			/*set normal sub*/
			/*make complete flag */
			int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
			if (ret) _ERR("check export xml");
			/*torch light off with diable emergency/ups mode, P140507-03841 */
			int t_ret = led_set_brightness_with_noti(0, TRUE);
			if (0 != t_ret) {
				_DBG(">>>led SET TO ZERO err [%d]", t_ret);
			} else {
				vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
				if (t_ret != 0) {
					_DBG("*** [ERR] Failed to disable popup off reminder %d***", t_ret);
				}
			}

			int b_network_lcd_off;
			vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, &b_network_lcd_off);
			if (b_network_lcd_off) {
				vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, EINA_FALSE);
			}
			_mode_apply_settings(ad, normal_settings);

			/* set to default */
			int opposite;
			ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
			if (ret) _DBG("vconf_get_bool failed");
			if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_NORMAL) == 0) {
				_DBG("set on SETTING_PSMODE_NORMAL");
			}
		} else {
			btn1 = elm_button_add(popup);
			elm_object_style_set(btn1, "popup");
			elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
			elm_object_part_content_set(popup, "button1", btn1);	/*right */
			evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_normal_cb, ad);
			btn2 = elm_button_add(popup);
			elm_object_style_set(btn2, "popup");
			elm_object_text_set(btn2, _("IDS_ST_BUTTON_DISABLE"));
			elm_object_part_content_set(popup, "button2", btn2);	/*left */
			evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_normal_cb, ad);
			evas_object_show(popup);

			if (vconf_notify_key_changed(VCONFKEY_PM_STATE, lcd_state_changed, ad) != 0)
				_DBG("vconf key notify failed");
		}
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_REMOVE_FLAG)) {
		int ret = remove_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check remove xml");
		if (popup) {
			evas_object_del(popup);
			popup = NULL;
		}
		elm_exit();
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_POWERFUL)) {
		int flight;
		int ret = vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight);
		if (ret) _DBG("vconf_get_bool failed");

		if (flight == EINA_TRUE) {
			/*cannot set with flight mode on */
			elm_object_style_set(popup, "transparent");
			evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			elm_object_text_set(popup, _("IDS_IM_POP_THIS_FEATURE_IS_NOT_AVAILABLE_WHILE_FLIGHT_MODE_IS_ON"));

			elm_popup_timeout_set(popup, 2.0);
			evas_object_smart_callback_add(popup, "block,clicked", timeout_response_cb, ad);
			evas_object_smart_callback_add(popup, "timeout", timeout_response_cb, ad);

			evas_object_show(popup);
		} else {
			if (popup) {
				evas_object_del(popup);
				popup = NULL;
			}
			int m_key, first;
			vconf_get_int(VCONFKEY_SETAPPL_PSMODE, &m_key);
			if (m_key == SETTING_PSMODE_NORMAL) {
				/*backup */
				_mode_backup_settings(ad);
			}

			/*set normal sub*/
			int ret = preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, &first);

			if (first) {
				_mode_apply_settings(ad, powerful_settings_def);
				preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, 0);
			} else {
				_mode_apply_settings(ad, powerful_settings);
				_DBG("right to set powerful");
			}
			/* set to default */
			int opposite;
			ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
			if (ret) _DBG("vconf_get_bool failed");
			/*emergency mode -> normal */
			if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_POWERFUL) == 0) {
				_DBG("set on SETTING_PSMODE_POWERFUL");
			}

			evas_object_smart_callback_add(elm_object_top_widget_get(popup), "rotation,changed", _help_rotation_changed, ad);

			/*help */
			if (ad->help_type != HELP_NONE) {
				if (ad->help_timer) {
					ecore_timer_del(ad->help_timer);
					ad->help_timer = NULL;
				}
				ad->help_eo = btn2;
				ad->help_timer = ecore_timer_add(0.5, (Ecore_Task_Cb)_help_handler, ad);
			}

			if (vconf_notify_key_changed(VCONFKEY_PM_STATE, lcd_state_changed, ad) != 0)
				_DBG("vconf key notify failed");
			elm_exit();
		}
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_POWERFUL_SETTING)) {
		if (popup) {
			evas_object_del(popup);
			popup = NULL;
		}

		if (m_key == SETTING_PSMODE_NORMAL) {
			/*backup */
			_mode_backup_settings(ad);
		}

		int first;
		/*set normal sub*/
		int ret = preference_get_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, &first);
		if (first) {
			_mode_apply_settings(ad, powerful_settings_def);
			preference_set_int(PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR, 0);
		} else {
			_mode_apply_settings(ad, powerful_settings);
			_DBG("right to set powerful");
		}

		/* set to default */
		int opposite;
		ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
		if (ret) _DBG("vconf_get_bool failed");
		/*emergency mode -> normal */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_POWERFUL) == 0) {
			_DBG("set on SETTING_PSMODE_POWERFUL");
		}
		elm_exit();
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_NORMAL_SETTING)) {
		/*make complete flag */
		int ret = export_xml(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml");
		if (ret) _ERR("check export xml");

		if (m_key == SETTING_PSMODE_POWERFUL) {
			/*backup */
			_mode_power_backup_settings(ad);
		}
		/*torch light off with diable emergency/ups mode, P140507-03841 */
		int t_ret = led_set_brightness_with_noti(0, TRUE);
		if (0 != t_ret) {
			_DBG(">>>led SET TO ZERO err [%d]", t_ret);
		} else {
			vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, EINA_FALSE);
			if (t_ret != 0) {
				_DBG("*** [ERR] Failed to disable popup off reminder %d***", t_ret);
			}
		}

		int b_network_lcd_off;
		vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, &b_network_lcd_off);
		if (b_network_lcd_off) {
			vconf_set_bool(VCONFKEY_SETAPPL_NETWORK_PERMIT_WITH_LCD_OFF_LIMIT, EINA_FALSE);
		}

		/*set normal sub*/
		_mode_apply_settings(ad, normal_settings);
		/* set to default */
		int opposite;
		ret = 0;
		ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, &opposite);
		if (ret) _DBG("vconf_get_bool failed");
		/*emergency mode -> normal */
		if (vconf_set_int(VCONFKEY_SETAPPL_PSMODE, SETTING_PSMODE_NORMAL) == 0) {
			_DBG("set on SETTING_PSMODE_NORMAL");
		}
		elm_exit();
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_BACKUP_SETTING)) {
		if (popup) {
			evas_object_del(popup);
			popup = NULL;
		}
		/*backup*/
		_mode_backup_settings(ad);
		elm_exit();
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_FLIGHTMODE_ON)) {

		setting_set_i18n(MODE_SYSPOPUP_PACKAGE, MODE_SYSPOPUP_LOCALEDIR);
		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		_DBG("--------------------------------------------------------------");
		elm_object_text_set(popup, _(KeyStr_Fligtmode_Popup_On_Text));
		_DBG("--------------------------------------------------------------");

		elm_object_part_text_set(popup, "title,text", _(KeyStr_Fligtmode_Popup_Title));
		btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
		elm_object_part_content_set(popup, "button1", btn1);	/*right */
		evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_flightmode_cb, ad);
		btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _("IDS_ST_BUTTON_ENABLE"));
		elm_object_part_content_set(popup, "button2", btn2);	/*left */
		evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_flightmode_cb, ad);
		evas_object_show(popup);

	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_FLIGHTMODE_OFF)) {

		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		elm_object_text_set(popup, _(KeyStr_Fligtmode_Popup_Off_Text));

		elm_object_part_text_set(popup, "title,text", _(KeyStr_Fligtmode_Popup_Title));
		btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
		elm_object_part_content_set(popup, "button1", btn1);	/*right */
		evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_flightmode_cb, ad);
		btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _("IDS_ST_BUTTON_OK"));
		elm_object_part_content_set(popup, "button2", btn2);	/*left */
		evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_flightmode_cb, ad);
		evas_object_show(popup);

	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_NETRESTRICTIONMODE_CHANGE)) {

		/* Network restriction mode change */
		int vconf_value;
		int ret = vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &vconf_value);
		if (ret != 0) {
			_DBG("network_restrict_mode vconf get failed");
		} else {
			_DBG("network_restrict_mode vconf get %d", vconf_value);
		}

		elm_object_style_set(popup, "transparent");
		evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		if (vconf_value) {
			/* on -> off */
			elm_object_text_set(popup, _("IDS_QP_POP_NETWORK_RESTRICTION_MODE_WILL_BE_DISABLED_JPN"));
		} else {
			/* off -> on */
			elm_object_text_set(popup, _("IDS_QP_POP_IN_NETWORK_RESTRICTION_MODE_YOU_CAN_USE_ALL_YOUR_APPLICATIONS_WITHOUT_CONNECTING_TO_THE_NETWORK_YOU_CAN_ALSO_RECEIVE_CALLS_AND_SMS_MESSAGES_MSG_JPN"));
		}
		elm_object_part_text_set(popup, "title,text", _("IDS_ST_HEADER_NETWORK_RESTRICTION_MODE_ABB_JPN"));
		btn1 = elm_button_add(popup);
		elm_object_style_set(btn1, "popup");
		elm_object_text_set(btn1, _("IDS_ST_BUTTON_CANCEL_ABB"));
		elm_object_part_content_set(popup, "button1", btn1);	/*right */

		btn2 = elm_button_add(popup);
		elm_object_style_set(btn2, "popup");
		elm_object_text_set(btn2, _("IDS_ST_BUTTON_OK"));
		elm_object_part_content_set(popup, "button2", btn2);	/*left */

		if (vconf_value) {
			/* on -> off */
			evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_netrestrictionmode_change_off_cb, popup);
			evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_netrestrictionmode_change_off_cb, popup);
		} else {
			/* off -> on */
			evas_object_smart_callback_add(btn1, "clicked", __mode_syspopup_netrestrictionmode_change_on_cb, popup);
			evas_object_smart_callback_add(btn2, "clicked", __mode_syspopup_netrestrictionmode_change_on_cb, popup);
		}

		evas_object_show(popup);
	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_MOBILEDATA_ON)) {
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = mode_syspopup_create_popup_with_label_check(ad, ad->win, NULL,
		                                                        _(Data_packet_Popup_desc),
		                                                        KeyStr_DO_NOT_ASK_AGAIN,
		                                                        &(ad->popup_chk), __mode_syspopup_mobiledata_cb, 0,
		                                                        "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_OK");


	} else if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_MOBILEDATA_OFF)) {
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = mode_syspopup_create_popup_with_label_check(ad, ad->win, NULL,
		                                                        KeyStr_MOBILEDATA_Popup_Off_Text,
		                                                        KeyStr_DO_NOT_ASK_AGAIN,
		                                                        &(ad->popup_chk), __mode_syspopup_mobiledata_cb, 0,
		                                                        "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_OK");
	}
}

static int __mode_syspopup_reset(bundle *b, void *data)
{
	struct appdata *ad = data;
	const char *val;
	int ret = 0;

	if (is_recovery) {
		_DBG("recovery");
		/*recovery normal */
		ad->type = strdup(MODE_SYSTEM_POPUP_NORMAL);
		evas_object_show(ad->win);
		__mode_syspopup_add(ad);

		return 0;
	}

	ad->b = bundle_dup(b);

	/* When syspopup is already loaded, remove the popup and load new popup */
	if (syspopup_has_popup(b)) {
		syspopup_reset(b);
	} else {
		ret = syspopup_create(b, &handler, ad->win, ad);
		_DBG("ret = %d", ret);

		char *help = NULL;
		help = bundle_get_val(b, "help_type");
		if (help != NULL)
			ad->help_type = HELP_PS_MODE;

		if (ret == 0) {
			val = bundle_get_val(b, "_MODE_SYSTEM_POPUP_TYPE_");
			if (val) {
				ad->type = strdup(val);
				_INFO("type :%s", ad->type);

				if (ad->type && !strcmp(ad->type, MODE_SYSTEM_POPUP_POWERFUL)) {
					int flight;
					int ret = vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight);
					if (ret) _DBG("vconf_get_bool failed");

					if (flight) {
						evas_object_show(ad->win);
					}
				} else {
					evas_object_show(ad->win);
				}
			}
		} else {
			_ERR("Unknown popup type");
			elm_exit();
		}
		__mode_syspopup_add(ad);

		return 0;
	}

	return 0;
}

static int mode_reboot_create_noti_only(const char *key_str, const char *package, const char *title)
{
	notification_h noti = NULL;
	notification_error_e ret = NOTIFICATION_ERROR_NONE;

	/* not need to remove noti */
	noti = notification_new(NOTIFICATION_TYPE_ONGOING,
	                        NOTIFICATION_GROUP_ID_NONE, NOTIFICATION_PRIV_ID_NONE);
	if (!noti) {
		_DBG("Fail to notification_new [%d]", ret);
		return SETTING_RETURN_FAIL;
	}

	ret = notification_set_layout(noti, NOTIFICATION_LY_ONGOING_EVENT);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_layout [%d]", ret);
		goto FAIL;
	}

	if (!strcmp(PKG_BLOCKING_MODE, package)) {
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, IMG_BlockingMode);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON_FOR_INDICATOR, INDI_IMG_BlockingMode);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
	} else if (!strcmp(PKG_FIREWALL, package)) {
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON, IMG_Firewall);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
		ret = notification_set_image(noti, NOTIFICATION_IMAGE_TYPE_ICON_FOR_INDICATOR, INDI_IMG_Firewall);
		if (ret != NOTIFICATION_ERROR_NONE) {
			_DBG("Fail to notification_set_image [%d]", ret);
			goto FAIL;
		}
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_TITLE,
	                            title,
	                            title, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text(noti,
	                            NOTIFICATION_TEXT_TYPE_CONTENT,
	                            key_str,
	                            key_str, NOTIFICATION_VARIABLE_TYPE_NONE);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_text_domain(noti, SETTING_PACKAGE, SETTING_LOCALEDIR);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_text [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_pkgname(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_pkgname [%d]", ret);
		goto FAIL;
	}

	ret = notification_set_application(noti, package);	/*pkg ex : "setting-blockingmode-efl" */
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_set_application [%d]", ret);
		goto FAIL;
	}

	int noti_id = 0;
	ret = notification_insert(noti, &noti_id);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_insert [%d]", ret);
		goto FAIL;
	}

	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
		goto FAIL;
	}

	return 0 ;

FAIL:
	ret = notification_free(noti);
	if (ret != NOTIFICATION_ERROR_NONE) {
		_DBG("Fail to notification_free [%d]", ret);
	}
	return 1;
}

/*
 * Check whether file existed or not
 *
 * @param	pszFilePathName	[in] : File path (/Folder)
 * @return	TRUE on success, FALSE on failure
 */
static int __is_file_exist(char *pszFilePathName)
{
	FILE	*pFile = NULL;

	pFile = fopen(pszFilePathName, "r");
	if (pFile == NULL) {
		_DBG("fopen('%s')=NULL, File not existed.", pszFilePathName);
		return 0;
	}

	_DBG("fopen('%s')!=NULL, File existed.", pszFilePathName);

	fclose(pFile);
	pFile = NULL;

	return 1;
}

static int __mode_reboot_noti_method()
{
	/*get vconf to notif register */
	/*blockingmode, drivingmode, flightmode */
	int driving, firewall, ret;
	ret = 0;
	ret |= vconf_get_bool(VCONFKEY_SETAPPL_FIREWALL_KEY, &firewall);
	/*ret |= vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight); */
	_DBG("__mode_reboot_noti_method");
	printf("noti method start\n");
	if (ret) {
		printf("mode get vconf failed\n");
		return ret;
	}
	/*set noti for on status */
	if (driving)
		ret |= mode_reboot_create_noti_only("IDS_ST_BODY_INCOMING_CALLS_AND_NEW_NOTIFICATIONS_WILL_BE_READ_OUT_AUTOMATICALLY", PKG_DRIVING_MODE, "IDS_VC_MBODY_NOTIFICATION_READ_OUT");
	if (firewall)
		ret |= mode_reboot_create_noti_only("IDS_ST_BODY_CONFIGURE_FIREWALL_SETTINGS", PKG_FIREWALL, "IDS_ST_BODY_FIREWALL_ENABLED_ABB");
	/*if(flight) mode_reboot_create_noti_only ... not need to */

	return ret;	/*zero for success */
}
static void __win_del(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}

static Evas_Object *__create_win(const char *name)
{
	_DBG("ENTER __create_win");
	Evas_Object *eo;
	int w;
	int h;
	Ecore_X_Window xwin;
	unsigned int val = 1;

	eo = elm_win_add(NULL, name, ELM_WIN_DIALOG_BASIC);
	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		elm_win_alpha_set(eo, EINA_TRUE);
		elm_win_raise(eo);
		evas_object_smart_callback_add(eo, "delete,request", __win_del, NULL);
		#if 0
		ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
		evas_object_resize(eo, w, h);
		xwin = elm_win_xwindow_get(eo);
		ecore_x_window_prop_card32_set(xwin, ECORE_X_ATOM_E_ILLUME_ACCESS_CONTROL, &val, 1);
		#endif
	}

	return eo;
}

static int __mode_syspopup_app_create(void *data)
{
	_DBG("ENTER __mode_syspopup_app_create");
	struct appdata *ad = data;
	int r;

	/* create window */
	ad->win = __create_win(PACKAGE);
	if (ad->win == NULL) {
		_ERR("Create win error");
		return -1;
	}

	ad->ly_main = elm_layout_add(ad->win);
	evas_object_size_hint_weight_set(ad->ly_main, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->ly_main, "layout", "application", "default");

	evas_object_show(ad->ly_main);
	elm_win_resize_object_add(ad->win, ad->ly_main);
	evas_object_show(ad->win);

	/* init internationalization */
	setting_set_i18n(MODE_SYSPOPUP_PACKAGE, MODE_SYSPOPUP_LOCALEDIR);
	/*#define TEST */
#ifdef TEST
	mode_syspopup_create_popup_with_label_check(ad, ad->ly_main, NULL,
	                                            KeyStr_MOBILEDATA_Popup_Off_Text,
	                                            KeyStr_DO_NOT_ASK_AGAIN,
	                                            &(ad->popup_chk), __mode_syspopup_mobiledata_cb, 0,
	                                            "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_OK");

#endif
	return 0;
}

static int __mode_syspopup_app_terminate(void *data)
{
	_DBG("__mode_syspopup_app_terminate");
	struct appdata *ad = data;

	if (ad->tts) {
		fini_tts(ad);
	}

	if (vconf_ignore_key_changed(VCONFKEY_PM_STATE, lcd_state_changed) != 0)
		_DBG("vconf key ignore failed");

	_mini_destroy();

	if (ad->win) {
		evas_object_del(ad->win);
		ad->win = NULL;
	}

	if (ad->lcd_freq_timer) {
		ecore_timer_del(ad->lcd_freq_timer);
		ad->lcd_freq_timer = NULL;
	}

	if (ad->play_timer) {
		ecore_timer_del(ad->play_timer);
		ad->play_timer = NULL;
	}

	if (ad->disable_popup) {
		evas_object_del(ad->disable_popup);
		ad->disable_popup = NULL;
	}

	if (ad->enable_popup) {
		evas_object_del(ad->enable_popup);
		ad->enable_popup = NULL;
	}

	elm_exit();
	return 0;
}

static int __mode_syspopup_app_pause(void *data)
{
	/*struct appdata *ad = data; */

	return 0;
}

static int __mode_syspopup_resume(void *data)
{
	/*struct appdata *ad = data; */

	return 0;
}

int main(int argc, char *argv[])
{
	_DBG("ENTER main");
#if REBOOT_METHOD_FROM_SHELL
	int i = 0;
	/*for(; i < argc;i++) _DBG("argv[%d]:%s", i, argv[i]); */
	int command = 99;
	int res = 77;
	if (argc >= 2) command = atoi(argv[1]);
	/*_DBG("[0]%s [1]%s [2]%s command=%d , argc = %d",argv[0], argv[1],argv[2],command, argc); */
	switch (command) {
		case 1:/* from sh */
			sleep(1);	/*notification_is_service_ready */
			/*function for vconf get and notification register */
			if (!__mode_reboot_noti_method()) {
				/*success */
				res = 0;
				/*function for recovery interrupt in emergency/upms work */
				if (__is_file_exist(DEFAULT_COMPLETE_CONFIG_FLAG_DIR"mode_perf.xml")) {
					/*should recovery!, set normal with reboot */
					is_recovery = true;
				} else {
					return res;
				}
			} else {
				_DBG("noti method failing");
				return 1;	/*failing code from sh */
			}
		default:
			res = 99;
	}
	_DBG("[in reboot-mode sys seq from shell]end:res[%d]\n", res);
#endif

	struct appdata ad;

	struct appcore_ops ops = {
		.create = __mode_syspopup_app_create,
		.terminate = __mode_syspopup_app_terminate,
		.pause = __mode_syspopup_app_pause,
		.resume = __mode_syspopup_resume,
		.reset = __mode_syspopup_reset,
	};

	memset(&ad, 0x0, sizeof(struct appdata));
	ops.data = &ad;

	return appcore_efl_main(PACKAGE, &argc, &argv, &ops);
}
