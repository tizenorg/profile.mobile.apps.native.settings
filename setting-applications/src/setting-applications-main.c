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
#include <setting-applications-main.h>
#include <dd-display.h>

#include <setting-common-draw-widget.h>

#define KEYSTR_SCREEN_ROTATION "IDS_ST_MBODY_SCREEN_ROTATION"
#define KEYSTR_TTS "TTS"

#define KEYSTR_APPLICATION_MANAGER "Application Manager"
#define KEYSTR_DEFAULT_APPLICATION "Default applications"



static int setting_applications_main_create(void *cb);
static int setting_applications_main_destroy(void *cb);
static int setting_applications_main_update(void *cb);
static int setting_applications_main_cleanup(void *cb);

setting_view setting_view_applications_main = {
	.create = setting_applications_main_create,
	.destroy = setting_applications_main_destroy,
	.update = setting_applications_main_update,
	.cleanup = setting_applications_main_cleanup,
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
static void
__setting_applications_main_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;

	if (ad->data_back) {
		ad->data_back->sub_desc = get_pa_backlight_time_str();
		elm_object_item_data_set(ad->data_back->item, ad->data_back);
		elm_genlist_item_update(ad->data_back->item);
	}
	/* End */

	if (data_parentItem == ad->data_back) {
		/* Update backlight menu for MDM update */
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		/*evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL); */
		/*elm_object_signal_emit(scroller, "realized", ""); */

		if (isEmulBin()) {
			ad->data_backlight_always_on = setting_create_Gendial_exp_sub_field(scroller,
			                                                                    &(ad->itc_1icon_1text_sub),
			                                                                    NULL, NULL, parentItem,
			                                                                    SWALLOW_Type_1RADIO, rgd,
			                                                                    0,				/* Always ON */
			                                                                    KeyStr_Backlight_Alays_On, NULL);

			if (ad->data_backlight_always_on) {
				ad->data_backlight_always_on->userdata = ad;
				/*ad->data_backlight_always_on->group_style = SETTING_GROUP_STYLE_CENTER; */
				setting_genlist_item_groupstyle_set(ad->data_backlight_always_on, SETTING_GROUP_STYLE_CENTER);
				__BACK_POINTER_SET(ad->data_backlight_always_on);
			} else {
				SETTING_TRACE_ERROR("ad->data_backlight_always_on is NULL");
			}

		}
		char backlight_time[MAX_DISPLAY_NAME_LEN_ON_UI] = {0, };
		snprintf(backlight_time, sizeof(backlight_time), "15 %s", (char *)(_("IDS_ST_BODY_SECONDS")));
		ad->data_backlight_15sec = setting_create_Gendial_exp_sub_field(scroller,
		                                                                &(ad->itc_1icon_1text_sub),
		                                                                NULL, NULL, parentItem,
		                                                                SWALLOW_Type_1RADIO, rgd,
		                                                                15,				/* 15 sec */
		                                                                KeyStr_Backlight_15_SEC_STR, NULL);
		if (ad->data_backlight_15sec) {
			ad->data_backlight_15sec->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_15sec, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_15sec);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_15sec is NULL");
		}
		snprintf(backlight_time, sizeof(backlight_time), "30 %s", (char *)(_("IDS_ST_BODY_SECONDS")));
		ad->data_backlight_30sec = setting_create_Gendial_exp_sub_field(scroller,
		                                                                &(ad->itc_1icon_1text_sub),
		                                                                NULL, NULL, parentItem,
		                                                                SWALLOW_Type_1RADIO, rgd,
		                                                                30,				/* 30 sec */
		                                                                KeyStr_Backlight_30_SEC_STR, NULL);
		if (ad->data_backlight_30sec) {
			ad->data_backlight_30sec->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_30sec, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_30sec);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_30sec is NULL");
		}
		snprintf(backlight_time, sizeof(backlight_time), "1 %s", (char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
		ad->data_backlight_1min = setting_create_Gendial_exp_sub_field(scroller,
		                                                               &(ad->itc_1icon_1text_sub),
		                                                               NULL, NULL, parentItem,
		                                                               SWALLOW_Type_1RADIO, rgd,
		                                                               60,				/* 60 sec */
		                                                               KeyStr_Backlight_1_MIN_STR, NULL);
		if (ad->data_backlight_1min) {
			ad->data_backlight_1min->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_1min, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_1min);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_1min is NULL");
		}
		snprintf(backlight_time, sizeof(backlight_time), "2 %s", (char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
		ad->data_backlight_2min = setting_create_Gendial_exp_sub_field(scroller,
		                                                               &(ad->itc_1icon_1text_sub),
		                                                               NULL, NULL, parentItem,
		                                                               SWALLOW_Type_1RADIO, rgd,
		                                                               120,				/* 2min */
		                                                               KeyStr_Backlight_2_MIN_STR, NULL);
		if (ad->data_backlight_2min) {
			ad->data_backlight_2min->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_2min, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_2min);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_2min is NULL");
		}
		snprintf(backlight_time, sizeof(backlight_time), "5 %s", (char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
		ad->data_backlight_5min = setting_create_Gendial_exp_sub_field(scroller,
		                                                               &(ad->itc_1icon_1text_sub),
		                                                               NULL, NULL, parentItem,
		                                                               SWALLOW_Type_1RADIO, rgd,
		                                                               300,				/* 5min */
		                                                               KeyStr_Backlight_5_MIN_STR, NULL);
		if (ad->data_backlight_5min) {
			ad->data_backlight_5min->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_5min, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_5min);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_5min is NULL");
		}
		snprintf(backlight_time, sizeof(backlight_time), "10 %s", (char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
		ad->data_backlight_10min = setting_create_Gendial_exp_sub_field(scroller,
		                                                                &(ad->itc_1icon_1text_sub),
		                                                                NULL, NULL, parentItem,
		                                                                SWALLOW_Type_1RADIO, rgd,
		                                                                600,				/* 10min */
		                                                                KeyStr_Backlight_10_MIN_STR, NULL);
		if (ad->data_backlight_10min) {
			ad->data_backlight_10min->userdata = ad;
			setting_genlist_item_groupstyle_set(ad->data_backlight_10min, SETTING_GROUP_STYLE_CENTER);
			__BACK_POINTER_SET(ad->data_backlight_10min);
		} else {
			SETTING_TRACE_ERROR("ad->data_backlight_10min is NULL");
		}

		setting_update_chk_status(rgd,
		                          data_parentItem->int_slp_setting_binded);

	} else if (data_parentItem == ad->data_touchkey_light_duration) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		char duration[MAX_DISPLAY_NAME_LEN_ON_UI] = {0, };
		snprintf(duration, sizeof(duration), "1.5 %s", (char *)(_("IDS_ST_BODY_SECONDS")));
		setting_create_Gendial_exp_sub_field_with_group_style(scroller,
		                                                      &(ad->itc_1icon_1text_sub),
		                                                      NULL, ad, parentItem,
		                                                      SWALLOW_Type_1RADIO, rgd,
		                                                      90, "IDS_ST_BODY_1_5_SECONDS",
		                                                      NULL, SETTING_GROUP_STYLE_CENTER);

		snprintf(duration, sizeof(duration), "6 %s", (char *)(_("IDS_ST_BODY_SECONDS")));
		setting_create_Gendial_exp_sub_field_with_group_style(scroller,
		                                                      &(ad->itc_1icon_1text_sub),
		                                                      NULL, ad, parentItem,
		                                                      SWALLOW_Type_1RADIO, rgd,
		                                                      360, "IDS_MSG_OPT_6_SECONDS",
		                                                      NULL, SETTING_GROUP_STYLE_CENTER);
		setting_create_Gendial_exp_sub_field_with_group_style(scroller,
		                                                      &(ad->itc_1icon_1text_sub),
		                                                      NULL, ad, parentItem,
		                                                      SWALLOW_Type_1RADIO, rgd,
		                                                      0, "IDS_ST_BODY_ALWAYS_OFF",
		                                                      NULL, SETTING_GROUP_STYLE_CENTER);
		setting_create_Gendial_exp_sub_field_with_group_style(scroller,
		                                                      &(ad->itc_1icon_1text_sub_touch),
		                                                      NULL, ad, parentItem,
		                                                      SWALLOW_Type_1RADIO, rgd,
		                                                      -1, "IDS_ST_BODY_ALWAYS_ON",
		                                                      NULL, SETTING_GROUP_STYLE_BOTTOM);

		setting_update_chk_status(rgd, data_parentItem->int_slp_setting_binded);
	}

	evas_object_show(scroller);
}
#if !SUPPOR_SEPARATE_BRIGHTNESS
/* low battery */
static void setting_applications_main_lowbat_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
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

char *__setting_applications_screen_capture_destination_get()
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
static void
setting_applications_main_chk_btn_cb(void *data, Evas_Object *obj,
                                     void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	if (!safeStrCmp(KeyStr_Dynamic_Status_Bar, list_item->keyStr)) {
		vconf_set_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, list_item->chk_status);
	}
}

static void
setting_applications_main_screen_capture_edit_chk_btn_cb(void *data, Evas_Object *obj,
                                                         void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	vconf_set_bool(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE, list_item->chk_status);
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
static void setting_applications_main_vconf_change_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);

	SettingApplicationsUG *ad = data;
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


static int setting_applications_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

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
	                                           KeyStr_Applications, setBtnStr,
	                                           NULL,
	                                           (setting_call_back_func)setting_applications_main_click_softkey_back_cb,
	                                           NULL, ad, &scroller,
	                                           &(ad->navi_bar));

	ad->genlist = scroller;
	elm_genlist_mode_set(ad->genlist, ELM_LIST_COMPRESS);
	/*register vconf key */

	setting_enable_expandable_genlist(scroller, ad, __setting_applications_main_exp_cb, NULL);
	evas_object_smart_callback_add(ad->genlist, "realized", __gl_realized_cb, NULL);

	Elm_Object_Item *item = NULL;

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	setting_create_Gendial_field_def(ad->genlist, &itc_1text,
	                                 setting_applications_main_mouse_up_Gendial_list_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL,
	                                 NULL, 0,
	                                 KEYSTR_APPLICATION_MANAGER, NULL, NULL);

	setting_create_Gendial_field_def(ad->genlist, &itc_1text,
	                                 setting_applications_main_mouse_up_Gendial_list_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL,
	                                 NULL, 0,
	                                 KEYSTR_DEFAULT_APPLICATION, NULL, NULL);

	setting_view_applications_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_applications_main.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	/*ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, setting_applications_main_vconf_change_cb); */

	if (ad->nf_it) {
		ad->nf_it = NULL;
	}
	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		/* if(ad->back_dialData) FREE(ad->back_dialData); */
	}
	setting_view_applications_main.is_create = 0;


	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

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

static int setting_applications_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_applications_destroy_appmgr_ug_cb(ui_gadget_h ug,
                                               void *priv)
{
	ret_if(priv == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_appmgr = NULL;
	}
	elm_genlist_realized_items_update(ad->genlist);
}

gboolean setting_applications_create_appmgr_ug(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;	/* ad is point to data */

	if (ad->ug_appmgr) {
		SETTING_TRACE("Font UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs) {
		return FALSE;
	}
	cbs->layout_cb = setting_applications_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_applications_destroy_appmgr_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return FALSE;
	}
	app_control_add_extra_data(svc, "viewtype", "manage-applications");
	ad->ug_appmgr = setting_ug_create(ad->ug, "setting-manage-applications-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_appmgr) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_appmgr");
		evas_object_show(ad->ly_main);
	}

	app_control_destroy(svc);
	FREE(cbs);

	return TRUE;
}

static void
setting_applications_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
                                                   void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KEYSTR_APPLICATION_MANAGER, list_item->keyStr)) {
		setting_applications_create_appmgr_ug(ad);
	} else if (!safeStrCmp(KEYSTR_DEFAULT_APPLICATION, list_item->keyStr)) {
		setting_view_change(&setting_view_applications_main,
		                    &setting_view_applications_defaultapp, ad);
	}
}

static Eina_Bool
setting_applications_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                                void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > Applications] Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;

}

static void setting_applications_main_tts_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingApplicationsUG *ad = (SettingApplicationsUG *)list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	SETTING_TRACE("TTS check - clicked");

	if (list_item->chk_status == 1) {
		/*SETTING_TRACE("TTS check - 1111111111111"); */
		vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, 1);
	} else {
		vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, 0);
	}
}



static void setting_applications_main_auto_rotate_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingApplicationsUG *ad = (SettingApplicationsUG *)list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */

	if (list_item->chk_status == 1) {
		int angle = elm_win_rotation_get(ad->win_main_layout);
		elm_win_rotation_with_resize_set(ad->win_main_layout, angle);
	} else {
		elm_win_rotation_with_resize_set(ad->win_main_layout, 0);
	}

	vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, list_item->chk_status);
}
