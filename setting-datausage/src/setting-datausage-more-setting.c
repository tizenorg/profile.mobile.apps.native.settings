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

#include <setting-datausage-more-setting.h>

static int __setting_datausage_more_setting_create(void *cb);
static int __setting_datausage_more_setting_destroy(void *cb);
static int __setting_datausage_more_setting_update(void *cb);
static int __setting_datausage_more_setting_cleanup(void *cb);

setting_view setting_view_datausage_more_setting = {
	.create = __setting_datausage_more_setting_create,
	.destroy = __setting_datausage_more_setting_destroy,
	.update = __setting_datausage_more_setting_update,
	.cleanup = __setting_datausage_more_setting_cleanup,
};

static void __setting_datausage_more_setting_chk_btn_cb(void *data, Evas_Object * obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	//error check
	retm_if(data == NULL, "Data parameter is NULL");
	SettingDatausageUG *ad = NULL;

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	list_item->chk_status = elm_check_state_get(obj);	// for genlist update status

	int opposite  = (list_item->chk_status == 1) ? SETTING_ON_OFF_BTN_ON : SETTING_ON_OFF_BTN_OFF;

	int err = -1;
	int ret = -1;

	if (obj == ad->data_roaming->eo_check) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");
	} else if (obj == ad->restrict_bg->eo_check) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_RESTRICT_BG_DATA_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");
	} else if (obj == ad->show_wifi->eo_check) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SHOW_WIFI_USAGE_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");
	}
}


/* ***************************************************
**
** basic func
**
** **************************************************/

static int __setting_datausage_more_setting_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDatausageUG *ad = (SettingDatausageUG *) cb;
	Evas_Object *scroller;

	setting_push_layout_navi_bar_genlist(ad->win_main_layout,
					ad->win_get,
					_(SETTING_DATAUSAGE_MORE_SETTING_STR),
					_("IDS_COM_BODY_BACK"),
					NULL,
					__setting_datausage_more_setting_back_cb,
					NULL,
					ad, &scroller, ad->navibar);

	setting_retvm_if(scroller == NULL, SETTING_RETURN_FAIL, "scroller is NULL");

	Elm_Object_Item *item = NULL;;

	item = elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int value = 0;
	int err = 0;

	// Data roaming - ON/OFF
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);
	if (err != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->data_roaming = setting_create_Gendial_field_def(scroller, &(ad->itc_1text_1icon),
					     __setting_datausage_more_setting_mouse_up_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, value, SETTING_DATAUSAGE_DATA_ROAMING_STR,
					     NULL, __setting_datausage_more_setting_chk_btn_cb);
	if (ad->data_roaming) {
		ad->data_roaming->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_roaming is NULL");
		return SETTING_RETURN_FAIL;
	}

	// Restrict background data - ON/OFF
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_RESTRICT_BG_DATA_BOOL, &value, &err);
	if (err != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->restrict_bg = setting_create_Gendial_field_def(scroller, &(ad->itc_1text_1icon),
					     __setting_datausage_more_setting_mouse_up_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, value, SETTING_DATAUSAGE_RESTRICT_BACKGROUND_STR,
					     NULL, __setting_datausage_more_setting_chk_btn_cb);
	if (ad->restrict_bg) {
		ad->restrict_bg->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_roaming is NULL");
		return SETTING_RETURN_FAIL;
	}

	// Show Wi-Fi usage - ON/OFF
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SHOW_WIFI_USAGE_BOOL, &value, &err);
	if (err != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->show_wifi = setting_create_Gendial_field_def(scroller, &(ad->itc_1text_1icon),
					     __setting_datausage_more_setting_mouse_up_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, value, SETTING_DATAUSAGE_SHOW_WIFI_STR,
					     NULL, __setting_datausage_more_setting_chk_btn_cb);
	if (ad->show_wifi) {
		ad->show_wifi->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_roaming is NULL");
		return SETTING_RETURN_FAIL;
	}

	setting_view_datausage_more_setting.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_more_setting_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	elm_naviframe_item_pop(ad->navibar);
	setting_view_datausage_more_setting.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_more_setting_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_more_setting_cleanup(void *cb)
{
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	__setting_datausage_more_setting_destroy(cb);
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
**
** call back func
**
** **************************************************/
static void __setting_datausage_more_setting_back_cb(void *data, Evas_Object * obj, void *event_info)
{
	//error check
	ret_if(data == NULL);
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_view_change(&setting_view_datausage_more_setting, &setting_view_datausage_main, ad);
}

static void __setting_datausage_more_setting_mouse_up_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);

	setting_retm_if(data == NULL, "Data parameter is NULL");

	int opposite = !list_item->chk_status;
	int err = -1;
	int ret = -1;

	if (!safeStrCmp(SETTING_DATAUSAGE_DATA_ROAMING_STR, list_item->keyStr)) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		elm_check_state_set(list_item->eo_check, opposite);
		list_item->chk_status = opposite;
	} else if (!safeStrCmp(SETTING_DATAUSAGE_RESTRICT_BACKGROUND_STR, list_item->keyStr)) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_RESTRICT_BG_DATA_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		elm_check_state_set(list_item->eo_check, opposite);
		list_item->chk_status = opposite;
	} else if (!safeStrCmp(SETTING_DATAUSAGE_SHOW_WIFI_STR, list_item->keyStr)) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SHOW_WIFI_USAGE_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		elm_check_state_set(list_item->eo_check, opposite);
		list_item->chk_status = opposite;
	}
}
