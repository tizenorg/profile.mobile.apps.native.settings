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
#include "setting-accessibility-screen-reader.h"

static int setting_accessibility_main_create(void *cb);
static int setting_accessibility_main_destroy(void *cb);
static int setting_accessibility_main_update(void *cb);
static int setting_accessibility_main_cleanup(void *cb);

setting_view setting_view_accessibility_main = {
		.create = setting_accessibility_main_create,
		.destroy = setting_accessibility_main_destroy,
		.update = setting_accessibility_main_update,
		.cleanup = setting_accessibility_main_cleanup, };

static void setting_accessibility_screen_reader_key_change_vconf_cb(
		keynode_t *key, void *user_data)
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *screen_reader_item = user_data;
	if (screen_reader_item->sub_desc)
		free(screen_reader_item->sub_desc);
	screen_reader_item->sub_desc = strdup(vconf_keynode_get_bool(key) ?
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_ON) :
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_OFF));

	elm_genlist_item_update(screen_reader_item->item);

	SETTING_TRACE_END;
}

/**
 * @brief Do process when clicking '<-' button
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 */
static void _softkey_back_click_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAccessibility *ad = (SettingAccessibility *)data;
	ret_if(!ad || ad->empty_flag);

	vconf_ignore_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
			setting_accessibility_screen_reader_key_change_vconf_cb);
	if (ad->md.win_main)
		elm_win_lower(ad->md.win_main);

	SETTING_TRACE_END;
	return;
}

static void setting_accessibility_vision_screen_reader_mouse_up_Gendial_list_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	SettingAccessibility *ad = (SettingAccessibility *)data;
	elm_genlist_item_selected_set(item, 0);
	setting_accessibility_screen_reader_page_create(ad);
	SETTING_TRACE_END;
}

/**
 * @brief create main view genlist items
 *
 * @param data application data
 *
 * @return FALSE for call it once and then destory the timer, TRUE for always
 *	call it when the timer is reached.
 */
int setting_accessibility_main_generate_genlist(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingAccessibility *ad = (SettingAccessibility *)data;
	Evas_Object *scroller = ad->md.genlist;
	(void)setting_create_Gendial_field_titleItem(scroller,
			&(itc_group_item),
			_(DEVOPTION_STR_ACCESSIBILITY_VISION), NULL);

	int screen_reader = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &screen_reader);

	Setting_GenGroupItem_Data *screen_reader_item = setting_create_Gendial_field_def(
			scroller, &itc_2text,
			setting_accessibility_vision_screen_reader_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID, NULL,
			NULL, 1, _(DEVOPTION_STR_ACCESSIBILITY_SCREENREADER),
			screen_reader ? _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_ON) :
					_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_OFF),
			NULL);
	int vconf_ret = vconf_notify_key_changed(
			VCONFKEY_SETAPPL_ACCESSIBILITY_TTS,
			setting_accessibility_screen_reader_key_change_vconf_cb,
			screen_reader_item);
	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS)");
		return SETTING_RETURN_FAIL;
	}
	screen_reader_item->userdata = ad;
	__BACK_POINTER_SET(screen_reader_item);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingAccessibility *ad = (SettingAccessibility *)cb;
	retv_if(ad == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	view_init(&ad->md, _("IDS_ST_BODY_ACCESSIBILITY"));
	elm_object_style_set(ad->md.genlist, "dialogue");

	setting_create_navi_bar_buttons(
			_("IDS_ST_BODY_ACCESSIBILITY"),
			_("IDS_ST_BUTTON_BACK"),
			_softkey_back_click_cb,
			ad, ad->md.genlist, ad->md.navibar_main,
			NULL);
	ad->md.navibar_main_it = elm_naviframe_top_item_get(ad->md.navibar_main);

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

	setting_view_accessibility_main.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_accessibility_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

