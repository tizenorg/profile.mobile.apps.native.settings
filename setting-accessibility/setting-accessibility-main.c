/*
 * accessibility
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Contact: Tomasz Olszak <t.olszak@samsung.com>
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

#include "setting-accessibility-main.h"
#include "app_control.h"


static int setting_accessibility_main_create(void *cb);
static int setting_accessibility_main_destroy(void *cb);
static int setting_accessibility_main_update(void *cb);
static int setting_accessibility_main_cleanup(void *cb);

setting_view setting_view_accessibility_main = {
	.create = setting_accessibility_main_create,
	.destroy = setting_accessibility_main_destroy,
	.update = setting_accessibility_main_update,
	.cleanup = setting_accessibility_main_cleanup,
};

/**
* @brief Do process when clicking '<-' button
*
* @param data application context
* @param obj evas object
* @param event_info event type
*/
static Eina_Bool
setting_accessibility_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) data;
	if (ad->empty_flag) {
		return EINA_FALSE;
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;
}

static void screen_reader_key_change_vconf_cb(keynode_t *node, void *user_data)
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *screenReaderItem = user_data;
	//SETTING_TRACE_DEBUG("check_status: %i, node->value.i: %i", screenReaderItem->chk_status, node->value.i);
	/* I don't know why following code does not update check box to checked/unchecked */
	//setting_update_gl_item_chk_status(screenReaderItem, node->value.b ? 1 : 0);
	elm_genlist_item_update(screenReaderItem->item);
	SETTING_TRACE_END;
}

static void setting_accessibility_main_chk_magnifier_cb(void *data,
                                                        Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingAccessibilityUG *ad = list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);   /* for genlist update status */

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	const char magnifierAppId[]  = "com.samsung.xmagnifier";
	if (list_item->chk_status) {
		app_control_h appControl;
		app_control_create(&appControl);
		app_control_set_app_id(appControl, magnifierAppId);
		SETTING_TRACE_DEBUG("Trying to launch %s app.", magnifierAppId);
		if (app_control_send_launch_request(appControl, NULL, NULL) != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_WARNING("Failed to launch %s app.", magnifierAppId);
		}
		app_control_destroy(appControl);
	} else {
		vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_ZOOM, 0);
	}

	SETTING_TRACE_END;
}

static void setting_accessibility_magnifier_mouse_up_Gendial_list_cb(void *data,
                                                                     Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;

	elm_genlist_item_selected_set(item, 0);

	SETTING_TRACE_END;
}

static void setting_accessibility_main_chk_screenreader_vconf_update(int state)
{
	vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, state ? 1 : 0);
}

static void setting_accessibility_main_chk_screenreader_cb(void *data,
                                                           Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingAccessibilityUG *ad = list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);   /* for genlist update status */

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_main_chk_screenreader_vconf_update(list_item->chk_status);
	SETTING_TRACE_END;
}

static void setting_accessibility_screenreader_mouse_up_Gendial_list_cb(void *data,
                                                                        Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;

	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_update_gl_item_chk_status(list_item, list_item->chk_status ? 0 : 1);
	setting_accessibility_main_chk_screenreader_vconf_update(list_item->chk_status ? 1 : 0);
	SETTING_TRACE_END;
}



/**
 * @brief create main view genlist items
 *
 * @param data application data
 *
 * @return FALSE for call it once and then destory the timer, TRUE for always call it when the timer is reached.
 */
int setting_accessibility_main_generate_genlist(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) data;
	/*bool is_emul_bin = isEmulBin(); */

	Evas_Object *scroller = ad->genlist;

	ad->screenreader_checkbox =
	    setting_create_Gendial_field_def(scroller, &itc_1text_1icon,
	                                     setting_accessibility_screenreader_mouse_up_Gendial_list_cb,
	                                     ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                     NULL, 1, DEVOPTION_STR_ACCESSIBILITY_SCREENREADER,
	                                     NULL, setting_accessibility_main_chk_screenreader_cb);

	if (ad->screenreader_checkbox) {
		ad->screenreader_checkbox->userdata = ad;
		__BACK_POINTER_SET(ad->screenreader_checkbox);
		int screen_reader = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &screen_reader);
		ad->screenreader_checkbox->chk_status = screen_reader ? 1 : 0;
		/*---------------------------------------------------------------------------------------------- */
		int vconf_ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
		                                         screen_reader_key_change_vconf_cb, &ad->screenreader_checkbox);
		if (vconf_ret != 0) {
			SETTING_TRACE("FAIL: vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS)");
			return SETTING_RETURN_FAIL;
		}

	} else {
		SETTING_TRACE_ERROR("ad->screenreader_checkbox is NULL");
		return SETTING_RETURN_FAIL;
	}
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}


static int setting_accessibility_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) cb;
	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
	         "Cannot set scroller object  as contento of layout");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller);	/* first to clear list */
	ad->genlist = scroller;
	ad->ly_main =
	    setting_create_layout_navi_bar(ad->win_main_layout, ad->win_get,
	                                   KeyStr_Accessibility,
	                                   _("IDS_ST_BUTTON_BACK"), NULL, NULL,
	                                   (setting_call_back_func)setting_accessibility_main_click_softkey_back_cb,
	                                   NULL, NULL, ad, scroller,
	                                   &ad->navi_bar, NULL);
	ad->navi_item = elm_naviframe_top_item_get(ad->navi_bar);

	setting_accessibility_main_generate_genlist((void *)ad);
	setting_view_accessibility_main.is_create = 1;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) cb;
	setting_view_accessibility_main.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) cb;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) cb;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}


