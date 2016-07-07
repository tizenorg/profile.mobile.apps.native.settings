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

#include "setting-accessibility-screen-reader.h"
#include "setting-accessibility-screen-reader-settings.h"

static void screen_reader_key_change_vconf_cb(keynode_t *node, void *user_data)
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *screenReaderItem = user_data;
	/*SETTING_TRACE_DEBUG("check_status: %i, node->value.i: %i",
	 *  screenReaderItem->chk_status, node->value.i); */
	/* I don't know why following code does not update check box to
	 * checked/unchecked */
	/* setting_update_gl_item_chk_status(screenReaderItem,
	 * node->value.b ? 1 : 0); */
	elm_genlist_item_update(screenReaderItem->item);
	SETTING_TRACE_END;
}

static Eina_Bool __setting_accessibility_screen_reader_page_hide(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, EINA_TRUE);
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	vconf_ignore_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
			screen_reader_key_change_vconf_cb);
	elm_naviframe_item_pop(ad->md.navibar_main);
	SETTING_TRACE_END;
	return EINA_TRUE;
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
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_main_chk_screenreader_vconf_update(
			list_item->chk_status);
	SETTING_TRACE_END;
}
static void setting_accessibility_screenreader_settings_mouse_up_Gendial_list_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;

	elm_genlist_item_selected_set(item, 0);

	setting_accessibility_screen_reader_settings_page_create(ad);

	SETTING_TRACE_END;
}

static void setting_accessibility_screenreader_mouse_up_Gendial_list_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;

	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_update_gl_item_chk_status(list_item,
			list_item->chk_status ? 0 : 1);
	setting_accessibility_main_chk_screenreader_vconf_update(
			list_item->chk_status ? 1 : 0);
	SETTING_TRACE_END;
}

void setting_accessibility_screen_reader_page_create(
		SettingAccessibilityUG *data)
{
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Evas_Object *genlist = elm_genlist_add(ad->md.navibar_main);
	retm_if(genlist == NULL,
			"Cannot set genlist object as content of layout");
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_object_style_set(genlist, "dialogue");
	elm_genlist_clear(genlist);

	Setting_GenGroupItem_Data *screenreader_checkbox = setting_create_Gendial_field_def(
			genlist, &itc_1text_1icon,
			setting_accessibility_screenreader_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_1ICON_1RADIO, NULL,
			NULL, 1, _(DEVOPTION_STR_ACCESSIBILITY_SCREENREADER),
			NULL, setting_accessibility_main_chk_screenreader_cb);
	screenreader_checkbox->userdata = ad;
	__BACK_POINTER_SET(screenreader_checkbox);
	int screen_reader = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &screen_reader);
	screenreader_checkbox->chk_status = screen_reader ? 1 : 0;
	/*---------------------------------------------------------------------------------------------- */
	int vconf_ret = vconf_notify_key_changed(
			VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
			screen_reader_key_change_vconf_cb,
			screenreader_checkbox);
	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS)");
		return;
	}

	Setting_GenGroupItem_Data *multiline_screen_reader_comment = setting_create_Gendial_field_def(
			genlist, &itc_multiline_text,
			NULL, ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0,
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_DESCRIPTION),
			NULL, NULL);
	multiline_screen_reader_comment->userdata = ad;
	__BACK_POINTER_SET(multiline_screen_reader_comment);

	Setting_GenGroupItem_Data *screen_reader_settings = setting_create_Gendial_field_def(
			genlist, &itc_1text,
			setting_accessibility_screenreader_settings_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0,
			DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS,
			NULL, NULL);
	screen_reader_settings->userdata = ad;
	__BACK_POINTER_SET(screen_reader_settings);

	Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->md.navibar_main,
			_(DEVOPTION_STR_ACCESSIBILITY_SCREENREADER), NULL, NULL,
			genlist, NULL);
	elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE,
			EINA_TRUE);

	elm_naviframe_item_pop_cb_set(navi_it,
			(Elm_Naviframe_Item_Pop_Cb)__setting_accessibility_screen_reader_page_hide,
			ad);
}
