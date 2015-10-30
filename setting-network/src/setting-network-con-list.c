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
#include <setting-network-con-list.h>

static int setting_network_con_list_create(void *cb);
static int setting_network_con_list_destroy(void *cb);
static int setting_network_con_list_update(void *cb);
static int setting_network_con_list_cleanup(void *cb);
void setting_network_con_list_chk_changed(void *data, Evas_Object *obj, void *event_info);

setting_view setting_view_network_con_list = {
	.create = setting_network_con_list_create,
	.destroy = setting_network_con_list_destroy,
	.update = setting_network_con_list_update,
	.cleanup = setting_network_con_list_cleanup,
};

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

static void
win_rotation_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *ctxpopup = data;
	move_more_ctxpopup(ctxpopup);
}

/* Icon + Text (More button style : Naviframe Toolbar) */
static void create_ctxpopup_more_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Evas_Object *it_obj;
	Evas_Object *nf = ad->navi_bar;
	Evas_Object *win;
	Elm_Object_Item *it;

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

	/* We convince the top widget is a window */
	win = elm_object_top_widget_get(nf);
	evas_object_smart_callback_add(win, "rotation,changed", win_rotation_changed_cb, ctxpopup);

	//---------------------------------------------------------------------------------------------
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BODY_MYTHEME_CREATE"), NULL, setting_network_con_list_click_softkey_create_cb, ad);
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BODY_DELETE"), NULL, setting_network_con_list_click_softkey_delete_cb, ad);
	//---------------------------------------------------------------------------------------------

	elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN, ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	move_more_ctxpopup(ctxpopup);
	evas_object_show(ctxpopup);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int __con_list_recreate(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	Evas_Object *scroller;
	scroller = ad->con_list_gl;
	elm_genlist_clear(scroller);	/* first to clear list */
	Elm_Object_Item *first_item;
	/* item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_GROUP, NULL, NULL); */
	first_item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                                     ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(first_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	Evas_Object *radio_group;
	Setting_GenGroupItem_Data *item_data = NULL;

	radio_group = elm_radio_add(scroller);
	elm_radio_state_value_set(radio_group, -1);
	ad->rdg = radio_group;
	//setting_disable_evas_object(radio_group);

	int srvType = ad->profile_service_type;

	char *def_apn = NULL;
	char *def_name = NULL;
	char *def_id = NULL;
	connection_profile_h def_profile = NULL;
	(void) connection_get_default_cellular_service_profile(ad->connection, srvType, &def_profile);

	if (def_profile)
		connection_profile_get_cellular_apn(def_profile, &def_apn);
	if (def_profile)
		connection_profile_get_name(def_profile, &def_name);
	if (def_profile)
		connection_profile_get_id(def_profile, &def_id);

	SETTING_TRACE("Default profile:%p, def_apn[%s], def_name[%s], def_id[%s]", def_profile, def_apn, def_name, def_id);

	setting_network_reget_profile_list(ad);

	Eina_List *elist = NULL;
	connection_profile_h profile_h = NULL;
	int idx = 0;
	char *id = NULL;
	ad->selected_profile = NULL;
	char *apn;
	char *name;
	char *proxy_address;
	EINA_LIST_FOREACH(ad->profile_list, elist, profile_h) {
		apn = name = NULL;
		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_name(profile_h, &name);
		connection_profile_get_proxy_address(profile_h, CONNECTION_ADDRESS_FAMILY_IPV4, &proxy_address);
		connection_profile_get_id(profile_h, &id);


		item_data = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
		setting_retvm_if(!item_data, SETTING_RETURN_FAIL, "calloc failed");
		item_data->keyStr = (char *)g_strdup(name);
		item_data->swallow_type = SWALLOW_TYPE_1RADIO_RIGHT_PROPAGATE_EVENTS_SET;

		item_data->r_swallow_path = (char *)g_strdup("reveal/extended");
		item_data->chk_status = idx;
		item_data->chk_change_cb = setting_network_con_list_chk_changed;
		item_data->stop_change_cb = setting_network_con_list_go_button_cb;
		item_data->rgd = radio_group;
		item_data->userdata = ad;
		item_data->sub_desc = (char *)g_strdup(apn);
		item_data->keyStr2 = (char *)g_strdup(id);

		if (idx == 0) {
			ad->selected_profile = item_data; /*defaulty,it the first item */
		}

		item_data->item =
		    elm_genlist_item_append(scroller, &(itc_2text_2icon_3), item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            setting_network_con_list_item_Gendial_mouse_up_cb, ad);

		SETTING_TRACE_ERROR(">>>> chk_status : <%d>, state_value_get for %s ---> (%d) value_get[%d], ptr : %x, org(%x)",
						item_data->chk_status, 
						item_data->keyStr, 
						elm_radio_state_value_get(item_data->eo_check), 
						elm_radio_value_get(item_data->eo_check),
						item_data->chk_change_cb,
						(void*)setting_network_con_list_chk_changed);

		SETTING_TRACE("profile_h:%p, apn:%s, id:%s", profile_h, apn, id);
		if (!safeStrCmp(def_apn, apn) && !safeStrCmp(def_name, name) && !safeStrCmp(def_id, id)) {
			SETTING_TRACE("Matched [idx:%d, apn:%s, id:%s]!", idx, apn, id);
			if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
				ad->selected_profile = item_data; /*the selected item */
				elm_radio_value_set(radio_group, idx);
				if (ad->internet_conn) {
					G_FREE(ad->internet_conn->sub_desc);
					FREE(ad->apn_internet);
					ad->apn_internet = strdup(name);
					ad->internet_conn->sub_desc = strdup(name);
					elm_object_item_data_set(ad->internet_conn->item, ad->internet_conn);
					elm_genlist_item_update(ad->internet_conn->item);
				}
			}
		}

		G_FREE(apn);
		G_FREE(name);
		G_FREE(proxy_address);
		G_FREE(id);
		idx++;
	}
	ADD_GL_SEPARATOR(scroller);

	SETTING_TRACE("idx:%d", idx);
	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_con_list, "toolbar");
	SETTING_TRACE("elm_object_text_get(del_btn):%s", elm_object_item_text_get(elm_toolbar_last_item_get(toolbar)));
	if (idx == 1) {
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	} else if (idx > 1) {
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	} else {
		FREE(ad->apn_internet);
		SETTING_TRACE("To disable");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
	}
	if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_MMS) {
		if (idx > 0) elm_radio_value_set(radio_group, 0);/*the first one */
		if (ad->selected_profile && !isEmptyStr(ad->selected_profile->keyStr) && ad->mms_conn) {
			SETTING_TRACE("ad->selected_profile->keyStr:%s", ad->selected_profile->keyStr);
			G_FREE(ad->mms_conn->sub_desc);
			FREE(ad->apn_MMS);
			ad->apn_MMS = strdup(ad->selected_profile->keyStr);
			ad->mms_conn->sub_desc = strdup(ad->selected_profile->keyStr);
			elm_genlist_item_fields_update(ad->mms_conn->item, "*", ELM_GENLIST_ITEM_FIELD_TEXT);
		}
	}
	ad->cur_profile_num = idx;

	SETTING_TRACE_END;
	G_FREE(def_id);
	G_FREE(def_apn);
	G_FREE(def_name);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_list_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
	         "Cannot set scroller object  as contento of layout");

	const char *title = NULL;
	switch (ad->profile_service_type) {
		case CONNECTION_CELLULAR_SERVICE_TYPE_MMS:
			title = "IDS_ST_BODY_MMS_CONNECTIONS";
			break;
		case CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET:
			title = "IDS_ST_BODY_INTERNET_CONNECTION";
			break;
		default:
			title = ad->con_name;
			break;
	}

	/* [UI] Internet connection */
	/* [UI] MMS connection */
	ad->navi_it_con_list =
	    setting_push_layout_navi_bar(_(title),
	                                 NULL, /* ARROW STYLE */
	                                 _("IDS_ST_BODY_DELETE"),
	                                 _("IDS_ST_BODY_MYTHEME_CREATE"),
	                                 setting_network_con_list_click_softkey_cancel_cb,
	                                 setting_network_con_list_click_softkey_delete_cb,
	                                 setting_network_con_list_click_softkey_create_cb,
	                                 ad, scroller, ad->navi_bar, NULL);

	elm_naviframe_item_pop_cb_set(ad->navi_it_con_list,
	                              setting_network_con_list_click_softkey_cancel_cb, ad);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);


	// Add ctx popup handler
	Evas_Object* btn = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn, "naviframe/more/default");
	evas_object_smart_callback_add(btn, "clicked", create_ctxpopup_more_button_cb, ad);
	elm_object_item_part_content_set(ad->navi_it_con_list, "toolbar_more_btn", btn);

	ad->con_list_gl = scroller;
	elm_genlist_mode_set(ad->con_list_gl, ELM_LIST_COMPRESS);	/* TNEF-4143 */

	__con_list_recreate(ad);

	setting_view_network_con_list.is_create = 1;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_list_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (setting_view_network_con_list.is_create) {
		/*ad->connections_gl = NULL; */
		setting_view_network_con_list.is_create = 0;
		ad->selected_profile = NULL;
		ad->navi_it_con_list = NULL;
		ad->con_list_gl = NULL;
		elm_naviframe_item_pop(ad->navi_bar);
	} else {
		SETTING_TRACE("why is this is_create == 0 ?? !!!");
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_list_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	__con_list_recreate(cb);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_list_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return setting_network_con_list_destroy(cb);
	/*return SETTING_RETURN_SUCCESS; */
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
static void setting_network_con_list_go_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = data;

	SettingNetworkUG *ad = item_data->userdata;
	FREE(ad->access_name);
	ad->access_name = strdup(item_data->keyStr2);
	setting_view_change(&setting_view_network_con_list, &setting_view_network_connection_create, ad);
}

#if 0
static void ___network_con_set_default_cb(connection_error_e result, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	SettingNetworkUG *ad = list_item->userdata;
	setting_retm_if(ad == NULL, "ad parameter is NULL");
	if (ad->network_ug_pop) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}

	int srvType = CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION;
	if (0 == safeStrCmp(ad->con_name, "IDS_ST_BODY_INTERNET_CONNECTION")) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
	} else if (0 == safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
	}
	SETTING_TRACE("The result is :%d,but it is meaningless for our Setting", result);
	ad->selected_profile = list_item;
	if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET && ad->internet_conn) {
		G_FREE(ad->internet_conn->sub_desc);
		FREE(ad->apn_internet);
		ad->apn_internet = strdup(list_item->keyStr);
		ad->internet_conn->sub_desc = strdup(list_item->keyStr);
		elm_object_item_data_set(ad->internet_conn->item, ad->internet_conn);
		elm_genlist_item_update(ad->internet_conn->item);
	}
	elm_genlist_realized_items_update(ad->con_list_gl);
}
#endif

static void __set_default_response_cb(void *data, Evas_Object *obj,
                                      void *event_info)
{
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	SettingNetworkUG *ad = list_item->userdata;

	int srvType = CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION;
	if (0 == safeStrCmp(ad->con_name, "IDS_ST_BODY_INTERNET_CONNECTION")) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
	} else if (0 == safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
	}
	/*SETTING_TRACE("The result is :%d,but it is meaningless for our Setting", result); */
	ad->selected_profile = list_item;
	if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET && ad->internet_conn) {
		G_FREE(ad->internet_conn->sub_desc);
		FREE(ad->apn_internet);
		ad->apn_internet = strdup(list_item->keyStr);
		ad->internet_conn->sub_desc = strdup(list_item->keyStr);
		elm_object_item_data_set(ad->internet_conn->item, ad->internet_conn);
		elm_genlist_item_update(ad->internet_conn->item);
	}
	elm_genlist_realized_items_update(ad->con_list_gl);

	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
}


void __set_default_profile(void *data)
{
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	SettingNetworkUG *ad = list_item->userdata;
	setting_retm_if(ad == NULL, "ad parameter is NULL");

	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;
	int rv = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
		return;
	}

	int cnt = 0;
	char *apn = NULL;
	char *profile_id = NULL;
	connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;

	int srvType = CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION;
	if (0 == safeStrCmp(ad->con_name, "IDS_ST_BODY_INTERNET_CONNECTION")) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
	} else if (0 == safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		srvType = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
	}
	while (connection_profile_iterator_has_next(profile_iter)) {
		if (cnt >= MAX_PROFILE_NUM) {
			break;
		}
		if (connection_profile_iterator_next(profile_iter, &profile_h) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile handle");
			break;
		}
		if (connection_profile_get_cellular_service_type(profile_h, &service_type) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get service type");
			continue;
		}

		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_id(profile_h, &profile_id);
		if (srvType != service_type) {
			G_FREE(apn);
			continue;
		}

		SETTING_TRACE("Record[%d] profile is %s(%s)", cnt, apn, profile_id);
		if (0 == safeStrCmp(profile_id, list_item->keyStr2)) {

			if (CONNECTION_CELLULAR_SERVICE_TYPE_MMS == srvType) {
				setting_create_popup(ad, ad->win_get, NULL, _("IDS_MSGF_POP_UNSUPPORTED"), NULL, 0, false, false, 0);
				if (ad->selected_profile)
					elm_radio_value_set(list_item->eo_check, ad->selected_profile->chk_status);
				break;
			} else {
				SETTING_TRACE("Set default[%d] as [%s], profile:%p, id:%s", srvType, apn, profile_h, profile_id);
				int ret = connection_set_default_cellular_service_profile(ad->connection, srvType, profile_h);
				SETTING_TRACE("The result is :%d", ret);

				if (ad->network_ug_pop) {
					evas_object_del(ad->network_ug_pop);
					ad->network_ug_pop = NULL;
				}
				if (ret != CONNECTION_ERROR_NONE) {
					if (ad->selected_profile)
						elm_radio_value_set(list_item->eo_check, ad->selected_profile->chk_status);

					setting_create_popup(list_item, ad->win_get, NULL,
		                                 _("IDS_CST_POP_FAILED"), NULL,
		                                 2.0, FALSE, FALSE, 0);
					return;
				}

				setting_create_popup(list_item, ad->win_get, NULL,
	                                 _("IDS_ST_BUTTON2_PROCESSING_ING"), __set_default_response_cb,
	                                 2.0, TRUE, FALSE, 0);
			}
			break;
		}
		cnt++;
	}
	G_FREE(apn);
	G_FREE(profile_id);

}
static void setting_network_con_list_item_Gendial_mouse_up_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");


	SettingNetworkUG *ad = list_item->userdata;
	FREE(ad->access_name);
	ad->access_name = strdup(list_item->keyStr2);
	setting_view_change(&setting_view_network_con_list, &setting_view_network_connection_create, ad);
}
void setting_network_con_list_chk_changed(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL || obj == NULL, "Parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	list_item->chk_status = elm_radio_value_get(obj);
	__set_default_profile(list_item);

}

static void setting_network_con_list_click_softkey_delete_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	setting_view_change(&setting_view_network_con_list, &setting_view_network_profile_delete, ad);
	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static void setting_network_con_list_click_softkey_create_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	FREE(ad->access_name);
	setting_view_change(&setting_view_network_con_list, &setting_view_network_connection_create, ad);
	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static Eina_Bool
setting_network_con_list_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_con_list,
	                    &setting_view_network_con, ad);
	return EINA_TRUE;
}
