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
#include <setting-display-main.h>
#include <dd-display.h>
#include <eventsystem.h>

#include <setting-common-draw-widget.h>
#include <bundle_internal.h>

#define KEYSTR_SCREEN_ROTATION "IDS_ST_MBODY_SCREEN_ROTATION"


static int setting_display_main_create(void *cb);
static int setting_display_main_destroy(void *cb);
static int setting_display_main_update(void *cb);
static int setting_display_main_cleanup(void *cb);

setting_view setting_view_display_main = {
	.create = setting_display_main_create,
	.destroy = setting_display_main_destroy,
	.update = setting_display_main_update,
	.cleanup = setting_display_main_cleanup,
};

extern const char *str_in_arr[];
extern const char *str_out_arr[];
const char *screen_capture_destination[] = {
	KeyStr_ScreenCapture_Ask,
	KeyStr_ScreenCapture_Gallery,
	KeyStr_ScreenCapture_KeepIt,
	NULL,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void __screen_timeout_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	ret_if(data == NULL);
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingDisplayUG *ad = data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	int value = 0;
	if (!safeStrCmp(KeyStr_Backlight_15_SEC_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 0);
		value = 15;
	} else if (!safeStrCmp(KeyStr_Backlight_30_SEC_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 1);
		value = 30;
	} else if (!safeStrCmp(KeyStr_Backlight_1_MIN_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 2);
		value = 60;
	} else if (!safeStrCmp(KeyStr_Backlight_2_MIN_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 3);
		value = 120;
	} else if (!safeStrCmp(KeyStr_Backlight_5_MIN_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 4);
		value = 300;
	} else if (!safeStrCmp(KeyStr_Backlight_10_MIN_STR, list_item->keyStr)) {
		elm_radio_value_set(ad->screen_timeout_rdg, 5);
		value = 600;
	}

	int err;
	setting_set_int_slp_key(ad->data_back->int_slp_setting_binded, value, &err);

	if (ad->screen_timeout_popup) {
		evas_object_del(ad->screen_timeout_popup);
		ad->screen_timeout_popup = NULL;
	}
}

static void setting_display_screen_timeout_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	Evas_Object *menu_glist = NULL;
	ad->screen_timeout_popup = setting_create_popup_with_list(&menu_glist, ad, ad->win_get,
	                                                          KeyStr_BacklightTime, NULL, 0, 0);
	_P(ad->screen_timeout_popup);

	Evas_Object *rdg = elm_radio_add(menu_glist);
	elm_object_style_set(rdg, "list");
	evas_object_propagate_events_set(rdg, EINA_TRUE);
	elm_radio_state_value_set(rdg, -1);
	evas_object_data_set(menu_glist, "radio", rdg);
	ad->screen_timeout_rdg = rdg;

	ad->data_backlight_15sec = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                               __screen_timeout_cb, ad,
	                                                               SWALLOW_Type_1RADIO,
	                                                               rdg, 0, KeyStr_Backlight_15_SEC_STR,
	                                                               NULL);
	if (ad->data_backlight_15sec) {
		ad->data_backlight_15sec->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_15sec);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_15sec is NULL");
	}

	ad->data_backlight_30sec = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                               __screen_timeout_cb, ad,
	                                                               SWALLOW_Type_1RADIO,
	                                                               rdg, 1, KeyStr_Backlight_30_SEC_STR,
	                                                               NULL);
	if (ad->data_backlight_30sec) {
		ad->data_backlight_30sec->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_30sec);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_30sec is NULL");
	}

	ad->data_backlight_1min = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                              __screen_timeout_cb, ad,
	                                                              SWALLOW_Type_1RADIO,
	                                                              rdg, 2, KeyStr_Backlight_1_MIN_STR,
	                                                              NULL);
	if (ad->data_backlight_1min) {
		ad->data_backlight_1min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_1min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_1min is NULL");
	}

	ad->data_backlight_2min = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                              __screen_timeout_cb, ad,
	                                                              SWALLOW_Type_1RADIO,
	                                                              rdg, 3, KeyStr_Backlight_2_MIN_STR,
	                                                              NULL);
	if (ad->data_backlight_2min) {
		ad->data_backlight_2min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_2min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_2min is NULL");
	}

	ad->data_backlight_5min = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                              __screen_timeout_cb, ad,
	                                                              SWALLOW_Type_1RADIO,
	                                                              rdg, 4, KeyStr_Backlight_5_MIN_STR,
	                                                              NULL);
	if (ad->data_backlight_5min) {
		ad->data_backlight_5min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_5min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_5min is NULL");
	}

	ad->data_backlight_10min = setting_create_Gendial_field_1radio(menu_glist, &(ad->itc_1icon_1text_sub),
	                                                               __screen_timeout_cb, ad,
	                                                               SWALLOW_Type_1RADIO,
	                                                               rdg, 5, KeyStr_Backlight_10_MIN_STR,
	                                                               NULL);
	if (ad->data_backlight_10min) {
		ad->data_backlight_10min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_10min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_10min is NULL");
	}

	/* update radio */
	int value;
	int err;
	setting_get_int_slp_key(ad->data_back->int_slp_setting_binded, &value, &err);
	if (15 == value) {
		elm_radio_value_set(rdg, 0);
	} else if (30 == value) {
		elm_radio_value_set(rdg, 1);
	} else if (60 == value) {
		elm_radio_value_set(rdg, 2);
	} else if (120 == value) {
		elm_radio_value_set(rdg, 3);
	} else if (300 == value) {
		elm_radio_value_set(rdg, 4);
	} else if (600 == value) {
		elm_radio_value_set(rdg, 5);
	}
}

#if !SUPPOR_SEPARATE_BRIGHTNESS
/* low battery */
static void setting_display_main_lowbat_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	SETTING_TRACE_BEGIN;
	ret_if(ad->data_br == NULL);

	char *vconf_key = vconf_keynode_get_name(key);
	SETTING_TRACE("the value of [ %s ] just changed", vconf_key);
	/* VCONFKEY_SYSMAN_BATTERY_STATUS_LOW */

	int battery_value;
	vconf_get_int(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, &battery_value);
	ad->low_bat = battery_value;

	if (battery_value < VCONFKEY_SYSMAN_BAT_WARNING_LOW) {
		elm_slider_value_set(ad->data_br->eo_check, 1);
		/* disabled */
		setting_disable_genlist_item(ad->data_br->item);
	} else {
		setting_enable_genlist_item(ad->data_br->item);
	}
	return;
}
#endif

static char *___touchkey_light_duration_sub_desc()
{
	SETTING_TRACE_BEGIN;
	int value = 0;
	vconf_get_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, &value);
	switch (value) {
		case 0:
			return g_strdup("IDS_ST_BODY_ALWAYS_OFF");
		case -1:
			return g_strdup("IDS_ST_BODY_ALWAYS_ON");
		case 90: {
				return g_strdup("IDS_ST_BODY_1_5_SECONDS");
			}
		case 360: {
				return g_strdup("IDS_MSG_OPT_6_SECONDS");
			}
		default:
			return g_strdup("IDS_ST_BODY_ALWAYS_ON");
	}
	return NULL;
}
static char *___format_touchkey_light_duration_sub_desc(char *temp)
{
	/*SETTING_TRACE_BEGIN; */
	/*SETTING_TRACE("temp:%s",temp); */
	if (!safeStrCmp(temp, "IDS_ST_BODY_ALWAYS_OFF")) {
		return g_strdup(_("IDS_ST_BODY_ALWAYS_OFF"));
	} else if (!safeStrCmp(temp, "IDS_ST_BODY_ALWAYS_ON")) {
		return g_strdup(_("IDS_ST_BODY_ALWAYS_ON"));
	} else if (!safeStrCmp(temp, "1.5")) {
		char duration[MAX_DISPLAY_NAME_LEN_ON_UI] = {0, };
		snprintf(duration, sizeof(duration), "%s %s", temp, (char *)(_("IDS_ST_BODY_SECONDS")));
		return g_strdup(duration);
	} else if (!safeStrCmp(temp, "6")) {
		char duration[MAX_DISPLAY_NAME_LEN_ON_UI] = {0, };
		snprintf(duration, sizeof(duration), "%s %s", temp, (char *)(_("IDS_ST_BODY_SECONDS")));
		return g_strdup(duration);
	} else {
		return g_strdup(_("IDS_ST_BODY_ALWAYS_ON"));
	}
	return NULL;
}

char *__setting_display_screen_capture_destination_get()
{
	SETTING_TRACE_BEGIN;
	int value = 0;
	int ret = vconf_get_int(VCONFKEY_SETAPPL_SCREEN_CAPTURE_DESTINATION, &value);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	char *temp = NULL;
	switch (value) {
		case SETTING_SCREEN_CAPTURE_DESTINATION_ASK:
			temp = (char *)(screen_capture_destination[SETTING_SCREEN_CAPTURE_DESTINATION_ASK]);
			break;
		case SETTING_SCREEN_CAPTURE_DESTINATION_GALLERY:
			temp = (char *)(screen_capture_destination[SETTING_SCREEN_CAPTURE_DESTINATION_GALLERY]);
			break;
		case SETTING_SCREEN_CAPTURE_DESTINATION_KEEPIT:
			temp = (char *)(screen_capture_destination[SETTING_SCREEN_CAPTURE_DESTINATION_KEEPIT]);
			break;
		default:
			SETTING_TRACE_ERROR("incorrect screen capture destination");
			temp = (char *)(screen_capture_destination[SETTING_SCREEN_CAPTURE_DESTINATION_ASK]);
			break;
	}
	return temp;
}

char *__item_touch_duration_sub_item_text_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;
	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr) {
			ret_str = ___format_touchkey_light_duration_sub_desc(item_data->keyStr);
		}
	}
	return ret_str;
}
char *__item_touch_duration_text_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;
	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr) {
			ret_str = (char *)g_strdup(_(item_data->keyStr));
		}
	} else if (!safeStrCmp(part, "elm.text.2")) {	/* bottom or right */
		if (item_data->sub_desc) {
			ret_str = ___format_touchkey_light_duration_sub_desc(item_data->sub_desc);
		}
	}
	return ret_str;
}
char *__item_backlight_sub_item_text_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;
	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr) {
			ret_str =  format_backlight_time_str(item_data->keyStr);
		}
	}
	return ret_str;
}


char *__item_backlight_text_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;
	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr) {
			ret_str = (char *)g_strdup(_(item_data->keyStr));
		}
	} else if (!safeStrCmp(part, "elm.text.2")) {	/* bottom or right */
		if (item_data->sub_desc) {
			ret_str =  format_backlight_time_str(item_data->sub_desc);
		}
	}
	/*SETTING_TRACE_END; */
	return ret_str;
}
static void setting_display_main_vconf_change_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);

	SettingDisplayUG *ad = data;
	int status = 0;

	status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("status:%d", status);
	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL)) {
		if (ad->data_auto_rotate && ad->data_auto_rotate->eo_check) {
			setting_update_gl_item_chk_status(ad->data_auto_rotate, status);
			if (status == TRUE) {
				if (ad->data_smart_rotation && ad->data_smart_rotation->item) {
					setting_enable_genlist_item(ad->data_smart_rotation->item);
				}
			} else if (status == FALSE) {
				if (ad->data_smart_rotation && ad->data_smart_rotation->item) {
					setting_disable_genlist_item(ad->data_smart_rotation->item);
				}
			}
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT)) {
		setting_update_gl_item_chk_status(ad->data_auto_adjust_scrn_tone, status);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR)) {
		setting_update_gl_item_chk_status(ad->data_dynamic, status);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL)) {
		if (ad->data_back) {
			G_FREE(ad->data_back->sub_desc);
			ad->data_back->sub_desc = get_pa_backlight_time_value_str();
			elm_object_item_data_set(ad->data_back->item, ad->data_back);
			elm_genlist_item_update(ad->data_back->item);
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION)) {
		if (ad->data_touchkey_light_duration) {
			G_FREE(ad->data_touchkey_light_duration->sub_desc);
			ad->data_touchkey_light_duration->sub_desc = ___touchkey_light_duration_sub_desc();
			elm_object_item_data_set(ad->data_touchkey_light_duration->item, ad->data_touchkey_light_duration);
			elm_genlist_item_update(ad->data_touchkey_light_duration->item);
		}
	}

}

void auto_rotate_event_handler(const char *event_name, bundle *data, void *user_data)
{
	const char *auto_rotate_set = NULL;
	SETTING_TRACE("auto rotate set event(%s) received", event_name);

	auto_rotate_set = bundle_get_val(data, EVT_KEY_SCREEN_AUTOROTATE_STATE);
	SETTING_TRACE("auto_rotate_set(%s", auto_rotate_set);
}

int auto_rotate_event_reg_id;

static int setting_display_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	Evas_Object *scroller = NULL;

	int value = 0;
	int ret = 0;

	/* add basic layout */
	char setBtnStr[MAX_DISPLAY_NAME_LEN_ON_UI];
	snprintf(setBtnStr, sizeof(setBtnStr), "%s",
	         (char *)dgettext("sys_string", "IDS_ST_BUTTON_BACK"));

	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           KeyStr_Display, setBtnStr,
	                                           NULL,
	                                           (setting_call_back_func)setting_display_main_click_softkey_back_cb,
	                                           NULL, ad, &scroller,
	                                           &(ad->navi_bar));

	ad->genlist = scroller;
	elm_genlist_mode_set(ad->genlist, ELM_LIST_COMPRESS);
	/*register vconf key */

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION,
	                               setting_display_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf notifications Failed %d", ret);
	}



	/* BRIGHTNESS */
	construct_brightness(ad, scroller);

	/* FONT */
	ad->data_font =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
	                                     setting_display_main_mouse_up_Gendial_list_cb,
	                                     ad, SWALLOW_Type_INVALID, NULL,
	                                     NULL, 0, KeyStr_Font,
	                                     NULL, NULL);
	if (ad->data_font) {
		ad->data_font->userdata = ad;
		ad->data_font->group_style = SETTING_GROUP_STYLE_TOP;
		__BACK_POINTER_SET(ad->data_font);
	} else {
		SETTING_TRACE_ERROR("ad->data_font is NULL");
	}

	/* AUTO ROTATE */
	ret = vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &value);
	ad->data_auto_rotate =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_1text_1icon),
	                                     setting_display_main_mouse_up_Gendial_list_cb,
	                                     ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                     NULL, value, "IDS_ST_HEADER_AUTO_ROTATE_SCREEN_ABB",
	                                     NULL,
	                                     setting_display_main_auto_rotate_chk_btn_cb);
	if (ad->data_auto_rotate) {
		ad->data_auto_rotate->userdata = ad;
		__BACK_POINTER_SET(ad->data_auto_rotate);
		setting_genlist_item_groupstyle_set(ad->data_auto_rotate, SETTING_GROUP_STYLE_TOP);
	} else {
		SETTING_TRACE_ERROR("ad->data_battery is NULL");
	}

	/* BACKLIGHTTIME */
	char *pa_backlight_time = get_pa_backlight_time_str();
	ad->data_back =
	    setting_create_Gendial_field_def(scroller,
	                                     &itc_2text_3_parent,
	                                     setting_display_main_mouse_up_Gendial_list_cb, ad,
	                                     SWALLOW_Type_INVALID, NULL, NULL,
	                                     0, KeyStr_BacklightTime,
	                                     pa_backlight_time,
	                                     NULL);

	if (ad->data_back) {
		ad->data_back->int_slp_setting_binded = INT_SLP_SETTING_LCD_TIMEOUT_NORMAL;
		ad->data_back->group_style = SETTING_GROUP_STYLE_CENTER;
		__BACK_POINTER_SET(ad->data_back);
	} else {
		SETTING_TRACE_ERROR("ad->data_back is NULL");
	}
	G_FREE(pa_backlight_time);

#if !SUPPOR_SEPARATE_BRIGHTNESS
	if (ad->data_br) {
		int battery_value;
		vconf_get_int(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, &battery_value);
		ad->low_bat = battery_value;

		if (battery_value < VCONFKEY_SYSMAN_BAT_WARNING_LOW) {
			elm_slider_value_set(ad->data_br->eo_check, 1);
			setting_disable_genlist_item(ad->data_br->item);
		} else {
			setting_enable_genlist_item(ad->data_br->item);
		}
	}
	if (ad->display_expand_state == DISPLAY_EXPAND_BACKLIGHT) {
		if (ad->data_back && ad->data_back->item) {
			elm_genlist_item_expanded_set(ad->data_back->item, TRUE);
		}
	}
	vconf_notify_key_changed(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, setting_display_main_lowbat_cb, ad);
#endif

	/* eventsystem */
	if (eventsystem_register_event(SYS_EVENT_SCREEN_AUTOROTATE_STATE,
	                               &auto_rotate_event_reg_id,
	                               (eventsystem_handler)auto_rotate_event_handler, cb) != ES_R_OK) {
		SETTING_TRACE_ERROR("error");
	}

	/*  add separator in botton of view */
	Elm_Object_Item *item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	setting_view_display_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_display_main.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;
	destruct_brightness(ad);
#if !SUPPORT_SEPARATE_BRIGHTNESS
	/* low battery handler - registration */
	vconf_ignore_key_changed(VCONFKEY_SYSMAN_BATTERY_STATUS_LOW, setting_display_main_lowbat_cb);
#endif
	vconf_ignore_key_changed(VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, __display_int_vconf_cb);
	int ret = 0;
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION,
	                               setting_display_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf ignore Failed %d", ret);
	}

	if (ad->nf_it) {
		ad->nf_it = NULL;
	}
	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		/* if(ad->back_dialData) FREE(ad->back_dialData); */
	}
	setting_view_display_main.is_create = 0;

	if (ES_R_OK != eventsystem_unregister_event(auto_rotate_event_reg_id)) {
		SETTING_TRACE_ERROR("error");
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
		if (ad->data_br) {
			ad->data_br->sub_desc =
			    (char *)g_strdup(get_brightness_mode_str());
			elm_object_item_data_set(ad->data_br->item, ad->data_br);
			elm_genlist_item_update(ad->data_br->item);

		}
#ifdef SUPPORT_SCREEN_MODE
		if (ad->data_screen_mode) {
			char *curmode = vconf_get_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME);
			SETTING_TRACE(">>> CUR SCREEN MODE : %s ", curmode);
			int i = 0;
			for (; i < SCREENMODE_MAX; i++) {
				if (0 == safeStrCmp(str_in_arr[i], curmode)) {
					ad->data_screen_mode->sub_desc = (char *)g_strdup(_(str_out_arr[i]));
					break;
				}
			}
			FREE(curmode);

			elm_object_item_data_set(ad->data_screen_mode->item, ad->data_screen_mode);
			elm_genlist_item_update(ad->data_screen_mode->item);
		}
#endif
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_display_destroy_font_ug_cb(ui_gadget_h ug,
                                        void *priv)
{
	ret_if(priv == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_font = NULL;
	}
	elm_genlist_realized_items_update(ad->genlist);
}


Eina_Bool ___display_freeze_event_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, EINA_FALSE);

	SettingDisplayUG *ad = (SettingDisplayUG *)cb;

	evas_object_freeze_events_set(ad->navi_bar, EINA_FALSE);
	ad->event_freeze_timer = NULL;
	return EINA_FALSE;
}

gboolean setting_display_create_font_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingDisplayUG *ad = (SettingDisplayUG *) data;	/* ad is point to data */
	if (0 == app_launcher("setting-font-efl")) {
		if (ad->event_freeze_timer) {
			ecore_timer_del(ad->event_freeze_timer);
			ad->event_freeze_timer = NULL;
		}
		ad->event_freeze_timer = ecore_timer_add(1, ___display_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
	return TRUE;
}

void setting_display_destroy_ledindicator_ug_cb(ui_gadget_h ug,
                                                void *priv)
{
	ret_if(priv == NULL);
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_ledindicator = NULL;
	}
	/* elm_genlist_realized_items_update(ad->genlist); */
}

gboolean setting_display_create_ledindicator_sg(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingDisplayUG *ad = (SettingDisplayUG *) data;	/* ad is point to data */

	if (ad->ug_ledindicator) {
		SETTING_TRACE("Font UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs) {
		return FALSE;
	}
	cbs->layout_cb = setting_display_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_display_destroy_ledindicator_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_ledindicator = setting_ug_create(ad->ug, "setting-ledindicator-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_ledindicator) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_ledindicator");
		evas_object_show(ad->ly_main);
	}

	FREE(cbs);

	return TRUE;
}

static void
setting_display_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
                                              void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp("IDS_ST_BODY_BRIGHTNESS_M_POWER_SAVING", list_item->keyStr)) {
		setting_view_change(&setting_view_display_main,
		                    &setting_view_display_brightness, ad);
	} else if (!safeStrCmp(KEYSTR_SCREEN_ROTATION, list_item->keyStr)) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
		vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, list_item->chk_status);
	} else if (!safeStrCmp(KeyStr_Font, list_item->keyStr)) {
		setting_display_create_font_sg(ad);
	} else if (!safeStrCmp(keystr_LedIndicator, list_item->keyStr)) {
		setting_display_create_ledindicator_sg(ad);
	} else if (!safeStrCmp(KeyStr_ScreenCapture_Edit, list_item->keyStr)) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
		vconf_set_bool(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE, list_item->chk_status);
	} else if (!safeStrCmp(KeyStr_Dynamic_Status_Bar, list_item->keyStr)) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
		vconf_set_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, list_item->chk_status);
	} else if (!safeStrCmp(KeyStr_BacklightTime, list_item->keyStr)) {
		setting_display_screen_timeout_popup(ad);
	}
}

static Eina_Bool
setting_display_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                           void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > Display] Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;

}

static void setting_display_main_auto_rotate_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingDisplayUG *ad = (SettingDisplayUG *)list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */

	if (list_item->chk_status == 1) {
		int angle = elm_win_rotation_get(ad->win_main_layout);
		elm_win_rotation_with_resize_set(ad->win_main_layout, angle);
		setting_set_event_system(SYS_EVENT_SCREEN_AUTOROTATE_STATE, EVT_KEY_SCREEN_AUTOROTATE_STATE, EVT_VAL_SCREEN_AUTOROTATE_ON);
	} else {
		elm_win_rotation_with_resize_set(ad->win_main_layout, 0);
		setting_set_event_system(SYS_EVENT_SCREEN_AUTOROTATE_STATE, EVT_KEY_SCREEN_AUTOROTATE_STATE, EVT_VAL_SCREEN_AUTOROTATE_OFF);
	}

	vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, list_item->chk_status);
}
