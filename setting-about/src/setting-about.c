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

#include <setting-about.h>
#include <setting-about-main.h>
#include <setting-cfg.h>


#define SETTING_ABOUT_PACKAGE_NAME "org.tizen.setting-about"
#define ABOUT_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-about/res/locale"

/**
 * @brief Event process when the sizeof view changes
 *
 * @param data
 * @param e
 * @param obj
 * @param event_info
 */
static void _cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingAbout *ad = (SettingAbout *)data;
	setting_view_update(&setting_view_about_main, ad);
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting About - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}
///////////////////////////////////////
static bool app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAbout *ad = (SettingAbout *) data;

	setting_retvm_if((!ad), NULL, "!priv");
	setting_set_i18n(SETTING_PACKAGE, ABOUT_LOCALEDIR);

	ad->modem_handle = tel_init(NULL);
	if (!ad->modem_handle)
		SETTING_TRACE_ERROR("*** [ERR] tel_init failed ***");
	else
		SETTING_TRACE("tel_init ok[handle:%p]", ad->modem_handle);

	if(app_init(&ad->md, SETTING_ABOUT_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	setting_view_create(&setting_view_about_main, ad);

	evas_object_event_callback_add(ad->md.win_main,
					EVAS_CALLBACK_RESIZE,
					_cb_resize, ad);

	SETTING_TRACE_END;
	return true;
}

static void app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!data), "!priv");
	SettingAbout *ad = data;
	ad->pause_flag = FALSE;

	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };

	if (ad->item_data_wifi) {
		setting_about_main_get_wifi_mac_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		ad->item_data_wifi->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(ad->item_data_wifi->item, ad->item_data_wifi);
		elm_genlist_item_update(ad->item_data_wifi->item);
	}
	memset(str, 0, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (ad->item_data_bt) {
		setting_about_main_get_bluetooth_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		ad->item_data_bt->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(ad->item_data_bt->item, ad->item_data_bt);
		elm_genlist_item_update(ad->item_data_bt->item);
	}

	if (ad->item_dev_name) {
		if (elm_object_focus_get(ad->item_dev_name->eo_check)) {
			elm_object_focus_set(ad->item_dev_name->eo_check, EINA_TRUE);
			Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(ad->item_dev_name->eo_check);
			if (imf_context) {
				ecore_imf_context_input_panel_show(imf_context);
			}
		}
	}
}

static void app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAbout *SettingAbout = data;
	SettingAbout->pause_flag = TRUE;
}

static void app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!data), "!data");
	SettingAbout *ad = data;

	evas_object_event_callback_del(ad->md.ly_main, EVAS_CALLBACK_RESIZE, _cb_resize);	/* fix flash issue for gallery */

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_about_main, ad);

	if (ad->modem_handle
		&& tel_deinit(ad->modem_handle) == TAPI_API_SUCCESS) {
		SETTING_TRACE("tel_deinit sucessed");
	}
	FREE(ad);

	SETTING_TRACE_END;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = app_create,
		.pause = app_pause,
		.resume = app_resume,
		.terminate = app_terminate,
		.app_control = NULL
	};
	SettingAbout app_data;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&app_data, 0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}

