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
#include <setting-flightmode.h>
#include <app.h>
#include <appcore-common.h>
#include <ui-gadget.h>

#define SETTING_FLIGHTMODE_PACKAGE_NAME "org.tizen.setting-flightmode"


static void setting_flightmode_cb_resize(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	SettingFlightMode *ad = (SettingFlightMode *) data;
	setting_view_update(&setting_view_flightmode_main, ad);
}

/**
* The event process when win object is destroyed
*/
static void setting_main_del_win(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}

static Evas_Object *_create_main_window(char *name)
{
	Evas_Object *win;
	int w, h;

	win = (Evas_Object *) elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (!win)
		win = elm_win_util_standard_add(name, name);
	else {
		/* elm_win_util_standard_add creates bg inside */
		Evas_Object *bg;

		bg = elm_bg_add(win);

		if (!bg) {
			evas_object_del(win);
			return NULL;
		}
		evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		elm_win_resize_object_add(win, bg);
		evas_object_show(bg);
	}
	if (win) {
		elm_win_title_set(win, name);
		evas_object_smart_callback_add(win, "delete,request",
				setting_main_del_win, NULL);
		elm_win_screen_size_get(win, NULL, NULL, &w, &h);
		evas_object_resize(win, w, h);
	}
	return win;
}

static bool app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingFlightMode *ad = (SettingFlightMode *) data;

	setting_retvm_if((!ad), NULL, "!priv");

	elm_app_base_scale_set(2.4);

	ad->win_main = _create_main_window(SETTING_FLIGHTMODE_PACKAGE_NAME);
	setting_retvm_if(ad->win_main == NULL, SETTING_RETURN_FAIL,
			"window is null");

//	UG_INIT_EFL(ad->win_main, UG_OPT_INDICATOR_ENABLE);

	ad->evas = evas_object_evas_get(ad->win_main);

	setting_set_i18n(SETTING_PACKAGE, FLIGHTMODE_LOCALEDIR);

	elm_theme_extension_add(NULL, SETTING_THEME_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_NEWUX_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_NEW_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_SLIDER_EDJ_NAME);

	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win_main, ELM_WIN_INDICATOR_OPAQUE);

	evas_object_show(ad->win_main);

//	setting_create_Gendial_itc("dialogue/1text.2icon",
//			&(ad->itc_1text_2icon));
//
//	/* creating a view. */
//	setting_view_node_set_cur_view(ad->view_to_load);
//
//	/* register view node table */
//	setting_view_node_table_intialize();
//	setting_view_node_table_register(&setting_view_flightmode_main, NULL);
//	ad->view_to_load = &setting_view_flightmode_main;
	setting_view_create(&setting_view_flightmode_main, ad);

//	evas_object_event_callback_add(ad->win_main,
//					EVAS_CALLBACK_RESIZE,
//					setting_flightmode_cb_resize, ad);

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
	evas_object_event_callback_del(priv->win_main, EVAS_CALLBACK_RESIZE,
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
