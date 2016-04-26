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
 * @file setting-cis-main2.c
 * @brief implement first view of Setting application
 *
 * first view has tabbar and Gengrid
 */
#define _GNU_SOURCE

#include <setting-main.h>
#include <setting-handler.h>
#include <setting-helper.h>

#include <system_settings.h>

#include <setting-common-sound.h>
#include <setting-common-alarmmgr.h>

#include <sqlite3.h>

static int setting_main_create(void *cb);
static int setting_main_destroy(void *cb);
static int setting_main_update(void *cb);
static int setting_main_cleanup(void *cb);

setting_view setting_view_main = {
	.create = setting_main_create,
	.update = setting_main_update,
	.destroy = setting_main_destroy,
	.cleanup = setting_main_cleanup,
};


/*//////////////////////////////////////////////////////////////////////////////////////////// */

void __all_gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	__gl_realized_cb(data, obj, event_info);
	setting_retm_if(event_info == NULL, "invalid parameter: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	Setting_GenGroupItem_Data *item_data = elm_object_item_data_get(item);
	if (!item_data) {
		return;
	}

	if (item_data->isPlaying) {
		elm_object_item_signal_emit(item_data->item, "elm,state,expanded", "elm");
	}
}

static void __all_gl_group_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	/*setting_retm_if(data == NULL, "Data parameter is NULL"); */
	/* int view_type = (int)data; */
	/* setting_main_appdata *ad = g_main_ad; */
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *data_parentItem = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
	setting_retm_if(data_parentItem == NULL, "data_parentItem is NULL");


	Elm_Object_Item *start_item = NULL;
	/* Elm_Object_Item *end_item = NULL; */
	/* Elm_Object_Item *it; */

	data_parentItem->isPlaying = !data_parentItem->isPlaying;
	int hide_flag = !(data_parentItem->isPlaying);
	SETTING_TRACE("----------------------> expand list:%s, hide_flag:%d", _(data_parentItem->keyStr), hide_flag);

	/*to controll the arrow */
	if (hide_flag)
		elm_object_item_signal_emit(data_parentItem->item, "elm,state,contracted", "elm");
	else
		elm_object_item_signal_emit(data_parentItem->item, "elm,state,expanded", "elm");

	if (!start_item) {
		SETTING_TRACE_ERROR(" ------------------> start_item IS NULL");
	}
}


Evas_Object *_view_list_geter(void *data)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, NULL, "Invalid argument: data is NULL");
	setting_main_appdata *ad = (setting_main_appdata *)data;
	SETTING_TRACE("ad->md.sc_gl[SC_All]:%p", ad->md.sc_gl[SC_All_List]);

	if (ad->md.sc_gl[SC_All_List]) {
		evas_object_show(ad->md.sc_gl[SC_All_List]);
		return ad->md.sc_gl[SC_All_List];
	}

	Evas_Object *genlist = elm_genlist_add(ad->md.win_main);
	retvm_if(genlist == NULL, NULL, "Cannot set genlist object as contento of layout");

	ad->md.sc_gl[SC_All_List] = genlist;
	/*elm_genlist_block_count_set(genlist, 2); */
	/*elm_object_style_set(genlist, "dialogue"); */
	elm_genlist_clear(genlist);	/* first to clear list */
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);	/* resolve abnormal height issue */
	evas_object_smart_callback_add(genlist, "realized", __all_gl_realized_cb, ad);
	evas_object_show(genlist);

	Setting_GenGroupItem_Data *item_data = NULL;
	int group_dx = GL_Group_Connection;
	/*---------------------------------------------------------------------------------------- */
	/* 2.category name loop */
	/*---------------------------------------------------------------------------------------- */
	int i, j;
	char *keyStr = NULL;
	char *icon_path = NULL;
	char *ug_args = NULL;
	char *group_name = NULL;
	int index = 0;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		group_name = setting_cfg_get_category_name(i);
		group_dx++;/*from the first */
		item_data = setting_create_Gendial_field_def(
						genlist,
						&(ad->md.itc_table[GENDIAL_Type_expandable_proc]),
						__all_gl_group_clicked_cb,
						(void *)NULL,
						SWALLOW_Type_INVALID,
						NULL, /* l_icon */
						NULL, /* r_icon */
						0,
						group_name,
						NULL, /* r_icon */
						NULL);
		if (item_data) {
			item_data->isPlaying = TRUE;/*expandable ? */
			item_data->chk_status = group_dx;/*keep the group idx */
		}

		index = 0;


		/*---------------------------------------------------------------------------------------- */
		/* 2. display each items, following category */
		/*---------------------------------------------------------------------------------------- */
		/* add sub items */
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			keyStr = setting_cfg_get_keyname_idx(i, j);

			ug_args = setting_cfg_get_ug_args_idx(i, j);
			icon_path = setting_cfg_get_icon_path_idx(i, j);

			mainlist_entry *entry =  settinig_drawer_hash_find(ad, keyStr);

			if (entry) {
				if (entry->type == UG_HANDLE) {
					if (is_ug_installed_by_ug_args(ug_args)) {
						item_data = entry->ui_handler(ad, keyStr, icon_path, ug_args, genlist, NULL);
						entry->item_data = item_data;
					}
				} else if (entry->type == UI_PROC) {
					item_data = entry->ui_handler(ad, keyStr, icon_path, ug_args, genlist, NULL);
					entry->item_data = item_data;
				}
			} else {
				entry = settinig_drawer_hash_find(ad, "Default");
				if (entry) {
					item_data = entry->ui_handler(ad, keyStr, icon_path, ug_args, genlist, NULL);
					entry->item_data = item_data;
				}
			}

			/* dupcated code (REFACTORING) */
			if (item_data && item_data->item) {

				set_node_pointer(keyStr, item_data->item);
				index++;
			}
		}
	}
	return genlist;
}

static Eina_Bool _navibar_back_pop_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, EINA_FALSE);
	setting_main_appdata *ad = (setting_main_appdata *) data;

	if (ad && ad->md.win_main) {
		elm_win_lower(ad->md.win_main);
	}

	SETTING_TRACE_END;
	return EINA_FALSE;
}


/**
 *
 * old : 1line -----------> new : type1
 * old : groupindex ------> new : group_index
 */
void _setting_genlist_itc_init(void *cb)
{
	setting_main_appdata *ad = (setting_main_appdata *) cb;

	/* [UI] create structures for genlist style */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(ad->md.itc_table[GENDIAL_Type_1text_1icon_2]));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(ad->md.itc_table[GENDIAL_Type_1icon_2text]));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(ad->md.itc_table[GENDIAL_Type_expandable_proc]));
}

static void _setting_tapi_init(void *cb)
{
	setting_main_appdata *ad = (setting_main_appdata *) cb;
	/*pass NULL to let TAPI access default module */
	ad->md.handle = tel_init(NULL);
	if (!ad->md.handle) {
		SETTING_TRACE_ERROR("*** [ERR] tel_init failed ***");
	} else {
		SETTING_TRACE("tel_init ok[handle:%p]", ad->md.handle);
	}
}

static void _setting_tapi_deinit(void *cb)
{
	setting_main_appdata *ad = (setting_main_appdata *) cb;

	if (ad->md.handle) {
		int tapi_ret = tel_deinit(ad->md.handle);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG("*** [ERR] tel_deinit. [%d] ***", tapi_ret);
		} else {
			SETTING_TRACE("***  tel_deinit OK ");
		}
		ad->md.handle = NULL;
	}
}

/* "virtualkeypad,state,on" */
static void ___title_toolbar_show(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *) data;

	Evas_Object *navi_bar = NULL;

	if (ad->isInUGMode && ad->ug) {
		Evas_Object *layout = (Evas_Object *)ug_get_layout(ad->ug);
		if (layout) {
			navi_bar = elm_object_part_content_get(layout, "elm.swallow.content");
		}
	} else {
		navi_bar = ad->md.navibar_main;
	}

	const char *support = evas_object_data_get(navi_bar, "sip.naviframe.title_obj");
	SETTING_TRACE("support:%s", support);
	Elm_Object_Item *top_item = elm_naviframe_top_item_get(navi_bar);
	if (0 == safeStrCmp("SEARCH", support)) {
		/* do nothing */
	} else if (support) {
		if (top_item) {
			elm_object_item_signal_emit(top_item, "elm,state,sip,shown", "");
		}
	}
}

/* "virtualkeypad,state,off" */
static void ___title_toolbar_hide(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *) data;

	Evas_Object *navi_bar = NULL;

	if (ad->isInUGMode && ad->ug) {
		Evas_Object *layout = (Evas_Object *)ug_get_layout(ad->ug);
		if (layout) {
			navi_bar = elm_object_part_content_get(layout, "elm.swallow.content");
		}
	} else {
		navi_bar = ad->md.navibar_main;
	}

	const char *support = evas_object_data_get(navi_bar, "sip.naviframe.title_obj");

	SETTING_TRACE("support:%s", support);
	Elm_Object_Item *top_item = elm_naviframe_top_item_get(navi_bar);
	if (0 == safeStrCmp("SEARCH", support)) {
		/* do nothing */
	} else if (support) {
		if (top_item) {
			elm_object_item_signal_emit(top_item, "elm,state,sip,hidden", "");
		}
	}
}

Evas_Object *setting_main_layout_conform_create(Evas_Object *win_layout, Evas_Object *win_obj, void *data)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	Evas_Object *layout = NULL;

	Evas_Object *conform = NULL;
	conform = elm_conformant_add(win_obj);

	setting_main_appdata *ad = (setting_main_appdata *) data;
	if (!conform) {
		return NULL;
	}

	SETTING_TRACE_DEBUG("Add conformant!!!!!");

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

	evas_object_smart_callback_add(conform, "virtualkeypad,state,on", ___title_toolbar_show, ad);
	evas_object_smart_callback_add(conform, "virtualkeypad,state,off", ___title_toolbar_hide, ad);

	elm_win_conformant_set(ad->md.win_main, EINA_TRUE);
	evas_object_show(conform);

	/* Indicator bg */
	Evas_Object *indicator_bg = elm_bg_add(conform);
	elm_object_style_set(indicator_bg, "indicator/headerbg");
	elm_object_part_content_set(conform, "elm.swallow.indicator_bg", indicator_bg);
	evas_object_show(indicator_bg);

	ad->md.conform = conform;
	evas_object_data_set(win_obj, "conformant", conform);
	return layout;
}

/**
* @brief create all view layout
*
* win_main
* conformant
* ly_main
* navibar <- view_layout 	<- "elm.swallow.bg" 		: bg
* 					  		<- "elm.swallow.content" 	: all_list
*                        	<- "elm.swallow.left"		: left_panel
*/
static int setting_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_main_appdata *ad = (setting_main_appdata *) cb;

	_setting_tapi_init(ad);
	_setting_genlist_itc_init(ad);
	settinig_drawer_hash_init(ad);

	/* create a layout conformant */
	ad->md.ly_main = setting_main_layout_conform_create(ad->md.win_main, ad->md.win_main, ad);
	setting_retvm_if(ad->md.ly_main == NULL, FALSE, "ly_main == NULL");


	/* create a navibar */
	ad->md.navibar_main = setting_create_navi_bar(ad->md.ly_main);
	setting_retvm_if(ad->md.navibar_main == NULL, FALSE, "navibar_main == NULL");
	setting_conformant_keypad_state(ad->md.win_main, TRUE);


	/* LAYOUT */
	Evas_Object *view_layout = elm_layout_add(ad->md.navibar_main);
	setting_retvm_if(view_layout == NULL, FALSE, "view_layout == NULL");
	elm_layout_theme_set(view_layout, "layout", "application", "default");
	evas_object_show(view_layout);
	ad->md.view_layout = view_layout;

	/* push a view to the naviframe */
	Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->md.navibar_main, _("IDS_ST_OPT_SETTINGS"), NULL, NULL, view_layout, NULL);
	elm_naviframe_item_title_enabled_set(navi_it, EINA_TRUE, EINA_TRUE);
	ad->md.navibar_main_it = navi_it;
	elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE, EINA_TRUE);
	elm_naviframe_item_pop_cb_set(navi_it, _navibar_back_pop_cb, ad);

	if (!is_searchmode_app(ad->is_searchmode)) {

		Evas_Object *all_list = NULL;

		all_list = _view_list_geter(ad);

		setting_retvm_if(all_list == NULL, FALSE, "all_list == NULL");
		elm_object_part_content_set(view_layout, "elm.swallow.content", all_list);

	}

	setting_view_main.is_create = 1;
	LAUNCH_SETTING_OUT();
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_main_appdata *ad = (setting_main_appdata *) cb;
	if (0 == setting_view_main.is_create) {
		/*already not exsit */
		return SETTING_RETURN_SUCCESS;
	}
	evas_object_smart_callback_del(ad->md.conform, "virtualkeypad,state,on", ___title_toolbar_show);
	evas_object_smart_callback_del(ad->md.conform, "virtualkeypad,state,off", ___title_toolbar_hide);
	evas_object_data_set(ad->md.win_main, "conformant", NULL);
	_setting_tapi_deinit(ad);

	if (ad->event_freeze_timer) {
		ecore_timer_del(ad->event_freeze_timer);
		ad->event_freeze_timer = NULL;
	}
	/*evas_object_smart_callback_del(ad->md.main_genlist, "realized", __gl_realized_cb); */

	elm_object_part_content_unset(ad->md.view_layout, "elm.swallow.content");
	elm_object_item_part_content_unset(ad->md.navibar_main_it, "tabbar");
#if 0
	if (ad->md.sc_gl[SC_All_List]) {
		evas_object_del(ad->md.sc_gl[SC_All_List]);
		ad->md.sc_gl[SC_All_List] = NULL;
	}
#endif
	/* sound */
	effect_playsound_close();

	/* then destroy the UI object. */
	if (ad->md.ly_main) {
		evas_object_del(ad->md.ly_main);
		ad->md.ly_main = NULL;
	}

	setting_view_main.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_main_appdata *ad = (setting_main_appdata *) cb;

	if (ad->md.ly_main != NULL) {
		/*evas_object_show(ad->md.ly_main); */
	}

	return SETTING_RETURN_SUCCESS;
}



static int setting_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_main_appdata *ad = (setting_main_appdata *) cb;

	if (ad->md.ly_main != NULL) {
		/*evas_object_hide(ad->md.ly_main); */
	}

	return SETTING_RETURN_SUCCESS;
}


/* ***************************************************
 *
 *general func
 *
 ***************************************************/
/*/////////////////////////////////////////////////////////////////////////////////// */


