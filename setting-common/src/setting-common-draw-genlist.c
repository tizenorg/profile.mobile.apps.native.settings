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
#include <system_settings.h>
#include <efl_extension.h>
#include <efl_assist.h>
#include <setting-debug.h>

#define DEF_BUF_SIZE 32

static Evas_Object *_gl_Gendial_content_get(void *data, Evas_Object *obj, const char *part);
static char *_gl_Gendial_text_get(void *data, Evas_Object *obj, const char *part);
static void _gl_Gendial_del(void *data, Evas_Object *obj);

EXPORT_PUBLIC
const Elm_Genlist_Item_Class itc_seperator = {
	.item_style = "dialogue/separator.transparent.2",
	.func.text_get = NULL,
	.func.content_get = NULL,
	.func.state_get = NULL,
	.func.del = NULL,
};

EXPORT_PUBLIC
const Elm_Genlist_Item_Class itc_bottom_seperator = {
	.item_style = NULL,
	.func.text_get = NULL,
	.func.content_get = NULL,
	.func.state_get = NULL,
	.func.del = NULL,
};

EXPORT_PUBLIC
const Elm_Genlist_Item_Class itc_sep_line = {
	.item_style = "dialogue/separator.transparent.2",
	.func.text_get = _gl_Gendial_text_get,
	.func.content_get = NULL,
	.func.state_get = NULL,
	.func.del = NULL,
};

EXPORT_PUBLIC
const Elm_Genlist_Item_Class itc_bottom_line = {
	.item_style = "bottom_line",
	.func.text_get = NULL,
	.func.content_get = NULL,
	.func.state_get = NULL,
	.func.del = NULL,
};

#define DEFINE_ITC1(style, name) \
	EXPORT_PUBLIC const Elm_Genlist_Item_Class name = {\
	                                                   .item_style = style,\
	                                                   .func.text_get = _gl_Gendial_text_get,\
	                                                   .func.content_get = _gl_Gendial_content_get,\
	                                                   .func.state_get = NULL,\
	                                                   .func.del = _gl_Gendial_del,\
	                                                  };

DEFINE_ITC1("1line", itc_1text);
DEFINE_ITC1("1line", itc_1text_1icon_2);
DEFINE_ITC1("1line", itc_1text_1icon_2_tb);

DEFINE_ITC1("dialogue/editfield/title", itc_dialogue_editfield_title);
DEFINE_ITC1("1icon", itc_1icon);
DEFINE_ITC1("1line", itc_1icon_1text_sub);
DEFINE_ITC1("1line", itc_1text_1icon);
DEFINE_ITC1("1line", itc_1text_1icon_3);
DEFINE_ITC1("1line", itc_1text_2icon);
DEFINE_ITC1("1line", itc_1text_2icon_2);
DEFINE_ITC1("1line", itc_1text_2icon_10);

DEFINE_ITC1("2line.top", itc_2text_2);
DEFINE_ITC1("2line.top", itc_2text);
DEFINE_ITC1("multiline_sub.main", itc_multiline_2text_1icon);
DEFINE_ITC1("2line.top", itc_2text_1icon_3);
DEFINE_ITC1("2line.top", itc_2text_1icon_2);
DEFINE_ITC1("2line.top", itc_2text_1icon_6);
DEFINE_ITC1("2line.top", itc_2text_1icon_7);
DEFINE_ITC1("2line.top", itc_2text_1icon_10);

DEFINE_ITC1("2line.top.2", itc_2text_3_parent);
DEFINE_ITC1("2line.top", itc_1text_parent);

DEFINE_ITC1("2line.top", itc_2text_3);
DEFINE_ITC1("2line.top", itc_2text_2icon_3);

/*DEFINE_ITC1("multiline/1text", itc_multiline_text); */
DEFINE_ITC1("multiline_sub", itc_multiline_text);
DEFINE_ITC1("1line", itc_1text_1icon_3_tb);
DEFINE_ITC1("1line", itc_cm_1text_1icon_2);
DEFINE_ITC1("select_all", itc_sel_all);
/*DEFINE_ITC1("dialogue/bg/1icon", itc_bg_1icon); */
DEFINE_ITC1("1icon", itc_bg_1icon);
DEFINE_ITC1("groupindex", itc_group_item);
DEFINE_ITC1("1icon", itc_1icon_with_no_line);
DEFINE_ITC1("1icon", itc_1icon_with_no_padding);
DEFINE_ITC1("1line", itc_multiline_1text_1icon);
DEFINE_ITC1("1line", itc_1text_2icon_divider);
DEFINE_ITC1("1line", itc_no_group_1text_1icon_2);

DEFINE_ITC1("entry.main", itc_editfield);
DEFINE_ITC1("2line.top", itc_multiline_2text);
DEFINE_ITC1("1line", itc_1text_1icon_divider);
DEFINE_ITC1("1text", itc_normal_1text);

typedef struct _tagParamData {
	Setting_GenGroupItem_Data *item_data;
	Evas_Object *parent;
	/*	Evas_Object *btn_left; */
	/*	Evas_Object *btn_right; */
	Evas_Object *box;
} Datefield_Paramdata;


/**
 * Do process when clicking on a common genlist item
 */
EXPORT_PUBLIC
void setting_mouse_up_Gendial_list_radio_cb(void *data, Evas_Object *obj,
                                            void *event_info)
{
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	setting_retm_if(data == NULL, "Data parameter is NULL");
	Evas_Object *radio = data;
	setting_retm_if(NULL == list_item, "list_item is NULL");
	elm_radio_value_set(radio, list_item->chk_status);
}

static void __radio_changed(void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_radio_value_get(obj);	/*  for update */
}

static void __entry_changed(void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	list_item->sub_desc = (char *)g_strdup(elm_entry_entry_get(obj));
	Evas_Object *entry_container = elm_object_parent_widget_get(obj);
	if (entry_container) {
		if (elm_entry_is_empty(obj)) {
			elm_object_signal_emit(entry_container, "elm,state,guidetext,show", "elm");
			elm_object_signal_emit(entry_container, "elm,state,eraser,hide", "elm");
		} else {
			elm_object_signal_emit(entry_container, "elm,state,guidetext,hide", "elm");
			if (elm_object_focus_get(obj)) { /*only do when entry is focused */
				elm_object_signal_emit(entry_container, "elm,state,eraser,show", "elm");
			}
		}
	}
}

static void __entry_unfocused(void *data, Evas_Object *obj, void *event_info) /* Unfocused callback will show guidetext and hide X marked button. */
{
	ret_if(!data);
	SETTING_TRACE_BEGIN;
	Evas_Object *entry_container = data;
	/*whe entry unfocused, its guidetext will becomes "Input here" */

	if (elm_entry_is_empty(obj)) {
		elm_object_part_text_set(entry_container, "elm.guidetext", _("IDS_ST_BODY_TAP_TO_INSERT"));
		elm_object_signal_emit(entry_container, "elm,state,guidetext,show", "elm");
	}
	elm_object_signal_emit(entry_container, "elm,state,eraser,hide", "elm");
}

static void __entry_focused(void *data, Evas_Object *obj, void *event_info) /* Focused callback will show X marked button and hide guidetext. */
{
	ret_if(!data);
	SETTING_TRACE_BEGIN;
	Evas_Object *entry_container = data;
	/*whe entry focused, its guidetext will becomes "Input here" */

	if (!elm_entry_is_empty(obj)) {
		elm_object_signal_emit(entry_container, "elm,state,eraser,show", "elm");
	} else {
		elm_object_part_text_set(entry_container, "elm.guidetext", _("IDS_ST_BODY_TAP_TO_INSERT"));
	}
	elm_object_signal_emit(entry_container, "elm,state,guidetext,hide", "elm");

	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(obj);
	if (imf_context) ecore_imf_context_input_panel_show(imf_context);
}

static void __eraser_clicked(void *data, Evas_Object *obj, const char *emission, const char *source) /* When X marked button is clicked, empty entry's contents. */
{
	ret_if(!data);
	Evas_Object *entry = data;
	elm_entry_entry_set(entry, "");
	elm_object_focus_set(entry, EINA_TRUE);
}

static void __chk_changed(void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_check_state_get(obj);	/*  for update */
}

static void __error_popup_response_cb(void *data, Evas_Object *obj,
                                      void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	if (list_item->notify) {
		evas_object_del(list_item->notify);
		list_item->notify = NULL;
	}
}

static void __max_len_reached(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");/*notify only when entry is being focused on. */

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	if (!list_item->notify) {
		list_item->notify = setting_create_popup_without_btn(list_item, list_item->win_main,
		                                                     NULL, _(EXCEED_LIMITATION_STR),
		                                                     __error_popup_response_cb,
		                                                     POPUP_INTERVAL, FALSE, FALSE);
		elm_object_focus_set(list_item->eo_check, EINA_FALSE);
	} else {
		/*postpone 2 seconds again */
		elm_popup_timeout_set(list_item->notify, POPUP_INTERVAL);
	}

}

static void __entry_keydown(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(event_info == NULL, "event_info is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	Evas_Event_Key_Down *ev = (Evas_Event_Key_Down *)event_info;
	list_item->enterKeyPressFlag = FALSE;
	list_item->spaceKeyPressFlag = FALSE;
	if (safeStrCmp(ev->key, "KP_Enter") == 0 /*it is for Japanese keyboard to fix N_SE-10719 */
	    || safeStrCmp(ev->key, "Return") == 0) { /*to disable the 'Enter' orginal function */
		SETTING_TRACE("ENTER %s ev->key:%s", __FUNCTION__, ev->key);
		list_item->enterKeyPressFlag = TRUE;
		/*hide the eraser button */
		/*format like:xx<br/> */
		if (list_item->isSinglelineFlag == FALSE) {
			const char *entry_str = remove_first_substring(elm_entry_entry_get(obj), "<br/>");
			elm_entry_entry_set(obj, entry_str);
			FREE(entry_str);
			elm_entry_cursor_end_set(obj);
		}
		setting_hide_input_pannel_cb(obj);
	} else {
		if (safeStrCmp(ev->key, "space") == 0) {
			list_item->spaceKeyPressFlag = TRUE;
		} else {
			list_item->spaceKeyPressFlag = FALSE;
		}
	}
}

/**
 * Do process when clicking radio in subitem of expandable item
 */
EXPORT_PUBLIC
void setting_sub_list_rd_change(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_radio_value_get(obj);	/*  for update */

	Elm_Object_Item *subItem = list_item->item;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subItem);

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subItem);	/* subItem data */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */

	ret_if(NULL == data_subItem || NULL == data_parentItem);
	int err;
	setting_set_int_slp_key(data_parentItem->int_slp_setting_binded,
	                        list_item->chk_status, &err);
	ret_if(0 != err);
	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
}

/**
 * Do process when clicking on subitem of expandable item
 */
EXPORT_PUBLIC
void setting_sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem =
	    elm_object_item_data_get(subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);
	int err;
	setting_set_int_slp_key(data_parentItem->int_slp_setting_binded,
	                        data_subItem->chk_status, &err);
	setting_retm_if(0 != err, "Set vconf error[%d]", data_parentItem->int_slp_setting_binded);

	data_parentItem->sub_desc = (char *)g_strdup(data_subItem->keyStr);
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
	elm_genlist_item_expanded_set(parentItem, EINA_FALSE);
}

#if 0
static Eina_Bool __upadte_list_on_idler(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);
	Elm_Object_Item *item = data;
	elm_genlist_item_update(item);
	return ECORE_CALLBACK_CANCEL;
	/*return EINA_FALSE; */
}
#endif
static void __exp_list_smart_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL || event_info == NULL);
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(item);	/* parent data */
	char *cb_type = data;

	/*SETTING_TRACE_DEBUG("cb_type: %s", cb_type); */
	if (0 == safeStrCmp(cb_type, "drag")
	    || 0 == safeStrCmp(cb_type, "longpressed")) {
	} else if (0 == safeStrCmp(cb_type, "contracted")) {
		elm_genlist_item_subitems_clear(item);
		/*SETTING_TRACE_DEBUG("%s:%d", _(data_parentItem->keyStr), data_parentItem->con_style); */
		if (data_parentItem) {
			setting_genlist_item_groupstyle_set(data_parentItem, data_parentItem->con_style);
			const char *tmp = data_parentItem->r_swallow_path;
			data_parentItem->r_swallow_path = g_strdup(SETTING_IMAGE_PATH_CFG"00_list_button_expand_closed.png");
			elm_object_item_data_set(data_parentItem->item, data_parentItem);
			elm_genlist_item_update(data_parentItem->item);
			FREE(tmp);
		}
	} else if (0 == safeStrCmp(cb_type, "expanded")) {
		/*SETTING_TRACE_DEBUG("%s:%d", _(data_parentItem->keyStr), data_parentItem->con_style); */
		if (data_parentItem) {
			setting_genlist_item_groupstyle_set(data_parentItem, data_parentItem->exp_style);
			const char *tmp = data_parentItem->r_swallow_path;
			data_parentItem->r_swallow_path = g_strdup(SETTING_IMAGE_PATH_CFG"00_list_button_expand_opened.png");
			elm_object_item_data_set(data_parentItem->item, data_parentItem);
			elm_genlist_item_update(data_parentItem->item);
			FREE(tmp);
		}
	}
}

static void _gl_Gendial_sel(void *data, Evas_Object *obj, void *event_info)
{
	/* SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
}

static void _gl_Gendial_sel_expand(void *data, Evas_Object *obj,
                                   void *event_info)
{
	/* SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	bool status = !elm_genlist_item_expanded_get(item);
	elm_genlist_item_expanded_set(item, status);
}

static char *_gl_Gendial_text_get(void *data, Evas_Object *obj,
                                  const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) data;
	/* SETTING_TRACE("part:%s", part); */
	if (SWALLOW_Type_LAYOUT_ENTRY == item_data->swallow_type) {	/* no need to create 'text' part */
		return NULL;
	}

	char *ret_str = NULL;
	/* for font size view */
	if (SWALLOW_Type_1RADIO_1LABLE == item_data->swallow_type) {
		char speciliztion[MAX_SPECIALIZITION_LEN] = {0,};
		int font_size = -1;
		/*SETTING_TRACE("item_data->chk_status:%d", item_data->chk_status); */
		if (SYSTEM_SETTINGS_FONT_SIZE_SMALL == item_data->chk_status) {
			font_size = LABEL_FONT_SIZE_SMALL;
		} else if (SYSTEM_SETTINGS_FONT_SIZE_LARGE == item_data->chk_status) {
			font_size = LABEL_FONT_SIZE_LARGE;
		} else if (SYSTEM_SETTINGS_FONT_SIZE_HUGE == item_data->chk_status) {
			font_size = LABEL_FONT_SIZE_HUGE;
		} else if (SYSTEM_SETTINGS_FONT_SIZE_GIANT == item_data->chk_status) {
			font_size = LABEL_FONT_SIZE_GIANT;
		} else {
			font_size = LABEL_FONT_SIZE_NORMAL;
		}
		/*SETTING_TRACE("font_size:%d", font_size); */
		snprintf(speciliztion, sizeof(speciliztion), "<font_size=%d>%s</font_size>",
		         font_size, _(item_data->keyStr));
		ret_str = (char *)g_strdup(speciliztion);
		return ret_str;
	}

	/*char buf[1024]; */
	/*SETTING_TRACE("part:[%s], text:[%s] ", part, _(item_data->keyStr)); */
	if (!safeStrCmp(part, "elm.text.main") || !safeStrCmp(part, "elm.text.main.left") || !safeStrCmp(part, "elm.text.main.left.top")) {	/* title */
		if (item_data->keyStr) {
			ret_str = (char *)g_strdup(_(item_data->keyStr));	/* use item_data->keyStr */
		}

	} else if (!safeStrCmp(part, "elm.text.2") || !safeStrCmp(part, "elm.text.sub.left.bottom")) {	/* bottom or right */
		if (item_data->sub_desc) {
			ret_str = (char *)g_strdup(_(item_data->sub_desc));
		}

	} else if (!safeStrCmp(part, "elm.text.multiline")) {	/* title */
		if (item_data->keyStr) {
			ret_str = (char *)g_strdup(_(item_data->keyStr));
		}
	} else {
		FREE(ret_str);
		return NULL;
	}

	return ret_str;
}

static Evas_Object *__add_check(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *content = elm_layout_add(parent);
	Evas_Object *check = elm_check_add(content);
	/*elm_layout_theme_set(content, "layout", "list/C/type.2", "default"); */
	/* To fix check button flicker issue when rotate the screen*/
	if (item_data->isItemDisableFlag)
		elm_object_disabled_set(check, EINA_FALSE);

	elm_check_state_set(check, item_data->chk_status);
	evas_object_propagate_events_set(check, 0);
	evas_object_size_hint_align_set(check, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/*SETTING_TRACE("item_data->swallow_type:%d", item_data->swallow_type); */

	if (SWALLOW_Type_1ICON_1CHECK == item_data->swallow_type
	    || SWALLOW_Type_1CHECK == item_data->swallow_type
	    || SWALLOW_Type_1CHECKICON == item_data->swallow_type) {
		/*elm_object_style_set(check, "default/genlist"); */
		elm_check_state_pointer_set(check, (Eina_Bool *)(&(item_data->chk_status)));
		/*evas_object_repeat_events_set(check, EINA_TRUE); */
		evas_object_pass_events_set(check, 1);
	} else {
		evas_object_pass_events_set(check, 1);
	}


	item_data->eo_check = check;
	setting_genlist_unregister_tts_access_highlight(item_data->eo_check, item_data->itc);

	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(check, "changed", item_data->chk_change_cb, item_data);
	} else {
		evas_object_smart_callback_add(check, "changed", __chk_changed, item_data);
	}
	evas_object_show(check);

	/*Evas_Object *content = elm_layout_add(parent); */
	/*"list/B/type.3" */
	elm_layout_theme_set(content, "layout", "list/B/type.4", "default");
	elm_layout_content_set(content, "elm.swallow.content", check);
	return content;
}

static Evas_Object *__add_check_icon(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	__add_check(item_data, parent);
	Evas_Object *check = item_data->eo_check;

	Evas_Object *thumb = NULL;
	if (item_data->r_swallow_path) {
		thumb = elm_icon_add(parent);
		elm_image_file_set(thumb,
		                   item_data->r_swallow_path,
		                   NULL);
		evas_object_size_hint_aspect_set(thumb,
		                                 EVAS_ASPECT_CONTROL_VERTICAL,
		                                 1, 1);
	}

	Evas_Object *layout = elm_layout_add(parent);
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME,
	                    "thumbnail_icon2");
	evas_object_size_hint_weight_set(layout,
	                                 EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	elm_object_part_content_set(layout, "checkbox", check);
	elm_object_part_content_set(layout, "thumbnail", thumb);
	evas_object_show(layout);
	return layout;
}

static Evas_Object *__add_toggle(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *ly_toggle = elm_layout_add(parent);
	elm_layout_theme_set(ly_toggle, "layout", "list/C/type.3", "default");
	__add_check(item_data, parent);
	Evas_Object *check = item_data->eo_check;
	elm_object_style_set(check, "on&off");

	/*SETTING_TRACE_DEBUG("toggleDisableFlag %u", item_data->toggleDisableFlag); */
	if (item_data->toggleDisableFlag)
		elm_object_disabled_set(check, EINA_TRUE);

	if (SWALLOW_Type_1TOGGLE == item_data->swallow_type
	    || SWALLOW_Type_1ICON_1RADIO == item_data->swallow_type) {
		/*SETTING_TRACE("Re-registered"); */
		char state[MAX_SPECIALIZITION_LEN] = {0,};
		snprintf(state, sizeof(state),
		         _("IDS_ST_BODY_P1SS_IS_CURRENTLY_P2SS_T_TTS"),
		         _(item_data->keyStr), item_data->chk_status ? _("IDS_ST_BODY_ON_M_STATUS") : _("IDS_ST_BODY_ALERTTYPE_OFF"));

		Evas_Object *eo = elm_object_item_access_object_get(item_data->item);
		elm_access_info_set(eo, ELM_ACCESS_STATE, isEmptyStr(state) ? NULL : state);
	}
	setting_genlist_unregister_tts_access_highlight(item_data->eo_check, item_data->itc);

	elm_layout_content_set(ly_toggle, "elm.swallow.content", check);
	/*return check; */
	return ly_toggle;
}

static Evas_Object *__add_dot_toggle(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	/*SETTING_TRACE_BEGIN; */
	retv_if(!item_data || !parent || -1 == item_data->chk_status, NULL);
	return NULL;
#if 0
	Evas_Object *eo_img = NULL;
	eo_img = elm_image_add(parent);

	if (eo_img) {
		evas_object_propagate_events_set(eo_img, EINA_FALSE);
		if (0 == item_data->chk_status) {
			elm_image_file_set(eo_img, SETTING_ICON_PATH_CFG"A01-1_top_off.png", NULL);
		} else if (1 == item_data->chk_status) {
			elm_image_file_set(eo_img, SETTING_ICON_PATH_CFG"A01-1_top_on.png", NULL);
		}
		evas_object_size_hint_min_set(eo_img, ELM_SCALE_SIZE(40), ELM_SCALE_SIZE(40));
		evas_object_size_hint_max_set(eo_img, 40, 40);
		evas_object_show(eo_img);
	}
	return eo_img;
#endif
}


static Evas_Object *__add_multiline_toggle(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *chk = __add_toggle(item_data, parent);
	elm_object_style_set(chk, "on&off");
	return chk;
}


static Evas_Object *__add_radio(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	if (!item_data->rgd) {	/* exceptional handling */
		item_data->rgd = elm_radio_add(parent);
		elm_radio_state_value_set(item_data->rgd, -1);
	}
	Evas_Object *ly_radio = elm_layout_add(parent);
	elm_layout_theme_set(ly_radio, "layout", "list/B/type.4", "default");
	Evas_Object *radio = elm_radio_add(ly_radio);
	evas_object_propagate_events_set(radio, EINA_FALSE);
	evas_object_repeat_events_set(radio, EINA_TRUE);
	elm_radio_state_value_set(radio,
	                          item_data->chk_status);
	elm_radio_group_add(radio, item_data->rgd);
	evas_object_show(radio);
	item_data->eo_check = radio;
	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               item_data->chk_change_cb,
		                               item_data);
	} else {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               __radio_changed,
		                               item_data);
	}
	setting_genlist_unregister_tts_access_highlight(item_data->eo_check, item_data->itc);
	elm_layout_content_set(ly_radio, "elm.swallow.content", radio);
	/*return check; */
	return ly_radio;
}

static Evas_Object *__add_radio_right(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	if (!item_data->rgd) {
		item_data->rgd = elm_radio_add(parent);
		elm_radio_state_value_set(item_data->rgd, -1);
	}
	Evas_Object *ly_radio = elm_layout_add(parent);
	elm_layout_theme_set(ly_radio, "layout", "list/C/type.2", "default");
	Evas_Object *radio = elm_radio_add(ly_radio);

	elm_object_style_set(radio, "list");

	elm_radio_state_value_set(radio,
	                          item_data->chk_status);
	elm_radio_group_add(radio, item_data->rgd);
	evas_object_show(radio);
	item_data->eo_check = radio;
	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               item_data->chk_change_cb,
		                               item_data);
	} else {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               __radio_changed,
		                               item_data);
	}
	setting_genlist_unregister_tts_access_highlight(item_data->eo_check, item_data->itc);
	elm_layout_content_set(ly_radio, "elm.swallow.content", radio);

	return ly_radio;
}

static Evas_Object *__add_radio_text(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	if (!item_data->rgd) {	/* exceptional handling */
		item_data->rgd = elm_radio_add(parent);
		elm_radio_state_value_set(item_data->rgd, -1);
	}
	Evas_Object *radio = elm_radio_add(parent);
	evas_object_propagate_events_set(radio, EINA_FALSE);
	evas_object_repeat_events_set(radio, EINA_FALSE);
	elm_radio_state_value_set(radio,
	                          item_data->chk_status);
	elm_radio_group_add(radio, item_data->rgd);
	evas_object_show(radio);
	item_data->eo_check = radio;
	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               item_data->chk_change_cb,
		                               item_data);
	} else {
		evas_object_smart_callback_add(radio,
		                               "changed",
		                               __radio_changed,
		                               item_data);
	}
	return radio;
}


static Evas_Object *__add_button(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	elm_object_text_set(btn, "Edit");
	evas_object_show(btn);
	if (item_data->r_swallow_path) {
		elm_object_style_set(btn, item_data->r_swallow_path);
	}
	if (item_data->keyStr && item_data->swallow_type != SWALLOW_Type_1RADIO_1BTN) {
		elm_object_text_set(btn, _(item_data->keyStr));
	}
	evas_object_propagate_events_set(btn, EINA_FALSE);

	if (item_data->stop_change_cb) {	/* just only for 1radio_1btn type */
		/* arrow button's tts info */
		if (safeStrCmp(item_data->keyStr, "IDS_ST_BODY_SIMPLE_PASSWORD") == 0)
			setting_set_tts_info(btn, "Change simple password", NULL, NULL, NULL);
		else if (safeStrCmp(item_data->keyStr, "IDS_ST_BODY_PASSWORD") == 0)
			setting_set_tts_info(btn, "Change password", NULL, NULL, NULL);

		evas_object_smart_callback_add(btn,
		                               "clicked",
		                               item_data->stop_change_cb,
		                               item_data);
	} else if (item_data->chk_change_cb) {	/* for other types */
		evas_object_smart_callback_add(btn,
		                               "clicked",
		                               item_data->chk_change_cb,
		                               item_data);
	}
	SETTING_TRACE_DEBUG("toggleDisableFlag %u", item_data->toggleDisableFlag);
	if (item_data->toggleDisableFlag)
		elm_object_disabled_set(btn, EINA_TRUE);
	return btn;
}


static Evas_Object *__add_gif(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *image = NULL;
	const char **png_list = (const char **)item_data->belongs_to;

	/*SETTING_TRACE("item_data->belongs_to:%p", png_list); */
	if (png_list && *png_list) {
		if (png_list[1]) {	/*png_list is an array end with 'NULL', and it has at least 2 png file, */
			image = setting_create_gif(parent, png_list);
		} else {	/*png_list just has one png file, */
			image = setting_create_image(parent, *png_list);
		}
	}

	return image;
}

static Evas_Object *__add_image(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent || !item_data->l_swallow_path, NULL);
	Evas_Object *image = NULL;
	image = setting_create_image(parent, item_data->l_swallow_path);
	return image;
}

static void __entry_without_layout_focused_cb(void *data, Evas_Object *obj, void *ei) /* Focused callback will show X marked button and hide guidetext. */
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *id = data;
	if (!elm_entry_is_empty(obj)) {
		elm_object_item_signal_emit(id->item, "elm,state,eraser,show", "");
	}
	elm_object_item_signal_emit(id->item, "elm,state,rename,hide", "");

	/* accessibiliy */
	Evas_Object *acc = elm_object_item_access_object_get(id->item);
	if (!acc) return;

	const char *txt = elm_entry_entry_get(obj);
	if (!txt || 0 == strlen(txt))
		txt = elm_object_part_text_get(obj, "elm.guide");

	/* Say entry focused */
	Eina_Stringshare *str = eina_stringshare_printf("%s, Edit Field, Keyboard opened", txt);
	elm_access_say((char *)str);
	eina_stringshare_del(str);
	SETTING_TRACE_END;
}

static void __entry_without_layout_unfocused_cb(void *data, Evas_Object *obj, void *event_info) /* Unfocused callback will show guidetext and hide X marked button. */
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "invalid parameter: data is NULL");
	Setting_GenGroupItem_Data *id = data;
	setting_retm_if(id->item == NULL, "item is NULL");
	elm_object_item_signal_emit(id->item, "elm,state,eraser,hide", "");
	elm_object_item_signal_emit(id->item, "elm,state,rename,show", "");
	SETTING_TRACE_END;
}

static void __rejected_char_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/*SETTING_TRACE_DEBUG("Input char is not an Alphanumeric&Symbol."); */
	setting_create_simple_popup(NULL, elm_object_parent_widget_get(obj), NULL, _("IDS_ST_BODY_PASSWORD_CONTAINS_INVALID_CHARACTER_ABB"));
}

static Evas_Object *__add_entry_without_layout(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	elm_genlist_mode_set(parent, ELM_LIST_COMPRESS);	/* resolve abnormal height issue */
	Evas_Object *entry = NULL;
	Ea_Editfield_Type type;

	if (item_data->isPasswordFlag) {
		type = EA_EDITFIELD_SCROLL_SINGLELINE_PASSWORD;
	} else if (item_data->isSinglelineFlag) {
		type = EA_EDITFIELD_SINGLELINE;
	} else {
		type = EA_EDITFIELD_MULTILINE;
	}
	item_data->eo_check = entry = ea_editfield_add(parent, type);
	evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
	if (item_data->focus_unallowed) {
		elm_object_focus_allow_set(entry, EINA_FALSE);
	}
	elm_entry_input_panel_layout_set(entry,
	                                 item_data->input_type);
	/*elm_entry_context_menu_disabled_set(entry, TRUE); */
	/*elm_entry_magnifier_disabled_set(entry, TRUE); */
	elm_entry_prediction_allow_set(entry, FALSE);
	elm_entry_input_panel_return_key_type_set(entry, item_data->return_key_type);
	if (item_data->disable_auto_cap) {
		elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_NONE);
	}

	if (item_data->isSinglelineFlag) {
		elm_entry_scrollable_set(entry, EINA_TRUE);
		elm_entry_single_line_set(entry, EINA_TRUE);
		elm_object_signal_emit(entry, "elm,state,scroll,enabled", "");
	} else {
		/*for multi-line editfield or entry, need to disable clear button,refer to P130802-3553*/
		ea_editfield_clear_button_disabled_set(entry, EINA_TRUE);
	}

	if (item_data->isPasswordFlag) {
		/*elm_entry_single_line_set(entry, EINA_TRUE); */
		elm_entry_password_set(entry, EINA_TRUE);
		elm_entry_input_panel_return_key_disabled_set(entry, EINA_TRUE);

		static Elm_Entry_Filter_Accept_Set accept_filter_data;
		accept_filter_data.accepted = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!""#$%&'()*+,-./:;<=>?@[]\\^_`{|}~";
		accept_filter_data.rejected = NULL;
		elm_entry_markup_filter_append(entry, elm_entry_filter_accept_set, &accept_filter_data);

		evas_object_smart_callback_add(entry, "rejected", __rejected_char_cb, item_data);
	}

	if (item_data->limit_filter_data) {
		elm_entry_markup_filter_append
		(entry,
		 elm_entry_filter_limit_size,
		 item_data->limit_filter_data);

		if (item_data->maxlength_reached_cb) {
			evas_object_smart_callback_add(entry,
			                               "maxlength,reached",
			                               item_data->maxlength_reached_cb,
			                               item_data);
		} else {
			evas_object_smart_callback_add(entry,
			                               "maxlength,reached",
			                               __max_len_reached,
			                               item_data);
		}
	}

	/*SETTING_TRACE("item_data->stop_change_cb:%p", item_data->stop_change_cb); */
	if (item_data->stop_change_cb) {/*invoked when stop focusing on */
		evas_object_smart_callback_add(entry,
		                               "unfocused",
		                               item_data->stop_change_cb,
		                               item_data);
	} else {
		evas_object_smart_callback_add(entry,
		                               "unfocused",
		                               __entry_without_layout_unfocused_cb,
		                               item_data);
	}
	if (item_data->digits_filter_data) {
		elm_entry_markup_filter_append(entry,
		                               elm_entry_filter_accept_set,
		                               item_data->digits_filter_data);
	}


	if (item_data->sub_desc && safeStrLen(item_data->sub_desc) > 0) {
		elm_entry_entry_set(entry, item_data->sub_desc);
	} else {
		if (item_data->guide_text) {
			elm_object_part_text_set(entry, "elm.guide", _(item_data->guide_text)); /* Add guide text to elm_entry. */
		} else {
			elm_object_part_text_set(entry, "elm.guide", _("IDS_ST_BODY_TAP_TO_INSERT"));
		}
	}


	/* for Setting App, All the entrys's context popup shouldn't be able */
	/* to insert images, text is the only one can be inserted. */
	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);

	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(entry,
		                               "changed",
		                               item_data->chk_change_cb,
		                               item_data);
		evas_object_smart_callback_add(entry,
		                               "preedit,changed",
		                               item_data->chk_change_cb,
		                               item_data);
	} else {
		/*default handle */
		evas_object_smart_callback_add(entry, "changed", __entry_changed, item_data);
	}

	/* callback for handling enter key */
	if (item_data->activated_cb) {
		evas_object_smart_callback_add(entry,
		                               "activated",
		                               item_data->activated_cb,
		                               item_data);
	}

	/*SETTING_TRACE("item_data->focus_cb:%p", item_data->focus_cb); */

	if (item_data->focus_cb) {
		evas_object_smart_callback_add(entry, "focused", item_data->focus_cb, item_data);
	} else {
		evas_object_smart_callback_add(entry, "focused", __entry_without_layout_focused_cb, item_data);
	}

	/*elm_object_signal_callback_add(ed_name, "elm,eraser,clicked", "elm", __eraser_clicked, entry); */

	if (item_data->start_change_cb) {
		evas_object_event_callback_add(entry, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)(item_data->start_change_cb), item_data->userdata);
	} else {
		evas_object_event_callback_add(entry, EVAS_CALLBACK_KEY_DOWN, __entry_keydown, item_data);
	}

	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(entry);
	if (imf_context && item_data->x_callback_cb) {
		ecore_imf_context_input_panel_event_callback_add(imf_context,
		                                                 ECORE_IMF_INPUT_PANEL_STATE_EVENT,
		                                                 item_data->x_callback_cb,
		                                                 item_data->userdata);
	}
	if (item_data->input_panel_disable_flag) {
		elm_entry_input_panel_enabled_set(entry, EINA_FALSE);
	}
	if (entry && item_data->entry_auto_focus) {
		evas_object_show(entry);
		elm_object_focus_set(entry, EINA_TRUE);
	}

	return entry;
}

static Evas_Object *__add_entry(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	elm_genlist_mode_set(parent, ELM_LIST_COMPRESS);	/* resolve abnormal height issue */
	Evas_Object *ed_name =
	    setting_create_editfiled(parent, _(item_data->keyStr),
	                             item_data->sub_desc, item_data->guide_text);
	evas_object_pass_events_set(ed_name, 1);
	evas_object_propagate_events_set(ed_name, 0);
	Evas_Object *entry = elm_object_part_content_get(ed_name, "elm.swallow.content");
	item_data->eo_check = entry;
	elm_entry_input_panel_layout_set(entry,
	                                 item_data->input_type);

	if (item_data->isSinglelineFlag) {
		elm_entry_scrollable_set(entry, EINA_TRUE);
		elm_entry_single_line_set(entry, EINA_TRUE);
	}

	if (item_data->isPasswordFlag) {
		elm_entry_single_line_set(entry, EINA_TRUE);
		elm_entry_password_set(entry, EINA_TRUE);
	}

	if (item_data->limit_filter_data) {
		elm_entry_markup_filter_append
		(entry,
		 elm_entry_filter_limit_size,
		 item_data->limit_filter_data);

		if (item_data->maxlength_reached_cb) {
			evas_object_smart_callback_add(entry,
			                               "maxlength,reached",
			                               item_data->maxlength_reached_cb,
			                               item_data);
		} else {
			evas_object_smart_callback_add(entry,
			                               "maxlength,reached",
			                               __max_len_reached,
			                               item_data);
		}
	}
	if (item_data->stop_change_cb) {/*invoked when stop focusing on */
		evas_object_smart_callback_add(entry,
		                               "unfocused",
		                               item_data->stop_change_cb,
		                               item_data);
	} else {
		evas_object_smart_callback_add(entry,
		                               "unfocused",
		                               __entry_unfocused,
		                               ed_name);
	}
	if (item_data->digits_filter_data) {
		elm_entry_markup_filter_append(entry,
		                               elm_entry_filter_accept_set,
		                               item_data->digits_filter_data);
	}

	/* for Setting App, All the entrys's context popup shouldn't be able */
	/* to insert images, text is the only one can be inserted. */
	elm_entry_cnp_mode_set(entry, ELM_CNP_MODE_PLAINTEXT);

	if (item_data->chk_change_cb) {
		evas_object_smart_callback_add(entry,
		                               "changed",
		                               item_data->chk_change_cb,
		                               item_data);
	} else {
		/*default handle */
		evas_object_smart_callback_add(entry, "changed", __entry_changed, item_data);
	}

	/* callback for handling enter key */
	if (item_data->activated_cb) {
		evas_object_smart_callback_add(entry,
		                               "activated",
		                               item_data->activated_cb,
		                               item_data);
	}

	if (item_data->focus_cb) {
		evas_object_smart_callback_add(entry, "focused", item_data->focus_cb, item_data);
	} else {
		evas_object_smart_callback_add(entry, "focused", __entry_focused, ed_name);
	}

	elm_object_signal_callback_add(ed_name, "elm,eraser,clicked", "elm", __eraser_clicked, entry);

	if (item_data->start_change_cb) {
		evas_object_event_callback_add(entry, EVAS_CALLBACK_KEY_DOWN, (Evas_Object_Event_Cb)(item_data->start_change_cb), item_data->userdata);
	} else {
		evas_object_event_callback_add(entry, EVAS_CALLBACK_KEY_DOWN, __entry_keydown, item_data);
	}

	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(entry);
	if (imf_context) {
		ecore_imf_context_input_panel_event_callback_add(imf_context,
		                                                 ECORE_IMF_INPUT_PANEL_STATE_EVENT,
		                                                 item_data->x_callback_cb,
		                                                 item_data->userdata);
	}
	if (item_data->input_panel_disable_flag) {
		elm_entry_input_panel_enabled_set(entry, EINA_FALSE);
	}
	return ed_name;
}

static Evas_Object *__add_5step_slider(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	SETTING_TRACE_BEGIN;
	retv_if(!item_data || !parent, NULL);
	Evas_Object *li_slider =
	    setting_create_5step_slider(parent, item_data->evas,
	                                item_data->l_swallow_path,
	                                item_data->r_swallow_path,
	                                item_data->chk_status,
	                                item_data->isIndicatorVisible,
	                                item_data->slider_min,
	                                item_data->slider_max,
	                                item_data->chk_change_cb,
	                                item_data->start_change_cb,
	                                item_data->stop_change_cb,
	                                item_data);
	evas_object_pass_events_set(li_slider, 1);
	evas_object_propagate_events_set(li_slider, 0);
	item_data->eo_check = li_slider;
	return li_slider;
}

static Evas_Object *__add_slider(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *li_slider =
	    setting_create_slider(parent, item_data->evas,
	                          item_data->l_swallow_path,
	                          item_data->r_swallow_path,
	                          item_data->chk_status,
	                          item_data->isIndicatorVisible,
	                          item_data->slider_min,
	                          item_data->slider_max,
	                          item_data->chk_change_cb,
	                          item_data->start_change_cb,
	                          item_data->stop_change_cb,
	                          item_data);
	evas_object_pass_events_set(li_slider, 1);
	evas_object_propagate_events_set(li_slider, 0);
	item_data->eo_check = li_slider;
	return li_slider;
}

static Evas_Object *__add_layout(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *layout = elm_layout_add(parent);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");
	elm_layout_theme_set(layout, "layout", "application", "default");
	int w = item_data->chk_status;
	int h = item_data->chk_status;
	elm_object_part_content_set(layout, "elm.swallow.bg", setting_create_blank_rect_customize(parent, w, h));
	/*elm_object_part_content_set(layout, "elm.swallow.content", navi); */
#if 0
	char **load_arglist = (char **)item_data->belongs_to;
	char *load_name = item_data->keyStr;
	char *load_style = item_data->item_style;
	if (0 == safeStrCmp(load_style, "APP"))
		app_launcher(load_name);/*how to adjust the size */
	else if (0 == safeStrCmp(load_style, "UG")) {
		struct ug_cbs cbs = {0,};

		cbs.layout_cb = __ug_layout_cb;
		cbs.result_cb = __ug_result_cb;
		cbs.destroy_cb = __ug_destroy_cb;
		cbs.priv = ad;

		ad->camera_ug = setting_ug_create(NULL, load_name, UG_MODE_FULLVIEW, ad->app_control_handle, &cbs);
	}

#endif
	return layout;
}

static void __picker_popup_cancel_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = data;
	evas_object_del(popup);
}

static void __picker_popup_set_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	/*printf("Set clicked\n"); */
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("DEFAULT @@@");
	elm_object_signal_emit(data, "picker,action,value,set", "");
	/*Evas_Object *popup = evas_object_data_get((Evas_Object *)data, "popup"); */
	/*evas_object_del(popup); */
}

static Evas_Object *_datefield_picker_popup_add(Evas_Object *parent, Evas_Object *datetime, void *data)
{
	Datefield_Paramdata  *param = (Datefield_Paramdata *)data;

	Setting_GenGroupItem_Data *item_data = param->item_data;

	Evas_Object *popup = elm_popup_add(parent);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	Evas_Object *cancel_btn = elm_button_add(popup);
	elm_object_style_set(cancel_btn, "popup");
	elm_object_text_set(cancel_btn, "Cancel");
	elm_object_part_content_set(popup, "button1", cancel_btn);
	evas_object_smart_callback_add(cancel_btn, "clicked", __picker_popup_cancel_btn_clicked_cb, popup);

	Evas_Object *set_btn = elm_button_add(popup);
	elm_object_style_set(set_btn, "popup");
	elm_object_text_set(set_btn, "Set");
	elm_object_part_content_set(popup, "button2", set_btn);

	if (item_data->activated_cb)
		/*evas_object_smart_callback_add(set_btn, "clicked", item_data->activated_cb, datetime); */
		evas_object_smart_callback_add(set_btn, "clicked", item_data->activated_cb, item_data);
	else
		evas_object_smart_callback_add(set_btn, "clicked", __picker_popup_set_btn_clicked_cb, datetime);

	return popup;
}

static void _datefield_date_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/*Evas_Object *parent = data; */
	Datefield_Paramdata  *param = (Datefield_Paramdata *)data;
	Evas_Object *popup = _datefield_picker_popup_add(param->parent, obj/*elm_button*/, param/* context */);

	elm_object_part_text_set(popup, "title,text", "Set date");

	Evas_Object *datetime = elm_datetime_add(param->parent);

	evas_object_data_set(datetime, "popup", popup);

	/*--------------------------------------- */
	param->item_data->eo_check = datetime;
	/*--------------------------------------- */

	elm_object_content_set(popup, datetime);
	evas_object_show(popup);

	/*--------------------------------------- */
	/*param->item_data->eo_check = param->box; */
	/*--------------------------------------- */
}

static void _datefield_time_btn_clicked_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Datefield_Paramdata  *param = (Datefield_Paramdata *)data;

	Evas_Object *datetime = elm_datetime_add(param->parent);

	elm_object_style_set(datetime, "date_layout");

	char *pa_time_format = get_pa_time_format_str();
	if (pa_time_format) {
		if (!strncmp(pa_time_format, "IDS_ST_BODY_12_HOURS", strlen("IDS_ST_BODY_12_HOURS")))
			elm_datetime_format_set(datetime, "%l:%M %P");
		else
			elm_datetime_format_set(datetime, "%H:%M");
	}

	evas_object_size_hint_align_set(datetime, 0.5 , 0.5);
	evas_object_show(datetime);

	Evas_Object *popup = _datefield_picker_popup_add(param->parent, obj, (void *)data);
	elm_object_part_text_set(popup, "title,text", "Set time");

	evas_object_data_set(datetime, "popup", popup);

	/*--------------------------------------- */
	param->item_data->eo_check = datetime;
	/*--------------------------------------- */

	elm_object_content_set(popup, datetime);
	evas_object_show(popup);

	FREE(pa_time_format);
	/*--------------------------------------- */
	/*param->item_data->eo_check = param->box; */
	/*--------------------------------------- */
}

EXPORT_PUBLIC char *__setting_phone_lang_get_by_pattern(const char *locale, const char *skeleton)
{
	/*setting_retvm_if(!locale, NULL, "locale parameter is NULL"); */
	/*setting_retvm_if(data == NULL, NULL, "Data parameter is NULL"); */

	/*remove ".UTF-8" in locale */
	char locale_tmp[DEF_BUF_SIZE] = {0,};
	strncpy(locale_tmp, locale, DEF_BUF_SIZE);
	locale_tmp[DEF_BUF_SIZE - 1] = '\0';

	char *p = safeStrStr(locale_tmp, ".UTF-8");
	if (p) {
		*p = 0;
	}

	char *ret_str = NULL;
	UChar customSkeleton[SETTING_STR_SLP_LEN] = { 0, };
	UErrorCode status = U_ZERO_ERROR;
	UDateFormat *formatter = NULL;

	UChar bestPattern[SETTING_STR_SLP_LEN] = { 0, };
	UChar formatted[SETTING_STR_SLP_LEN] = { 0, };

	char bestPatternString[SETTING_STR_SLP_LEN] = { 0, };
	char formattedString[SETTING_STR_SLP_LEN] = { 0, };

	u_uastrncpy(customSkeleton, skeleton, safeStrLen(skeleton));

	UDateTimePatternGenerator *pattern_generator;
	pattern_generator = udatpg_open(locale_tmp, &status);

	int32_t bestPatternCapacity = (int32_t)(sizeof(bestPattern) / sizeof((bestPattern)[0]));
	(void)udatpg_getBestPattern(pattern_generator, customSkeleton, u_strlen(customSkeleton), bestPattern, bestPatternCapacity, &status);

	u_austrcpy(bestPatternString, bestPattern);
	SETTING_TRACE("BestPattern is %s", bestPatternString);

	UDate date = ucal_getNow();
	formatter = udat_open(UDAT_IGNORE, UDAT_IGNORE, locale_tmp, NULL, -1, bestPattern, -1, &status);
	int32_t formattedCapacity = (int32_t)(sizeof(formatted) / sizeof((formatted)[0]));
	(void)udat_format(formatter, date, formatted, formattedCapacity, NULL, &status);
	u_austrcpy(formattedString, formatted);
	SETTING_TRACE("DATE & TIME is %s", formattedString);

	udat_close(formatter);

	ret_str = g_strdup(formattedString);
	return ret_str;
}


/**
 * eo_check ??
 */
static Evas_Object *__add_datefield_new(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	SETTING_TRACE_BEGIN;
	retv_if(!item_data || !parent, NULL);

	Datefield_Paramdata  *param = malloc(sizeof(Datefield_Paramdata));
	setting_retvm_if(NULL == param, NULL, "param is NULL");

	param->item_data = item_data;
	/*param->parent = parent; */
	param->parent = item_data->nf;

	char *date = __setting_phone_lang_get_by_pattern("en_US", "MMM/d/yyyy");
	char *pa_time_format = get_pa_time_format_str();
	char *time;

	if (pa_time_format) {
		if (!strncmp(pa_time_format, "IDS_ST_BODY_12_HOURS", strlen("IDS_ST_BODY_12_HOURS")))
			time = __setting_phone_lang_get_by_pattern("en_US", "hhmm");
		else
			time = __setting_phone_lang_get_by_pattern("en_US", "Hm");
		SETTING_TRACE("DATE : %s ", date);
		SETTING_TRACE("TIME : %s ", time);
	}

	Evas_Object *btn_left = elm_button_add(parent);
	elm_object_style_set(btn_left, "datetime");
	evas_object_size_hint_weight_set(btn_left, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(btn_left, ELM_SCALE_SIZE(90), -1);
	evas_object_resize(btn_left, 300, -1);

	elm_object_text_set(btn_left, date);
	evas_object_smart_callback_add(btn_left, "clicked", _datefield_date_btn_clicked_cb , param);
	evas_object_show(btn_left);

	Evas_Object *btn_right = elm_button_add(parent);
	elm_object_style_set(btn_right, "datetime");
	evas_object_size_hint_weight_set(btn_right, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_min_set(btn_right, ELM_SCALE_SIZE(90), -1);
	evas_object_resize(btn_right, 270, -1);
	elm_object_text_set(btn_right, time);
	evas_object_smart_callback_add(btn_right, "clicked", _datefield_time_btn_clicked_cb , param);
	evas_object_show(btn_right);

	param->item_data->btn_left = btn_left;
	param->item_data->btn_right = btn_right;

	Evas_Object *box = elm_box_add(parent);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object *box1 = elm_box_add(box);
	evas_object_size_hint_weight_set(box1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_box_horizontal_set(box1, 1);
	elm_box_align_set(box1, 0.5, 0.0);
	elm_box_padding_set(box1, ELM_SCALE_SIZE(15), ELM_SCALE_SIZE(5));

	elm_box_pack_end(box1, btn_left);
	elm_box_pack_end(box1, btn_right);
	evas_object_show(box1);

	elm_box_pack_end(box, box1);
	evas_object_show(box);

	FREE(pa_time_format);
	return box;
}

static Evas_Object *__add_radio_search(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *box = elm_box_add(parent);
	elm_box_horizontal_set(box, 1);

	/*a.radio.. */
	/*Evas_Object *radio = __add_radio(item_data, parent);
	setting_disable_evas_object(radio);
	elm_box_pack_end(box, radio);

	//b.separator..
	Evas_Object *separator =
		setting_create_blank_rect_customize(box,
				SETTING_HALF_SCREEN_WIDTH,
				SETTING_PADDING_HEIGHT);
	elm_box_pack_end(box, separator);*/

	/*c.progressbar.. */
	Evas_Object *progressbar = elm_progressbar_add(parent);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	evas_object_size_hint_align_set(progressbar,
	                                EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar,
	                                 EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	elm_box_pack_end(box, progressbar);

	evas_object_show(box);
	return box;
}

static Evas_Object *__add_search(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	/* added by JTS: CQ H0100135171 */
	Evas_Object *box = elm_box_add(parent);
	elm_box_horizontal_set(box, 1);

	/*b.separator.. */
	Evas_Object *separator =
	    setting_create_blank_rect_customize(box,
	                                        SETTING_HALF_SCREEN_WIDTH, SETTING_PADDING_HEIGHT);
	elm_box_pack_end(box, separator);

	/*c.progressbar.. */
	Evas_Object *progressbar = elm_progressbar_add(parent);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar,
	                                 EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	elm_box_pack_end(box, progressbar);

	evas_object_show(box);
	return box;

}

static Evas_Object *__add_process(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);
	Evas_Object *progressbar = elm_progressbar_add(parent);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	evas_object_size_hint_align_set(progressbar,
	                                EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar,
	                                 EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	return progressbar;
}

static Evas_Object *__add_radio_lable(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	retv_if(!item_data || !parent, NULL);

	/*a.radio.. */
	Evas_Object *radio = __add_radio(item_data, parent);

	return radio;
}

static Evas_Object *__add_left_default(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	if (!item_data || !parent || !item_data->l_swallow_path) {
		return NULL;
	}

	if (SWALLOW_Type_1ICON_WITHOUT_CIRCLE_BG == item_data->swallow_type
	    || SWALLOW_Type_1ICON_1DOTTOOGLE == item_data->swallow_type
	    || SWALLOW_Type_1ICON_1IMAGE == item_data->swallow_type) {
		Evas_Object *icon = elm_icon_add(parent);
		elm_image_file_set(icon, item_data->l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		return icon;
	}

	Evas_Object *ic = NULL;
	ic = elm_layout_add(parent);
	Evas_Object *c = NULL;

	elm_layout_theme_set(ic, "layout", "list/B/type.4", "default");
	c = elm_image_add(parent);
	elm_image_file_set(c, item_data->l_swallow_path, NULL);
	setting_decorate_image_RGBA(c, 14, 41, 73, 255);
	evas_object_size_hint_align_set(c, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(c, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_content_set(ic, "elm.swallow.content", c);

	return ic;
}

static Evas_Object *__add_right_default(Setting_GenGroupItem_Data *item_data, Evas_Object *parent)
{
	if (!item_data || !parent || !item_data->r_swallow_path) {
		return NULL;
	}

#if 0
	Evas_Object *icon = elm_icon_add(parent);
	elm_image_file_set(icon, item_data->r_swallow_path, NULL);
	evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
	if (!safeStrCmp(item_data->r_swallow_path, SETTING_IMAGE_PATH_CFG"00_list_button_expand_closed.png"))
		else if (!safeStrCmp(item_data->r_swallow_path, SETTING_IMAGE_PATH_CFG"00_list_button_expand_opened.png"))
			return icon;
#else
	Evas_Object *ic = NULL;
	ic = elm_layout_add(parent);
	Evas_Object *icon = NULL;

	elm_layout_theme_set(ic, "layout", "list/C/type.3", "default");
	icon = elm_image_add(parent);
	if (!safeStrCmp(item_data->r_swallow_path, SETTING_IMAGE_PATH_CFG"00_list_button_expand_closed.png"))
		setting_decorate_image_RGBA(icon, 14, 41, 73, 255);
	else if (!safeStrCmp(item_data->r_swallow_path, SETTING_IMAGE_PATH_CFG"00_list_button_expand_opened.png"))
		setting_decorate_image_RGBA(icon, 14, 41, 73, 255);
	elm_image_file_set(icon, item_data->r_swallow_path, NULL);
	evas_object_size_hint_align_set(icon, 0.0, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_layout_content_set(ic, "elm.swallow.content", icon);
	return ic;

#endif
}

/* draw handler */
typedef Evas_Object *(*__drawer_fp)(Setting_GenGroupItem_Data *item_data, Evas_Object *parent);
typedef struct {
	SWALLOW_Type type;			/* ui type */
	__drawer_fp draw;
} __Content_Drawer;
static __Content_Drawer __cd_left[SWALLOW_Type_MAX] = {
	/*base objects */
	{SWALLOW_Type_1CHECK, __add_check},
	{SWALLOW_Type_1ICON_1CHECK, __add_check},
	{SWALLOW_Type_1TOGGLE, __add_toggle},
	{SWALLOW_Type_1RADIO, __add_radio},
	{SWALLOW_Type_1BUTTON, __add_button},
	{SWALLOW_Type_1GIF, __add_gif},
	{SWALLOW_Type_1IMAGE, __add_image},
	{SWALLOW_Type_1TEXT_1IMAGE, __add_image},

	/*base objects in an layout */
	{SWALLOW_Type_LAYOUT_ENTRY, __add_entry},
	{SWALLOW_Type_LAYOUT_SLIDER, __add_slider},
	{SWALLOW_Type_LAYOUT_5STEP_SLIDER, __add_5step_slider},
	/*{SWALLOW_Type_LAYOUT_DATEFIELD, __add_datefield2}, */
	{SWALLOW_Type_LOAD_LAYOUT, __add_layout},

	/*other compelx objects */
	{SWALLOW_Type_1SEARCH, __add_search},
	{SWALLOW_Type_1PROCESS, __add_process},
	{SWALLOW_Type_MULTILINE_1TOGGLE, __add_multiline_toggle},
	{SWALLOW_Type_1CHECKICON, __add_check_icon},
	{SWALLOW_Type_1RADIO_1SEARCH, __add_radio_search},
	{SWALLOW_Type_1RADIO_1LABLE, __add_radio_lable},
	{SWALLOW_Type_1RADIO_1BTN, __add_radio},
	{SWALLOW_Type_1RADIO_1TEXT, __add_radio_text},

	/*left part object of the type */
	{SWALLOW_Type_1ICON_1RADIO, __add_left_default},
	{SWALLOW_Type_1ICON_1PROCESS, __add_left_default},
	{SWALLOW_Type_1RADIO_1PROCESS, __add_radio},
};
static __Content_Drawer __cd_right[SWALLOW_Type_MAX] = {
	/*right part object of the type */
	{SWALLOW_Type_1CHECK_RIGHT, __add_check},
	{SWALLOW_Type_1ICON_1RADIO, __add_toggle},
	{SWALLOW_Type_1ICON_1CHECK, __add_right_default},
	{SWALLOW_Type_1ICON_1DOTTOOGLE, __add_dot_toggle},
	{SWALLOW_Type_1ICON_1PROCESS, __add_process},
	{SWALLOW_Type_1RADIO_1PROCESS, __add_process},
	{SWALLOW_Type_1RADIO_1BTN, __add_button},
	{SWALLOW_Type_1RADIO_RIGHT, __add_radio_right},
};

static __Content_Drawer __cd_end[SWALLOW_Type_MAX] = {
	/*end part object of the type */
	{SWALLOW_Type_1RADIO, __add_radio},
};

static void __multiline_eraser_clicked(void *data, Evas_Object *obj, void *event_info) /* When X marked button clicked, make string as empty. */
{
	Setting_GenGroupItem_Data *list_item = data;
	Evas_Object *entry = elm_object_item_part_content_get(list_item->item, "elm.icon.entry");
	elm_object_focus_set(entry, EINA_TRUE); /* After button is clicked, entry should get focus again. */
	elm_entry_entry_set(entry, "");
}

static Evas_Object *_gl_Gendial_content_get(void *data, Evas_Object *obj,
                                            const char *part)
{
	retv_if(!data, NULL);
	Setting_GenGroupItem_Data *item_data = data;
	retv_if(!data, NULL);
	__Content_Drawer *cd_list = NULL;
	__drawer_fp fp = NULL;
	/*SETTING_TRACE("content get [%s]",part); */


	if (!safeStrCmp(part, "elm.icon.1")) { /* LEFT AREA */
		if (item_data->swallow_type == SWALLOW_Type_LAYOUT_EDITFIELD
		    && item_data->isPasswordFlag == TRUE)
			return NULL;
		fp = __add_left_default; /*hold default drawer */
		cd_list = __cd_left;
	} else if (!safeStrCmp(part, "elm.icon.2")) { /* RIGHT AREA */
		if (item_data->swallow_type == SWALLOW_Type_LAYOUT_EDITFIELD
		    && item_data->isPasswordFlag == TRUE)
			return NULL;
		fp = __add_right_default; /*hold default drawer */
		cd_list = __cd_right;
	} else if (!safeStrCmp(part, "elm.icon")) { /* CENTER WHOLE */
		fp = __add_left_default; /*hold default drawer */
		cd_list = __cd_left;
	} else if (!safeStrCmp(part, "elm.swallow.end")) {
		/*the default value of fp is NULL here */
		cd_list = __cd_end;
	}
	/* To do : add code for editfield */
	else if (!safeStrCmp(part, "elm.icon.entry")) {
		if (item_data->swallow_type == SWALLOW_Type_LAYOUT_DATEFIELD) {
			/*fp = __add_datefield; */
			fp = __add_datefield_new;
		} else {
			fp = __add_entry_without_layout;
		}
	} else if (!safeStrCmp(part, "elm.icon.eraser")) {
		/*fp = __add_button_eraser; */
		Evas_Object *btn = elm_button_add(obj);
		elm_object_style_set(btn, "editfield_clear"); /*Make "X" marked button by changing style. */
		evas_object_smart_callback_add(btn, "clicked", __multiline_eraser_clicked, item_data);
		return btn;
	} else if (!safeStrCmp(part, "elm.icon.edit")) {
		Evas_Object *btn = elm_button_add(obj);
		elm_object_style_set(btn, "minus");
		evas_object_propagate_events_set(btn, EINA_FALSE);
		return btn;
	}
	/* End. */

	/*if (!cd_list) return NULL;//invalid part */

	if (cd_list) {
		int idx = 0;
		for (; idx < SWALLOW_Type_MAX; idx++) {
			if (item_data->swallow_type == cd_list[idx].type) { /*match using swallow type */
				fp = cd_list[idx].draw;
				break;
			}
		}
	}
	return fp ? fp(item_data, obj) : NULL;
}

static void _gl_Gendial_del(void *data, Evas_Object *obj)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) data;
	if (item_data) {
		item_data->eo_check = NULL;/*set to NULL at once */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->guide_text);
		G_FREE((item_data->l_swallow_path));
		G_FREE((item_data->r_swallow_path));
		G_FREE(item_data->item_style);


		if (item_data->digits_filter_data) {
			G_FREE(item_data->digits_filter_data->accepted);
			G_FREE(item_data->digits_filter_data->rejected);
			FREE(item_data->digits_filter_data);

		}

		if (item_data->notify) {
			evas_object_del(item_data->notify);
			item_data->notify = NULL;
		}

		FREE(item_data->limit_filter_data);
		__BACK_POINTER_UNSET(item_data);
		FREE(item_data);
		data = NULL;
	}
	/* SETTING_TRACE_END; */
}

/**
 * To intialize an Elm_Genlist_Item_Class, according to item style
 *
 * @param[in] item_style
 * @param[in/out] itc
 */
EXPORT_PUBLIC
bool setting_create_Gendial_itc(const char *item_style,
                                Elm_Genlist_Item_Class *itc)
{
	itc->item_style = item_style;
	itc->func.text_get = _gl_Gendial_text_get;
	itc->func.content_get = _gl_Gendial_content_get;
	itc->func.state_get = NULL;
	itc->func.del = _gl_Gendial_del;	/* _gl_Gendial_del; */
	return TRUE;
}

/**
 * Create group style item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_group_titleItem(Evas_Object *genlist,
                                                                        const Elm_Genlist_Item_Class *itc,
                                                                        Elm_Object_Item *parent,
                                                                        Elm_Genlist_Item_Type flag,	/* ELM_GENLIST_ITEM_GROUP or ELM_GENLIST_ITEM_NONE */
                                                                        const char *keyStr,
                                                                        setting_call_back_func gl_sel)
{
	/* SETTING_TRACE_BEGIN; */

	ADD_GL_SEPARATOR(genlist);
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, parent,
		                            flag, gl_sel, NULL);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, parent,
		                            flag, _gl_Gendial_sel, NULL);
	}
	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item_data;
}

/**
 * Create separator style item with title
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC Setting_GenGroupItem_Data *setting_create_Gendial_field_titleItem(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    const char *keyStr,
    setting_call_back_func gl_sel)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            NULL);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, NULL);
	}
	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item_data;
}

/**
 * Create 1radio-1text style item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func chk_change_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->swallow_type = swallow_type;
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->rgd = rgd;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	return item_data;
}

/**
 * Create 1radio-2text style item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio_2text(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    char *sub_desc,
    setting_call_back_func chk_change_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->swallow_type = swallow_type;
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->rgd = rgd;
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	return item_data;
}


EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio_1button(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *button_style,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func chk_change_cb,
    setting_call_back_func btn_clicked_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->swallow_type = swallow_type;
	if (SWALLOW_Type_1RADIO_RIGHT == swallow_type) {
		/*Radio in right,so icon is in left */
		item_data->l_swallow_path = (char *)g_strdup(button_style);
	} else {
		item_data->r_swallow_path = (char *)g_strdup(button_style);
	}
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->stop_change_cb = btn_clicked_cb;
	item_data->rgd = rgd;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	return item_data;
}

EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_2radio(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    const char *sub_desc,
    setting_call_back_func chk_change_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->rgd = rgd;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	return item_data;
}

/**
 * Create common style item with menu icon
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_groupitem(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    Elm_Object_Item *parent,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *l_icon_path,
    char *r_icon_path, int chk_status,
    const char *keyStr, char *sub_desc,
    setting_call_back_func
    chk_change_cb)
{
	/*	SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (
	                                           Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");

	/* to do formatting customizition in one place */
	if (SWALLOW_Type_LAYOUT_SPECIALIZTION == swallow_type
	    || SWALLOW_Type_LAYOUT_SPECIALIZTION_X == swallow_type) {
		elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

		char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
		char *temp1 = (char *)keyStr;
		char *temp2 = strchr(temp1, '&');
		char *temp = NULL;

		while (NULL != temp2) {
			if (0 == safeStrNCmp(temp2, "&amp;", strlen("&amp;")) ||/*'&' */
			    0 == safeStrNCmp(temp2, "&lt;", strlen("&lt;")) ||/*'<' */
			    0 == safeStrNCmp(temp2, "&gt;", strlen("&gt;")) ||/*'>' */
			    0 == safeStrNCmp(temp2, "&quot;", strlen("&quot;")) ||/*double quotes */
			    0 == safeStrNCmp(temp2, "&apos;", strlen("&apos;")) ||/*single quotes */
			    0 == safeStrNCmp(temp2, "&nbsp;", strlen("&nbsp;")) ||/*space character */
			    0 == safeStrNCmp(temp2, "&copy;", strlen("&copy;")) ||/*copyright character */
			    0 == safeStrNCmp(temp2, "&reg;", strlen("&reg;"))/*registeration character */
			   ) {/*escape character,do nothing */
				temp = temp2 + 1;
				if ('\0' == temp[0]) break;
				temp2 = strchr(temp, '&');
			} else {
				g_strlcat(speciliztion, temp1, temp2 - temp1 + 1);
				g_strlcat(speciliztion, "&amp;", MAX_SPECIALIZITION_LEN);
				temp1 = temp2 + 1;
				if ('\0' == temp1[0]) break;
				temp2 = strchr(temp1, '&');
			}
		}
		if ('\0' != temp1[0]) {
			g_strlcat(speciliztion, temp1, MAX_SPECIALIZITION_LEN);
		}


		item_data->keyStr = (char *)g_strdup(speciliztion);
	} else {
		item_data->keyStr = (char *)g_strdup(keyStr);
	}

	item_data->sub_desc = NULL;
	if (sub_desc)
		item_data->sub_desc = (char *)g_strdup(sub_desc);

	item_data->swallow_type = swallow_type;
	item_data->l_swallow_path = (char *)g_strdup(l_icon_path);
	item_data->r_swallow_path = (char *)g_strdup(r_icon_path);
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	const char *insert_type = evas_object_data_get(genlist, "InsertType");
	/*SETTING_TRACE("insert_type:%s", insert_type); */
	if (!gl_sel) gl_sel = _gl_Gendial_sel;
	if (0 == safeStrCmp(insert_type, "Insert before")) {
		item_data->item =
		    elm_genlist_item_insert_before(genlist, itc, item_data, parent, parent,
		                                   ELM_GENLIST_ITEM_NONE, gl_sel,
		                                   sel_data);
	} else if (0 == safeStrCmp(insert_type, "Insert after")) {
		item_data->item =
		    elm_genlist_item_insert_after(genlist, itc, item_data, parent, parent,
		                                  ELM_GENLIST_ITEM_NONE, gl_sel,
		                                  sel_data);
	} else if (0 == safeStrCmp(insert_type, "Prepend")) {
		item_data->item =
		    elm_genlist_item_prepend(genlist, itc, item_data, parent,
		                             ELM_GENLIST_ITEM_NONE, gl_sel,
		                             sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, parent,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	}
	/*help text clicking issue */
	if (SWALLOW_Type_LAYOUT_SPECIALIZTION_X == swallow_type)
		elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	evas_object_data_set(genlist, "InsertType", NULL);/*redundant process,it should be due to invokers,but maybe they forget it */

	return item_data;
}

EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_helpitem_without_bottom_separator(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    SWALLOW_Type swallow_type,
    const char *keyStr)
{
	/*	SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (
	                                           Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
	char *temp1 = (char *)keyStr;
	char *temp2 = strchr(temp1, '&');
	char *temp = NULL;

	while (NULL != temp2) {
		if (0 == safeStrNCmp(temp2, "&amp;", strlen("&amp;")) ||/*'&' */
		    0 == safeStrNCmp(temp2, "&lt;", strlen("&lt;")) ||/*'<' */
		    0 == safeStrNCmp(temp2, "&gt;", strlen("&gt;")) ||/*'>' */
		    0 == safeStrNCmp(temp2, "&quot;", strlen("&quot;")) ||/*double quotes */
		    0 == safeStrNCmp(temp2, "&apos;", strlen("&apos;")) ||/*single quotes */
		    0 == safeStrNCmp(temp2, "&nbsp;", strlen("&nbsp;")) ||/*space character */
		    0 == safeStrNCmp(temp2, "&copy;", strlen("&copy;")) ||/*copyright character */
		    0 == safeStrNCmp(temp2, "&reg;", strlen("&reg;"))/*registeration character */
		   ) {/*escape character,do nothing */
			temp = temp2 + 1;
			if ('\0' == temp[0]) break;
			temp2 = strchr(temp, '&');
		} else {
			g_strlcat(speciliztion, temp1, temp2 - temp1 + 1);
			g_strlcat(speciliztion, "&amp;", MAX_SPECIALIZITION_LEN);
			temp1 = temp2 + 1;
			if ('\0' == temp1[0]) break;
			temp2 = strchr(temp1, '&');
		}
	}
	if ('\0' != temp1[0]) {
		g_strlcat(speciliztion, temp1, MAX_SPECIALIZITION_LEN);
	}


	item_data->keyStr = (char *)g_strdup(speciliztion);
	item_data->swallow_type = swallow_type;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	item_data->item =
	    elm_genlist_item_append(genlist, itc, item_data, NULL,
	                            ELM_GENLIST_ITEM_NONE,
	                            NULL, NULL);

	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item_data;
}

EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_insert_help_without_bottom_separator(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    Elm_Object_Item *after_it,
    SWALLOW_Type swallow_type,
    const char *keyStr)
{
	/*	SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (
	                                           Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");

	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);

	char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
	char *temp1 = (char *)keyStr;
	char *temp2 = strchr(temp1, '&');
	char *temp = NULL;

	while (NULL != temp2) {
		if (0 == safeStrNCmp(temp2, "&amp;", strlen("&amp;")) ||/*'&' */
		    0 == safeStrNCmp(temp2, "&lt;", strlen("&lt;")) ||/*'<' */
		    0 == safeStrNCmp(temp2, "&gt;", strlen("&gt;")) ||/*'>' */
		    0 == safeStrNCmp(temp2, "&quot;", strlen("&quot;")) ||/*double quotes */
		    0 == safeStrNCmp(temp2, "&apos;", strlen("&apos;")) ||/*single quotes */
		    0 == safeStrNCmp(temp2, "&nbsp;", strlen("&nbsp;")) ||/*space character */
		    0 == safeStrNCmp(temp2, "&copy;", strlen("&copy;")) ||/*copyright character */
		    0 == safeStrNCmp(temp2, "&reg;", strlen("&reg;"))/*registeration character */
		   ) {/*escape character,do nothing */
			temp = temp2 + 1;
			if ('\0' == temp[0]) break;
			temp2 = strchr(temp, '&');
		} else {
			g_strlcat(speciliztion, temp1, temp2 - temp1 + 1);
			g_strlcat(speciliztion, "&amp;", MAX_SPECIALIZITION_LEN);
			temp1 = temp2 + 1;
			if ('\0' == temp1[0]) break;
			temp2 = strchr(temp1, '&');
		}
	}
	if ('\0' != temp1[0]) {
		g_strlcat(speciliztion, temp1, MAX_SPECIALIZITION_LEN);
	}


	item_data->keyStr = (char *)g_strdup(speciliztion);
	item_data->swallow_type = swallow_type;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	item_data->item =
	    elm_genlist_item_insert_after(genlist, itc, item_data, NULL, after_it,
	                                  ELM_GENLIST_ITEM_NONE,
	                                  NULL, NULL);

	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	return item_data;
}


/**
 * Common function to create a genlist item
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC Setting_GenGroupItem_Data *setting_create_Gendial_field_def(Evas_Object *
                                                                          genlist,
                                                                          const Elm_Genlist_Item_Class *itc,
                                                                          setting_call_back_func
                                                                          gl_sel,
                                                                          void *sel_data,
                                                                          SWALLOW_Type
                                                                          swallow_type,
                                                                          char *l_icon_path,
                                                                          char *r_icon_path,
                                                                          int chk_status,
                                                                          const char *keyStr,
                                                                          char *sub_desc,
                                                                          setting_call_back_func chk_change_cb)
{
	return setting_create_Gendial_field_groupitem(genlist, itc, NULL,
	                                              gl_sel, sel_data,
	                                              swallow_type, l_icon_path,
	                                              r_icon_path, chk_status,
	                                              keyStr, sub_desc,
	                                              chk_change_cb);
}

/**
 * Create 1entry style item
 * focus out
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_entry_fo(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *l_icon_path,
    char *r_icon_path,
    int chk_status,
    const char *keyStr,
    char *sub_desc,
    setting_call_back_func chk_change_cb,
    setting_call_back_func chk_focus_out_cb,
    Elm_Input_Panel_Layout input_type,
    bool isPasswordFlag,
    bool isFocusFlag,
    int min_char_count,
    int max_char_count,
    int max_byte_count,
    char *accepted,
    char *rejected)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->l_swallow_path = (char *)g_strdup(l_icon_path);
	item_data->r_swallow_path = (char *)g_strdup(r_icon_path);
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->stop_change_cb = chk_focus_out_cb;
	item_data->isSinglelineFlag = true;

	item_data->input_type = input_type;
	item_data->isPasswordFlag = isPasswordFlag;
	item_data->isFocusFlag = isFocusFlag;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	Elm_Entry_Filter_Accept_Set *digits_filter_data =
	    (Elm_Entry_Filter_Accept_Set *) calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
	if (!digits_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Accept_Set failed");
		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		G_FREE(item_data->itc);
		FREE(item_data);
		return NULL;
	}


	Elm_Entry_Filter_Limit_Size *limit_filter_data =
	    (Elm_Entry_Filter_Limit_Size *) calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (!limit_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Limit_Size failed");

		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		G_FREE(item_data->itc);
		FREE(item_data);
		FREE(digits_filter_data);
		return NULL;
	}

	limit_filter_data->max_char_count = max_char_count;
	limit_filter_data->max_byte_count = max_byte_count;
	digits_filter_data->accepted = (char *)g_strdup(accepted);
	digits_filter_data->rejected = (char *)g_strdup(rejected);

	item_data->digits_filter_data = digits_filter_data;
	item_data->limit_filter_data = limit_filter_data;
	item_data->userdata = sel_data;

	item_data->minlength = min_char_count;

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	return item_data;
}

/**
 * Create 1entry style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_entry(Evas_Object *
                                                              genlist,
                                                              const Elm_Genlist_Item_Class *itc,
                                                              setting_call_back_func gl_sel,
                                                              void *sel_data,
                                                              SWALLOW_Type
                                                              swallow_type,
                                                              char *l_icon_path,
                                                              char *r_icon_path,
                                                              int chk_status,
                                                              const char *keyStr,
                                                              char *sub_desc,
                                                              setting_call_back_func
                                                              chk_change_cb,
                                                              Elm_Input_Panel_Layout
                                                              input_type,
                                                              bool isPasswordFlag,
                                                              bool isFocusFlag,
                                                              int max_char_count,
                                                              int max_byte_count,
                                                              char *accepted,
                                                              char *rejected)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(1,
	                                                                            sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->l_swallow_path = (char *)g_strdup(l_icon_path);
	item_data->r_swallow_path = (char *)g_strdup(r_icon_path);
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;

	item_data->input_type = input_type;
	item_data->isPasswordFlag = isPasswordFlag;
	item_data->isFocusFlag = isFocusFlag;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}

	Elm_Entry_Filter_Accept_Set *digits_filter_data =
	    (Elm_Entry_Filter_Accept_Set *) calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
	if (!digits_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Accept_Set failed");
		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		G_FREE(item_data->itc);
		FREE(item_data);
		return NULL;
	}


	Elm_Entry_Filter_Limit_Size *limit_filter_data =
	    (Elm_Entry_Filter_Limit_Size *) calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (!limit_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Limit_Size failed");

		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		G_FREE(item_data->itc);
		FREE(item_data);
		FREE(digits_filter_data);
		return NULL;
	}

	limit_filter_data->max_char_count = max_char_count;
	limit_filter_data->max_byte_count = max_byte_count;
	digits_filter_data->accepted = (char *)g_strdup(accepted);
	digits_filter_data->rejected = (char *)g_strdup(rejected);

	item_data->digits_filter_data = digits_filter_data;
	item_data->limit_filter_data = limit_filter_data;
	item_data->userdata = sel_data;

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}

	if (swallow_type == SWALLOW_Type_LAYOUT_EDITFIELD)
		elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	return item_data;
}

/**
 * Create 1entry style item with parameter return_key_type
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_entry_with_return_key(Evas_Object *
                                                                              genlist,
                                                                              const Elm_Genlist_Item_Class *itc,
                                                                              setting_call_back_func gl_sel,
                                                                              void *sel_data,
                                                                              SWALLOW_Type
                                                                              swallow_type,
                                                                              char *l_icon_path,
                                                                              char *r_icon_path,
                                                                              int chk_status,
                                                                              const char *keyStr,
                                                                              char *sub_desc,
                                                                              setting_call_back_func
                                                                              chk_change_cb,
                                                                              setting_call_back_func
                                                                              max_reach_cb,
                                                                              Elm_Input_Panel_Layout
                                                                              input_type,
                                                                              bool isPasswordFlag,
                                                                              bool isFocusFlag,
                                                                              int max_char_count,
                                                                              int max_byte_count,
                                                                              char *accepted,
                                                                              char *rejected,
                                                                              Elm_Input_Panel_Return_Key_Type return_key_type,
                                                                              setting_call_back_func
                                                                              activated_cb,
                                                                              setting_call_back_func x_callback_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(1,
	                                                                            sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->l_swallow_path = (char *)g_strdup(l_icon_path);
	item_data->r_swallow_path = (char *)g_strdup(r_icon_path);
	item_data->chk_status = chk_status;
	item_data->chk_change_cb = chk_change_cb;
	item_data->maxlength_reached_cb = max_reach_cb;
	item_data->x_callback_cb = x_callback_cb;

	item_data->input_type = input_type;
	item_data->isPasswordFlag = isPasswordFlag;
	item_data->isSinglelineFlag = isPasswordFlag;
	item_data->isFocusFlag = isFocusFlag;
	item_data->return_key_type = return_key_type;
	item_data->disable_auto_cap = EINA_TRUE;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	Elm_Entry_Filter_Accept_Set *digits_filter_data =
	    (Elm_Entry_Filter_Accept_Set *) calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
	if (!digits_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Accept_Set failed");
		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		G_FREE(item_data->itc);
		FREE(item_data);
		return NULL;
	}


	Elm_Entry_Filter_Limit_Size *limit_filter_data =
	    (Elm_Entry_Filter_Limit_Size *) calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (!limit_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Limit_Size failed");

		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->l_swallow_path);
		G_FREE(item_data->r_swallow_path);
		FREE(item_data->itc);
		FREE(item_data);
		FREE(digits_filter_data);
		return NULL;
	}

	limit_filter_data->max_char_count = max_char_count;
	limit_filter_data->max_byte_count = max_byte_count;
	digits_filter_data->accepted = (char *)g_strdup(accepted);
	digits_filter_data->rejected = (char *)g_strdup(rejected);

	item_data->digits_filter_data = digits_filter_data;
	item_data->limit_filter_data = limit_filter_data;
	item_data->userdata = sel_data;

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}

	if (swallow_type == SWALLOW_Type_LAYOUT_EDITFIELD)
		elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	return item_data;
}

/**
 * Create 1entry style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_field_editfield(Evas_Object *
                                                                  genlist,
                                                                  const Elm_Genlist_Item_Class *itc,
                                                                  setting_call_back_func gl_sel,
                                                                  void *sel_data,
                                                                  SWALLOW_Type
                                                                  swallow_type,
                                                                  const char *keyStr,
                                                                  char *sub_desc,
                                                                  setting_call_back_func
                                                                  chk_change_cb,
                                                                  setting_call_back_func
                                                                  focused_cb,
                                                                  setting_call_back_func
                                                                  unfocused_cb,
                                                                  setting_call_back_func
                                                                  activated_cb,
                                                                  setting_call_back_func
                                                                  max_reached_cb,
                                                                  Elm_Input_Panel_Layout
                                                                  input_type,
                                                                  bool isPasswordFlag,
                                                                  bool isFocusFlag,
                                                                  int max_char_count,
                                                                  int max_byte_count,
                                                                  char *accepted,
                                                                  char *rejected)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(1,
	                                                                            sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->l_swallow_path = NULL;
	item_data->r_swallow_path = NULL;
	item_data->chk_status = 0;
	item_data->chk_change_cb = chk_change_cb;
	item_data->focus_cb = focused_cb;
	item_data->stop_change_cb = unfocused_cb;
	item_data->activated_cb = activated_cb;
	item_data->maxlength_reached_cb = max_reached_cb;

	item_data->input_type = input_type;
	item_data->isPasswordFlag = isPasswordFlag;
	if (isPasswordFlag)
		item_data->guide_text = (char *)g_strdup(_("IDS_ST_BODY_ENTER_PASSWORD"));
	item_data->isSinglelineFlag = isPasswordFlag;
	item_data->isFocusFlag = isFocusFlag;
	item_data->userdata = sel_data;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	Elm_Entry_Filter_Accept_Set *digits_filter_data =
	    (Elm_Entry_Filter_Accept_Set *) calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
	if (!digits_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Accept_Set failed");
		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->guide_text);
		G_FREE(item_data->itc);
		FREE(item_data);
		return NULL;
	}


	Elm_Entry_Filter_Limit_Size *limit_filter_data =
	    (Elm_Entry_Filter_Limit_Size *) calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (!limit_filter_data) {
		SETTING_TRACE_ERROR("calloc Elm_Entry_Filter_Limit_Size failed");

		/* free allocated data */
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		G_FREE(item_data->guide_text);
		G_FREE(item_data->itc);
		FREE(item_data);
		FREE(digits_filter_data);
		return NULL;
	}

	limit_filter_data->max_char_count = max_char_count;
	limit_filter_data->max_byte_count = max_byte_count;
	digits_filter_data->accepted = (char *)g_strdup(accepted);
	digits_filter_data->rejected = (char *)g_strdup(rejected);

	item_data->digits_filter_data = digits_filter_data;
	item_data->limit_filter_data = limit_filter_data;
	item_data->userdata = sel_data;

	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            _gl_Gendial_sel, sel_data);
	}
	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	return item_data;
}

/**
 * General function to create a parent item of expandable item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_exp_parent_field(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    const char *keyStr,
    char *sub_desc,
    setting_group_style exp_style,
    setting_group_style con_style)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->sub_desc = (char *)g_strdup(sub_desc);
	item_data->swallow_type = swallow_type;
	item_data->exp_style = exp_style;
	item_data->con_style = con_style;
	item_data->group_style = con_style;
	item_data->r_swallow_path = (char *)g_strdup(SETTING_IMAGE_PATH_CFG"00_list_button_expand_closed.png");
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	if (gl_sel) {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_TREE, gl_sel,
		                            sel_data);
	} else {
		item_data->item =
		    elm_genlist_item_append(genlist, itc, item_data, NULL,
		                            ELM_GENLIST_ITEM_TREE,
		                            _gl_Gendial_sel_expand, sel_data);
	}
	return item_data;
}

/**
 * General function to create a subitem of expandable item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_exp_sub_field(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    Elm_Object_Item *parent,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func
    chk_change_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->swallow_type = swallow_type;
	item_data->chk_status = chk_status;
	item_data->rgd = rgd;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	if (!gl_sel) {
		gl_sel = setting_sub_list_sel_cb;
	}
	if (!chk_change_cb) {
		chk_change_cb = setting_sub_list_rd_change;
	}

	item_data->chk_change_cb = chk_change_cb;
	item_data->item =
	    elm_genlist_item_append(genlist, itc, item_data, parent,
	                            ELM_GENLIST_ITEM_NONE, gl_sel, sel_data);

	return item_data;
}

/**
 * General function to create a subitem of expandable item with group style
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
EXPORT_PUBLIC
Setting_GenGroupItem_Data *setting_create_Gendial_exp_sub_field_with_group_style(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    Elm_Object_Item *parent,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func
    chk_change_cb,
    setting_group_style group_style)
{
	SETTING_TRACE_BEGIN
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
	                                         sizeof
	                                         (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, NULL, "calloc failed");
	item_data->keyStr = (char *)g_strdup(keyStr);
	item_data->swallow_type = swallow_type;
	item_data->chk_status = chk_status;
	item_data->rgd = rgd;
	if (itc && itc->item_style) {
		item_data->itc = (char *)g_strdup(itc->item_style);
	}
	if (!gl_sel) {
		gl_sel = setting_sub_list_sel_cb;
	}
	if (!chk_change_cb) {
		chk_change_cb = setting_sub_list_rd_change;
	}

	item_data->chk_change_cb = chk_change_cb;
	item_data->group_style = group_style;
	item_data->item =
	    elm_genlist_item_append(genlist, itc, item_data, parent,
	                            ELM_GENLIST_ITEM_NONE, gl_sel, sel_data);
	return item_data;
}

/**
 * To make genlist support expandable style item
 */
EXPORT_PUBLIC
void setting_enable_expandable_genlist(Evas_Object *genlist, void *data,
                                       setting_call_back_func gl_exp_cb,
                                       setting_call_back_func gl_smart_cb)
{
	ret_if(NULL == genlist);
	elm_genlist_tree_effect_enabled_set(genlist, EINA_FALSE);
	if (!gl_smart_cb) {
		gl_smart_cb = __exp_list_smart_cb;
	}

	if (gl_exp_cb)
		evas_object_smart_callback_add(genlist, "expanded", gl_exp_cb, data);
	evas_object_smart_callback_add(genlist, "contracted", gl_smart_cb, "contracted");
	evas_object_smart_callback_add(genlist, "expanded", gl_smart_cb, "expanded");

	evas_object_smart_callback_add(genlist, "drag", gl_smart_cb, "drag");
	evas_object_smart_callback_add(genlist, "longpressed", gl_smart_cb, "longpressed");
}

/*
example:

setting_create_Gendial_itc("dialogue/1text.2icon.2",
							&(networkUG->itc_1text_2icon));

1. create a process genlist item : [intial state is 'PROCESS']
ad->data_searching_net = setting_create_Gendial_field_1radio(scroller, &(ad->itc_1text_2icon),
						setting_network_select_network_mouse_up_Gendial_list_radio_cb, ad,
						SWALLOW_Type_1RADIO_1PROCESS,
						ad->chk_sel, TAPI_NETWORK_SELECTIONMODE_MANUAL,
						"Searching..", NULL);
or [intial state is 'UNPROCESS']
ad->data_searching_net = setting_create_Gendial_field_1radio(scroller, &(ad->itc_1text_2icon),
						setting_network_select_network_mouse_up_Gendial_list_radio_cb, ad,
						SWALLOW_Type_1RADIO,
						ad->chk_sel, TAPI_NETWORK_SELECTIONMODE_MANUAL,
						"Searching..", NULL);

2. to set the PROCESS state at any place as you like:
setting_begin_progress_genlist_item(ad->data_searching_net); -- change from UNPROCESS->PROCESS
setting_finish_progress_genlist_item(ad->data_searching_net);--change from PROCESS->UNPROCESS
*/
EXPORT_PUBLIC
void setting_begin_progress_genlist_item(Setting_GenGroupItem_Data *data_item)
{
	if (data_item) {
		if (SWALLOW_Type_1RADIO != data_item->swallow_type) {
			return;
		}

		data_item->swallow_type = SWALLOW_Type_1RADIO_1PROCESS;
		elm_object_item_data_set(data_item->item, data_item);
		elm_genlist_item_update(data_item->item);
	}
}

EXPORT_PUBLIC
void setting_finish_progress_genlist_item(Setting_GenGroupItem_Data *data_item)
{
	if (data_item) {
		if (SWALLOW_Type_1RADIO_1PROCESS != data_item->swallow_type) {
			return;
		}
		data_item->swallow_type = SWALLOW_Type_1RADIO;
		elm_object_item_data_set(data_item->item, data_item);
		elm_genlist_item_update(data_item->item);
	}
}

/**
* Check if some item is selected in done-list
*/
EXPORT_PUBLIC
bool setting_done_list_is_some_item_selected(Setting_Done_List_Data *list_data)
{
	SETTING_TRACE_BEGIN;
	bool isFound = FALSE;	/* wether can found some item checked */
	int idx = 0;
	for (; idx < list_data->cur_item_num; idx++) {
		if (list_data->chk_items[idx].data_GenItem->chk_status) {
			isFound = TRUE;
			break;
		}
	}
	return isFound;
}

static void __setting_done_list_item_sel(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data == NULL");
	Setting_Done_List_Data *list_data = (Setting_Done_List_Data *)data;

	int sel_num = 0;
	int idx = 0;
	/* else for other checks, do rotinue checking */
	bool isFound = FALSE;	/* wether found the item not checked */
	for (idx = 0; idx < list_data->cur_item_num; idx++) {
		if (!list_data->chk_items[idx].data_GenItem->chk_status) {
			/* SETTING_TRACE("%s not select", _(list_data->chk_items[idx].item_name)); */
			isFound = TRUE;
			/* break; */
		} else {
			/* SETTING_TRACE("%s select", _(list_data->chk_items[idx].item_name)); */
			sel_num++;
		}
	}

	list_data->cur_item_selnum = sel_num;
	if (isFound) {	/* reset state of chk_sel_all */
		list_data->select_all_checked = 0;
		elm_check_state_pointer_set(list_data->select_all_checkbox, &list_data->select_all_checked);
	} else {
		list_data->select_all_checked = 1;
		elm_check_state_pointer_set(list_data->select_all_checkbox, &list_data->select_all_checked);
	}

	if (sel_num > 0) {
		char text[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
		snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, _("IDS_ST_HEADER_PD_SELECTED"), sel_num);
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", text);
	} else {
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", _(list_data->navi_title));
	}

	/* SETTING_TRACE("sel_num:%d", sel_num); */
	if (!sel_num) {
		if (list_data->navi_it) {
			Evas_Object  *toolbar = elm_object_item_part_content_get(list_data->navi_it, "toolbar");
			elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		}
		evas_object_hide(list_data->selInfoPop);
		return;
	}

	if (list_data->navi_it) {
		Evas_Object  *toolbar = elm_object_item_part_content_get(list_data->navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	}
}

/**
* Do process when clicking the check object in the done-list item
*/
EXPORT_PUBLIC
void setting_done_list_Gendial_chk_btn_cb(void *data, Evas_Object *obj,
                                          void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	/* new status */
	list_item->chk_status = elm_check_state_get(obj);

	Setting_Done_List_Data *list_data = list_item->userdata;
	__setting_done_list_item_sel(list_data);
}

static void setting_done_list_Gendial_mouse_up_cb(void *data, Evas_Object *obj,
                                                  void *event_info)
{
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	/* new status */
	setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));

	/* other process.. */
	Setting_Done_List_Data *list_data = list_item->userdata;
	__setting_done_list_item_sel(list_data);
}

static void __setting_select_all_done(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data == NULL");
	Setting_Done_List_Data *list_data = (Setting_Done_List_Data *)data;

	int sel_num = 0;
	int idx = 0;

	if (list_data->select_all_checked) {
		/* set all other checks state to be 1 */
		SETTING_TRACE_DEBUG("Select All checked");
		for (idx = 0; idx < list_data->cur_item_num; idx++) {
			list_data->chk_items[idx].data_GenItem->chk_status = 1;
			elm_object_item_data_set(list_data->chk_items[idx].data_GenItem->item, list_data->chk_items[idx].data_GenItem);
			elm_genlist_item_update(list_data->chk_items[idx].data_GenItem->item);
		}
		sel_num = list_data->cur_item_num;
	} else {
		SETTING_TRACE_DEBUG("Select All unchecked");
		/* set all other checks state to be 0 */
		for (idx = 0; idx < list_data->cur_item_num; idx++) {
			list_data->chk_items[idx].data_GenItem->chk_status = 0;
			elm_object_item_data_set(list_data->chk_items[idx].data_GenItem->item, list_data->chk_items[idx].data_GenItem);
			elm_genlist_item_update(list_data->chk_items[idx].data_GenItem->item);
		}
		sel_num = 0;
	}

	/*  text set and timeout set */
	if (sel_num > 0) {
		char text[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
		snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, _("IDS_ST_HEADER_PD_SELECTED"), sel_num);
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", text);
	} else {
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", _(list_data->navi_title));
	}
	/* SETTING_TRACE("sel_num:%d", sel_num); */
	if (!sel_num) {
		if (list_data->navi_it) {
			Evas_Object *toolbar = elm_object_item_part_content_get(list_data->navi_it, "toolbar");
			elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		}

		evas_object_hide(list_data->selInfoPop);
		return;
	}

	if (list_data->navi_it) {
		Evas_Object *toolbar = elm_object_item_part_content_get(list_data->navi_it, "toolbar");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	}
}

static void __setting_select_all_layout_mouse_up_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data == NULL");
	Setting_Done_List_Data *list_data = (Setting_Done_List_Data *)data;

	list_data->select_all_checked = !list_data->select_all_checked;
	/*elm_check_state_pointer_set(list_data->select_all_checkbox, &list_data->select_all_checked); */

	__setting_select_all_done(list_data);
}

/*static void __setting_select_all_check_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data == NULL");
	Setting_Done_List_Data *list_data = (Setting_Done_List_Data *)data;
	list_data->select_all_checkbox = obj;
	list_data->select_all_checked = elm_check_state_get(list_data->select_all_checkbox);
	elm_check_state_pointer_set(list_data->select_all_checkbox, &list_data->select_all_checked);
	__setting_select_all_done(data);
}*/
static void setting_callback_select_all_button_pressed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data is NULL");

	Evas_Object *select_button_ic = (Evas_Object *)data;
	elm_image_file_set(select_button_ic, SETTING_ICON_PATH_CFG"select_all.png", NULL);
}
static void setting_callback_select_all_button_unpressed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data is NULL");

	Evas_Object *select_button_ic = (Evas_Object *)data;
	elm_image_file_set(select_button_ic, SETTING_ICON_PATH_CFG"select_all.png", NULL);
}
/**
* The API to create done-list.
*
* @param[in] list_data
* @return a done-list
*/
EXPORT_PUBLIC
Evas_Object *setting_create_done_list(Setting_Done_List_Data *list_data, const char *title)
{
	SETTING_TRACE_BEGIN;
	retvm_if(list_data == NULL, NULL, "list_data == NULL");
	setting_create_Gendial_itc("1line", &(list_data->itc_1text_1icon));
	if (title) {
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", _(title));
	} else {
		title = elm_object_item_part_text_get(list_data->navi_it, "elm.text.title");
	}

	memset((char *)(list_data->navi_title), 0, sizeof(list_data->navi_title));
	safeCopyStr((char *)(list_data->navi_title), title, sizeof(list_data->navi_title));
	SETTING_TRACE("list_data->navi_title:%s", list_data->navi_title);

	Evas_Object *allbtn = elm_button_add(list_data->win_main);
	elm_object_style_set(allbtn, "naviframe/title_icon");

	Evas_Object *icon = elm_icon_add(allbtn);
	elm_image_file_set(icon, SETTING_ICON_PATH_CFG"select_all.png", NULL);

	elm_object_content_set(allbtn, icon);
	elm_object_focus_allow_set(allbtn, EINA_FALSE);
	evas_object_propagate_events_set(allbtn, EINA_FALSE);
	evas_object_smart_callback_add(allbtn, "clicked", __setting_select_all_layout_mouse_up_cb, list_data);
	evas_object_smart_callback_add(allbtn, "pressed", setting_callback_select_all_button_pressed_cb, icon);
	evas_object_smart_callback_add(allbtn, "unpressed", setting_callback_select_all_button_unpressed_cb, icon);

	evas_object_show(allbtn);
	elm_object_item_part_content_set(list_data->navi_it, "title_right_btn", allbtn);

	Evas_Object *scroller = elm_genlist_add(list_data->win_main);
	retvm_if(scroller == NULL, NULL, "scroller == NULL");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_genlist_clear(scroller);	/* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);


	int index = 0;
	for (; index < list_data->cur_item_num; index++) {
		list_data->chk_items[index].data_GenItem =
		    setting_create_Gendial_field_def(scroller,
		                                     &(list_data->itc_1text_1icon),
		                                     setting_done_list_Gendial_mouse_up_cb,
		                                     list_data,
		                                     SWALLOW_Type_1CHECK, NULL,
		                                     NULL, 0,
		                                     list_data->chk_items
		                                     [index].item_name, NULL,
		                                     setting_done_list_Gendial_chk_btn_cb);
		if (list_data->chk_items[index].data_GenItem) {
			list_data->chk_items[index].data_GenItem->userdata = list_data;
		} else {
			SETTING_TRACE_ERROR("list_data->chk_items[index].data_GenItem is NULL");
		}

	}

	return scroller;
}

/**
* The API to create done-list-with-image
*
* @param[in] list_data
* @return a done-list-with-image
*/
EXPORT_PUBLIC
Evas_Object *setting_create_done_list_with_image(Setting_Done_List_Data *list_data, const char *title)
{
	SETTING_TRACE_BEGIN;
	retvm_if(list_data == NULL, NULL, "list_data == NULL");
	setting_create_Gendial_itc("1text.2icon.4", &(list_data->itc_1text_2icon));
	if (title) {
		elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", title);
	}
	/*elm_object_item_part_text_set(list_data->navi_it, "elm.text.title", _(KeyStr_Select_Item)); */
	memset((char *)(list_data->navi_title), 0, sizeof(list_data->navi_title));
	safeCopyStr((char *)(list_data->navi_title), elm_object_item_part_text_get(list_data->navi_it, "elm.text.title"), sizeof(list_data->navi_title));
	SETTING_TRACE("list_data->navi_title:%s", list_data->navi_title);

	Evas_Object *allbtn = elm_button_add(list_data->win_main);
	elm_object_style_set(allbtn, "naviframe/title_icon");
	Evas_Object *icon = elm_icon_add(allbtn);
	elm_image_file_set(icon, SETTING_ICON_PATH_CFG"select_all.png", NULL);

	/*evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1 , 1); */
	/*elm_image_resizable_set(icon, EINA_TRUE, EINA_TRUE); */
	elm_object_content_set(allbtn, icon);
	elm_object_focus_allow_set(allbtn, EINA_FALSE);
	evas_object_propagate_events_set(allbtn, EINA_FALSE);
	evas_object_smart_callback_add(allbtn, "clicked", __setting_select_all_layout_mouse_up_cb, list_data);
	evas_object_smart_callback_add(allbtn, "pressed", setting_callback_select_all_button_pressed_cb, icon);
	evas_object_smart_callback_add(allbtn, "unpressed", setting_callback_select_all_button_unpressed_cb, icon);

	evas_object_show(allbtn);
	elm_object_item_part_content_set(list_data->navi_it, "title_right_btn", allbtn);

	Evas_Object *scroller = elm_genlist_add(list_data->win_main);
	retvm_if(scroller == NULL, NULL, "scroller == NULL");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_genlist_clear(scroller);	/* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	int index = 0;
	for (; index < list_data->cur_item_num; index++) {
		list_data->chk_items[index].data_GenItem =
		    setting_create_Gendial_field_def(scroller,
		                                     &(list_data->itc_1text_2icon),
		                                     setting_done_list_Gendial_mouse_up_cb,
		                                     list_data,
		                                     SWALLOW_Type_1CHECK, NULL,
		                                     list_data->allow_items[index].thumb_path, 0,
		                                     list_data->chk_items
		                                     [index].item_name, NULL,
		                                     setting_done_list_Gendial_chk_btn_cb);
		if (list_data->chk_items[index].data_GenItem) {
			list_data->chk_items[index].data_GenItem->userdata = list_data;
		} else {
			SETTING_TRACE_ERROR("list_data->chk_items[index].data_GenItem is NULL");
		}
	}

	return scroller;
}

/**
* To disable a specialized genlist item
*
* @param[in] item
*/
EXPORT_PUBLIC
void setting_disable_genlist_item(Elm_Object_Item *item)
{
	if (item) {
		elm_object_item_disabled_set(item, EINA_TRUE); /* make it be dim displayed */
		/*elm_genlist_item_update(item); */
	}
}

/**
* To enable a specialized genlist item
*
* @param[in] item
*/
EXPORT_PUBLIC
void setting_enable_genlist_item(Elm_Object_Item *item)
{
	if (item) {
		elm_object_item_disabled_set(item, EINA_FALSE);	/* make it be normally displayed */
		/*elm_genlist_item_update(item); */
	}
}

EXPORT_PUBLIC
void setting_genlist_item_disabled_set(Setting_GenGroupItem_Data *item_data, int disable)
{
	if (item_data && item_data->item) {
		elm_object_item_disabled_set(item_data->item, disable); /* make it be dim displayed */
	}
}
EXPORT_PUBLIC void setting_genlist_unregister_tts_access_highlight(Evas_Object *obj, const char *itc)
{
	setting_retm_if(!obj, "obj is NULL");
	if (!itc || (itc && 0 != safeStrCmp(itc, "dialogue/1text.2icon.divider")
	             && 0 != safeStrCmp(itc, "dialogue/1text.1icon.divider")
	             && 0 != safeStrCmp(itc, "dialogue/setting.1text.2icon.4.divider.tb")
	             && 0 != safeStrCmp(itc, "dialogue/setting.1text.2icon.divider")
	             && 0 != safeStrCmp(itc, "dialogue/newset.1text.2icon.divider")))	{

		/*SETTING_TRACE("check/radio highlight is unregistered"); */
		elm_access_object_unregister(obj);
	}
}


EXPORT_PUBLIC
void __add_gl_tts_feature(Elm_Object_Item *item)
{
	/*SETTING_TRACE_BEGIN; */
	ret_if(!item);
	const Elm_Genlist_Item_Class *itc = elm_genlist_item_item_class_get(item);
	ret_if(!itc);
	/*SETTING_TRACE("itc->item_style:%s", itc->item_style); */
	if (0 == safeStrCmp(itc->item_style, "dialogue/separator")) {
		elm_object_item_access_unregister(item);
		return;
	}

	/*all items which may have text to read */
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
	ret_if(!list_item);
	if (isSpaceStr(list_item->keyStr)) {
		elm_object_item_access_unregister(item);
		return;
	}

	if (!safeStrCmp(itc->item_style, "1icon/with_no_line")
	    && list_item->swallow_type == SWALLOW_Type_1BUTTON) {
		elm_object_item_access_unregister(item);
		return;
	}

	char lable[MAX_SPECIALIZITION_LEN] = {0,};
	char state[MAX_SPECIALIZITION_LEN] = {0,};
	char guide[MAX_SPECIALIZITION_LEN] = {0,};



	if (list_item) {
		/*SETTING_TRACE("list_item->keyStr:%s", _(list_item->keyStr)); */
		if (list_item->swallow_type == SWALLOW_Type_1TOGGLE
		    || list_item->swallow_type == SWALLOW_Type_1ICON_1RADIO
		    || list_item->swallow_type == SWALLOW_Type_1RADIO
		    || list_item->swallow_type == SWALLOW_Type_1CHECK
		    || list_item->swallow_type == SWALLOW_Type_1RADIO_1LABLE
		    || list_item->swallow_type == SWALLOW_Type_1ICON_1DOTTOOGLE) {
			/*unregister_access_highlight(list_item); */

		}
		switch (list_item->swallow_type) {
			case SWALLOW_Type_1TOGGLE:
			case SWALLOW_Type_1ICON_1RADIO: {
					/*SETTING_TRACE("case SWALLOW_Type_1TOGGLE or SWALLOW_Type_1ICON_1RADIO"); */
					/*"dialogue/1text.2icon.divider".keystr current is on/off,switch the switcher to chaneg it,click the item for detail settings" */
					/*not:keystr current is on/off,switch the switcher or click the item to chaneg it. */
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					snprintf(state, sizeof(state),
					         _("IDS_ST_BODY_P1SS_IS_CURRENTLY_P2SS_T_TTS"),
					         _(list_item->keyStr), list_item->chk_status ? _("IDS_ST_BODY_ON_M_STATUS") : _("IDS_ST_BODY_ALERTTYPE_OFF"));

					if (0 == safeStrCmp(itc->item_style, "dialogue/1text.2icon.divider")
					    || 0 == safeStrCmp(itc->item_style, "dialogue/1text.1icon.divider")) {
						/*g_strlcat(guide, _("Tap switcher to change it, or double tap the item for detail settings"), MAX_SPECIALIZITION_LEN); */
						g_strlcat(guide, _("IDS_ST_BODY_TOGGLE_THE_BUTTON_TO_ENABLE_OR_DISABLE_SCREEN_READER_OR_DOUBLE_TAP_TO_OPEN_THE_MENU_T_TTS"), MAX_SPECIALIZITION_LEN);
					} else {
						/*g_strlcat(guide, _("Tap item or switcher to chaneg it."), MAX_SPECIALIZITION_LEN); */
						g_strlcat(guide, _("IDS_ST_BODY_DOUBLE_TAP_TO_EDIT_THIS_FIELD_T_TTS"), MAX_SPECIALIZITION_LEN);
					}
					break;
				}
			case SWALLOW_Type_1CHECK:
			case SWALLOW_Type_1RADIO:
			case SWALLOW_Type_1RADIO_1LABLE: {
					/*SETTING_TRACE("case SWALLOW_Type_1CHECK or SWALLOW_Type_1RADIO or SWALLOW_Type_1RADIO_1LABLE"); */
					/*keystr, click to select it */
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					/*g_strlcat(guide, _("Double tap it to select"), MAX_SPECIALIZITION_LEN); */
					g_strlcat(guide, _("IDS_ST_BODY_DOUBLE_TAP_TO_EDIT_THIS_FIELD_T_TTS"), MAX_SPECIALIZITION_LEN);
					break;
				}
			case SWALLOW_Type_1ICON_1PROCESS: {
					/*SETTING_TRACE("case SWALLOW_Type_1ICON_1PROCESS"); */
					/*keystr,its setting is in processing */
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					g_strlcat(guide, _("IDS_ST_BODY_YOUR_CHANGES_ARE_BEING_MADE_T_TTS"), MAX_SPECIALIZITION_LEN);
					break;
				}
			case SWALLOW_Type_LAYOUT_ENTRY:
			case SWALLOW_Type_LAYOUT_EDITFIELD: {
					/*SETTING_TRACE("case SWALLOW_Type_LAYOUT_ENTRY"); */
					/*keystr is substr,to tap and modify it if you want */
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					snprintf(state, sizeof(state), _("IDS_ST_BODY_P1SS_IS_CURRENTLY_P2SS_T_TTS"), _(list_item->keyStr), list_item->sub_desc);
					g_strlcat(guide, _("IDS_ST_BODY_DOUBLE_TAP_TO_EDIT_THIS_FIELD_T_TTS"), MAX_SPECIALIZITION_LEN);
					break;
				}
			case SWALLOW_Type_LAYOUT_DATEFIELD: {
					/*year month ... */
					/*to do.. */
					/*SETTING_TRACE("case SWALLOW_Type_LAYOUT_DATEFIELD"); */
					break;
				}
			case SWALLOW_Type_INVALID:
			case SWALLOW_Type_1ICON_1DOTTOOGLE: {
					/*SETTING_TRACE("case SWALLOW_Type_INVALID"); */
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					/*SETTING_TRACE("list_item->sub_desc:%s", list_item->sub_desc); */
					if (list_item->sub_desc) {
						snprintf(state, sizeof(state), _("IDS_ST_BODY_P1SS_IS_CURRENTLY_P2SS_T_TTS"), _(list_item->keyStr), _(list_item->sub_desc));
					}
					/*SETTING_TRACE("state:%s", state); */

					if (0 != safeStrCmp(itc->item_style, "dialogue/multiline/2text")
					    && 0 != safeStrCmp(itc->item_style, "dialogue/2text.1icon.3")) {
						if (0 == safeStrCmp(itc->item_style, "dialogue/2text.3/expandable")) {
							g_strlcat(guide, _("IDS_ST_BODY_DOUBLE_TAP_TO_OPEN_THE_MENU_T_TTS"), MAX_SPECIALIZITION_LEN);
						} else
							g_strlcat(guide, _(TTS_CONTENT_MENU_CLICK_INFO), MAX_SPECIALIZITION_LEN);
					}
					/*SETTING_TRACE("guild:%s", guide); */
					break;
				}
				/*read the KEYSTR directly */
			case SWALLOW_Type_1TEXT_1IMAGE: {
					g_strlcat(lable, _(list_item->keyStr), MAX_SPECIALIZITION_LEN);
					break;
				}
			case SWALLOW_Type_LAYOUT_SPECIALIZTION:
			default: { /*skip them */
					return;
				}
		}

		/*SETTING_TRACE("speciliztion:%s", speciliztion); */
		Evas_Object *eo = elm_object_item_access_object_get(item);
		/*setting_set_tts_info(eo, speciliztion, " ", NULL, TTS_CONTENT_MENU_CLICK_INFO); */
		setting_set_tts_info(eo, lable, " ", isEmptyStr(state) ? NULL : state, isEmptyStr(guide) ? NULL : guide);
	}
}

EXPORT_PUBLIC
void setting_check_genlist_item_bottom_line(Elm_Object_Item *item)
{
	/*SETTING_TRACE_BEGIN; */
	ret_if(!item);
	const Elm_Genlist_Item_Class *itc = elm_genlist_item_item_class_get(item);
	if (itc && itc->item_style) {
		/*SETTING_TRACE("itc->item_style:%s", itc->item_style); */
		if (0 == safeStrCmp(itc->item_style, "groupindex")
		    || 0 == safeStrNCmp(itc->item_style, "dialogue/separator", safeStrLen("dialogue/separator"))
		    || 0 == safeStrNCmp(itc->item_style, "bottom_line", safeStrLen("bottom_line"))
		    || 0 == safeStrNCmp(itc->item_style, "multiline_sub", safeStrLen("multiline_sub"))
		    || 0 == safeStrNCmp(itc->item_style, "1icon", safeStrLen("1icon"))
		   ) {
			if (0 == safeStrCmp(itc->item_style, "groupindex")) {
				Elm_Object_Item *prev_item = elm_genlist_item_prev_get(item);
				if (prev_item) {
					const Elm_Genlist_Item_Class *itc2 = elm_genlist_item_item_class_get(prev_item);
					if (itc2 && !safeStrCmp(itc2->item_style, "bottom_line")) {
						elm_object_item_del(prev_item);
					}
				}
			}
			return;
		} else {

			/*SETTING_TRACE("Adding[prev_item:%p, next_item:%p].....", prev_item, next_item); */
			Evas_Object *scroller = elm_object_item_widget_get(item);
			Elm_Object_Item *parent = elm_genlist_item_parent_get(item);
			/*SETTING_TRACE("parent:%p", parent); */
			if (parent) { /*expandable */
				Elm_Object_Item *it = elm_genlist_item_insert_before(scroller, &(itc_bottom_line), NULL, parent,
				                                                     item, ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(it, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
				//Evas_Object *real_obj = elm_object_item_edje_get(item);
				//setting_decorate_image_RGBA(real_obj, 0, 0, 85, 255);
			}

		}
	}
}

EXPORT_PUBLIC
void __gl_realized_cb(void *data, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retm_if(event_info == NULL, "invalid parameter: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
#if SUPPORT_TTS
	__add_gl_tts_feature(item);
#endif
	setting_check_genlist_item_bottom_line(item);

	if (list_item) {
		/*SETTING_TRACE("%s:%d", _(list_item->keyStr), list_item->group_style); */
		if (list_item->group_style == SETTING_GROUP_STYLE_TOP) {
			elm_object_item_signal_emit(item, "elm,state,top", "");
		} else if (list_item->group_style == SETTING_GROUP_STYLE_BOTTOM) {
			elm_object_item_signal_emit(item, "elm,state,bottom", "");
		} else if (list_item->group_style == SETTING_GROUP_STYLE_CENTER) {
			elm_object_item_signal_emit(item, "elm,state,center", "");
		} else {
			elm_object_item_signal_emit(item, "elm,state,normal", "");
		}
	} else {
		/* item is used for description */
		elm_object_item_signal_emit(item, "elm,state,center", "");
	}
}

EXPORT_PUBLIC void setting_genlist_item_groupstyle_set(Setting_GenGroupItem_Data *list_item, setting_group_style group_style)
{
	if (list_item) {
		list_item->group_style = group_style;
		/*SETTING_TRACE("list_item->group_style:%d", list_item->group_style); */
		if (list_item->item) {
			/*SETTING_TRACE("%s:%d", _(list_item->keyStr), list_item->group_style); */
			if (list_item->group_style == SETTING_GROUP_STYLE_TOP) {
				elm_object_item_signal_emit(list_item->item, "elm,state,top", "");
			} else if (list_item->group_style == SETTING_GROUP_STYLE_BOTTOM) {
				elm_object_item_signal_emit(list_item->item, "elm,state,bottom", "");
			} else if (list_item->group_style == SETTING_GROUP_STYLE_CENTER) {
				elm_object_item_signal_emit(list_item->item, "elm,state,center", "");
			} else {
				elm_object_item_signal_emit(list_item->item, "elm,state,normal", "");
			}
		}
	}
}

/*for check,radio,slider etc.. */
EXPORT_PUBLIC void setting_update_gl_item_chk_status(Setting_GenGroupItem_Data *item_data, int status)
{
	if (item_data && item_data->chk_status != status) {
		item_data->chk_status = status;
		if (item_data->item) {
			/*SETTING_TRACE_DEBUG("update item %d", item_data->chk_status); */
			elm_genlist_item_fields_update(item_data->item, "*", ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

/*do not check current status, only update the icon.2 of item which has 2 icons. */
EXPORT_PUBLIC void setting_force_update_gl_item_chk_status(Setting_GenGroupItem_Data *item_data, int status)
{
	if (item_data) {
		item_data->chk_status = status;
		if (item_data->item) {
			/*SETTING_TRACE_DEBUG("update item %s : %d", item_data->keyStr, item_data->chk_status); */
			elm_genlist_item_fields_update(item_data->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}
}

