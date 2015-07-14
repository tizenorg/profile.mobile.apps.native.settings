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
#include <setting-network-connection-create.h>
#include <notification.h>

#define MAX_PDP_APN_LEN_MAX 100
#define MAX_PDP_AUTH_USERNAME_LEN_MAX 32
#define MAX_PDP_AUTH_PASSWORD_LEN_MAX 32
#define MAX_HOME_URL_LEN_MAX 512
#define MAX_PROXY_LEN_MAX 64

static int setting_network_connection_create(void *cb);
static int setting_network_connection_destroy(void *cb);
static int setting_network_connection_update(void *cb);
static int setting_network_connection_cleanup(void *cb);

setting_view setting_view_network_connection_create = {
	.create = setting_network_connection_create,
	.destroy = setting_network_connection_destroy,
	.update = setting_network_connection_update,
	.cleanup = setting_network_connection_cleanup,
};

SettingNetworkUG *g_networkUG = NULL;
#define SUPPORT_EXPANDABLE_AUTHTYPE 1
#define URL_HEAD "http://"
#define INTERNET "IDS_ST_BODY_CONNECTIONSETTINGS_INTERNET"
#define PLUS_MMS "+ MMS"
/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

#if SUPPORT_TETHERING
static int __get_naviframe_depth(void *data)
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
		SETTING_TRACE("OK button is clicked");
	}
	evas_object_del(ad->popup_concreate);
	ad->popup_concreate = NULL;

	ad->viewtype = ERROR_STATE;

	if (POPUP_RESPONSE_OK == response_type) {
		SETTING_TRACE("OK button is clicked - catched");
		/* YES */
		int frames = __get_naviframe_depth(ad);
		SETTING_TRACE(" -------> frames == (%d) ", frames);
		if (frames == 3) {
			ad->viewtype = CREATE_CONNECTIONS_MAIN;
			SETTING_TRACE(" destroy --> setting-network-connection-create");
			SETTING_TRACE(" destroy --> setting-network-connections");
			SETTING_TRACE(" goto --> setting-network-main");
			setting_view_destroy(&setting_view_network_connection_create, ad);
			setting_view_destroy(&setting_view_network_con, ad);

		} else if (frames == 4) {
			ad->viewtype = CREATE_CONLIST_CONNECTIONS_MAIN;
			SETTING_TRACE(" destroy --> setting-network-connection-create");
			SETTING_TRACE(" destroy --> setting-network-con-list");
			SETTING_TRACE(" destroy --> setting-network-connections");
			SETTING_TRACE(" goto --> setting-network-main");

			setting_view_destroy(&setting_view_network_connection_create, ad);
			setting_view_destroy(&setting_view_network_con_list, ad);
			setting_view_destroy(&setting_view_network_con, ad);
		} else {
			SETTING_TRACE(" THINK MORE !!!!! OOPS ");
			ad->viewtype = ERROR_STATE;
		}
	}

}

static void __enabled_tethering(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is enabled -- callback\n");
	/* back, back */

	/* create a popup */
	ad->popup_concreate = setting_create_popup_with_btn(ad, ad->win_get,
	                                                    NULL, _(SETTING_NETWORK_NOT_ALLOWED_WITH_TETHERING),
	                                                    __popup_cb, 0, 1, "OK");
}

static void __disabled_tethering(void *data)
{
	SETTING_TRACE("Tethering is disabled -- callback\n");
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
	(void)tethering_create(&th);

	ad->th_concreate = th;

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

#if 0
static void
__connection_gl_mouse_up(void *data, Evas *e, Evas_Object *obj,
                         void *event)
{
	ret_if(!data);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->data_profile_name) {
		setting_hide_input_pannel_cb(ad->data_profile_name->eo_check);
	}

	if (ad->data_acs_name) {
		setting_hide_input_pannel_cb(ad->data_acs_name->eo_check);
	}
	if (ad->data_user_name) {
		setting_hide_input_pannel_cb(ad->data_user_name->eo_check);
	}
	if (ad->data_pwd) {
		setting_hide_input_pannel_cb(ad->data_pwd->eo_check);
	}
	if (ad->data_pxy_addr) {
		setting_hide_input_pannel_cb(ad->data_pxy_addr->eo_check);
	}
	if (ad->data_pxy_port) {
		setting_hide_input_pannel_cb(ad->data_pxy_port->eo_check);
	}
	if (ad->data_hm_url) {
		setting_hide_input_pannel_cb(ad->data_hm_url->eo_check);
	}

}
#endif

void __sub_list_rd_change(void *data, Evas_Object *obj, void *event_info)
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

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);

	SettingNetworkUG *ad = data_parentItem->userdata;

	if (data_parentItem == ad->data_auth_type) {
		elm_genlist_item_expanded_set(ad->data_auth_type->item, FALSE);
		elm_genlist_item_update(ad->data_auth_type->item);

		setting_network_connection_display_auth_type(data_parentItem->userdata, list_item->chk_status);
	} else if (data_parentItem == ad->data_srv_type) {
		if (ad->srvType != data_subItem->chk_status)
			ad->srvType = data_subItem->chk_status;
		if (ad->srvType != CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
			if (!ad->data_hm_url) {
				/*need home url */
				ad->data_hm_url =
				    setting_create_Gendial_field_def(obj,
				                                     &itc_editfield,
				                                     NULL, NULL,
				                                     SWALLOW_Type_LAYOUT_EDITFIELD,
				                                     NULL, NULL, 0,
				                                     "IDS_ST_BODY_HOME_URL",
				                                     (char *)ad->ed_hm_url_desc,
				                                     setting_network_connection_entry_changed_cb);
				if (ad->data_hm_url) {
					ad->data_hm_url->userdata = ad;
					ad->data_hm_url->input_type = ELM_INPUT_PANEL_LAYOUT_URL;
					ad->data_hm_url->limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
					if (ad->data_hm_url->limit_filter_data) {
						ad->data_hm_url->limit_filter_data->max_byte_count = MAX_HOME_URL_LEN_MAX - 1;
						ad->data_hm_url->win_main = ad->win_get;
					}
				} else {
					SETTING_TRACE_ERROR("ad->data_hm_url is NULL");
				}
			}
		} else {
			/*do need home-url */
			if (ad->data_hm_url) {
				elm_object_item_del(ad->data_hm_url->item);
				ad->data_hm_url = NULL;
			}
		}
	}
}

void __sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info)
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

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	/*elm_genlist_item_update(data_parentItem->item); */
	elm_genlist_item_fields_update(data_parentItem->item, "*", ELM_GENLIST_ITEM_FIELD_TEXT);

	SettingNetworkUG *ad = data_parentItem->userdata;
	if (data_parentItem == ad->data_auth_type) {
		elm_genlist_item_expanded_set(ad->data_auth_type->item, FALSE);
		elm_genlist_item_update(ad->data_auth_type->item);

		setting_network_connection_display_auth_type(data, data_subItem->chk_status);
	} else if (data_parentItem == ad->data_srv_type) {
		if (ad->srvType != data_subItem->chk_status)
			ad->srvType = data_subItem->chk_status;
		if (ad->srvType != CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
			if (!ad->data_hm_url) {
				/*need home url */
				ad->data_hm_url =
				    setting_create_Gendial_field_def(obj,
				                                     &itc_editfield,
				                                     NULL, NULL,
				                                     SWALLOW_Type_LAYOUT_EDITFIELD,
				                                     NULL, NULL, 0,
				                                     "IDS_ST_BODY_HOME_URL",
				                                     (char *)ad->ed_hm_url_desc,
				                                     setting_network_connection_entry_changed_cb);
				if (ad->data_hm_url) {
					ad->data_hm_url->userdata = ad;
					ad->data_hm_url->input_type = ELM_INPUT_PANEL_LAYOUT_URL;
					ad->data_hm_url->limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
					if (ad->data_hm_url->limit_filter_data) {
						ad->data_hm_url->limit_filter_data->max_byte_count = MAX_HOME_URL_LEN_MAX - 1;
						ad->data_hm_url->win_main = ad->win_get;
					}
				} else {
					SETTING_TRACE_ERROR("ad->data_hm_url is NULL");
				}
			}

		} else {
			/*do need home-url */
			if (ad->data_hm_url) {
				elm_object_item_del(ad->data_hm_url->item);
				ad->data_hm_url = NULL;
			}
		}
	}
}

static void __setting_network_connection_exp_cb(void *data, Evas_Object *obj,
                                                void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;

	setting_genlist_item_groupstyle_set(data_parentItem, SETTING_GROUP_STYLE_TOP);
	Setting_GenGroupItem_Data *list_item = NULL;
	if (data_parentItem == ad->data_auth_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_AUTH_TYPE_PAP,
		                                                 "IDS_ST_MBODY_PAP",
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_CENTER);

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_AUTH_TYPE_CHAP,
		                                                 "IDS_ST_POP_CHAP",
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_CENTER);

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_AUTH_TYPE_NONE,
		                                                 "IDS_ST_BODY_NONE",
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_BOTTOM);

		elm_radio_value_set(rgd, ad->chkType);
	} else if (data_parentItem == ad->data_srv_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET,
		                                                 "IDS_ST_BODY_INTERNET_CONNECTION",
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_CENTER);

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_SERVICE_TYPE_MMS,
		                                                 STR_SETTING_MMS_CONNECTIONS,
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_CENTER);

		int r = 0;
		char tmp_str[SETTING_STR_SLP_LEN + 1] = {0,};
		r = snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s %s", _(INTERNET), PLUS_MMS);
		if (r < 0)
			SETTING_TRACE("snprintf failed");

		list_item = setting_create_Gendial_exp_sub_field(scroller,
		                                                 &itc_1icon_1text_sub,
		                                                 __sub_list_sel_cb, ad,
		                                                 parentItem,
		                                                 SWALLOW_Type_1RADIO, rgd,
		                                                 CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION,
		                                                 tmp_str,
		                                                 __sub_list_rd_change);
		setting_genlist_item_groupstyle_set(list_item, SETTING_GROUP_STYLE_CENTER);

		elm_radio_value_set(rgd, ad->srvType);
	}
}

static Eina_Bool __connection_idler(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, FALSE);
	SettingNetworkUG *ad = data;
	/*__genlist_disable_set(ad->scl_edit, TRUE); */
	setting_network_connection_hide_input_pannel(ad);
	return FALSE;
}
void __get_connection_info(void *cb)
{
	SETTING_TRACE_BEGIN;
	ret_if(cb == NULL);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	/*int ret; */
	G_FREE(ad->ed_profile_name_desc);
	G_FREE(ad->ed_acs_name_desc);
	G_FREE(ad->ed_user_name_desc);
	G_FREE(ad->ed_pwd_desc);

	G_FREE(ad->ed_pxy_addr_desc);
	G_FREE(ad->ed_pxy_port_desc);

	G_FREE(ad->ed_hm_url_desc);
	ad->ed_auth_type_desc = NULL;
	ad->chkType = CONNECTION_CELLULAR_AUTH_TYPE_NONE;

	SETTING_TRACE("ad->con_name:%s, ad->profile_service_type:%d",
	              ad->con_name, ad->profile_service_type);

	int ServiceType = ad->profile_service_type;
	char *con_name_utf8 = NULL;
	if (ad->access_name) {
		con_name_utf8 = strdup(ad->access_name);
	} else {
		con_name_utf8 = strdup("");
	}
	SETTING_TRACE("con_name_utf8:%s", con_name_utf8);

	/*match with the correct profile */
	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;
	char *profile_name = NULL;
	char *apn = NULL;
	char *profile_id = NULL;
	connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
	int err = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (err != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", err);
		FREE(con_name_utf8);
		return;
	}

	int cnt = 0;
	bool found = FALSE;
	while (connection_profile_iterator_has_next(profile_iter)) {
		/*reset and release.. */
		G_FREE(apn);
		G_FREE(profile_id);
		G_FREE(profile_name);

		if (cnt >= MAX_PROFILE_NUM || connection_profile_iterator_next(profile_iter, &profile_h) != CONNECTION_ERROR_NONE) {
			break;
		}

		connection_profile_get_type(profile_h, &profile_type);
		connection_profile_get_cellular_service_type(profile_h, &service_type);
		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_id(profile_h, &profile_id);
		connection_profile_get_name(profile_h, &profile_name);
		SETTING_TRACE("Record[%d] apn[%s], profile_id[%s], profile_name[%s], profile_type[%d], service_type[%d]",
		              cnt, apn, profile_id, profile_name, profile_type, service_type);
		if (CONNECTION_PROFILE_TYPE_CELLULAR != profile_type)
			continue;

#if 0
		if (CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION != service_type) {
			/*match with ServiceType */
			if (ServiceType == service_type) {
				ad->sel_profile_h = profile_h;
				found = TRUE;
				break;
			}
		} else { /*the customer's profiles */
			if (0 == safeStrCmp(con_name_utf8, apn)) {
				ad->sel_profile_h = profile_h;
				found = TRUE;
				break;
			}
		}
#endif
		/*match with ServiceType */
		if (ServiceType == service_type) {
			if (ServiceType == CONNECTION_CELLULAR_SERVICE_TYPE_MMS) {

				/*for MMS,it just have one profile */
				ad->sel_profile_h = profile_h;
				found = TRUE;
				break;
			} else {
				if (0 == safeStrCmp(con_name_utf8, profile_id)) {
					ad->sel_profile_h = profile_h;
					found = TRUE;
					break;
				}
			}
		}

		cnt++;
	}
	FREE(con_name_utf8);
	ad->is_editable = TRUE;
	if (found) { /*found sucessfully */
		SETTING_TRACE("found the following record:");
		SETTING_TRACE("\tapn:%s", apn);
		SETTING_TRACE("\tprofile_id:%s", profile_id);
		SETTING_TRACE("\tprofile_name:%s", profile_name);
		SETTING_TRACE("\tservice_type:%d", service_type);
		SETTING_TRACE("\tprofile_type:%d", profile_type);
		ad->srvType = service_type;
		ad->ed_profile_name_desc = profile_name;

		connection_profile_is_cellular_editable(ad->sel_profile_h, &(ad->is_editable));
		SETTING_TRACE("ad->is_editable:%d", ad->is_editable);

		/* load access name */
		/*ad->ed_acs_name_desc = ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.Apn; */
		connection_profile_get_cellular_apn(ad->sel_profile_h, &(ad->ed_acs_name_desc));
		/*ad->ed_user_name_desc =
		    ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.AuthInfo.UserName;
		ad->chkType =
		    ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.AuthInfo.AuthType;
		ad->ed_pwd_desc =
		    ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.AuthInfo.Password;*/
		connection_cellular_auth_type_e auth_type = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
		connection_profile_get_cellular_auth_info(ad->sel_profile_h, &auth_type, &(ad->ed_user_name_desc), &(ad->ed_pwd_desc));
		ad->chkType = auth_type;

		switch (ad->chkType) {
			case CONNECTION_CELLULAR_AUTH_TYPE_PAP:
				ad->ed_auth_type_desc = _("IDS_ST_MBODY_PAP");
				break;
			case CONNECTION_CELLULAR_AUTH_TYPE_CHAP:
				ad->ed_auth_type_desc = _("IDS_ST_POP_CHAP");
				break;
			case CONNECTION_CELLULAR_AUTH_TYPE_NONE:
				ad->ed_auth_type_desc = _("IDS_ST_BODY_NONE");
				break;
			default:
				ad->chkType = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
				ad->ed_auth_type_desc = _("IDS_ST_BODY_NONE");
				break;
		}


		char *full_addr = NULL;
		char *addr = NULL;
		char *port = NULL;
		/*Fix the Port 'NULL' issue when enter connection view repeatly.. */
		connection_profile_get_proxy_address(ad->sel_profile_h, CONNECTION_ADDRESS_FAMILY_IPV4, &full_addr);
		SETTING_TRACE("full_addr: %s", full_addr);
		char ProxyAddr[MAX_HOME_URL_LEN_MAX + 1] = {0,};
		safeCopyStr(ProxyAddr, full_addr, MAX_HOME_URL_LEN_MAX);
		G_FREE(full_addr);

		/*several real format after removing "http://": */
		/*	":80", "1.1.1.1:80","1.1.1.1: ", "127.0.0.1/", "127.0.0.1" */
		full_addr = ProxyAddr;
		int url_head_len = strlen(URL_HEAD);
		if (0 == safeStrNCmp(full_addr, URL_HEAD, url_head_len)) {
			full_addr += url_head_len;
		}
		addr = strsep(&full_addr, ":");
		if (NULL == full_addr) { /*format like "http://127.0.0.1/" or "http://127.0.0.1" */
			SETTING_TRACE("NULL == full_addr");
			addr = strsep(&addr, "/");
		}

		ad->ed_pxy_addr_desc = NULL;
		/*proxy address support not only ip but also url, need not check the format */
		/*bool is_ip = is_ip_string(addr, &tmp); */
		if (!isEmptyStr(addr)) {
			ad->ed_pxy_addr_desc = g_strdup(addr);
			port = strsep(&full_addr, "/");
		} else { /*invalid format,let addr and port to be NULL. */
			addr = NULL;
		}
		SETTING_TRACE("addr:%s", addr);
		SETTING_TRACE("port:%s", port);
		SETTING_TRACE("ad->ed_pxy_addr_desc:%s", ad->ed_pxy_addr_desc);
		/*ad->ed_pxy_addr_desc = g_strdup(addr); */
		ad->ed_pxy_port_desc = g_strdup(port);
		/*if (ad->con_type == NET_SERVICE_MMS) {	/* Message Connection * / */
		if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {	/* Message Connection */
			/*ad->ed_hm_url_desc = ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.HomeURL; */
			connection_profile_get_cellular_home_url(ad->sel_profile_h, &(ad->ed_hm_url_desc));
		}
	} else {
		SETTING_TRACE("Didn't find any record, to let add a new one:");
		ad->sel_profile_h = NULL;
		ad->ed_auth_type_desc = _("IDS_ST_BODY_NONE");
		ad->chkType = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
		ad->srvType = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
	}

	/*release the memory.. */
	G_FREE(apn);
	G_FREE(profile_id);
	/*G_FREE(profile_name); */
	return;
}
void __update_genlist_info(void *cb)
{
	SETTING_TRACE_BEGIN;
	ret_if(cb == NULL);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (ad->data_profile_name) {
		ad->data_profile_name->sub_desc = g_strdup(ad->ed_profile_name_desc);
	}

	if (ad->data_acs_name) {
		ad->data_acs_name->sub_desc = g_strdup(ad->ed_acs_name_desc);
	}
	if (ad->data_srv_type) {
		switch (ad->srvType) {
			case CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET:
				ad->data_srv_type->sub_desc = g_strdup(_("IDS_ST_BODY_INTERNET_CONNECTION"));
				break;
			case CONNECTION_CELLULAR_SERVICE_TYPE_MMS:
				ad->data_srv_type->sub_desc = g_strdup(_(STR_SETTING_MMS_CONNECTIONS));
				break;
			case CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION: {
					int r = 0;
					char tmp_str[SETTING_STR_SLP_LEN + 1] = {0,};
					r = snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s %s", _(INTERNET), PLUS_MMS);
					if (r < 0)
						SETTING_TRACE("snprintf failed");
					ad->data_srv_type->sub_desc = g_strdup(tmp_str);
					break;
				}
			default:
				ad->data_srv_type->sub_desc = NULL;
		}
	}

	if (ad->data_auth_type) {
		ad->data_auth_type->sub_desc = g_strdup(ad->ed_auth_type_desc);
		if (ad->sel_profile_h) {
			/*int authType = ad->prof_list[ad->prof_sel_idx].ProfileInfo.Pdp.AuthInfo.AuthType; */
			connection_cellular_auth_type_e authType = 0;
			char *user_name;
			char *pwd;
			connection_profile_get_cellular_auth_info(ad->sel_profile_h, &authType, &user_name, &pwd);
			ad->chkType = !ad->chkType;
			setting_network_connection_display_auth_type(ad, authType);
			FREE(user_name);
			FREE(pwd);
		}
	}
	if (ad->data_user_name) {
		ad->data_user_name->sub_desc = g_strdup(ad->ed_user_name_desc);
	}
	if (ad->data_pwd) {
		ad->data_pwd->sub_desc = g_strdup(ad->ed_pwd_desc);
	}

	if (ad->data_pxy_addr) {
		ad->data_pxy_addr->sub_desc = g_strdup(ad->ed_pxy_addr_desc);
	}
	if (ad->data_pxy_port) {
		ad->data_pxy_port->sub_desc = g_strdup(ad->ed_pxy_port_desc);
	}
	if (ad->data_hm_url) {
		ad->data_hm_url->sub_desc = g_strdup(ad->ed_hm_url_desc);
	}
	elm_genlist_realized_items_update(ad->scl_edit);
	return;
}

static void __network_max_len_reached(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");/*notify only when entry is being focused on. */

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	list_item->maxLengthReachFlag = true;
}

/*
 * Entry - activated callback (When 'Next' key in SIP touched)
 */
static void setting_network_connection_entry_next_activated_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data	*list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG			*ad = list_item->userdata;

	Evas_Object		*pObj_NextEntry = NULL;

	if (list_item == ad->data_profile_name) {
		/* Profile name -> Access name */
		if (ad->data_acs_name)
			pObj_NextEntry = ad->data_acs_name->eo_check;
	} else if (list_item == ad->data_user_name) {
		/* User name -> Password */
		if (ad->data_pwd)
			pObj_NextEntry = ad->data_pwd->eo_check;
	} else if (list_item == ad->data_pwd) {
		/* Password -> Proxy address */
		if (ad->data_pxy_addr)
			pObj_NextEntry = ad->data_pxy_addr->eo_check;
	} else if (list_item == ad->data_pxy_addr) {
		/* Proxy address -> Proxy port */
		if (ad->data_pxy_port)
			pObj_NextEntry = ad->data_pxy_port->eo_check;
	} else if (list_item == ad->data_pxy_port) {
		/* Proxy port -> Home URL */
		if (ad->data_hm_url)
			pObj_NextEntry = ad->data_hm_url->eo_check;
	} else {
		SETTING_TRACE_ERROR("list_item unknown");
		return;
	}

	/* Hide & unfocus keypad */
	setting_hide_input_pannel_cb(list_item->eo_check);

	/* Focus */
	if (pObj_NextEntry != NULL) {
		elm_object_focus_set(pObj_NextEntry, EINA_TRUE);
		elm_entry_cursor_end_set(pObj_NextEntry);
	}

	return;
}

static int setting_network_connection_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	setting_retvm_if(!ad->con_name, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "!ad->con_name")

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
	         "Cannot set scroller object  as contento of layout");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	elm_genlist_clear(scroller);	/* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	/* connection intializes */
	ad->data_profile_name = NULL;
	ad->data_acs_name = NULL;
	ad->data_auth_type = NULL;
	ad->data_user_name = NULL;
	ad->data_pwd = NULL;
	ad->data_pxy_addr = NULL;
	ad->data_pxy_port = NULL;
	ad->data_hm_url = NULL;

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
	ad->navi_it = setting_push_layout_navi_bar(_(title),
	                                           _("IDS_ST_BUTTON_BACK"),
	                                           _("IDS_ST_BODY_SAVE"),
	                                           _("IDS_ST_BUTTON_CANCEL_ABB"),
	                                           NULL,/*setting_network_connection_click_softkey_back_cb2, */
	                                           setting_network_connection_click_softkey_save_cb,
	                                           setting_network_connection_click_softkey_cancel_cb, /* setting_network_connection_click_softkey_back_cb, */
	                                           ad, scroller, ad->navi_bar, NULL);

	elm_naviframe_item_pop_cb_set(ad->navi_it, setting_network_connection_click_softkey_back_cb, ad);

	/*Apply new SIP concept */
	evas_object_data_set(ad->navi_bar, "sip.naviframe.title_obj", "*");
	/*setting_append_naviframe_title_buttons(ad->navi_it, ad->navi_bar,
					       NULL, NULL, _("IDS_SA_BUTTON_DONE_ABB") ,NULL,
					       NULL,
					       NULL,
					       setting_network_connection_click_softkey_done_cb,
					       setting_network_connection_click_softkey_cancel_cb,
					       ad);*/

	retv_if(!ad->navi_it, SETTING_RETURN_FAIL);
	ad->has_form_changed  = FALSE;

	setting_enable_expandable_genlist(scroller, ad,
	                                  __setting_network_connection_exp_cb,
	                                  NULL);
	/* Do not hide input panel while drag the genlist
	evas_object_event_callback_add(scroller, EVAS_CALLBACK_MOUSE_UP,
				       __connection_gl_mouse_up, ad);
	*/
	__get_connection_info(ad);
	Elm_Object_Item *item;

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	bool new_flag = FALSE;
	if (!safeStrCmp(STR_SETTING_NEW_CONNECTIONS, ad->con_name)) { /*new a profile,need to select the "Service type" */
		new_flag = TRUE;
		ad->data_srv_type =
		    setting_create_Gendial_exp_parent_field(scroller,
		                                            &itc_2text_3_parent,
		                                            NULL, NULL,
		                                            SWALLOW_Type_INVALID,
		                                            _("IDS_MSGF_BODY_SERVICE_TYPE"),
		                                            _("IDS_ST_BODY_INTERNET_CONNECTION"),
		                                            SETTING_GROUP_STYLE_TOP,
		                                            SETTING_GROUP_STYLE_TOP);
		/* ad->data_auth_type->int_slp_setting_binded = INT_SLP_SETTING_INVALID; */
		if (ad->data_srv_type) {
			ad->data_srv_type->userdata = ad;
			__BACK_POINTER_SET(ad->data_srv_type);
		} else {
			SETTING_TRACE_ERROR("ad->data_auth_type is NULL");
		}
	}

	ad->ed_profile_name_desc = elm_entry_utf8_to_markup(ad->ed_profile_name_desc);
	ad->ed_acs_name_desc = elm_entry_utf8_to_markup(ad->ed_acs_name_desc);
	SETTING_TRACE("name :%s", ad->ed_profile_name_desc);
	SETTING_TRACE("apn :%s", ad->ed_acs_name_desc);

	/* Profile name */
	if (!isEmptyStr(ad->ed_profile_name_desc)) {
		/* Display */
		ad->data_profile_name =
		    setting_create_Gendial_field_groupitem(scroller,
		                                           &itc_multiline_2text,
		                                           NULL,
		                                           NULL,
		                                           ad,
		                                           SWALLOW_Type_INVALID,
		                                           NULL,
		                                           NULL,
		                                           0, /* chk_status*/
		                                           "IDS_ST_BODY_PROFILE_NAME",
		                                           (char *)ad->ed_profile_name_desc, /* sub_desk*/
		                                           NULL);
	} else {
		/* Editor */
		ad->data_profile_name =
		    setting_create_Gendial_field_entry_with_return_key(scroller,
		                                                       &itc_editfield, NULL,
		                                                       ad, SWALLOW_Type_LAYOUT_EDITFIELD,
		                                                       NULL, NULL, 0,
		                                                       "IDS_ST_BODY_PROFILE_NAME",
		                                                       (char *)ad->ed_profile_name_desc,
		                                                       setting_network_connection_entry_changed_cb,
		                                                       __network_max_len_reached,
		                                                       ELM_INPUT_PANEL_LAYOUT_NORMAL,
		                                                       FALSE, FALSE,
		                                                       0, 64, NULL, NULL, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT, setting_network_connection_entry_next_activated_cb, NULL);
	}

	if (ad->data_profile_name) {
		if (new_flag)
			setting_genlist_item_groupstyle_set(ad->data_profile_name, SETTING_GROUP_STYLE_CENTER);
		else
			setting_genlist_item_groupstyle_set(ad->data_profile_name, SETTING_GROUP_STYLE_TOP);
		__BACK_POINTER_SET(ad->data_profile_name);
		ad->data_profile_name->userdata = ad;
		ad->data_profile_name->win_main = ad->win_get;
		/*there is no CAPI to set profile name,so disable it: */
		/*setting_disable_genlist_item(ad->data_profile_name->item); */
		if (!isEmptyStr(ad->ed_profile_name_desc)) { /*not create(first time) */
			elm_genlist_item_select_mode_set(ad->data_profile_name->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

			/*setting_disable_genlist_item(ad->data_profile_name->item); */
		}
	} else {
		SETTING_TRACE_ERROR("ad->data_acs_name is NULL");
	}

	/* Access point name */
	ad->data_acs_name =
	    setting_create_Gendial_field_entry_with_return_key(scroller,
	                                                       &itc_editfield, NULL,
	                                                       ad, SWALLOW_Type_LAYOUT_EDITFIELD,
	                                                       NULL, NULL, 0,
	                                                       "IDS_DLNA_BODY_ACCESS_POINT_NAME",
	                                                       (char *)ad->ed_acs_name_desc,
	                                                       setting_network_connection_entry_changed_cb,
	                                                       __network_max_len_reached,
	                                                       ELM_INPUT_PANEL_LAYOUT_NORMAL,
	                                                       FALSE, FALSE,
	                                                       0, 64, NULL, NULL, ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE, NULL, NULL);
	if (ad->data_acs_name) {
		setting_genlist_item_groupstyle_set(ad->data_acs_name, SETTING_GROUP_STYLE_BOTTOM);
		__BACK_POINTER_SET(ad->data_acs_name);
		ad->data_acs_name->userdata = ad;
		ad->data_acs_name->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_acs_name is NULL");
	}

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->data_auth_type =
	    setting_create_Gendial_exp_parent_field(scroller,
	                                            &itc_2text_3_parent,
	                                            NULL, NULL,
	                                            SWALLOW_Type_INVALID,
	                                            "IDS_ST_BODY_AUTH_TYPE",
	                                            (char *)ad->ed_auth_type_desc,
	                                            SETTING_GROUP_STYLE_TOP, SETTING_GROUP_STYLE_NONE);
	/* ad->data_auth_type->int_slp_setting_binded = INT_SLP_SETTING_INVALID; */
	if (ad->data_auth_type) {
		ad->data_auth_type->userdata = ad;
		__BACK_POINTER_SET(ad->data_auth_type);
	} else {
		SETTING_TRACE_ERROR("ad->data_auth_type is NULL");
	}

	ad->item_above_user_name = item =
	                               elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                                                       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->is_show_user = 0;
	if (CONNECTION_CELLULAR_AUTH_TYPE_NONE != ad->chkType) {
		/* User ID */
		ad->data_user_name =
		    setting_create_Gendial_field_entry_with_return_key(scroller,
		                                                       &itc_editfield,
		                                                       NULL, ad,
		                                                       SWALLOW_Type_LAYOUT_EDITFIELD,
		                                                       NULL, NULL, 0,
		                                                       KeyStr_UserID,
		                                                       (char *)
		                                                       ad->ed_user_name_desc,
		                                                       setting_network_connection_entry_changed_cb,
		                                                       __network_max_len_reached,
		                                                       ELM_INPUT_PANEL_LAYOUT_NORMAL,
		                                                       FALSE, FALSE,
		                                                       0, MAX_PDP_AUTH_USERNAME_LEN_MAX,
		                                                       NULL, NULL, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT, setting_network_connection_entry_next_activated_cb, NULL);
		if (ad->data_user_name) {
			__BACK_POINTER_SET(ad->data_user_name);
			setting_genlist_item_groupstyle_set(ad->data_user_name, SETTING_GROUP_STYLE_TOP);
			ad->data_user_name->userdata = ad;
			ad->data_user_name->win_main = ad->win_get;
		} else {
			SETTING_TRACE_ERROR("ad->data_user_name is NULL");
		}

		/* Password */
		ad->data_pwd =
		    setting_create_Gendial_field_entry_with_return_key(scroller,
		                                                       &itc_editfield,
		                                                       NULL, ad,
		                                                       SWALLOW_Type_LAYOUT_EDITFIELD,
		                                                       NULL, NULL, 0,
		                                                       "IDS_ST_BODY_PASSWORD",
		                                                       (char *)ad->ed_pwd_desc,
		                                                       setting_network_connection_entry_changed_cb,
		                                                       __network_max_len_reached,
		                                                       ELM_INPUT_PANEL_LAYOUT_NORMAL,
		                                                       TRUE, FALSE,
		                                                       0, MAX_PDP_AUTH_USERNAME_LEN_MAX,
		                                                       NULL, NULL, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT, setting_network_connection_entry_next_activated_cb, NULL);
		if (ad->data_pwd) {
			__BACK_POINTER_SET(ad->data_pwd);
			setting_genlist_item_groupstyle_set(ad->data_pwd, SETTING_GROUP_STYLE_BOTTOM);
			ad->data_pwd->userdata = ad;
			ad->data_pwd->win_main = ad->win_get;
			ad->is_show_user = 1;
		} else {
			SETTING_TRACE_ERROR("ad->data_pwd is NULL");
		}

		ad->item_above_proxy_add = item =
		                               elm_genlist_item_append(scroller, &itc_seperator,
		                                                       NULL, NULL, ELM_GENLIST_ITEM_NONE,
		                                                       NULL, NULL);
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	/* Proxy address */
	ad->data_pxy_addr =
	    setting_create_Gendial_field_entry_with_return_key(scroller,
	                                                       &itc_editfield, NULL,
	                                                       ad, SWALLOW_Type_LAYOUT_EDITFIELD,
	                                                       NULL, NULL, 0,
	                                                       "IDS_ST_BODY_CONNECTIONSETTINGS_PROXY_ADDRESS",
	                                                       (char *)ad->ed_pxy_addr_desc,
	                                                       setting_network_connection_entry_changed_cb,
	                                                       __network_max_len_reached,
	                                                       ELM_INPUT_PANEL_LAYOUT_URL,
	                                                       FALSE, FALSE,
	                                                       0, 512, NULL, NULL, ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT, setting_network_connection_entry_next_activated_cb, NULL);
	if (ad->data_pxy_addr) {
		setting_genlist_item_groupstyle_set(ad->data_pxy_addr, SETTING_GROUP_STYLE_TOP);
		__BACK_POINTER_SET(ad->data_pxy_addr);
		ad->data_pxy_addr->userdata = ad;
		ad->data_pxy_addr->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_pxy_addr is NULL");
	}

	/* Set SIP key on Next or Done if Home URL existed or not below. */
	Elm_Input_Panel_Return_Key_Type type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
	setting_call_back_func cbFunc = NULL;

	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
		cbFunc = setting_network_connection_entry_next_activated_cb;
	}

	/* Proxy port */
	ad->data_pxy_port =
	    setting_create_Gendial_field_entry_with_return_key(scroller,
	                                                       &itc_editfield, NULL,
	                                                       ad, SWALLOW_Type_LAYOUT_EDITFIELD,
	                                                       NULL, NULL, 0,
	                                                       "IDS_ST_BODY_PROXY_PORT",
	                                                       (char *)ad->ed_pxy_port_desc,
	                                                       setting_network_connection_entry_changed_cb,
	                                                       __network_max_len_reached,
	                                                       ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
	                                                       FALSE, FALSE,
	                                                       0,
	                                                       5, "0123456789", NULL, type, cbFunc, NULL);
	if (ad->data_pxy_port) {
		ad->data_pxy_port->userdata = ad;
		setting_genlist_item_groupstyle_set(ad->data_pxy_port, SETTING_GROUP_STYLE_BOTTOM);
		__BACK_POINTER_SET(ad->data_pxy_port);
		ad->data_pxy_port->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_pxy_port is NULL");
	}

	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {	/*  mms Connection */
		if (ad->data_pxy_port) {
			setting_genlist_item_groupstyle_set(ad->data_pxy_port, SETTING_GROUP_STYLE_CENTER);
			ad->data_pxy_port->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT;
		}

		/* Home URL */
		ad->data_hm_url =
		    setting_create_Gendial_field_entry_with_return_key(scroller,
		                                                       &itc_editfield,
		                                                       NULL, ad,
		                                                       SWALLOW_Type_LAYOUT_EDITFIELD,
		                                                       NULL, NULL, 0,
		                                                       "IDS_ST_BODY_HOME_URL",
		                                                       (char *)ad->ed_hm_url_desc,
		                                                       setting_network_connection_entry_changed_cb,
		                                                       __network_max_len_reached,
		                                                       ELM_INPUT_PANEL_LAYOUT_URL,
		                                                       FALSE, FALSE,
		                                                       0, 521, NULL, NULL,
		                                                       ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE, NULL, NULL);
		if (ad->data_hm_url) {
			setting_genlist_item_groupstyle_set(ad->data_hm_url, SETTING_GROUP_STYLE_BOTTOM);
			__BACK_POINTER_SET(ad->data_hm_url);
			ad->data_hm_url->userdata = ad;
			ad->data_hm_url->win_main = ad->win_get;
			ad->data_hm_url->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
		} else {
			SETTING_TRACE_ERROR("ad->data_hm_url is NULL");
		}
		/* ad->is_show_url = 1; */
	}

#if SUPPORT_TETHERING
	is_tethering_enabled(ad);
#endif
	setting_view_network_connection_create.is_create = 1;
	ad->scl_edit = scroller;
	/*__genlist_disable_set(ad->scl_edit, TRUE); */
	/*ad->is_editable = FALSE; for test */
	SETTING_TRACE("==> ad->is_editable:%d", ad->is_editable);
	if (!ad->is_editable) {
		/*SETTING_TRACE("xxxx....."); */
		if (ad->data_srv_type) setting_disable_genlist_item(ad->data_srv_type->item);
		if (ad->data_profile_name) setting_disable_genlist_item(ad->data_profile_name->item);
		if (ad->data_acs_name) setting_disable_genlist_item(ad->data_acs_name->item);
		if (ad->data_auth_type) setting_disable_genlist_item(ad->data_auth_type->item);


		if (ad->data_user_name) setting_disable_genlist_item(ad->data_user_name->item);
		if (ad->data_pwd) setting_disable_genlist_item(ad->data_pwd->item);
		if (ad->data_pxy_addr) setting_disable_genlist_item(ad->data_pxy_addr->item);
		if (ad->data_pxy_port) setting_disable_genlist_item(ad->data_pxy_port->item);
		if (ad->data_hm_url) setting_disable_genlist_item(ad->data_hm_url->item);
	}
	ecore_idler_add(__connection_idler, ad);

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_connection_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	evas_object_data_set(ad->navi_bar, "sip.naviframe.title_obj", NULL);
	ad->navi_it = NULL;
	ad->bottom_btn = NULL;
	ad->back_btn = NULL;
	/*manager separatly, */
	if (ad->l_button) {
		evas_object_del(ad->l_button);
		ad->l_button = NULL;
	}
	/*if (ad->m_button)
	{
		evas_object_del(ad->m_button);
		ad->m_button = NULL;
	}*/
	if (ad->r_button) {
		evas_object_del(ad->r_button);
		ad->r_button = NULL;
	}
	/*FREE(ad->con_name); */
	G_FREE(ad->ed_profile_name_desc);
	G_FREE(ad->ed_acs_name_desc);
	G_FREE(ad->ed_user_name_desc);
	G_FREE(ad->ed_pwd_desc);

	G_FREE(ad->ed_pxy_addr_desc);
	G_FREE(ad->ed_pxy_port_desc);

	G_FREE(ad->ed_hm_url_desc);

#if SUPPORT_TETHERING
	if (ad->th_concreate) {
		SETTING_TRACE("tethering destruction");
		tethering_destroy(ad->th_concreate);
		ad->th_concreate = NULL;
	}
#endif

	/*elm_naviframe_item_pop(ad->navi_bar); */

	setting_view_network_connection_create.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_connection_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_connection_cleanup(void *cb)
{
	return setting_network_connection_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
bool need_check_default_profile(void *data, connection_cellular_service_type_e inputtype)
{
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("inputtype:%d", inputtype);
	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;
	int rv = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
		return SETTING_RETURN_FAIL;
	}

	int cnt = 0;
	connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
	while (connection_profile_iterator_has_next(profile_iter)) {
		if (cnt >= MAX_PROFILE_NUM) {
			break;
		}
		cnt++;
		if (connection_profile_iterator_next(profile_iter, &profile_h) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile handle");
			break;
		}

		if (connection_profile_get_type(profile_h, &profile_type) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile type");
			continue;
		}
		if (connection_profile_get_cellular_service_type(profile_h, &service_type) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get service type");
			continue;
		}
		if (CONNECTION_PROFILE_TYPE_CELLULAR == profile_type && inputtype == service_type) {
			char *apn = NULL;
			connection_profile_get_cellular_apn(profile_h, &apn);
			SETTING_TRACE("Matched:%s", apn);
			FREE(apn);
			return FALSE;
		}
	}
	return TRUE;
}

static int __save_connection(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	const char *profile_name = NULL;
	const char *acs_name = NULL;
	const char *usr_name = NULL;
	const char *pwd = NULL;
	const char *addr = NULL;
	const char *port = NULL;
	const char *home = NULL;

	if (ad->data_profile_name)
		profile_name = elm_entry_markup_to_utf8(ad->data_profile_name->sub_desc);
	if (ad->data_acs_name)
		acs_name = elm_entry_markup_to_utf8(ad->data_acs_name->sub_desc);
	if (ad->data_user_name)
		usr_name = elm_entry_markup_to_utf8(ad->data_user_name->sub_desc);
	if (ad->data_pwd)
		pwd = elm_entry_markup_to_utf8(ad->data_pwd->sub_desc);
	if (ad->data_pxy_addr)
		addr = elm_entry_markup_to_utf8(ad->data_pxy_addr->sub_desc);
	if (ad->data_pxy_port)
		port = elm_entry_markup_to_utf8(ad->data_pxy_port->sub_desc);

	if (!profile_name)
		profile_name = strdup("");
	if (!acs_name)
		acs_name = strdup("");
	if (!usr_name)
		usr_name = strdup("");
	if (!pwd)
		pwd = strdup("");
	if (!addr)
		addr = strdup("");
	if (!port)
		port = strdup("");
	SETTING_TRACE("acs_name:%s", acs_name);
	SETTING_TRACE("usr_name:%s", usr_name);
	SETTING_TRACE("pwd:%s", pwd);

	SETTING_TRACE("addr:%s", addr);
	SETTING_TRACE("port:%s", port);
	SETTING_TRACE("home:%s", home);

	char sz_text[MAX_HOME_URL_LEN_MAX + 1] = { 0, };
	int type;
	int ret = SETTING_RETURN_SUCCESS;
	int err;

	if (!safeStrCmp(profile_name, "")) {

		char name[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
		snprintf(name, sizeof(name), _("IDS_ST_POP_ENTER_PS"), _("IDS_ST_BODY_PROFILE_NAME"));
		setting_create_simple_popup(ad, ad->win_get, NULL, _(name));
		FREE(acs_name);
		FREE(profile_name);
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_ACCESS_NAME;
	}

	if (!safeStrCmp(acs_name, "")) {

		char name[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
		snprintf(name, sizeof(name), _("IDS_ST_POP_ENTER_PS"), _("IDS_DLNA_BODY_ACCESS_POINT_NAME"));
		setting_create_simple_popup(ad, ad->win_get, NULL, _(name));
		FREE(acs_name);
		FREE(profile_name);
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_ACCESS_NAME;
	}
	/*if (test_update_cellular_info(profile) == -1) */
	bool add_flag = FALSE;/*add a new profile */
	connection_profile_h con_info = NULL;
	connection_profile_h con_info_2 = NULL;/*for MMS */
	if (ad->sel_profile_h) {
		con_info = ad->sel_profile_h;
		if (0 != safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)
		    && 0 != safeStrCmp(ad->con_name, "IDS_ST_BODY_INTERNET_CONNECTION")) {
			/*need to change con_name */
			/*FREE(ad->con_name); */
			/*ad->con_name = strdup(acs_name); */
		}
	} else {
		if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
			(void)connection_profile_create(CONNECTION_PROFILE_TYPE_CELLULAR, _(profile_name), &con_info);
			(void)connection_profile_set_cellular_service_type(con_info, CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
		} else if (!safeStrCmp(ad->con_name, "IDS_ST_BODY_INTERNET_CONNECTION")) {
			(void)connection_profile_create(CONNECTION_PROFILE_TYPE_CELLULAR, _(profile_name), &con_info);
			(void)connection_profile_set_cellular_service_type(con_info, CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
		} else {
			/*need to change con_name */
			/*FREE(ad->con_name); */
			/*ad->con_name = strdup(acs_name); */
			(void)connection_profile_create(CONNECTION_PROFILE_TYPE_CELLULAR, _(profile_name), &con_info);
			if (CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET == ad->srvType) {
				/*Internet */
				(void)connection_profile_set_cellular_service_type(con_info, CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
			} else if (CONNECTION_CELLULAR_SERVICE_TYPE_MMS == ad->srvType) {
				/*MMS */
				(void)connection_profile_set_cellular_service_type(con_info, CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
			} else { /*MMS + Internet(Together) */
				(void)connection_profile_set_cellular_service_type(con_info, CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
				(void)connection_profile_create(CONNECTION_PROFILE_TYPE_CELLULAR, _(profile_name), &con_info_2);
				(void)connection_profile_set_cellular_service_type(con_info_2, CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
				SETTING_TRACE("con_info_2:%p", con_info_2);
			}
		}

		add_flag = TRUE;
	}

	SETTING_TRACE("ad->con_name:%s", ad->con_name);
	SETTING_TRACE("profile_name:%s", profile_name);
	FREE(profile_name);
	/* save access name */
	SETTING_TRACE("acs_name:%s", acs_name);

	(void) connection_profile_set_cellular_apn(con_info, acs_name);
	if (con_info_2)(void)connection_profile_set_cellular_apn(con_info_2, acs_name);
	FREE(acs_name);

	/* save auth type */
	/* type = elm_radio_value_get(ad->chk_type); */
	type = ad->chkType;

#ifdef OLD_AUTH_CONCEPT
	/* save user name */
	if (!safeStrCmp(usr_name, "")
	    && CONNECTION_CELLULAR_AUTH_TYPE_NONE != type) {
		setting_create_simple_popup(ad, ad->win_get,
		                            NULL, _(Insert_User_Name_Desc));
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_USER_NAME;
	}

	/* save password */
	if (!safeStrCmp(pwd, "")
	    && CONNECTION_CELLULAR_AUTH_TYPE_NONE != type) {
		setting_create_simple_popup(ad, ad->win_get,
		                            NULL, _(""));/*this code is not used now, so remove the ID which is not used in po file*/
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_PASSWORD;
	}
#endif
	(void)connection_profile_set_cellular_auth_info(con_info, type, usr_name, pwd);
	if (con_info_2)(void)connection_profile_set_cellular_auth_info(con_info_2, type, usr_name, pwd);

	/* save proxy addr */
	SETTING_TRACE("addr:port[%s:%s]", addr, port);
	char *output_addr = NULL;
	if (isEmptyStr(addr)) {
		/* check proxy port */
		if (!isEmptyStr(port)) {
			setting_create_simple_popup(ad, ad->win_get,
			                            NULL, _("IDS_COM_BODY_ENTER_PROXY_EMPTY"));
			FREE(usr_name);
			FREE(pwd);
			FREE(addr);
			FREE(port);
			return SETTING_DNET_RETURN_INVALID_PROXY_ADDR;
		}
	} else {
		output_addr = g_strdup(addr);
		if (isEmptyStr(port)) {
			snprintf(sz_text, sizeof(sz_text), "%s", output_addr);
		} else {
			snprintf(sz_text, sizeof(sz_text), "%s:%s", output_addr, port);
		}
	}

	G_FREE(output_addr);
	SETTING_TRACE("sz_text:%s", sz_text);
	if (!isEmptyStr(sz_text)) {
		connection_profile_set_proxy_address(con_info, CONNECTION_ADDRESS_FAMILY_IPV4, sz_text);
		if (con_info_2) {
			connection_profile_set_proxy_address(con_info_2, CONNECTION_ADDRESS_FAMILY_IPV4, sz_text);
		}
	} else {
		connection_profile_set_proxy_address(con_info, CONNECTION_ADDRESS_FAMILY_IPV4, NULL);
		if (con_info_2) {
			connection_profile_set_proxy_address(con_info_2, CONNECTION_ADDRESS_FAMILY_IPV4, NULL);
		}
	}
	/* save home url(message connection only) */
	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)
	    || (!safeStrCmp(ad->con_name, STR_SETTING_NEW_CONNECTIONS)
	        && CONNECTION_CELLULAR_SERVICE_TYPE_MMS == ad->srvType)) {	/* Message Connection */
		home = elm_entry_markup_to_utf8(ad->data_hm_url->sub_desc);
		if (home == (char *)NULL) {
			(void)connection_profile_set_cellular_home_url(con_info, URL_HEAD);
		} else {
			if (!safeStrCmp(home, "")) {
				FREE(home);
				home = strdup(URL_HEAD);
				ret = SETTING_DNET_RETURN_NULL_HOMEURL;
			}
			(void)connection_profile_set_cellular_home_url(con_info, home);
		}
	}
	FREE(home);
	if (con_info_2) { /*creating both internet + MMS */
		home = elm_entry_markup_to_utf8(ad->data_hm_url->sub_desc);
		if (home == (char *)NULL) {
			(void)connection_profile_set_cellular_home_url(con_info_2, URL_HEAD);
		} else {
			if (!safeStrCmp(home, "")) {
				FREE(home);
				home = strdup(URL_HEAD);
				ret = SETTING_DNET_RETURN_NULL_HOMEURL;
			}
			(void)connection_profile_set_cellular_home_url(con_info_2, home);
		}
	}

	FREE(usr_name);
	FREE(pwd);
	FREE(addr);
	FREE(port);
	FREE(home);
	/*SETTING_TRACE("ad->con_type:%d", ad->con_type); */
	/* save connection */
	if (add_flag) {
		SETTING_TRACE("con_info is %p, con_info_2:%p", con_info, con_info_2);

		/*Add a new profile */
		connection_cellular_service_type_e type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
		(void)connection_profile_get_cellular_service_type(con_info, &type);

		err = connection_add_profile(ad->connection, con_info);
		if (err != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("*** [ERR] connection_add_profile. err=%d ***", err);
			ret = SETTING_DNET_RETURN_ERR;
			setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_CST_POP_FAILED"));
			return ret;
		}
		connection_profile_h tmp_profile = NULL;
		(void)connection_get_default_cellular_service_profile(ad->connection, type, &tmp_profile);
		/*
		=> if there is already default profile, after_profile will be handle of default profile.
		=> there is no default profile, after_profile will be handle of the profile which we have added just before.
		*/
		if (setting_network_equal_profile(con_info, tmp_profile)) {
			SETTING_TRACE_ERROR("Going to set profile")
			(void)connection_set_default_cellular_service_profile(ad->connection, type, tmp_profile);
		} else {
			/*nothing to do */
		}

		(void)connection_destroy(tmp_profile);

		(void)connection_profile_destroy(con_info);



		if (con_info_2) { /*just for MMS + internet */
			connection_cellular_service_type_e type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
			(void)connection_profile_get_cellular_service_type(con_info_2, &type);
			/*Add a new profile */
			err = connection_add_profile(ad->connection, con_info_2);
			if (err != CONNECTION_ERROR_NONE) {
				SETTING_TRACE_ERROR("*** [ERR] connection_add_profile. err=%d ***", err);
				ret = SETTING_DNET_RETURN_ERR;
				setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_CST_POP_FAILED"));
				return ret;
			}
			connection_profile_h tmp_profile = NULL;
			(void)connection_get_default_cellular_service_profile(ad->connection, type, &tmp_profile);
			/*
			=> if there is already default profile, after_profile will be handle of default profile.
			=> there is no default profile, after_profile will be handle of the profile which we have added just before.
			*/
			if (setting_network_equal_profile(con_info_2, tmp_profile)) {
				SETTING_TRACE_ERROR("Going to set profile")
				(void)connection_set_default_cellular_service_profile(ad->connection, type, tmp_profile);
			} else {
				/*nothing to do */
			}

			(void)connection_destroy(tmp_profile);
			(void)connection_profile_destroy(con_info_2);
		}
	} else {

		err = connection_update_profile(ad->connection, con_info);
		if (err != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR
			("%s*** [ERR] net_modify_profile. err=%d ***%s",
			 SETTING_FONT_RED, err, SETTING_FONT_BLACK);
			ret = SETTING_DNET_RETURN_ERR;
			setting_create_simple_popup(ad, ad->win_get,
			                            NULL, _("IDS_CST_POP_FAILED"));
		}
	}
	return ret;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
#if 0
static void
setting_network_connection_click_softkey_done_cb(void *data,
                                                 Evas_Object *obj,
                                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->data_profile_name) {
		setting_hide_input_pannel_cb(ad->data_profile_name->eo_check);
	}

	if (ad->data_acs_name) {
		setting_hide_input_pannel_cb(ad->data_acs_name->eo_check);
	}
	if (ad->data_user_name) {
		setting_hide_input_pannel_cb(ad->data_user_name->eo_check);
	}
	if (ad->data_pwd) {
		setting_hide_input_pannel_cb(ad->data_pwd->eo_check);
	}
	if (ad->data_pxy_addr) {
		setting_hide_input_pannel_cb(ad->data_pxy_addr->eo_check);
	}
	if (ad->data_pxy_port) {
		setting_hide_input_pannel_cb(ad->data_pxy_port->eo_check);
	}
	if (ad->data_hm_url) {
		setting_hide_input_pannel_cb(ad->data_hm_url->eo_check);
	}
}
#endif

static void __save_response_cb(void *data, Evas_Object *obj,
                               void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
	/*re-fetch connection info.. */
	/*__get_connection_info(ad); */

	SETTING_TRACE("ad->con_name:%s", ad->con_name);

	ad->apn_MMS = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_MMS, ad);

	elm_naviframe_item_pop(ad->navi_bar);	/* Call automatically setting_network_connection_click_softkey_back_cb() by registering elm_naviframe_item_pop_cb_set() */

	/*
	//UI update
	//elm_object_item_text_set(ad->navi_it, _(ad->con_name));
	//setting_network_connection_click_softkey_cancel_cb(ad, ad->r_button, NULL);
	SETTING_TRACE("ad->con_name:%s", ad->con_name);
	if(!safeStrCmp(STR_SETTING_NEW_CONNECTIONS, ad->con_name)
	   || ad->profile_topper_view == &setting_view_network_con)//new a profile,need to select the "Service type"
	{
		//not normal view change,need do specially
		setting_view_cleanup(&setting_view_network_connection_create, ad);
		//setting_view_update(&setting_view_network_con, ad);
	}
	else
	{
		elm_naviframe_item_pop(ad->navi_bar);	// Call automatically setting_network_connection_click_softkey_back_cb() by registering elm_naviframe_item_pop_cb_set()
		//setting_view_change(&setting_view_network_connection_create, &setting_view_network_con_list, ad);
	}
	*/
}

static void
setting_network_connection_click_softkey_save_cb(void *data,
                                                 Evas_Object *obj,
                                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	/* elm_box_unpack_all(ad->bx_blk); */
	/* elm_dialoguegroup_remove_all(ad->dg_blk); */

	int ret = __save_connection(ad);
	/* need special handle */
	if (SETTING_DNET_RETURN_NULL_ACCESS_NAME == ret
	    || SETTING_DNET_RETURN_NULL_USER_NAME == ret
	    || SETTING_DNET_RETURN_NULL_PASSWORD == ret
	    || SETTING_DNET_RETURN_INVALID_PROXY_ADDR == ret
	    || SETTING_DNET_RETURN_ERR == ret) {
		return;
	}
	if (ret != SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR
		("%s*** [ERR] __save_connection ***%s",
		 SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	/*setting_create_popup_without_btn(ad, ad->win_get, NULL, _(KeyStr_Saved), __save_response_cb, POPUP_INTERVAL, FALSE, FALSE); */
	setting_create_popup_without_btn(ad, ad->win_get, NULL, _(KeyStr_Saved), __save_response_cb, 1, FALSE, FALSE);
}


static Eina_Bool
setting_network_connection_click_softkey_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_TRUE, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("ad->con_name:%s", ad->con_name);
#if 0
	Ecore_IMF_Context *imf_context = NULL;
	Ecore_IMF_Input_Panel_State imf_state = ECORE_IMF_INPUT_PANEL_STATE_HIDE;
	Setting_GenGroupItem_Data *data_item = NULL;
	SETTING_TRACE("ECORE_IMF_INPUT_PANEL_STATE_SHOW:%d", ECORE_IMF_INPUT_PANEL_STATE_SHOW);

	do {
		data_item = ad->data_profile_name;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);

			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);

					return EINA_FALSE;
				}
			}
		}

		data_item = ad->data_acs_name;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			SETTING_TRACE("elm_object_focus_get(data_item->eo_check):%d", elm_object_focus_get(data_item->eo_check));
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}
		data_item = ad->data_user_name;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			SETTING_TRACE("elm_object_focus_get(data_item->eo_check):%d", elm_object_focus_get(data_item->eo_check));
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}
		data_item = ad->data_pwd;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}
		data_item = ad->data_pxy_addr;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}
		data_item = ad->data_pxy_port;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}

		data_item = ad->data_hm_url;
		if (data_item) {
			SETTING_TRACE("item:%s, entry_str:%s", _(data_item->keyStr), data_item->sub_desc);
			/*isFoundEmptyEntry = TRUE; */
			imf_context = elm_entry_imf_context_get(data_item->eo_check);
			if (elm_object_focus_get(data_item->eo_check) && imf_context) {
				imf_state = ecore_imf_context_input_panel_state_get(imf_context);
				SETTING_TRACE("imf_state:%d", imf_state);
				if (imf_state == ECORE_IMF_INPUT_PANEL_STATE_SHOW) {
					setting_hide_input_pannel_cb(data_item->eo_check);
					return EINA_FALSE;
				}
			}
		}

	} while (0);

#endif
	if (!safeStrCmp(STR_SETTING_NEW_CONNECTIONS, ad->con_name)
	    || ad->profile_topper_view == &setting_view_network_con) { /*new a profile,need to select the "Service type" */
		/*not normal view change,need do specially */
		setting_view_cleanup(&setting_view_network_connection_create, ad);
		setting_view_update(&setting_view_network_con, ad);
	} else {
		setting_view_change(&setting_view_network_connection_create, &setting_view_network_con_list, ad);
	}
	return EINA_TRUE;
}

static void
setting_network_connection_click_softkey_cancel_cb(void *data,
                                                   Evas_Object *obj,
                                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	elm_naviframe_item_pop(ad->navi_bar);	/* Call automatically setting_network_connection_click_softkey_back_cb() by registering elm_naviframe_item_pop_cb_set() */

	/*
	__get_connection_info(ad);
	__update_genlist_info(ad);
	if(ad->data_auth_type) elm_genlist_item_expanded_set(ad->data_auth_type->item, FALSE);
	ecore_idler_add(__connection_idler, ad);
	*/
}

static void setting_network_connection_check_entry_empty(SettingNetworkUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (!ad->has_form_changed) {
		SETTING_TRACE_DEBUG("ad->has_form_changed == FALSE!");
		return;
	}
	bool isFoundEmptyEntry = FALSE;

	do {
		if (ad->data_profile_name && isEmptyStr(ad->data_profile_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s", ad->data_profile_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_acs_name && isEmptyStr(ad->data_acs_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s", ad->data_acs_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_user_name && isEmptyStr(ad->data_user_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s", ad->data_user_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_pwd && isEmptyStr(ad->data_pwd->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s", ad->data_pwd->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_hm_url && isEmptyStr(ad->data_hm_url->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s", ad->data_hm_url->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
	} while (0);
	Evas_Object *done_btn = elm_object_item_part_content_get(ad->navi_it, NAVI_BTN_STYLE_LEFT);
	if (isFoundEmptyEntry) {
		if (!evas_object_pass_events_get(done_btn)) {/*it had not yet been disabled */
			setting_disable_evas_object(done_btn);
			setting_dim_evas_object(done_btn, TRUE);
		} else {
			/*do nothing..if invoke setting_dim_evas_object many times, the done button will be dimer until it cannot be see. */
			/*refer to the API of setting_dim_evas_object(). The API is supplied by Setting itself, winset doesn't support until now as I now. */
		}
	} else {
		setting_enable_evas_object(done_btn);
		setting_undo_dim_evas_object(done_btn, TRUE);
	}
}

static void
setting_network_connection_display_auth_type(SettingNetworkUG *ad,
                                             int auth_type)
{
	SETTING_TRACE_BEGIN;
	retm_if(ad == NULL, "Data parameter is NULL");
	if (ad->chkType == auth_type) return;

	ad->chkType = auth_type;	/* for update genlist */
	if (CONNECTION_CELLULAR_AUTH_TYPE_NONE != auth_type) {	/* to show */
		SETTING_TRACE("ad->is_show_user:%d", ad->is_show_user);
		if (ad->is_show_user == 0) {	/* do only when hiden */
			SETTING_TRACE("to create");

			ad->data_user_name =
			    (Setting_GenGroupItem_Data *) calloc(1,
			                                         sizeof
			                                         (Setting_GenGroupItem_Data));
			setting_retm_if(ad->data_user_name == NULL, "calloc failed");
			ad->data_user_name->keyStr =
			    (char *)g_strdup(KeyStr_UserID);
			ad->data_user_name->sub_desc =
			    (char *)g_strdup(ad->ed_user_name_desc);
			ad->data_user_name->swallow_type =
			    SWALLOW_Type_LAYOUT_EDITFIELD;
			ad->data_user_name->userdata = ad;
			ad->data_user_name->group_style = SETTING_GROUP_STYLE_TOP;

			ad->data_user_name->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
			ad->data_user_name->activated_cb = setting_network_connection_entry_next_activated_cb;

			ad->data_user_name->maxlength_reached_cb = __network_max_len_reached;
			ad->data_user_name->disable_auto_cap = EINA_TRUE;

			__BACK_POINTER_SET(ad->data_user_name);

			ad->data_user_name->chk_change_cb =
			    setting_network_connection_entry_changed_cb;
			ad->data_user_name->limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
			if (ad->data_user_name->limit_filter_data) {
				ad->data_user_name->limit_filter_data->max_byte_count = MAX_PDP_AUTH_USERNAME_LEN_MAX;
				ad->data_user_name->win_main = ad->win_get;
			}
			ad->data_user_name->item =
			    elm_genlist_item_insert_after(ad->scl_edit,
			                                  &itc_editfield,
			                                  ad->data_user_name,
			                                  NULL,
			                                  ad->item_above_user_name,
			                                  ELM_GENLIST_ITEM_NONE,
			                                  NULL, ad);

			ad->data_pwd =
			    (Setting_GenGroupItem_Data *) calloc(1,
			                                         sizeof
			                                         (Setting_GenGroupItem_Data));

			setting_retm_if(ad->data_pwd == NULL, "calloc failed");
			ad->data_pwd->keyStr =
			    (char *)g_strdup("IDS_ST_BODY_PASSWORD");
			ad->data_pwd->sub_desc =
			    (char *)g_strdup(ad->ed_pwd_desc);
			ad->data_pwd->swallow_type = SWALLOW_Type_LAYOUT_EDITFIELD;
			ad->data_pwd->userdata = ad;
			ad->data_pwd->chk_change_cb =
			    setting_network_connection_entry_changed_cb;
			ad->data_pwd->isPasswordFlag = TRUE;
			ad->data_pwd->isSinglelineFlag = TRUE;

			ad->data_pwd->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
			ad->data_pwd->activated_cb = setting_network_connection_entry_next_activated_cb;

			ad->data_pwd->maxlength_reached_cb = __network_max_len_reached;
			ad->data_pwd->disable_auto_cap = EINA_TRUE;

			__BACK_POINTER_SET(ad->data_pwd);
			ad->data_pwd->group_style = SETTING_GROUP_STYLE_BOTTOM;

			/* ad->data_pwd->chk_change_cb = chk_change_cb; */
			ad->data_pwd->limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
			if (ad->data_pwd->limit_filter_data) {
				ad->data_pwd->limit_filter_data->max_byte_count = MAX_PDP_AUTH_PASSWORD_LEN_MAX;
				ad->data_pwd->win_main = ad->win_get;
			}
			ad->data_pwd->item =
			    elm_genlist_item_insert_after(ad->scl_edit,
			                                  &itc_editfield,
			                                  ad->data_pwd, NULL,
			                                  ad->data_user_name->item,
			                                  ELM_GENLIST_ITEM_NONE,
			                                  NULL, ad);

			ad->item_above_proxy_add =
			    elm_genlist_item_insert_after(ad->scl_edit,
			                                  &itc_seperator,
			                                  NULL, NULL,
			                                  ad->data_pwd->item,
			                                  ELM_GENLIST_ITEM_NONE,
			                                  NULL, NULL);
			elm_genlist_item_select_mode_set
			(ad->item_above_proxy_add, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

			ad->has_form_changed  = TRUE;
			setting_network_connection_check_entry_empty(ad);
			elm_genlist_item_select_mode_set(ad->data_user_name->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			elm_genlist_item_select_mode_set(ad->data_pwd->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			/* ad->is_show_user = 1; */
		}
		ad->is_show_user = 1;
	} else {		/* to hide */

		SETTING_TRACE("ad->is_show_user:%d", ad->is_show_user);
		if (ad->is_show_user == 1) {	/* do only when shown */
			SETTING_TRACE("to destroy");
			if (ad->data_user_name) {
				elm_object_item_del(ad->data_user_name->item);
				ad->data_user_name = NULL;
			}
			if (ad->data_pwd) {
				elm_object_item_del(ad->data_pwd->item);
				ad->data_pwd = NULL;
			}
			if (ad->item_above_proxy_add) {
				elm_object_item_del(ad->item_above_proxy_add);
				ad->item_above_proxy_add = NULL;
			}

		}
		ad->has_form_changed  = TRUE;
		setting_network_connection_check_entry_empty(ad);
		ad->is_show_user = 0;

	}
}

static void
setting_network_connection_entry_changed_cb(void *data, Evas_Object *obj,
                                            void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingNetworkUG *ad = list_item->userdata;
	const char *entry_str = elm_entry_entry_get(obj);

	if (list_item->maxLengthReachFlag && list_item->enterKeyPressFlag == FALSE) {
		int ret = notification_status_message_post(_("IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
		if (ret != NOTIFICATION_ERROR_NONE)
			SETTING_TRACE_ERROR("notification_status_message_post() failed(%d)", ret);

		/*
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get, NULL,
					 _("IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"),
					 NULL, 2, TRUE, FALSE);
		*/
		elm_object_focus_set(list_item->eo_check, EINA_FALSE);
	}
	list_item->maxLengthReachFlag = FALSE;
	list_item->enterKeyPressFlag = FALSE;
	/*G_FREE(list_item->sub_desc);//release first */
	list_item->sub_desc = (char *)g_strdup(entry_str);
	ad->has_form_changed  = TRUE;
	setting_network_connection_check_entry_empty(ad);
}
static void
setting_network_connection_hide_input_pannel(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = data;
	if (ad->data_profile_name && ad->data_profile_name->eo_check) {
		setting_hide_input_pannel_cb(ad->data_profile_name->eo_check);
	}
	if (ad->data_acs_name && ad->data_acs_name->eo_check) {
		setting_hide_input_pannel_cb(ad->data_acs_name->eo_check);
	}
	if (ad->data_user_name && ad->data_user_name->eo_check) {
		setting_hide_input_pannel_cb(ad->data_user_name->eo_check);
	}
	if (ad->data_pwd && ad->data_pwd->eo_check) {
		setting_hide_input_pannel_cb(ad->data_pwd->eo_check);
	}
	if (ad->data_pxy_addr && ad->data_pxy_addr->eo_check) {
		setting_hide_input_pannel_cb(ad->data_pxy_addr->eo_check);
	}
	if (ad->data_pxy_port && ad->data_pxy_port->eo_check) {
		setting_hide_input_pannel_cb(ad->data_pxy_port->eo_check);
	}
	if (ad->data_hm_url && ad->data_hm_url->eo_check) {
		setting_hide_input_pannel_cb(ad->data_hm_url->eo_check);
	}
}

