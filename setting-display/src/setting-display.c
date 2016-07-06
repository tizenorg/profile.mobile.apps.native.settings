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

#include "setting-display.h"
#include <setting-cfg.h>

#include <app.h>
#include <appcore-common.h>
#include <Eina.h>

#define SETTING_DISPLAY_PACKAGE_NAME "org.tizen.setting-display"

/******************************APP CONTROL***********************************/
static bool _setting_display_app_create(void *data);
static void _setting_display_app_control_cb(app_control_h app_control, void *data);
static void _setting_display_app_on_pause(void *data);
static void _setting_display_app_on_resume(void *data);
static void _setting_display_app_terminate(void *data);
/********************************OTHER***************************************/
static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info);
static void _low_battery(app_event_info_h event_info, void *data);
static void _lang_changed(app_event_info_h event_info, void *data);

static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	elm_exit();
}

/**
 * view selection by service variable
 *
 * viewtype == 'brightness' --> setting_view_display_brightness
 * viewtype != NULL && viewtype --> setting_view_display_main
 */
setting_view *__get_display_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;
	SettingDisplay *ad = (SettingDisplay *)data;

	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "!data");
	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;
	ret = app_control_get_uri(service, &uri);
	if (!ret && uri) {
		SETTING_TRACE("uri:%s", uri);
		/* DO NOTHING NOW */
	} else {

		SETTING_TRACE_DEBUG("viewtype:%s", viewtype);

		if (!safeStrCmp(viewtype, "brightness")) {
			setting_view_node_table_register(
					&setting_view_display_brightness, NULL);
			FREE(viewtype);
			return &setting_view_display_brightness;
		} else if (!safeStrCmp(viewtype, "backlight")) {
			setting_view_node_table_register(
					&setting_view_display_backlight, NULL);
			FREE(viewtype);
			return &setting_view_display_backlight;
		} else {
			if (!safeStrCmp(viewtype, "frontpage_backlight")) {
				ad->display_expand_state =
						DISPLAY_EXPAND_BACKLIGHT;
			} else if (!safeStrCmp(viewtype,
					"frontpage_duration")) {
				ad->display_expand_state =
						DISPLAY_EXPAND_TOUCH_KEY_LIGHT_DURATION;
			} else {
				ad->display_expand_state =
						DISPLAY_EXPAND_NONE;
			}
			setting_view_node_table_register(
					&setting_view_display_main, NULL);
			setting_view_node_table_register(
					&setting_view_display_brightness,
					&setting_view_display_main);
			FREE(viewtype);
			return &setting_view_display_main;
		}
	}
	return NULL;
}

/**
 * return view layout
 *
 * viewtype != 'wallpaper' --> ly_main
 */
Evas_Object *__get_display_layout_to_return(app_control_h service, void *priv)
{
	SettingDisplay *ad = priv;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;

	SETTING_TRACE_BEGIN;

	ret = app_control_get_uri(service, &uri);
	if (!ret && uri)
		return ad->md.view_layout;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);
	FREE(viewtype);

	return ad->md.win_main;
}

static void setting_display_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingDisplay *ad = (SettingDisplay *)data;
	setting_view_update(ad->view_to_load, ad);
}

/*help smart screen popup*/
Eina_Bool __show_smartstay_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Data parameter is NULL");

	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retvm_if(ad->smart_stay_sublayout == NULL,
			ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Dad->smart_stay_sublayout parameter is NULL");

	/*hide top popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout, "elm,state,top,hide",
			"elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout,
			"elm,state,center,show", "elm");

	return ECORE_CALLBACK_CANCEL;
}
Eina_Bool __show_smartrotation_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Data parameter is NULL");

	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retvm_if(ad->smart_rotation_sublayout == NULL,
			ECORE_CALLBACK_CANCEL,
			"[Setting > Display] ad->smart_rotation_sublayout parameter is NULL");
	/*hide top popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout,
			"elm,state,top,hide", "elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout,
			"elm,state,center,show", "elm");

	return ECORE_CALLBACK_CANCEL;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

static void _low_battery(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = data;
	setting_retm_if(NULL == data, "data is NULL");

	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
		if(ad->md.navibar_main)
			setting_navi_items_update(ad->md.navibar_main);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

static void _setting_display_app_control_cb(app_control_h app_control, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = data;
	setting_retm_if(NULL == data, "data is NULL");

	ad->view_to_load = __get_display_view_to_load(ad, app_control);

	setting_retm_if(NULL == ad->view_to_load, "NULL == ad->view_to_load");
	setting_view_create(ad->view_to_load, (void *)ad);
}

static bool _setting_display_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, false, "!data");

	SettingDisplay *display_ad = (SettingDisplay *)data;

	setting_set_i18n("setting", DISPLAY_LOCALEDIR);

	if (app_init(&display_ad->md, SETTING_DISPLAY_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	evas_object_smart_callback_add(display_ad->md.win_main, "delete,request",
			_main_win_del_cb, display_ad);

//	Create base Layout
	setting_create_Gendial_itc("1line.top", &(display_ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_2text_3));

	evas_object_event_callback_add(display_ad->md.win_main,
			EVAS_CALLBACK_RESIZE,
			setting_display_cb_resize, display_ad);
	SETTING_TRACE_BEGIN;
	return true;
}

static void _setting_display_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retm_if((!data), "!data");


	setting_view_destroy(&setting_view_display_main, ad);
	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.ly_main,
			EVAS_CALLBACK_RESIZE, setting_display_cb_resize);

	FREE(ad->uri);
	if (ad->help_popup) {
		evas_object_del(ad->help_popup);
		ad->help_popup = NULL;
	}

	if (&setting_view_display_main == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, ad);
		setting_view_destroy(&setting_view_display_main, ad);
	} else if (&setting_view_display_brightness == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, ad);
	} else if (&setting_view_display_backlight == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_backlight, ad);
	}

	if (NULL != ad->md.ly_main) {
		evas_object_hide((Evas_Object *)ad->md.ly_main);
		evas_object_del(ad->md.ly_main);
	}

	SETTING_TRACE_END;

}

static void _setting_display_app_on_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void _setting_display_app_on_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SettingDisplay display_ad ;
	app_event_handler_h handlers[5] = {NULL, };

	ui_app_lifecycle_callback_s ops = {
		.create = _setting_display_app_create,
		.pause = _setting_display_app_on_pause,
		.resume = _setting_display_app_on_resume,
		.terminate = _setting_display_app_terminate,
		.app_control = _setting_display_app_control_cb,
	};


	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, _low_battery, &display_ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &display_ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&display_ad, 0x00, sizeof(SettingDisplay));
	int r = 0;
	r = ui_app_main(argc, argv, &ops, &display_ad);
	retv_if(r == -1, -1);

	return 0;
}
