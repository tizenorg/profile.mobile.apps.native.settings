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

#include <setting-about-main.h>
#include <aul.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <system_info.h>
#include <fcntl.h>

#include <bluetooth.h>
#include <wifi.h>

#include <app_manager.h>
#include <efl_extension.h>
#include <ITapiModem.h>

#define MAX_DEVICE_NAME_LEN		32

#define DEVICED_BUS_NAME			"org.tizen.system.deviced"
#define DEVICED_OBJECT_PATH			"/Org/Tizen/System/DeviceD"
#define DEVICED_INTERFACE_NAME		DEVICED_BUS_NAME
#define DEVICED_PATH_BOARD			DEVICED_OBJECT_PATH"/Board"
#define DEVICED_INTERFACE_BOARD		DEVICED_INTERFACE_NAME".Board"
#define METHOD_GET_SERIAL			"GetSerial"

/** @todo : need to handle unicode encoded input characters */

#define DEVICE_NAME		"IDS_ST_HEADER_DEVICE_NAME"
#define DEVICE_NAME_EXCEED_STR	"IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"

#define EMPTY_LIMITATION_STR	"IDS_ST_BODY_THE_NAME_FIELD_CANNOT_BE_EMPTY"
#define SOFTWARE_UPDATE_STR	"IDS_ST_MBODY_SOFTWARE_UPDATE"
#define DEVICE_NAME_UNALLOWED_SPACE_KEY_STR "Device name cannot contain spaces"
#define DEVICE_NAME_DEFAULE	"Redwood"

#define APP_NAME "Settings"

static int setting_about_main_create(void *cb);
static int setting_about_main_destroy(void *cb);
static int setting_about_main_update(void *cb);
static int setting_about_main_cleanup(void *cb);

setting_view setting_view_about_main = {
		.create = setting_about_main_create,
		.destroy = setting_about_main_destroy,
		.update = setting_about_main_update,
		.cleanup = setting_about_main_cleanup, };

/**
 * @brief popup response callback fuc.
 *
 * @param data application context
 * @param obj Evas Object
 * @param event_info even type
 *
 * @return no return
 */
static void __setting_about_popup_rsp_cb(void *data, Evas_Object *obj,
		void *event_info)
{

	SETTING_TRACE_BEGIN;
	setting_retm_if(obj == NULL, "obj parameter is NULL");
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	/*Evas_Object *popup = data; */
	switch (btn_type(obj)) {
	case POPUP_RESPONSE_OK: {
		if (ad->popup) {
			ad->popup_showed_flag = FALSE;

			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		if (ad->item_dev_name && ad->item_dev_name->eo_check) {
			char *name_value = NULL;
			name_value = vconf_get_str(
					VCONFKEY_SETAPPL_DEVICE_NAME_STR);
			if (NULL == name_value) {
				/*use the defaule name if get vconf failed*/
				name_value = (char *)g_strdup(
						DEVICE_NAME_DEFAULE);
			}
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_TRUE);
			/*for genlist update */
			elm_entry_entry_set(ad->item_dev_name->eo_check,
					name_value);
			elm_entry_cursor_end_set(ad->item_dev_name->eo_check);
			G_FREE(ad->item_dev_name->sub_desc);/*release first */
			ad->item_dev_name->sub_desc = (char *)g_strdup(
					name_value);
			FREE(name_value);
			SETTING_TRACE("ad->item_dev_name->sub_desc:%s",
					ad->item_dev_name->sub_desc);
			Ecore_IMF_Context *imf_context =
					(Ecore_IMF_Context *)elm_entry_imf_context_get(
							ad->item_dev_name->eo_check);
			if (imf_context)
				ecore_imf_context_input_panel_show(imf_context);

		}
		break;
	}
	default: {
		break;
	}
	}

}

/**
 * @brief callback run for idler task to destroy an popup.
 *
 * @param data application context
 *
 * @return FALSE for call it once, TRUE for call once the CPU is in idler
 */
static Eina_Bool __setting_about_popup_remove_delay(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = data;
	if (ad->popup_space) {
		evas_object_del(ad->popup_space);
		ad->popup_space = NULL;
		ad->popup_showed_flag = FALSE;
	}
	return FALSE;
}

/**
 * @brief callback for popup destory.
 *
 * @param data application context
 * @param e Evas handler
 * @param obj Evas object
 * @param event_info event type
 */
static void __setting_about_popup_del_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)

{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	ad->popup = NULL;
	ad->popup_showed_flag = FALSE;
}

/**
 * @brief callback run for idler task to show an popup.
 *
 * @param data application context
 */
static Eina_Bool __setting_about_popup_show_delay(void *data)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, FALSE, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	/* popup the message and display the original name if the input is
	 * empty*/
	SETTING_TRACE("POP UP again");

	if (NULL == ad->popup) {
		ad->popup_showed_flag = true;
		ad->popup = setting_create_popup(ad, ad->md.win_main,
				"IDS_ST_HEADER_ENTER_VALID_DEVICE_NAME_ABB",
				"IDS_ST_POP_YOU_MUST_ENTER_A_DEVICE_NAME",
				__setting_about_popup_rsp_cb, 0, FALSE, FALSE,
				1, "IDS_ST_BUTTON_OK_ABB");
		evas_object_event_callback_add(ad->popup, EVAS_CALLBACK_DEL,
				__setting_about_popup_del_cb, ad);
	}

	return FALSE;
}

/**
 * @brief callback run for idler task to destroy an popup.
 *
 * @param data application context
 *
 * @return FALSE for call it once, TRUE for call once the CPU is in idler
 */
static void __setting_about_popup_mobile_ap_turn_off_ask_resp_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	int response_type = btn_type(obj);

	if (POPUP_RESPONSE_OK == response_type) {
		/* disable tehering*/
		if (ad->item_dev_name && ad->item_dev_name->eo_check) {
			elm_object_focus_allow_set(ad->item_dev_name->eo_check,
					EINA_TRUE);
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_TRUE);
			elm_entry_cursor_end_set(ad->item_dev_name->eo_check);

			Ecore_IMF_Context *imf_context =
					(Ecore_IMF_Context *)elm_entry_imf_context_get(
							ad->item_dev_name->eo_check);
			if (imf_context)
				ecore_imf_context_input_panel_show(imf_context);

		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {

		/* do nothing */

	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

}

/**
 * @brief regulatory popup response cp.
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 */

static void __setting_about_popup_regulatory_info_rsp_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	evas_object_del(data);
}

/**
 * @brief show regulatory info popup.
 *
 * @param data application context
 */
static void __setting_about_popup_regulatory_info(void *data)
{
	SETTING_TRACE_BEGIN;

	Evas_Object *popup;
	SettingAboutUG *ad;
	Evas_Object *layout;
	Evas_Object *icon;
	Evas_Object *btn1;
	char buf[4096];

	ad = (SettingAboutUG *)data;
	popup = elm_popup_add(ad->md.ly_main);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			eext_popup_back_cb, NULL);

	elm_object_part_text_set(popup, "title,text",
			_("IDS_ST_BODY_REGULATORY_INFORMATION_ABB"));
	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME,
			"popup_center_image");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	icon = elm_image_add(layout);
	snprintf(buf, sizeof(buf), "%s/00_popup_regulatory_info.png",
			SETTING_ICON_PATH);
	elm_image_file_set(icon, buf, NULL);
	elm_image_resizable_set(icon, EINA_TRUE, EINA_TRUE);
	elm_object_part_content_set(layout, "elm.swallow.content", icon);

	evas_object_show(layout);
	elm_object_content_set(popup, layout);

	btn1 = elm_button_add(popup);
	elm_object_style_set(btn1, "popup");
	elm_object_text_set(btn1, _("IDS_ST_BUTTON_OK"));
	elm_object_part_content_set(popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked",
			__setting_about_popup_regulatory_info_rsp_cb, popup);
	evas_object_show(popup);

	SETTING_TRACE_END;
}

/**
 * @brief get Model Name.
 *		- No ini: Unavailable
 *		- I9500 @ target: GT-I9500
 *		- I9500 @ emul: SDK
 *		- SLP	@ target: GT-SLP
 *		- SLP	@ emul: SDK
 *
 * @param szStr the buffer for store the model name
 * @param nSize buffer size
 */
void setting_about_main_get_phone_model_name(char *szStr, int nSize)
{
	retm_if(szStr == NULL, "szStr parameter is NULL");

	char *value = NULL;
	int ret = system_info_get_platform_string(
			"http://tizen.org/system/model_name", &value);
	SETTING_TRACE("value : %s", value);
	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"fail to call system_info_get_platform_string");
		FREE(value);
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return;
	}

	if (value)
		snprintf(szStr, nSize, "%s", value);
	else
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));


	FREE(value);
}

/**
 * @brief get software version.
 *
 * @param szStr the buffer for store SW version
 * @param nSize buffer size
 */
void setting_about_main_get_sw_version(char *szStr, int nSize)
{
	retm_if(szStr == NULL, "szStr parameter is NULL");

	char *version = NULL;
	int ret = system_info_get_platform_string(
			"http://tizen.org/feature/platform.version", &version);
	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"fail to call system_info_get_platform_string");
		FREE(version);
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return;
	}

	char *build_info = NULL;
	ret = system_info_get_platform_string(
			"http://tizen.org/system/build.string", &build_info);
	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"fail to call system_info_get_platform_string");
		FREE(version);
		FREE(build_info);
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return;
	}

#if 0
	snprintf(szStr, nSize, "TIZEN %s (%s)", version, build_info);
#else
	snprintf(szStr, nSize, "TIZEN %s", version);
#endif
	FREE(version);
	FREE(build_info);
}

/**
 * @brief get software version.
 *
 * @param szStr the buffer for store SW version
 * @param nSize buffer size
 */
void setting_about_main_get_wifi_mac_address_string(char *str, int size)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(str == NULL, "str parameter is NULL");
	setting_retm_if(size < SETTING_ABOUT_WIFI_MAC_STR_LEN + 1,
			"size parameter is wrong");

	char *mac_addr = NULL;
	int status = wifi_get_mac_address(&mac_addr);
	if (WIFI_ERROR_NONE != status)
		SETTING_TRACE_ERROR("Failed to call wifi_get_mac_address()");
	else
		SETTING_TRACE_ERROR("Success to call wifi_get_mac_address()");

	if (mac_addr) {
		safeCopyStr(str, mac_addr, SETTING_ABOUT_WIFI_MAC_STR_LEN);
		SETTING_TRACE_DEBUG("get_wifi_mac_address : %s", mac_addr);
		free(mac_addr);
	}

	SETTING_TRACE_DEBUG("get_wifi_mac_address : %s", str);

	int state = 0;
	int ret = vconf_get_int(VCONFKEY_WIFI_STATE, &state);
	if (0 != ret)
		SETTING_TRACE_ERROR("Failed to get wifi state");
	else
		SETTING_TRACE("get wifi state: %d", state);

	if ((status != WIFI_ERROR_NONE) || (safeStrLen(str) == 0)
			|| 0 == state) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	}
}

static DBusMessage *dbus_method_sync_with_reply(const char *dest,
		const char *path, const char *interface, const char *method,
		const char *sig, char *param[])
{
	DBusConnection *conn;
	DBusMessage *msg;
	DBusMessageIter iter;
	DBusMessage *reply;
	DBusError err;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	retv_if(!conn, NULL);

	msg = dbus_message_new_method_call(dest, path, interface, method);
	retv_if(!msg, NULL);

	dbus_message_iter_init_append(msg, &iter);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	if (dbus_error_is_set(&err)) {
		SETTING_TRACE_ERROR("dbus_connection_send error(%s:%s)",
				err.name, err.message);
		dbus_error_free(&err);
		reply = NULL;
	}

	dbus_message_unref(msg);
	return reply;
}

/**
 * @brief get serial number.
 *
 * @param szStr the buffer for store SN
 * @param nSize buffer size
 */
void setting_about_main_get_sn(char *szStr, int nSize)
{
	retm_if(szStr == NULL, "szStr parameter is NULL");

	DBusError err;
	DBusMessage *msg;
	int ret, len;
	char *serial_num;

	msg = dbus_method_sync_with_reply(DEVICED_BUS_NAME, DEVICED_PATH_BOARD,
	DEVICED_INTERFACE_BOARD, METHOD_GET_SERIAL, NULL, NULL);
	if (!msg) {
		SETTING_TRACE_ERROR("fail send message");
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return;
	}

	dbus_error_init(&err);

	ret = dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &serial_num,
			DBUS_TYPE_INT32, &len, DBUS_TYPE_INVALID);
	if (!ret) {
		SETTING_TRACE_ERROR("no message : [%s:%s]", err.name,
				err.message);
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return;
	}

	dbus_message_unref(msg);
	dbus_error_free(&err);

	if (serial_num && strlen(serial_num))
		snprintf(szStr, nSize, "%s", serial_num);
	else
		snprintf(szStr, nSize, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
}

/**
 * @brief get current battery percentage.
 *
 * @param szStr the buffer for store battery percentage
 * @param nSize buffer size
 */
void setting_about_main_get_battery_string(char *str, int size)
{
	setting_retm_if(str == NULL, "str parameter is NULL");

	int val = -1;
	char file[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
	snprintf(file, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO, "%s/%s/%s",
			SETTING_ABOUT_POWER_SUPPLY_PATH, "battery", "capacity");

	char buf[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
	int fd = 0;
	int r = 0;

	fd = open(file, O_RDONLY);
	if (fd != -1) {
		r = read(fd, buf, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		if ((r >= 0) && (r < MAX_DISPLAY_STR_LEN_ON_PHONE_INFO)) {
			buf[r] = '\0';
			val = atoi(buf);
			snprintf(str, size, "%d%s", val, "\%");
		} else {
			SETTING_TRACE_ERROR("read file fail");
			snprintf(str, size, "%s",
					_("IDS_ST_HEADER_UNAVAILABLE"));
		}

		close(fd);
	} else {
		SETTING_TRACE_ERROR("open file fail");
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	}
}

/**
 * @brief get bluetooth address.
 *
 * @param szStr the buffer for store bluetooth address
 * @param nSize buffer size
 */
void setting_about_main_get_bluetooth_address_string(char *str, int size)
{
	setting_retm_if(str == NULL, "str parameter is NULL");

	int ret = 0;

	ret = bt_initialize();
	setting_retm_if(ret != 0, "bluetooth initialization failed");

	char *local_address = NULL;
	ret = bt_adapter_get_address(&local_address);

	bt_adapter_state_e state;
	ret = bt_adapter_get_state(&state);

	bt_deinitialize();

	if (ret < 0 || BT_ADAPTER_DISABLED == state) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		SETTING_TRACE_DEBUG(
				"bt address : %s failed get bt address with error code:%d",
				str, ret);
	} else {
		SETTING_TRACE_DEBUG("bt address : %s", local_address);
		snprintf(str, size, "%s", local_address);
	}

	if (local_address)
		free(local_address);
}

/**
 * @brief get CPU usage.
 *
 * @param szStr the buffer for store CPU usage
 * @param nSize buffer size
 *
 * @return -ENOENT means failed to get CPU info, otherwise ok.
 */
int setting_about_main_stat_get_cpuinfo(float *usr_pct, float *sys_pct)
{
	setting_retvm_if(usr_pct == NULL, -ENOENT, "param usr_pct is null");
	setting_retvm_if(sys_pct == NULL, -ENOENT, "param sys_pct is null");

	/*	default value */
	*usr_pct = 0.0;
	*sys_pct = 0.0;
	static unsigned long long usr_time = 0, /*nice_time = 0,*/sys_time = 0;
	static unsigned long long old_usr = 0, /*old_nice = 0,*/old_sys = 0;
	static struct timeval old_tv, cur_tv;
	unsigned long long elapsed_tick;
	long tick_per_sec;
	long cpu_num;
	FILE *fp = NULL;
	int ret = 0;

	tick_per_sec = sysconf(_SC_CLK_TCK);
	cpu_num = sysconf(_SC_NPROCESSORS_ONLN);

	if (cpu_num < 1)
		cpu_num = 1;

	gettimeofday(&cur_tv, NULL);
	fp = fopen(SETTING_ABOUT_STAT_PATH, "r");
	if (fp == NULL) {
		SETTING_TRACE_ERROR("fp == NULL");
		return -ENOENT;
	}

	char cpu_info[MAX_COMMON_BUFFER_LEN] = { 0, };
	char *cpu_info_p = fgets(cpu_info, MAX_COMMON_BUFFER_LEN, fp);

	if (cpu_info_p == NULL) {
		SETTING_TRACE_ERROR("fgets failed");
		fclose(fp); /* free - code */
		return -ENOENT;
	}

	char *substr = NULL;
	unsigned long long tmp_long = 0;
	int i = 0;
	/* split cpu_info, get 3 numbers headmost*/
	while ((substr = strsep(&cpu_info_p, " \t")) != NULL) {
		char *endptr = NULL;
		tmp_long = strtoull(substr, &endptr, 10);
		if (tmp_long != 0 && tmp_long != ULLONG_MAX) {
			switch (i) {
			case 0:
				usr_time = tmp_long;
				break;
			case 1:
				/*nice_time = tmp_long;*/
				break;
			case 2:
				sys_time = tmp_long;
				break;
			default:
				break;
			}
			i++;
		}
		if (i >= 3)
			break;
	}

	fclose(fp);
	fp = NULL;
	if (old_usr == 0) {
		ret = -EAGAIN;
		SETTING_TRACE_ERROR("old_usr == 0");
		goto out;
	}

	elapsed_tick = (unsigned long long)(((long long)cur_tv.tv_sec
			- old_tv.tv_sec) * tick_per_sec
			+ (((long long)cur_tv.tv_usec - old_tv.tv_usec)
					* tick_per_sec) / 1000000);

	/* REMOVE BS */
	if (elapsed_tick != 0) {
		*usr_pct = ((float)(usr_time - old_usr) * 100 / elapsed_tick)
				/ cpu_num;
	} else {
		*usr_pct = 0;
	}
	/* REMOVE BS */
	if (elapsed_tick != 0) {
		*sys_pct = ((float)(sys_time - old_sys) * 100 / elapsed_tick)
				/ cpu_num;
	} else {
		*sys_pct = 0;
	}

out:
	old_usr = usr_time;
	/*old_nice = nice_time; */
	old_sys = sys_time;
	old_tv = cur_tv;

	return ret;
}

#if 0
/**
 * @brief get device root status.
 *
 * @param szStr the buffer for store device root status
 * @param nSize buffer size
 */
static void setting_about_main_get_status_string(char *str, int size)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(str == NULL, "str parameter is NULL");

	struct_cordon_status *cordon_status = NULL;
	cordon_status = (struct_cordon_status *)malloc(
			sizeof(struct_cordon_status));
	if (NULL == cordon_status) {
		SETTING_TRACE_ERROR("Memory allocation failed");
		return;
	}
	memset(cordon_status, 0, sizeof(struct_cordon_status));

	SETTING_TRACE("cordon_get_previous_root_status_before");
	int ret = cordon_get_previous_root_status(cordon_status);
	SETTING_TRACE("cordon_get_previous_root_status_end");
	if (0 == ret) {
		if (cordon_status->is_device_rooted) {
			snprintf(str, size, "%s",
					SETTING_ABOUT_STATUS_ROOT_STR);
		} else {
			snprintf(str, size, "%s",
					SETTING_ABOUT_STATUS_UNROOT_STR);
		}
	} else {
		SETTING_TRACE_ERROR(
				"Failed to call cordon_get_previous_root_status() with error code %d",
				ret);
		snprintf(str, size, "%s", "IDS_ST_HEADER_UNAVAILABLE");
	}
	free(cordon_status);
}
#endif

/**
 * @brief create button on naviframe tile part.
 *
 * @param parent the new created button's parent layout
 * @param text button text
 * @param func callback for button click
 * @param data application data for button click callback.
 *
 * @return new created button
 */
static Evas_Object *setting_about_naviframe_btn_create(Evas_Object *parent,
		const char *text, Evas_Smart_Cb func, void *data)
{
	Evas_Object *btn = elm_button_add(parent);
	retv_if(!btn, NULL);
	elm_object_style_set(btn, "naviframe/title_text");
	elm_object_text_set(btn, text);
	evas_object_smart_callback_add(btn, "clicked", func, data);
	return btn;
}

/**
 * @brief button done response callback.
 *
 * @param data application data
 * @param obj the button evas object
 * @param event_info event type
 */
static void setting_about_naviframe_btn_done_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	if (ad->item_dev_name && ad->item_dev_name->eo_check) {
		const char *entry_str = NULL;
		char *entry_str_utf8 = NULL;
		char *name_value = NULL;

		entry_str = elm_entry_entry_get(ad->item_dev_name->eo_check);
		if (entry_str)
			entry_str_utf8 = elm_entry_markup_to_utf8(entry_str);

		name_value = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);

		if (!ad->empty_flag
				&& safeStrCmp(name_value, entry_str_utf8) != 0) {
			if (0 != vconf_set_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR,
					entry_str_utf8)) {
				SETTING_TRACE_ERROR("Set vconf[%s] failed",
						VCONFKEY_SETAPPL_DEVICE_NAME_STR);
			}
		}
		if (EINA_TRUE == elm_object_focus_get(
					ad->item_dev_name->eo_check)) {
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_FALSE);
		}
		FREE(entry_str_utf8);
	}
	elm_naviframe_item_pop(ad->md.navibar_main);
	SETTING_TRACE_END;
}

/**
 * @brief button cancel response callback.
 *
 * @param data application data
 * @param obj the button evas object
 * @param event_info event type
 */
static void setting_about_naviframe_btn_cancel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	if (ad->item_dev_name && ad->item_dev_name->eo_check) {
		char *name = NULL;
		char *pa_sub_desc = NULL;
		name = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
		if (name)
			pa_sub_desc = elm_entry_utf8_to_markup(name);

		if (pa_sub_desc) {
			elm_entry_entry_set(ad->item_dev_name->eo_check,
					pa_sub_desc);
			G_FREE(ad->item_dev_name->sub_desc);/*release first */
			ad->item_dev_name->sub_desc = (char *)g_strdup(
					pa_sub_desc);
		}
		FREE(pa_sub_desc);

		if (isSpaceStr(ad->item_dev_name->sub_desc))
			ad->empty_flag = TRUE;
		else
			ad->empty_flag = FALSE;

		SETTING_TRACE("ad->item_dev_name->sub_desc:%s",
				ad->item_dev_name->sub_desc);
		if (EINA_TRUE
				== elm_object_focus_get(
						ad->item_dev_name->eo_check)) {
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_FALSE);
		}
	}
	elm_naviframe_item_pop(ad->md.navibar_main);
	SETTING_TRACE_END;
}
static void __setting_about_main_popup_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(obj == NULL, "obj parameter is NULL");
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	switch (btn_type(obj)) {
	case POPUP_RESPONSE_OK: {
		if (ad->popup_space) {
			ad->popup_showed_flag = FALSE;
			evas_object_del(ad->popup_space);
			ad->popup_space = NULL;
		}
		break;
	}
	default: {
		break;
	}
	}
}

/**
 * @brief device name entry input handing
 *	- get the current entry text
 *	- check if the current input text is reached to max length
 *	  and show messge to user
 *
 * @param data application data
 * @param obj the entry evas object
 */
static void __setting_about_entry_changed_cb(void *data, Evas_Object *obj)
{
	retm_if(!data || !obj, "Data parameter is NULL");
	/*return if entry is not focused too */

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	SettingAboutUG *ad = list_item->userdata;
	retm_if(ad == NULL, "Data parameter is NULL");

	const char *entry_str = elm_entry_entry_get(obj);
	int entry_len = safeStrLen(entry_str);
	SETTING_TRACE("entry_str:[%s], lenght:%d", entry_str, entry_len);

	SETTING_TRACE("maxLengthReachFlag:[%d], enterKeyPressFlag:%d",
			list_item->maxLengthReachFlag,
			list_item->enterKeyPressFlag);

	/*fix PLM P131106-02776
	 after keyboard popup cut the entry text, the entry text don't reach
	 max length, so need to delete popup */
	if (ad->popup_showed_flag && ad->popup_space
			&& !list_item->maxLengthReachFlag) {
		evas_object_del(ad->popup_space);
		ad->popup_space = NULL;
		ad->popup_showed_flag = FALSE;

	}

	list_item->maxLengthReachFlag = FALSE;
	list_item->enterKeyPressFlag = FALSE;
	list_item->spaceKeyPressFlag = FALSE;

	/*for genlist update */
	G_FREE(list_item->sub_desc);/*release first */
	list_item->sub_desc = (char *)g_strdup(entry_str);

	if (isSpaceStr(entry_str)) {
		if (!ad->empty_flag)
			ad->empty_flag = TRUE;
	} else {
		if (ad->empty_flag)
			ad->empty_flag = FALSE;
	}
}

/**
 * @brief device name entry changed callback.
 *
 * @param data application data
 * @param obj the entry evas object
 * @param event_info event type
 */
static void __setting_about_entry_device_name_changed_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retm_if(obj == NULL, "obj is NULL");
	setting_retm_if(data == NULL, "data is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;
	SettingAboutUG *ad = list_item->userdata;
	retm_if(ad == NULL, "Data parameter is NULL");

	/* 1. entry is empty */
	/* 2. entry is not empty */
	if (list_item->item) {
		if (elm_entry_is_empty(obj)) {
			elm_object_item_signal_emit(list_item->item,
					"elm,state,eraser,hide", "");
		} else {
			Eina_Bool device_focus_status = elm_object_focus_get(
					ad->item_dev_name->eo_check);
			if (device_focus_status) {
				/*SETTING_TRACE("name is NOT empty, but focus"); */
				elm_object_item_signal_emit(list_item->item,
						"elm,state,eraser,show", "");
			}
		}
		elm_object_item_signal_emit(list_item->item,
				"elm,state,guidetext,hide", "");
	}

	__setting_about_entry_changed_cb(data, obj);
	/*SETTING_TRACE_END; */
}

/**
 * @brief device name entry focus callback.
 *
 * @param data application data
 * @param obj the entry evas object
 * @param event_info event type
 *
 * Focused callback will show X marked button and hide guidetext.
 */
static void __setting_about_entry_focused(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data || !obj, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_dev_name = data;

	/* accessibiliy */
	elm_entry_cursor_end_set(obj);
	const char *txt = elm_entry_entry_get(obj);
	if (!txt || 0 == strlen(txt))
		txt = elm_object_part_text_get(obj, "elm.guide");

	/* Say entry focused */
	Eina_Stringshare *str = eina_stringshare_printf(
			"%s, Edit Field, Keyboard opened", txt);
	elm_access_say((char *)str);
	eina_stringshare_del(str);

	SettingAboutUG *ad = item_dev_name->userdata;
	retm_if(!ad, "ad parameter is NULL");
	Ecore_IMF_Context *imf_context =
			(Ecore_IMF_Context *)elm_entry_imf_context_get(
					item_dev_name->eo_check);
	if (imf_context)
		ecore_imf_context_input_panel_show(imf_context);
}

/**
 * @brief device name entry unfocus callback.
 *
 * @param data application data
 * @param obj the entry evas object
 * @param event_info event type
 */
static void __setting_about_entry_unfocus_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data || !obj, "Data parameter is NULL");

	const char *entry_str = elm_entry_entry_get(obj);
	char *entry_str_utf8 = NULL;
	entry_str_utf8 = elm_entry_markup_to_utf8(entry_str);
	SETTING_TRACE("To store \"%s\" into vconf[%s]", entry_str_utf8,
			VCONFKEY_SETAPPL_DEVICE_NAME_STR);

	Setting_GenGroupItem_Data *item_dev_name = data;
	SettingAboutUG *ad = item_dev_name->userdata;

	if (isSpaceStr(entry_str)) {
		if (!ad->empty_flag)
			ad->empty_flag = TRUE;
	} else {
		if (ad->empty_flag)
			ad->empty_flag = FALSE;
	}

	if (item_dev_name->enterKeyPressFlag == TRUE) {
		char *name_value = vconf_get_str(
				VCONFKEY_SETAPPL_DEVICE_NAME_STR);
		if (!ad->empty_flag
				&& safeStrCmp(name_value, entry_str_utf8) != 0) {
			if (0 != vconf_set_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR,
					entry_str_utf8)) {
				SETTING_TRACE_ERROR("Set vconf[%s] failed",
						VCONFKEY_SETAPPL_DEVICE_NAME_STR);
			}
		}
		if (EINA_TRUE
				== elm_object_focus_get(
						ad->item_dev_name->eo_check)) {
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_FALSE);
		}
		FREE(name_value);
	}
	setting_hide_input_pannel_cb(item_dev_name->eo_check);

	FREE(entry_str_utf8);
	elm_entry_select_none(obj);
	SETTING_TRACE_END;
}

/**
 * @brief device name entry max length reached callback.
 *
 * @param data application data
 * @param obj the entry evas object
 * @param event_info event type
 */
static void __setting_about_entry_max_len_reached(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	/*notify only when entry is being focused on. */
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	list_item->maxLengthReachFlag = true;

	SettingAboutUG *ad = list_item->userdata;
	if (list_item->maxLengthReachFlag
			&& list_item->enterKeyPressFlag == FALSE) {

		if (!ad->popup_showed_flag && NULL == ad->popup_space) {
			/*workaroud to fix popup blink issue. */
			ad->popup_showed_flag = TRUE;
			char strMax[BUF_SIZE] = { 0, };
			snprintf(strMax, BUF_SIZE,
					_("IDS_ST_POP_THE_MAXIMUM_NUMBERS_OF_CHARACTERS_FOR_YOUR_DEVICE_NAME_HPD_HAS_BEEN_EXCEEDED"),
					MAX_DEVICE_NAME_LEN);

			ad->popup_space = setting_create_popup(ad, ad->md.win_main,
					"IDS_ST_HEADER_ENTER_VALID_DEVICE_NAME_ABB",
					strMax, __setting_about_main_popup_cb,
					0, FALSE, FALSE, 1,
					"IDS_ST_BUTTON_OK_ABB");
		}
	}
}

/**
 * @brief device name entry input panel state event call back
 *	- input panel state event:ECORE_IMF_INPUT_PANEL_STATE_EVENT
 *
 * @param data application data
 * @param obj the entry evas object
 * @param event_info event type
 */
static void __setting_about_entry_input_panel_event_cb(void *data,
		Ecore_IMF_Context *ctx, int value)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(ctx == NULL, "obj parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		SETTING_TRACE("value == ECORE_IMF_INPUT_PANEL_STATE_SHOW");
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		SETTING_TRACE("value == ECORE_IMF_INPUT_PANEL_STATE_HIDE");

		if (ad->item_dev_name) {
			if (ad->empty_flag) {
				ad->idler_add_popup = ecore_idler_add(
						__setting_about_popup_show_delay,
						ad);
			}
		}
		/*if Setting is overlapped by other app,the entry will lost
		 * its focus, the input pannel will be hided and any popup
		 * should be removed to fix blinking issue.*/
		if (ad->pause_flag) {
			ad->idler_remove_popup = ecore_idler_add(
					__setting_about_popup_remove_delay, ad);
		}
		elm_object_focus_set(ad->md.navibar_main, EINA_FALSE);

	}
}

/**
 * @brief aboutUG vconf changed callback
 *
 * @param key the changed vconf key node.
 * @param data application data
 * @param event_info event type
 */
static void __setting_about_main_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_WIFI_STATE)) {
		SETTING_TRACE("status:%d", status);
		char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
		setting_about_main_get_wifi_mac_address_string(str,
				sizeof(str));
		if (ad->item_data_wifi) {
			G_FREE(ad->item_data_wifi->sub_desc);
			ad->item_data_wifi->sub_desc = (char *)g_strdup(str);
			elm_object_item_data_set(ad->item_data_wifi->item,
					ad->item_data_wifi);
			elm_genlist_item_update(ad->item_data_wifi->item);
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_BT_STATUS)) {
		SETTING_TRACE("status:%d", status);
		char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
		setting_about_main_get_bluetooth_address_string(str,
				sizeof(str));
		if (ad->item_data_bt) {
			G_FREE(ad->item_data_bt->sub_desc);
			ad->item_data_bt->sub_desc = (char *)g_strdup(str);
			elm_object_item_data_set(ad->item_data_bt->item,
					ad->item_data_bt);
			elm_genlist_item_update(ad->item_data_bt->item);
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DEVICE_NAME_STR)) {
		char *name_value = NULL;
		name_value = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
		if (NULL == name_value) {
			/*use the defaule name if get vconf failed*/
			name_value = (char *)g_strdup(DEVICE_NAME_DEFAULE);
		}
		SETTING_TRACE("name:%s", name_value);
		if (ad->item_dev_name && ad->item_dev_name->eo_check) {
			/*for genlist update */
			if (safeStrCmp(ad->item_dev_name->sub_desc, name_value)
					!= 0) {
				char *str_name = elm_entry_utf8_to_markup(
						name_value);
				elm_entry_entry_set(ad->item_dev_name->eo_check,
						str_name);
				elm_entry_cursor_end_set(
						ad->item_dev_name->eo_check);
				/*release first */
				G_FREE(ad->item_dev_name->sub_desc);
				ad->item_dev_name->sub_desc = (char *)g_strdup(
						str_name);
				FREE(str_name);
			}
		}
		if (ad->item_dev_name_main
				&& ad->item_dev_name_main->sub_desc) {
			if (safeStrCmp(ad->item_dev_name_main->sub_desc,
					name_value) != 0) {
				char *str_name = elm_entry_utf8_to_markup(
						name_value);
				char *tmp = ad->item_dev_name_main->sub_desc;
				ad->item_dev_name_main->sub_desc =
						(char *)g_strdup(str_name);
				elm_genlist_item_fields_update(
						ad->item_dev_name_main->item,
						"*",
						ELM_GENLIST_ITEM_FIELD_TEXT);
				FREE(str_name);
				G_FREE(tmp);

			}
		}
		FREE(name_value);
	} else {
		SETTING_TRACE_ERROR("vconf_name is error");
	}
}

/**
 * @brief UG destroy callback
 *
 * @param ug the UG which is needed to be destoried
 * @param priv application data
 */
static void __destroy_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)priv; /* ad is point to priv */
	/* restore the '<-' button on the navigate bar */
	ret_if(!priv);

	if (ug) {
		setting_ug_destroy(ug);
	}
	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_TRUE);
}

/**
 * @brief Lunch cetificates UG(setting-manage-certificates-efl)
 *
 * @param data application data
 */
static void __setting_about_main_certificates_clicked(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	ret_if(!cbs);

	cbs->layout_cb = setting_about_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = __destroy_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);

	if (NULL ==
		setting_ug_create(NULL, "setting-manage-certificates-efl", UG_MODE_FULLVIEW, NULL, cbs)
		)
		SETTING_TRACE_ERROR("NULL == ad->ug_loading");

	FREE(cbs);
}

/**
 * @brief Lunch UG setting-phone-efl
 *
 * @param data application context
 */
static void __setting_about_main_licence_launch(void *data)
{
	app_control_h svc;
	struct ug_cbs *cbs = NULL;
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;

	ret_if(app_control_create(&svc));

	app_control_add_extra_data(svc, "viewtype", "license");

	cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs) {
		app_control_destroy(svc);
		return;
	}

	cbs->layout_cb = setting_about_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = __destroy_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);

	if (NULL ==
		setting_ug_create(NULL, "setting-phone-efl", UG_MODE_FULLVIEW, svc, cbs)
		)
		SETTING_TRACE_ERROR("NULL == ad->ug_loading");

	app_control_destroy(svc);
	FREE(cbs);
}

/**
 * @brief expanlist sub-item selected callback
 *
 * @param data app context
 * @param obj evas object
 * @param event_info event type
 */
static void __setting_about_sub_list_sel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *)event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	int err;
	setting_set_string_slp_key(data_parentItem->int_slp_setting_binded,
			data_subItem->keyStr, &err);
	setting_retm_if(0 != err, "Set vconf error[%d]",
			data_parentItem->int_slp_setting_binded);

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);
}

/**
 * @brief expanlist sub-item radio checked callback
 *
 * @param data app context
 * @param obj evas object
 * @param event_info event type
 */

static void __setting_about_sub_list_rd_change(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	Elm_Object_Item *subItem = list_item->item;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subItem);

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subItem); /* subItem data */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	int err;
	setting_set_string_slp_key(data_parentItem->int_slp_setting_binded,
			data_subItem->keyStr, &err);

	ret_if(0 != err);
	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
	elm_radio_value_set(obj, data_subItem->chk_status);
}

/**
 * @brief expanlist state changed callback
 *
 * @param data app context
 * @param obj evas object
 * @param event_info event type
 */
void setting_about_main_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL, "event_info parameter is NULL");

	SettingAboutUG *ad = (SettingAboutUG *)data;
	Elm_Object_Item *parentItem = event_info; /* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem);
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd = elm_radio_add(scroller);
	elm_radio_value_set(rgd, -1);

	int err;
	char sel_num[SETTING_SIM_MSISDN_DIALING_NUMBER_LEN] = { 0, };
	setting_retm_if(NULL == data_parentItem, "data_parentItem is NULL");
	setting_get_string_slp_key(data_parentItem->int_slp_setting_binded,
			sel_num, &err);
	SETTING_TRACE("binded: %d, checked: %s, err: %d",
			data_parentItem->int_slp_setting_binded, sel_num, err);

	int i = 0;
	int sel_idx = -1;
	Setting_GenGroupItem_Data *item_data = NULL;

	/*for(; i < SETTING_ABOUT_MY_NUMBERS_LEN; i++) { */
	for (; i < ad->my_numbers.count; i++) {
		if (ad->my_numbers.list[i].num[0] == '\0')
			break;

		if (sel_idx == -1
				&& 0 == safeStrCmp(sel_num,
						ad->my_numbers.list[i].num)) {
			sel_idx = i;
		}
		item_data = setting_create_Gendial_exp_sub_field(scroller,
				&(ad->itc_1icon_1text_sub),
				__setting_about_sub_list_sel_cb, ad, parentItem,
				SWALLOW_Type_1RADIO, rgd, i,
				ad->my_numbers.list[i].num,
				__setting_about_sub_list_rd_change);
		if (item_data)
			item_data->userdata = ad;
		else
			SETTING_TRACE_ERROR("item_data is NULL");
	}

	elm_radio_value_set(rgd, sel_idx);
}

/**
 * @brief Do process when clicking '<-' button
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 */
static void setting_about_main_click_softkey_back_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)data;

	if(!ad || !!ad->empty_flag)
		return;

	/* imf must be hided before view is destroyed. */
	/* Following code is just to improve the hiding speed. If not add these
	 * code, the input pannel will also be hided with the view destroyed,
	 * but it works too slow. */
	if (ad->item_dev_name) {
		elm_object_focus_set(ad->item_dev_name->eo_check, EINA_FALSE);
		setting_hide_input_pannel_cb(ad->item_dev_name->eo_check);
	}
}

static Eina_Bool __setting_about_child_view_back_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_TRUE, "Data parameter is NULL");

	SettingAboutUG *ad = data;
	if (ad->empty_flag) {
		return EINA_FALSE;
	} else {
		if (ad->event_handler) {
			ecore_event_handler_del(ad->event_handler);
			ad->event_handler = NULL;
		}
		if (ad->name_update_idler) {
			ecore_idler_del(ad->name_update_idler);
			ad->name_update_idler = NULL;
		}
		return EINA_TRUE;
	}
}

static Eina_Bool __setting_about_name_view_key_down(void *data, int type,
		void *event)
{
	SETTING_TRACE_BEGIN;
	Evas_Event_Key_Down *ev = event;

	retv_if(!ev || !data, ECORE_CALLBACK_RENEW);

	SettingAboutUG *ad = data;
	if (!strcmp(ev->keyname, "XF86Back"))
		setting_about_naviframe_btn_cancel_cb(ad, NULL, NULL);

	return ECORE_CALLBACK_RENEW;
}

static Eina_Bool setting_about_name_focus_update_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_RENEW,
			"Data parameter is NULL");
	SettingAboutUG *ad = data;
	if (ad->item_dev_name && ad->item_dev_name->eo_check) {
		evas_object_show(ad->item_dev_name->eo_check);
		elm_object_focus_set(ad->item_dev_name->eo_check, EINA_TRUE);

		ad->name_update_idler = NULL;
		return ECORE_CALLBACK_CANCEL;
	} else {
		return ECORE_CALLBACK_RENEW;
	}
}

/**
 * Setting > About > Device name > (new view) here
 */
static void __setting_about_main_creat_name_view(void *data)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *list = NULL;
	Evas_Object *btn = NULL;
	Elm_Object_Item *navi_it = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;
	SettingAboutUG *ad = data;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	list = elm_genlist_add(ad->md.navibar_main);
	setting_retm_if(list == NULL, "Cannot set scroller object  as content of layout");
	elm_genlist_clear(list);	/* first to clear list */

	elm_genlist_mode_set(list, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(list, "realized", __gl_realized_cb,
			NULL);

	/* Device name */
	char *name_value = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
	char *pa_sub_desc = elm_entry_utf8_to_markup(name_value);
	FREE(name_value);

	ad->empty_flag = FALSE;
	if (NULL == pa_sub_desc || '\0' == pa_sub_desc[0])
		ad->empty_flag = TRUE;

	/* add genlist item here for the device name */
	item_data = setting_create_Gendial_field_editfield(
			list,
			&(itc_editfield),
			NULL,
			ad,
			SWALLOW_Type_LAYOUT_EDITFIELD,
			SETTING_ABOUT_DEVICE_NAME_STR,
			pa_sub_desc,
			__setting_about_entry_device_name_changed_cb,
			__setting_about_entry_focused,
			__setting_about_entry_unfocus_cb,
			NULL,
			__setting_about_entry_max_len_reached,
			ELM_INPUT_PANEL_LAYOUT_NORMAL,
			false,
			TRUE,
			MAX_DEVICE_NAME_LEN,
			0,
			NULL,
			NULL);

	if (!item_data)
		FREE(pa_sub_desc);

	setting_retm_if(!item_data, "calloc failed");

	item_data->swallow_type = SWALLOW_Type_LAYOUT_EDITFIELD;
	item_data->isSinglelineFlag = TRUE;
	item_data->x_callback_cb = __setting_about_entry_input_panel_event_cb;
	item_data->guide_text = (char *)g_strdup(EMPTY_LIMITATION_STR);
	item_data->focus_unallowed = get_tethering_status();
	item_data->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;

	item_data->limit_filter_data = calloc(1,
			sizeof(Elm_Entry_Filter_Limit_Size));
	if (item_data->limit_filter_data) {
		/*max byte len is 32 -> MAX_DEVICE_NAME_LEN */
		item_data->limit_filter_data->max_byte_count = MAX_DEVICE_NAME_LEN;
		item_data->win_main = ad->md.win_main;
	} else {
		SETTING_TRACE_ERROR("fail to calloc");
	}

	item_data->digits_filter_data = calloc(1,
			sizeof(Elm_Entry_Filter_Accept_Set));
	if (item_data->digits_filter_data)
		item_data->digits_filter_data->accepted = NULL;
	else
		SETTING_TRACE_ERROR("fail to calloc");

	item_data->input_panel_disable_flag = EINA_FALSE;

	ad->item_dev_name = item_data;
	if (ad->item_dev_name) {
		__BACK_POINTER_SET(ad->item_dev_name);
		elm_genlist_item_select_mode_set(ad->item_dev_name->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->item_dev_name is NULL");
	}

	FREE(pa_sub_desc);

	navi_it = setting_push_layout_navi_bar(_(DEVICE_NAME),
								NULL, NULL, NULL,
								NULL, NULL, NULL, ad, list, ad->md.navibar_main, NULL);
	elm_naviframe_item_pop_cb_set(navi_it,
			__setting_about_child_view_back_cb, ad);

	/* Title Cancel Button */
	btn = elm_button_add(ad->md.navibar_main);
	elm_object_style_set(btn, "naviframe/title_cancel");
	evas_object_smart_callback_add(btn, "clicked",
			setting_about_naviframe_btn_cancel_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_left_btn", btn);

	/* Title Done Button */
	btn = elm_button_add(ad->md.navibar_main);
	elm_object_style_set(btn, "naviframe/title_done");
	evas_object_smart_callback_add(btn, "clicked",
			setting_about_naviframe_btn_done_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_right_btn", btn);

	ad->event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
			__setting_about_name_view_key_down, ad);
	ad->name_update_idler = ecore_idler_add(
			setting_about_name_focus_update_cb, ad);
}

/**
 * @brief Do process when clicking on some genlist item
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 */
static void setting_about_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	retm_if(list_item == NULL, "return of elm_object_item_data_get is NULL");

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (!safeStrCmp("IDS_ST_HEADER_MANAGE_CERTIFICATES_ABB",
			list_item->keyStr)) {
		__setting_about_main_certificates_clicked(data);
	} else if (!safeStrCmp("IDS_ST_BODY_REGULATORY_INFORMATION_ABB",
			list_item->keyStr)) {
		__setting_about_popup_regulatory_info(data);
	} else if (!safeStrCmp("IDS_ST_MBODY_LEGAL_INFORMATION_ABB",
			list_item->keyStr)) {
		__setting_about_main_licence_launch(data);
	} else if (!safeStrCmp("IDS_ST_MBODY_SOFTWARE_UPDATE",
			list_item->keyStr)) {
#ifdef SUPPORT_FOTA
		SettingAboutUG *ad = (SettingAboutUG *)list_item->userdata;

		Evas_Object *back_btn = elm_object_item_part_content_get(
				ad->md.navibar_main_it, "prev_btn");

		if (back_btn) {
			SETTING_TRACE_DEBUG("Change focus to back_btn");
			elm_object_focus_set(back_btn, EINA_TRUE);
		}

		app_launcher("org.tizen.oma-dm", NULL, NULL);
#endif
	} else if (!safeStrCmp(SETTING_ABOUT_DEVICE_NAME_STR,
			list_item->keyStr)) {
		__setting_about_main_creat_name_view(data);
	}
}

/**
 * @brief callback function for genlist event "drag"
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 */
static void setting_about_main_gl_drag(void *data, Evas_Object *obj,
		void *event)
{
	ret_if(!data);
	retm_if(event == NULL, "Invalid argument: event info is NULL");
	SettingAboutUG *ad = (SettingAboutUG *)data;
	ad->drag_flag = TRUE;
}

/**
 * @brief callback function for EVAS_CALLBACK_MOUSE_UP
 *
 * @param data application context
 * @param e evas
 * @param obj evas object
 * @param event_info event type
 */
static void setting_about_main_gl_mouse_up(void *data, Evas *e,
		Evas_Object *obj, void *event)
{
	SettingAboutUG *ad = (SettingAboutUG *)data;
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event;
	Ecore_IMF_Context *imf_context = NULL;

	ret_if(!data);
	retm_if(event == NULL, "Invalid argument: event info is NULL");

	if (ad->drag_flag) {
		ad->drag_flag = FALSE;
		return;
	}

	if (ad->item_dev_name) {
		Elm_Object_Item *selected_item = elm_genlist_at_xy_item_get(
				ad->md.genlist, ev->output.x, ev->output.y, NULL);

		if (ad->item_dev_name->item == selected_item) {
			if (elm_object_focus_get(ad->item_dev_name->eo_check)) {
				imf_context =
						(Ecore_IMF_Context *)elm_entry_imf_context_get(
						ad->item_dev_name->eo_check);
				if (imf_context) {
					ecore_imf_context_input_panel_show(
							imf_context);
				}
				return;
			}

			if (get_tethering_status()) {
				if (ad->popup) {
					evas_object_del(ad->popup);
					ad->popup = NULL;
				}

				ad->popup = setting_create_popup(ad,
					ad->md.win_main,
					NULL,
					SETTING_ABOUT_MOBILE_AP_TURNED_OFF,
					__setting_about_popup_mobile_ap_turn_off_ask_resp_cb,
					0,
					FALSE, FALSE, 2,
					"IDS_ST_BODY_TURN_OFF",
					"IDS_ST_BUTTON_CANCEL_ABB2");

			} else {
				elm_object_focus_set(
						ad->item_dev_name->eo_check,
						EINA_TRUE);
				elm_entry_cursor_end_set(
						ad->item_dev_name->eo_check);

				imf_context =
						(Ecore_IMF_Context *)elm_entry_imf_context_get(
								ad->item_dev_name->eo_check);
				if (imf_context) {
					ecore_imf_context_input_panel_show(
							imf_context);
				}
			}
		} else {
			/*to make inputing words input */
			elm_object_focus_set(ad->item_dev_name->eo_check,
					EINA_FALSE);
		}
	}
}

/**
 * @brief callback run for timer task to update CPU, Battery info
 *
 * @param data application context
 *
 * @return FALSE for call it once and then destory the timer, TRUE for always
 *	call it when the timer is reached.
 */
static Eina_Bool setting_about_main_timer_update_cb(const void *data)
{
	retv_if(data == NULL, TRUE);
	SettingAboutUG *ad = (SettingAboutUG *)data;

	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
	if (ad->item_data_cpu) {
		float usr, sys;
		int ret = setting_about_main_stat_get_cpuinfo(&usr, &sys);
		if (ret == -ENOENT) {
			SETTING_TRACE_ERROR("call __stat_get_cpuinfo fail");
			snprintf(str, sizeof(str), "%s",
					_("IDS_ST_HEADER_UNAVAILABLE"));
		} else {
			snprintf(str, sizeof(str), "%.0f%s", usr + sys, "\%");
		}

		ad->item_data_cpu->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(ad->item_data_cpu->item,
				ad->item_data_cpu);
		elm_genlist_item_update(ad->item_data_cpu->item);
	}

	if (ad->item_data_battery) {
		setting_about_main_get_battery_string(str, sizeof(str));
		/*SETTING_TRACE("str:%s", str);*/
		ad->item_data_battery->sub_desc = (char *)g_strdup(str);

		elm_object_item_data_set(ad->item_data_battery->item,
				ad->item_data_battery);
		elm_genlist_item_update(ad->item_data_battery->item);
	}
	if (ad->need_update) {
		if (ad->item_data_my_phone_number) {
			G_FREE(ad->item_data_my_phone_number->sub_desc);
			ad->item_data_my_phone_number->sub_desc =
					(char *)g_strdup(
							SETTING_ABOUT_PUK_LOCKED);
			elm_object_item_data_set(
					ad->item_data_my_phone_number->item,
					ad->item_data_my_phone_number);
			elm_genlist_item_update(
					ad->item_data_my_phone_number->item);
			ad->need_update = FALSE;
		}
	}
	return TRUE;
}

/**
 * @brief create main view genlist items
 *
 * @param data application data
 *
 * @return FALSE for call it once and then destory the timer, TRUE for always
 *	call it when the timer is reached.
 */
int setting_about_main_generate_genlist(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAboutUG *ad = (SettingAboutUG *)data;

	bool is_emul_bin = isEmulBin();

	Evas_Object *scroller = ad->md.genlist;
	Setting_GenGroupItem_Data *item_data = NULL;
	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };
	telephony_handle_list_s tapi_handle_list;
	int i = 0;
	int tapi_handle_count = 0;
	int ret_value = 0;
	int ret_get_app_info = 0;
	app_info_h app_info = NULL;

	//ret_get_app_info = app_info_create("org.tizen.oma-dm", &app_info);

	/* [UI] Software update */
	//if (ret_get_app_info != APP_MANAGER_ERROR_NONE) {
		//SETTING_TRACE("No corresponding app_id for [%s]\n",
			//	"org.tizen.oma-dm");
	//} else {
		if (!is_emul_bin) {
			item_data = setting_create_Gendial_field_def(scroller,
					&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]),
					setting_about_main_mouse_up_Gendial_list_cb,
					ad, SWALLOW_Type_INVALID, NULL, NULL, 0,
					"IDS_ST_MBODY_SOFTWARE_UPDATE",
					NULL, NULL);
			if (item_data)
				item_data->userdata = ad;
			else
				SETTING_TRACE_ERROR("item_data is NULL");
		}
	//}

	/* [UI] Certificates */
	/* implementation is in progress. */
	item_data = setting_create_Gendial_field_def(scroller, &(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]),
			setting_about_main_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_INVALID, NULL, NULL, 0,
			"IDS_ST_HEADER_MANAGE_CERTIFICATES_ABB", NULL, NULL);
	if (!item_data)
		SETTING_TRACE_ERROR("item_data is NULL");

	/* [UI] Legal Information */
	setting_create_Gendial_field_def(scroller, &(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]),
			setting_about_main_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_INVALID, NULL, NULL, 0,
			"IDS_ST_MBODY_LEGAL_INFORMATION_ABB", NULL, NULL);

	/*////////////////////////////////////////////////////////////////// */
	(void)setting_create_Gendial_field_titleItem(scroller,
			&(ad->md.itc_table[GENDIAL_Type_expandable_proc]), "IDS_ST_BODY_DEVICE_INFORMATION",
			NULL);
	/*////////////////////////////////////////////////////////////////// */

	/* 1. Name */
	/* Device name */
	char *name_value = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
	char *pa_sub_desc = elm_entry_utf8_to_markup(name_value);
	FREE(name_value);

	ad->empty_flag = FALSE;
	if (NULL == pa_sub_desc || '\0' == pa_sub_desc[0])
		ad->empty_flag = TRUE;

	/* [UI] Name */
	ad->item_dev_name_main = item_data = setting_create_Gendial_field_def(
			scroller, &(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]),
			setting_about_main_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_INVALID, NULL,
			NULL, 0, SETTING_ABOUT_DEVICE_NAME_STR, pa_sub_desc,
			NULL);
	if (item_data)
		__BACK_POINTER_SET(ad->item_dev_name_main);
	else
		SETTING_TRACE_ERROR("item_data is NULL");

	FREE(pa_sub_desc);

	/* 2. Phone number */
	/* Initialize telephony handle */
	int telephony_init_ret = telephony_init(&tapi_handle_list);
	if (telephony_init_ret != TELEPHONY_ERROR_NONE)
		tapi_handle_count = 0;
	else
		tapi_handle_count = tapi_handle_list.count;

	SETTING_TRACE_DEBUG("tapi_handle_list.count:%d",
			tapi_handle_list.count);

	/* my numbers(SIM1 and SIM2) */
	for (i = 0; i < tapi_handle_count; i++) {
		memset(str, 0x00, sizeof(str));
		char *my_number = NULL;
		ret_value = telephony_sim_get_subscriber_number(
				tapi_handle_list.handle[i], &my_number);
		if (ret_value != TELEPHONY_ERROR_NONE) {
			SETTING_TRACE_ERROR(
					"telephony_sim_get_subscriber_number() failed!!! [%d]",
					ret_value);
			my_number = (char *)g_strdup(
					"IDS_ST_HEADER_UNAVAILABLE");
		} else if (isEmptyStr(my_number)) {
			SETTING_TRACE_DEBUG(
					"telephony_sim_get_subscriber_number() success, BUT return empty my number");
			my_number = (char *)g_strdup(
					"IDS_ST_HEADER_UNAVAILABLE");
		}
		/* [UI] Phone number */
		item_data = setting_create_Gendial_field_def(scroller,
				&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
				NULL, SWALLOW_Type_INVALID, NULL,
				NULL, 0, "IDS_ST_BODY_PHONE_NUMBER", my_number,
				NULL);
		if (item_data) {
			elm_object_item_disabled_set(item_data->item,
					EINA_TRUE);
			elm_genlist_item_select_mode_set(item_data->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
		FREE(my_number);

	}

	/* 3. [UI] Model number */
	memset(str, 0x00, sizeof(str));
	setting_about_main_get_phone_model_name(str, sizeof(str));
	item_data = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, "IDS_ST_BODY_MODEL_NUMBER", str, NULL);
	if (item_data) {
		elm_object_item_disabled_set(item_data->item, EINA_TRUE);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}
	ad->item_model = item_data;

	/* 4. [UI] Tizen version */
	memset(str, 0x00, sizeof(str));
	setting_about_main_get_sw_version(str, sizeof(str));
	item_data = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, "IDS_ST_MBODY_TIZEN_VERSION", str, NULL);
	if (item_data) {
		elm_object_item_disabled_set(item_data->item, EINA_TRUE);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}
	ad->item_version = item_data;

	/* 5. IMEI(IMEI 1 andd IMEI 2) */
	for (i = 0; i < tapi_handle_count; i++) {
		memset(str, 0x00, sizeof(str));
		/*setting_about_main_get_imei(str, sizeof(str),
		 * tapi_handle_list.handle[i]); */
		char *imei = NULL;
		ret_value = telephony_modem_get_imei(tapi_handle_list.handle[i],
				&imei);
		if (ret_value != TELEPHONY_ERROR_NONE) {
			SETTING_TRACE_ERROR(
					"telephony_modem_get_imei() failed!!! [%d]",
					ret_value);
			imei = (char *)g_strdup("IDS_ST_HEADER_UNAVAILABLE");
		}
		/* [UI] IMEI */
		item_data = setting_create_Gendial_field_def(scroller,
				&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
				NULL, SWALLOW_Type_INVALID, NULL,
				NULL, 0, SETTING_ABOUT_IMEI_STR, imei, NULL);
		if (item_data) {
			elm_object_item_disabled_set(item_data->item,
					EINA_TRUE);
			elm_genlist_item_select_mode_set(item_data->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
		FREE(imei);

	}
	ret_value = telephony_deinit(&tapi_handle_list);
	if (ret_value != TELEPHONY_ERROR_NONE)
		SETTING_TRACE_ERROR("Deinitialize failed!!!");

	/*ad->item_data_imei = item_data; */

	/* 6. [UI] Bluetooth address */
	if (!is_emul_bin) { /* requested by DI Kim due to BT BS on 11/26 */
		memset(str, 0x00, sizeof(str));
		setting_about_main_get_bluetooth_address_string(str,
				sizeof(str));
		ad->item_data_bt = setting_create_Gendial_field_def(scroller,
				&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
				NULL, SWALLOW_Type_INVALID, NULL,
				NULL, 0, "IDS_ST_MBODY_BLUETOOTH_ADDRESS", str,
				NULL); /*ad->md.itc_table[GENDIAL_Type_1text_1icon_2] */
		if (ad->item_data_bt) {
			elm_object_item_disabled_set(ad->item_data_bt->item,
					EINA_TRUE);
			elm_genlist_item_select_mode_set(ad->item_data_bt->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			__BACK_POINTER_SET(ad->item_data_bt);
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}

	/* 7. [UI] Wi-Fi MAC address */
	memset(str, 0x00, sizeof(str));
	setting_about_main_get_wifi_mac_address_string(str, sizeof(str));

	SETTING_TRACE_DEBUG("[about main] get_wifi_mac_address : %s", str);

	ad->item_data_wifi = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, "IDS_ST_BODY_WI_FI_MAC_ADDRESS", str, NULL);
	if (ad->item_data_wifi) {
		elm_object_item_disabled_set(ad->item_data_wifi->item,
				EINA_TRUE);
		elm_genlist_item_select_mode_set(ad->item_data_wifi->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		__BACK_POINTER_SET(ad->item_data_wifi);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 8. [UI] Serial number */
	memset(str, 0x00, sizeof(str));
	setting_about_main_get_sn(str, sizeof(str));
	item_data = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, SETTING_ABOUT_SN_STR, str, NULL);
	if (item_data) {
		elm_object_item_disabled_set(item_data->item, EINA_TRUE);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}
	ad->item_data_sn = item_data;

	/* 9. [UI] Battery power */
	memset(str, 0x00, sizeof(str));
	setting_about_main_get_battery_string(str, sizeof(str));
	item_data = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, "IDS_ST_BUTTON_BATTERY_POWER_ABB", str, NULL);
	if (item_data) {
		elm_object_item_disabled_set(item_data->item, EINA_TRUE);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		ad->item_data_battery = item_data;
		__BACK_POINTER_SET(ad->item_data_battery);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 10. [UI] CPU usage */
	memset(str, 0x00, sizeof(str));
	float usr, sys;
	int ret = setting_about_main_stat_get_cpuinfo(&usr, &sys);
	if (ret == -ENOENT) {
		SETTING_TRACE_ERROR("call __stat_get_cpuinfo fail");
		snprintf(str, sizeof(str), "%s",
				_("IDS_ST_HEADER_UNAVAILABLE"));
	} else {
		snprintf(str, sizeof(str), "%.0f%s", usr + sys, "\%");
	}
	ad->item_data_cpu = item_data = setting_create_Gendial_field_def(
			scroller, &(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, SETTING_ABOUT_CPU_USAGE_STR, str, NULL);
	if (ad->item_data_cpu) {
		elm_object_item_disabled_set(ad->item_data_cpu->item,
				EINA_TRUE);
		elm_genlist_item_select_mode_set(ad->item_data_cpu->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		__BACK_POINTER_SET(ad->item_data_cpu);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 12. Security status */
#if 0
	app_info_h app_info2 = NULL;
	ret_get_app_info = app_info_create("org.tizen.smack-update-service",
			&app_info2);
	if (ret_get_app_info == APP_MANAGER_ERROR_NONE) {
		char *security_status = NULL;
		security_status = vconf_get_str(
				VCONFKEY_SECURITY_SPD_INSTALLED_POLICY_VERSION);
		if (!security_status) {
			SETTING_TRACE_ERROR(
					"Failed to register into SPD server");
			security_status = (char *)g_strdup(
					"IDS_ST_HEADER_UNAVAILABLE");
		}
	}
#else
	char *security_status = (char *)g_strdup("IDS_ST_HEADER_UNAVAILABLE");
#endif
	item_data = setting_create_Gendial_field_def(scroller,
			&(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]), NULL,
			NULL, SWALLOW_Type_INVALID, NULL,
			NULL, 0, "IDS_ST_TMBODY_SECURITY_STATUS",
			security_status, NULL);
	if (item_data) {
		elm_object_item_disabled_set(item_data->item, EINA_TRUE);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}
	G_FREE(security_status);

	if (app_info)
		app_info_destroy(app_info);

	ad->update_timer = ecore_timer_add(1,
			(Ecore_Task_Cb)setting_about_main_timer_update_cb, ad);

	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief create aboutUG main view layout
 *
 * @param cb aboutUG data
 * @return 1 for success
 */
static int setting_about_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAboutUG *ad = (SettingAboutUG *)cb;

	view_init(&ad->md, _("IDS_ST_BODY_ABOUT_DEVICE"));

	ad->btn_done = setting_about_naviframe_btn_create(ad->md.navibar_main, _("IDS_SA_BUTTON_DONE_ABB"), setting_about_naviframe_btn_done_cb, ad);
	/*Title Text Right Button */
	ad->btn_cancel = setting_about_naviframe_btn_create(ad->md.navibar_main, _("IDS_ST_BUTTON_CANCEL_ABB"), setting_about_naviframe_btn_cancel_cb, ad);

	evas_object_smart_callback_add(ad->md.genlist, "drag", setting_about_main_gl_drag, ad);

	evas_object_event_callback_add(ad->md.genlist, EVAS_CALLBACK_MOUSE_UP, setting_about_main_gl_mouse_up, ad);

	setting_about_main_generate_genlist((void *)ad);

	setting_create_navi_bar_buttons(
			_("IDS_ST_BODY_ABOUT_DEVICE"),
	       _("IDS_ST_BUTTON_BACK"),
	       setting_about_main_click_softkey_back_cb,
	       &ad->md, ad->md.genlist, ad->md.navibar_main,
	       NULL);


	int ret = vconf_notify_key_changed(VCONFKEY_WIFI_STATE,
			__setting_about_main_vconf_change_cb, ad);
	if (ret != 0)
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");

	ret = vconf_notify_key_changed(VCONFKEY_BT_STATUS,
			__setting_about_main_vconf_change_cb, ad);
	if (ret != 0)
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_DEVICE_NAME_STR,
			__setting_about_main_vconf_change_cb, ad);
	if (ret != 0)
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");

	setting_view_about_main.is_create = 1;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief destroy aboutUG main view layout
 *
 * @param cb aboutUG data
 * @return 1 for success
 */
static int setting_about_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAboutUG *ad = (SettingAboutUG *)cb;

	//evas_object_smart_callback_del(ad->genlsit, "realized",
		//	__gl_realized_cb);

	int ret = vconf_ignore_key_changed(VCONFKEY_WIFI_STATE,
			__setting_about_main_vconf_change_cb);
	if (ret != 0)
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_DEVICE_NAME_STR,
			__setting_about_main_vconf_change_cb);
	if (ret != 0)
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");

	/*__setting_about_main_remove_noti(ad);*/

	ecore_timer_del(ad->update_timer);
	ad->update_timer = NULL;

	ecore_idler_del(ad->update_idler);
	ad->update_idler = NULL;

	ecore_idler_del(ad->idler_add_popup);
	ad->idler_add_popup = NULL;

	ecore_idler_del(ad->idler_remove_popup);
	ad->idler_remove_popup = NULL;

	ecore_idler_del(ad->idler_remove_space_popup);
	ad->idler_remove_space_popup = NULL;

	evas_object_del(ad->popup);
	ad->popup = NULL;

	evas_object_del(ad->popup_space);
	ad->popup_space = NULL;

	evas_object_del(ad->md.ly_main);
	ad->md.ly_main = NULL;

	ecore_event_handler_del(ad->event_handler);
	ad->event_handler = NULL;

	ecore_idler_del(ad->name_update_idler);
	ad->name_update_idler = NULL;

	setting_view_about_main.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief update aboutUG main view
 *
 * @param cb aboutUG data
 * @return 1 for success
 */
static int setting_about_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAboutUG *ad = (SettingAboutUG *)cb;

	if (ad->item_dev_name) {
		ad->is_dev_name_focus = elm_object_focus_get(
				ad->item_dev_name->eo_check);
		ad->cursor_pos = elm_entry_cursor_pos_get(
				ad->item_dev_name->eo_check);

		SETTING_TRACE("elm_object_focus_get(ad->item_dev_name->eo_check):%d",
				elm_object_focus_get(ad->item_dev_name->eo_check));

		SETTING_TRACE("ad->is_dev_name_focus:%d",
				ad->is_dev_name_focus);
		/*elm_object_focus_set(ad->item_dev_name->eo_check, EINA_TRUE);
		 Ecore_IMF_Context *imf_context =
		 (Ecore_IMF_Context *)elm_entry_imf_context_get(ad->item_dev_name->eo_check);
		 if (imf_context) {
		 ecore_imf_context_input_panel_show(imf_context);
		 }*/
	}

	if (ad->md.ly_main != NULL) {
		evas_object_show(ad->md.ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief hide aboutUG main view
 *
 * @param cb aboutUG data
 * @return 1 for success
 */
static int setting_about_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)cb;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	evas_object_hide(ad->md.ly_main);

	return SETTING_RETURN_SUCCESS;
}

