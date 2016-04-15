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

#include <setting-display-brightness.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

#define BRIGHTNESS_MAX 100
#define BRIGHTNESS_MIN 1
#define BRIGHTNESS_AUTO_MAX_LEVEL 5
#define BRIGHTNESS_AUTO_MIN_LEVEL -5

static int setting_display_brightness_create(void *cb);
static int setting_display_brightness_destroy(void *cb);
static int setting_display_brightness_update(void *cb);
static int setting_display_brightness_cleanup(void *cb);

static void setting_display_brightness_click_softkey_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void setting_display_birghtness_bright_slider_value_change_cb(void *data, Evas_Object *obj, void *event_info);

void setting_display_update_slider_icon(Setting_GenGroupItem_Data *item_data, int value);

static void setting_display_set_slider_value(void *data, Evas_Object *obj, double value);

setting_view setting_view_display_brightness = {
	.create = setting_display_brightness_create,
	.destroy = setting_display_brightness_destroy,
	.update = setting_display_brightness_update,
	.cleanup = setting_display_brightness_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
#define MAX_LOCAL_BUFSIZE 128
#define DBUS_PATH "/Org/Tizen/System/DeviceD/Process"
#define DBUS_SIGNAL_INTERFACE "org.tizen.system.deviced.Process"
#define DBUS_SIGNAL_NAME "ChangedSiop"

const char *iconPath[SETTING_DISPLAY_ICON_PATH] = {
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_00.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_01.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_02.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_03.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_04.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_05.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_06.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_07.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_08.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_09.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_10.png",
	SETTING_ICON_PATH_CFG"brightness_icon/quick_icon_brightness_11.png"
};


static void update_overheating_text(void *data)
{
	SettingDisplayUG *ad = (SettingDisplayUG *)data;
	int auto_brightnes = 0;

	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &auto_brightnes);

	if (ad->data_overheating && ad->data_br_sli) {
		if (ad->data_br_sli->slider_max < 100 && !auto_brightnes) {
			G_FREE(ad->data_overheating->keyStr);
			ad->data_overheating->keyStr = (char *)g_strdup(KeyStr_Brightness_Overheating);
		} else {
			G_FREE(ad->data_overheating->keyStr);
			ad->data_overheating->keyStr = NULL;
		}
		elm_object_item_data_set(ad->data_overheating->item, ad->data_overheating);
		elm_genlist_item_update(ad->data_overheating->item);
	}
}
/*------------------------------------------------------ */
/* for client - bus */
/*------------------------------------------------------ */
static DBusHandlerResult setting_brightness_dbus_signal_filter(DBusConnection *conn, DBusMessage *message, void *user_data)
{
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;
	DBusError error;
	int brightness_level;


	SETTING_TRACE_BEGIN;
	/*setting_main_appdata *ad = user_data; */
	setting_retvm_if(!user_data , 0, "!user_data ");
	dbus_error_init(&error);

	if (dbus_message_is_signal(message, DBUS_SIGNAL_INTERFACE, DBUS_SIGNAL_NAME)) {
		int value = 0;

		if (dbus_message_get_args(message, &error,
		                          DBUS_TYPE_INT32, &brightness_level,
		                          DBUS_TYPE_INVALID) == FALSE) {
			SETTING_TRACE_ERROR("Fail to get data : %s", error.message);
			dbus_error_free(&error);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}

		/* update max brightness level*/
		vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &value);
		if (!value && ad->data_br_sli) {
			ad->data_br_sli->slider_max = display_get_max_brightness();
			/*device_display_get_max_brightness(0, (int*)&ad->data_br_sli->slider_max); */
			SETTING_TRACE("brightness_level:%d", brightness_level);
			SETTING_TRACE("Brightness max value:%lf", ad->data_br_sli->slider_max);
			vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &value);
			if (value > ad->data_br_sli->slider_max)
				vconf_set_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, ad->data_br_sli->slider_max);

			update_overheating_text(ad);
		}
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}
static int setting_brightness_dbus_handler_fini(void *user_data)
{
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;
	DBusError error;
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0, };

	SETTING_TRACE_BEGIN;
	/*do safty checking first. */
	setting_retvm_if(!user_data , 0, "!user_data ");
	setting_retvm_if(!ad->bus, 0, "!ad->bus");

	memset(&error, 0, sizeof(DBusError));

	dbus_error_init(&error);
	dbus_connection_remove_filter(ad->bus, setting_brightness_dbus_signal_filter, ad);

	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);
	dbus_bus_remove_match(ad->bus, rule, &error);

	if (dbus_error_is_set(&error)) {
		SETTING_TRACE("Fail to rule unset: %s", error.message);
		dbus_error_free(&error);
	}

	dbus_connection_close(ad->bus);
	dbus_connection_unref(ad->bus);
	ad->bus = NULL;
	SETTING_TRACE("app signal finalized");

	return 0;
}

static int setting_brightness_dbus_handler_init(void *user_data)
{
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;
	DBusError error;
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0,};

	SETTING_TRACE_BEGIN;
	setting_retvm_if(!user_data , 0, "!user_data ");
	SETTING_TRACE("[TEST]slider_max: %lf", ad->data_br_sli->slider_max);

	if (ad->bus) {
		SETTING_TRACE("already get a bus, need release first.");
		setting_brightness_dbus_handler_fini(ad);
	}

	dbus_threads_init_default();

	memset(&error, 0, sizeof(DBusError));
	dbus_error_init(&error);

	ad->bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	if (!ad->bus) {
		SETTING_TRACE("Fail to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}

	dbus_connection_set_exit_on_disconnect(ad->bus, FALSE);
	dbus_connection_setup_with_g_main(ad->bus, NULL);
	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);

	dbus_bus_add_match(ad->bus, rule, &error);
	if (dbus_error_is_set(&error)) {
		SETTING_TRACE("Fail to rule set; %s", error.message);
		dbus_bus_remove_match(ad->bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(ad->bus);
		ad->bus = NULL;
		return -1;
	}

	if (dbus_connection_add_filter(ad->bus, setting_brightness_dbus_signal_filter, user_data, NULL) == FALSE) {
		dbus_bus_remove_match(ad->bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(ad->bus);
		ad->bus = NULL;
		return -1;
	}

	SETTING_TRACE("app signal initialized");

	return 0;
}

static void _brightness_register_event_cb(void *data)
{
	int ret = 0;
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SETTING_TRACE("[TEST]slider_max: %lf", ad->data_br_sli->slider_max);

	if (ad->is_event_registered == 0) {
		ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_BRIGHTNESS,
		                               __display_int_vconf_cb, ad);
		if (ret != 0)
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d",
			                    "VCONFKEY_SETAPPL_LCD_BRIGHTNESS", ret);

		ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,
		                               __display_int_vconf_cb, ad);
		if (ret != 0)
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d",
			                    "VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS", ret);

		ad->is_event_registered = 1;

		SETTING_TRACE_END;
	}
}

static void _brightness_deregister_event_cb(void *data)
{
	int ret = 0;
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");

	if (ad->is_event_registered == 1) {
		ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, __display_int_vconf_cb);
		if (ret != 0)
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d", "VCONFKEY_SETAPPL_LCD_BRIGHTNESS", ret);

		ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS, __display_int_vconf_cb);
		if (ret != 0)
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d", "VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS", ret);

		ad->is_event_registered = 0;
	}

	SETTING_TRACE_END;
}

static void
_brightness_slider_delayed_changed_cb(void *data,
                                      Evas_Object *obj,
                                      void *event_info)
{
	int value = 0;
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");

	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	value = ad->last_requested_level;

	if (value >= list_item->slider_min && value <= list_item->slider_max) {
		SETTING_TRACE("brightness is changed to %d", value);
		setting_display_set_slider_value(ad, obj, value);
	}
}

static void _brightness_overheat_check(void *data)
{
	int value = 0;
	int max_brightness = BRIGHTNESS_MAX;
	SettingDisplayUG *ad = data;
	int automatic_on = 0;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");

	value = ad->last_requested_level;

	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &automatic_on);

	if (automatic_on == 0) {
		max_brightness = display_get_max_brightness();

		if (value > max_brightness && max_brightness != BRIGHTNESS_MAX) {
			SETTING_TRACE("max brightness is limited");

			elm_slider_value_set(ad->data_br_sli->eo_check, (double)max_brightness);
			ad->last_requested_level = max_brightness;
			ad->data_br_sli->slider_max = max_brightness;

			SETTING_TRACE("[TEST]slider_max: %lf", ad->data_br_sli->slider_max);

			setting_display_set_slider_value(ad, ad->data_br_sli->eo_check, (double)max_brightness);
			update_overheating_text(ad);
		}
	}
}

void static _brightness_slider_mouse_down_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");

	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	SETTING_TRACE_END;
}

static void _brightness_slider_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");

	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	_brightness_overheat_check(ad);
	SETTING_TRACE_END;
}

const char  *setting_brightness_get_slider_icon(int level)
{
	int mapped_level = 0;

	if (level <= 1) {
		mapped_level = 0;
	} else if (level >= 100) {
		mapped_level = 11;
	} else if (level > 1 && level <= 9) {
		mapped_level = 1;
	} else {
		mapped_level = (level / 10);
	}

	SETTING_TRACE_DEBUG("mapped_level:%d", mapped_level);

	return iconPath[mapped_level];
}

void setting_display_update_slider_icon(Setting_GenGroupItem_Data *item_data, int value)
{
	Evas_Object *cur_icon = NULL;
	const char *new_file = NULL;

	SETTING_TRACE_BEGIN;

	cur_icon = elm_object_content_get(item_data->eo_check);
	new_file = setting_brightness_get_slider_icon(value);
	elm_image_file_set(cur_icon, new_file, NULL);
}

void __display_int_vconf_cb(keynode_t *key, void *data)
{
	SettingDisplayUG *ad = data;
	int status = 0;
	char *vconf_name = NULL;

	SETTING_TRACE_BEGIN;
	retm_if(!data, "Invalid argument: data is NULL");

	status = vconf_keynode_get_int(key);
	vconf_name = vconf_keynode_get_name(key);

	SETTING_TRACE("Enter %s(%s=%d)", __FUNCTION__, vconf_name, status);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_BRIGHTNESS)) {
		char strValue[BUF_SIZE] = {0};
		int value = 0;

		if (ad->data_br_sli) {
			elm_slider_value_set(ad->data_br_sli->eo_check, status);
			setting_display_update_slider_icon(ad->data_br_sli, status);
		}

		if (status <= 2) {
			value = (int)(2 * 1000);
		} else {
			value = (int)(status * 1000);
		}
		snprintf(strValue, BUF_SIZE, "%d", value);

	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT)) {
		setting_update_gl_item_chk_status(ad->data_br_auto, status);
		if (ad->data_br) {
			ad->data_br->sub_desc = (char *)g_strdup(get_brightness_mode_str());
			elm_object_item_data_set(ad->data_br->item, ad->data_br);
			elm_genlist_item_update(ad->data_br->item);
		}
		if (status) {
			int val = 50;

			SETTING_TRACE("off->on");

			/*need to update adjust text*/
			if (ad->data_adjust) {
				G_FREE(ad->data_adjust->keyStr);
				ad->data_adjust->keyStr = (char *)g_strdup(KeyStr_Brightness_Auto_Adjust);
				elm_object_item_data_set(ad->data_adjust->item, ad->data_adjust);
				elm_genlist_item_update(ad->data_adjust->item);
			}

			/*save current slider value at VCONFKEY_SETAPPL_LCD_BRIGHTNESS */
			/* And, slider value should be changed to VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,to use */
			vconf_get_int(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS, &val);
			if (ad->data_br_sli) {
				ad->data_br_sli->slider_min = BRIGHTNESS_MIN;
				ad->data_br_sli->slider_max = BRIGHTNESS_MAX;
				ad->data_br_sli->chk_status = val;
				elm_object_item_data_set(ad->data_br_sli->item, ad->data_br_sli);
				elm_genlist_item_update(ad->data_br_sli->item);
			}

		} else { /*on->off */
			int val = 50;
			int err;

			SETTING_TRACE("on->off");

			/*need to update adjust text*/
			if (ad->data_adjust) {
				G_FREE(ad->data_adjust->keyStr);
				ad->data_adjust->keyStr = (char *)g_strdup(KeyStr_Brightness_Adjust);
				elm_object_item_data_set(ad->data_adjust->item, ad->data_adjust);
				elm_genlist_item_update(ad->data_adjust->item);
			}

			/*save current slider value at VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,to backup */
			/*And, slider value should be changed to VCONFKEY_SETAPPL_LCD_BRIGHTNESS,to use */
			setting_get_int_slp_key(INT_SLP_SETTING_LCD_BRIGHTNESS, &val, &err);
			if (ad->data_br_sli) {
				ad->data_br_sli->slider_min = BRIGHTNESS_MIN;
				ad->data_br_sli->slider_max = display_get_max_brightness();
				/*device_display_get_max_brightness(0, (int*)&ad->data_br_sli->slider_max); */
				ad->data_br_sli->chk_status = val;
				elm_object_item_data_set(ad->data_br_sli->item, ad->data_br_sli);
				elm_genlist_item_update(ad->data_br_sli->item);
			}

			/*set the device side value */
			/*err =  device_display_set_brightness(0, val); */
			err = display_set_brightness_with_setting(val);
			if (err != DEVICE_ERROR_NONE) {
				SETTING_TRACE(" device_display_set_brightness : failed[ret=%d]", err);
				setting_create_popup(ad, ad->win_get, NULL, "IDS_CST_POP_FAILED",
				                                 NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
			}

		}
		update_overheating_text(ad);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS)) {
		int auto_matic_status = 0;

		vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &auto_matic_status);
		if (auto_matic_status && ad->data_br_sli)
			elm_slider_value_set(ad->data_br_sli->eo_check, status);
	}

	return;
}

const char *__display_brightness_get_vconf_need_save()
{
	int state = 0;

	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &state);

	if (state) {
		return VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS;
	} else {
		return VCONFKEY_SETAPPL_LCD_BRIGHTNESS;
	}
}

static double _step_size_calculate(Evas_Object *obj, double min, double max)
{
	double step = 0.0;
	int steps = 0;

	steps = max - min;
	if (steps) step = (1.0 / steps);

	return step;
}

static char *_setting_display_brightness_indicator_format(double val)
{
	char buf[16] = {0,};
	int value = (int)(val + 0.5);

	value = value / 10;
	value = value - 5;
	snprintf(buf, 15, "%d", value);

	return strdup(buf);
}

static void _indicator_free(char *str)
{
	free(str);
}

static Evas_Object *__setting_brightness_add_slider(void *data, Evas_Object *obj, const char *part)
{
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	Eina_List *items = NULL;

	SETTING_TRACE_BEGIN;

#if 1
	SETTING_TRACE(" ------------> slider content get [%s]",part);
#endif
	setting_retvm_if(!data || !obj, NULL, "!data || !obj");
	retv_if(!data, NULL);

	if (!safeStrCmp(part, "elm.swallow.content")) {
		int auto_value = SETTING_BRIGHTNESS_AUTOMATIC_ON;
		int err, ret;
		Evas_Object *slider = elm_slider_add(obj);	/*  "elm/slider/horizontal/default" */
		retv_if(slider == NULL, NULL);

		ret = setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &auto_value, &err);/*if get failed,to hold value SETTING_BRIGHTNESS_AUTOMATIC_ON */
		SETTING_TRACE("auto_value:%d", auto_value);

		elm_layout_signal_emit(item_data->eo_check, "elm,state,val,hide", "");

		if (0 != ret) {
			/*add error handle,due to different target env.. */
			SETTING_TRACE_ERROR("Failed to get value of [%s]", VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT);
		}

		if (auto_value) {
			double step;
			elm_slider_indicator_show_set(slider, EINA_TRUE);
			elm_slider_indicator_format_function_set(slider, _setting_display_brightness_indicator_format, _indicator_free);
			elm_object_style_set(slider, "center_point");
			elm_slider_indicator_format_set(slider, "%1.0f");

			step = _step_size_calculate(slider, BRIGHTNESS_AUTO_MIN_LEVEL, BRIGHTNESS_AUTO_MAX_LEVEL);
			elm_slider_step_set(slider, step);
		}

		elm_slider_min_max_set(slider, BRIGHTNESS_MIN, BRIGHTNESS_MAX);
		evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, 0.5);

		if (item_data->chk_change_cb)
			evas_object_smart_callback_add(slider, "changed", item_data->chk_change_cb, item_data);

		if (item_data->stop_change_cb)
			evas_object_smart_callback_add(slider, "slider,drag,stop", item_data->stop_change_cb, item_data);

		if (item_data->start_change_cb)
			evas_object_smart_callback_add(slider, "slider,drag,start", item_data->start_change_cb, item_data);

		evas_object_event_callback_add(slider, EVAS_CALLBACK_MOUSE_DOWN, _brightness_slider_mouse_down_cb, item_data);
		evas_object_event_callback_add(slider, EVAS_CALLBACK_MOUSE_UP, _brightness_slider_mouse_up_cb, item_data);
		evas_object_smart_callback_add(slider, "delay,changed", _brightness_slider_delayed_changed_cb, item_data);

		Evas_Object *icon1 = elm_icon_add(slider);
		elm_image_file_set(icon1, item_data->l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(icon1, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_content_set(slider, icon1);

		elm_slider_value_set(slider, item_data->chk_status);
		evas_object_pass_events_set(slider, 1);
		evas_object_propagate_events_set(slider, 0);
		item_data->eo_check = slider;

		if (item_data->item) {
			/* convey highlight to its content */
			items = eina_list_append(items, slider);
			elm_object_item_access_order_set(item_data->item, items);
		}

		return slider;
	}

	return NULL;
}

void construct_brightness(void *data, Evas_Object *genlist)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	const char *left_icon = NULL;
	int auto_value = SETTING_BRIGHTNESS_AUTOMATIC_ON;
	int err;
	int ret = -1;
	int value = SETTING_BRIGHTNESS_LEVEL5;
	char *temp = NULL;

	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	//setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(ad->itc_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE, &(ad->itc_1icon));

	ad->itc_1icon.func.content_get = __setting_brightness_add_slider;
	ad->last_requested_level = -1;

	ret = setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &auto_value, &err);/*if get failed,to hold value SETTING_BRIGHTNESS_AUTOMATIC_ON */
	SETTING_TRACE("auto_value:%d", auto_value);
	if (0 != ret) {
		/*add error handle,due to different target env.. */
		SETTING_TRACE_ERROR("Failed to get value of [%s]", VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT);
	}

	/* check manual */
	ret = vconf_get_int(__display_brightness_get_vconf_need_save(), &value);
	SETTING_TRACE("******* value = %d ", value);
	if (0 != ret) {
		/*add error handle,due to different target env.. */
		SETTING_TRACE_ERROR("Failed to get value of [%s]", __display_brightness_get_vconf_need_save());
	}

	left_icon = setting_brightness_get_slider_icon(value);

	/* [UI] Slider control for Bightness */
	ad->data_br_sli =
	    setting_create_Gendial_field_def(genlist, &(ad->itc_1icon), NULL,
	                                     NULL, SWALLOW_Type_LAYOUT_SLIDER,
	                                     (char*)left_icon,
	                                     NULL, value, "IDS_ST_BODY_BRIGHTNESS_M_POWER_SAVING",
	                                     NULL,
	                                     setting_display_birghtness_bright_slider_value_change_cb);

	if (ad->data_br_sli) {
		ad->data_br_sli->win_main = ad->win_main_layout;
		ad->data_br_sli->evas = ad->evas;
		if (auto_value) {
			ad->data_br_sli->isIndicatorVisible = 1;
			ad->data_br_sli->slider_min = BRIGHTNESS_MIN;
			ad->data_br_sli->slider_max = BRIGHTNESS_MAX;
		} else {
			ad->data_br_sli->slider_min = BRIGHTNESS_MIN;
			ad->data_br_sli->slider_max = display_get_max_brightness();
			ad->data_br_sli->isIndicatorVisible = 0;
		}
		ad->data_br_sli->userdata = ad;

		__BACK_POINTER_SET(ad->data_br_sli);
	} else {
		SETTING_TRACE_ERROR("ad->data_br_sli is NULL");
	}

	if (auto_value) {
		temp = (char *)g_strdup(KeyStr_Brightness_Auto_Adjust);
	} else {
		temp = (char *)g_strdup(KeyStr_Brightness_Adjust);
	}

	if (ad->data_adjust) {
		ad->data_adjust->userdata = ad;
		__BACK_POINTER_SET(ad->data_adjust);
	} else {
		SETTING_TRACE_ERROR("ad->data_adjust is NULL");
	}
	G_FREE(temp);

	_brightness_register_event_cb(ad);
	setting_brightness_dbus_handler_init(ad);
}

void destruct_brightness(void *data)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	_brightness_deregister_event_cb(ad);
	setting_brightness_dbus_handler_fini(ad);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (ad->set_idler) {
		ecore_idler_del(ad->set_idler);
		ad->set_idler = NULL;
	}
}

static int setting_display_brightness_create(void *cb)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) cb;
	Evas_Object *genlist = NULL;

	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	genlist = elm_genlist_add(ad->win_main_layout);

	retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "Cannot set scroller object as contento of layout");

	elm_genlist_clear(genlist);	/* first to clear list */

	/* add basic layout */
	if (&setting_view_display_brightness == ad->view_to_load) {
		ad->ly_main =
		    setting_create_layout_navi_bar(ad->win_main_layout,
		                                   ad->win_get,
		                                   "IDS_ST_BODY_BRIGHTNESS_M_POWER_SAVING",
		                                   _("IDS_ST_BUTTON_BACK"),
		                                   setting_display_brightness_click_softkey_cancel_cb,
		                                   ad,
		                                   genlist,
		                                   &ad->navi_bar, NULL);
	} else {
		setting_push_layout_navi_bar("IDS_ST_BODY_BRIGHTNESS_M_POWER_SAVING",
		                             _("IDS_ST_BUTTON_BACK"), NULL, NULL,
		                             setting_display_brightness_click_softkey_cancel_cb,
		                             NULL,
		                             NULL, ad, genlist, ad->navi_bar, NULL);
	}

	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, ad);

	construct_brightness(ad, genlist);

	/*key notify for refreshing */
	if (&setting_view_display_brightness == ad->view_to_load)
		vconf_notify_key_changed(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, __display_int_vconf_cb, ad);

	setting_view_display_brightness.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_brightness_destroy(void *cb)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_display_brightness.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	destruct_brightness(cb);

	if (&setting_view_display_brightness == ad->view_to_load)
		vconf_ignore_key_changed(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, __display_int_vconf_cb);

	if (&setting_view_display_brightness == ad->view_to_load) {
		if (ad->ly_main != NULL) {
			evas_object_del(ad->ly_main);
			ad->ly_main = NULL;
		}
		setting_view_display_brightness.is_create = 0;
	} else {
		setting_view_display_brightness.is_create = 0;
		elm_naviframe_item_pop(ad->navi_bar);
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_brightness_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_brightness_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_display_brightness_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void
setting_display_brightness_click_softkey_cancel_cb(void *data,
                                                   Evas_Object *obj,
                                                   void *event_info)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	if (&setting_view_display_brightness == ad->view_to_load) {
		/* Send destroy request */
		ug_destroy_me(ad->ug);
		return;
	}

	setting_view_change(&setting_view_display_brightness,
	                    &setting_view_display_main, ad);
}

/**
 * [UI] 'Automatic' toggle
 */
static void setting_display_set_slider_value(void *data, Evas_Object *obj, double value)
{
	SettingDisplayUG *ad = data;
	const char *vconf_str = NULL;

	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");

	vconf_str = __display_brightness_get_vconf_need_save();
	if (!safeStrCmp(vconf_str, VCONFKEY_SETAPPL_LCD_BRIGHTNESS)) {
		int ret, err;
		ret = display_set_brightness_with_setting(value);
		if (ret == DEVICE_ERROR_NONE) {
			ad->data_br_sli->chk_status = (int)value;
			SETTING_TRACE("value:%f", value);
		} else {
			/*rollback.. */
			int tmp = SETTING_BRIGHTNESS_LEVEL5;
			SETTING_TRACE(" device_display_set_brightness : failed[ret=%d]", ret);
			setting_get_int_slp_key(INT_SLP_SETTING_LCD_BRIGHTNESS, &tmp, &err);
			vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &tmp);
			elm_slider_value_set(obj, tmp);
			setting_create_popup(ad, ad->win_get, NULL, "IDS_CST_POP_FAILED",
			                                 NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		}
	} else {
		int ret = vconf_set_int(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS, value);
		/*add error handle.. */
		if (0 != ret) {
			setting_create_popup(ad, ad->win_get, NULL, "IDS_CST_POP_FAILED",
			                                 NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		}
	}
}

static void
setting_display_birghtness_bright_slider_value_change_cb(void *data,
                                                         Evas_Object *obj,
                                                         void *event_info)
{
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	int value = 0;
	double time_current = 0.0;
	static double time_before = 0.0;
	double val = 0.0;

	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	ad = list_item->userdata;

	time_current = ecore_loop_time_get();

	val = elm_slider_value_get(obj);

	SETTING_TRACE("val:%f", val);

	value = (int)(val + 0.5);

	SETTING_TRACE("value:%d, list_item->slider_max:%lf", value, list_item->slider_max);

	if (value != ad->last_requested_level && value >= list_item->slider_min &&
		  value <= list_item->slider_max) {
				ad->last_requested_level = value;
				SETTING_TRACE("value:%d", value);

				if (time_current - time_before >= 0.1) {
					setting_display_set_slider_value(ad, obj, value);
					setting_display_update_slider_icon(ad->data_br_sli, value);
					time_before = time_current;
				}
	}
}
