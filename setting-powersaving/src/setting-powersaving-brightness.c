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

#include <setting-powersaving-brightness.h>

#define Left_Bright_Icon IMG_BRIGTHNESS_LEFT
#define Right_Bright_Icon IMG_BRIGHTNESS_RIGHT

static int setting_powersaving_brightness_create(void *cb);
static int setting_powersaving_brightness_destroy(void *cb);
static int setting_powersaving_brightness_update(void *cb);
static int setting_powersaving_brightness_cleanup(void *cb);

setting_view setting_view_powersaving_brightness = {
	.create = setting_powersaving_brightness_create,
	.destroy = setting_powersaving_brightness_destroy,
	.update = setting_powersaving_brightness_update,
	.cleanup = setting_powersaving_brightness_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_powersaving_brightness_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;

	Evas_Object *navi_bar = ad->navi_bar;
	Evas_Object *sub_layout = elm_layout_add(navi_bar);
	elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME,
			    "search_layout");

	Evas_Object *genlist = elm_genlist_add(ad->win_main_layout);
	elm_object_style_set(genlist, "dialogue");
	retvm_if(genlist == NULL, SETTING_RETURN_FAIL,
		 "Cannot set scroller object as contento of layout");
	elm_genlist_clear(genlist);	/* first to clear list */

	setting_push_layout_navi_bar(_(KeyStr_Brightness),
				     _("IDS_COM_BODY_BACK"), NULL, NULL,
				     setting_powersaving_brightness_click_softkey_cancel_cb,
				     NULL,
				     NULL, ad, sub_layout, navi_bar, NULL);;
	elm_object_part_content_set(sub_layout, "search_list2", genlist);

	int auto_value = 0;
	int value = 10;

	Elm_Object_Item *item = NULL;;

	item =
	    elm_genlist_item_append(genlist, &itc_seperator, NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	// [control] AUTOMATIC - boolean - vconf
	vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_AUTO_STATUS, &auto_value);	//if failed, keep intial value
	SETTING_TRACE("auto_value:%d", auto_value);

	// [UI] AUTOMATIC - boolean
	ad->data_br_auto =
	    setting_create_Gendial_field_def(genlist, &itc_1text_1icon,
					     setting_powersaving_brightness_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, auto_value,
					     "IDS_COM_BODY_AUTOMATIC", NULL,
					     setting_powersaving_brightness_chk_btn_cb);
	if (ad->data_br_auto) {
		__BACK_POINTER_SET(ad->data_br_auto);
		ad->data_br_auto->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_br_auto is NULL");
	}

	/* check manual */
	vconf_get_int(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_VALUE, &value);	//if failed, keep intial value
	SETTING_TRACE("auto_value:%d", value);
	ad->data_br_sli =
	    setting_create_Gendial_field_def(genlist, &itc_1icon, NULL,
					     NULL, SWALLOW_Type_LAYOUT_SLIDER,
					     Left_Bright_Icon,
					     Right_Bright_Icon, value, NULL,
					     NULL,
					     setting_powersaving_birghtness_bright_slider_value_change_cb);

	if (ad->data_br_sli) {
		__BACK_POINTER_SET(ad->data_br_sli);

		ad->data_br_sli->win_main = ad->win_main_layout;
		ad->data_br_sli->evas = ad->evas;
		ad->data_br_sli->isIndicatorVisible = true;
		ad->data_br_sli->slider_min = 1;
		ad->data_br_sli->slider_max = 100;
		ad->data_br_sli->userdata = ad;

	} else {
		SETTING_TRACE_ERROR("ad->data_br_sli is NULL");
	}

	if (auto_value && ad->data_br_sli) {
		setting_disable_genlist_item(ad->data_br_sli->item);
	}

	setting_view_powersaving_brightness.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_brightness_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;
	elm_naviframe_item_pop(ad->navi_bar);

	setting_view_powersaving_brightness.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_brightness_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;

}

static int setting_powersaving_brightness_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_powersaving_brightness_destroy(cb);

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
setting_powersaving_brightness_click_softkey_cancel_cb(void *data,
						       Evas_Object *obj,
						       void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	setting_view_change(&setting_view_powersaving_brightness,
			    &setting_view_powersaving_customed, ad);
}

static void
setting_powersaving_brightness_chk_btn_cb(void *data, Evas_Object *obj,
					  void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingPowersavingUG *ad = list_item->userdata;
	list_item->chk_status = elm_check_state_get(obj);/*  for genlist update status */

	int err;
	// enable /disable toggle button
	if (list_item->chk_status) {
		setting_disable_genlist_item(ad->data_br_sli->item);
	} else {
		setting_enable_genlist_item(ad->data_br_sli->item);
	}

	err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_AUTO_STATUS,
	     			list_item->chk_status);

	return;
}

/**
 *[UI] 'Automatic' toggle
 */
static void setting_powersaving_brightness_mouse_up_Gendial_list_cb(void *data,
								    Evas_Object
								 *obj,
								    void
								    *event_info)
{
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	retm_if(data == NULL, "Invalid argument: data is NULL");
	SettingPowersavingUG *ad = data;

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	int old_status = elm_check_state_get(list_item->eo_check);
	/* new status */
	list_item->chk_status = !old_status;
	elm_check_state_set(list_item->eo_check, list_item->chk_status);
	int err;

	if (0 == safeStrCmp("IDS_COM_BODY_AUTOMATIC", list_item->keyStr)) {
		err =
		    vconf_set_bool
		    (VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_AUTO_STATUS,
		     list_item->chk_status);
		if (err < 0) {
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}

		if (list_item->chk_status) {
			setting_disable_genlist_item(ad->data_br_sli->item);
		} else {
			setting_enable_genlist_item(ad->data_br_sli->item);
		}
	}
}

static void
setting_powersaving_birghtness_bright_slider_value_change_cb(void *data,
							     Evas_Object *obj,
							     void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	int value = 0;
	SettingPowersavingUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;

	value = elm_check_state_get(ad->data_br_auto->eo_check);
	if (value == 1) { /* it is automatic setting mode, */
		/* do nothing */
		return;
	}

	double val = elm_slider_value_get(obj);
	value = (int)(val + 0.5);
	SETTING_TRACE("value:%d", value);

	int ret =
	    vconf_set_int(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_VALUE, value);
	if (ret < 0) //rollback
	{
		elm_check_state_set(ad->data_br_auto->eo_check,
				    ad->data_br_sli->chk_status);
		return;
	}
	ad->data_br_sli->chk_status = value;
}
