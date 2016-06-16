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

#include <vconf.h>
#include <vconf-internal-keys.h>
#include <efl_extension.h>
#include <tts_setting.h>

#include "setting-accessibility-screen-reader-settings.h"

#define POPUP_CONTENT_BASE_SIZE 126

typedef struct {
	char *label;
	char *vconf_key;
	int *status;
} vconf_check_data_s;

vconf_check_data_s vconf_check_data[] = {
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_TIME,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_TIME,
		.status = NULL },
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_BATTERY,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_BATTERY,
		.status = NULL },
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_BLUETOOTH,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_BLUETOOTH,
		.status = NULL },
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_SIGNAL_STRENGTH,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_SIGNAL_STRENGHT,
		.status = NULL },
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_DATE,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_DATE,
		.status = NULL },
	{
		.label = DEVOPTION_STR_ACCESSIBILITY_INDICATOR_UNREAD_NOTIFICATIONS,
		.vconf_key = VCONFKEY_SETAPPL_ACCESSIBILITY_TTS_INDICATOR_INFORMATION_MISSED_EVENTS,
		.status = NULL } };
static Eina_List *items = NULL;

static Setting_GenGroupItem_Data *speech_rate_list_item = NULL;

static Eina_Bool __setting_accessibility_screen_reader_settings_page_hide(
		void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, EINA_TRUE);
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) data;
	if (0 != tts_setting_unset_speed_changed_cb()) {
		SETTING_TRACE("Fail to unset speed changed cb");
	}

	if (0 != tts_setting_unset_voice_changed_cb()) {
		SETTING_TRACE("Fail to unset voice changed cb");
	}

	if (0 != tts_setting_finalize()) {
		LOGE("Fail to setting finalize");
		return -1;
	}
	elm_naviframe_item_pop(ad->navi_bar);
	SETTING_TRACE_END;
	return EINA_TRUE;
}

static Eina_Bool _vconf_key_value_get(char *key)
{
	retvm_if(!key, EINA_FALSE, "Key is NULL");
	int value = 0;
	int ret = vconf_get_bool(key, &value);
	if (ret != 0) {
		LOGE("Failed to get vconf bool value %s. Error: %d", key, ret);
		return EINA_FALSE;
	}
	return (value != 0);
}

static void _vconf_key_value_set(char *key, int value)
{
	retm_if(!key, "key == NULL");

	int ret = vconf_set_bool(key, value);

	if (ret != 0) {
		LOGE("Failed to set vconf key %s. Error: %d", key, ret);
		return;
	}
}

static void _vconf_key_foreach_check_set(void)
{
	int i;
	int size = sizeof(vconf_check_data) / sizeof(vconf_check_data[0]);
	int status = 0;
	char *key = NULL;

	for (i = 0; i < size; ++i) {
		key = vconf_check_data[i].vconf_key;
		status = *(vconf_check_data[i].status);
		_vconf_key_value_set(key, status);
	}
}

static void _cancel_button_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = (Evas_Object *) data;
	retm_if(!popup, "popup == NULL");
	evas_object_del(popup);
	SETTING_TRACE_END;
}

static void _done_button_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = (Evas_Object *) data;
	retm_if(!popup, "popup == NULL");
	evas_object_del(popup);

	_vconf_key_foreach_check_set();
	SETTING_TRACE_END;
}

static void _check_state_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	if (!data) {
		LOGD("data == NULL");
		return;
	}

	Setting_GenGroupItem_Data *item_data =
			(Setting_GenGroupItem_Data *) data;
	item_data->chk_status = elm_check_state_get(obj);
	LOGE("Check changed: %s; status: %d, %p", item_data->keyStr,
			item_data->chk_status, &item_data->chk_status);

	SETTING_TRACE_END;
}

static void _speech_rate_popup_done_button_clicked_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *slider;
	Evas_Object *layout;
	int val;
	Evas_Object *popup = (Evas_Object *)data;
	retm_if(!popup, "popup == NULL");
	layout = elm_object_content_get(popup);
	slider = elm_object_part_content_get(layout, "slider");

	val = (int)elm_slider_value_get(slider);
	if (0 != tts_setting_set_speed(val))
		SETTING_TRACE("Setting speed to tts engine failed");

	evas_object_del(popup);
	SETTING_TRACE_END;
}

static void _list_item_clicked_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	retm_if(!item, "item == NULL");

	elm_genlist_item_selected_set(item, EINA_FALSE);

	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) elm_object_item_data_get(
			item);
	retm_if(!list_item, "list_item == NULL");

	LOGI("ITEM SELECTED: %s", list_item->keyStr);
	setting_update_gl_item_chk_status(list_item, !list_item->chk_status);

	SETTING_TRACE_END;
}

static void _on_popup_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *)data;
	char buf[256];
	int speed_val = -1;
	if (speech_rate_list_item) {
		if (0 != tts_setting_get_speed(&speed_val)) {
			SETTING_TRACE("Failed to get speed");
		}
		snprintf(buf, sizeof(buf), "%d", speed_val);
		SETTING_TRACE("Updating speed to %s", buf);
		G_FREE(speech_rate_list_item->sub_desc);
		speech_rate_list_item->sub_desc = (char *) g_strdup (buf);
		elm_genlist_item_update(speech_rate_list_item->item);
	}
	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_settings_speech_rate_popup_create(
			void *data, void *item)
{
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Evas_Object *popup;
	Evas_Object *layout;
	Evas_Object *slider;
	Evas_Object *btn1, *btn2;
	int val = -1;

	popup = elm_popup_add(ad->win_main_layout);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	/* eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
	 *  eext_popup_back_cb, NULL); */
	elm_object_part_text_set(popup, "title,text",
									 _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH_RATE));
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	/* layout */
	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, SETTINGS_EDJ, "popup_text_slider_view_layout");
	elm_object_part_text_set(layout, "elm.text.description",
									 _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH_RATE_POPUP_DESCRIPTION));
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_content_set(popup, layout);

	/* slider */
	slider = elm_slider_add(layout);
	elm_slider_indicator_show_set(slider, EINA_TRUE);
	evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, EVAS_HINT_FILL);

	elm_slider_indicator_format_set(slider, "%1.0f");
	elm_slider_min_max_set(slider, 1, 15);
	if (0 != tts_setting_get_speed(&val)) {
		SETTING_TRACE("Fail to get speed");
	}
	elm_slider_step_set(slider, 2);
	elm_slider_value_set(slider, val);
	elm_object_part_content_set(layout, "slider", slider);

	/* ok button */
	btn1 = elm_button_add(popup);
	elm_object_style_set(btn1, "bottom");
	elm_object_text_set(btn1, _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH_RATE_POPUP_CANCEL));
	elm_object_part_content_set(popup, "button1", btn1);
	evas_object_smart_callback_add(btn1, "clicked", _cancel_button_clicked_cb, popup);

	btn2 = elm_button_add(popup);
	elm_object_style_set(btn2, "bottom");
	elm_object_text_set(btn2, _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH_RATE_POPUP_DONE));
	elm_object_part_content_set(popup, "button2", btn2);
	evas_object_smart_callback_add(btn2, "clicked", _speech_rate_popup_done_button_clicked_cb, popup);

	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, _on_popup_del, item);

	evas_object_show(popup);

}

static void __tts_setting_speed_changed_cb(int speed, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("=== Speed changed to (%d)", speed);
	Setting_GenGroupItem_Data *speech_rate_item = (Setting_GenGroupItem_Data *) user_data;
	char buf[256];

	snprintf(buf, sizeof(buf), "%d", speed);
	elm_object_item_part_text_set(speech_rate_item->item, "elm.text.2", buf);
	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_settings_speech_rate_mouse_up_Gendial_list_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;

	elm_genlist_item_selected_set(item, EINA_FALSE);
	setting_accessibility_screen_reader_settings_speech_rate_popup_create(ad, item);

	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_keyboard_feedback_vconf_update(int state)
{
	vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK, state ? 1 : 0);
}

static void setting_accessibility_screen_reader_keyboard_feedback_check_update(void *data,
														   Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_screen_reader_keyboard_feedback_vconf_update(
			list_item->chk_status);
	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_settings_keyboard_feedback_mouse_up_Gendial_list_cb(
			void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	elm_genlist_item_selected_set(item, EINA_FALSE);
	setting_update_gl_item_chk_status(list_item, list_item->chk_status ? 0 : 1);
	setting_accessibility_screen_reader_keyboard_feedback_vconf_update(list_item->chk_status ? 1 : 0);
}

static void setting_accessibility_screen_reader_speak_while_screen_off_vconf_update(int state)
{
	vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK, state ? 1 : 0);
}

static void setting_accessibility_screen_reader_speak_while_screen_off_check_update(void *data,
														   Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_screen_reader_speak_while_screen_off_vconf_update(
			list_item->chk_status);
	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_settings_speak_while_screen_off_mouse_up_Gendial_list_cb(
			void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	elm_genlist_item_selected_set(item, EINA_FALSE);
	setting_update_gl_item_chk_status(list_item, list_item->chk_status ? 0 : 1);
	setting_accessibility_screen_reader_speak_while_screen_off_vconf_update(list_item->chk_status ? 1 : 0);
}

static void setting_accessibility_screen_reader_hints_vconf_update(int state)
{
	vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_DESCRIPTION, state ? 1 : 0);
}

static void setting_accessibility_screen_reader_hints_check_update(void *data,
														   Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_screen_reader_hints_vconf_update(
			list_item->chk_status);
	SETTING_TRACE_END;
}

static void setting_accessibility_screen_reader_settings_hints_mouse_up_Gendial_list_cb(
			void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	elm_genlist_item_selected_set(item, EINA_FALSE);
	setting_update_gl_item_chk_status(list_item, list_item->chk_status ? 0 : 1);
	setting_accessibility_screen_reader_hints_vconf_update(list_item->chk_status ? 1 : 0);
}

static void setting_accessibility_screen_reader_vibration_feedback_vconf_update(int state)
{
	vconf_set_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_HAPTIC, state ? 1 : 0);
}

static void setting_accessibility_screen_reader_vibration_feedback_check_update(void *data,
														   Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	SETTING_TRACE_DEBUG("check_status: %i", list_item->chk_status);
	setting_accessibility_screen_reader_vibration_feedback_vconf_update(
			list_item->chk_status);
	SETTING_TRACE_END;
}


static void setting_accessibility_screen_reader_settings_vibration_feedback_mouse_up_Gendial_list_cb(
			void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *)data;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	elm_genlist_item_selected_set(item, EINA_FALSE);
	setting_update_gl_item_chk_status(list_item, list_item->chk_status ? 0 : 1);
	setting_accessibility_screen_reader_vibration_feedback_vconf_update(list_item->chk_status ? 1 : 0);
}

static void setting_accessibility_screen_reader_settings_status_bar_info_mouse_up_Gendial_list_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) data;
	Evas_Object *popup;
	Evas_Object *box;
	Evas_Object *genlist;
	Setting_GenGroupItem_Data *screenreader_checkbox = NULL;
	int i;
	int size = sizeof(vconf_check_data) / sizeof(vconf_check_data[0]);
	int status;

	elm_genlist_item_selected_set(item, EINA_FALSE);

	popup = elm_popup_add(ad->win_main_layout);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	/* eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
	 *  eext_popup_back_cb, NULL); */
	elm_object_part_text_set(popup, "title,text",
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_STATUS_BAR_INFORMATION));
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	/* box */
	box = elm_box_add(popup);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	/* genlist */
	genlist = elm_genlist_add(box);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL,
			EVAS_HINT_FILL);

	for (i = 0; i < size; ++i) {
		status = _vconf_key_value_get(vconf_check_data[i].vconf_key);

		screenreader_checkbox = setting_create_Gendial_field_def(
				genlist, &itc_1text_1icon,
				_list_item_clicked_cb, ad,
				SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, status, _(vconf_check_data[i].label),
				NULL, _check_state_changed_cb);

		vconf_check_data[i].status = &screenreader_checkbox->chk_status;
		items = eina_list_append(items, screenreader_checkbox);
	}

	elm_scroller_movement_block_set(genlist,
			ELM_SCROLLER_MOVEMENT_BLOCK_VERTICAL
			| ELM_SCROLLER_MOVEMENT_BLOCK_HORIZONTAL);
	elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF,
			ELM_SCROLLER_POLICY_OFF);

	evas_object_show(genlist);
	elm_box_pack_end(box, genlist);
	evas_object_size_hint_min_set(box, -1,
			ELM_SCALE_SIZE(POPUP_CONTENT_BASE_SIZE) * size);
	evas_object_size_hint_max_set(box, -1,
			ELM_SCALE_SIZE(POPUP_CONTENT_BASE_SIZE) * size);
	elm_object_content_set(popup, box);

	const char *btnCancelText = "Cancel";
	const char *btnDoneText = "Done";

	Evas_Object *btnCancel = setting_create_button(popup, btnCancelText,
			NULL, _cancel_button_clicked_cb, popup);
	elm_object_style_set(btnCancel, "popup");
	elm_object_part_content_set(popup, _("button1"), btnCancel);
	evas_object_data_set(popup, _("button1"), btnCancelText);

	Evas_Object *btnDone = setting_create_button(popup, btnDoneText, NULL,
			_done_button_clicked_cb, popup);
	elm_object_style_set(btnDone, "popup");

	elm_object_part_content_set(popup, _("button2"), btnDone);
	evas_object_data_set(popup, _("button2"), btnDoneText);

	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK,
			_cancel_button_clicked_cb, popup);

	evas_object_show(popup);

	SETTING_TRACE_END;
}

void setting_accessibility_screen_reader_settings_page_create(
		SettingAccessibilityUG *data)
{
	SETTING_TRACE_BEGIN;
	SettingAccessibilityUG *ad = (SettingAccessibilityUG *) data;
	char buf[256];
	int s_speed = -1;
	int ret = 0;
	int vconf_val = 0;
	int vconf_ret;

	Evas_Object *genlist = elm_genlist_add(ad->navi_bar);
	retm_if(genlist == NULL,
			"Cannot set genlist object as content of layout");
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_object_style_set(genlist, "dialogue");
	elm_genlist_clear(genlist);

	SETTING_TRACE("Creating items");

	if (0 != tts_setting_initialize()) {
		SETTING_TRACE("Fail to setting initialize");
	}
	SETTING_TRACE("tts initialized");
	if (0 != tts_setting_get_speed(&s_speed)) {
		SETTING_TRACE("Fail to get speed");
	}
	snprintf(buf, sizeof(buf), "%d", s_speed);
	SETTING_TRACE("TTS rate is %d", s_speed);
// Item 1: Speech Rate

	Setting_GenGroupItem_Data *speech_rate_item = setting_create_Gendial_field_def(
			 genlist, &itc_2text,
			 setting_accessibility_screen_reader_settings_speech_rate_mouse_up_Gendial_list_cb,
			 ad, SWALLOW_Type_INVALID, NULL,
			 NULL, 1,
			 _(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH_RATE),
			 buf,
			 NULL);
	tts_setting_set_speed_changed_cb(__tts_setting_speed_changed_cb, speech_rate_item);
	speech_rate_item->userdata = ad;
	__BACK_POINTER_SET(speech_rate_item);
	speech_rate_list_item = speech_rate_item;
	SETTING_TRACE("Speech rate Item added");

// Item 2 : Keyboard feedback
	vconf_ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK, &vconf_val);
	if (vconf_ret != 0) {
	LOGE("Failed to get vconf bool value "
				"VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK. Error: %d", vconf_ret);
	}

	Setting_GenGroupItem_Data *keyboard_feedback_item = setting_create_Gendial_field_def(
				genlist, &itc_multiline_2text,
				setting_accessibility_screen_reader_settings_keyboard_feedback_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, vconf_val ? 1 : 0,
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_KEYBOARD_FEEDBACK),
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_KEYBOARD_FEEDBACK_INFO),
				setting_accessibility_screen_reader_keyboard_feedback_check_update);

	keyboard_feedback_item->userdata = ad;
	__BACK_POINTER_SET(keyboard_feedback_item);
	SETTING_TRACE("Keyboard feedback Item added");

//Item 3 : Speak while screen off
	vconf_ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK, &vconf_val);
	if (vconf_ret != 0) {
	LOGE("Failed to get vconf bool value "
				"VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_KEYBOARD_FEEDBACK. Error: %d", vconf_ret);
	}

	Setting_GenGroupItem_Data *speak_while_screen_off_item = setting_create_Gendial_field_def(
				genlist, &itc_multiline_2text,
				setting_accessibility_screen_reader_settings_speak_while_screen_off_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, vconf_val ? 1 : 0,
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEAK_WHILE_SCREEN_OFF),
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEAK_WHILE_SCREEN_OFF_INFO),
				setting_accessibility_screen_reader_speak_while_screen_off_check_update);

	speak_while_screen_off_item->userdata = ad;
	__BACK_POINTER_SET(speak_while_screen_off_item);
	SETTING_TRACE("Speak while screen off Item added");

// Item 4 : Hints
	vconf_ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_DESCRIPTION, &vconf_val);
	if (vconf_ret != 0) {
	LOGE("Failed to get vconf bool value "
				"VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_DESCRIPTION. Error: %d", vconf_ret);
	}

	Setting_GenGroupItem_Data *hints_item = setting_create_Gendial_field_def(
				genlist, &itc_1text_1icon,
				setting_accessibility_screen_reader_settings_hints_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, vconf_val ? 1 : 0,
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_HINTS),
				NULL,
				setting_accessibility_screen_reader_hints_check_update);

	hints_item->userdata = ad;
	__BACK_POINTER_SET(hints_item);
	SETTING_TRACE("Hints Item added");

// Item 5 : Vibration Feedback
	vconf_ret = vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_HAPTIC, &vconf_val);
	if (vconf_ret != 0) {
	LOGE("Failed to get vconf bool value "
				"VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_READER_HAPTIC. Error: %d", vconf_ret);
	}

	Setting_GenGroupItem_Data *vibration_feedback_item = setting_create_Gendial_field_def(
				genlist, &itc_1text_1icon,
				setting_accessibility_screen_reader_settings_vibration_feedback_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, vconf_val ? 1 : 0,
				_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_VIBRATION_FEEDBACK),
				NULL,
				setting_accessibility_screen_reader_vibration_feedback_check_update);

	vibration_feedback_item->userdata = ad;
	__BACK_POINTER_SET(vibration_feedback_item);
	SETTING_TRACE("Vibration feedback Item added");

	Setting_GenGroupItem_Data *status_bar_information_item = setting_create_Gendial_field_def(
			genlist, &itc_1text,
			setting_accessibility_screen_reader_settings_status_bar_info_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID, NULL,
			NULL, 1,
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_STATUS_BAR_INFORMATION),
			NULL,
			NULL);

	status_bar_information_item->userdata = ad;
	__BACK_POINTER_SET(status_bar_information_item);

	Setting_GenGroupItem_Data *multiline_status_bar_information_description = setting_create_Gendial_field_def(
			genlist, &itc_multiline_text,
			NULL, ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0,
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_STATUS_BAR_INFORMATION_DESCRIPTION),
			NULL, NULL);
	multiline_status_bar_information_description->userdata = ad;
	__BACK_POINTER_SET(multiline_status_bar_information_description);

	Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->navi_bar,
			_(DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS),
			NULL, NULL, genlist, NULL);
	elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE,
			EINA_TRUE);

	elm_naviframe_item_pop_cb_set(navi_it,
			(Elm_Naviframe_Item_Pop_Cb) __setting_accessibility_screen_reader_settings_page_hide,
			ad);

	SETTING_TRACE_END;
}
