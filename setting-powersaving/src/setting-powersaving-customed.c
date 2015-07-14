/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <setting-powersaving-customed.h>

static int setting_powersaving_customed_create(void *cb);
static int setting_powersaving_customed_destroy(void *cb);
static int setting_powersaving_customed_update(void *cb);
static int setting_powersaving_customed_cleanup(void *cb);

setting_view setting_view_powersaving_customed = {
	.create = setting_powersaving_customed_create,
	.destroy = setting_powersaving_customed_destroy,
	.update = setting_powersaving_customed_update,
	.cleanup = setting_powersaving_customed_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_powersaving_customed_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;

	Evas_Object *scroller = NULL;

	/* add basic layout */
	setting_push_layout_navi_bar_genlist(ad->win_main_layout,
					     ad->win_get,
					     _(KeyStr_Powersaving_Customed),
					     _("IDS_COM_BODY_BACK"),
					     NULL,
					     setting_powersaving_customed_click_softkey_cancel_cb,
					     NULL, ad, &scroller, ad->navi_bar);
	Elm_Object_Item *item = NULL;
	/*  add separator */
	item = elm_genlist_item_append(scroller, &itc_seperator,
				       NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	char *str_value = NULL;
#if SUPPORT_UI0_3
	setting_enable_expandable_genlist(scroller, ad, setting_powersaving_ug_exp_cb,NULL);
	str_value = get_pa_powersaving_at_str();
	ad->data_saving_at =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &itc_2text_3_parent,
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    KeyStr_Powersaving_AT,
						    str_value);

	if (ad->data_saving_at) {
		ad->data_saving_at->int_slp_setting_binded =
		    INT_SLP_SETTING_POWERSAVING_AT;
	} else {
		SETTING_TRACE_ERROR("ad->data_back is NULL");
	}
	__BACK_POINTER_SET(ad->data_saving_at);
	G_FREE(str_value);
#endif
	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad, SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 "Turn off each settings when it is not in use.",
					 NULL, NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int ret;
	int value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_WIFI, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}


	ad->data_wifi_off = setting_create_Gendial_field_def(scroller,
							     &itc_1text_1icon_2,
							     setting_powersaving_customed_mouse_up_Gendial_list_cb,
							     ad,
							     SWALLOW_Type_1CHECK,
							     NULL, NULL, value,
							     KeyStr_WIFI_Off,
							     NULL,
							     setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_wifi_off) {
		__BACK_POINTER_SET(ad->data_wifi_off);
		ad->data_wifi_off->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}

	value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BT, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}

	ad->data_bt_off = setting_create_Gendial_field_def(scroller,
							   &itc_1text_1icon_2,
							   setting_powersaving_customed_mouse_up_Gendial_list_cb,
							   ad,
							   SWALLOW_Type_1CHECK,
							   NULL, NULL, value,
							   KeyStr_BT_Off, NULL,
							   setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_bt_off) {
		__BACK_POINTER_SET(ad->data_bt_off);
		ad->data_bt_off->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}

	value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_GPS, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}

	ad->data_gps_off = setting_create_Gendial_field_def(scroller,
							    &itc_1text_1icon_2,
							    setting_powersaving_customed_mouse_up_Gendial_list_cb,
							    ad,
							    SWALLOW_Type_1CHECK,
							    NULL, NULL, value,
							    KeyStr_GPS_Off,
							    NULL,
							    setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_gps_off) {
		__BACK_POINTER_SET(ad->data_gps_off);
		ad->data_gps_off->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}
	value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DATASYNC, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}

	ad->data_sync_off = setting_create_Gendial_field_def(scroller,
							     &itc_1text_1icon_2,
							     setting_powersaving_customed_mouse_up_Gendial_list_cb,
							     ad,
							     SWALLOW_Type_1CHECK,
							     NULL, NULL, value,
							     KeyStr_SYNC_Off,
							     NULL,
							     setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_sync_off) {
		__BACK_POINTER_SET(ad->data_sync_off);
		ad->data_sync_off->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}

	value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_HOTSPOT, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}

	ad->data_hotspot_off = setting_create_Gendial_field_def(scroller,
							     &itc_1text_1icon_2,
							     setting_powersaving_customed_mouse_up_Gendial_list_cb,
							     ad,
							     SWALLOW_Type_1CHECK,
							     NULL, NULL, value,
							     KeyStr_HOTSPOT_Off,
							     NULL,
							     setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_hotspot_off) {
		__BACK_POINTER_SET(ad->data_hotspot_off);
		ad->data_hotspot_off->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}

	/*  add separator */
	item = elm_genlist_item_append(scroller, &itc_seperator,
				       NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* ADJUST BRIGHTNESS */
	int adjust_value;
	value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_STATUS, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	adjust_value = value;

	ad->data_adjust_bright = setting_create_Gendial_field_def(scroller,
							     &itc_1text_1icon,
							     setting_powersaving_customed_mouse_up_Gendial_list_cb,
							     ad,
							     SWALLOW_Type_1TOGGLE,
							     NULL, NULL, value,
							     KeyStr_Adjust_Bright,
							     NULL,
							     setting_powersaving_customed_use_chk_btn_cb);
	if (ad->data_adjust_bright) {
		__BACK_POINTER_SET(ad->data_adjust_bright);
		ad->data_adjust_bright->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_use_tilt is NULL");
	}


	/* BRIGHTNESS */
	str_value = setting_powersaving_ug_get_brightness_str();
	ad->data_brightness =
	    setting_create_Gendial_field_def(scroller, &itc_2text_3,
					     setting_powersaving_customed_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, KeyStr_Brightness,
					     str_value, NULL);
	if (ad->data_brightness) {
		ad->data_brightness->userdata = ad;
		__BACK_POINTER_SET(ad->data_screen_timeout);
	} else {
		SETTING_TRACE_ERROR("ad->data_br is NULL");
	}
	G_FREE(str_value);


	/*  add separator */
	item = elm_genlist_item_append(scroller, &itc_seperator,
				       NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);


	/* BACKLIGHT */
	str_value = get_pa_screen_timeout_str();
	ad->data_screen_timeout =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &itc_2text_3_parent,
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    KeyStr_Screen_Timeout,
						    str_value);

	if (ad->data_screen_timeout) {
		ad->data_screen_timeout->int_slp_setting_binded =
		    INT_SLP_SETTING_POWERSAVING_SCREEN_TIMEOUT;
	} else {
		SETTING_TRACE_ERROR("ad->data_back is NULL");
	}
	__BACK_POINTER_SET(ad->data_screen_timeout);
	G_FREE(str_value);

	if (!adjust_value && ad->data_brightness) {
		setting_disable_genlist_item(ad->data_brightness->item);
	}

	setting_view_powersaving_customed.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_customed_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	elm_naviframe_item_pop(ad->navi_bar);

	setting_view_powersaving_customed.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_customed_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;
	if (ad->data_brightness)
	{
		ad->data_brightness->sub_desc = setting_powersaving_ug_get_brightness_str();
		elm_object_item_data_set(ad->data_brightness->item, ad->data_brightness);
		elm_genlist_item_update(ad->data_brightness->item);
	}
	return SETTING_RETURN_SUCCESS;

}

static int setting_powersaving_customed_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_powersaving_customed_destroy(cb);

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
setting_powersaving_customed_click_softkey_cancel_cb(void *data,
						     Evas_Object *obj,
						     void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	setting_view_change(&setting_view_powersaving_customed,
			    &setting_view_powersaving_main, ad);

}

static void setting_powersaving_customed_mouse_up_Gendial_list_cb(void *data,
								  Evas_Object *
								  obj, void
								  *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KeyStr_Brightness, list_item->keyStr)) {
		setting_view_change(&setting_view_powersaving_customed,
				    &setting_view_powersaving_brightness, ad);
		return;
	}

	const char *vconf = NULL;
	if (!safeStrCmp(KeyStr_WIFI_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_WIFI;
	} else if (!safeStrCmp(KeyStr_BT_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BT;
	} else if (!safeStrCmp(KeyStr_GPS_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_GPS;
	} else if (!safeStrCmp(KeyStr_SYNC_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DATASYNC;
	} else if (!safeStrCmp(KeyStr_HOTSPOT_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_HOTSPOT;
	} else if (!safeStrCmp(KeyStr_Adjust_Bright, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_STATUS;
	}
	int old_status = elm_check_state_get(list_item->eo_check);
	int ret = vconf_set_bool(vconf, !old_status);
	setting_retm_if(0 != ret, "Failed to set vconf [%s]", vconf);
	/* new status */
	list_item->chk_status = !old_status;
	elm_check_state_set(list_item->eo_check, list_item->chk_status);
	if (!safeStrCmp(KeyStr_Adjust_Bright, list_item->keyStr) && ad->data_brightness) {
		if (0 == list_item->chk_status) {
			setting_disable_genlist_item(ad->data_brightness->item);
		} else {
			setting_enable_genlist_item(ad->data_brightness->item);
		}
	}

	return;
}

static void
setting_powersaving_customed_use_chk_btn_cb(void *data, Evas_Object *obj,
					    void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingPowersavingUG *ad = list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj); /*  for genlist update status */

	const char *vconf = NULL;
	if (!safeStrCmp(KeyStr_WIFI_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_WIFI;
	} else if (!safeStrCmp(KeyStr_BT_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BT;
	} else if (!safeStrCmp(KeyStr_GPS_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_GPS;
	} else if (!safeStrCmp(KeyStr_SYNC_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DATASYNC;
	} else if (!safeStrCmp(KeyStr_HOTSPOT_Off, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_HOTSPOT;
	} else if (!safeStrCmp(KeyStr_Adjust_Bright, list_item->keyStr)) {
		vconf = VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_STATUS;
	}

	int err = vconf_set_bool(vconf, list_item->chk_status);
	if (0 != err) {	/* rollback */
		SETTING_TRACE_ERROR("Set vconf Failed");
		list_item->chk_status = !list_item->chk_status;
		elm_check_state_set(obj, list_item->chk_status);
	}
	if (!safeStrCmp(KeyStr_Adjust_Bright, list_item->keyStr) && ad->data_brightness) {
		if (0 == list_item->chk_status) {
			setting_disable_genlist_item(ad->data_brightness->item);
		} else {
			setting_enable_genlist_item(ad->data_brightness->item);
		}
	}

	return;
}
