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

#include "setting-debug.h"
#include "setting-flightmode.h"

#include <app.h>
#include <appcore-common.h>


#define SETTING_FLIGHTMODE_PACKAGE_NAME "org.tizen.setting-flightmode"
#define FLIGHTMODE_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-flightmode/res/locale"

static void setting_flightmode_cb_resize(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	SettingFlightMode *ad = (SettingFlightMode *)data;
	setting_view_update(&setting_view_flightmode_main, ad);
}

setting_view *__get_flightmode_view_to_load()
{
	SETTING_TRACE_BEGIN;
	setting_view_node_table_intialize();
	/* default action */
	setting_view_node_table_register(&setting_view_flightmode_main, NULL);
	return &setting_view_flightmode_main;
}

static bool app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	app_control_h service = NULL;
	SettingFlightMode *ad = (SettingFlightMode *)data;
	retv_if(!ad, false);

	setting_set_i18n(SETTING_PACKAGE, FLIGHTMODE_LOCALEDIR);

	if (app_init(&ad->md, SETTING_FLIGHTMODE_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	/* register view node table */
	ad->view_to_load = __get_flightmode_view_to_load();
	retv_if(!ad->view_to_load, false);

	setting_create_Gendial_itc("dialogue/1text.2icon",
			&ad->itc_1text_2icon);

	/* creating a view. */
	setting_view_node_set_cur_view(ad->view_to_load);
	setting_view_create(ad->view_to_load, ad);

	evas_object_event_callback_add(ad->md.win_main,
					EVAS_CALLBACK_RESIZE,
					setting_flightmode_cb_resize, ad);

	SETTING_TRACE_END;
	return true;
}

static void app_reset(app_control_h service, void *data)
{
}

static void app_pause(void *data)
{
}

static void app_resume(void *data)
{
}

static void app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingFlightMode *priv = data;

	setting_retm_if((!priv), "!priv");
	/*fix flash issue for gallery */
	evas_object_event_callback_del(priv->md.win_main, EVAS_CALLBACK_RESIZE,
					setting_flightmode_cb_resize);
	if (priv->bg)
		evas_object_del(priv->bg);

	priv->bg = NULL;

	/* called when this shared gadget is terminated. similar to app_exit */
	setting_view_destroy(&setting_view_flightmode_main, priv);
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
		.app_control = app_reset,
	};
	SettingFlightMode app_data;

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

	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}
