/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

/**
 *@defgroup setting-security
 *Creation code for setting-security
 */

#include <setting-locktype.h>
#include <efl_extension.h>

#include <auth-passwd.h>

#define Max_Passwd_View_Type_Len	64
#define SETTING_LOCKTYPE_PACKAGE_NAME "org.tizen.setting-locktype"
#define LOCKTYPE_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-locktype/res/locale"


static void _result_password_cb(app_control_h request, app_control_h reply,
		app_control_result_e code, void *priv);


static void _popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingLocktype *ad = (SettingLocktype *)data;
	if (ad->save_popup) {
		evas_object_del(ad->save_popup);
		ad->save_popup = NULL;
	}
	ui_app_exit();
}

static int _pwd_handler_do_nothing(SettingLocktype *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	/*SettingLocktype *ad = (SettingLocktype *)data; */
	/* DO NOTHING */
	return 0;
}

static int _pwd_handler_sec_pw_passwd(SettingLocktype *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingLocktype *ad = (SettingLocktype *)data;

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
			SETTING_SCREEN_LOCK_TYPE_PASSWORD) == 0) {
		/* set radio */
		if (ad->lock_type_rd)
			elm_radio_value_set(ad->lock_type_rd,
					SETTING_SCREEN_LOCK_TYPE_PASSWORD);

		ad->save_popup = setting_create_popup(ad, ad->md.win_main, NULL,
				"IDS_ST_POP_NEW_PASSWD_SAVED",
				_popup_resp_cb,
				POPUP_INTERVAL, FALSE, FALSE, 0);
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}

	return 0;
}

static int _pwd_handler_sec_pw_simple_passwd(SettingLocktype *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingLocktype *ad = (SettingLocktype *)data;

	SETTING_TRACE("ad->selected_lock_type:%s", ad->selected_lock_type);
	/*'face and voice ' and 'simple password' share the same PASSWD_TYPE
	 * and callback function */
	int lock_type = SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD;
	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type) ==
			0) {
		/* set radio */
		if (ad->lock_type_rd)
			elm_radio_value_set(ad->lock_type_rd, lock_type);

		ad->save_popup = setting_create_popup(ad, ad->md.win_main, NULL,
				"IDS_ST_POP_NEW_PASSWD_SAVED",
				_popup_resp_cb,
				POPUP_INTERVAL, FALSE, FALSE, 0);
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}
	return 0;
}

static struct _locktype_item locktype_table[] = { {
	SETTING_LOCKTYPE_PW_PASSWORD,
	"SETTING_PW_TYPE_SET_PASSWORD",
	_pwd_handler_sec_pw_passwd }, {
	SETTING_LOCKTYPE_PW_SIMPLE_PASSWD,
	"SETTING_PW_TYPE_SET_SIMPLE_PASSWORD",
	_pwd_handler_sec_pw_simple_passwd }, {
	SETTING_LOCKTYPE_PW_CHANGE_PASSWD,
	"SETTING_PW_TYPE_SET_PASSWORD",
	_pwd_handler_sec_pw_passwd }, {
	SETTING_LOCKTYPE_PW_CHANGE_SIMPLE_PASSWD,
	"SETTING_PW_TYPE_SET_SIMPLE_PASSWORD",
	_pwd_handler_sec_pw_simple_passwd },
/*--------------------------------------------------------------------------- */
{ SETTING_LOCKTYPE_PW_MAX, NULL, _pwd_handler_do_nothing } };

static void _cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);

	/* ad is point to data */
	SettingLocktype *ad = (SettingLocktype *)data;
	setting_view_update(&setting_view_locktype_main, ad);
}

bool on_app_create(void *priv)
{
	SettingLocktype *ad = priv;
	retv_if(!ad, false);

	if (app_init(&ad->md, SETTING_LOCKTYPE_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	bindtextdomain(SETTING_PACKAGE, LOCKTYPE_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_locktype_main, NULL);

	setting_create_Gendial_itc("dialogue/1text", &(ad->itc_1text));
	setting_create_Gendial_itc("multiline/1text", &(ad->itc_bg_1icon));
	setting_create_Gendial_itc("dialogue/2text.3", &(ad->itc_2text_2));
	setting_create_Gendial_itc("dialogue/1text.1icon",
			&(ad->itc_1text_1icon));
	setting_create_Gendial_itc("dialogue/title", &(ad->itc_group_item));
	setting_create_Gendial_itc("dialogue/2text.3/expandable",
			&(ad->itc_2text_3_parent));

	ad->input_pwd = NULL;
	ad->notify = NULL;

	return true;
}

static void on_app_control(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingLocktype *ad = priv;
	ret_if(!ad);

	app_control_get_extra_data(service, "current", &ad->input_pwd);
	app_control_get_extra_data(service, "caller", &ad->caller);

	if (ad->caller && !safeStrCmp(ad->caller, "encryption")) {
		ad->viewtype = SETTING_LOCKTYPE_VIEWTYPE_ENCRYPTION;
	} else if (ad->caller && !safeStrCmp(ad->caller, "lockscreen")) {
		ad->viewtype = SETTING_LOCKTYPE_VIEWTYPE_RECOVERY;
		vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
				SETTING_SCREEN_LOCK_TYPE_SWIPE);
#if SECURITY_SERVER
//		int result = security_server_reset_pwd("0000", 0, 0);
//??		int result = auth_passwd_reset_passwd(AUTH_PWD_NORMAL,uid,
//		"0000");
//		SETTING_TRACE_DEBUG("reset_pwd result : %d", result);
#endif
	} else {
		ad->viewtype = SETTING_LOCKTYPE_VIEWTYPE_MAIN;
	}
	FREE(ad->caller);

	setting_view_node_set_cur_view(&setting_view_locktype_main);
	setting_view_create(&setting_view_locktype_main, (void *)ad);
	evas_object_event_callback_add(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, _cb_resize,
			ad);
}

static void on_app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void on_app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void on_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingLocktype *ad = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, _cb_resize);

	if (ad->save_popup) {
		evas_object_del(ad->save_popup);
		ad->save_popup = NULL;
	}

	setting_view_destroy(&setting_view_locktype_main, ad);

	FREE(ad->input_pwd);

	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}

	SETTING_TRACE_END;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = on_app_create,
		.app_control = on_app_control,
		.pause = on_app_pause,
		.resume = on_app_resume,
		.terminate = on_app_terminate,
	};
	SettingLocktype app_data;
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

gboolean setting_locktype_create_password_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, FALSE);
	SettingLocktype *ad = (SettingLocktype *)data;

	/* prevent the ug from being loaded again due to window event queuing */
	/* added by JTS: CQ H0100135346 */
	if (ad->passwd_loaded) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}

	if ((ad->pw_type < 0) || (ad->pw_type > SETTING_LOCKTYPE_PW_MAX))
		return FALSE;

	char str[Max_Passwd_View_Type_Len + 1] = { 0, };
	safeCopyStr(str, locktype_table[ad->pw_type].pw_type_string,
			Max_Passwd_View_Type_Len);

	app_control_h svc;
	if (app_control_create(&svc))
		return FALSE;

	app_control_add_extra_data(svc, "viewtype", str);
	if (ad->viewtype == SETTING_LOCKTYPE_VIEWTYPE_ENCRYPTION)
		app_control_add_extra_data(svc, "encryption", "TRUE");

	if (ad->input_pwd)
		app_control_add_extra_data(svc, "current", ad->input_pwd);

	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);
	ad->passwd_loaded = !app_launcher_svc("org.tizen.setting-password", svc,
			_result_password_cb, ad);
	if (!ad->passwd_loaded) { /* error handling */
		evas_object_show(ad->md.ly_main);
	}

	app_control_destroy(svc);

	return TRUE;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
/* -1	on fail */
/* 0~n	on success */
static int _get_locktype_table_index(char *name)
{
	int i = 0;

	for (i = 0; i < SETTING_LOCKTYPE_PW_MAX; i++) {
		if (0 == safeStrCmp(name, locktype_table[i].pw_type_string))
			return i;
	}
	return -1;
}

static void _result_password_cb(app_control_h request, app_control_h reply,
		app_control_result_e code, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingLocktype *ad = (SettingLocktype *)priv;
	char *result = NULL;
	ret_if(!ad);

	ad->passwd_loaded = false;
	if (ad->md.ly_main)
		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_TRUE);

	app_control_get_extra_data(reply, "result", &result);

	/* divided pw view type */
	SETTING_TRACE("ad->pw_type: %d result :%s", ad->pw_type, result);
	ret_if(NULL == result);

	if (safeStrCmp(result, "Cancel")) {
		int index = _get_locktype_table_index(result);

		if (index != -1) {
			locktype_table[index].passwd_handler(ad, NULL);
			ad->pw_type = locktype_table[index].pw_type_num;
		} else {
			SETTING_TRACE_ERROR("no table entry");
		}
	}

	evas_object_show(ad->md.ly_main);
	FREE(result);
	SETTING_TRACE_END;
}
