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
int app_init(MainData *ad, char *name)
{
	SETTING_TRACE_BEGIN;

	elm_app_base_scale_set(2.4);

	/* create window */
	ad->win_main = _create_main_window(name);
	setting_retvm_if(ad->win_main == NULL, SETTING_RETURN_FAIL,
			"window is null");

	ad->evas = evas_object_evas_get(ad->win_main);

	elm_theme_extension_add(NULL, SETTING_THEME_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_GENLIST_EDJ_NAME);

	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win_main, ELM_WIN_INDICATOR_OPAQUE);

	evas_object_show(ad->win_main);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}


static void _tapi_init(MainData *ad)
{
	ad->handle = tel_init(NULL);
	if (!ad->handle)
		SETTING_TRACE_ERROR("*** [ERR] tel_init failed ***");
	else
		SETTING_TRACE("tel_init ok[handle:%p]", ad->handle);
}

static void _genlist_itc_init(MainData *ad)
{
	/* [UI] create structures for genlist style */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_table[GENDIAL_Type_1text_1icon_2]));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_table[GENDIAL_Type_1icon_2text]));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_table[GENDIAL_Type_expandable_proc]));
}

static Evas_Object *_layout_conform_create(Evas_Object *win_layout,
		Evas_Object *win_obj, MainData *ad)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *layout = NULL;
	Evas_Object *conform = NULL;
	Evas_Object *bg = NULL;
	Evas_Object *indicator_bg = NULL;

	conform = elm_conformant_add(win_obj);
	if (!conform)
		return NULL;

	/*  Base Layout */
	layout = elm_layout_add(conform);
	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");

	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(conform, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
	elm_win_resize_object_add(win_obj, conform);
	elm_object_content_set(conform, layout);

	bg = setting_create_bg(layout, win_obj, "group_list");
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);
	evas_object_show(layout);

	elm_win_conformant_set(ad->win_main, EINA_TRUE);
	evas_object_show(conform);

	/* Indicator bg */
	indicator_bg = elm_bg_add(conform);
	elm_object_style_set(indicator_bg, "indicator/headerbg");
	elm_object_part_content_set(conform, "elm.swallow.indicator_bg",
			indicator_bg);
	evas_object_show(indicator_bg);

	ad->conform = conform;
	evas_object_data_set(win_obj, "conformant", conform);
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

static Evas_Object *_view_list_prepare(MainData *ad)
{
	SETTING_TRACE_BEGIN;
	retvm_if(ad == NULL, NULL, "Invalid argument: data is NULL");

	if (ad->sc_gl[SC_All_List]) {
		evas_object_show(ad->sc_gl[SC_All_List]);
		return ad->sc_gl[SC_All_List];
	}

	Evas_Object *genlist = elm_genlist_add(ad->win_main);
	retvm_if(genlist == NULL, NULL,
			"Cannot set genlist object as content of layout");

	ad->sc_gl[SC_All_List] = genlist;
	/* first to clear list */
	elm_genlist_clear(genlist);
	/* resolve abnormal height issue */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(genlist, "realized",
			__all_gl_realized_cb, ad);
	evas_object_show(genlist);

	return genlist;
}

EXPORT_PUBLIC
int view_init(MainData *ad, char *name)
{
	retv_if(ad == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	_tapi_init(ad);
	_genlist_itc_init(ad);
	eina_init();

	/* create a layout conformant */
	ad->ly_main = _layout_conform_create(ad->win_main, ad->win_main, ad);
	setting_retvm_if(ad->ly_main == NULL, FALSE, "ly_main == NULL");

	/* create a navibar */
	ad->navibar_main = setting_create_navi_bar(ad->ly_main);
	setting_retvm_if(ad->navibar_main == NULL, FALSE,
			"navibar_main == NULL");
	setting_conformant_keypad_state(ad->win_main, TRUE);

	/* LAYOUT */
	ad->view_layout = elm_layout_add(ad->navibar_main);
	setting_retvm_if(ad->view_layout == NULL, FALSE, "view_layout == NULL");
	elm_layout_theme_set(ad->view_layout, "layout", "application",
			"default");
	evas_object_show(ad->view_layout);

	/* push a view to the naviframe */
	Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->navibar_main,
			name, NULL, NULL,
			ad->view_layout, NULL);
	elm_naviframe_item_title_enabled_set(navi_it, EINA_TRUE, EINA_TRUE);
	ad->navibar_main_it = navi_it;
	elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE,
			EINA_TRUE);

	/* create genlist */
	ad->genlist = _view_list_prepare(ad);
	setting_retvm_if(ad->genlist == NULL, SETTING_RETURN_FAIL,
			"all_list == NULL");
	elm_object_part_content_set(ad->view_layout, "elm.swallow.content",
			ad->genlist);

	return SETTING_RETURN_SUCCESS;
}
