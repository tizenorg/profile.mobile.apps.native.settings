/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
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
#include "setting-storage-utils.h"
#include "setting-storage-async-worker.h"
#include "setting-storage-miscellaneous.h"


#include <media_content.h>
#include <efl_extension.h>

#define STR_DELETE_ITEMS	"IDS_ST_HEADER_DELETE_ITEMS_ABB2"
typedef struct {
	char *id;
	char *path;
	char *name;
	Setting_GenGroupItem_Data *data;
	SettingStorageUG *ad;
} Node_Info;


static void storageUg_misces_sel_all_clicked(void *data, Evas_Object *obj, void *event_info);
static inline void storageUg_misces_gl_append_select_all_item(SettingStorageUG *ad);
static inline void storageUg_misces_gl_remove_select_all_item(SettingStorageUG *ad);

static setting_view setting_view_storage_misc;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Evas_Object* ctxpopup;

static void
ctxpopup_dismissed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	evas_object_del(ctxpopup);
	ctxpopup = NULL;
}

static void
move_more_ctxpopup(Evas_Object *ctxpopup)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *win;
	Evas_Coord w, h;
	int pos = -1;

	/* We convince the top widget is a window */
	win = elm_object_top_widget_get(ctxpopup);
	elm_win_screen_size_get(win, NULL, NULL, &w, &h);
	pos = elm_win_rotation_get(win);

	switch (pos) {
		case 0:
		case 180:
			evas_object_move(ctxpopup, (w / 2), h);
			break;
		case 90:
			evas_object_move(ctxpopup,  (h / 2), w);
			break;
		case 270:
			evas_object_move(ctxpopup, (h / 2), w);
			break;
	}
}

static void
naviframe_resize_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *ctxpopup = data;
	move_more_ctxpopup(ctxpopup);
}

static void
more_ctxpopup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *nf = data;
	evas_object_event_callback_del_full(nf, EVAS_CALLBACK_RESIZE, naviframe_resize_cb, ctxpopup);
}

//static inline void storageUg_misces_gl_remove_select_all_item(SettingStorageUG *ad);
static void _genlist_check_hide(SettingStorageUG *ad, bool hide)
{
	SETTING_TRACE_BEGIN;
	int count = 0;
	Evas_Object *gl = ad->gl_misces;
	Elm_Object_Item *it = elm_genlist_first_item_get(gl);
	while (it)
	{
		Setting_GenGroupItem_Data *sel = (Setting_GenGroupItem_Data *)elm_object_item_data_get(it);
		//sel->isItemHideFlag = 0;
		sel->isItemHideFlag = hide;

		// reset if check boxs get hidden
		if (hide == true) {
			sel->chk_status = 0; 
		}
		SETTING_TRACE(" item_update %d ", count++);
		elm_genlist_item_fields_update(it, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);
		it = elm_genlist_item_next_get(it);
	}
}

/**
 * [UI-CALLBACK] 'Delete' button handler on ctxpopup
 */
static void
setting_storage_ctx_click_softkey_cb(void *data,
                                                 Evas_Object *obj,
                                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingStorageUG  *ad = (SettingStorageUG*) data;

	//-------------------------------------------------------------------------------------------
	// check genlist check box show/hide
	//-------------------------------------------------------------------------------------------
	ad->misces_ctx_popup_selected = true;

#if 0
	int count = 0;
	Eina_List *l = elm_genlist_realized_items_get(ad->gl_misces);
	Elm_Object_Item *it;
	EINA_LIST_FREE(l, it)
	{
		Setting_GenGroupItem_Data *sel = (Setting_GenGroupItem_Data *)elm_object_item_data_get(it);

		sel->isItemHideFlag = 0;

		SETTING_TRACE(" item_update %d ", count++);
		elm_genlist_item_fields_update(it, "elm.swallow.end", ELM_GENLIST_ITEM_FIELD_CONTENT);

	}
#else
	storageUg_misces_gl_append_select_all_item(ad);
	_genlist_check_hide(ad, false);
#endif

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}



/* Icon + Text (More button style : Naviframe Toolbar) */
static void create_ctxpopup_more_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG  *ad = (SettingStorageUG*) data;
	Evas_Object *nf = ad->navi;

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
	}

	ctxpopup = elm_ctxpopup_add(nf);
	elm_ctxpopup_auto_hide_disabled_set(ctxpopup, EINA_TRUE);
	elm_object_style_set(ctxpopup, "more/default");
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_MORE, eext_ctxpopup_back_cb, NULL);
	evas_object_smart_callback_add(ctxpopup, "dismissed", ctxpopup_dismissed_cb, NULL);
	evas_object_event_callback_add(ctxpopup, EVAS_CALLBACK_DEL, more_ctxpopup_del_cb, nf);
	evas_object_event_callback_add(nf, EVAS_CALLBACK_RESIZE, naviframe_resize_cb, ctxpopup);

	//---------------------------------------------------------------------------------------------
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BODY_DELETE"), NULL, setting_storage_ctx_click_softkey_cb, ad);
	//---------------------------------------------------------------------------------------------

	elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	move_more_ctxpopup(ctxpopup);
	evas_object_show(ctxpopup);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
 * back button handler
 *
 * 1. "default" -> delete : back to default
 */
static Eina_Bool storageUg_misces_back_cb(void *data, Elm_Object_Item *it)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, EINA_TRUE);
	retv_if(NULL != ad->del_worker, EINA_FALSE);

	setting_view_change(ad->misces_view, ad->main_view, ad);

	return EINA_TRUE;
}

static inline Evas_Object *_create_ly_misces_no_item(Evas_Object *parent)
{
	Evas_Object *layout, *icon;

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	icon = elm_image_add(layout);
	elm_image_file_set(icon, SETTING_ICON_PATH_CFG "/00_nocontents_text.png", NULL);
	elm_object_part_content_set(layout, "nocontents.image", icon);

	elm_object_domain_translatable_part_text_set(layout, "elm.text", SETTING_PACKAGE,
	                                             STORAGEUG_STR_NO_FILES);
	elm_layout_signal_emit(layout, "text,disabled", "");

	return layout;
}

static inline Evas_Object *_create_ly_app_default(Evas_Object *parent)
{
	Evas_Object *layout;

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "application", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	return layout;
}

static inline void _update_navibar_buttons(SettingStorageUG *ad)
{
	/*update delete item buttons*/
	if (ad->misces_checked > 0) {
		if (ad->misces_navi_it && elm_object_item_part_content_get(ad->misces_navi_it, "title_left_btn") == NULL) {
			elm_object_item_part_content_set(ad->misces_navi_it, "title_left_btn", ad->title_left_btn);
		}
		if (ad->misces_navi_it && elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn") == NULL) {
			elm_object_item_part_content_set(ad->misces_navi_it, "title_right_btn", ad->title_right_btn);
		}
	} else {
		if (ad->misces_navi_it && elm_object_item_part_content_get(ad->misces_navi_it, "title_left_btn") != NULL) {
			elm_object_item_part_content_unset(ad->misces_navi_it, "title_left_btn");
			evas_object_hide(ad->title_left_btn);
		}
		if (ad->misces_navi_it && elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn") != NULL) {
			elm_object_item_part_content_unset(ad->misces_navi_it, "title_right_btn");
			evas_object_hide(ad->title_right_btn);
		}
	}
}

static bool storageUg_misces_media_item_cb(media_info_h media, void *data)
{
	Node_Info *node;
	char *media_id = NULL;
	char *file_path = NULL;
	char *display_name = NULL;
	SettingStorageUG *ad = data;

	retv_if(NULL == media, true);
	retv_if(NULL == data, false);

	media_info_get_file_path(media, &file_path);
	if (!ecore_file_exists(file_path)) {
		FREE(file_path);
		return true;
	}

	media_info_get_media_id(media, &media_id);
	media_info_get_display_name(media, &display_name);

	node = calloc(1, sizeof(Node_Info));
	if (node) {
		node->id = strdup(media_id);
		node->path = strdup(file_path);
		node->name = strdup(display_name);
		ad->misces_list = eina_list_append(ad->misces_list, node);
	}
	FREE(media_id);
	FREE(file_path);
	FREE(display_name);

	return true;
}

static void storageUg_misces_clear_list(Eina_List *list)
{
	Eina_List *l = NULL;
	Node_Info *node = NULL;

	EINA_LIST_FOREACH(list, l, node) {
		free(node->id);
		free(node->path);
		free(node->name);
		free(node);
	}
	eina_list_free(list);
}

static inline void storageUg_meisces_get_list(SettingStorageUG *ad)
{
	int ret;
	char *condition;
	filter_h filter = NULL;

	ret_if(NULL == ad);

	if (ad->misces_list) {
		storageUg_misces_clear_list(ad->misces_list);
		ad->misces_list = NULL;
	}

	/*Set Filter*/
	condition = "(MEDIA_TYPE=4) and MEDIA_PATH LIKE \'/opt/usr/media/%%\'";
	/*
	   condition = "(MEDIA_TYPE=4) and (MEDIA_PATH LIKE \'/opt/usr/media/.tmp_download/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/.video-hub/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Camera/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Downloads/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Images/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Others/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Sounds/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/Videos/%%\'"
	   "or MEDIA_PATH LIKE \'/opt/usr/media/lost+found/%%\')";
	   */

	/*Set Filter*/
	ret = media_filter_create(&filter);
	if (ret != MEDIA_CONTENT_ERROR_NONE) {
		SETTING_TRACE_ERROR("media_filter_create() Fail(%d)", ret);
		return;
	}
	ret = media_filter_set_condition(filter, condition, MEDIA_CONTENT_COLLATE_DEFAULT);
	if (ret != MEDIA_CONTENT_ERROR_NONE) {
		media_filter_destroy(filter);
		SETTING_TRACE_ERROR("media_filter_set_condition() Fail(%d)", ret);
		return;
	}

	ret = media_info_foreach_media_from_db(filter, storageUg_misces_media_item_cb, ad);
	if (ret != MEDIA_CONTENT_ERROR_NONE) {
		media_filter_destroy(filter);
		SETTING_TRACE_ERROR("media_info_foreach_media_from_db() Fail(%d)", ret);
		return;
	}

	ret = media_filter_destroy(filter);
	if (ret != MEDIA_CONTENT_ERROR_NONE) {
		SETTING_TRACE_ERROR("media_filter_destroy() Fail(%d)", ret);
		return;
	}

	ad->misces_sz_all = eina_list_count(ad->misces_list);
}

static void storageUg_misces_handle_selecting(SettingStorageUG *ad)
{
	Evas_Object *toolbar;

	ret_if(NULL == ad);

	if (ad->misces_checked == ad->misces_sz_all)
		ad->misces_is_select_all = EINA_TRUE;
	else
		ad->misces_is_select_all = EINA_FALSE;

	/*  text set and timeout set */
	if (0 < ad->misces_checked) {
		char text[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};

		snprintf(text, sizeof(text), _(STORAGEUG_STR_N_SELECTED), ad->misces_checked);
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", text);

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	} else {
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", "");
		elm_object_item_signal_emit(ad->misces_navi_it, "elm,state,subtitle,hide", "elm");

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
	}
}

static int storageUg_compare_str(const void *data1, const void *data2)
{
	return safeStrCmp(data1, data2);
}

static void storageUg_misces_handle_sel_list(SettingStorageUG *ad, char *id,
                                             int checked)
{
	SETTING_TRACE_BEGIN;

	if (checked) {
		SETTING_TRACE("----- checked : %d ", ad->misces_checked);
		ad->misces_checked++;
		ad->misces_sel_list = eina_list_append(ad->misces_sel_list, strdup(id));
	} else {
		SETTING_TRACE("----- un-checked : %d", ad->misces_checked);
		Eina_List *found;

		ad->misces_checked--;
		found = eina_list_search_unsorted_list(ad->misces_sel_list, storageUg_compare_str,
		                                       id);
		free(eina_list_data_get(found));
		ad->misces_sel_list = eina_list_remove_list(ad->misces_sel_list, found);
	}

	/*update delete item buttons*/
	_update_navibar_buttons(ad);
}

static void storageUg_misces_genlist_sel(void *data, Evas_Object *obj,
                                         void *event_info)
{
	SETTING_TRACE_BEGIN;
	Node_Info *node;
	SettingStorageUG *ad = data;
	Elm_Object_Item *item = event_info;
	Setting_GenGroupItem_Data *d_item = NULL;

	ret_if(data == NULL);

	elm_genlist_item_selected_set(item, 0);
	d_item = elm_object_item_data_get(item);
	if (d_item) {
		node = d_item->userdata;

		setting_update_gl_item_chk_status(d_item, !(d_item->chk_status));

		storageUg_misces_handle_sel_list(ad, node->id, d_item->chk_status);
		storageUg_misces_handle_selecting(ad);
	}

#if 0
	if (NULL == ad->misces_lo_noitem) {
		ad->misces_lo_noitem = _create_ly_misces_no_item(ad->navi);

		Evas_Object *btn = elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn");
		elm_object_disabled_set(btn, EINA_TRUE);
		ad->gl_misces = NULL;

		if (ad->misces_lo_noitem) {
			elm_object_item_part_content_set(ad->misces_navi_it, "elm.swallow.content", ad->misces_lo_noitem);
		}
	}
#endif


}

static void storageUg_misces_genlist_chk_cb(void *data, Evas_Object *obj,
                                            void *event_info)
{
	SETTING_TRACE_BEGIN;

	Node_Info *node;
	SettingStorageUG *ad;
	Setting_GenGroupItem_Data *d_item = data;

	ret_if(data == NULL);

	node = d_item->userdata;
	if (node)
	{
		ad = node->ad;
		d_item->chk_status = elm_check_state_get(obj);

		storageUg_misces_handle_sel_list(ad, node->id, d_item->chk_status);
		storageUg_misces_handle_selecting(ad);
	} else {
		SETTING_TRACE_ERROR(">>>>>>>>> node is NULL");
	}
}



static inline void storageUg_misces_clear_sel_list(Eina_List *list)
{
	Eina_List *l = NULL;
	char *node = NULL;

	EINA_LIST_FOREACH(list, l, node)
	free(node);
	eina_list_free(list);
}



/**
 * "Select All" - check button handler
 */
static inline void setting_storage_select_all_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingStorageUG *ad = (SettingStorageUG *)list_item->userdata;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	ad->misces_is_select_all = elm_check_state_get(obj);

	SETTING_TRACE_ERROR("1");



	Eina_List *l = NULL;
	Evas_Object *toolbar;
	Node_Info *node = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;


	SETTING_TRACE_ERROR("2 : ad->misces_is_select_all--->%d", ad->misces_is_select_all);
	SETTING_TRACE_ERROR("ad->misces_list--->%x", ad->misces_list);
	EINA_LIST_FOREACH(ad->misces_list, l, node) {
		if (node && node->data) {
			item_data = node->data;
			SETTING_TRACE_ERROR("2.1");
			if (item_data) {
				item_data->chk_status = ad->misces_is_select_all;
				SETTING_TRACE_ERROR("2.2");
				elm_object_item_data_set(item_data->item, item_data);
				SETTING_TRACE_ERROR("2.3");
				storageUg_misces_handle_sel_list(ad, node->id, item_data->chk_status);
				SETTING_TRACE_ERROR("2.4");
			}
		}
	}

	SETTING_TRACE_ERROR("3");
	elm_genlist_realized_items_update(ad->gl_misces);

	SETTING_TRACE_ERROR("4");
	bool select_all_state = ad->misces_is_select_all;
	if (select_all_state) {
		char text[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};

		snprintf(text, sizeof(text), _(STORAGEUG_STR_N_SELECTED), ad->misces_sz_all);
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", text);

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
		ad->misces_checked = ad->misces_sz_all;
	} else {
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", "");
		elm_object_item_signal_emit(ad->misces_navi_it, "elm,state,subtitle,hide", "elm");

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		ad->misces_checked = 0;
		SETTING_TRACE_ERROR("********** SET TO ZERO ad->misces_checked = 0");
	}
	SETTING_TRACE_ERROR("5");
}

/**
 * Delete 'Select All' item in genlist
 */
static inline void storageUg_misces_gl_remove_select_all_item(SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *gl = ad->gl_misces;

	Elm_Object_Item *it = elm_genlist_first_item_get(gl);
	if (it) {
		elm_object_item_del	(it);
	}
}



/**
 * "Select All" check button
 */
static inline void storageUg_misces_gl_append_select_all_item(SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;
	int checked = 0;
	Setting_GenGroupItem_Data *item_data = NULL;
	//ad->gl_misces

	evas_object_data_set(ad->gl_misces, "InsertType", "Prepend");

	item_data = setting_create_Gendial_field_def(ad->gl_misces,
		                                             &(itc_1text_1icon), storageUg_misces_sel_all_clicked, ad,
		                                        	 SWALLOW_Type_1ICON_1RADIO, NULL, NULL, checked, _("IDS_ST_BODY_SELECT_ALL"), NULL,
		                                             setting_storage_select_all_chk_btn_cb);

	if (item_data) {
		item_data->userdata = ad;
		item_data->isItemHideFlag = TRUE;
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	evas_object_data_set(ad->gl_misces, "InsertType", "");
}



static inline void storageUg_misces_gl_append_items(SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;
	int checked;
	Node_Info *node;
	Eina_List *found, *cur, *tmp_sel_list;
	Setting_GenGroupItem_Data *item_data = NULL;

	tmp_sel_list = ad->misces_sel_list;
	ad->misces_sel_list = NULL;

	EINA_LIST_FOREACH(ad->misces_list, cur, node) {
		if (NULL == node)
			continue;

		found = eina_list_search_unsorted_list(tmp_sel_list, storageUg_compare_str,
		                                       node->id);
		if (found) {
			checked = 1;
			storageUg_misces_handle_sel_list(ad, node->id, 1);
		} else {
			checked = 0;
		}

		item_data = setting_create_Gendial_field_def(ad->gl_misces,
		                                             &(itc_1text_1icon), storageUg_misces_genlist_sel, ad,
		                                        	 SWALLOW_Type_1CHECK_RIGHT_INVISIBLE, NULL, NULL, checked, node->name, node->path,
		                                             storageUg_misces_genlist_chk_cb);

		if (item_data) {
			item_data->userdata = node;
			node->data = item_data;
			node->ad = ad;
			item_data->isItemHideFlag = TRUE;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}
	storageUg_misces_handle_selecting(ad);
	storageUg_misces_clear_sel_list(tmp_sel_list);
}

static inline Evas_Object *storageUg_misces_genlist(Evas_Object *parent)
{
	Evas_Object *genlist;

	genlist = elm_genlist_add(parent);
	if (NULL == genlist) {
		SETTING_TRACE_ERROR("elm_genlist_add() Fail");
		return NULL;
	}
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_block_count_set(genlist, 2);
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(genlist);

	return genlist;
}

static void storageUg_misces_delete_list(SettingStorageUG *ad)
{
	SETTING_TRACE("Entering storageUg_misces_delete_list");

	ret_if(NULL == ad);

	ad->misces_checked = 0;
	Setting_GenGroupItem_Data *item_data = NULL;

	/* Elm_Object_Item *it = elm_genlist_first_item_get(ad->gl_misces); */
	Elm_Object_Item *it = elm_genlist_nth_item_get(ad->gl_misces, 1); /* second */

	while (it) {
		item_data = elm_object_item_data_get(it);
		it = elm_genlist_item_next_get(it);
		if (item_data != NULL) {
			SETTING_TRACE("chk_status: %d", item_data->chk_status);
			if (item_data->chk_status) {
				elm_object_item_del(item_data->item);
				Node_Info *node = item_data->userdata;
				if (node)
				{
					ad->misces_list = eina_list_remove(ad->misces_list, node);
					FREE(node->id);
					FREE(node->path);
					FREE(node->name);
					FREE(node);
				}
			}
		}
	}

	ad->misces_sz_all = eina_list_count(ad->misces_list);

	SETTING_TRACE("ad->misces_sz_all - %d", ad->misces_sz_all);
	if (0 == ad->misces_sz_all) {

		if (elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn")) {
			Evas_Object *btn_done = elm_object_item_part_content_unset(ad->misces_navi_it, "title_right_btn");
			evas_object_hide(btn_done);
		}

		if (elm_object_item_part_content_get(ad->misces_navi_it, "title_left_btn")) {
			Evas_Object *btn_cancel = elm_object_item_part_content_unset(ad->misces_navi_it, "title_left_btn");
			evas_object_hide(btn_cancel);
		}
		if (NULL == ad->misces_lo_noitem) {
			ad->misces_lo_noitem = _create_ly_misces_no_item(ad->navi);
		}

		ad->gl_misces = NULL;

		if (ad->misces_lo_noitem) {
			elm_object_item_part_content_set(ad->misces_navi_it, "elm.swallow.content", ad->misces_lo_noitem);
		}
	}
}

/**
 *
 * int misces_sz_all
 */
static void storageUg_misces_add_list(SettingStorageUG *ad)
{
	Evas_Object *toolbar, *btn;
	Evas_Object *lo_new = NULL;

	SETTING_TRACE("Entering storageUg_misces_add_list");

	ret_if(NULL == ad);

	ad->misces_checked = 0;
	ad->misces_is_select_all = EINA_FALSE;

	if (0 == ad->misces_sz_all) {
		if (NULL == ad->misces_lo_noitem) {
			lo_new = ad->misces_lo_noitem = _create_ly_misces_no_item(ad->navi);

			btn = elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn");
			if (NULL != btn)
				elm_object_disabled_set(btn, EINA_TRUE);
			ad->gl_misces = NULL;
		}
	} else {
		if (ad->gl_misces)
			elm_genlist_clear(ad->gl_misces);
		else
			lo_new = ad->gl_misces = storageUg_misces_genlist(ad->navi);

		storageUg_misces_gl_append_items(ad);

		ad->misces_lo_noitem = NULL;
	}

	if (lo_new) {
		elm_object_item_part_content_set(ad->misces_navi_it, "elm.swallow.content", lo_new);
	}

	/* disable toolbar object */
	if (0 == ad->misces_checked) {
		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		if (NULL != toolbar)
			elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
	}
}

void storageUg_misces_delete_files_cb(int fn_result, SettingStorageUG *ad)
{
	ret_if(NULL == ad);

	ad->del_worker = NULL;

	/* update genlist */
	SETTING_TRACE("After deleting, updating the file list");

	storageUg_misces_delete_list(ad);

	if (SETTING_RETURN_SUCCESS != fn_result) {
		SETTING_TRACE_ERROR("storageUg_misces_delete_files() Fail(%d)", fn_result);
		storageUg_fail_popup(ad);
		return;
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->misces_navi_it) {
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", "");
		elm_object_item_signal_emit(ad->misces_navi_it, "elm,state,subtitle,hide", "elm");
	}
}


/**
 * [controller] delete actual files with ad->misces_list
 * 
 */
static int storageUg_misces_delete_files(SettingStorageUG *ad)
{
	Eina_List *l = NULL;
	Node_Info *node = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;

	retv_if(NULL == ad, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	EINA_LIST_FOREACH(ad->misces_list, l, node) {
		if (node) {
			item_data = node->data;

			if (!item_data->chk_status)
				continue;

			if (EINA_TRUE == ecore_file_remove(node->path)) {
				media_content_scan_file(node->path);
			} else {
				SETTING_TRACE_ERROR("failed to ecore_file_remove");
				return SETTING_RETURN_FAIL;
			}
		}
		storageUG_STOP_POINT;
	}

	return SETTING_RETURN_SUCCESS;
}


/**
 * [UI] Done button -> create popup -> button handler
 */
static void storageUg_misces_delete_resp(void *data, Evas_Object *obj,
                                         void *event_info)
{
	int response_type;
	SettingStorageUG *ad = data;

	ret_if(NULL == data);
	ret_if(NULL == obj);

	response_type = btn_type(obj);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK != response_type) {
		SETTING_TRACE("CANCEL");
		return;
	}

	SETTING_TRACE(">>>>> checked : %d before deletion ", ad->misces_checked);

	if (ad->misces_checked) {
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win, PROGRESSBAR_STYLE,
		                                                  NULL, NULL, storageUg_popup_del, 0, TRUE, TRUE, 0);

		if (ad->del_worker)
			storageUg_stop_async_worker(ad->del_worker);
		ad->del_worker = storageUg_start_async_worker(storageUg_misces_delete_files,
		                                              storageUg_misces_delete_files_cb, ad);
	} else {
		ad->popup = setting_create_popup(ad, ad->win, NULL,
										 STORAGEUG_STR_NO_SELECTED, storageUg_popup_del,
										 SETTING_STORAGE_POPUP_TIMER, FALSE, FALSE, 0);
	}
}


/**
 * Done button handler for 'delete'
 */
static void storageUg_misces_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = (SettingStorageUG *) data;

	ret_if(NULL == data);

	ad->popup = setting_create_popup(ad, ad->win, NULL,
									 STORAGEUG_STR_DELETE_Q, storageUg_misces_delete_resp,
									 0, FALSE, FALSE,
									 2, STORAGEUG_STR_DELETE, STORAGEUG_STR_CANCEL);
}


/**
 * Genlist Touch handler
 * @todo setting_storage_select_all_chk_btn_cb is similar to this function
 */
static void storageUg_misces_sel_all_clicked(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Eina_List *l = NULL;
	Evas_Object *toolbar;
	Node_Info *node = NULL;
	SettingStorageUG *ad = data;
	//Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	Setting_GenGroupItem_Data *item_data = NULL;

	SETTING_TRACE_ERROR("1");

	ret_if(data == NULL);

	ad->misces_is_select_all = elm_check_state_get(obj);

	SETTING_TRACE_ERROR("2 : ad->misces_is_select_all--->%d", ad->misces_is_select_all);
	SETTING_TRACE_ERROR("ad->misces_list--->%x", ad->misces_list);
	EINA_LIST_FOREACH(ad->misces_list, l, node) {
		if (node && node->data) {
			item_data = node->data;
			SETTING_TRACE_ERROR("2.1");
			if (item_data) {
				item_data->chk_status = (ad->misces_is_select_all == 1) ? 0 : 1;
				SETTING_TRACE_ERROR("2.2");
				elm_object_item_data_set(item_data->item, item_data);
				SETTING_TRACE_ERROR("2.3");
				storageUg_misces_handle_sel_list(ad, node->id, item_data->chk_status);
				SETTING_TRACE_ERROR("2.4");
			}
		}
	}

	SETTING_TRACE_ERROR("3");
	elm_genlist_realized_items_update(ad->gl_misces);

	SETTING_TRACE_ERROR("4");
	bool select_all_state = (ad->misces_is_select_all == 1) ? 0 : 1;

	if (select_all_state) {
		char text[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};

		snprintf(text, sizeof(text), _(STORAGEUG_STR_N_SELECTED), ad->misces_sz_all);
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", text);

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
		ad->misces_checked = ad->misces_sz_all;
	} else {
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", "");
		elm_object_item_signal_emit(ad->misces_navi_it, "elm,state,subtitle,hide", "elm");

		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		ad->misces_checked = 0;
		SETTING_TRACE_ERROR("********** SET TO ZERO ad->misces_checked = 0");
	}
	SETTING_TRACE_ERROR("5");
}

/**
 * Naviframe 'cancel' button handler
 */
static void
storageUg_misces_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingStorageUG *ad = data;

	//ad->misces_ctx_popup_selected = true;
	if (ad->misces_ctx_popup_selected) {

		SETTING_TRACE_ERROR("ad->misces_ctx_popup_selected == true");

		// change UI style
		ad->misces_ctx_popup_selected = false;

		// 1. remove 1st item of genlist
		storageUg_misces_gl_remove_select_all_item(ad);
		// 2. remove all 'toggle' from all list
		_genlist_check_hide(ad, true);

		// remove sub text of naviframe title
		elm_object_item_part_text_set(ad->misces_navi_it, "subtitle", "");
		elm_object_item_signal_emit(ad->misces_navi_it, "elm,state,subtitle,hide", "elm");

		Evas_Object *toolbar = NULL;
		toolbar = elm_object_item_part_content_get(ad->misces_navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		ad->misces_checked = 0;

		#if 0
		// delete all items
		if (ad->misces_list) {
			storageUg_misces_clear_list(ad->misces_list);
			ad->misces_list = NULL;
		}
		#endif

		// change button style
		_update_navibar_buttons(ad);

		SETTING_TRACE_ERROR("********** SET TO ZERO ad->misces_checked = 0");

	} else {
		SETTING_TRACE_ERROR("ad->misces_ctx_popup_selected == false, go naviframe_back");
		elm_naviframe_item_pop(ad->navi);
	}

	SETTING_TRACE_END;
}

//--------------------------------------------------
//	name: "storageview_layout";
//--------------------------------------------------
static int storageUg_misces_create(void *data)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*first get misc file count*/
	storageUg_meisces_get_list(ad);

	if (ad->misces_sz_all > 0) {

		/*misc file list*/
		storageUg_misces_add_list(ad);
		// focus a list
		elm_object_focus_set(ad->gl_misces, EINA_TRUE);

		// IDS_ST_BODY_MISCELLANEOUS_FILES
		// STR_DELETE_ITEMS
		ad->misces_navi_it = setting_push_layout_navi_bar(_(IDS_ST_BODY_MISCELLANEOUS_FILES),
															NULL, NULL, NULL,
															(setting_call_back_func)storageUg_misces_back_cb,
															NULL,
															NULL,
															ad, NULL, ad->navi, NULL);

		elm_object_item_part_content_set(ad->misces_navi_it, "elm.swallow.content", ad->gl_misces);

		/* Title Cancel Button */
		ad->title_left_btn = elm_button_add(ad->navi);
		elm_object_style_set(ad->title_left_btn, "naviframe/title_cancel");
		evas_object_smart_callback_add(ad->title_left_btn, "clicked", storageUg_misces_cancel_cb, ad);

		/* Title Done Button */
		ad->title_right_btn = elm_button_add(ad->navi);
		elm_object_style_set(ad->title_right_btn, "naviframe/title_done");
		evas_object_smart_callback_add(ad->title_right_btn, "clicked", storageUg_misces_delete_cb, ad);
	} else {

		ad->misces_navi_it = setting_push_layout_navi_bar(_(IDS_ST_BODY_MISCELLANEOUS_FILES),
															NULL,
															NULL,
															NULL,
															(setting_call_back_func)storageUg_misces_back_cb,
															NULL,
															NULL,
															ad, NULL, ad->navi, NULL);

		if (NULL == ad->misces_lo_noitem) {
			ad->misces_lo_noitem = _create_ly_misces_no_item(ad->navi);

			Evas_Object *btn = elm_object_item_part_content_get(ad->misces_navi_it, "title_right_btn");
			elm_object_disabled_set(btn, EINA_TRUE);
			ad->gl_misces = NULL;

			if (ad->misces_lo_noitem) {
				elm_object_item_part_content_set(ad->misces_navi_it, "elm.swallow.content", ad->misces_lo_noitem);
			}
		}
	}
	elm_naviframe_item_pop_cb_set(ad->misces_navi_it, storageUg_misces_back_cb, ad);

	// Add ctx popup handler
	Evas_Object* btn = elm_button_add(ad->navi);
	elm_object_style_set(btn, "naviframe/more/default");
	evas_object_smart_callback_add(btn, "clicked", create_ctxpopup_more_button_cb, ad);
	elm_object_item_part_content_set(ad->misces_navi_it, "toolbar_more_btn", btn);

	setting_view_storage_misc.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_misces_destroy(void *data)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->del_worker) {
		storageUg_stop_async_worker(ad->del_worker);
		ad->del_worker = NULL;
	}

	if (ad->misces_list) {
		storageUg_misces_clear_list(ad->misces_list);
		ad->misces_list = NULL;
	}

	if (ad->misces_sel_list) {
		storageUg_misces_clear_sel_list(ad->misces_sel_list);
		ad->misces_sel_list = NULL;
	}

	ad->misces_navi_it = NULL;
	ad->gl_misces = NULL;
	ad->misces_lo_noitem = NULL;
	ad->misces_checked = 0;

	setting_view_storage_misc.is_create = 0;
	elm_naviframe_item_pop(ad->navi);

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_misces_update(void *data)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	storageUg_misces_add_list(ad);
	return SETTING_RETURN_SUCCESS;
}

static int storageUg_misces_cleanup(void *data)
{
	return storageUg_misces_destroy(data);
}


void storageUg_misc_init(SettingStorageUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->misces_view);

	setting_view_storage_misc.create = storageUg_misces_create;
	setting_view_storage_misc.destroy = storageUg_misces_destroy;
	setting_view_storage_misc.update = storageUg_misces_update;
	setting_view_storage_misc.cleanup = storageUg_misces_cleanup;

	ad->misces_view = &setting_view_storage_misc;
}

