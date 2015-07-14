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
#include <setting-network-con.h>
/*#include <tethering.h> */

static int setting_network_con_create(void *cb);
static int setting_network_con_destroy(void *cb);
static int setting_network_con_update(void *cb);
static int setting_network_con_cleanup(void *cb);

setting_view setting_view_network_con = {
	.create = setting_network_con_create,
	.destroy = setting_network_con_destroy,
	.update = setting_network_con_update,
	.cleanup = setting_network_con_cleanup,
};

static Eina_Bool __add_other_apn(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *)data;

	/*show other APN whose type is not internet/APN */
	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;

	char *profile_name = NULL;
	char *apn = NULL;
	bool profile_bhidden = false;
	connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;

	int rv = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
		ad->add_apn_timer = NULL;
		return FALSE;
	}

	int cnt = 0;
	int item_cnt = 0;
	Elm_Object_Item *last_item = NULL;

	Setting_GenGroupItem_Data *item_data = NULL;

	int	before_groupstyle = SETTING_GROUP_STYLE_NONE;

	while (connection_profile_iterator_has_next(profile_iter)) {
		if (cnt >= MAX_PROFILE_NUM) {
			break;
		}
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

		if (connection_profile_get_name(profile_h, &profile_name) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile name");
			continue;
		}

		if (connection_profile_is_cellular_hidden(profile_h, &profile_bhidden) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile hidden attr (true/false)");
			continue;
		}

		if (connection_profile_get_cellular_apn(profile_h, &apn) != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get apn");
			G_FREE(profile_name);
			continue;
		}

		SETTING_TRACE("Record[%d:%s], profile_name:%s, profile_type:%d, service_type:%d",
		              cnt, apn, profile_name, profile_type, service_type);
		if (CONNECTION_PROFILE_TYPE_CELLULAR != profile_type) {
			G_FREE(apn);
			G_FREE(profile_name);
			continue;
		}
		if (profile_bhidden == false
		    && CONNECTION_CELLULAR_SERVICE_TYPE_MMS != service_type
		    && CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET != service_type) { /*unknow, match with ad->con_name */
			if (before_groupstyle == SETTING_GROUP_STYLE_NONE) {
				last_item = elm_genlist_item_append(ad->connections_gl, &itc_seperator,
				                                    NULL, NULL,
				                                    ELM_GENLIST_ITEM_NONE, NULL, NULL);
				elm_genlist_item_select_mode_set(last_item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			}

#ifdef SUPPORT_REVERSED_ORDER
			item_data = setting_create_Gendial_field_def(ad->connections_gl, &itc_1text,
			                                             setting_network_con_item_Gendial_mouse_up_cb,
			                                             ad, SWALLOW_Type_INVALID, NULL, NULL,
			                                             0, apn, NULL,
			                                             NULL);
#else
			item_data = calloc(1, sizeof(Setting_GenGroupItem_Data));
			setting_retvm_if(!item_data, FALSE, "calloc failed");
			item_data->keyStr = (char *)g_strdup(profile_name);
			item_data->sub_desc = (char *)g_strdup(apn);
			item_data->swallow_type = SWALLOW_Type_INVALID;

			item_data->item = elm_genlist_item_insert_after(ad->connections_gl,
			                                                &itc_2text, item_data, NULL, last_item,
			                                                ELM_GENLIST_ITEM_NONE,
			                                                setting_network_con_item_Gendial_mouse_up_cb, ad);

			last_item = item_data->item;
#endif

			item_data->chk_status = service_type;/*record the service type */
			item_cnt++;

			if (before_groupstyle == SETTING_GROUP_STYLE_NONE) {
				elm_object_item_signal_emit(item_data->item, "elm,state,top", "");
				before_groupstyle = SETTING_GROUP_STYLE_TOP;
			} else if (before_groupstyle == SETTING_GROUP_STYLE_TOP) {
				elm_object_item_signal_emit(item_data->item, "elm,state,center", "");
				before_groupstyle = SETTING_GROUP_STYLE_CENTER;
			}

			SETTING_TRACE("profile hidden attribute : %d ", profile_bhidden);
		} else {
			SETTING_TRACE("profile hidden attribute : %d ", profile_bhidden);
		}
		G_FREE(apn);
		G_FREE(profile_name);
		cnt++;
	}

	if (item_cnt == 1) {
		elm_object_item_signal_emit(item_data->item, "elm,state,normal", "");
	} else if (item_cnt > 1) {
		elm_object_item_signal_emit(item_data->item, "elm,state,bottom", "");
	}

	ad->add_apn_timer = NULL;
	return FALSE;
}

static void __setting_network_con_genlist_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	elm_genlist_clear(ad->connections_gl);	/* first to clear list */

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(ad->connections_gl, &itc_seperator, NULL, NULL,
	                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* scroller is a genlist */

	/* [UI] Internet Connections */
	SETTING_TRACE("ad->apn_internet:%d, ad->apn_MMS:%d", ad->apn_internet, ad->apn_MMS);
	ad->apn_internet = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET, ad);
	Setting_GenGroupItem_Data *internet_conn = setting_create_Gendial_field_def(ad->connections_gl, &itc_2text_3,
	                                                                            setting_network_con_item_Gendial_mouse_up_cb,
	                                                                            ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                                                            0, "IDS_ST_BODY_INTERNET_CONNECTION",
	                                                                            g_strdup(ad->apn_internet), NULL);
	if (internet_conn) {
		/*internet_conn->group_style = SETTING_GROUP_STYLE_NONE; */
		setting_genlist_item_groupstyle_set(internet_conn, SETTING_GROUP_STYLE_NONE);
	} else {
		SETTING_TRACE_ERROR("internet_conn is NULL");
	}
	ad->internet_conn = internet_conn;

	if (internet_conn) {
		setting_genlist_item_groupstyle_set(internet_conn, SETTING_GROUP_STYLE_TOP);
	}
	ad->apn_MMS = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_MMS, ad);
	/* [MMS Connections] Internet Connections */
	Setting_GenGroupItem_Data *mms_conn = setting_create_Gendial_field_def(ad->connections_gl, &itc_2text_3,
	                                                                       setting_network_con_item_Gendial_mouse_up_cb,
	                                                                       ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                                                       0, STR_SETTING_MMS_CONNECTIONS,
	                                                                       g_strdup(ad->apn_MMS), NULL);
	if (mms_conn) {
		setting_genlist_item_groupstyle_set(mms_conn, SETTING_GROUP_STYLE_BOTTOM);
	} else {
		SETTING_TRACE_ERROR("mms_conn is NULL");
	}

	ad->mms_conn = mms_conn;


#ifdef ENABLED_PREFERRED_NETWORKS
	setting_create_Gendial_field_def(ad->connections_gl, &itc_1text,
	                                 setting_network_con_item_Gendial_mouse_up_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                 0, "IDS_ST_BODY_PREFERRED_NETWORKS",
	                                 NULL, NULL);
#endif
#ifdef ENABLED_2G3G_NETWORK
	/*  create 3g connection option */
	setting_create_Gendial_field_def(ad->connections_gl, &itc_1text,
	                                 setting_network_con_item_Gendial_mouse_up_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                 0, "2G/3G Connection", NULL, NULL);

#endif

	/* add other APN
	if (!ad->is_update) {
		if (ad->add_apn_timer) {
			ecore_idler_del(ad->add_apn_timer);
			ad->add_apn_timer = NULL;
		}
		ad->add_apn_timer = ecore_timer_add(0.5, (Ecore_Task_Cb) __add_other_apn, ad);
	} else {
		__add_other_apn(ad);
	}
	*/
	/*	__add_other_apn(ad); */
	SETTING_TRACE_END;

}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

/**
 * Connections
 */
static int setting_network_con_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
	         "Cannot set scroller object  as contento of layout");
//	elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	ad->connections_gl = scroller;
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	Elm_Object_Item *navi_it = setting_push_layout_navi_bar(_("IDS_ST_BODY_CONNECTIONS"),
	                                                        _("IDS_ST_BUTTON_BACK"),
	                                                        NULL,/*_(KeyStr_Reset_Def), */
	                                                        _(KeyStr_Reset_Def),/*_("IDS_ST_BODY_MYTHEME_CREATE"), */
	                                                        NULL,
	                                                        NULL,/*setting_network_con_click_softkey_reset_cb, */
	                                                        setting_network_con_click_softkey_reset_cb,/*setting_network_con_click_softkey_create_cb, */
	                                                        ad, scroller, ad->navi_bar, NULL);

	elm_naviframe_item_pop_cb_set(navi_it, setting_network_con_click_softkey_cancel_cb, ad);

	__setting_network_con_genlist_create(ad);

	setting_view_network_con.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	ad->is_update = 0;
	/*FREE(ad->con_name); */
	ad->connections_gl = NULL;
	if (setting_view_network_con.is_create) {
#if SUPPORT_TETHERING
		if (ad->th_connections) {
			SETTING_TRACE("tethering destruction ");
			tethering_destroy(ad->th_connections);
			ad->th_connections = NULL;
		}
#endif
		if (ad->add_apn_timer) {
			ecore_idler_del(ad->add_apn_timer);
			ad->add_apn_timer = NULL;
		}

		elm_naviframe_item_pop(ad->navi_bar);
		setting_view_network_con.is_create = 0;
	} else {
		SETTING_TRACE("why is this is_create == 0 ?? !!!");
	}


	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	__setting_network_con_genlist_create(ad);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_con_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return setting_network_con_destroy(cb);
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
setting_network_con_item_Gendial_mouse_up_cb(void *data, Evas_Object *obj,
                                             void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	ad->profile_topper_view = NULL;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	ad->profile_service_type = 0;

	if (!safeStrCmp("IDS_COM_BODY_NETWORK_OPERATORS", list_item->keyStr)) {
		setting_view_change(&setting_view_network_con,
		                    &setting_view_network_select_network,
		                    ad);
	}
#ifdef ENABLED_PREFERRED_NETWORKS
	else if (!safeStrCmp("IDS_ST_BODY_PREFERRED_NETWORKS",
	                     list_item->keyStr)) {

		int value = -1;
		int err = -1;

		setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value,
		                        &err);
		/** @todo check 'err' variable for exception handling */
		SETTING_TRACE("INT_SLP_SETTING_SIM_SLOT value:%d", value);
		switch (value) {
			case VCONFKEY_TELEPHONY_SIM_INSERTED:
				/* To Recreate the view in order to search the perferred network again. */
				/* ret = setting_view_destroy(&setting_view_network_preferred_network, ad); */

				setting_view_change(&setting_view_network_con,
				                    &setting_view_network_preferred_network,
				                    ad);

				break;

			case VCONFKEY_TELEPHONY_SIM_NOT_PRESENT:

				setting_create_simple_popup(NULL, ad->win_get,
				                            NULL, _(""));/*this code is not used now, so remove the ID which is not in po file*/
				SETTING_TRACE_DEBUG
				("%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
				 SETTING_FONT_RED, value, SETTING_FONT_BLACK);
				return;

				break;

			case VCONFKEY_TELEPHONY_SIM_CARD_ERROR:
			case VCONFKEY_TELEPHONY_SIM_UNKNOWN:

				setting_create_simple_popup(NULL, ad->win_get,
				                            NULL, _("IDS_SIM_BODY_INVALID_SIM_CARD"));
				SETTING_TRACE_DEBUG
				("%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
				 SETTING_FONT_RED, value, SETTING_FONT_BLACK);
				return;

				break;
			default:
				SETTING_TRACE("Unknown SIM information from vconf");
				break;
		}
	}
#endif
#ifdef ENABLED_2G3G_NETWORK
	else if (!safeStrCmp("2G/3G Connection", list_item->keyStr)) {
		setting_view_change(&setting_view_network_con,
		                    &setting_view_network_3gcon, ad);
	}
#endif
	else if (!safeStrCmp("IDS_ST_BODY_INTERNET_CONNECTION", list_item->keyStr)) {/*connections.. */

		FREE(ad->con_name);
		SETTING_TRACE("list_item->keyStr:%s, ad->con_name:%s", list_item->keyStr, ad->con_name);
		ad->con_name = strdup(list_item->keyStr);
		ad->profile_service_type = CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET;
		setting_view_change(&setting_view_network_con,
		                    &setting_view_network_con_list,
		                    ad);
	} else if (!safeStrCmp(STR_SETTING_MMS_CONNECTIONS, list_item->keyStr)) {
		FREE(ad->con_name);
		ad->profile_topper_view = &setting_view_network_con;
		ad->con_name = strdup(STR_SETTING_MMS_CONNECTIONS);
		ad->access_name = strdup(STR_SETTING_MMS_CONNECTIONS);
		ad->profile_service_type = CONNECTION_CELLULAR_SERVICE_TYPE_MMS;
		setting_view_create(&setting_view_network_connection_create, ad);
	} else {
		FREE(ad->con_name);
		ad->profile_topper_view = &setting_view_network_con;
		ad->con_name = strdup(list_item->sub_desc);
		ad->access_name = strdup(list_item->sub_desc);
		ad->profile_service_type = list_item->chk_status;
		setting_view_create(&setting_view_network_connection_create, ad);
	}
	SETTING_TRACE_END;
}

static void
setting_network_con_click_softkey_reset_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	ad->popup_conreset = setting_create_popup_with_btn(ad, ad->win_get,
	                                                   NULL, _("IDS_ST_POP_RESET_TO_DEFAULT_Q"),
	                                                   setting_network_con_reset_popup_cb,
	                                                   0, 2, _("IDS_ST_BUTTON_OK"), _("IDS_ST_BUTTON_CANCEL_ABB"));

	return;
}

void _reset_cb(connection_error_e result, void *user_data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == user_data, "NULL == data");
	SettingNetworkUG *ad = (SettingNetworkUG *) user_data;

	if (ad->network_ug_pop) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}
	if (result == CONNECTION_ERROR_NONE) {
		SETTING_TRACE("reset successfully");
		FREE(ad->apn_internet);
		FREE(ad->apn_MMS);
		__setting_network_con_genlist_create(ad);
	} else {
		SETTING_TRACE_ERROR("failed to reset ret = %d", result);
	}
}

static void
setting_network_con_reset_popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	setting_retm_if(NULL == data, "NULL == data");

	int	response_type = btn_type(obj);
	int ret = 0;

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (ad) {
		evas_object_del(ad->popup_conreset);
		ad->popup_conreset = NULL;
	}

	if (response_type == POPUP_RESPONSE_OK) {
		/*setting_create_simple_popup(ad, ad->win_get, NULL, _("IDS_IM_BODY_SUCCESSFULLY_COMPLETED")); */
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_IM_BODY_SUCCESSFULLY_COMPLETED"),
		                                 NULL, POPUP_INTERVAL, TRUE, FALSE);

		if (ad->connection) {
			ret = connection_reset_profile(ad->connection, CONNECTION_RESET_DEFAULT_PROFILE,
			                               0, _reset_cb, ad);
			if (CONNECTION_ERROR_NONE == ret) {
				SETTING_TRACE("Reset successfully");
			} else {
				SETTING_TRACE("Failed to reset, ret = %d", ret);
			}
		}
	}

	return;
}

#if 0
static void
setting_network_con_click_softkey_create_cb(void *data, Evas_Object *obj,
                                            void *event_info)
{
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE_BEGIN;
	/*setting_create_simple_popup(ad, ad->win_get, NULL, _("UX Undefined")); */
	/*return; */

	/*to add a new connection */
	/*ad->con_type = -1; */
	FREE(ad->con_name);
	ad->con_name = strdup(STR_SETTING_NEW_CONNECTIONS);
	ad->profile_topper_view = &setting_view_network_con;
	/*not normal view realtion,so need to change manully */
	/*setting_view_change(&setting_view_network_con, &setting_view_network_connection_create, ad); */
	setting_view_create(&setting_view_network_connection_create, ad);
}
#endif

static Eina_Bool
setting_network_con_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_con,
	                    &setting_view_network_main, ad);
	return EINA_TRUE;
}
