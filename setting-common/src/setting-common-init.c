/*
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: Bartlomiej Uliasz <b.uliasz@samsung.com>
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
 */

#include "setting-common-init.h"
#include "setting-handler.h"

/**
* The event process when win object is destroyed
*/
static void _setting_main_del_win(void *data, Evas_Object *obj, void *event)
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
		evas_object_color_set(bg, 120,120, 250, 125);
		evas_object_show(bg);
	}
	if (win) {
		elm_win_title_set(win, name);
		evas_object_smart_callback_add(win, "delete,request",
				_setting_main_del_win, NULL);
		elm_win_screen_size_get(win, NULL, NULL, &w, &h);
		evas_object_resize(win, w, h);
	}
	return win;
}

EXPORT_PUBLIC
int app_init(MainData *md, char *name)
{
	SETTING_TRACE_BEGIN;

	elm_app_base_scale_set(2.4);

	/* create window */
	md->win_main = _create_main_window(name);
	setting_retvm_if(md->win_main == NULL, SETTING_RETURN_FAIL,
			"window is null");

	md->evas = evas_object_evas_get(md->win_main);

	elm_theme_extension_add(NULL, SETTING_THEME_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_GENLIST_EDJ_NAME);

	elm_win_indicator_mode_set(md->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(md->win_main, ELM_WIN_INDICATOR_OPAQUE);

	evas_object_show(md->win_main);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static Evas_Object *_layout_conform_create(Evas_Object *win_obj, MainData *md)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *layout = NULL;
	Evas_Object *bg = NULL;
	Evas_Object *indicator_bg = NULL;

	md->conform = elm_conformant_add(win_obj);
	if (!md->conform)
		return NULL;

	/*  Base Layout */
	layout = elm_layout_add(md->conform);
	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");

	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(md->conform, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(md->conform, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
	elm_win_resize_object_add(win_obj, md->conform);
	elm_object_content_set(md->conform, layout);

	bg = setting_create_bg(layout, win_obj, "group_list");
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);
	evas_object_show(layout);

	elm_win_conformant_set(md->win_main, EINA_TRUE);
	evas_object_show(md->conform);

	/* Indicator bg */
	indicator_bg = elm_bg_add(md->conform);
	elm_object_style_set(indicator_bg, "indicator/headerbg");
	elm_object_part_content_set(md->conform, "elm.swallow.indicator_bg",
			indicator_bg);
	evas_object_show(indicator_bg);

	evas_object_data_set(win_obj, "conformant", md->conform);
	return layout;
}


static void __all_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	__gl_realized_cb(data, obj, event_info);
	setting_retm_if(event_info == NULL,
			"invalid parameter: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	Setting_GenGroupItem_Data *item_data = elm_object_item_data_get(item);
	ret_if(!item_data);

	if (item_data->isPlaying) {
		elm_object_item_signal_emit(item_data->item,
				"elm,state,expanded", "elm");
	}
}

static Evas_Object *_view_list_prepare(MainData *md)
{
	SETTING_TRACE_BEGIN;
	retvm_if(md == NULL, NULL, "Invalid argument: data is NULL");

	Evas_Object *genlist = elm_genlist_add(md->win_main);
	retvm_if(genlist == NULL, NULL,
			"Cannot set genlist object as content of layout");

	/* first to clear list */
	elm_genlist_clear(genlist);
	/* resolve abnormal height issue */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "realized",
			__all_gl_realized_cb, md);
	evas_object_show(genlist);

	return genlist;
}

EXPORT_PUBLIC
int view_init(MainData *md, char *name)
{
	retv_if(md == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	eina_init();

	/* create a layout conformant */
	md->ly_main = _layout_conform_create(md->win_main, md);
	setting_retvm_if(md->ly_main == NULL, FALSE, "ly_main == NULL");

	/* create a navibar */
	md->navibar_main = setting_create_navi_bar(md->ly_main);
	setting_retvm_if(md->navibar_main == NULL, FALSE,
			"navibar_main == NULL");
	setting_conformant_keypad_state(md->win_main, TRUE);

	/* LAYOUT */
	md->view_layout = elm_layout_add(md->navibar_main);
	setting_retvm_if(md->view_layout == NULL, FALSE, "view_layout == NULL");
	elm_layout_theme_set(md->view_layout, "layout", "application",
			"default");
	evas_object_show(md->view_layout);

	/* push a view to the naviframe */
	md->navibar_main_it = elm_naviframe_item_push(md->navibar_main, name,
			NULL, NULL, md->view_layout, NULL);
	elm_naviframe_item_title_enabled_set(md->navibar_main_it, EINA_TRUE,
			EINA_TRUE);
	elm_object_item_domain_text_translatable_set(md->navibar_main_it,
			SETTING_PACKAGE, EINA_TRUE);

	/* create genlist */
	md->genlist = _view_list_prepare(md);
	setting_retvm_if(md->genlist == NULL, SETTING_RETURN_FAIL,
			"all_list == NULL");
	elm_object_part_content_set(md->view_layout, "elm.swallow.content",
			md->genlist);

	return SETTING_RETURN_SUCCESS;
}

EXPORT_PUBLIC
int app_control_init(MainData *md, app_control_h service)
{
	int ret;
	retv_if(!service, APP_CONTROL_ERROR_INVALID_PARAMETER);
	ret = app_control_clone(&md->app_caller_svc, service);
	retv_if(ret != APP_CONTROL_ERROR_NONE, ret);
	return app_control_create(&md->app_reply_svc);
}

EXPORT_PUBLIC
int app_control_finish(MainData *md)
{
	int ret = APP_CONTROL_ERROR_NONE;

	retv_if(!md->app_caller_svc && !md->app_reply_svc,
			APP_CONTROL_ERROR_INVALID_PARAMETER);

	if (md->app_reply_svc && md->app_caller_svc) {
		ret = app_control_reply_to_launch_request(md->app_reply_svc,
				md->app_caller_svc,
				APP_CONTROL_RESULT_SUCCEEDED);
		if (ret != APP_CONTROL_ERROR_NONE)
			SETTING_TRACE_ERROR(
					"Cannot send reply to launch request");
	}

	if (md->app_caller_svc) {
		ret = app_control_destroy(md->app_caller_svc);
		if (ret != APP_CONTROL_ERROR_NONE)
			SETTING_TRACE_ERROR("Cannot destroy service");
		else
			md->app_caller_svc = NULL;
	}

	if (md->app_reply_svc) {
		ret = app_control_destroy(md->app_reply_svc);
		if (ret != APP_CONTROL_ERROR_NONE)
			SETTING_TRACE_ERROR("Cannot destroy service");
		else
			md->app_reply_svc = NULL;
	}

	return ret;
}

EXPORT_PUBLIC
int add_app_reply(MainData *md, char *key, char *val)
{
	retv_if(!key || !val || !md->app_reply_svc,
			APP_CONTROL_ERROR_INVALID_PARAMETER);

	return app_control_add_extra_data(md->app_reply_svc, key, val);
}
