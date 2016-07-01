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
static void setting_network_connection_click_softkey_back_ctx_cb(void *data,
		Evas_Object *obj, void *event_info);

setting_view setting_view_network_connection_create = {
		.create = setting_network_connection_create,
		.destroy = setting_network_connection_destroy,
		.update = setting_network_connection_update,
		.cleanup = setting_network_connection_cleanup, };

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

void __sub_list_rd_change(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_radio_value_get(obj); /*	for update */

	Elm_Object_Item *subItem = list_item->item;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subItem);

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subItem); /* subItem data */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */

	ret_if(NULL == data_subItem || NULL == data_parentItem);

	data_parentItem->sub_desc = (char *) g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);

	SettingNetworkUG *ad = data_parentItem->userdata;

	if (data_parentItem == ad->data_auth_type) {
		elm_genlist_item_expanded_set(ad->data_auth_type->item, FALSE);
		elm_genlist_item_update(ad->data_auth_type->item);

		setting_network_connection_display_auth_type(
				data_parentItem->userdata,
				list_item->chk_status);
	} else if (data_parentItem == ad->data_srv_type) {
		if (ad->srvType != data_subItem->chk_status)
			ad->srvType = data_subItem->chk_status;
		if (ad->srvType != CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
			if (!ad->data_hm_url) {
				/*need home url */
				ad->data_hm_url = setting_create_Gendial_field_def(
						obj, &itc_editfield,
						NULL, NULL,
						SWALLOW_Type_LAYOUT_EDITFIELD,
						NULL, NULL, 0,
						"IDS_ST_BODY_HOME_URL",
						(char *) ad->ed_hm_url_desc,
						setting_network_connection_entry_changed_cb);
				if (ad->data_hm_url) {
					ad->data_hm_url->userdata = ad;
					ad->data_hm_url->input_type = ELM_INPUT_PANEL_LAYOUT_URL;
					ad->data_hm_url->isSinglelineFlag = TRUE;
					ad->data_hm_url->limit_filter_data = calloc(
							1,
							sizeof(Elm_Entry_Filter_Accept_Set));
					if (ad->data_hm_url->limit_filter_data) {
						ad->data_hm_url->limit_filter_data->max_byte_count = MAX_HOME_URL_LEN_MAX
								- 1;
						ad->data_hm_url->win_main = ad->win_get;
					}
				} else {
					SETTING_TRACE_ERROR(
							"ad->data_hm_url is NULL");
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

static void _create_new_cellular_internet_profile(SettingNetworkUG *ad)
{
	int ret;
	const char *profile_name = NULL;

	if (ad->data_profile_name)
		profile_name = elm_entry_markup_to_utf8(
				ad->data_profile_name->sub_desc);
	if (!profile_name)
		profile_name = strdup("");

	ret = connection_profile_create(
			CONNECTION_PROFILE_TYPE_CELLULAR,
			_(profile_name), &ad->sel_profile_h);

	if (ret == CONNECTION_ERROR_NONE)
		ret = connection_profile_set_cellular_service_type(
				ad->sel_profile_h,
				CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);

	if (ret != CONNECTION_ERROR_NONE)
		SETTING_TRACE_ERROR("Cannot create new profile. Error: %s",
				get_error_message(ret));

	FREE(profile_name);
}

void __sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	int ret;

	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(subitem, 0);

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subitem);
	Setting_GenGroupItem_Data *data_parentItem =
			(Setting_GenGroupItem_Data *) data;
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	SettingNetworkUG *ad = data_parentItem->userdata;
	ret_if(NULL == ad);

	int chk_status = data_subItem->chk_status;
	elm_radio_value_set(data_subItem->rgd, chk_status);

	data_parentItem->sub_desc = (char *) g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);

	if (data_parentItem == ad->data_auth_type) {
		elm_genlist_item_expanded_set(ad->data_auth_type->item, FALSE);
		elm_genlist_item_update(ad->data_auth_type->item);

		setting_network_connection_display_auth_type(ad, chk_status);

		/*remove network mode popup */
		if (ad->popup_auth_type) {
			evas_object_del(ad->popup_auth_type);
			ad->popup_auth_type = NULL;
		}

	} else if (data_parentItem == ad->data_srv_type) {
		if (ad->srvType != data_subItem->chk_status)
			ad->srvType = data_subItem->chk_status;
		if (ad->srvType != CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET) {
			if (!ad->data_hm_url) {
				/*need home url */
				ad->data_hm_url = setting_create_Gendial_field_def(
						obj, &itc_editfield,
						NULL, NULL,
						SWALLOW_Type_LAYOUT_EDITFIELD,
						NULL, NULL, 0,
						"IDS_ST_BODY_HOME_URL",
						(char *) ad->ed_hm_url_desc,
						setting_network_connection_entry_changed_cb);
				if (ad->data_hm_url) {
					ad->data_hm_url->userdata = ad;
					ad->data_hm_url->input_type = ELM_INPUT_PANEL_LAYOUT_URL;
					ad->data_hm_url->limit_filter_data = calloc(
							1,
							sizeof(Elm_Entry_Filter_Accept_Set));
					if (ad->data_hm_url->limit_filter_data) {
						ad->data_hm_url->limit_filter_data->max_byte_count = MAX_HOME_URL_LEN_MAX
								- 1;
						ad->data_hm_url->win_main = ad->win_get;
					}
				} else {
					SETTING_TRACE_ERROR(
							"ad->data_hm_url is NULL");
				}
			}
		} else {
			/*do need home-url */
			if (ad->data_hm_url) {
				elm_object_item_del(ad->data_hm_url->item);
				ad->data_hm_url = NULL;
			}
		}

	} else if (data_parentItem == ad->data_pdn_type) {
		elm_genlist_item_expanded_set(ad->data_pdn_type->item, FALSE);
		elm_genlist_item_update(ad->data_pdn_type->item);

		if (!ad->sel_profile_h)
			_create_new_cellular_internet_profile(ad);

		ret = connection_profile_set_cellular_pdn_type(
				ad->sel_profile_h,
				chk_status);
		if (ret != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to set cellular pdn type!"
					" Error code (%d): %s",
					ret, get_error_message(ret));
		} else {
/*TODO: select proper connection[] */
			ad->chkType_pdn = chk_status;
			connection_update_profile(ad->connection[0],
					ad->sel_profile_h);
		}

		/*remove network mode popup */
		if (ad->popup_pdn_type) {
			evas_object_del(ad->popup_pdn_type);
			ad->popup_pdn_type = NULL;
		}

	} else if (data_parentItem == ad->data_roam_pdn_type) {
		elm_genlist_item_expanded_set(ad->data_roam_pdn_type->item,
				FALSE);
		elm_genlist_item_update(ad->data_roam_pdn_type->item);

		if (!ad->sel_profile_h)
			_create_new_cellular_internet_profile(ad);

		ret = connection_profile_set_cellular_roam_pdn_type(
				ad->sel_profile_h,
				chk_status);
		if (ret != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to set cellular roam pdn "
					"type: %s", get_error_message(ret));
		} else {
/*TODO: select proper connection[] */
			ad->chkType_roam_pdn = chk_status;
			connection_update_profile(ad->connection[0],
					ad->sel_profile_h);
		}

		/*remove network mode popup */
		if (ad->popup_pdn_type) {
			evas_object_del(ad->popup_pdn_type);
			ad->popup_pdn_type = NULL;
		}
	}
}

void __popup_del(void *data, Evas_Object *obj, void *event_info)
{
	SettingNetworkUG *ad = data;

	ret_if(data == NULL);

	if (ad->popup_auth_type) {
		evas_object_del(ad->popup_auth_type);
		ad->popup_auth_type = NULL;
	} else if (ad->popup_pdn_type) {
		evas_object_del(ad->popup_pdn_type);
		ad->popup_auth_type = NULL;
	}
}

static void __create_auth_type_popup(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Elm_Object_Item *parentItem = event_info; /* parent item */
	elm_genlist_item_selected_set(parentItem, 0);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */

	/* create popup */
	if (ad->popup_auth_type) {
		evas_object_del(ad->popup_auth_type);
		ad->popup_auth_type = NULL;
	}
	Evas_Object *scroller = NULL;
	ad->popup_auth_type = setting_create_popup_with_list(&scroller, ad,
			ad->win_get, data_parentItem->keyStr, __popup_del, 0,
			false, false, 0);
	_P(ad->popup_auth_type);

	Evas_Object *rgd;

	if (data_parentItem == ad->data_auth_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_PAP,
				"IDS_ST_MBODY_PAP",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_CHAP,
				"IDS_ST_POP_CHAP",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_NONE,
				"IDS_ST_BODY_NONE",
				NULL);

		elm_radio_value_set(rgd, ad->chkType);
		elm_object_signal_emit(rgd, "elm,event,pass,enabled", "elm");
	} else if (data_parentItem == ad->data_srv_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET,
				"IDS_ST_BODY_INTERNET_CONNECTION",
				__sub_list_rd_change);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_MMS,
				STR_SETTING_MMS_CONNECTIONS,
				__sub_list_rd_change);

		int r = 0;
		char tmp_str[SETTING_STR_SLP_LEN + 1] = { 0, };
		r = snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s %s", _(INTERNET),
				PLUS_MMS);
		if (r < 0)
			SETTING_TRACE("snprintf failed");

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION,
				tmp_str, __sub_list_rd_change);

		elm_radio_value_set(rgd, ad->srvType);
		elm_object_signal_emit(rgd, "elm,event,pass,enabled", "elm");
	}
}

static void __create_pdn_type_popup(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Elm_Object_Item *parentItem = event_info; /* parent item */
	elm_genlist_item_selected_set(parentItem, 0);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */

	/* create popup */
	if (ad->popup_pdn_type) {
		evas_object_del(ad->popup_pdn_type);
		ad->popup_pdn_type = NULL;
	}
	Evas_Object *scroller = NULL;
	ad->popup_pdn_type = setting_create_popup_with_list(&scroller, ad,
			ad->win_get, data_parentItem->keyStr, __popup_del, 0,
			false, false, 0);
	_P(ad->popup_pdn_type);

	Evas_Object *rgd;

	if (data_parentItem == ad->data_pdn_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon,
				__sub_list_sel_cb,
				data_parentItem,
				SWALLOW_Type_1RADIO_RIGHT,
				rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV4,
				"IPv4",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon,
				__sub_list_sel_cb,
				data_parentItem,
				SWALLOW_Type_1RADIO_RIGHT,
				rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV4_IPv6,
				"IPv4v6",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon,
				__sub_list_sel_cb,
				data_parentItem,
				SWALLOW_Type_1RADIO_RIGHT,
				rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV6,
				"IPv6",
				NULL);

		elm_radio_value_set(rgd, ad->chkType_pdn);
		elm_object_signal_emit(rgd, "elm,event,pass,enabled", "elm");
	} else if (data_parentItem == ad->data_roam_pdn_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV4, "IPv4",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV4_IPv6,
				"IPv4v6",
				NULL);

		setting_create_Gendial_field_1radio(scroller,
				&itc_multiline_1text_1icon, __sub_list_sel_cb,
				data_parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_PDN_TYPE_IPV6, "IPv6",
				NULL);

		elm_radio_value_set(rgd, ad->chkType_roam_pdn);
		elm_object_signal_emit(rgd, "elm,event,pass,enabled", "elm");

	}
}

static void __setting_network_connection_exp_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	Elm_Object_Item *parentItem = event_info; /* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;

	if (data_parentItem == ad->data_auth_type) {
		rgd = elm_radio_add(scroller);
		/*elm_object_style_set(rgd, "list"); */
		elm_radio_value_set(rgd, -1);

		SETTING_TRACE("CONNECTION_CELLULAR_AUTH_TYPE_NONE : %d: ",
				CONNECTION_CELLULAR_AUTH_TYPE_NONE);
		SETTING_TRACE("CONNECTION_CELLULAR_AUTH_TYPE_PAP : %d: ",
				CONNECTION_CELLULAR_AUTH_TYPE_PAP);
		SETTING_TRACE("CONNECTION_CELLULAR_AUTH_TYPE_CHAP : %d: ",
				CONNECTION_CELLULAR_AUTH_TYPE_CHAP);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_NONE,
				IDS_ST_BODY_NONE, __sub_list_rd_change);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_PAP,
				IDS_ST_MBODY_PAP, __sub_list_rd_change);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_AUTH_TYPE_CHAP,
				IDS_ST_POP_CHAP, __sub_list_rd_change);

		SETTING_TRACE(
				" ---------------------->>> ad->chkType : %d ",
				ad->chkType);

		elm_radio_value_set(rgd, ad->chkType);
	} else if (data_parentItem == ad->data_srv_type) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET,
				"IDS_ST_BODY_INTERNET_CONNECTION",
				__sub_list_rd_change);

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_MMS,
				STR_SETTING_MMS_CONNECTIONS,
				__sub_list_rd_change);

		int r = 0;
		char tmp_str[SETTING_STR_SLP_LEN + 1] = { 0, };
		r = snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s %s", _(INTERNET),
				PLUS_MMS);
		if (r < 0)
			SETTING_TRACE("snprintf failed");

		setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, __sub_list_sel_cb, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd,
				CONNECTION_CELLULAR_SERVICE_TYPE_APPLICATION,
				tmp_str, __sub_list_rd_change);

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

	G_FREE(ad->ed_profile_name_desc);
	G_FREE(ad->ed_acs_name_desc);
	G_FREE(ad->ed_user_name_desc);
	G_FREE(ad->ed_pwd_desc);
	G_FREE(ad->ed_pxy_addr_desc);
	G_FREE(ad->ed_pxy_port_desc);
	G_FREE(ad->ed_hm_url_desc);

	ad->ed_auth_type_desc = NULL;
	ad->ed_pdn_type_desc = NULL;
	ad->ed_roam_pdn_type_desc = NULL;
	ad->chkType = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
	ad->chkType_pdn = CONNECTION_CELLULAR_PDN_TYPE_UNKNOWN;

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
	connection_profile_type_e profile_type =
			CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type =
			CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
/*TODO: select proper connection[] */
	int err = connection_get_profile_iterator(ad->connection[0],
			CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
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

		if (cnt >= MAX_PROFILE_NUM
				|| connection_profile_iterator_next(
						profile_iter, &profile_h)
						!= CONNECTION_ERROR_NONE) {
			break;
		}

		connection_profile_get_type(profile_h, &profile_type);
		connection_profile_get_cellular_service_type(profile_h,
				&service_type);
		connection_profile_get_cellular_apn(profile_h, &apn);
		connection_profile_get_id(profile_h, &profile_id);
		connection_profile_get_name(profile_h, &profile_name);
		SETTING_TRACE("Record[%d] apn[%s], profile_id[%s], "
				"profile_name[%s], profile_type[%d], "
				"service_type[%d]",
				cnt, apn, profile_id, profile_name,
				profile_type, service_type);
		if (CONNECTION_PROFILE_TYPE_CELLULAR != profile_type)
			continue;

		/*match with ServiceType */
		if (ServiceType == service_type) {
			if (ServiceType == CONNECTION_CELLULAR_SERVICE_TYPE_MMS) {
				/*for MMS,it just have one profile */
				ad->sel_profile_h = profile_h;
				found = TRUE;
				break;
			} else {
				if (0 == safeStrCmp(con_name_utf8,
								profile_id)) {
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

		connection_profile_is_cellular_editable(ad->sel_profile_h,
				&(ad->is_editable));
		SETTING_TRACE("ad->is_editable:%d", ad->is_editable);

		/* load access name */
		connection_profile_get_cellular_apn(ad->sel_profile_h,
				&(ad->ed_acs_name_desc));
		connection_cellular_auth_type_e auth_type =
				CONNECTION_CELLULAR_AUTH_TYPE_NONE;
		connection_profile_get_cellular_auth_info(ad->sel_profile_h,
				&auth_type, &(ad->ed_user_name_desc),
				&(ad->ed_pwd_desc));
		SETTING_TRACE(" ----> auth_type: (%d)", auth_type);

		ad->chkType = auth_type;
		/*SETTING_TRACE("-----> radio button state: %d",
		 * elm_radio_value_get(ad->chk_type));*/
		SETTING_TRACE(
				"-------------> connection_profile_get_cellular"
				"_auth_info: %d",
				auth_type);
		elm_radio_value_set(ad->chk_type, ad->chkType);

		switch (ad->chkType) {
		case CONNECTION_CELLULAR_AUTH_TYPE_NONE:
			ad->ed_auth_type_desc = _("IDS_ST_BODY_NONE");
			break;
		case CONNECTION_CELLULAR_AUTH_TYPE_PAP:
			ad->ed_auth_type_desc = _("IDS_ST_MBODY_PAP");
			break;
		case CONNECTION_CELLULAR_AUTH_TYPE_CHAP:
			ad->ed_auth_type_desc = _("IDS_ST_POP_CHAP");
			break;
		default:
			ad->chkType = CONNECTION_CELLULAR_AUTH_TYPE_NONE;
			ad->ed_auth_type_desc = _("IDS_ST_BODY_NONE");
			break;
		}

		/* pdn type */
		connection_cellular_pdn_type_e pdn_type;
		if (CONNECTION_ERROR_NONE
				!= connection_profile_get_cellular_pdn_type(
						ad->sel_profile_h, &pdn_type)) {
			SETTING_TRACE_ERROR("Fail to get cellular pdn type!\n");
		} else {
			ad->chkType_pdn = pdn_type;
			SETTING_TRACE("Cellular pdn type : %d\n", pdn_type);
			elm_radio_value_set(ad->chk_type_pdn, ad->chkType_pdn);

			switch (ad->chkType_pdn) {
			case CONNECTION_CELLULAR_PDN_TYPE_IPV4:
				ad->ed_pdn_type_desc = _("IPv4");
				break;
			case CONNECTION_CELLULAR_PDN_TYPE_IPV6:
				ad->ed_pdn_type_desc = _("IPv6");
				break;
			case CONNECTION_CELLULAR_PDN_TYPE_IPV4_IPv6:
				ad->ed_pdn_type_desc = _("IPv4v6");
				break;
			default:
				ad->chkType_pdn = CONNECTION_CELLULAR_PDN_TYPE_UNKNOWN;
				ad->ed_pdn_type_desc = _("Unknown");
			}
		}

		/* roam_pdn_type*/
		connection_cellular_pdn_type_e roam_pdn_type;
		if (CONNECTION_ERROR_NONE
				!= connection_profile_get_cellular_roam_pdn_type(
						ad->sel_profile_h,
						&roam_pdn_type)) {
			SETTING_TRACE_ERROR("Fail to get cellular pdn type!\n");
		} else {
			ad->chkType_roam_pdn = roam_pdn_type;
			SETTING_TRACE("Cellular roam pdn type : %d\n",
					roam_pdn_type);
			elm_radio_value_set(ad->chk_type_roam_pdn,
					ad->chkType_roam_pdn);

			switch (ad->chkType_roam_pdn) {
			case CONNECTION_CELLULAR_PDN_TYPE_IPV4:
				ad->ed_roam_pdn_type_desc = _("IPv4");
				break;
			case CONNECTION_CELLULAR_PDN_TYPE_IPV6:
				ad->ed_roam_pdn_type_desc = _("IPv6");
				break;
			case CONNECTION_CELLULAR_PDN_TYPE_IPV4_IPv6:
				ad->ed_roam_pdn_type_desc = _("IPv4v6");
				break;
			default:
				ad->chkType_roam_pdn =
						CONNECTION_CELLULAR_PDN_TYPE_UNKNOWN;
				ad->ed_roam_pdn_type_desc = _("Unknown");
			}
		}

		char *full_addr = NULL;
		char *addr = NULL;
		char *port = NULL;
		/*Fix the Port 'NULL' issue when enter connection view
		 * repeatly.. */
		connection_profile_get_proxy_address(ad->sel_profile_h,
				CONNECTION_ADDRESS_FAMILY_IPV4, &full_addr);
		SETTING_TRACE("full_addr: %s", full_addr);
		char ProxyAddr[MAX_HOME_URL_LEN_MAX + 1] = { 0, };
		safeCopyStr(ProxyAddr, full_addr, MAX_HOME_URL_LEN_MAX);
		G_FREE(full_addr);

		/*several real format after removing "http://": */
		/*	":80", "1.1.1.1:80","1.1.1.1: ", "127.0.0.1/",
		 * "127.0.0.1" */
		full_addr = ProxyAddr;
		int url_head_len = strlen(URL_HEAD);
		if (0 == safeStrNCmp(full_addr, URL_HEAD, url_head_len)) {
			full_addr += url_head_len;
		}
		addr = strsep(&full_addr, ":");
		if (NULL == full_addr) { /*format like "http://127.0.0.1/" or
		"http://127.0.0.1" */
			SETTING_TRACE("NULL == full_addr");
			addr = strsep(&addr, "/");
		}

		ad->ed_pxy_addr_desc = NULL;
		/*proxy address support not only ip but also url, need not
		 * check the format */
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
		ad->ed_pxy_port_desc = g_strdup(port);
		/* Message Connection */
		if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
			connection_profile_get_cellular_home_url(
					ad->sel_profile_h,
					&(ad->ed_hm_url_desc));
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

static void __network_max_len_reached(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	/*notify only when entry is being focused on. */
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");

	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	list_item->maxLengthReachFlag = true;

	SETTING_TRACE("list_item->enterKeyPressFlag : %d",
			list_item->enterKeyPressFlag);
	SettingNetworkUG *ad = list_item->userdata;

	/* popup show */
	ad->popup = setting_create_popup(ad, ad->win_get, NULL,
			"IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED",
			NULL, 2, TRUE, FALSE, 0);
}

/*
 * Entry - activated callback (When 'Next' key in SIP touched)
 */
static void setting_network_connection_entry_next_activated_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	SettingNetworkUG *ad = list_item->userdata;

	Evas_Object *pObj_NextEntry = NULL;

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

static void _init_context(void *cb)
{
	SETTING_TRACE_BEGIN;
	ret_if(cb == NULL);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	/* connection intializes */
	ad->data_profile_name = NULL;
	ad->data_acs_name = NULL;
	ad->data_auth_type = NULL;
	ad->data_user_name = NULL;
	ad->data_pwd = NULL;
	ad->data_pxy_addr = NULL;
	ad->data_pxy_port = NULL;
	ad->data_hm_url = NULL;
}

/*////////////////////////////////////////////////////////////////////////// */
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
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
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

	/*------------------------------------------------------------------ */
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BODY_SAVE"), NULL,
			setting_network_connection_click_softkey_save_cb, ad);
	elm_ctxpopup_item_append(ctxpopup, _("IDS_ST_BUTTON_CANCEL_ABB"), NULL,
			setting_network_connection_click_softkey_back_ctx_cb,
			ad);
	/*------------------------------------------------------------------ */

	elm_ctxpopup_direction_priority_set(ctxpopup, ELM_CTXPOPUP_DIRECTION_UP,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN,
			ELM_CTXPOPUP_DIRECTION_UNKNOWN);
	move_more_ctxpopup(ctxpopup);
	evas_object_show(ctxpopup);
}
/*////////////////////////////////////////////////////////////////////////// */

static int setting_network_connection_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	setting_retvm_if(!ad->con_name, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"!ad->con_name");

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
			"Cannot set scroller object  as contento of layout");
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	elm_genlist_clear(scroller); /* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			NULL);

	ad->con_create_gl = scroller;

	_init_context(ad);

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
	NULL, /* ARROW STYLE */
	_("IDS_ST_BODY_SAVE"), _("IDS_ST_BUTTON_CANCEL_ABB"),
			setting_network_connection_click_softkey_back_cb,
			setting_network_connection_click_softkey_save_cb,
			/* setting_network_connection_click_softkey_back_cb, */
			setting_network_connection_click_softkey_cancel_cb,
			ad, scroller, ad->navi_bar, NULL);

	elm_naviframe_item_pop_cb_set(ad->navi_it,
			setting_network_connection_click_softkey_back_cb, ad);
	evas_object_data_set(ad->navi_bar, "sip.naviframe.title_obj", "*");

	/* Add ctx popup handler */
	Evas_Object *btn = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn, "naviframe/more/default");
	evas_object_smart_callback_add(btn, "clicked",
			create_ctxpopup_more_button_cb, ad);
	elm_object_item_part_content_set(ad->navi_it, "toolbar_more_btn", btn);

	retv_if(!ad->navi_it, SETTING_RETURN_FAIL);
	ad->has_form_changed = FALSE;

	setting_enable_expandable_genlist(scroller, ad,
			__setting_network_connection_exp_cb,
			NULL);
	__get_connection_info(ad);

	/*Elm_Object_Item *item = NULL;*/

	/*new a profile,need to select the "Service type" */
	if (!safeStrCmp(STR_SETTING_NEW_CONNECTIONS, ad->con_name)) {
		ad->data_srv_type = setting_create_Gendial_exp_parent_field(
				scroller, &itc_2text_3_parent,
				NULL, NULL, SWALLOW_Type_INVALID,
				_("IDS_MSGF_BODY_SERVICE_TYPE"),
				_("IDS_ST_BODY_INTERNET_CONNECTION"));
		/* ad->data_auth_type->int_slp_setting_binded =
		 * INT_SLP_SETTING_INVALID; */
		if (ad->data_srv_type) {
			ad->data_srv_type->userdata = ad;
			__BACK_POINTER_SET(ad->data_srv_type);
		} else {
			SETTING_TRACE_ERROR("ad->data_auth_type is NULL");
		}
	}

	ad->ed_profile_name_desc = elm_entry_utf8_to_markup(
			ad->ed_profile_name_desc);
	ad->ed_acs_name_desc = elm_entry_utf8_to_markup(ad->ed_acs_name_desc);
	SETTING_TRACE("name :%s", ad->ed_profile_name_desc);
	SETTING_TRACE("apn :%s", ad->ed_acs_name_desc);

	/* [UI] Profile name */
	if (!isEmptyStr(ad->ed_profile_name_desc)) {
		/* Display */
		ad->data_profile_name = setting_create_Gendial_field_def(
				scroller, &itc_multiline_2text,
				NULL, ad, SWALLOW_Type_INVALID,
				NULL,
				NULL, 0, /* chk_status*/
				"IDS_ST_BODY_PROFILE_NAME",
				(char *) ad->ed_profile_name_desc, /* sub_desk*/
				NULL);
	} else {
		/* Editor */
		ad->data_profile_name = setting_create_Gendial_field_entry_with_return_key(
				scroller, &itc_editfield, NULL, ad,
				SWALLOW_Type_LAYOUT_EDITFIELD,
				NULL, NULL, 0, "IDS_ST_BODY_PROFILE_NAME",
				(char *) ad->ed_profile_name_desc,
				setting_network_connection_entry_changed_cb,
				__network_max_len_reached,
				ELM_INPUT_PANEL_LAYOUT_NORMAL,
				FALSE, FALSE, 64, 64, NULL, NULL,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT,
				setting_network_connection_entry_next_activated_cb,
				NULL);
	}

	if (ad->data_profile_name) {
		__BACK_POINTER_SET(ad->data_profile_name);
		ad->data_profile_name->userdata = ad;
		ad->data_profile_name->win_main = ad->win_get;
		ad->data_profile_name->isSinglelineFlag = TRUE;
		/*there is no CAPI to set profile name,so disable it: */
		/*setting_disable_genlist_item(ad->data_profile_name->item); */
		/*not create(first time) */
		if (!isEmptyStr(ad->ed_profile_name_desc)) {
			elm_genlist_item_select_mode_set(
					ad->data_profile_name->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

			/*setting_disable_genlist_item(
			 * ad->data_profile_name->item); */
		}
	} else {
		SETTING_TRACE_ERROR("ad->data_acs_name is NULL");
	}

	/* [UI] Access point name */
	ad->data_acs_name = setting_create_Gendial_field_entry_with_return_key(
			scroller, &itc_editfield, NULL, ad,
			SWALLOW_Type_LAYOUT_EDITFIELD,
			NULL, NULL, 0, "IDS_DLNA_BODY_ACCESS_POINT_NAME",
			(char *) ad->ed_acs_name_desc,
			setting_network_connection_entry_changed_cb,
			__network_max_len_reached,
			ELM_INPUT_PANEL_LAYOUT_NORMAL,
			FALSE, FALSE, 64, 64, NULL, NULL,
			ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE, NULL, NULL);
	if (ad->data_acs_name) {
		__BACK_POINTER_SET(ad->data_acs_name);
		ad->data_acs_name->userdata = ad;
		ad->data_acs_name->isSinglelineFlag = TRUE;
		ad->data_acs_name->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_acs_name is NULL");
	}

	/* [UI] Authentication type */
	ad->data_auth_type = setting_create_Gendial_field_def(scroller,
			&itc_2text_3_parent, __create_auth_type_popup, ad,
			SWALLOW_Type_INVALID,
			NULL, NULL, 0, "IDS_ST_BODY_AUTH_TYPE",
			(char *) ad->ed_auth_type_desc, NULL);
	if (ad->data_auth_type) {
		ad->data_auth_type->userdata = ad;
		__BACK_POINTER_SET(ad->data_auth_type);
	} else {
		SETTING_TRACE_ERROR("ad->data_auth_type is NULL");
	}

	ad->item_above_user_name = ad->data_auth_type->item;
	ad->is_show_user = 0;
	if (CONNECTION_CELLULAR_AUTH_TYPE_NONE != ad->chkType) {
		/* [UI] User ID */
		ad->data_user_name = setting_create_Gendial_field_entry_with_return_key(
				scroller, &itc_editfield,
				NULL, ad, SWALLOW_Type_LAYOUT_EDITFIELD,
				NULL, NULL, 0,
				KeyStr_UserID, (char *) ad->ed_user_name_desc,
				setting_network_connection_entry_changed_cb,
				__network_max_len_reached,
				ELM_INPUT_PANEL_LAYOUT_NORMAL,
				FALSE, FALSE,
				MAX_PDP_AUTH_USERNAME_LEN_MAX,
				MAX_PDP_AUTH_USERNAME_LEN_MAX,
				NULL, NULL,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT,
				setting_network_connection_entry_next_activated_cb,
				NULL);
		if (ad->data_user_name) {
			__BACK_POINTER_SET(ad->data_user_name);
			ad->data_user_name->userdata = ad;
			ad->data_user_name->isSinglelineFlag = TRUE;
			ad->data_user_name->win_main = ad->win_get;
		} else {
			SETTING_TRACE_ERROR("ad->data_user_name is NULL");
		}

		/* [UI] Password */
		ad->data_pwd = setting_create_Gendial_field_entry_with_return_key(
				scroller, &itc_editfield,
				NULL, ad, SWALLOW_Type_LAYOUT_EDITFIELD,
				NULL, NULL, 0, "IDS_ST_BODY_PASSWORD",
				(char *) ad->ed_pwd_desc,
				setting_network_connection_entry_changed_cb,
				__network_max_len_reached,
				ELM_INPUT_PANEL_LAYOUT_NORMAL,
				TRUE, FALSE,
				MAX_PDP_AUTH_USERNAME_LEN_MAX,
				MAX_PDP_AUTH_USERNAME_LEN_MAX,
				NULL, NULL,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT,
				setting_network_connection_entry_next_activated_cb,
				NULL);
		if (ad->data_pwd) {
			__BACK_POINTER_SET(ad->data_pwd);
			ad->data_pwd->userdata = ad;
			ad->data_pwd->win_main = ad->win_get;
			ad->data_pwd->isSinglelineFlag = TRUE;
			ad->is_show_user = 1;
		} else {
			SETTING_TRACE_ERROR("ad->data_pwd is NULL");
		}

		ad->item_above_proxy_add = ad->data_pwd->item;
	}

	/* [UI] Proxy address */
	ad->data_pxy_addr = setting_create_Gendial_field_entry_with_return_key(
			scroller, &itc_editfield, NULL, ad,
			SWALLOW_Type_LAYOUT_EDITFIELD,
			NULL, NULL, 0,
			"IDS_ST_BODY_CONNECTIONSETTINGS_PROXY_ADDRESS",
			(char *) ad->ed_pxy_addr_desc,
			setting_network_connection_entry_changed_cb,
			__network_max_len_reached, ELM_INPUT_PANEL_LAYOUT_URL,
			FALSE, FALSE, 512, 512, NULL, NULL,
			ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT,
			setting_network_connection_entry_next_activated_cb,
			NULL);
	if (ad->data_pxy_addr) {
		__BACK_POINTER_SET(ad->data_pxy_addr);
		ad->data_pxy_addr->userdata = ad;
		ad->data_pxy_addr->isSinglelineFlag = TRUE;
		ad->data_pxy_addr->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_pxy_addr is NULL");
	}

	/* Set SIP key on Next or Done if Home URL existed or not below. */
	Elm_Input_Panel_Return_Key_Type type =
			ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
	setting_call_back_func cbFunc = NULL;

	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
		cbFunc = setting_network_connection_entry_next_activated_cb;
		/*SETTING_TRACE_ERROR("Enter here hwy ---> ad->con_name :
		 *  %s ", ad->con_name); */
	}

	/* [UI] Proxy port */
	ad->data_pxy_port = setting_create_Gendial_field_entry_with_return_key(
			scroller, &itc_editfield, NULL, ad,
			SWALLOW_Type_LAYOUT_EDITFIELD,
			NULL, NULL, 0, "IDS_ST_BODY_PROXY_PORT",
			(char *) ad->ed_pxy_port_desc,
			setting_network_connection_entry_changed_cb,
			__network_max_len_reached,
			ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			FALSE, FALSE, 0, 5, "0123456789", NULL, type, cbFunc,
			NULL);
	if (ad->data_pxy_port) {
		ad->data_pxy_port->userdata = ad;
		ad->data_pxy_port->isSinglelineFlag = TRUE;
		__BACK_POINTER_SET(ad->data_pxy_port);
		ad->data_pxy_port->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("ad->data_pxy_port is NULL");
	}

	/* if current view is "MMS connection" */
	/*	mms Connection */
	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
		/* [UI] Home URL */
		ad->data_hm_url = setting_create_Gendial_field_entry_with_return_key(
				scroller, &itc_editfield,
				NULL, ad, SWALLOW_Type_LAYOUT_EDITFIELD,
				NULL, NULL, 0, "IDS_ST_BODY_HOME_URL",
				(char *) ad->ed_hm_url_desc,
				setting_network_connection_entry_changed_cb,
				__network_max_len_reached,
				ELM_INPUT_PANEL_LAYOUT_URL,
				FALSE, FALSE, 0, 521, NULL, NULL,
				ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE, NULL,
				NULL);
		if (ad->data_hm_url) {
			__BACK_POINTER_SET(ad->data_hm_url);
			ad->data_hm_url->userdata = ad;
			ad->data_hm_url->isSinglelineFlag = TRUE;
			ad->data_hm_url->win_main = ad->win_get;
			ad->data_hm_url->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
		} else {
			SETTING_TRACE_ERROR("ad->data_hm_url is NULL");
		}
	}

	/* [UI] APN protocol */
	ad->data_pdn_type = setting_create_Gendial_field_def(scroller,
			&itc_2text_3_parent, __create_pdn_type_popup, ad,
			SWALLOW_Type_INVALID,
			NULL, NULL, 0, "APN protocol",
			(char *) ad->ed_pdn_type_desc, NULL);
	if (ad->data_pdn_type) {
		ad->data_pdn_type->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_pdn_type is NULL");
	}

	/* [UI] APN roaming protocol */
	ad->data_roam_pdn_type = setting_create_Gendial_field_def(scroller,
			&itc_2text_3_parent, __create_pdn_type_popup, ad,
			SWALLOW_Type_INVALID,
			NULL, NULL, 0, "APN roaming protocol",
			(char *) ad->ed_roam_pdn_type_desc, NULL);
	if (ad->data_roam_pdn_type) {
		ad->data_roam_pdn_type->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_roam_pdn_type is NULL");
	}

	setting_view_network_connection_create.is_create = 1;
	ad->scl_edit = scroller;

	SETTING_TRACE("==> ad->is_editable:%d", ad->is_editable);
	if (!ad->is_editable) {
		if (ad->data_srv_type)
			setting_disable_genlist_item(ad->data_srv_type->item);
		if (ad->data_profile_name)
			setting_disable_genlist_item(
					ad->data_profile_name->item);
		if (ad->data_acs_name)
			setting_disable_genlist_item(ad->data_acs_name->item);
		if (ad->data_auth_type)
			setting_disable_genlist_item(ad->data_auth_type->item);
		if (ad->data_user_name)
			setting_disable_genlist_item(ad->data_user_name->item);
		if (ad->data_pwd)
			setting_disable_genlist_item(ad->data_pwd->item);
		if (ad->data_pxy_addr)
			setting_disable_genlist_item(ad->data_pxy_addr->item);
		if (ad->data_pxy_port)
			setting_disable_genlist_item(ad->data_pxy_port->item);
		if (ad->data_hm_url)
			setting_disable_genlist_item(ad->data_hm_url->item);
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

	elm_naviframe_item_pop(ad->navi_bar);

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
bool need_check_default_profile(void *data,
		connection_cellular_service_type_e inputtype)
{
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("inputtype:%d", inputtype);
	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;
/*TODO: select proper connection[] */
	int rv = connection_get_profile_iterator(ad->connection[0],
			CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
		return SETTING_RETURN_FAIL;
	}

	int cnt = 0;
	connection_profile_type_e profile_type =
			CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type =
			CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
	while (connection_profile_iterator_has_next(profile_iter)) {
		if (cnt >= MAX_PROFILE_NUM) {
			break;
		}
		cnt++;
		if (connection_profile_iterator_next(profile_iter, &profile_h)
				!= CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile handle");
			break;
		}

		if (connection_profile_get_type(profile_h, &profile_type)
				!= CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile type");
			continue;
		}
		if (connection_profile_get_cellular_service_type(profile_h,
				&service_type) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get service type");
			continue;
		}
		if (CONNECTION_PROFILE_TYPE_CELLULAR == profile_type
				&& inputtype == service_type) {
			char *apn = NULL;
			connection_profile_get_cellular_apn(profile_h, &apn);
			SETTING_TRACE("Matched:%s", apn);
			FREE(apn);
			return FALSE;
		}
	}
	return TRUE;
}

/**
 * popup - timeout and 'ok' callback
 */
static void __setting_network_connection_popup_rsp_cb(void *data,
		Evas_Object *obj, void *event_info)
{

	SETTING_TRACE_BEGIN;
	setting_retm_if(obj == NULL, "obj parameter is NULL");
	setting_retm_if(data == NULL, "Data parameter is NULL");
	/*SettingNetworkUG *ad = (SettingNetworkUG *) data; */

	/* remove ctxpopup */
	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
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
		profile_name = elm_entry_markup_to_utf8(
				ad->data_profile_name->sub_desc);
	if (ad->data_acs_name)
		acs_name = elm_entry_markup_to_utf8(
				ad->data_acs_name->sub_desc);
	if (ad->data_user_name)
		usr_name = elm_entry_markup_to_utf8(
				ad->data_user_name->sub_desc);
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

		char name[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = { 0, };
		snprintf(name, sizeof(name), _("IDS_ST_POP_ENTER_PS"),
				_("IDS_ST_BODY_PROFILE_NAME"));

		setting_create_popup(ad, ad->win_get, NULL, _(name),
				__setting_network_connection_popup_rsp_cb,
				2/*SECONDS*/, false, false, 0);
		FREE(acs_name);
		FREE(profile_name);
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_ACCESS_NAME;
	}

	if (!safeStrCmp(acs_name, "")) {

		char name[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = { 0, };
		snprintf(name, sizeof(name), _("IDS_ST_POP_ENTER_PS"),
				_("IDS_DLNA_BODY_ACCESS_POINT_NAME"));
		setting_create_popup(ad, ad->win_get, NULL, _(name),
				__setting_network_connection_popup_rsp_cb,
				2/*SECONDS*/, false, false, 0);
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
				&& 0 != safeStrCmp(ad->con_name,
						"IDS_ST_BODY_INTERNET_CONNECTION")) {
			/*need to change con_name */
			/*FREE(ad->con_name); */
			/*ad->con_name = strdup(acs_name); */
		}
	} else {
		if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)) {
			(void) connection_profile_create(
					CONNECTION_PROFILE_TYPE_CELLULAR,
					_(profile_name), &con_info);
			(void) connection_profile_set_cellular_service_type(
					con_info,
					CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
		} else if (!safeStrCmp(ad->con_name,
				"IDS_ST_BODY_INTERNET_CONNECTION")) {
			(void) connection_profile_create(
					CONNECTION_PROFILE_TYPE_CELLULAR,
					_(profile_name), &con_info);
			(void) connection_profile_set_cellular_service_type(
					con_info,
					CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
		} else {
			/*need to change con_name */
			/*FREE(ad->con_name); */
			/*ad->con_name = strdup(acs_name); */
			(void) connection_profile_create(
					CONNECTION_PROFILE_TYPE_CELLULAR,
					_(profile_name), &con_info);
			if (CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET
					== ad->srvType) {
				/*Internet */
				(void) connection_profile_set_cellular_service_type(
						con_info,
						CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
			} else if (CONNECTION_CELLULAR_SERVICE_TYPE_MMS
					== ad->srvType) {
				/*MMS */
				(void) connection_profile_set_cellular_service_type(
						con_info,
						CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
			} else { /*MMS + Internet(Together) */
				(void) connection_profile_set_cellular_service_type(
						con_info,
						CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET);
				(void) connection_profile_create(
						CONNECTION_PROFILE_TYPE_CELLULAR,
						_(profile_name), &con_info_2);
				(void) connection_profile_set_cellular_service_type(
						con_info_2,
						CONNECTION_CELLULAR_SERVICE_TYPE_MMS);
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
	if (con_info_2)
		(void) connection_profile_set_cellular_apn(con_info_2,
				acs_name);
	FREE(acs_name);

	/* save auth type */
	type = ad->chkType;
	SETTING_TRACE("-----> auth type: %d", type);
	SETTING_TRACE("-----> radio button state: %d",
			elm_radio_value_get(ad->chk_type));

#ifdef OLD_AUTH_CONCEPT
	/* save user name */
	if (!safeStrCmp(usr_name, "")
			&& CONNECTION_CELLULAR_AUTH_TYPE_NONE != type) {
		setting_create_popup(ad, ad->win_get,
				NULL, _(Insert_User_Name_Desc),
				__setting_network_connection_popup_rsp_cb, 2/*SECONDS*/, false, false, 0);
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_USER_NAME;
	}

	/* save password */
	if (!safeStrCmp(pwd, "")
			&& CONNECTION_CELLULAR_AUTH_TYPE_NONE != type) {
		/*this code is not used now, so remove the ID which is not
		 *  used in po file*/
		setting_create_popup(ad, ad->win_get,
				NULL, _(""),
				__setting_network_connection_popup_rsp_cb,
				2/*SECONDS*/, false, false, 0);
		FREE(usr_name);
		FREE(pwd);
		FREE(addr);
		FREE(port);
		return SETTING_DNET_RETURN_NULL_PASSWORD;
	}
#endif
	(void) connection_profile_set_cellular_auth_info(con_info, type,
			usr_name, pwd);
	if (con_info_2)
		(void) connection_profile_set_cellular_auth_info(con_info_2,
				type, usr_name, pwd);

	/* save proxy addr */
	SETTING_TRACE("addr:port[%s:%s]", addr, port);
	char *output_addr = NULL;
	if (isEmptyStr(addr)) {
		/* check proxy port */
		if (!isEmptyStr(port)) {
			setting_create_popup(ad, ad->win_get,
					NULL,
					_("IDS_COM_BODY_ENTER_PROXY_EMPTY"),
					__setting_network_connection_popup_rsp_cb,
					2/*SECONDS*/, false, false, 0);
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
			snprintf(sz_text, sizeof(sz_text), "%s:%s", output_addr,
					port);
		}
	}

	G_FREE(output_addr);
	SETTING_TRACE("sz_text:%s", sz_text);
	if (!isEmptyStr(sz_text)) {
		connection_profile_set_proxy_address(con_info,
				CONNECTION_ADDRESS_FAMILY_IPV4, sz_text);
		if (con_info_2) {
			connection_profile_set_proxy_address(con_info_2,
					CONNECTION_ADDRESS_FAMILY_IPV4,
					sz_text);
		}
	} else {
		connection_profile_set_proxy_address(con_info,
				CONNECTION_ADDRESS_FAMILY_IPV4, NULL);
		if (con_info_2) {
			connection_profile_set_proxy_address(con_info_2,
					CONNECTION_ADDRESS_FAMILY_IPV4, NULL);
		}
	}
	/* save home url(message connection only) */
	/* Message Connection */
	if (!safeStrCmp(ad->con_name, STR_SETTING_MMS_CONNECTIONS)
			|| (!safeStrCmp(ad->con_name,
					STR_SETTING_NEW_CONNECTIONS)
					&& CONNECTION_CELLULAR_SERVICE_TYPE_MMS
							== ad->srvType)) {
		home = elm_entry_markup_to_utf8(ad->data_hm_url->sub_desc);
		if (home == (char *) NULL) {
			(void) connection_profile_set_cellular_home_url(
					con_info, URL_HEAD);
		} else {
			if (!safeStrCmp(home, "")) {
				FREE(home);
				home = strdup(URL_HEAD);
				ret = SETTING_DNET_RETURN_NULL_HOMEURL;
			}
			(void) connection_profile_set_cellular_home_url(
					con_info, home);
		}
	}
	FREE(home);
	if (con_info_2) { /*creating both internet + MMS */
		home = elm_entry_markup_to_utf8(ad->data_hm_url->sub_desc);
		if (home == (char *) NULL) {
			(void) connection_profile_set_cellular_home_url(
					con_info_2, URL_HEAD);
		} else {
			if (!safeStrCmp(home, "")) {
				FREE(home);
				home = strdup(URL_HEAD);
				ret = SETTING_DNET_RETURN_NULL_HOMEURL;
			}
			(void) connection_profile_set_cellular_home_url(
					con_info_2, home);
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
		SETTING_TRACE("con_info is %p, con_info_2:%p", con_info,
				con_info_2);

		/*Add a new profile */
		connection_cellular_service_type_e type =
				CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
		(void) connection_profile_get_cellular_service_type(con_info,
				&type);
/*TODO: select proper connection[] */
		err = connection_add_profile(ad->connection[0], con_info);
		if (err != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR(
					"*** [ERR] connection_add_profile. "
					"err=%d ***",
					err);
			ret = SETTING_DNET_RETURN_ERR;
			setting_create_popup(ad, ad->win_get, NULL,
					_("IDS_CST_POP_FAILED"), NULL, 0, false,
					false, 0);
			return ret;
		}
/*TODO: select proper connection[] */
		connection_profile_h tmp_profile = NULL;
		(void) connection_get_default_cellular_service_profile(
				ad->connection[0], type, &tmp_profile);
		/*
		 => if there is already default profile, after_profile will be
		 handle of default profile.
		 => there is no default profile, after_profile will be handle
		 of the profile which we have added just before.
		 */
/*TODO: select proper connection[] */
		if (setting_network_equal_profile(con_info, tmp_profile)) {
			SETTING_TRACE_ERROR("Going to set profile");
			(void) connection_set_default_cellular_service_profile(
					ad->connection[0], type, tmp_profile);
		} else {
			/*nothing to do */
		}

		(void) connection_destroy(tmp_profile);

		(void) connection_profile_destroy(con_info);

		if (con_info_2) { /*just for MMS + internet */
			connection_cellular_service_type_e type =
					CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;
			(void) connection_profile_get_cellular_service_type(
					con_info_2, &type);
/*TODO: select proper connection[] */
			/*Add a new profile */
			err = connection_add_profile(ad->connection[0],
					con_info_2);
			if (err != CONNECTION_ERROR_NONE) {
				SETTING_TRACE_ERROR(
						"*** [ERR] connection_add_pro"
						"file. err=%d ***",
						err);
				ret = SETTING_DNET_RETURN_ERR;
				setting_create_popup(ad, ad->win_get, NULL,
						_("IDS_CST_POP_FAILED"), NULL,
						0, false, false, 0);
				return ret;
			}
/*TODO: select proper connection[] */
			connection_profile_h tmp_profile = NULL;
			(void) connection_get_default_cellular_service_profile(
					ad->connection[0], type, &tmp_profile);
			/*
			 => if there is already default profile, after_profile
			  will be handle of default profile.
			 => there is no default profile, after_profile will be
			  handle of the profile which we have added just before.
			 */
/*TODO: select proper connection[] */
			if (setting_network_equal_profile(con_info_2,
					tmp_profile)) {
				SETTING_TRACE_ERROR("Going to set profile");
				(void) connection_set_default_cellular_service_profile(
						ad->connection[0], type,
						tmp_profile);
			} else {
				/*nothing to do */
			}

			(void) connection_destroy(tmp_profile);
			(void) connection_profile_destroy(con_info_2);
		}
	} else {

		err = connection_update_profile(ad->connection[0], con_info);
		if (err != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR(
					"%s*** [ERR] net_modify_profile."
					" err=%d ***%s",
					SETTING_FONT_RED, err,
					SETTING_FONT_BLACK);
			ret = SETTING_DNET_RETURN_ERR;
			setting_create_popup(ad, ad->win_get,
			NULL, _("IDS_CST_POP_FAILED"), NULL, 0, false, false,
					0);
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

static void __save_response_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
	/*re-fetch connection info.. */
	SETTING_TRACE("ad->con_name:%s", ad->con_name);

	ad->apn_MMS = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_MMS,
			ad);

	/* Call automatically
	 * setting_network_connection_click_softkey_back_cb() by registering
	 * elm_naviframe_item_pop_cb_set() */
	elm_naviframe_item_pop(ad->navi_bar);
}

/**
 * [UI-CALLBACK] 'Save' button handler
 */
static void setting_network_connection_click_softkey_save_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

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
		SETTING_TRACE_ERROR("%s*** [ERR] __save_connection ***%s",
				SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	elm_object_tree_focus_allow_set(ad->con_create_gl, EINA_FALSE);

	setting_create_popup(ad, ad->win_get, NULL, KeyStr_Saved,
			__save_response_cb, 1, FALSE, FALSE, 0);

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}
}

static Eina_Bool setting_network_connection_click_softkey_back_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_TRUE, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	elm_object_tree_focus_allow_set(ad->con_create_gl, EINA_FALSE);

	SETTING_TRACE("ad->con_name:%s", ad->con_name);
	/*new a profile,need to select the "Service type" */
	if (!safeStrCmp(STR_SETTING_NEW_CONNECTIONS, ad->con_name)
			|| ad->profile_topper_view
					== &setting_view_network_con) {
		/*not normal view change,need do specially */
		setting_view_cleanup(&setting_view_network_connection_create,
				ad);
		setting_view_update(&setting_view_network_con, ad);
	} else {
		setting_view_change(&setting_view_network_connection_create,
				&setting_view_network_con_list, ad);
	}

	if (ctxpopup != NULL) {
		evas_object_del(ctxpopup);
		ctxpopup = NULL;
	}

	return EINA_TRUE;
}

static void setting_network_connection_click_softkey_back_ctx_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	setting_network_connection_click_softkey_back_cb(data, NULL);
}

static void setting_network_connection_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	/* Call automatically
	 * setting_network_connection_click_softkey_back_cb() by registering
	 * elm_naviframe_item_pop_cb_set() */
	elm_naviframe_item_pop(ad->navi_bar);
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
		if (ad->data_profile_name
				&& isEmptyStr(ad->data_profile_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s",
			 *  ad->data_profile_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_acs_name
				&& isEmptyStr(ad->data_acs_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s",
			 *  ad->data_acs_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_user_name
				&& isEmptyStr(ad->data_user_name->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s",
			 * ad->data_user_name->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_pwd && isEmptyStr(ad->data_pwd->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s",
			 *  ad->data_pwd->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
		if (ad->data_hm_url && isEmptyStr(ad->data_hm_url->sub_desc)) {
			/*SETTING_TRACE("entry_str:%s",
			 * ad->data_hm_url->sub_desc); */
			isFoundEmptyEntry = TRUE;
			break;
		}
	} while (0);
	Evas_Object *done_btn = elm_object_item_part_content_get(ad->navi_it,
			NAVI_BTN_STYLE_LEFT);
	if (isFoundEmptyEntry) {
		/*it had not yet been disabled */
		if (!evas_object_pass_events_get(done_btn)) {
			setting_disable_evas_object(done_btn);
			setting_dim_evas_object(done_btn, TRUE);
		} else {
			/*do nothing..if invoke setting_dim_evas_object many
			 * times, the done button will be dimer until it cannot
			 * be see. */
			/*refer to the API of setting_dim_evas_object().
			 * The API is supplied by Setting itself, winset
			 * doesn't support until now as I now. */
		}
	} else {
		setting_enable_evas_object(done_btn);
		setting_undo_dim_evas_object(done_btn, TRUE);
	}
}

static void setting_network_connection_display_auth_type(SettingNetworkUG *ad,
		int auth_type)
{
	SETTING_TRACE_BEGIN;
	retm_if(ad == NULL, "Data parameter is NULL");
	if (ad->chkType == auth_type)
		return;

	ad->chkType = auth_type; /* for update genlist */
	if (CONNECTION_CELLULAR_AUTH_TYPE_NONE != auth_type) { /* to show */
		SETTING_TRACE("ad->is_show_user:%d", ad->is_show_user);
		if (ad->is_show_user == 0) { /* do only when hiden */
			SETTING_TRACE("to create");

			ad->data_user_name = (Setting_GenGroupItem_Data *) calloc(
					1, sizeof(Setting_GenGroupItem_Data));
			setting_retm_if(ad->data_user_name == NULL,
					"calloc failed");
			ad->data_user_name->keyStr = (char *) g_strdup(
					KeyStr_UserID);
			ad->data_user_name->sub_desc = (char *) g_strdup(
					ad->ed_user_name_desc);
			ad->data_user_name->swallow_type =
					SWALLOW_Type_LAYOUT_EDITFIELD;
			ad->data_user_name->isSinglelineFlag = TRUE;
			ad->data_user_name->userdata = ad;

			ad->data_user_name->return_key_type =
					ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
			ad->data_user_name->activated_cb =
					setting_network_connection_entry_next_activated_cb;

			ad->data_user_name->maxlength_reached_cb =
					__network_max_len_reached;
			ad->data_user_name->disable_auto_cap = EINA_TRUE;

			__BACK_POINTER_SET(ad->data_user_name);

			ad->data_user_name->chk_change_cb =
					setting_network_connection_entry_changed_cb;
			ad->data_user_name->limit_filter_data = calloc(1,
					sizeof(Elm_Entry_Filter_Accept_Set));
			if (ad->data_user_name->limit_filter_data) {
				ad->data_user_name->limit_filter_data->max_byte_count =
						MAX_PDP_AUTH_USERNAME_LEN_MAX;
				ad->data_user_name->win_main = ad->win_get;
			}
			ad->data_user_name->item = elm_genlist_item_insert_after(
					ad->scl_edit, &itc_editfield,
					ad->data_user_name,
					NULL, ad->item_above_user_name,
					ELM_GENLIST_ITEM_NONE,
					NULL, ad);

			ad->data_pwd = (Setting_GenGroupItem_Data *) calloc(1,
					sizeof(Setting_GenGroupItem_Data));

			setting_retm_if(ad->data_pwd == NULL, "calloc failed");
			ad->data_pwd->keyStr = (char *) g_strdup(
					"IDS_ST_BODY_PASSWORD");
			ad->data_pwd->sub_desc = (char *) g_strdup(
					ad->ed_pwd_desc);
			ad->data_pwd->swallow_type =
					SWALLOW_Type_LAYOUT_EDITFIELD;
			ad->data_pwd->userdata = ad;
			ad->data_pwd->chk_change_cb =
					setting_network_connection_entry_changed_cb;
			ad->data_pwd->isPasswordFlag = TRUE;
			ad->data_pwd->isSinglelineFlag = TRUE;

			ad->data_pwd->return_key_type =
					ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
			ad->data_pwd->activated_cb =
					setting_network_connection_entry_next_activated_cb;

			ad->data_pwd->maxlength_reached_cb =
					__network_max_len_reached;
			ad->data_pwd->disable_auto_cap = EINA_TRUE;

			__BACK_POINTER_SET(ad->data_pwd);

			/* ad->data_pwd->chk_change_cb = chk_change_cb; */
			ad->data_pwd->limit_filter_data = calloc(1,
					sizeof(Elm_Entry_Filter_Accept_Set));
			if (ad->data_pwd->limit_filter_data) {
				ad->data_pwd->limit_filter_data->max_byte_count =
						MAX_PDP_AUTH_PASSWORD_LEN_MAX;
				ad->data_pwd->win_main = ad->win_get;
			}
			ad->data_pwd->item = elm_genlist_item_insert_after(
					ad->scl_edit, &itc_editfield,
					ad->data_pwd, NULL,
					ad->data_user_name->item,
					ELM_GENLIST_ITEM_NONE,
					NULL, ad);

			ad->item_above_proxy_add = ad->data_pwd->item;
			/*item = st_item_insert_after(ad->scl_edit,
			 &itc_seperator,
			 NULL, NULL,
			 ad->data_pwd->item,
			 ELM_GENLIST_ITEM_NONE,
			 NULL, NULL);
			 elm_genlist_item_select_mode_set
			 (item , ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

			ad->has_form_changed = TRUE;
			setting_network_connection_check_entry_empty(ad);
			elm_genlist_item_select_mode_set(
					ad->data_user_name->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			elm_genlist_item_select_mode_set(ad->data_pwd->item,
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			/* ad->is_show_user = 1; */
		}
		ad->is_show_user = 1;
	} else { /* to hide */

		SETTING_TRACE("ad->is_show_user:%d", ad->is_show_user);
		if (ad->is_show_user == 1) { /* do only when shown */
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
		ad->has_form_changed = TRUE;
		setting_network_connection_check_entry_empty(ad);
		ad->is_show_user = 0;

	}
}

static void setting_network_connection_entry_changed_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	SettingNetworkUG *ad = list_item->userdata;
	const char *entry_str = elm_entry_entry_get(obj);

	if (list_item->maxLengthReachFlag
			&& list_item->enterKeyPressFlag == false) {
#if 0
		int ret = notification_status_message_post(
				_("IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"));
		if (ret != NOTIFICATION_ERROR_NONE)
		SETTING_TRACE_ERROR(
				"notification_status_message_post() failed(%d)",
				ret);
		ad->popup = setting_create_popup(ad, ad->win_get, NULL,
				"IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED",
				NULL, 2, TRUE, FALSE, 0);
		elm_object_focus_set(list_item->eo_check, EINA_FALSE);
#endif
	}
	list_item->maxLengthReachFlag = FALSE;
	list_item->enterKeyPressFlag = FALSE;
	/*G_FREE(list_item->sub_desc);//release first */
	list_item->sub_desc = (char *) g_strdup(entry_str);
	ad->has_form_changed = TRUE;
	setting_network_connection_check_entry_empty(ad);
}
static void setting_network_connection_hide_input_pannel(void *data)
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

