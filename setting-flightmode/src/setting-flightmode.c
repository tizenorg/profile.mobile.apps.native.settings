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

#include <setting-flightmode.h>
#include <app.h>
#include <appcore-common.h>

#define SETTING_FLIGHTMODE_PACKAGE_NAME "org.tizen.setting-flightmode"


static void setting_flightmode_cb_resize(void *data, Evas *e,
						Evas_Object *obj, void *event_info)
{
	SettingFlightMode *ad = (SettingFlightMode *) data;
	setting_view_update(&setting_view_flightmode_main, ad);
}

setting_view *__get_flightmode_view_to_load(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");

	setting_view_node_table_intialize();

	/* default action */
	setting_view_node_table_register(&setting_view_flightmode_main, NULL);
	return &setting_view_flightmode_main;
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
		evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(win, bg);
		evas_object_show(bg);
	}
	if (win) {
		elm_win_title_set(win, name);
		evas_object_smart_callback_add(win, "delete,request", setting_main_del_win, NULL);
		elm_win_screen_size_get(win, NULL, NULL, &w, &h);
		evas_object_resize(win, w, h);
	}
	return win;
}

Evas_Object *setting_main_layout_conform_create(Evas_Object *win_layout, Evas_Object *win_obj, SettingFlightMode *ad)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	Evas_Object *layout = NULL;
	Evas_Object *conform = NULL;

	conform = elm_conformant_add(win_obj);
	if (!conform)
		return NULL;

	/*  Base Layout */
	layout = elm_layout_add(conform);
	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");

	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(conform, EVAS_HINT_FILL, EVAS_HINT_FILL);
	/*elm_win_resize_object_add(win_obj, layout); */
	elm_win_resize_object_add(win_obj, conform);
	elm_object_content_set(conform, layout);

	Evas_Object *bg = setting_create_bg(layout, win_obj, "group_list");
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);
	evas_object_show(layout);

//	evas_object_smart_callback_add(conform, "virtualkeypad,state,on", ___title_toolbar_show, ad);
//	evas_object_smart_callback_add(conform, "virtualkeypad,state,off", ___title_toolbar_hide, ad);

	elm_win_conformant_set(ad->win_main, EINA_TRUE);
	evas_object_show(conform);

	/* Indicator bg */
	Evas_Object *indicator_bg = elm_bg_add(conform);
	elm_object_style_set(indicator_bg, "indicator/headerbg");
	elm_object_part_content_set(conform, "elm.swallow.indicator_bg", indicator_bg);
	evas_object_show(indicator_bg);

	ad->conform = conform;
	evas_object_data_set(win_obj, "conformant", conform);
	LAUNCH_SETTING_OUT();
	return layout;
}

static bool app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingFlightMode *priv = data;

	setting_retvm_if((!priv), NULL, "!priv");
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	priv->win_main = _create_main_window(SETTING_FLIGHTMODE_PACKAGE_NAME);
	setting_retvm_if(priv->win_main == NULL, SETTING_RETURN_FAIL, "window is null");

	priv->win_main_layout = setting_main_layout_conform_create(priv->win_main, priv->win_main, priv);
	setting_retvm_if(priv->win_main_layout == NULL, FALSE, "win_main_layout == NULL");

	priv->evas = evas_object_evas_get(priv->win_main);
	setting_retvm_if(priv->win_main == NULL, NULL, "cannot get main window ");

	evas_object_show(priv->win_main);

	/* register view node table */
	priv->view_to_load = __get_flightmode_view_to_load(priv);
	setting_retvm_if(NULL == priv->view_to_load, NULL, "NULL == priv->view_to_load");

	setting_create_Gendial_itc("dialogue/1text.2icon", &(priv->itc_1text_2icon));

	/* creating a view. */
	setting_view_node_set_cur_view(priv->view_to_load);
	setting_view_create(priv->view_to_load, (void *)priv);

	evas_object_event_callback_add(priv->win_main,
					EVAS_CALLBACK_RESIZE,
					setting_flightmode_cb_resize, priv);

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
	evas_object_event_callback_del(priv->win_main, EVAS_CALLBACK_RESIZE,
					setting_flightmode_cb_resize);	/*fix flash issue for gallery */
	if (priv->bg)
		evas_object_del(priv->bg);
	priv->bg = NULL;

	/* called when this shared gadget is terminated. similar with app_exit */
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

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, _lang_changed, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}
