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


#define SUPPORT_DISABLE_IN_AUTO_MODE 0
#define DISABLE_LOW_BATERY_CHANGE 0 /*disable user to set the brightness when the Mobile is low battery. */

#define Left_Bright_Icon IMG_BRIGTHNESS_LEFT
#define Right_Bright_Icon IMG_BRIGHTNESS_RIGHT
#define SUPPORT_DISPLAY_HOMESCREEN_IMAGE 0

#define AUTO_DISPLAY_ADJUSTMENT_STR _("Auto Display Adjustment")
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
#define LOW_BATTERY_NOTIFY "Battery is too low, you cannot change brightness"
#define NORMAL_BATTERY_NOTIFY "Battery becomes normal, now you can change brightness.."

#define MAX_CONTENT_LEN 512
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
	SETTING_TRACE_BEGIN;
	DBusError error;
	dbus_error_init(&error);

	/*setting_main_appdata *ad = user_data; */
	setting_retvm_if(!user_data , 0, "!user_data ");
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;

	int brightness_level;

	if (dbus_message_is_signal(message, DBUS_SIGNAL_INTERFACE, DBUS_SIGNAL_NAME)) {
		if (dbus_message_get_args(message, &error,
		                          DBUS_TYPE_INT32, &brightness_level,
		                          DBUS_TYPE_INVALID) == FALSE) {
			SETTING_TRACE_ERROR("Fail to get data : %s", error.message);
			dbus_error_free(&error);
			return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
		}
		/* update max brightness level*/
		int value = 0;
		vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &value);
		if (!value && ad->data_br_sli) {
			ad->data_br_sli->slider_max = display_get_max_brightness();
			/*device_display_get_max_brightness(0, (int*)&ad->data_br_sli->slider_max); */
			SETTING_TRACE("brightness_level:%d", brightness_level);
			SETTING_TRACE("Brightness max value:%lf", ad->data_br_sli->slider_max);
			vconf_get_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, &value);
			if (value > ad->data_br_sli->slider_max) {
				vconf_set_int(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, ad->data_br_sli->slider_max);
			}
			update_overheating_text(ad);
		}
	}

	return DBUS_HANDLER_RESULT_HANDLED;
}
static int setting_brightness_dbus_handler_fini(void *user_data)
{
	SETTING_TRACE_BEGIN;
	/*do safty checking first. */
	setting_retvm_if(!user_data , 0, "!user_data ");
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;
	setting_retvm_if(!ad->bus, 0, "!ad->bus");

	DBusError error;
	memset(&error, 0, sizeof(DBusError));
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0, };

	dbus_error_init(&error);

	/* why?? */
	/*ad->bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error); */

	dbus_connection_remove_filter(ad->bus, setting_brightness_dbus_signal_filter, ad);


	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);
	dbus_bus_remove_match(ad->bus, rule, &error);

	if (dbus_error_is_set(&error)) {
		SETTING_TRACE("Fail to rule unset: %s", error.message);
		dbus_error_free(&error);
		/*return -1; */
	}

	dbus_connection_close(ad->bus);
	dbus_connection_unref(ad->bus);
	ad->bus = NULL;
	SETTING_TRACE("app signal finalized");
	return 0;
}

static int setting_brightness_dbus_handler_init(void *user_data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(!user_data , 0, "!user_data ");
	SettingDisplayUG *ad = (SettingDisplayUG *)user_data;

	SETTING_TRACE("[TEST]slider_max: %lf", ad->data_br_sli->slider_max);

	if (ad->bus) {
		SETTING_TRACE("already get a bus, need release first.");
		setting_brightness_dbus_handler_fini(ad);
		/*		return -1; */
	}
	dbus_threads_init_default();
	DBusError error;
	memset(&error, 0, sizeof(DBusError));
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0,};
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
	SETTING_TRACE_BEGIN;
	int ret = 0;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE("[TEST]slider_max: %lf", ad->data_br_sli->slider_max);

	if (ad->is_event_registered == 0) {
		ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_BRIGHTNESS,
		                               __display_int_vconf_cb, ad);
		if (ret != 0) {
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d",
			                    "VCONFKEY_SETAPPL_LCD_BRIGHTNESS", ret);
		}
		ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,
		                               __display_int_vconf_cb, ad);
		if (ret != 0) {
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d",
			                    "VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS", ret);
		}
		ad->is_event_registered = 1;
		SETTING_TRACE_END;
	}
}

static void _brightness_deregister_event_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	if (ad->is_event_registered == 1) {
		ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_BRIGHTNESS, __display_int_vconf_cb);
		if (ret != 0) {
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d", "VCONFKEY_SETAPPL_LCD_BRIGHTNESS", ret);
		}
		ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS, __display_int_vconf_cb);
		if (ret != 0) {
			SETTING_TRACE_ERROR("failed to register a cb key:%s err:%d", "VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS", ret);
		}
		ad->is_event_registered = 0;
	}
	SETTING_TRACE_END;
}
static void
_brightness_slider_delayed_changed_cb(void *data,
                                      Evas_Object *obj,
                                      void *event_info)
{
	SETTING_TRACE_BEGIN;
	int value = 0;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	value = ad->last_requested_level;

	if (value >= list_item->slider_min && value <= list_item->slider_max) {
		SETTING_TRACE("brightness is changed to %d", value);
		setting_display_set_slider_value(ad, obj, value);
	}

	/*_brightness_register_event_cb(ad); */
}
static void _brightness_overheat_check(void *data)
{
	SETTING_TRACE_BEGIN;
	int value = 0;
	int max_brightness = BRIGHTNESS_MAX;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = data;

	value = ad->last_requested_level;
	int automatic_on = 0;
	vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &automatic_on);
	if (automatic_on == 0) {
		max_brightness = display_get_max_brightness();
		/*device_display_get_max_brightness(0, (int*)&max_brightness); */
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
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	/*_brightness_deregister_event_cb(ad); */
	SETTING_TRACE_END;
}

static void _brightness_slider_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, , "Data parameter is NULL");
	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	setting_retm_if(ad == NULL, , "ad parameter is NULL");

	_brightness_overheat_check(ad);
	SETTING_TRACE_END;
}


#if DISABLE_LOW_BATERY_CHANGE
static void
__low_battery_on_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	evas_object_del(ad->popup);
	ad->popup = NULL;
}

static Eina_Bool __disable_on_idle(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	ad->popup = setting_create_popup(ad, ad->win_get, NULL, LOW_BATTERY_NOTIFY,
	                                 __low_battery_on_resp_cb, POPUP_INTERVAL, FALSE, FALSE, 0);
	if (ad->data_br_sli && ad->data_br_sli->item)
		setting_disable_genlist_item(ad->data_br_sli->item);
	if (ad->data_br_auto && ad->data_br_auto->item)
		setting_disable_genlist_item(ad->data_br_auto->item);
	return FALSE;
}

/* low battery */
static void setting_display_brightness_lowbat_cb(keynode_t *key, void *data)
{
	/*	SETTING_TRACE_BEGIN; */
	ret_if(data == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	SETTING_TRACE_BEGIN;

	char *vconf_key = vconf_keynode_get_name(key);
	SETTING_TRACE("the value of [ %s ] just changed", vconf_key);
	ad->low_bat = vconf_keynode_get_int(key);
	if (ad->low_bat <= VCONFKEY_SYSMAN_BAT_CRITICAL_LOW) {
		SETTING_TRACE("Battery becomes ctitical low !");
		/*popup was showed by system FW */
		/* disabled */
		if (ad->data_br_sli) setting_disable_genlist_item(ad->data_br_sli->item);
		if (ad->data_br_auto) setting_disable_genlist_item(ad->data_br_auto->item);
	} else {
		SETTING_TRACE("Battery becomes normal!");
		if (ad->data_br_sli) {
			int automatic_val, err;
			setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &automatic_val, &err);
			if (ad->data_br_auto) setting_enable_genlist_item(ad->data_br_auto->item);
			if (automatic_val == SETTING_BRIGHTNESS_AUTOMATIC_OFF) { /* not automatic */
				if (ad->data_br_sli) setting_enable_genlist_item(ad->data_br_sli->item);
			}
		}
	}
}
#endif
#if 0
/*
auto brightness level:-5 ~ 5
convet this auto level to auto brightness value 1~ 100
and then store it into vconf for kernel usage
*/
static int __display_auto_brightness_convert_level_to_value(int level)
{
	int value = 0;
	if (0 == level) {
		value = 50;
	} else if (level >= -5 && level < 0) {
		value = (int)(10 * (5 + level) + 0.5);
	} else if (level > 0 && level <= 5) {
		value = (int)(50 + 10 * level + 0.5);
	}
	if (value > BRIGHTNESS_MAX) {
		value = BRIGHTNESS_MAX;
	}
	if (value < BRIGHTNESS_MIN) {
		value = BRIGHTNESS_MIN;
	}
	return value;
}

/*
auto brightness value:1~ 100
convet this value to auto brightness level -5 ~ 5
and then show it to user
*/
static int __display_auto_brightness_convert_value_to_level(int value)
{
	int level = 0;
	if (value == 50) {
		level = 0;
	} else if (value >= 1 && value < 50) {
		level = -(int)((50 - value) * 0.1 + 0.5);
	} else if (value > 50 && value <= 100) {
		level = (int)((value - 50) * 0.1 + 0.5);
	}
	if (level > BRIGHTNESS_AUTO_MAX_LEVEL) {
		level = BRIGHTNESS_AUTO_MAX_LEVEL;
	}
	if (level < BRIGHTNESS_AUTO_MIN_LEVEL) {
		level = BRIGHTNESS_AUTO_MIN_LEVEL;
	}
	return level;
}
#endif

char  *setting_brightness_get_slider_icon(int level)
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
	SETTING_TRACE_BEGIN;
	Evas_Object *cur_icon = elm_object_content_get(item_data->eo_check);
	char *new_file = setting_brightness_get_slider_icon(value);
	elm_image_file_set(cur_icon, new_file, NULL);
}

void __display_int_vconf_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *ad = data;
	retm_if(!data, "Invalid argument: data is NULL");

	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("Enter %s(%s=%d)", __FUNCTION__, vconf_name, status);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_BRIGHTNESS)) {
		if (ad->data_br_sli) {
			elm_slider_value_set(ad->data_br_sli->eo_check, status);
			setting_display_update_slider_icon(ad->data_br_sli, status);
		}
		char strValue[BUF_SIZE] = {0};
		int value = 0;
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
		if (status) { /*off->on */
			SETTING_TRACE("off->on");

			/*need to update adjust text*/
			if (ad->data_adjust) {
				G_FREE(ad->data_adjust->keyStr);
				ad->data_adjust->keyStr = (char *)g_strdup(KeyStr_Brightness_Auto_Adjust);
				elm_object_item_data_set(ad->data_adjust->item, ad->data_adjust);
				elm_genlist_item_update(ad->data_adjust->item);

			}

#if SUPPORT_CENTER_POINT
			/*need to show central line when automatic is on.*/
			if (ad->data_br_sli && ad->data_br_sli->eo_check) {
				/*elm_object_signal_emit(ad->data_br_sli->eo_check,"elm,state,rect,visible","elm"); */
				elm_layout_signal_emit(ad->data_br_sli->eo_check, "slider,center,point,show", "");
			} else {
				SETTING_TRACE_ERROR("ad->data_br_sli->eo_check is NULL, failed to send visible signal")
			}
#endif

#if SUPPORT_DISABLE_IN_AUTO_MODE
			if (ad->data_br_sli)
				setting_disable_genlist_item(ad->data_br_sli->item);
#else
			/*save current slider value at VCONFKEY_SETAPPL_LCD_BRIGHTNESS */
			/* And, slider value should be changed to VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,to use */
			int val = 50;
			vconf_get_int(VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS, &val);
			if (ad->data_br_sli) {
				ad->data_br_sli->slider_min = BRIGHTNESS_MIN;
				ad->data_br_sli->slider_max = BRIGHTNESS_MAX;
				ad->data_br_sli->chk_status = val;
				elm_object_item_data_set(ad->data_br_sli->item, ad->data_br_sli);
				elm_genlist_item_update(ad->data_br_sli->item);
			}
#endif

		} else { /*on->off */
			SETTING_TRACE("on->off");

			/*need to update adjust text*/
			if (ad->data_adjust) {
				G_FREE(ad->data_adjust->keyStr);
				ad->data_adjust->keyStr = (char *)g_strdup(KeyStr_Brightness_Adjust);
				elm_object_item_data_set(ad->data_adjust->item, ad->data_adjust);
				elm_genlist_item_update(ad->data_adjust->item);
			}

#if SUPPORT_CENTER_POINT
			/*need to hide central line when automatic is off*/
			if (ad->data_br_sli && ad->data_br_sli->eo_check) {
				/*elm_object_signal_emit(ad->data_br_sli->eo_check,"elm,state,rect,hidden","elm"); */
				elm_layout_signal_emit(ad->data_br_sli->eo_check, "slider,center,point,hide", "");
			} else {
				SETTING_TRACE_ERROR("ad->data_br_sli->eo_check is NULL, failed to send hidden signal")
			}
#endif

#if SUPPORT_DISABLE_IN_AUTO_MODE
			if (ad->data_br_sli)
				setting_enable_genlist_item(ad->data_br_sli->item);
#else
			/*save current slider value at VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS,to backup */
			/*And, slider value should be changed to VCONFKEY_SETAPPL_LCD_BRIGHTNESS,to use */
			int val = 50;
			int err;
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
#endif

		}
		update_overheating_text(ad);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS)) {
		int auto_matic_status = 0;
		vconf_get_int(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, &auto_matic_status);
		if (auto_matic_status && ad->data_br_sli) {
			elm_slider_value_set(ad->data_br_sli->eo_check, status);
		}
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
	SETTING_TRACE_BEGIN;
	setting_retvm_if(!data || !obj, NULL, "!data || !obj");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	retv_if(!data, NULL);
	Eina_List *items = NULL;
	if (!safeStrCmp(part, "elm.icon")) {
		Evas_Object *slider = elm_slider_add(obj);	/*  "elm/slider/horizontal/default" */
		retv_if(slider == NULL, NULL);

		int auto_value = SETTING_BRIGHTNESS_AUTOMATIC_ON;
		int err;
		int ret = setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &auto_value, &err);/*if get failed,to hold value SETTING_BRIGHTNESS_AUTOMATIC_ON */
		elm_layout_signal_emit(item_data->eo_check, "elm,state,val,hide", "");
		SETTING_TRACE("auto_value:%d", auto_value);
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
		/*elm_object_style_set(slider,"tap_to_drag"); */
		evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, 0.0);
		evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, 0.5);


		if (item_data->chk_change_cb) {
			evas_object_smart_callback_add(slider, "changed", item_data->chk_change_cb, item_data);
		}
		if (item_data->stop_change_cb) {
			evas_object_smart_callback_add(slider, "slider,drag,stop", item_data->stop_change_cb, item_data);
		}
		if (item_data->start_change_cb) {
			evas_object_smart_callback_add(slider, "slider,drag,start", item_data->start_change_cb, item_data);
		}
		evas_object_event_callback_add(slider, EVAS_CALLBACK_MOUSE_DOWN, _brightness_slider_mouse_down_cb, item_data);
		evas_object_event_callback_add(slider, EVAS_CALLBACK_MOUSE_UP, _brightness_slider_mouse_up_cb, item_data);
		evas_object_smart_callback_add(slider, "delay,changed",
		                               _brightness_slider_delayed_changed_cb, item_data);
#if 0
		Evas_Object *icon1 = elm_icon_add(slider);
		elm_image_file_set(icon1, item_data->l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(icon1, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

		Evas_Object *icon2 = elm_icon_add(slider);
		elm_image_file_set(icon2, item_data->r_swallow_path, NULL);
		evas_object_size_hint_aspect_set(icon2, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

		elm_object_content_set(slider, icon1);
		elm_object_part_content_set(slider, "end", icon2);
#else
		Evas_Object *icon1 = elm_icon_add(slider);
		elm_image_file_set(icon1, item_data->l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(icon1, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_content_set(slider, icon1);
#endif
		elm_slider_value_set(slider, item_data->chk_status);
		evas_object_pass_events_set(slider, 1);
		evas_object_propagate_events_set(slider, 0);
		item_data->eo_check = slider;


#if SUPPORT_CENTER_POINT
		if (auto_value) {
			/*elm_object_signal_emit(item_data->eo_check,"elm,state,rect,visible","elm"); */
			elm_layout_signal_emit(item_data->eo_check, "slider,center,point,show", "");
		} else {
			/*elm_object_signal_emit(item_data->eo_check,"elm,state,rect,hidden","elm"); */
			elm_layout_signal_emit(item_data->eo_check, "slider,center,point,hide", "");
		}
#endif
		if (item_data->item) {
			/* convey highlight to its content */
			items = eina_list_append(items, slider);
			elm_object_item_access_order_set(item_data->item, items);
		}
		return slider;
	}
	return NULL;
}
char *__item_multiline_overheating_text_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;
	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr && item_data->sub_desc) {
			if (!safeStrCmp(item_data->sub_desc, "overheating")) {
				ret_str = setting_customize_text(_(item_data->keyStr), -1, "#FF0000", "middle");
			}
		}
	}

	return ret_str;
}

void construct_brightness(void *data, Evas_Object *genlist)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	setting_create_Gendial_itc("slider.main", &(ad->itc_1icon));
	char *left_icon = NULL;
	ad->itc_1icon.func.content_get = __setting_brightness_add_slider;
	ad->last_requested_level = -1;

	/* [control] AUTOMATIC - boolean - vconf */
	int auto_value = SETTING_BRIGHTNESS_AUTOMATIC_ON;
	int err;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &auto_value, &err);/*if get failed,to hold value SETTING_BRIGHTNESS_AUTOMATIC_ON */
	SETTING_TRACE("auto_value:%d", auto_value);
	if (0 != ret) {
		/*add error handle,due to different target env.. */
		SETTING_TRACE_ERROR("Failed to get value of [%s]", VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT);
	}

	/* check manual */
	int value = SETTING_BRIGHTNESS_LEVEL5;
	/*ret = setting_get_int_slp_key(INT_SLP_SETTING_LCD_BRIGHTNESS, &value, &err); */
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
	                                     left_icon,
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

	char *temp = NULL;
	if (auto_value) {
		temp = (char *)g_strdup(KeyStr_Brightness_Auto_Adjust);
	} else {
		temp = (char *)g_strdup(KeyStr_Brightness_Adjust);
	}

	/*ADD_GL_HELP_NO_SEP(genlist, temp); */

	if (ad->data_adjust) {
		ad->data_adjust->userdata = ad;
		__BACK_POINTER_SET(ad->data_adjust);
		/*elm_genlist_item_select_mode_set(ad->data_adjust->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY); */
	} else {
		SETTING_TRACE_ERROR("ad->data_adjust is NULL");
	}
	G_FREE(temp);


	_brightness_register_event_cb(ad);
	setting_brightness_dbus_handler_init(ad);
}

void destruct_brightness(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
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
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;
	Evas_Object *genlist = elm_genlist_add(ad->win_main_layout);
	retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "Cannot set scroller object as contento of layout");
	/*elm_object_style_set(genlist, "dialogue"); */
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
	/*ad->genlist = genlist; */
	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, ad);

	construct_brightness(ad, genlist);


	/*key notifi for refreshing */
	if (&setting_view_display_brightness == ad->view_to_load) {
		vconf_notify_key_changed(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, __display_int_vconf_cb, ad);
	}
	setting_view_display_brightness.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_brightness_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_display_brightness.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

#if DISABLE_LOW_BATERY_CHANGE
	/* low battery handler - registration */
	vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, setting_display_brightness_lowbat_cb);
#endif

	destruct_brightness(cb);/*------- */

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;
	if (&setting_view_display_brightness == ad->view_to_load) {
		vconf_ignore_key_changed(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, __display_int_vconf_cb);
	}

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
	SETTING_TRACE_BEGIN;
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
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
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
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");

	SettingDisplayUG *ad = data;
	const char *vconf_str = __display_brightness_get_vconf_need_save();
	if (!safeStrCmp(vconf_str, VCONFKEY_SETAPPL_LCD_BRIGHTNESS)) {
		int ret, err;
		/*ret =  device_display_set_brightness(0, value);/*this API will set vconf value, so no need to set vconf in setting side* / */
		ret = display_set_brightness_with_setting(value);
		if (ret == DEVICE_ERROR_NONE) {
			ad->data_br_sli->chk_status = (int)value;
			SETTING_TRACE("value:%f", value);
		} else {
			SETTING_TRACE(" device_display_set_brightness : failed[ret=%d]", ret);
			/*rollback.. */
			int tmp = SETTING_BRIGHTNESS_LEVEL5;
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
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	int value = 0;

	SettingDisplayUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	double time_current = 0.0;
	static double time_before = 0.0;

	time_current = ecore_loop_time_get();

	double val = elm_slider_value_get(obj);
	SETTING_TRACE("val:%f", val);
	value = (int)(val + 0.5);
	/* SETTING_TRACE("val:%f", val); */
	SETTING_TRACE("value:%d, list_item->slider_max:%lf", value, list_item->slider_max);
	if (value != ad->last_requested_level) {
		if (value >= list_item->slider_min && value <= list_item->slider_max) {
			ad->last_requested_level = value;
			SETTING_TRACE("value:%d", value);
			if (time_current - time_before >= 0.1) {
				setting_display_set_slider_value(ad, obj, value);
				setting_display_update_slider_icon(ad->data_br_sli, value);
				time_before = time_current;
			}
		}
	}
}


