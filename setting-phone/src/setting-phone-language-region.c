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
#include <setting-phone-language-region.h>
#include <setting-common-draw-widget.h>
#include <app_manager.h>
#include <pkgmgr-info.h>


static int setting_phone_language_region_create(void *cb);
static int setting_phone_language_region_destroy(void *cb);
static int setting_phone_language_region_update(void *cb);
static int setting_phone_language_region_cleanup(void *cb);

setting_view setting_view_phone_language_region = {
	.create = setting_phone_language_region_create,
	.destroy = setting_phone_language_region_destroy,
	.update = setting_phone_language_region_update,
	.cleanup = setting_phone_language_region_cleanup,
};



#if 0
/**
 * @brief Callback of language change flush timer
 *
 * @param cb The view data passed between all callbacks
 * @return #1 means call it again, else means call it only once
 */
static Eina_Bool setting_phone_language_region_flush_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhoneUG *ad = (SettingPhoneUG *) cb;

	elm_config_all_flush();
	ad->lang_flush_timer = NULL;

	SETTING_TRACE_END;
	return EINA_FALSE;
}
#endif

static Eina_Bool setting_phone_language_region_freeze_event_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhoneUG *ad = (SettingPhoneUG *) cb;

	evas_object_freeze_events_set(ad->navi_bar, EINA_FALSE);

	ad->event_freeze_timer = NULL;
	SETTING_TRACE_END;
	return EINA_FALSE;
}

/**
 * @brief Finds appid with specific category
 *
 * @return 0 if success, negative value(<0) if fail. Callback is not called if return value is negative
 */
static int _find_appid_ime_list(const pkgmgrinfo_appinfo_h handle, void *user_data)
{
	if (user_data) {
		char **result = (char **)user_data;
		char *appid = NULL;
		int ret = 0;

		/* Get appid */
		ret = pkgmgrinfo_appinfo_get_appid(handle, &appid);
		if (ret == PMINFO_R_OK) {
			*result = strdup(appid);
		} else {
			LOGW("pkgmgrinfo_appinfo_get_appid failed!");
		}
	} else {
		LOGW("user_data is null!");
	}

	return -1;  /* This callback is no longer called. */
}

/**
 * @brief create keyboard ug by app control
 *
 * @param ad The view data passed
 */
static void setting_phone_lang_create_keyboard_ug(SettingPhoneUG *ad)
{
	SETTING_TRACE_BEGIN;
	ret_if(ad == NULL);

	/* Launch IME List application; e.g., org.tizen.inputmethod-setting-list */
	int ret;
	app_control_h app_control;
	char *app_id = NULL;
	pkgmgrinfo_appinfo_filter_h handle;

	/* Find appid with "http://tizen.org/category/ime-list" category; appid might be different in models. */
	ret = pkgmgrinfo_appinfo_filter_create(&handle);
	if (ret == PMINFO_R_OK) {
		ret = pkgmgrinfo_appinfo_filter_add_string(handle, PMINFO_APPINFO_PROP_APP_CATEGORY, "http://tizen.org/category/ime-list");
		if (ret == PMINFO_R_OK) {
			ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(handle, _find_appid_ime_list, &app_id);
		}
		pkgmgrinfo_appinfo_filter_destroy(handle);
	}

	if (app_id) {
		ret = app_control_create(&app_control);
		if (ret == APP_CONTROL_ERROR_NONE) {
			app_control_set_operation(app_control, APP_CONTROL_OPERATION_DEFAULT);
			app_control_set_app_id(app_control, app_id);
			app_control_add_extra_data(app_control, "caller", "settings");  /* Indicates Settings application is caller. */
			app_control_set_launch_mode(app_control, APP_CONTROL_LAUNCH_MODE_GROUP);
			ret = app_control_send_launch_request(app_control, NULL, NULL);
			if (ret != APP_CONTROL_ERROR_NONE) {
				LOGW("app_control_send_launch_request failed(%d): %s", ret, app_id);
			}
			app_control_destroy(app_control);
		}
		free(app_id);
	} else {
		LOGW("AppID with http://tizen.org/category/ime-list category is not available");
	}
}

/**
 * @brief create voice recognition ug by app control
 *
 */
static void setting_phone_lang_create_voice_recognition_ug(SettingPhoneUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (app_group_launcher("setting-voice-efl|show_what:stt") == 0) {
		ad->event_freeze_timer = ecore_timer_add(1, setting_phone_language_region_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
}

/**
 * @brief create text-to-speech ug by app control
 *
 */
static void setting_phone_lang_create_text_speech_ug(SettingPhoneUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (app_group_launcher("setting-voice-efl|show_what:tts") == 0) {
		ad->event_freeze_timer = ecore_timer_add(1, setting_phone_language_region_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
}

/**
 * @brief create readout(old name : drivingmode) ug by app control
 *
 * @param ad The view data passed
 */
static void setting_phone_lang_create_readout_ug(SettingPhoneUG *ad)
{
	SETTING_TRACE_BEGIN;
	ret_if(ad == NULL);

	if (app_launcher("setting-drivingmode-efl") == 0) {
		ad->event_freeze_timer = ecore_timer_add(1, setting_phone_language_region_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
}

/**
 * @brief Callback of cancel button
 *
 * @param cb The view data passed between all callbacks
 */
static Eina_Bool setting_phone_lang_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;

	Eina_Bool is_freezed = evas_object_freeze_events_get(ad->navi_bar);
	SETTING_TRACE_DEBUG("is_freezed : %d", is_freezed);
	if (is_freezed)
		return EINA_FALSE;

	ug_destroy_me(ad->ug);
	return EINA_FALSE;
}

/**
 * @brief Callback of view cleanup
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static void setting_phone_lang_item_Gendial_mouse_up_cb(void *data,
														Evas_Object *obj,
														void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(!data || !event_info, "data||event_info is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp("IDS_ST_HEADER_DISPLAY_LANGUAGE", list_item->keyStr)) {
		setting_view_change(&setting_view_phone_language_region,
							&setting_view_phone_display_language, ad);
	} else if (!safeStrCmp(_(REGION_FORMAT_LOWER), list_item->keyStr)) {
		setting_view_change(&setting_view_phone_language_region,
							&setting_view_phone_region_format, ad);
	} else if (!safeStrCmp("IDS_ST_BODY_KEYBOARD", list_item->keyStr)) {
		setting_phone_lang_create_keyboard_ug(ad);
	} else if (!safeStrCmp("IDS_VC_HEADER_VOICE_RECOGNITION", list_item->keyStr)) {
		setting_phone_lang_create_voice_recognition_ug(ad);
	} else if (!safeStrCmp("IDS_ST_HEADER_TEXT_TO_SPEECH", list_item->keyStr)) {
		setting_phone_lang_create_text_speech_ug(ad);
	} else if (!safeStrCmp("IDS_VC_MBODY_NOTIFICATION_READ_OUT", list_item->keyStr)) {
		setting_phone_lang_create_readout_ug(ad);
	} else if (!safeStrCmp("IDS_ST_BODY_VOICE_CONTROL", list_item->keyStr)) {
		app_group_launcher("org.tizen.voice-setting|show:voice-control");
	} else if (!safeStrCmp("IDS_ST_BODY_TTS", list_item->keyStr)) {
		app_group_launcher("org.tizen.voice-setting|show:tts");
	}
}

/* ***************************************************
 *
 * main view func
 *
 ***************************************************/
/**
 * @brief Callback of view creating
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_language_region_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhoneUG *ad = (SettingPhoneUG *) cb;
	Evas_Object *scroller = NULL;

	int region_automatic;

	ad->lang_change = EINA_FALSE;

	char *title = "IDS_ST_HEADER_LANGUAGE_AND_INPUT";
	ad->ly_language =
		setting_create_layout_navi_bar_genlist(ad->win_main_layout,
											   ad->win_get, title,
											   NULL,
											   NULL,
											   setting_phone_lang_click_softkey_cancel_cb,
											   NULL, ad, &scroller,
											   &(ad->navi_bar));
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	elm_naviframe_item_pop_cb_set(navi_it, setting_phone_lang_click_softkey_cancel_cb, ad);
	retv_if(ad->ly_language == NULL, SETTING_RETURN_FAIL);
	ad->gl_lang_region = scroller;
	ad->lang_region_navi_it = navi_it;
	SETTING_TRACE("navi_it:%p", navi_it);
	evas_object_smart_callback_add(ad->gl_lang_region, "realized", __gl_realized_cb, NULL);

	char *pa_display_lang = get_pa_display_language_str();
	ad->data_display_language =
		setting_create_Gendial_field_def(scroller,
										 &(ad->itc_2text_2),
										 setting_phone_lang_item_Gendial_mouse_up_cb,
										 ad, SWALLOW_Type_INVALID,
										 NULL, NULL, 0,
										 "IDS_ST_HEADER_DISPLAY_LANGUAGE",
										 _(pa_display_lang),
										 NULL);
	if (ad->data_display_language) {
		ad->data_display_language->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_display_language is NULL");
	}
	G_FREE(pa_display_lang);

	char *pa_region = vconf_get_str(VCONFKEY_REGIONFORMAT);
	if (pa_region == NULL) {
		SETTING_TRACE_ERROR
		("[Error] get value of VCONFKEY_REGIONFORMAT fail.");
	}
	ad->prev_locale = (char *)g_strdup(pa_region);
	/*check if the region is set to auto */
	char *region_format_str = NULL;
	vconf_get_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, &region_automatic);
	if (region_automatic == 1) {
		region_format_str = (char *)g_strdup(_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"));
	} else {
		region_format_str = setting_phone_lang_get_region_str(pa_region);
	}
	ad->data_region_fmt =
		setting_create_Gendial_field_def(scroller,
										 &(ad->itc_2text_2),
										 setting_phone_lang_item_Gendial_mouse_up_cb,
										 ad, SWALLOW_Type_INVALID,
										 NULL, NULL, 0,
										 _(REGION_FORMAT_LOWER),
										 region_format_str, NULL);
	if (ad->data_region_fmt) {
		ad->data_region_fmt->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_region_fmt is NULL");
	}

	char *example_desc = setting_phone_lang_get_example_desc(pa_region, ad);
	ad->data_region_fmt_example = setting_create_Gendial_field_def(scroller, &itc_multiline_text,
																   NULL,
																   NULL,
																   SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
																   NULL, NULL, 0, example_desc, NULL, NULL);
	if (ad->data_region_fmt_example)
		elm_genlist_item_select_mode_set(ad->data_region_fmt_example->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* keyboard */
	ad->data_title_keyboard = setting_create_Gendial_field_titleItem(scroller, &itc_group_item, _("IDS_ST_BODY_KEYBOARD"), NULL);
	ad->data_keyboard =
		setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
										 setting_phone_lang_item_Gendial_mouse_up_cb,
										 ad, SWALLOW_Type_INVALID, NULL,
										 NULL, 0, "IDS_ST_BODY_KEYBOARD",
										 NULL, NULL);

	/* Speech */
	ad->data_title_speech = setting_create_Gendial_field_titleItem(scroller, &itc_group_item, _("IDS_ST_BODY_SPEECH"), NULL);
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
									 setting_phone_lang_item_Gendial_mouse_up_cb,
									 ad, SWALLOW_Type_INVALID, NULL,
									 NULL, 0, "IDS_ST_BODY_VOICE_CONTROL",
									 NULL, NULL);
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
									 setting_phone_lang_item_Gendial_mouse_up_cb,
									 ad, SWALLOW_Type_INVALID, NULL,
									 NULL, 0, "IDS_ST_BODY_TTS",
									 NULL, NULL);
	if (pa_region) {
		FREE(pa_region);
	}
	if (example_desc) {
		FREE(example_desc);
	}

	setting_view_phone_language_region.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief Callback of view destroy
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_language_region_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhoneUG *ad = (SettingPhoneUG *) cb;

	evas_object_smart_callback_del(ad->gl_lang_region, "realized", __gl_realized_cb);

	if (ad->ly_language != NULL) {
		evas_object_del(ad->ly_language);
		ad->ly_language = NULL;

		if (ad->lang_flush_timer) {
			ecore_timer_del(ad->lang_flush_timer);
			ad->lang_flush_timer = NULL;
		}

		if (ad->event_freeze_timer) {
			ecore_timer_del(ad->event_freeze_timer);
			ad->event_freeze_timer = NULL;
		}

		if (ad->pattern_generator) {
			udatpg_close(ad->pattern_generator);
			ad->pattern_generator = NULL;
		}
		setting_view_phone_language_region.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}



/**
 * @brief Callback of view update
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_language_region_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhoneUG *ad = (SettingPhoneUG *) cb;

	if (ad->ly_language != NULL) {
		evas_object_show(ad->ly_language);
		/*refresh the title */
		elm_object_item_text_set(ad->lang_region_navi_it, _("IDS_ST_HEADER_LANGUAGE_AND_INPUT")); /*"IDS_ST_BODY_LANGUAGE_AND_KEYBOARD")); */

		if (ad->data_display_language) {
			G_FREE(ad->data_display_language->sub_desc);
			ad->data_display_language->sub_desc = get_pa_display_language_str();
			elm_object_item_data_set(ad->data_display_language->item, ad->data_display_language);
			elm_genlist_item_update(ad->data_display_language->item);
		}
		/*refresh region ex */
		char *pa_region = vconf_get_str(VCONFKEY_REGIONFORMAT);
		if (pa_region == NULL) {
			SETTING_TRACE_ERROR
			("[Error] get value of VCONFKEY_REGIONFORMAT fail.");
			return SETTING_RETURN_FAIL;
		}

		if (ad->data_region_fmt) {
			int region_automatic = 1;
			int ret = vconf_get_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, &region_automatic);
			if (ret == VCONF_ERROR) {
				SETTING_TRACE_ERROR("get vconf region automatic err.");
			}

			if (region_automatic == 1) {
				ad->data_region_fmt->sub_desc = (char *)g_strdup(_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"));
			} else {
				ad->data_region_fmt->sub_desc = setting_phone_lang_get_region_str(pa_region);
			}
			G_FREE(ad->data_region_fmt->keyStr);
			ad->data_region_fmt->keyStr = (char *)g_strdup(_(REGION_FORMAT_LOWER));
			elm_object_item_data_set(ad->data_region_fmt->item, ad->data_region_fmt);
			elm_genlist_item_update(ad->data_region_fmt->item);
		}
		if (ad->data_region_fmt_example) {
			char *example_desc = setting_phone_lang_get_example_desc(pa_region, ad);
			G_FREE(ad->data_region_fmt_example->keyStr);
			ad->data_region_fmt_example->keyStr = (char *)g_strdup(example_desc);
			FREE(example_desc);
			elm_object_item_data_set(ad->data_region_fmt_example->item, ad->data_region_fmt_example);
			elm_genlist_item_update(ad->data_region_fmt_example->item);
		}
		if (ad->data_title_keyboard) {
			G_FREE(ad->data_title_keyboard->keyStr);
			ad->data_title_keyboard->keyStr = (char *)g_strdup(_("IDS_ST_BODY_KEYBOARD"));
			elm_object_item_data_set(ad->data_title_keyboard->item, ad->data_title_keyboard);
			elm_genlist_item_update(ad->data_title_keyboard->item);
		}
		if (ad->data_title_speech) {
			G_FREE(ad->data_title_speech->keyStr);
			ad->data_title_speech->keyStr = (char *)g_strdup(_("IDS_ST_BODY_SPEECH"));
			elm_object_item_data_set(ad->data_title_speech->item, ad->data_title_speech);
			elm_genlist_item_update(ad->data_title_speech->item);
		}
		if (ad->data_readout) {
			G_FREE(ad->data_readout->sub_desc);
			ad->data_readout->sub_desc = get_pa_noti_readout_str();
			elm_object_item_data_set(ad->data_readout->item, ad->data_readout);
			elm_genlist_item_update(ad->data_readout->item);
		}

		FREE(pa_region);
	}
#if 0
	if (ad->lang_change) {
		ad->lang_flush_timer = ecore_timer_add(0.3, setting_phone_language_region_flush_timer_cb, ad);
		ad->lang_change = EINA_FALSE;
	}
#endif
	return SETTING_RETURN_SUCCESS;
}



/**
 * @brief Callback of view cleanup
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_language_region_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}
