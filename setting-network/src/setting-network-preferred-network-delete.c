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
#include <setting-network-preferred-network-delete.h>
#include <efl_extension.h>

static int setting_network_preferred_network_delete_create(void *cb);
static int setting_network_preferred_network_delete_destroy(void *cb);
static int setting_network_preferred_network_delete_update(void *cb);
static int setting_network_preferred_network_delete_cleanup(void *cb);

typedef struct {
	bool is_checked;
	Evas_Object *eo_check;
	int prefer_idx;
	Setting_GenGroupItem_Data *data_Genitem;
} preferred_list_item_t;

static int check_count = 0;

setting_view setting_view_network_preferred_network_delete = {
	.create = setting_network_preferred_network_delete_create,
	.destroy = setting_network_preferred_network_delete_destroy,
	.update = setting_network_preferred_network_delete_update,
	.cleanup = setting_network_preferred_network_delete_cleanup,
};

static void setting_network_preferred_network_delete_select_all_cb(void *data,
                                                                   Evas_Object *
                                                                   obj, void
                                                                   *event_info);
static void setting_network_preferred_network_delete_check_cb(void *data,
                                                              Evas_Object *obj,
                                                              void *event_info);
static void setting_network_perferred_network_delete_mouse_up_cb(void *data,
                                                                 Evas_Object *
                                                                 obj, void
                                                                 *event_info);

static void
setting_network_preferred_network_delete_click_softkey_cancel_cb(void *data,
                                                                 Evas_Object *
                                                                 obj, void
                                                                 *event_info);
static void
setting_network_preferred_network_delete_click_softkey_delete_cb(void *data,
                                                                 Evas_Object *
                                                                 obj, void
                                                                 *event_info);
static void setting_network_preferred_network_delete_no_cb(void *cb);
static void setting_network_preferred_network_delete_yes_cb(void *cb);

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_network_preferred_network_delete_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME, "selinfo_bottom");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	ad->sub_clear_layout = layout;

	setting_push_layout_navi_bar(_("IDS_ST_BODY_DELETE"),
	                             dgettext("sys_string", "IDS_ST_BUTTON_CANCEL_ABB"),
	                             dgettext("sys_string", "IDS_ST_BODY_DELETE"),
	                             NULL,
	                             setting_network_preferred_network_delete_click_softkey_cancel_cb,
	                             setting_network_preferred_network_delete_click_softkey_delete_cb,
	                             NULL, ad, layout, ad->navi_bar, NULL);
	/* elm_object_item_disabled_set(elm_toolbar_last_item_get(ad->controllbar_clear), EINA_TRUE); */

	Evas_Object *sub_layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME,
	                    "search_layout");
	evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_show(sub_layout);

	ad->gl_sel_all = elm_genlist_add(ad->win_main_layout);
	retvm_if(ad->gl_sel_all == NULL, SETTING_RETURN_FAIL,
	         "gl_sel_all == NULL");
	//elm_genlist_realization_mode_set(ad->gl_sel_all, EINA_TRUE);
	elm_object_style_set(ad->gl_sel_all, "dialogue");
	elm_genlist_clear(ad->gl_sel_all);	/* first to clear list */
	elm_scroller_bounce_set(ad->gl_sel_all, 0, 0);
	evas_object_smart_callback_add(ad->gl_sel_all, "realized", __gl_realized_cb, NULL);

	ad->scl_pref_del = elm_genlist_add(ad->win_main_layout);
	retvm_if(ad->scl_pref_del == NULL, SETTING_RETURN_FAIL,
	         "ad->scl_pref_del == NULL");
	//elm_genlist_realization_mode_set(ad->scl_pref_del, EINA_TRUE);
	elm_object_style_set(ad->scl_pref_del, "dialogue");
	elm_genlist_clear(ad->scl_pref_del);	/* first to clear list */
	evas_object_smart_callback_add(ad->scl_pref_del, "realized", __gl_realized_cb, NULL);

	elm_object_part_content_set(sub_layout, "search_list", ad->scl_pref_del);
	elm_object_part_content_set(sub_layout, "search_bar", ad->gl_sel_all);
	elm_object_part_content_set(layout, "elm.swallow.contents", sub_layout);

	setting_network_preferred_network_delete_draw_network_list(ad);
	setting_view_network_preferred_network_delete.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_delete_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_preferred_network_delete.is_create = 0;

	/* free data */
	preferred_list_item_t *list_item;
	if (ad->item_check_list != NULL) {
		Eina_List *l = NULL;
		EINA_LIST_FOREACH(ad->item_check_list, l, list_item) {
			FREE(list_item);
		}
		eina_list_free(ad->item_check_list);
		ad->item_check_list = NULL;
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_delete_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	setting_network_preferred_network_delete_refresh_view(ad);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_delete_cleanup(void *cb)
{
	return setting_network_preferred_network_delete_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void
setting_network_preferred_network_delete_refresh_view(SettingNetworkUG *ad)
{
	SETTING_TRACE_BEGIN;
	/* int i =0; */

	if (ad->item_check_list) {
		eina_list_free(ad->item_check_list);
		ad->item_check_list = NULL;
	}
	/* create list */
	setting_network_preferred_network_delete_draw_network_list(ad);

}

void
setting_network_preferred_network_delete_draw_network_list(SettingNetworkUG *
                                                           ad)
{
	SETTING_TRACE_BEGIN;
	int i = 0;
	preferred_list_item_t *list_item = NULL;

	/* free data */
	if (ad->item_check_list != NULL) {
		Eina_List *l = NULL;
		EINA_LIST_FOREACH(ad->item_check_list, l, list_item) {
			FREE(list_item);
		}
		eina_list_free(ad->item_check_list);
		ad->item_check_list = NULL;
	}

	elm_genlist_clear(ad->scl_pref_del);	/* first to clear list */

	check_count = 0;

	ad->data_cl_select_all =
	    setting_create_Gendial_field_def(ad->gl_sel_all, &itc_sel_all,
	                                     setting_network_perferred_network_delete_mouse_up_cb,
	                                     ad, SWALLOW_Type_1CHECK, NULL,
	                                     NULL, 0, KeyStr_Select_All, NULL,
	                                     setting_network_preferred_network_delete_select_all_cb);
	if (ad->data_cl_select_all) {
		ad->data_cl_select_all->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_cl_select_all is NULL");
	}

	for (i = 0; i < ad->pref_list.NumOfPrefPlmns; i++) {
		list_item = calloc(1, sizeof(preferred_list_item_t));
		setting_retm_if(!list_item, "calloc failed");
		list_item->prefer_idx = i;
		list_item->is_checked = 0;

		list_item->data_Genitem =
		    setting_create_Gendial_field_def(ad->scl_pref_del,
		                                     &itc_cm_1text_1icon_2,
		                                     setting_network_perferred_network_delete_mouse_up_cb,
		                                     ad, SWALLOW_Type_1CHECK,
		                                     NULL, NULL, 0,
		                                     _(ad->pref_list.PrefPlmnRecord[i].network_name), NULL,
		                                     setting_network_preferred_network_delete_check_cb);
		if (list_item->data_Genitem) {
			list_item->data_Genitem->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("ad->data_Genitem is NULL");
		}

		ad->item_check_list =
		    eina_list_append(ad->item_check_list, list_item);
	}
	return;
}

static void
setting_network_preferred_network_delete_select_all_cb(void *data,
                                                       Evas_Object *obj,
                                                       void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingNetworkUG *ad = list_item->userdata;
	setting_create_sel_info_popup(ad->sub_clear_layout,
	                              &(ad->selInfoPop_layout),
	                              &(ad->selInfoPop));
	int state = list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	Eina_List *check_list = ad->item_check_list;
	preferred_list_item_t *pre_list_item = NULL;

	if (state) {
		while (check_list) {
			pre_list_item = (preferred_list_item_t *)
			                eina_list_data_get(check_list);
			if (NULL == pre_list_item)
				continue;
			setting_update_gl_item_chk_status(pre_list_item->data_Genitem, 1);
			check_list = eina_list_next(check_list);
		}
		check_count = ad->pref_list.NumOfPrefPlmns;

		char text[MAX_DISPLAY_NAME_LEN_ON_UI];
		snprintf(text, sizeof(text), "%d Items Selected", check_count);
		edje_object_part_text_set(_EDJ(ad->selInfoPop_layout),
		                          "elm.text", text);
	} else {
		while (check_list) {
			pre_list_item = (preferred_list_item_t *)
			                eina_list_data_get(check_list);
			if (NULL == pre_list_item)
				continue;
			setting_update_gl_item_chk_status(pre_list_item->data_Genitem, 0);

			check_list = eina_list_next(check_list);
		}
		check_count = 0;
		evas_object_hide(ad->selInfoPop);
	}

	SETTING_TRACE("check_count:%d", check_count);
	return;
}

static void
setting_network_preferred_network_delete_check_cb(void *data, Evas_Object *obj,
                                                  void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingNetworkUG *ad = list_item->userdata;
	setting_create_sel_info_popup(ad->sub_clear_layout,
	                              &(ad->selInfoPop_layout),
	                              &(ad->selInfoPop));

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	if (list_item->chk_status) {
		check_count++;
	} else {
		check_count--;
	}

	if (check_count < ad->pref_list.NumOfPrefPlmns) {
		setting_update_gl_item_chk_status(ad->data_cl_select_all, 0);
	} else {
		setting_update_gl_item_chk_status(ad->data_cl_select_all, 1);
	}

	SETTING_TRACE("check count is [%d]", check_count);
	if (!check_count) {
		evas_object_hide(ad->selInfoPop);
		return;
	}

	char text[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
	snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, "%d %s", check_count, KeyStr_Item_Sel);


	/*  text set and timeout set */
	edje_object_part_text_set(_EDJ(ad->selInfoPop_layout), "elm.text",
	                          text);

}

static void
setting_network_perferred_network_delete_mouse_up_cb(void *data,
                                                     Evas_Object *obj,
                                                     void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
	SETTING_TRACE("list_item->keyStr:%s", list_item->keyStr);

	SettingNetworkUG *ad = list_item->userdata;
	setting_create_sel_info_popup(ad->sub_clear_layout,
	                              &(ad->selInfoPop_layout),
	                              &(ad->selInfoPop));
	int state = list_item->chk_status;	/*  for genlist update status */
	Eina_List *check_list = ad->item_check_list;
	preferred_list_item_t *pre_list_item = NULL;

	if (0 == safeStrCmp(KeyStr_Select_All, list_item->keyStr)) {	/* if(obj == chk_sel_all) */
		SETTING_TRACE("Select All");
		if (state) {
			while (check_list) {
				pre_list_item = (preferred_list_item_t *)
				                eina_list_data_get(check_list);
				if (NULL == pre_list_item)
					continue;
				setting_update_gl_item_chk_status(pre_list_item->data_Genitem, 1);
				check_list = eina_list_next(check_list);
			}
			check_count = ad->pref_list.NumOfPrefPlmns;

			char text[MAX_DISPLAY_NAME_LEN_ON_UI];
			snprintf(text, sizeof(text), "%d Items Selected",
			         check_count);
			edje_object_part_text_set(_EDJ(ad->selInfoPop_layout),
			                          "elm.text", text);
		} else {
			while (check_list) {
				pre_list_item = (preferred_list_item_t *)
				                eina_list_data_get(check_list);
				if (NULL == pre_list_item)
					continue;
				setting_update_gl_item_chk_status(pre_list_item->data_Genitem, 0);
				check_list = eina_list_next(check_list);
			}
			check_count = 0;
			evas_object_hide(ad->selInfoPop);
		}
	} else {
		SETTING_TRACE("Select %s", list_item->keyStr);
		if (state) {
			check_count++;
		} else {
			check_count--;
		}

		if (check_count < ad->pref_list.NumOfPrefPlmns) {
			setting_update_gl_item_chk_status(ad->data_cl_select_all, 0);
		} else {
			setting_update_gl_item_chk_status(ad->data_cl_select_all, 1);
		}

		if (!check_count) {
			evas_object_hide(ad->selInfoPop);
			return;
		}

		char text[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
		snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, "%d %s", check_count, KeyStr_Item_Sel);

		/*  text set and timeout set */
		edje_object_part_text_set(_EDJ(ad->selInfoPop_layout),
		                          "elm.text", text);
	}
	SETTING_TRACE("check_count:%d", check_count);
}

static bool
setting_network_preferred_network_delete_one(SettingNetworkUG *ad,
                                             preferred_list_item_t *list_item)
{
	retvm_if(list_item == NULL, FALSE, "list_item parameter is NULL");
	bool ret = TRUE;
	int tapi_ret = TAPI_API_SUCCESS;
	TelNetworkPreferredPlmnInfo_t preff_plmn_info = { 0, };
	memcpy(&preff_plmn_info,
	       &ad->pref_list.PrefPlmnRecord[list_item->prefer_idx],
	       sizeof(TelNetworkPreferredPlmnInfo_t));
	SETTING_TRACE_WARNING
	("preff_plmn_info.network_name = %s, preff_plmn_info.Index = %d",
	 preff_plmn_info.network_name, preff_plmn_info.Index);
	tapi_ret =
	    tel_set_network_preferred_plmn(ad->handle, TAPI_NETWORK_PREF_PLMN_DELETE,
	                                   &preff_plmn_info, setting_tapi_set_preferred_plmn_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_preferred_plmn(PLMN_DEL) err=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Delete Failed"), NULL, POPUP_INTERVAL, FALSE, FALSE);
		ret = FALSE;
	}
	return ret;
}

void setting_network_preferred_network_delete_ok(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	bool ret = TRUE;
	preferred_list_item_t *list_item = NULL;
	int total_count = eina_list_count(ad->checked_data_list);
	ad->deleted_number++;
	if (ad->deleted_number >= total_count) {
		if (ad->network_ug_pop != NULL) {
			evas_object_del(ad->network_ug_pop);
			ad->network_ug_pop = NULL;
		}
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_IM_BODY_SUCCESSFULLY_COMPLETED"), NULL, POPUP_INTERVAL, FALSE, FALSE);

		int tapi_ret = TAPI_API_SUCCESS;

		tapi_ret = tel_get_network_preferred_plmn(ad->handle, setting_tapi_get_preferred_plmn_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG
			("%s*** [ERR] tel_get_network_preferred_plmn. err=%d ***%s",
			 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		}
	} else {
		/* update info */
		char info[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
		snprintf(info, sizeof(info), "%s (%d/%d)",
		         _("IDS_ST_BODY_DELETE"), ad->deleted_number,
		         total_count);
		elm_object_text_set(ad->popup_label, info);
		/* delete next */
		list_item =
		    eina_list_nth(ad->checked_data_list, ad->deleted_number);
		ret =
		    setting_network_preferred_network_delete_one(ad, list_item);
		if (!ret) {
			setting_network_preferred_network_delete_failed(ad);
		}
	}
}

void setting_network_preferred_network_delete_failed(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->network_ug_pop != NULL) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}
	setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Failed!"), NULL, POPUP_INTERVAL, FALSE, FALSE);


	int tapi_ret = TAPI_API_SUCCESS;

	tapi_ret = tel_get_network_preferred_plmn(ad->handle, setting_tapi_get_preferred_plmn_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_get_network_preferred_plmn. err=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
	}
}

static void setting_network_preferred_network_create_delete_pop(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->network_ug_pop != NULL) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}
	Evas_Object *popup;
	popup = elm_popup_add(ad->win_get);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb_2, NULL);
	setting_add_hardkey_features(popup, ad);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	Evas_Object *box, *progressbar;
	ad->popup_label = elm_label_add(popup);
	char info[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
	snprintf(info, sizeof(info), "%s (%d/%d)", _("IDS_ST_BODY_DELETE"),
	         ad->deleted_number, eina_list_count(ad->checked_data_list));
	elm_object_text_set(ad->popup_label, info);
	evas_object_show(ad->popup_label);
	box = elm_box_add(popup);
	progressbar = elm_progressbar_add(popup);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	elm_box_pack_end(box, ad->popup_label);
	elm_box_pack_end(box, progressbar);
	evas_object_show(box);
	elm_object_content_set(popup, box);
	evas_object_show(popup);
	ad->network_ug_pop = popup;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void setting_network_preferred_network_delete_yes_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(cb == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	bool ret = TRUE;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	ad->deleted_number = 0;
	eina_list_free(ad->checked_data_list);
	ad->checked_data_list = NULL;
	Eina_List *check_list = ad->item_check_list;
	preferred_list_item_t *list_item = NULL;
	bool isChecked = FALSE;
	while (check_list) {
		list_item =
		    (preferred_list_item_t *) eina_list_data_get(check_list);
		if (NULL == list_item)
			continue;
		isChecked = list_item->data_Genitem->chk_status;

		if (isChecked) {
			ad->checked_data_list =
			    eina_list_prepend(ad->checked_data_list, list_item);
		}
		check_list = eina_list_next(check_list);
	}
	setting_network_preferred_network_create_delete_pop(ad);
	list_item = eina_list_nth(ad->checked_data_list, ad->deleted_number);
	ret = setting_network_preferred_network_delete_one(ad, list_item);
	if (!ret) {
		setting_network_preferred_network_delete_failed(ad);
	}
	ad->op_type = SETTING_NETWORK_PREFERRED_NETWORK_DELETE;
}

static void setting_network_preferred_network_delete_no_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(cb == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

static void setting_preferred_network_delete_resp_cb(void *data,
                                                     Evas_Object *obj,
                                                     void *event_info)
{
	SETTING_TRACE_BEGIN;
	int response_type = btn_type(obj);
	if (POPUP_RESPONSE_OK == response_type) {
		setting_network_preferred_network_delete_yes_cb(data);
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		setting_network_preferred_network_delete_no_cb(data);
	}
}

static void
setting_network_preferred_network_delete_click_softkey_delete_cb(void *data,
                                                                 Evas_Object *
                                                                 obj, void
                                                                 *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (ad->pref_list.NumOfPrefPlmns == 1) {
		setting_create_simple_popup(ad, ad->win_get,
		                            NULL, _("Do not Delete!"));
	} else {
		int sel_count = 0;
		Eina_List *check_list = ad->item_check_list;
		preferred_list_item_t *list_item = NULL;
		while (check_list) {
			list_item = (preferred_list_item_t *)
			            eina_list_data_get(check_list);
			if (NULL == list_item)
				continue;
			SETTING_TRACE("item[%d]'s check flag is %d",
			              list_item->prefer_idx,
			              list_item->data_Genitem->chk_status);
			if (list_item->data_Genitem->chk_status) {
				sel_count++;
			}
			check_list = eina_list_next(check_list);
		}

		if (0 == sel_count) {
			setting_create_simple_popup(ad, ad->win_get,
			                            NULL, _("Please select item(s) to process!"));
			return;
		}

		char notify_str[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
		if (1 == sel_count) {
			snprintf(notify_str, sizeof(notify_str),
			         "%s 1 network?", _("IDS_ST_BODY_DELETE"));
		} else {
			snprintf(notify_str, sizeof(notify_str),
			         "%s %d networks?", _("IDS_ST_BODY_DELETE"),
			         sel_count);
		}
		ad->popup = setting_create_popup_with_btn(ad, ad->win_get, NULL, notify_str,
		                                          setting_preferred_network_delete_resp_cb,
		                                          0, 2, _("IDS_ST_BUTTON_OK"), _("IDS_ST_BUTTON_CANCEL_ABB"));
	}

	if (ad->selInfoPop) {
		evas_object_del(ad->selInfoPop);
		ad->selInfoPop = NULL;
	}
}

static void
setting_network_preferred_network_delete_click_softkey_cancel_cb(void *data,
                                                                 Evas_Object *
                                                                 obj, void
                                                                 *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->selInfoPop) {
		evas_object_del(ad->selInfoPop);
		ad->selInfoPop = NULL;
	}

	ad->op_type = SETTING_NETWORK_PREFERRED_NETWORK_NONE;
	setting_view_change(&setting_view_network_preferred_network_delete,
	                    &setting_view_network_preferred_network, ad);
}
