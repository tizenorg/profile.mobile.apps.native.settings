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
static int setting_network_profile_delete_update(void *cb);
static int setting_network_profile_delete_cleanup(void *cb);
setting_view setting_view_network_profile_delete = {
	.create = setting_network_profile_delete_create,
	.destroy = setting_network_profile_delete_destroy,
	.update = setting_network_profile_delete_update,
	.cleanup = setting_network_profile_delete_cleanup,
};

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
	elm_genlist_clear(ad->gl_profile_del);	/* first to clear list */
	Elm_Object_Item *first_item;
	/* item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL); */
	first_item = elm_genlist_item_append(ad->gl_profile_del, &itc_seperator, NULL, NULL,
	                                     ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(first_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	Eina_List *elist = NULL;
	connection_profile_h profile_h = NULL;
	int idx = 0;
	char *id = NULL;
	char *apn = NULL;
	char *name = NULL;
	char *proxy_address = NULL;
	connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
	Setting_GenGroupItem_Data *item_data = NULL;

	setting_network_reget_profile_list(ad);
	SETTING_TRACE("ad->profile_list:%p", ad->profile_list);
	EINA_LIST_FOREACH(ad->profile_list, elist, profile_h) {
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
		connection_profile_get_proxy_address(profile_h, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
		connection_profile_get_id(profile_h, &id);

		if (connection_profile_get_type(profile_h, &profile_type) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile type");
			continue;
		}

		SETTING_TRACE("Record[%s], profile_type:%d, service_type:%d, id:%s",
		              apn, profile_type, ad->profile_service_type, id);

		item_data = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
		if (0 == item_data) {
			SETTING_TRACE_ERROR("calloc failed");
			G_FREE(apn);
			return;
		}
		item_data->keyStr = (char *)g_strdup(name);
		item_data->swallow_type = SWALLOW_Type_1CHECK;
		item_data->r_swallow_path = (char *)g_strdup("reveal/extended");
		/*item_data->chk_status = 0; */
		item_data->chk_change_cb = setting_network_profile_delete_check_cb;
		item_data->userdata = ad;
		item_data->sub_desc = (char *)g_strdup(apn);
		item_data->belongs_to = (int) profile_h;
		if (idx == 0) {
			item_data->group_style = SETTING_GROUP_STYLE_TOP;
		} else
			item_data->group_style = SETTING_GROUP_STYLE_CENTER;

		item_data->item =
		    elm_genlist_item_append(ad->gl_profile_del, &(itc_2text_1icon_2), item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            setting_network_profile_delete_mouse_up_cb, ad);
		if (idx == 0)
			setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_TOP);
		else
			setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_CENTER);
		ad->profile_del_list =
		    eina_list_append(ad->profile_del_list, item_data);

		idx++;
	}
	setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_BOTTOM);

	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_profile_del_list, "toolbar");
	Evas_Object *allbtn = elm_object_item_part_content_get(ad->navi_it_profile_del_list, "title_left_btn");
	SETTING_TRACE("idx:%d", idx);
	if (idx == 1) {
		setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_NONE);
		/*if (button) elm_object_disabled_set(button, EINA_FALSE); */
		if (allbtn) elm_object_disabled_set(allbtn, EINA_FALSE);
	} else if (idx > 1) {
		setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_BOTTOM);
		/*if (button) elm_object_disabled_set(button, EINA_FALSE); */
		if (allbtn) elm_object_disabled_set(allbtn, EINA_FALSE);
	} else {
		/*all are invalid,to delete directly */
		if (ad->profile_list != NULL) {
			eina_list_free(ad->profile_list);
			ad->profile_list = NULL;
		}
		SETTING_TRACE("To disable");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
		/*if (button) elm_object_disabled_set(button, EINA_TRUE); */
		if (allbtn) elm_object_disabled_set(allbtn, EINA_TRUE);
		SETTING_TRACE("To disable");
	}
	G_FREE(apn);
	G_FREE(name);
	G_FREE(proxy_address);
	G_FREE(id);
	return;
}

static int setting_network_profile_delete_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
	         "Cannot set scroller object  as contento of layout");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller);	/* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);
	ad->gl_profile_del = scroller;

	ad->navi_it_profile_del_list = setting_push_layout_navi_bar(_("IDS_ST_BODY_DELETE"),
	                                                            dgettext("sys_string", "IDS_ST_BUTTON_BACK"),
	                                                            dgettext("sys_string", "IDS_ST_BODY_DELETE"),
	                                                            _("IDS_ST_BUTTON_CANCEL_ABB"),
	                                                            setting_network_profile_delete_click_softkey_cancel_cb,
	                                                            setting_network_profile_delete_click_softkey_delete_cb,
	                                                            setting_network_profile_delete_click_softkey_cancel_cb,
	                                                            ad, scroller, ad->navi_bar, NULL);
	/*disable Delete button */
	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_profile_del_list, "toolbar");
	Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
	elm_object_item_disabled_set(it, EINA_TRUE);

	Evas_Object *allbtn = elm_button_add(ad->navi_bar);

	elm_object_style_set(allbtn, "naviframe/title_icon");
	Evas_Object *icon = elm_icon_add(allbtn);
	elm_object_text_set(allbtn, _("IDS_ST_OPT_ALL"));
	/*elm_image_file_set(icon, EDJ_NAME, "icon.select_all"); */
	elm_image_file_set(icon, SETTING_ICON_PATH_CFG"select_all.png", NULL);

	/*evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1 , 1); */
	/*elm_image_resizable_set(icon, EINA_TRUE, EINA_TRUE); */
	elm_object_content_set(allbtn, icon);
	elm_object_focus_allow_set(allbtn, EINA_FALSE);
	evas_object_propagate_events_set(allbtn, EINA_FALSE);
	evas_object_smart_callback_add(allbtn, "clicked", setting_network_profile_delete_click_softkey_select_all_cb, ad);
	evas_object_show(allbtn);
	elm_object_item_part_content_set(ad->navi_it_profile_del_list, "title_left_btn", allbtn);
	__profile_delete_list_draw(ad);
	ADD_GL_SEPARATOR(scroller);
	setting_view_network_profile_delete.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_profile_delete_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (!setting_view_network_profile_delete.is_create)
		return SETTING_RETURN_SUCCESS;

	/* free data */
	if (ad->profile_del_list != NULL) {
		eina_list_free(ad->profile_del_list);
		ad->profile_del_list = NULL;
	}
	ad->gl_profile_del = NULL;
	ad->navi_it_profile_del_list = NULL;
	setting_view_network_profile_delete.is_create = 0;
	elm_naviframe_item_pop(ad->navi_bar);

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_profile_delete_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	__profile_delete_list_draw(cb);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_profile_delete_cleanup(void *cb)
{
	return setting_network_profile_delete_destroy(cb);
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
		item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(check_list);
		if (NULL == item_data)
			continue;

		SETTING_TRACE("%s :%d", _(item_data->keyStr), item_data->chk_status);
		if (item_data->chk_status) {
			selnum++;
		}
		check_list = eina_list_next(check_list);
	}
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	if (navi_it) {
		Evas_Object *toolbar = elm_object_item_part_content_get(navi_it, "toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		if (selnum != 0) {
			char title[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
			snprintf(title, MAX_DISPLAY_NAME_LEN_ON_UI, _("IDS_ST_HEADER_PD_SELECTED"), selnum);
			elm_object_item_text_set(navi_it, _(title));
			elm_object_item_disabled_set(it, EINA_FALSE);
		} else {
			elm_object_item_text_set(navi_it, _("IDS_ST_BODY_DELETE"));
			elm_object_item_disabled_set(it, EINA_TRUE);
		}
	}
}

static void
setting_network_profile_delete_check_cb(void *data, Evas_Object *obj,
                                        void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	___profile_selected(list_item->userdata);
}

static void
setting_network_profile_delete_mouse_up_cb(void *data,
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
	SETTING_TRACE("list_item->keyStr:%s", list_item->keyStr);

	setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
	___profile_selected(list_item->userdata);
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
static void
setting_network_profile_delete_click_softkey_cancel_cb(void *data,
                                                       Evas_Object *
                                                       obj, void
                                                       *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	setting_view_change(&setting_view_network_profile_delete,
	                    &setting_view_network_con_list, ad);
}

static void __popup_deleted_response_cb(void *data, Evas_Object *obj,
                                        void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_profile_delete, &setting_view_network_con_list, ad);
}

static void
setting_network_profile_delete_click_softkey_delete_cb(void *data,
                                                       Evas_Object *
                                                       obj, void
                                                       *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
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
	(void) connection_get_default_cellular_service_profile(ad->connection, ad->profile_service_type, &def_profile);
	if (def_profile) connection_profile_get_cellular_apn(def_profile, &def_apn);
	if (def_profile) connection_profile_get_name(def_profile, &def_name);
	if (def_profile) connection_profile_get_id(def_profile, &def_id);
	SETTING_TRACE("Default profile:%p, def_apn[%s], def_name[%s]", def_profile, def_apn, def_name);

	bool result = TRUE;
	Setting_GenGroupItem_Data *first_valid_item = NULL;
	int ok_cnt = 0;
	bool is_default = 0;
	while (check_list) {
		item_data = (Setting_GenGroupItem_Data *) eina_list_data_get(check_list);
		if (NULL == item_data) {
			check_list = eina_list_next(check_list);
			continue;
		}
		SETTING_TRACE("item_data->chk_status:%d", item_data->chk_status);
		if (!item_data->chk_status) {
			if (!first_valid_item) first_valid_item = item_data;/*intilize at the first time */
			check_list = eina_list_next(check_list);
			continue;
		}
		profile_h = (connection_profile_h)(item_data->belongs_to);
		connection_profile_get_name(profile_h, &name);
		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_id(profile_h, &id);
		if (!safeStrCmp(def_apn, apn) && !safeStrCmp(def_name, name) && !safeStrCmp(def_id, id)) {
			is_default = TRUE;
		}
		if ((err = connection_remove_profile(ad->connection, profile_h)) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("*** [ERR] connection_add_profile. err=%d ***", err);
			/*setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_CST_POP_FAILED")); */
			G_FREE(name);
			connection_profile_get_name(profile_h, &name);
			SETTING_TRACE("name:%s", name);
			if (!result) { /*already false */
				g_strlcat(speciliztion, ", ", MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, _(name), MAX_SPECIALIZITION_LEN);
			} else {
				g_strlcat(speciliztion, _(name), MAX_SPECIALIZITION_LEN);
			}
			result = FALSE;
		} else {
			ok_cnt++;
			if (is_default) {
				/*setting_create_simple_popup(ad, ad->win_get, NULL, _("Deleting default profile")); */

				if (first_valid_item) {
					connection_profile_h first_profile_h = (connection_profile_h)(first_valid_item->belongs_to);
					(void)connection_set_default_cellular_service_profile(ad->connection,
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
		g_strlcat(speciliztion, _("IDS_COM_POP_DELETE_FAILED"), MAX_SPECIALIZITION_LEN);
		setting_create_simple_popup(ad, ad->win_get, NULL, _(speciliztion));

		if (ad->navi_it_profile_del_list)
			elm_object_item_text_set(ad->navi_it_profile_del_list, _("IDS_ST_BODY_DELETE"));

		if (ok_cnt) __profile_delete_list_draw(ad);

		/*disable Delete button */
		Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_profile_del_list, "toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		elm_object_item_disabled_set(it, EINA_TRUE);
	}

	if (result) {
		/*setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_IM_BODY_SUCCESSFULLY_COMPLETED")); */
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_ST_BODY_DELETED"), __popup_deleted_response_cb, 1, FALSE, FALSE);
		/*setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_ST_BODY_DELETED"), __popup_deleted_response_cb, 0.5, FALSE, FALSE); */
	}

	if (def_profile) {
		G_FREE(def_apn);
		G_FREE(def_name);
		G_FREE(def_id);
	}
}

static void
setting_network_profile_delete_click_softkey_select_all_cb(void *data,
                                                           Evas_Object *obj,
                                                           void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = data;
	Eina_List *check_list = ad->profile_del_list;
	Setting_GenGroupItem_Data *item_data = NULL;
	int selnum = 0;
	int un_selnum = 0;
	while (check_list) {
		item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(check_list);
		if (NULL == item_data)
			continue;

		SETTING_TRACE("%s :%d", _(item_data->keyStr), item_data->chk_status);
		if (item_data->chk_status) {
			selnum++;
		} else {
			un_selnum++;
		}
		check_list = eina_list_next(check_list);
	}
	SETTING_TRACE("selnum:%d, un_selnum:%d", selnum, un_selnum);
	if (un_selnum == 0 && selnum != 0) {
		/*current all are selected,so it need to unselect all after click the button */
		SETTING_TRACE("to unselect all");
		check_list = ad->profile_del_list;
		un_selnum = 0;
		while (check_list) {
			item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(check_list);
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
			item_data = (Setting_GenGroupItem_Data *)eina_list_data_get(check_list);
			if (NULL == item_data)
				continue;

			setting_update_gl_item_chk_status(item_data, 1);
			selnum++;
			check_list = eina_list_next(check_list);
		}
		un_selnum = 0;
	}

	if (ad->navi_it_profile_del_list) {
		Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_profile_del_list, "toolbar");
		Elm_Object_Item *it = elm_toolbar_last_item_get(toolbar);
		if (selnum != 0) {
			char title[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
			snprintf(title, MAX_DISPLAY_NAME_LEN_ON_UI, _("IDS_ST_HEADER_PD_SELECTED"), selnum);
			elm_object_item_text_set(ad->navi_it_profile_del_list, _(title));
			elm_object_item_disabled_set(it, EINA_FALSE);
		} else {
			elm_object_item_text_set(ad->navi_it_profile_del_list, _("IDS_ST_BODY_DELETE"));
			elm_object_item_disabled_set(it, EINA_TRUE);
		}
	}
	return;
}


