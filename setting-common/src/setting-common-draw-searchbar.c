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
#include <setting-common-draw-widget.h>
#include <glib.h>
#include <efl_extension.h>
#include <efl_assist.h>

/**
* Get searchbar text and register redraw_cb by idler.
*
* @param[in] data
* @param[in] search_bar
* @param[in] search_text
* @param[in] search_idler
* @param[in] redraw_cb
*/
EXPORT_PUBLIC
int setting_searchbar_redraw(void *data,
                             Evas_Object *search_bar,
                             char *search_text,
                             Ecore_Idler **search_idler,
                             Ecore_Task_Cb redraw_cb)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(search_bar == NULL, -1, "search_bar parameter is NULL");
	/*const char *str = elm_object_text_get(search_bar); */
	Evas_Object *entry = elm_object_part_content_get(search_bar, "elm.swallow.content");
	const char *str = elm_entry_entry_get(entry);
	int str_len = safeStrLen(str);
	setting_retvm_if(str_len > MAX_SEARCH_STR_LEN, -1, "str_len > MAX_SEARCH_STR_LEN");

	memset(search_text, '\0', MAX_SEARCH_STR_LEN + 1);
	safeCopyStr(search_text, str, MAX_SEARCH_STR_LEN);

	SETTING_TRACE("Update the search view text, search_text=%s",
	              search_text);

	if (*search_idler) {
		ecore_idler_del(*search_idler);
		*search_idler = NULL;
	}
	*search_idler = ecore_idler_add(redraw_cb, data);
	return 0;
}


static void __searchbar_chg_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *searchbar_layout = data;
	if (elm_object_focus_get(searchbar_layout)) {
		if (elm_entry_is_empty(obj)) {
			elm_object_signal_emit(searchbar_layout, "elm,state,eraser,hide", "elm");
		} else
			elm_object_signal_emit(searchbar_layout, "elm,state,eraser,show", "elm");
	}
	if (!elm_entry_is_empty(obj))
		elm_object_signal_emit(searchbar_layout, "elm,state,guidetext,hide", "elm");
}


static void __searchbar_focused_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(!data || !obj);
	Evas_Object *searchbar_layout = data;
	if (!elm_entry_is_empty(obj)) {
		elm_object_signal_emit(searchbar_layout, "elm,state,eraser,show", "elm");
	}
	elm_object_signal_emit(searchbar_layout, "elm,state,guidetext,hide", "elm");
	/*elm_object_signal_emit(searchbar_layout, "cancel,in", ""); */
	elm_object_signal_emit(obj, "elm,state,focus,on", "");
}


static void __searchbar_unfocused_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(!data || !obj);
	Evas_Object *searchbar_layout = data;
	if (elm_entry_is_empty(obj))
		elm_object_signal_emit(searchbar_layout, "elm,state,guidetext,show", "elm");
	elm_object_signal_emit(searchbar_layout, "elm,state,eraser,hide", "elm");
	elm_object_signal_emit(obj, "elm,state,focus,off", "");
}


static void __searchbar_activated_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(!obj);
	Evas_Object *searchbar_layout = data;
	Evas_Object *entry = elm_object_part_content_get(searchbar_layout, "elm.swallow.content");
	ret_if(!entry);
	elm_object_focus_set(entry, EINA_FALSE);
}

#if 0
static void __searchbar_eraser_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ret_if(!data);
	Evas_Object *entry = data;
	elm_entry_entry_set(entry, "");
}
#endif

#if 0
static void __searchbar_bg_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ret_if(!data);
	Evas_Object *entry = data;
	elm_object_focus_set(entry, EINA_TRUE);
}
#endif

#if 0
static void __searchbar_cancel_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(!data || !obj);
	evas_object_hide(obj);
	Evas_Object *searchbar_layout = data;
	/*elm_object_signal_emit(searchbar_layout, "cancel,out", ""); */
	Evas_Object *entry = elm_object_part_content_get(searchbar_layout, "elm.swallow.content");
	ret_if(!entry);
	const char *text = elm_entry_entry_get(entry);
	if (text != NULL && strlen(text) > 0)
		elm_entry_entry_set(entry, NULL);
	elm_object_focus_set(entry, EINA_FALSE);
}
#endif

static void __searchbar_searchsymbol_clicked_cb(void *data, Evas_Object *obj, const char *emission, const char *source)
{
	ret_if(!data);
	Evas_Object *entry = data;
	elm_object_focus_set(entry, EINA_TRUE);
}

static void ___searchbar_input_panel_event_cb(void *data, Ecore_IMF_Context *ctx, int value)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(ctx == NULL, "obj parameter is NULL");
	Evas_Object *search_entry = (Evas_Object *)data;

	if (value == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
		SETTING_TRACE("value == ECORE_IMF_INPUT_PANEL_STATE_SHOW");
		elm_object_focus_set(search_entry, EINA_TRUE);
	} else if (value == ECORE_IMF_INPUT_PANEL_STATE_HIDE) {
		SETTING_TRACE("value == ECORE_IMF_INPUT_PANEL_STATE_HIDE");
		elm_object_focus_set(search_entry, EINA_FALSE);
	}
}


static void __searchbar_entry_max_len_reached(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/*retm_if(data == NULL, "Data parameter is NULL"); */
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");/*notify only when entry is being focused on. */
	setting_create_indicator_notification(_(EXCEED_LIMITATION_STR));
}



/**
* Create searchar, register change cb and cancel cb.
*
* @param[in] data
* @param[in] parent
* @param[in] change_cb
* @param[in] cancel_cb
*/
EXPORT_PUBLIC
Evas_Object *setting_create_searchbar(void *data, Evas_Object *parent,
                                      Evas_Smart_Cb change_cb,
                                      Evas_Smart_Cb cancel_cb)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(parent == NULL, NULL, "parent parameter is NULL");
	Evas_Object *searchbar_layout = elm_layout_add(parent);

	elm_layout_file_set(searchbar_layout, SETTING_THEME_EDJ_NAME, "region_searchbar");
	elm_object_part_content_set(parent, "searchbar", searchbar_layout);
#if 0
	Evas_Object *entry = ea_editfield_add(searchbar_layout, EA_EDITFIELD_SEARCHBAR_FIXED_SIZE);

	if (change_cb) {
		evas_object_smart_callback_add(entry, "changed", change_cb, data);
	} else {
		evas_object_smart_callback_add(entry, "changed", __searchbar_chg_cb, searchbar_layout);
	}

	evas_object_smart_callback_add(entry, "focused", __searchbar_focused_cb, searchbar_layout);
	evas_object_smart_callback_add(entry, "unfocused", __searchbar_unfocused_cb, searchbar_layout);
	evas_object_smart_callback_add(entry, "activated", __searchbar_activated_cb, searchbar_layout);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);

	elm_object_part_content_set(searchbar_layout, "elm.swallow.content", entry);
	elm_object_part_text_set(entry, "elm.guide", _("IDS_ST_BODY_SEARCH"));
	elm_entry_input_panel_return_key_type_set(entry, ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH);
#endif
	Evas_Object *icon = setting_create_icon(searchbar_layout, SETTING_ICON_PATH_CFG"A01-1_title_icon_search.png",
	                                        NULL, NULL, NULL, NULL);
	setting_decorate_image_RGBA(icon, 14, 41, 73, 255);

	elm_object_part_content_set(searchbar_layout, "elm.swallow.search.icon", icon);
	/*"elm.swallow.search.icon" */

#if 0
	/* is this required ? */
	elm_object_signal_callback_add(searchbar_layout, "elm,eraser,clicked", "elm", __searchbar_eraser_clicked_cb, entry);
	elm_object_signal_callback_add(searchbar_layout, "elm,bg,clicked", "elm", __searchbar_bg_clicked_cb, entry);
	evas_object_size_hint_weight_set(searchbar_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(searchbar_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);


	Elm_Entry_Filter_Limit_Size *limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (limit_filter_data) {
		limit_filter_data->max_char_count = 0;
		limit_filter_data->max_byte_count = 20;
		elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, limit_filter_data);
		evas_object_smart_callback_add(entry, "maxlength,reached", __searchbar_entry_max_len_reached, NULL);
	}

	/*Evas_Object *cancel_btn = elm_button_add(searchbar_layout);
	elm_object_part_content_set(searchbar_layout, "button_cancel", cancel_btn);
	elm_object_style_set(cancel_btn, "searchbar/default");
	elm_object_text_set(cancel_btn, _("IDS_ST_BUTTON_CANCEL_ABB"));
	elm_object_focus_allow_set(cancel_btn, EINA_FALSE);
	elm_object_signal_emit(searchbar_layout, "cancel,show", "");

	if (!cancel_cb){
		cancel_cb = __searchbar_cancel_clicked_cb;
	}
	evas_object_smart_callback_add(cancel_btn, "clicked", cancel_cb, searchbar_layout);*/
	evas_object_data_set(searchbar_layout, "entry", entry);
	elm_object_signal_callback_add(searchbar_layout, "elm,action,click", "", __searchbar_searchsymbol_clicked_cb, entry);

	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(entry);
	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_add(imf_context,
		                                                 ECORE_IMF_INPUT_PANEL_STATE_EVENT,
		                                                 ___searchbar_input_panel_event_cb,
		                                                 entry);
		SETTING_TRACE("OK to get imf_context -- elm_entry_imf_context_get");
	} else {
		SETTING_TRACE_ERROR("FAILED TO get imf_context -- elm_entry_imf_context_get");
	}

#endif
	return searchbar_layout;
}
