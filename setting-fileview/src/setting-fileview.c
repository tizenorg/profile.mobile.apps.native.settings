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

/**
 *@defgroup setting-fileview
 *UG creation code for setting-fileview
 * html file viewer
 */

#include <setting-fileview.h>
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__((visibility("default")))
#endif

#define SETTING_FILEVIEW_PACKAGE_NAME "org.tizen.setting-fileview"
#define FILEVIEW_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-fileview/res/locale"

bool __parse_ug_argument(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingFileviewUG *ad = priv;
	ad->input_file = NULL;

	app_control_get_extra_data(service, "file", &(ad->input_file));
	setting_retvm_if(!ad->input_file, FALSE,
			"no arguement to specialize file");

	ad->input_title = NULL;

	app_control_get_extra_data(service, "title", &(ad->input_title));
	setting_retvm_if(!ad->input_title, FALSE, "no arguement to title");
	SETTING_TRACE("ad->input_file:%s", ad->input_file);
	SETTING_TRACE("ad->input_title:%s", ad->input_title);
	return TRUE;
}

/**
 * Event process when the sizeof UG view changes
 *
 * @param data
 * @param e
 * @param obj
 * @param event_info
 */
static void setting_fileview_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingFileviewUG *ad = (SettingFileviewUG *)data;
	setting_view_update(&setting_view_fileview_main, ad);
}

/**
 * on_create function
 *
 * @param data
 *
 * @return
 */
bool app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	app_control_h service;
	SettingFileviewUG *ad = priv;
	retv_if(!ad, false);

	if (app_init(&ad->md, SETTING_FILEVIEW_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	setting_set_i18n(SETTING_PACKAGE, FILEVIEW_LOCALEDIR);

	retv_if(app_control_create(&service) != APP_CONTROL_ERROR_NONE, false);
	/* creating a view. */
	if (!__parse_ug_argument(service, priv)) {
		SETTING_TRACE_ERROR("Invalid arguement");
		app_control_destroy(service);
		return NULL;
	}
	setting_view_create(&setting_view_fileview_main, (void *)ad);
	evas_object_event_callback_add(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_fileview_ug_cb_resize,
			ad);
	app_control_destroy(service);
	return true;
}

static void app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void app_controll(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
}

/**
 * on_destroy function of the UG
 *
 * @param ug
 * @param data
 * @param priv
 */
static void app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingFileviewUG *ad = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_fileview_ug_cb_resize);
	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_fileview_main, ad);

	if (ad->input_file)
		FREE(ad->input_file);
	if (ad->input_title)
		FREE(ad->input_title);

	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}

	SETTING_TRACE_END;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
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
		.app_control = app_controll,
	};
	SettingFileviewUG app_data;
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}
