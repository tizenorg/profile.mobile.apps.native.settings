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
#include <setting-network-preferred-network-list.h>


static int setting_network_preferred_network_list_create(void *cb);
static int setting_network_preferred_network_list_destroy(void *cb);
static int setting_network_preferred_network_list_update(void *cb);
static int setting_network_preferred_network_list_cleanup(void *cb);

static Evas_Object *__gl_network_radio_group = NULL;
static char *STR_SETTING_NETWORK_LIST = "IDS_ST_OPT_NETWORK_LIST";

setting_view setting_view_network_preferred_network_list = {
	.create = setting_network_preferred_network_list_create,
	.destroy = setting_network_preferred_network_list_destroy,
	.update = setting_network_preferred_network_list_update,
	.cleanup = setting_network_preferred_network_list_cleanup,
};

#define LIST_COUNT 100
#define MAX_PLMN_LEN 6

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int gl_network_genlist_label_initialize(SettingNetworkUG *ad)
{
	Elm_Object_Item *gl_item = NULL;
	char *sel_mem = NULL;

	if (ad->gl_network) {
		gl_item = elm_genlist_first_item_get(ad->gl_network);
		while (gl_item) {
			sel_mem = (char *)elm_object_item_data_get(gl_item);
			if (sel_mem) {
				FREE(sel_mem);
			}
			gl_item = elm_genlist_item_next_get(gl_item);
		}
		elm_genlist_clear(ad->gl_network);

		return 0;
	}

	return SETTING_RETURN_FAIL;
}

static char *gl_network_genlist_text_get(void *data, Evas_Object *obj,
                                         const char *part)
{
	char buf[PATH_MAX];

	struct gl_network_data *sel_mem = (struct gl_network_data *)data;

	if (safeStrCmp(part, "elm.text") == 0) {
		/* strcpy(buf, data); */
		/* sprintf(buf, "<font color=#080808>%s</font>", sel_mem->name); */
		snprintf(buf, sizeof(buf), "%s", sel_mem->name);
		return strdup(buf);
	} else {
		safeCopyStr(buf, "", strlen(""));
		return strdup(buf);
	}
}

static void gl_network_genlist_delete(void *data, Evas_Object *obj)
{
	struct gl_network_data *one_item = (struct gl_network_data *)data;
	if (data) {
		FREE(one_item);
	}
	return;
}

static Evas_Object *gl_network_getlist_content_get(void *data, Evas_Object *obj,
                                                   const char *part)
{
	Evas_Object *radio = NULL;
	struct gl_network_data *sel_mem = (struct gl_network_data *)data;

	if (safeStrCmp(part, "elm.swallow.end") == 0
	    || safeStrCmp(part, "elm.icon") == 0) {
		radio = elm_radio_add(obj);
		retv_if(radio == NULL, NULL);
		elm_radio_state_value_set(radio, sel_mem->index);
		elm_radio_group_add(radio, __gl_network_radio_group);
	}
	return radio;
}

static void gl_network_genlist_select(void *data, Evas_Object *obj,
                                      void *event_info)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Elm_Object_Item *item = NULL;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);

	ad->cur_sel_mem =
	    (struct gl_network_data *)elm_object_item_data_get(item);

	if (ad->cur_sel_mem == NULL) {
		return;
	}

	ad->cur_sel_index = ad->cur_sel_mem->index;
	elm_radio_value_set(__gl_network_radio_group, ad->cur_sel_index);
	/* SETTING_TRACE_WARNING("******sel_mem->index = %d, elm_radio_value_get = %d", ad->cur_sel_index, elm_radio_value_get(__gl_network_radio_group)); */
	return;
}

static void __get_preferred_network_list_item_name(char *name,
                                                   settings_preferred_network_table
                                                   network)
{
	setting_retm_if(NULL == name, "NULL == name")
	const char *nw_type_str = NULL;
	switch (network.nw_type) {
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_900: {
				nw_type_str = "GSM900";
				break;
			}
		case SETTING_PREFERRED_NETWORK_TYPE_DCS_1800: {
				nw_type_str = "DCS";
				break;
			}
		case SETTING_PREFERRED_NETWORK_TYPE_PCS_1900: {
				nw_type_str = "PCS";
				break;
			}
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT: {
				nw_type_str = "GSMSAT";
				break;
			}
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_850: {
				nw_type_str = "GSM850";
				break;
			}
		case SETTING_PREFERRED_NETWORK_TYPE_UMTS: {
				nw_type_str = "UMTS";
				break;
			}
		default: {
				nw_type_str = NULL;
				break;
			}
	}
	snprintf(name, 255, "%s (%s)", network.short_name_ptr, nw_type_str);
}

static Eina_Bool __genlist_update(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, 0, "NULL == data")

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	const char *match_str = NULL;

	SETTING_TRACE("match_str:%s", match_str);

	struct gl_network_data *one_item;
	elm_genlist_clear(ad->gl_network);
	int i;
	int items = (sizeof(network_table) / sizeof(network_table[0]));
	for (i = 0; i < items; i++) {
		one_item =
		    (struct gl_network_data *)calloc(1,
		                                     sizeof(struct
		                                            gl_network_data));
		setting_retvm_if(!one_item, FALSE, "calloc failed");
		one_item->index = i;
		/* one_item->name = __get_preferred_network_list_item_name(network_table[i]); */
		__get_preferred_network_list_item_name(one_item->name,
		                                       network_table[i]);

		if ('\0' == ad->search_text[0]
		    || ('\0' == one_item->name[0] && is_substr_ncase(one_item->name, ad->search_text))) {	/*  is substring */
			/* SETTING_TRACE("Adding item[%s]", one_item->name); */
			/* SETTING_TRACE("ad->gl_network:%p", ad->gl_network); */
			elm_genlist_item_append(ad->gl_network, &ad->itc,
			                        one_item, NULL,
			                        ELM_GENLIST_ITEM_NONE,
			                        gl_network_genlist_select,
			                        ad);
		} else {
			FREE(one_item);
		}
	}
	ad->add_view_timer = NULL;
	return FALSE;
}

/* ***search bar****/
static void __searchbar_changed_cb(void *data, Evas_Object *obj,
                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data")

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (elm_object_focus_get(ad->nw_search_bar)) {
		if (elm_entry_is_empty(obj))
			elm_object_signal_emit(ad->nw_search_bar, "elm,state,eraser,hide", "elm");
		else
			elm_object_signal_emit(ad->nw_search_bar, "elm,state,eraser,show", "elm");
	}

	const char *str = elm_object_text_get(obj);
	/* SETTING_TRACE("the string in searchbar:%s, len(str):%d", str, strlen(str)); */
	/* SETTING_TRACE("ad->search_text: %s", ad->search_text); */

	if (str == NULL) {
		SETTING_TRACE_ERROR("Searchbar text is NULL!");
		return;
	}
	/* if(!safeStrCmp(str, dgettext("sys_string", "IDS_ST_BODY_SEARCH")))  return; */
	if (!safeStrCmp(str, ad->search_text)) {
		return;
	}
	/* Evas_Object *entry = elm_searchbar_entry_get(ad->nw_search_bar); */

	if (strlen(str) > MAX_SEARCH_STR_LEN) {
		/* the content of entry won't change if enter too many characters */
		elm_object_text_set(obj, ad->search_text);
		/* elm_entry_cursor_end_set(entry); */
		/*  if input too long, show popup */
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Input too long!"), NULL, POPUP_INTERVAL, FALSE, FALSE);

		return;
	}

	memset(ad->search_text, '\0', MAX_SEARCH_STR_LEN + 1);
	safeCopyStr(ad->search_text, str, MAX_SEARCH_STR_LEN);

	/* SETTING_TRACE("ad->search_text:%s", ad->search_text); */
	/* if(safeStrCmp(ad->search_text, dgettext("sys_string", "IDS_ST_BODY_SEARCH"))) */
	/* { */
	SETTING_TRACE("Update the search view text, search_text=%s",
	              ad->search_text);

	if (ad->add_view_timer) {
		/* ecore_idler_del(ad->add_view_timer); */
		/* ad->add_view_timer = NULL; */
	}

	ad->add_view_timer = ecore_idler_add(__genlist_update, ad);
	/* } */

}

static void __searchbar_cancel_clicked_cb(void *data, Evas_Object *obj,
                                          void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	/* memset(ad->search_text, '\0', MAX_SEARCH_STR_LEN + 1); */
	/* safeCopyStr(ad->search_text, dgettext("sys_string", "IDS_ST_BODY_SEARCH"), MAX_SEARCH_STR_LEN); */
	/* elm_searchbar_text_set(ad->nw_search_bar, dgettext("sys_string", "IDS_ST_BODY_SEARCH")); */

	Evas_Object *entry = elm_object_part_content_get(ad->nw_search_bar, "elm.swallow.content");
	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(entry);
	if (imf_context) {
		ecore_imf_context_input_panel_hide(imf_context);
	}
	/* ad->add_view_timer = ecore_idler_add(__genlist_update, ad); */

}

static void __searchbar_focused_cb(void *data, Evas_Object *obj,
                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (!elm_entry_is_empty(obj))
		elm_object_signal_emit(ad->nw_search_bar, "elm,state,eraser,show", "elm");
	elm_object_signal_emit(ad->nw_search_bar, "elm,state,guidetext,hide", "elm");
	elm_object_signal_emit(ad->nw_search_bar, "cancel,in", "");
}

static Evas_Object *__add_searchbar_add(Evas_Object *parent, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, NULL, "NULL == data");

	Evas_Object *searchbar_layout = elm_layout_add(parent);
	elm_layout_theme_set(searchbar_layout, "layout", "searchbar", "cancel_button");

	Evas_Object *entry = elm_entry_add(searchbar_layout);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NORMAL);
	elm_object_part_content_set(searchbar_layout, "elm.swallow.content", entry);
	elm_object_part_text_set(searchbar_layout, "elm.guidetext", dgettext("sys_string", "IDS_ST_BODY_SEARCH"));

	evas_object_size_hint_weight_set(searchbar_layout, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(searchbar_layout, EVAS_HINT_FILL, 0.0);
	evas_object_size_hint_min_set(searchbar_layout, ELM_SCALE_SIZE(480), ELM_SCALE_SIZE(66));

	evas_object_smart_callback_add(entry, "changed", __searchbar_changed_cb, data);
	evas_object_smart_callback_add(entry, "focused", __searchbar_focused_cb, data);

	Evas_Object *cancel_btn = elm_button_add(searchbar_layout);
	elm_object_part_content_set(searchbar_layout, "button_cancel", cancel_btn);
	elm_object_style_set(cancel_btn, "searchbar/default");
	/*elm_object_text_set(cancel_btn, "Cancel"); */

	evas_object_smart_callback_add(cancel_btn, "clicked",
	                               __searchbar_cancel_clicked_cb, data);

	edje_object_part_swallow(_EDJ(parent), "search_bar", searchbar_layout);

	return searchbar_layout;
}



static int setting_network_preferred_network_list_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *sub_layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME,
	                    "search_layout");
	evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_show(sub_layout);

	/* add basic layout */
	setting_push_layout_navi_bar(_(STR_SETTING_NETWORK_LIST),
	                             dgettext("sys_string", "IDS_ST_BUTTON_CANCEL_ABB"),
	                             dgettext("sys_string", "IDS_COM_ADD"),
	                             NULL,
	                             setting_network_preferred_network_list_click_softkey_cancel,
	                             setting_network_preferred_network_list_click_softkey_add,
	                             NULL, ad, sub_layout, ad->navi_bar, NULL);

	Evas_Object *gl_network = NULL;
	gl_network_genlist_label_initialize(ad);

	ad->cur_sel_index = -1;
	ad->itc.item_style = "1text.1icon.2";
	ad->itc.func.text_get = gl_network_genlist_text_get;
	ad->itc.func.content_get = gl_network_getlist_content_get;
	ad->itc.func.state_get = NULL;
	ad->itc.func.del = gl_network_genlist_delete;

	ad->gl_network = gl_network = elm_genlist_add(ad->win_main_layout);
	retvm_if(ad->gl_network == NULL, SETTING_RETURN_FAIL,
	         "ad->gl_network == NULL");
	//elm_genlist_realization_mode_set(ad->gl_network, EINA_TRUE);
	elm_object_style_set(ad->gl_network, "dialogue");
	elm_genlist_clear(gl_network);
	evas_object_show(gl_network);
	evas_object_smart_callback_add(ad->gl_network, "realized", __gl_realized_cb, NULL);

	__gl_network_radio_group = elm_radio_add(ad->win_main_layout);
	elm_radio_state_value_set(__gl_network_radio_group, -1);

	ad->nw_search_bar = __add_searchbar_add(sub_layout, ad);
	edje_object_part_swallow(_EDJ(sub_layout), "search_list",
	                         ad->gl_network);
	__genlist_update(ad);

	elm_radio_value_set(__gl_network_radio_group, -100);
	setting_view_network_preferred_network_list.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_list_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (__gl_network_radio_group != NULL) {
		evas_object_del(__gl_network_radio_group);
	}
	if (ad->gl_network != NULL) {
		elm_genlist_clear(ad->gl_network);
		evas_object_del(ad->gl_network);
		ad->gl_network = NULL;
	}
	if (ad->add_view_timer) {
		ecore_idler_del(ad->add_view_timer);
		ad->add_view_timer = NULL;
	}
	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_preferred_network_list.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_list_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/* SettingNetworkUG *ad = (SettingNetworkUG*)cb; */

	/*  change background style */
	/* if (ad->bg) */
	/*         elm_object_style_set(ad->bg, "list"); */

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_list_cleanup(void *cb)
{
	return setting_network_preferred_network_list_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void
setting_network_preferred_network_list_click_softkey_cancel(void *data,
                                                            Evas_Object *obj,
                                                            void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred list] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_preferred_network_list,
	                    &setting_view_network_preferred_network, ad);
}

static void
setting_network_preferred_network_list_click_softkey_add(void *data,
                                                         Evas_Object *obj,
                                                         void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred list] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	int sel = 0;
	int tapi_ret = TAPI_API_SUCCESS;
	TelNetworkPreferredPlmnInfo_t pref_plmn_info = { 0, };

	/* sel = ad->cur_sel_index; */
	sel = elm_radio_value_get(__gl_network_radio_group);
	SETTING_TRACE
	("setting_network_preferred_network_list_click_softkey_add: sel = %d",
	 sel);

	if (sel < 0) {
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Please select item(s) to process!"), NULL, POPUP_INTERVAL, FALSE, FALSE);

		return;
	}

	/* sprintf((char *)pref_plmn_info.Plmn, "%d", network_table[sel].mcc); */
	/* sprintf((char *)pref_plmn_info.Plmn+3, "%d", network_table[sel].mnc); */
	int ret = 0;
	ret = snprintf((char *)pref_plmn_info.Plmn, MAX_PLMN_LEN / 2, "%d",
	               network_table[sel].mcc);
	ret_if(ret < 0);

	ret = snprintf((char *)pref_plmn_info.Plmn + (MAX_PLMN_LEN / 2),
	               MAX_PLMN_LEN / 2, "%d", network_table[sel].mnc);
	ret_if(ret < 0);

	SETTING_TRACE("selected item is:%s", network_table[sel].short_name_ptr);

	switch (network_table[sel].nw_type) {
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_900:
		case SETTING_PREFERRED_NETWORK_TYPE_DCS_1800:
		case SETTING_PREFERRED_NETWORK_TYPE_PCS_1900:
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_SAT:
		case SETTING_PREFERRED_NETWORK_TYPE_GSM_850:
			pref_plmn_info.SystemType = TAPI_NETWORK_SYSTEM_GSM;
			break;

		case SETTING_PREFERRED_NETWORK_TYPE_UMTS:
			pref_plmn_info.SystemType = TAPI_NETWORK_SYSTEM_UMTS;
			break;
		default:
			break;
	}

	pref_plmn_info.Index =
	    ad->pref_list.PrefPlmnRecord[ad->pref_list.NumOfPrefPlmns -
	                                 1].Index + 1;

	ad->network_ug_pop =
	    setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_ST_POP_SAVING"),
	                                     NULL, 0.0, TRUE, TRUE);
	SETTING_TRACE_DEBUG("%s*** plmn=%s, index=%d ***%s",
	                    SETTING_FONT_BGREEN, pref_plmn_info.Plmn,
	                    (int)(pref_plmn_info.Index), SETTING_FONT_BLACK);

	tapi_ret =
	    tel_set_network_preferred_plmn(ad->handle, TAPI_NETWORK_PREF_PLMN_ADD,
	                                   &pref_plmn_info, setting_tapi_set_preferred_plmn_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_preferred_plmn(PLMN_ADD) err=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Save Failed!"), NULL, POPUP_INTERVAL, FALSE, FALSE);
	}

	ad->op_type = SETTING_NETWORK_PREFERRED_NETWORK_LIST;
}
