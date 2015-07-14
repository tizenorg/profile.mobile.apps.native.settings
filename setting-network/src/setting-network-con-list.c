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


/**
 *  3
 */
#if SUPPORT_TETHERING
static int __get_naviframe_depth_con_list(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	Eina_List *list = elm_naviframe_items_get(ad->navi_bar);
	/*Object_Drawer *list_item = NULL; */

	int index = 0;
	while (list) {
		Elm_Object_Item *item = (Elm_Object_Item *) eina_list_data_get(list);
		if (NULL == item)
			continue;

		SETTING_TRACE("ad->navi_bar : %x ---  %d --- address of the item = %x", ad->navi_bar, index, item);
		index += 1;

		/*if not matched,to check next node. */
		list = eina_list_next(list);
		item = NULL;
	}
	return index;
}

static void __popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int response_type = POPUP_RESPONSE_CANCEL;
	if (0 == safeStrCmp(elm_object_text_get(obj), "OK")) {
		response_type = POPUP_RESPONSE_OK;
	}
	evas_object_del(ad->popup_conlist);
	ad->popup_conlist = NULL;

	if (POPUP_RESPONSE_OK == response_type) {
		setting_view_destroy(&setting_view_network_con_list, ad);
		setting_view_destroy(&setting_view_network_con, ad);
	}
}
/**
 * HERE
 */

static void __enabled_tethering(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is enabled -- callback\n");

	int frames =  __get_naviframe_depth_con_list(ad);
	if (frames == 3) {
		/* create a popup */
		ad->popup_conlist = setting_create_popup_with_btn(ad, ad->win_get,
		                                                  NULL, _(SETTING_NETWORK_NOT_ALLOWED_WITH_TETHERING),
		                                                  __popup_cb, 0, 1, "OK");
	}
}

static void __disabled_tethering(void *data)
{
	SETTING_TRACE("Tethering is disabled -- callback\n");
	/*setting_enable_genlist_item(ad->data_connection->item); */
}

static void __enabled_cb(tethering_error_e error, tethering_type_e type, bool is_requested, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is enabled -- callback\n");
	__enabled_tethering(ad);
}

static void __disabled_cb(tethering_error_e error, tethering_type_e type, tethering_disabled_cause_e code, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is disabled -- callback\n");
	__disabled_tethering(ad);
}

/**
 * DOCOMO required
 * if tethering is ON, 'connection' should be inactivated.
 */
static bool is_tethering_enabled(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	bool ret = false;
	tethering_h th = NULL;
	(void) tethering_create(&th);

	ad->th_conlists = th;

	tethering_set_enabled_cb(th, TETHERING_TYPE_ALL, __enabled_cb, data);
	tethering_set_disabled_cb(th, TETHERING_TYPE_ALL, __disabled_cb, data);

	if (tethering_is_enabled(th, TETHERING_TYPE_WIFI) == true ||
	    tethering_is_enabled(th, TETHERING_TYPE_USB) == true ||
	    tethering_is_enabled(th, TETHERING_TYPE_BT) == true) {

		SETTING_TRACE("Tethering is enabled\n");
		__enabled_tethering(ad);
		ret = true;
	} else {
		SETTING_TRACE("Tethering is not enabled\n");
		__disabled_tethering(ad);
		ret = false;;
	}

	/*tethering_destroy(th); */
	return ret;
}
#endif

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

	Evas_Object *radio;
	Setting_GenGroupItem_Data *item_data = NULL;

	radio = elm_radio_add(scroller);
	elm_radio_state_value_set(radio, -1);
	ad->rdg = radio;
	setting_disable_evas_object(radio);

	int srvType = ad->profile_service_type;

	char *def_apn = NULL;
	char *def_name = NULL;
	char *def_id = NULL;
	connection_profile_h def_profile = NULL;
	(void) connection_get_default_cellular_service_profile(ad->connection, srvType, &def_profile);
	if (def_profile) connection_profile_get_cellular_apn(def_profile, &def_apn);
	if (def_profile) connection_profile_get_name(def_profile, &def_name);
	if (def_profile) connection_profile_get_id(def_profile, &def_id);
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
		item_data->swallow_type = SWALLOW_Type_1RADIO_1BTN;
		item_data->r_swallow_path = (char *)g_strdup("reveal/extended");
		item_data->chk_status = idx;
		item_data->chk_change_cb = setting_network_con_list_chk_changed;
		item_data->stop_change_cb = setting_network_con_list_go_button_cb;
		item_data->rgd = radio;
		item_data->userdata = ad;
		item_data->sub_desc = (char *)g_strdup(apn);
		item_data->keyStr2 = (char *)g_strdup(id);
		if (idx == 0) {
			ad->selected_profile = item_data; /*defaulty,it the first item */
			item_data->group_style = SETTING_GROUP_STYLE_TOP;
		} else
			item_data->group_style = SETTING_GROUP_STYLE_CENTER;

		item_data->item =
		    elm_genlist_item_append(scroller, &(itc_2text_2icon_3), item_data, NULL,
		                            ELM_GENLIST_ITEM_NONE,
		                            setting_network_con_list_item_Gendial_mouse_up_cb, ad);
		if (idx == 0)
			setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_TOP);
		else
			setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_CENTER);

		SETTING_TRACE("profile_h:%p, apn:%s, id:%s", profile_h, apn, id);
		if (!safeStrCmp(def_apn, apn) && !safeStrCmp(def_name, name) && !safeStrCmp(def_id, id)) {
			SETTING_TRACE("Matched [idx:%d, apn:%s, id:%s]!", idx, apn, id);
			if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
				ad->selected_profile = item_data; /*the selected item */
				elm_radio_value_set(radio, idx);
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

	setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_BOTTOM);
	SETTING_TRACE("idx:%d", idx);
	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it_con_list, "toolbar");
	SETTING_TRACE("elm_object_text_get(del_btn):%s", elm_object_item_text_get(elm_toolbar_last_item_get(toolbar)));
	if (idx == 1) {
		setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_NONE);
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	} else if (idx > 1) {
		setting_genlist_item_groupstyle_set(item_data, SETTING_GROUP_STYLE_BOTTOM);
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
	} else {
		FREE(ad->apn_internet);
		SETTING_TRACE("To disable");
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
	}
	if (srvType == CONNECTION_CELLULAR_SERVICE_TYPE_MMS) {
		if (idx > 0) elm_radio_value_set(radio, 0);/*the first one */
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
	/*if (0 == idx) */
	/*	setting_create_simple_popup(ad, ad->navi_bar,NULL, "There is no avaiable profile,you can create one."); */

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
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");

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

	ad->navi_it_con_list =
	    setting_push_layout_navi_bar(_(title),
	                                 _("IDS_ST_BUTTON_BACK"),
	                                 _("IDS_ST_BODY_DELETE"),
	                                 _("IDS_ST_BODY_MYTHEME_CREATE"),
	                                 NULL,
	                                 setting_network_con_list_click_softkey_delete_cb,
	                                 setting_network_con_list_click_softkey_create_cb,
	                                 ad, scroller, ad->navi_bar, NULL);

	elm_naviframe_item_pop_cb_set(ad->navi_it_con_list,
	                              setting_network_con_list_click_softkey_cancel_cb, ad);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);
	ad->con_list_gl = scroller;
	__con_list_recreate(ad);

#if SUPPORT_TETHERING
	is_tethering_enabled(ad);
#endif
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
#if SUPPORT_TETHERING
		if (ad->th_conlists) {
			SETTING_TRACE("tethering destruction ");
			tethering_destroy(ad->th_conlists);
			ad->th_conlists = NULL;
		}
#endif
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
				setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_MSGF_POP_UNSUPPORTED"));
				/*elm_radio_value_set(list_item->eo_check, old_type); */
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

					setting_create_popup_without_btn(list_item, ad->win_get, NULL,
					                                 _("IDS_CST_POP_FAILED"), NULL,
					                                 2.0, FALSE, FALSE);
					return;
				}

				setting_create_popup_without_btn(list_item, ad->win_get, NULL,
				                                 _("IDS_ST_BUTTON2_PROCESSING_ING"), __set_default_response_cb,
				                                 2.0, TRUE, FALSE);
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

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (list_item->chk_status == elm_radio_value_get(list_item->eo_check)) {
		SettingNetworkUG *ad = data;
		elm_genlist_realized_items_update(ad->con_list_gl);
		return;
	}

	elm_radio_value_set(list_item->eo_check, list_item->chk_status);
	__set_default_profile(list_item);
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
}

static void setting_network_con_list_click_softkey_create_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	/*setting_create_simple_popup(ad, ad->win_get, NULL, _("UX Undefined")); */
	/*return; */

	/*to add a new connection */
	/*ad->con_type = -1; */
	/*FREE(ad->con_name); */
	/*ad->need_check_srvtype = FALSE; */
	/*ad->con_name = strdup(STR_SETTING_NEW_CONNECTIONS); */
	FREE(ad->access_name);
	setting_view_change(&setting_view_network_con_list, &setting_view_network_connection_create, ad);
}

static Eina_Bool
setting_network_con_list_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_ERROR("TTT");
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_con_list,
	                    &setting_view_network_con, ad);
	return EINA_TRUE;
}
