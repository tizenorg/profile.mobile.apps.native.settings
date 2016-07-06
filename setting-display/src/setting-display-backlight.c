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
#include <setting-common-data-type.h>
#include <setting-common-data-slp-setting.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <setting-display-backlight.h>
#include <efl_extension.h>

static int setting_display_backlight_create(void *cb);
static int setting_display_backlight_destroy(void *cb);
static int setting_display_backlight_update(void *cb);
static int setting_display_backlight_cleanup(void *cb);

setting_view setting_view_display_backlight = {
	.create = setting_display_backlight_create,
	.destroy = setting_display_backlight_destroy,
	.update = setting_display_backlight_update,
	.cleanup = setting_display_backlight_cleanup, };

/* ***************************************************
 **
 **basic func
 **
 ****************************************************/
Eina_Bool _backlight_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	ui_app_exit();
	return EINA_TRUE;
}

static int setting_display_backlight_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = (SettingDisplay *)cb;
	Evas_Object *scroller = NULL, *rgd = NULL;
	Elm_Object_Item *nf_it = NULL;

	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	// Conformant
	Evas_Object *conform= elm_conformant_add(ad->md.win_main);
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->md.win_main, conform);
	evas_object_show(conform);

	/* navi frame */
	Evas_Object *navi = NULL;
	navi = elm_naviframe_add(conform);

	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_TRUE);
	elm_object_part_content_set(conform, "elm.swallow.content", navi);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	if (navi == NULL) {
		SETTING_TRACE(" *** elm_naviframe_add returns NULL *** ");
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;
	}
	evas_object_show(navi);
	ad->md.navibar_main = navi;

	Evas_Object *button = elm_button_add(ad->md.navibar_main);
	elm_object_style_set(button, NAVI_BACK_ARROW_BUTTON_STYLE);
	evas_object_smart_callback_add(button, "clicked",
			setting_display_backlight_click_softkey_back_cb,
			ad);
	evas_object_show(button);


	scroller = elm_genlist_add(navi);
	nf_it = elm_naviframe_item_push(navi, _(KeyStr_BacklightTime), button, NULL, scroller, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, _backlight_back_cb, ad);


	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
	/*essential to auto compute the height of genlist */
	elm_scroller_content_min_limit(scroller, EINA_FALSE, EINA_TRUE);
	evas_object_show(scroller);


	retvm_if(scroller == NULL, SETTING_RETURN_FAIL,
			"Cannot set genlist object as content of layout");

	rgd = elm_radio_add(scroller);
	elm_radio_value_set(rgd, -1);

	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			NULL);

	/* to do : radio menu */
	if (isEmulBin()) {
		ad->data_backlight_always_on = setting_create_Gendial_field_1radio(
				scroller, &itc_1text_1icon_2,
				setting_display_backlight_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1RADIO, rgd, 0, /* Always ON */
				"IDS_ST_BODY_ALWAYS_ON", NULL);

		if (ad->data_backlight_always_on) {
			ad->data_backlight_always_on->userdata = ad;
			__BACK_POINTER_SET(ad->data_backlight_always_on);
		} else {
			SETTING_TRACE_ERROR(
					"ad->data_backlight_always_on is NULL");
		}
	}
	ad->data_backlight_15sec = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 15, /* 15 sec */
			SETTING_DISPLAY_TIME_15_SEC_STR, NULL);
	if (ad->data_backlight_15sec) {
		ad->data_backlight_15sec->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_15sec);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_15sec is NULL");
	}

	ad->data_backlight_30sec = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 30, /* 30 sec */
			SETTING_DISPLAY_TIME_30_SEC_STR, NULL);
	if (ad->data_backlight_30sec) {
		ad->data_backlight_30sec->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_30sec);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_30sec is NULL");
	}

	ad->data_backlight_1min = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 60, /* 60 sec */
			SETTING_DISPLAY_TIME_1_MIN_STR, NULL);
	if (ad->data_backlight_1min) {
		ad->data_backlight_1min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_1min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_1min is NULL");
	}

	ad->data_backlight_2min = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 120, /* 2min */
			SETTING_DISPLAY_TIME_2_MIN_STR, NULL);
	if (ad->data_backlight_2min) {
		ad->data_backlight_2min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_2min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_2min is NULL");
	}

	ad->data_backlight_5min = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 300, /* 5min */
			SETTING_DISPLAY_TIME_5_MIN_STR, NULL);
	if (ad->data_backlight_5min) {
		ad->data_backlight_5min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_5min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_5min is NULL");
	}

	ad->data_backlight_10min = setting_create_Gendial_field_1radio(scroller,
			&itc_1text_1icon_2,
			setting_display_backlight_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1RADIO, rgd, 600, /* 10min */
			SETTING_DISPLAY_TIME_10_MIN_STR, NULL);
	if (ad->data_backlight_10min) {
		ad->data_backlight_10min->userdata = ad;
		__BACK_POINTER_SET(ad->data_backlight_10min);
	} else {
		SETTING_TRACE_ERROR("ad->data_backlight_10min is NULL");
	}

	setting_update_chk_status(rgd, INT_SLP_SETTING_LCD_TIMEOUT_NORMAL);

	setting_view_display_backlight.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_backlight_destroy(void *cb)
{
	SettingDisplay *ad = (SettingDisplay *)cb;

	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_display_backlight.is_create),
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	if (ad->md.ly_main != NULL) {
		evas_object_del(ad->md.ly_main);
		ad->md.ly_main = NULL;
	}

	setting_view_display_backlight.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_backlight_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_backlight_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/
static void setting_display_backlight_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	Setting_GenGroupItem_Data *list_item = NULL;
	int val, list_val, old_val;
	int err = 0;

	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	elm_genlist_item_selected_set(item, 0);

	list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	SETTING_TRACE("chk_status[%d]", list_item->chk_status);

	list_val = list_item->chk_status;
	old_val = elm_radio_value_get(list_item->eo_check);

	setting_get_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL, &val, &err);
	if (list_val == old_val && list_val == val) {
		SETTING_TRACE_DEBUG("Selected same type");
		return;
	}

	if (setting_set_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL,
			list_item->chk_status, &err) != 0) {
		SETTING_TRACE_ERROR("set vconf failed");
		elm_radio_value_set(list_item->eo_check, val);
		return;
	}

	elm_radio_value_set(list_item->eo_check, list_item->chk_status);
}

static void setting_display_backlight_click_softkey_back_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "[Setting > Security] Data parameter is NULL");

	/* Send destroy request */
	ui_app_exit();


	return;
}
