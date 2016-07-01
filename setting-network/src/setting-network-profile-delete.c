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
#include <setting-network-profile-delete.h>

static int setting_network_profile_delete_create(void *cb);
static int setting_network_profile_delete_destroy(void *cb);
#if 0
static int setting_network_profile_delete_update(void *cb);
#endif
static int setting_network_profile_delete_cleanup(void *cb);
static void setting_network_profile_delete_click_softkey_cancel_ctx_cb(
		void *data, Evas_Object *obj, void *event_info);

setting_view setting_view_network_profile_delete = {
	.create = setting_network_profile_delete_create,
	.destroy = setting_network_profile_delete_destroy,
	/*.update = setting_network_profile_delete_update, */
	.update = NULL,
	.cleanup = setting_network_profile_delete_cleanup, };

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void __profile_delete_list_draw(SettingNetworkUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (ad->profile_del_list != NULL) {
		eina_list_free(ad->profile_del_list);
		ad->profile_del_list = NULL;
	}
	elm_genlist_clear(ad->gl_profile_del); /* first to clear list */

	/* item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 * ELM_GENLIST_ITEM_GROUP, NULL, NULL); */
	/*m_Object_Item *first_item;
	 first_item = elm_genlist_item_append(ad->gl_profile_del,
	 &itc_seperator, NULL, NULL,
	 ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(first_item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	Eina_List *elist = NULL;
	connection_profile_h profile_h = NULL;
	int idx = 0;
	char *id = NULL;
	char *apn = NULL;
	char *name = NULL;
	char *proxy_address = NULL;
	connection_profile_type_e profile_type =
			CONNECTION_PROFILE_TYPE_CELLULAR;
	Setting_GenGroupItem_Data *item_data = NULL;

	setting_network_reget_profile_list(ad);

	/* CREATE GENLIST 'Select All' */

	/* CREATE GENLIST */
	SETTING_TRACE("ad->profile_list:%p", ad->profile_list);
	EINA_LIST_FOREACH(ad->profile_list, elist, profile_h)
	{
		SETTING_TRACE("profile_h:%p", profile_h);
		if (!profile_h)
			continue;
		apn = name = proxy_address = id = NULL;
		connection_profile_get_cellular_apn(profile_h, &apn);
		if (isEmptyStr(apn))
			continue;
		connection_profile_get_name(profile_h, &name);
		if (isEmptyStr(name))
			continue;
		connection_profile_get_proxy_address(profile_h,
				CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
		connection_profile_get_id(profile_h, &id);

		if (connection_profile_get_type(profile_h, &profile_type)
				!= CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile type");
			continue;
		}

		SETTING_TRACE("Record[%s], profile_type:%d, service_type:%d, id:%s",
				apn, profile_type, ad->profile_service_type,
				id);

		item_data = (Setting_GenGroupItem_Data *)calloc(1,
				sizeof(Setting_GenGroupItem_Data));
		if (0 == item_data) {
			SETTING_TRACE_ERROR("calloc failed");
			G_FREE(apn);
			return;
		}
		item_data->keyStr = (char *)g_strdup(name);
		item_data->swallow_type = SWALLOW_Type_1CHECK_RIGHT;
		item_data->r_swallow_path = (char *)g_strdup("reveal/extended");
		item_data->chk_change_cb =
				setting_network_profile_delete_check_cb;
		item_data->userdata = ad;
		item_data->sub_desc = (char *)g_strdup(apn);
		item_data->belongs_to = (int)profile_h;

		item_data->item = elm_genlist_item_append(ad->gl_profile_del,
				&(itc_2text_1icon_2), item_data, NULL,
				ELM_GENLIST_ITEM_NONE,
				setting_network_profile_delete_mouse_up_cb, ad);
		ad->profile_del_list = eina_list_append(ad->profile_del_list,
				item_data);

		idx++;
	}

	Evas_Object *toolbar = elm_object_item_part_content_get(
			ad->navi_it_profile_del_list, "toolbar");
	Evas_Object *allbtn = elm_object_item_part_content_get(
			ad->navi_it_profile_del_list, "title_left_btn");
	SETTING_TRACE("idx:%d", idx);
	if (idx == 1) {
		if (allbtn)
			elm_object_disabled_set(allbtn, EINA_FALSE);
	} else if (idx > 1) {
		if (allbtn)
			elm_object_disabled_set(allbtn, EINA_FALSE);
	} else {
		if (ad->profile_list != NULL) {
			eina_list_free(ad->profile_list);
			ad->profile_list = NULL;
		}
		SETTING_TRACE("To disable");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar),
				EINA_TRUE);
		if (allbtn)
			elm_object_disabled_set(allbtn, EINA_TRUE);
		SETTING_TRACE("To disable");
	}
	G_FREE(apn);
	G_FREE(name);
	G_FREE(proxy_address);
	G_FREE(id);
	return;
}

/*/////////////////////////////////////////////////////////////////////////// */
static Evas_Object *ctxpopup;

static void ctxpopup_dismissed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	evas_object_del(ctxpopup);
	ctxpopup = NULL;
}

static void move_more_ctxpopup(Evas_Object *ctxpopup)
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
		evas_object_move(ctxpopup, (h / 2), w);
		break;
	case 270:
		evas_object_move(ctxpopup, (h / 2), w);
		break;
	}
}

static void naviframe_resize_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *ctxpopup = data;
	move_more_ctxpopup(ctxpopup);
}

static void more_ctxpopup_del_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *nf = data;
	evas_object_event_callback_del_full(nf, EVAS_CALLBACK_RESIZE,
			naviframe_resize_cb, ctxpopup);
}

static void win_rotation_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *ctxpopup = data;
	move_more_ctxpopup(ctxpopup);
}

/* Icon + Text (More button style : Naviframe Toolbar) */
static void create_ctxpopup_more_button_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	/*Evas_Object *it_obj; */
	Evas_Object *nf = ad->navi_bar;
	Evas_Object *win;
	/*Elm_Object_Item *it; */

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
	}

	ctxpopup = elm_ctxpopup_add(nf);
	elm_ctxpopup_auto_hide_disabled_set(ctxpopup, EINA_TRUE);
	elm_object_style_set(ctxpopup, "more/default");
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_BACK,
			eext_ctxpopup_back_cb, NULL);
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_MORE,
			eext_ctxpopup_back_cb, NULL);
	evas_object_smart_callback_add(ctxpopup, "dismissed",
			ctxpopup_dismissed_cb, NULL);
	evas_object_event_callback_add(ctxpopup, EVAS_CALLBACK_DEL,
			more_ctxpopup_del_cb, nf);
	evas_object_event_callback_add(nf, EVAS_CALLBACK_RESIZE,
			naviframe_resize_cb, ctxpopup);

	/* We convince the top widget is a window */
	win = elm_object_top_widget_get(nf);
	evas_object_smart_callback_add(win, "rotation,changed",
			win_rotation_changed_cb, ctxpopup);

	/*------------------------------------------------------------------- */
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BUTTON_CANCEL_ABB"), NULL,
			setting_network_profile_delete_click_softkey_cancel_ctx_cb,
			ad);
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BODY_DELETE"), NULL,
			setting_network_profile_delete_click_softkey_delete_cb,
			ad);
	/*------------------------------------------------------------------- */

	elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	move_more_ctxpopup(ctxpopup);
	evas_object_show(ctxpopup);
}
/*/////////////////////////////////////////////////////////////////////////// */

static int setting_network_profile_delete_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *)cb;

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
			"Cannot set scroller object  as contento of layout");
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller); /* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			NULL);
	ad->gl_profile_del = scroller;

	ad->navi_it_profile_del_list = setting_push_layout_navi_bar(
			_(IDS_ST_BODY_DELETE),
			NULL, _(IDS_ST_BODY_DELETE),
			_(IDS_ST_BUTTON_CANCEL_ABB),
			setting_network_profile_delete_click_softkey_cancel_cb,
			setting_network_profile_delete_click_softkey_delete_cb,
			setting_network_profile_delete_click_softkey_cancel_cb,
			ad, scroller, ad->navi_bar, NULL);
	/*disable Delete button */
	Evas_Object *toolbar = elm_object_item_part_content_get(
			ad->navi_it_profile_del_list, "toolbar");
	Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
	elm_object_item_disabled_set(it, EINA_TRUE);

	Evas_Object *allbtn = elm_button_add(ad->navi_bar);
	evas_object_size_hint_weight_set(allbtn, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(allbtn, EVAS_HINT_FILL, 0.5);

	elm_object_text_set(allbtn, _("IDS_ST_OPT_ALL"));
	evas_object_smart_callback_add(allbtn, "clicked",
			setting_network_profile_delete_click_softkey_select_all_cb,
			ad);
	evas_object_show(allbtn);
	elm_object_item_part_content_set(ad->navi_it_profile_del_list,
			"title_right_btn", allbtn);

	if (ad->navi_it_profile_del_list) {
		elm_object_item_domain_text_translatable_set(
				ad->navi_it_profile_del_list, SETTING_PACKAGE,
				EINA_TRUE);
		elm_naviframe_item_pop_cb_set(ad->navi_it_profile_del_list,
				setting_network_profile_delete_click_softkey_cancel_cb,
				ad);
	}

	/* Add ctx popup handler */
	Evas_Object *morebtn = elm_button_add(ad->navi_bar);
	elm_object_style_set(morebtn, "naviframe/more/default");
	evas_object_smart_callback_add(morebtn, "clicked",
			create_ctxpopup_more_button_cb, ad);
	elm_object_item_part_content_set(ad->navi_it_profile_del_list,
			"toolbar_more_btn", morebtn);

#if 0
	/* ADD 'SELECT ALL' */
	ad->data_delete_all = setting_create_Gendial_field_1radio(
			ad->gl_profile_del, &(ad->itc_1icon_1text_sub),
			__screen_timeout_cb, ad,
			SWALLOW_Type_1RADIO_RIGHT,
			rdg, 4, KeyStr_Backlight_5_MIN_STR,
			NULL);
	if (ad->data_delete_all) {
		ad->data_delete_all->userdata = ad;
		__BACK_POINTER_SET(ad->data_delete_all);
	} else {
		SETTING_TRACE_ERROR("ad->data_delete_all is NULL");
	}
#endif

	__profile_delete_list_draw(ad);
	setting_view_network_profile_delete.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_profile_delete_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *)cb;
	if (!setting_view_network_profile_delete.is_create) {
		return SETTING_RETURN_SUCCESS;
	}

	/* free data */
	if (ad->profile_del_list != NULL) {
		eina_list_free(ad->profile_del_list);
		ad->profile_del_list = NULL;
	}
	ad->gl_profile_del = NULL;
	ad->navi_it_profile_del_list = NULL;
	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_profile_delete.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

#if 0
static int setting_network_profile_delete_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	__profile_delete_list_draw(cb);
	return SETTING_RETURN_SUCCESS;
}
#endif

static int setting_network_profile_delete_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	setting_network_profile_delete_destroy(cb);
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
void ___profile_selected(void *data)
{
	SettingNetworkUG *ad = data;
	retm_if(ad == NULL, "Invalid argument: ad is NULL");
	Eina_List *check_list = ad->profile_del_list;
	Setting_GenGroupItem_Data *item_data = NULL;
	int selnum = 0;
	while (check_list) {
		item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(
				check_list);
		if (NULL == item_data)
			continue;

		SETTING_TRACE("%s :%d", _(item_data->keyStr),
				item_data->chk_status);
		if (item_data->chk_status) {
			selnum++;
		}
		check_list = eina_list_next(check_list);
	}
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	if (navi_it) {
		Evas_Object *toolbar = elm_object_item_part_content_get(navi_it,
				"toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		if (selnum != 0) {
			char title[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = { 0, };
			snprintf(title, MAX_DISPLAY_NAME_LEN_ON_UI,
					_("IDS_ST_HEADER_PD_SELECTED"), selnum);
			elm_object_item_text_set(navi_it, _(title));
			elm_object_item_disabled_set(it, EINA_FALSE);
		} else {
			elm_object_item_text_set(navi_it,
					_("IDS_ST_BODY_DELETE"));
			elm_object_item_disabled_set(it, EINA_TRUE);
		}
	}
}

static void setting_network_profile_delete_check_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);
	___profile_selected(list_item->userdata);
}

static void setting_network_profile_delete_mouse_up_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	setting_retm_if(NULL == list_item, "list_item is NULL");
	SETTING_TRACE("list_item->keyStr:%s", list_item->keyStr);

	setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
	___profile_selected(list_item->userdata);
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
static Eina_Bool setting_network_profile_delete_click_softkey_cancel_cb(
		void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, FALSE, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	setting_view_change(&setting_view_network_profile_delete,
			&setting_view_network_con_list, ad);

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
	return EINA_TRUE;
}

static void setting_network_profile_delete_click_softkey_cancel_ctx_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	setting_network_profile_delete_click_softkey_cancel_cb(data, NULL);
}

static void __popup_deleted_response_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *)data;

	setting_view_change(&setting_view_network_profile_delete,
			&setting_view_network_con_list, ad);
}

static void setting_network_profile_delete_click_softkey_delete_cb(void *data,
		Evas_Object * obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	Eina_List *check_list = ad->profile_del_list;
	Setting_GenGroupItem_Data *item_data = NULL;
	int err;
	connection_profile_h profile_h;
	char *name = NULL;
	char *apn = NULL;
	char *id = NULL;
	char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
	char *def_apn = NULL;
	char *def_name = NULL;
	char *def_id = NULL;
	connection_profile_h def_profile = NULL;
/*TODO: select proper connection[] */
	(void)connection_get_default_cellular_service_profile(ad->connection[0],
			ad->profile_service_type, &def_profile);
	if (def_profile)
		connection_profile_get_cellular_apn(def_profile, &def_apn);
	if (def_profile)
		connection_profile_get_name(def_profile, &def_name);
	if (def_profile)
		connection_profile_get_id(def_profile, &def_id);
	SETTING_TRACE("Default profile:%p, def_apn[%s], def_name[%s]",
			def_profile, def_apn, def_name);

	bool result = TRUE;
	Setting_GenGroupItem_Data *first_valid_item = NULL;
	int ok_cnt = 0;
	bool is_default = 0;
	while (check_list) {
		item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(
				check_list);
		if (NULL == item_data) {
			check_list = eina_list_next(check_list);
			continue;
		}
		SETTING_TRACE("item_data->chk_status:%d",
				item_data->chk_status);
		if (!item_data->chk_status) {
			/*intilize at the first time */
			if (!first_valid_item)
				first_valid_item = item_data;
			check_list = eina_list_next(check_list);
			continue;
		}
		profile_h = (connection_profile_h)(item_data->belongs_to);
		connection_profile_get_name(profile_h, &name);
		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_id(profile_h, &id);
		if (!safeStrCmp(def_apn, apn) && !safeStrCmp(def_name, name)
				&& !safeStrCmp(def_id, id)) {
			is_default = TRUE;
		}
/*TODO: select proper connection[] */
		if ((err = connection_remove_profile(ad->connection[0], profile_h))
				!= CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR(
					"*** [ERR] connection_add_profile. err=%d ***",
					err);
			G_FREE(name);
			connection_profile_get_name(profile_h, &name);
			SETTING_TRACE("name:%s", name);
			if (!result) { /*already false */
				g_strlcat(speciliztion, ", ",
						MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, _(name),
						MAX_SPECIALIZITION_LEN);
			} else {
				g_strlcat(speciliztion, _(name),
						MAX_SPECIALIZITION_LEN);
			}
			result = FALSE;
		} else {
			ok_cnt++;
			if (is_default) {
				if (first_valid_item) {
					connection_profile_h first_profile_h =
							(connection_profile_h)(first_valid_item->belongs_to);
/*TODO: select proper connection[] */
					(void)connection_set_default_cellular_service_profile(
							ad->connection[0],
							ad->profile_service_type,
							first_profile_h);
				}
			}
			/*eina_list_remove(ad->profile_list, profile_h); */
		}

		G_FREE(apn);
		G_FREE(name);
		G_FREE(id);

		check_list = eina_list_next(check_list);
	}

	if (!result) {
		g_strlcat(speciliztion, _(": "), MAX_SPECIALIZITION_LEN);
		g_strlcat(speciliztion, _("IDS_COM_POP_DELETE_FAILED"),
				MAX_SPECIALIZITION_LEN);
		setting_create_popup(ad, ad->win_get, NULL, _(speciliztion),
				NULL, 0, false, false, 0);

		if (ad->navi_it_profile_del_list)
			elm_object_item_text_set(ad->navi_it_profile_del_list,
					_("IDS_ST_BODY_DELETE"));

		if (ok_cnt)
			__profile_delete_list_draw(ad);

		/*disable Delete button */
		Evas_Object *toolbar = elm_object_item_part_content_get(
				ad->navi_it_profile_del_list, "toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		elm_object_item_disabled_set(it, EINA_TRUE);
	}

	if (result) {
		setting_create_popup(ad, ad->win_get, NULL,
				"IDS_ST_BODY_DELETED",
				__popup_deleted_response_cb, 1, FALSE, FALSE,
				0);
	}

	if (def_profile) {
		G_FREE(def_apn);
		G_FREE(def_name);
		G_FREE(def_id);
	}

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void setting_network_profile_delete_click_softkey_select_all_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = data;
	Eina_List *check_list = ad->profile_del_list;
	Setting_GenGroupItem_Data *item_data = NULL;
	int selnum = 0;
	int un_selnum = 0;
	while (check_list) {
		item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(
				check_list);
		if (NULL == item_data)
			continue;

		SETTING_TRACE("%s :%d", _(item_data->keyStr),
				item_data->chk_status);
		if (item_data->chk_status) {
			selnum++;
		} else {
			un_selnum++;
		}
		check_list = eina_list_next(check_list);
	}
	SETTING_TRACE("selnum:%d, un_selnum:%d", selnum, un_selnum);
	if (un_selnum == 0 && selnum != 0) {
		/*current all are selected,so it need to unselect all after
		 * click the button */
		SETTING_TRACE("to unselect all");
		check_list = ad->profile_del_list;
		un_selnum = 0;
		while (check_list) {
			item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(
					check_list);
			if (NULL == item_data)
				continue;
			setting_update_gl_item_chk_status(item_data, 0);
			un_selnum++;
			check_list = eina_list_next(check_list);
		}
		selnum = 0;
	} else {
		/*or else, to select all */
		SETTING_TRACE("to select all");
		check_list = ad->profile_del_list;
		selnum = 0;
		while (check_list) {
			item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(
					check_list);
			if (NULL == item_data)
				continue;

			setting_update_gl_item_chk_status(item_data, 1);
			selnum++;
			check_list = eina_list_next(check_list);
		}
		un_selnum = 0;
	}

	if (ad->navi_it_profile_del_list) {
		Evas_Object *toolbar = elm_object_item_part_content_get(
				ad->navi_it_profile_del_list, "toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		if (selnum != 0) {
			char title[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = { 0, };
			snprintf(title, MAX_DISPLAY_NAME_LEN_ON_UI,
					_("IDS_ST_HEADER_PD_SELECTED"), selnum);
			elm_object_item_text_set(ad->navi_it_profile_del_list,
					_(title));
			elm_object_item_disabled_set(it, EINA_FALSE);
		} else {
			elm_object_item_text_set(ad->navi_it_profile_del_list,
					_("IDS_ST_BODY_DELETE"));
			elm_object_item_disabled_set(it, EINA_TRUE);
		}
	}
	return;
}

