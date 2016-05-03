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
#include <setting-network.h>
#include <setting-debug.h>
#include <setting-cfg.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif


const char *STR_SETTING_OPERATION_FAILED = "IDS_BT_POP_OPERATION_FAILED";
static Evas_Object *__create_registering_popup(void *data);

char *__get_profile_name(int conType, void *data)
{
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	char *def_name = NULL;
	if (CONNECTION_CELLULAR_SERVICE_TYPE_MMS == conType) {
		connection_profile_iterator_h profile_iter = NULL;
		connection_profile_h profile_h = NULL;

		char *profile_id = NULL;
		char *profile_name = NULL;
		char *apn = NULL;
		bool profile_bhidden = false;
		connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
		connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;

		int rv = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
		if (rv != CONNECTION_ERROR_NONE) {
			SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
			return NULL;
		}

		int cnt = 0;
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

			connection_profile_get_id(profile_h, &profile_id);

			SETTING_TRACE("Record[%d:%s] profile_handle:%s, profile_id:%s, profile_name:%s, profile_type:%d, service_type:%d",
						  cnt, apn, profile_h, profile_id, profile_name, profile_type, service_type);
			if (CONNECTION_PROFILE_TYPE_CELLULAR != profile_type) {
				G_FREE(apn);
				G_FREE(profile_id);
				G_FREE(profile_name);
				continue;
			}
			if (profile_bhidden == false
				&& CONNECTION_CELLULAR_SERVICE_TYPE_MMS == service_type) { /*unknow, match with ad->con_name */
				FREE(def_name); /*keep the last one */
				def_name = strdup(profile_name);
			} else {
				SETTING_TRACE("profile hidden attribute : %d ", profile_bhidden);
			}
			G_FREE(apn);
			G_FREE(profile_id);
			G_FREE(profile_name);
			cnt++;
		}

		return def_name != NULL ? def_name : strdup(KeyStr_No_Profile);
	}

	connection_profile_h def_profile = NULL;
	(void)connection_get_default_cellular_service_profile(ad->connection,
														  conType, &def_profile);
	if (def_profile) connection_profile_get_name(def_profile, &def_name);

	def_name = def_name != NULL ? def_name : strdup(KeyStr_No_Profile);
	return def_name;
}

static void __notify_response_cb(void *data, Evas_Object *obj,
								 void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(!data, "!data");
	SettingNetworkUG *ad = data;
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

static void __selected_network_change_cb(keynode_t *key, void *data)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retm_if(!data, "!data");
	SettingNetworkUG *ad = data;
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_NWNAME)
		|| !safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_SPN_DISP_CONDITION)
		|| !safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_SPN_NAME)
		|| !safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_SVCTYPE)
	   ) {
		ad->sel_network = get_pa_select_network();
		if (!ad->sel_network) {
			ad->sel_network = strdup(Keystr_UNKNOWN_NETWORK);
		}

		if (0 != vconf_set_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, ad->sel_net)) {
			SETTING_TRACE_ERROR("vconf[%s] set failed");
		}

		if (0 != vconf_set_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT, ad->sel_act)) {
			SETTING_TRACE_ERROR("vconf[%s] set failed");
		}

		setting_network_update_sel_network(ad);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_FLIGHT_MODE)) {
		SETTING_TRACE("vconf_name:%s", vconf_name);
		setting_create_popup(ad, ad->ly_main, NULL, "IDS_ST_BODY_FLIGHT_MODE_HAS_BEEN_ENABLED_NETWORK_SETTINGS_WILL_CLOSE",
							 __notify_response_cb,
							 POPUP_INTERVAL * 2, TRUE, FALSE, 0);

	}
}


static void setting_network_ug_cb_resize(void *data, Evas *e,
										 Evas_Object *obj, void *event_info)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	setting_view_update(&setting_view_network_main, ad);
}

static setting_view *__get_network_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");
	SettingNetworkUG *networkUG = (SettingNetworkUG *)data;

	setting_view_node_table_intialize();

	if (0 == safeStrCmp(networkUG->view_type_string, "netsearch")) {
		setting_view_node_table_register(&setting_view_network_select_network, NULL);
		return &setting_view_network_select_network;

	} else {
		setting_view_node_table_register(&setting_view_network_main, NULL);
		setting_view_node_table_register(&setting_view_network_select_network, &setting_view_network_main);

		setting_view_node_table_register(&setting_view_network_con, &setting_view_network_main);
		setting_view_node_table_register(&setting_view_network_con_list, &setting_view_network_con);
		setting_view_node_table_register(&setting_view_network_connection_create, &setting_view_network_con_list);
		setting_view_node_table_register(&setting_view_network_profile_delete, &setting_view_network_con_list);

		return &setting_view_network_main;
	}
}



void ___popup_view_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int err = 0;		/* error check	 */
	int response_type = btn_type(obj);

	int value = 0;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, &value, &err);
	setting_retm_if(err != 0, "get vconf failed");
	SETTING_TRACE("value = %d", value);
	if (POPUP_RESPONSE_OK == response_type) {
		if (value == 0) {
			setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
									 SETTING_ON_OFF_BTN_ON, &err);
			setting_retm_if(err != 0, "set vconf failed");
			setting_update_gl_item_chk_status(ad->data_mobile_data, 1);
		} else {
			setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
									 SETTING_ON_OFF_BTN_OFF, &err);
			setting_retm_if(err != 0, "set vconf failed");

			setting_update_gl_item_chk_status(ad->data_mobile_data, 0);

			/* should set data_roming as same as status of use_packet_data */
			int data_roaming_value = 0;
			setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (data_roaming_value) {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING,
										 SETTING_ON_OFF_BTN_OFF, &err);
				setting_retm_if(err != 0, "set vconf failed");
				setting_update_gl_item_chk_status(ad->data_roaming, SETTING_ON_OFF_BTN_OFF);
			}
		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		/* rollback */
		setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
								 value, &err);
		setting_retm_if(err != 0, "set vconf failed");
		setting_update_gl_item_chk_status(ad->data_mobile_data, value);
	}
#if 0
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
#else

	evas_object_hide(ad->popup);
	SETTING_TRACE("ad->ug:%p", ad->ug);
	ug_destroy_me(ad->ug);
#endif
}

static void *___load_popup_view(ui_gadget_h ug,
								enum ug_mode mode, app_control_h service,
								void *priv)
{
	SettingNetworkUG *ad = priv;
	char *viewtype = NULL;
	app_control_get_extra_data(service, "viewtype", &viewtype);

	if (!safeStrCmp(viewtype, "mobile_data")) {
		int status = SETTING_ON_OFF_BTN_OFF;
		int err;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, &status, &err);
		if (status) {
			/*set to off */
			ad->popup =
				setting_create_popup(ad, ad->win_get,
									 _(KeyStr_Network_Turn_Off_Mobile_Data),
									 _(KeyStr_Network_Mobile_Data_Has_Been_Disabled_Msg),
									 ___popup_view_resp_cb,
									 0, FALSE, FALSE,
									 2, keyStr_OK, keyStr_CANCEL);
		} else {
			/*set to on */
			ad->popup =
				setting_create_popup(ad, ad->win_get,
									 NULL, _(Data_packet_Popup_desc),
									 ___popup_view_resp_cb,
									 0, FALSE, FALSE,
									 2, _("IDS_ST_SK_YES"), _("IDS_ST_SK_NO"));
		}

	}
	if (viewtype) {
		FREE(viewtype);
	}
	return ad->popup;
}

static void *setting_network_ug_on_create(ui_gadget_h ug,
										  enum ug_mode mode, app_control_h service,
										  void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingNetworkUG *networkUG = priv;
	networkUG->ug = ug;

	networkUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	networkUG->win_get = (Evas_Object *) ug_get_window();

	evas_object_show(networkUG->win_main_layout);
	networkUG->evas = evas_object_evas_get(networkUG->win_main_layout);

	setting_retvm_if(networkUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");
#if 1
	Evas_Object *popup_view = ___load_popup_view(ug, mode, service, priv);
	if (popup_view) {
		return popup_view;
	}
#endif

	/* init */
	ecore_imf_init();

	/*pass NULL to let TAPI access default module */
	networkUG->handle =	 tel_init(NULL);
	if (!networkUG->handle) {
		SETTING_TRACE_DEBUG("%s*** [ERR] tel_init. ***%s",
							SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	if (CONNECTION_ERROR_NONE != connection_create(&(networkUG->connection))) {
		SETTING_TRACE_ERROR("***Failed to connection_create.***");
	}

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	networkUG->view_type_string = NULL;
	app_control_get_extra_data(service, "viewtype", &(networkUG->view_type_string));
	networkUG->view_to_load = __get_network_view_to_load(networkUG, service);
	/*SETTING_TRACE("ad->whitelist_doc:%p", networkUG->whitelist_doc); */
	/*SETTING_TRACE("ad->whitelist_root_node:%p", networkUG->whitelist_root_node); */

	/*Read information from vconf, need not check whitelist in Settings */
	/*load_XML(WHITELIST_DIR_PATH, "whiteLists", &(networkUG->whitelist_doc),&(networkUG->whitelist_root_node)); */

	/*SETTING_TRACE("ad->whitelist_doc:%p", networkUG->whitelist_doc); */
	/*SETTING_TRACE("ad->whitelist_root_node:%p", networkUG->whitelist_root_node); */

	/*	creating a view. */
	setting_view_node_set_cur_view(networkUG->view_to_load);
	/*fix load network ug repeatedly issue */
	if (networkUG->view_to_load == &setting_view_network_main
		&& networkUG->view_to_load->is_create) {
		networkUG->view_to_load->is_create = 0;
	}
	setting_view_create(networkUG->view_to_load, (void *)networkUG);


	/* register view node table */
#ifdef UI_NETWORK_MODE
	if (tel_get_network_mode(networkUG->handle, setting_tapi_get_band_cb, networkUG) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("*** [ERR] tel_get_network_band. ***");
	}
#endif

	if (tel_get_network_selection_mode(networkUG->handle, setting_tapi_get_plmn_mode_cb, networkUG) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("*** [ERR] tel_get_network_selection_mode. ***");
	}

	if (tel_get_network_serving(networkUG->handle, setting_tapi_get_serving_network_cb, networkUG) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("*** [ERR] tel_get_network_serving. ***");
	}

	evas_object_event_callback_add(networkUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_network_ug_cb_resize, networkUG);

	(void)vconf_notify_key_changed(VCONFKEY_TELEPHONY_NWNAME, __selected_network_change_cb, networkUG);
	(void)vconf_notify_key_changed(VCONFKEY_TELEPHONY_SPN_DISP_CONDITION, __selected_network_change_cb, networkUG);
	(void)vconf_notify_key_changed(VCONFKEY_TELEPHONY_SPN_NAME, __selected_network_change_cb, networkUG);
	(void)vconf_notify_key_changed(VCONFKEY_TELEPHONY_SVCTYPE, __selected_network_change_cb, networkUG);
	(void)vconf_notify_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, __selected_network_change_cb, networkUG);

	return networkUG->ly_main;
}


static void setting_network_ug_on_start(ui_gadget_h ug, app_control_h service,
										void *priv)
{
	setting_retm_if((!priv), "!priv");

	SettingNetworkUG *ad = priv;
	ad->apn_internet = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_INTERNET, ad);
	ad->apn_MMS = __get_profile_name(CONNECTION_CELLULAR_SERVICE_TYPE_MMS, ad);

}

static void setting_network_ug_on_pause(ui_gadget_h ug, app_control_h service,
										void *priv)
{
}

static void setting_network_ug_on_resume(ui_gadget_h ug, app_control_h service,
										 void *priv)
{
}

static void setting_network_ug_on_destroy(ui_gadget_h ug, app_control_h service,
										  void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingNetworkUG *networkUG = priv;
	/*unload_XML(&(networkUG->whitelist_doc), &(networkUG->whitelist_root_node)); */

	(void)vconf_ignore_key_changed(VCONFKEY_TELEPHONY_NWNAME, __selected_network_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SPN_DISP_CONDITION, __selected_network_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SPN_NAME, __selected_network_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_TELEPHONY_SVCTYPE, __selected_network_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, __selected_network_change_cb);

	evas_object_event_callback_del(networkUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_network_ug_cb_resize);	/* fix flash issue for gallery */
	networkUG->ug = ug;

	if (networkUG->view_type_string)
		FREE(networkUG->view_type_string);
	G_FREE(networkUG->ed_pxy_addr_desc);
	G_FREE(networkUG->ed_pxy_port_desc);
	FREE(networkUG->sel_network);
	FREE(networkUG->access_name);
	FREE(networkUG->con_name);
	FREE(networkUG->apn_internet);
	FREE(networkUG->apn_MMS);
	/* release */
	ecore_imf_shutdown();

	/*	unregister dnet client */
	if (networkUG->connection) {
		connection_destroy(networkUG->connection);
		networkUG->connection = NULL;
	}

	if (networkUG->handle && tel_deinit(networkUG->handle) != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] setting_network_unsubscribe_tapi_events. ***%s",
		 SETTING_FONT_RED, SETTING_FONT_BLACK);
	}
	if (networkUG->profile_list != NULL) {
		eina_list_free(networkUG->profile_list);
		networkUG->profile_list = NULL;
	}

	setting_network_popup_delete(networkUG);
	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_network_select_network, networkUG);

	setting_view_destroy(&setting_view_network_con, networkUG);
	setting_view_destroy(&setting_view_network_con_list, networkUG);
	setting_view_destroy(&setting_view_network_connection_create,
						 networkUG);
	setting_view_destroy(&setting_view_network_profile_delete,
						 networkUG);
	setting_view_destroy(&setting_view_network_main, networkUG);

	if (NULL != ug_get_layout(networkUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(networkUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(networkUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_network_ug_on_message(ui_gadget_h ug, app_control_h msg,
										  app_control_h service, void *priv)
{
}

static void __update_mobile_data_on_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	ret_if(NULL == ad->popup_data_on);

	elm_object_text_set(ad->popup_data_on, _(Data_packet_Popup_desc));
}

static void __update_mobile_data_off_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	ret_if(NULL == ad->popup_data_off);

	char *pop_info = NULL;
	char *pop_title = NULL;

	pop_info = (char *)g_strdup(_(KeyStr_Network_Mobile_Data_Has_Been_Disabled_Msg));
	pop_title = (char *)g_strdup(_(KeyStr_Network_Turn_Off_Mobile_Data));

	elm_object_text_set(ad->popup_data_off, _(pop_info));

	if (pop_title) {
		elm_object_part_text_set(ad->popup_data_off, "title,text", _(pop_title));
	}
	G_FREE(pop_title);
	G_FREE(pop_info);
}

static void setting_network_ug_on_event(ui_gadget_h ug,
										enum ug_event event, app_control_h service,
										void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) priv;

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		if (ad->genlist && setting_view_network_main.is_create) {
			/*update items */
			elm_genlist_realized_items_update(ad->genlist);
			Elm_Object_Item *item = NULL;
			Setting_GenGroupItem_Data *item_data = NULL;
			if (ad->data_mobile_data) {
				item = elm_genlist_item_next_get(ad->data_mobile_data->item);
				if (item) {
					item = elm_genlist_item_next_get(item);
				}
				if (item) {
					item_data = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
				}
			}
			if (item_data) {
				const char *title = Data_packet_Sub_desc;
				item_data->keyStr = strdup(title);
				elm_object_item_data_set(item_data->item, item_data);
				elm_genlist_item_update(item_data->item);
			}
#ifdef UI_NETWORK_MODE
			/*update sub text */
			if (ad->handle && tel_get_network_mode(ad->handle, setting_tapi_get_band_cb, ad) != TAPI_API_SUCCESS) {
				SETTING_TRACE_ERROR("*** [ERR] tel_get_network_band. ***");
			}
			setting_network_update_sel_network(ad);
#endif
			/*update title */
			Elm_Object_Item *navi_it = elm_naviframe_bottom_item_get(ad->navi_bar);
			ret_if(!navi_it);
			elm_object_item_text_set(navi_it, _("IDS_ST_BODY_NETWORK"));
			/*update popup */
			__update_mobile_data_on_popup(ad);
			__update_mobile_data_off_popup(ad);
		}
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_network_ug_on_key_event(ui_gadget_h ug,
											enum ug_key_event event,
											app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
			/* setting_network_popup_delete(ad); */
			if (elm_naviframe_top_item_get(ad->navi_bar) ==
				elm_naviframe_bottom_item_get(ad->navi_bar)) {
				ug_destroy_me(ug);
			} else {
				/* elm_naviframe_item_pop(ad->navi_bar); */
				setting_view_cb_at_endKey(ad);
			}
		}
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *networkUG = calloc(1, sizeof(SettingNetworkUG));
	setting_retvm_if(!networkUG, -1, "Create SettingNetworkUG obj failed");

	ops->create = setting_network_ug_on_create;
	ops->start = setting_network_ug_on_start;
	ops->pause = setting_network_ug_on_pause;
	ops->resume = setting_network_ug_on_resume;
	ops->destroy = setting_network_ug_on_destroy;
	ops->message = setting_network_ug_on_message;
	ops->event = setting_network_ug_on_event;
	ops->key_event = setting_network_ug_on_key_event;
	ops->priv = networkUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingNetworkUG *networkUG;
	setting_retm_if(!ops, "ops == NULL");

	networkUG = ops->priv;
	FREE(networkUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
void setting_network_popup_delete(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL,
			"setting_network_tapi_popup_create Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	if (ad->network_select_registering_pop != NULL) {
		evas_object_del(ad->network_select_registering_pop);
		ad->network_select_registering_pop = NULL;
	}
	if (ad->network_ug_pop != NULL) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}
	if (ad->popup != NULL) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
#ifdef UI_NETWORK_MODE
/**
 * @see also setting_tapi_set_band_cb
 * @see also tel_get_network_mode
 */
void setting_tapi_get_band_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x", result);
	ret_if(!user_data);
	SettingNetworkUG *ad = user_data;

	if (ad->network_ug_pop) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}

	if (result == TAPI_NETWORK_NO_ERR) {
		int band;
		int temp_net_mode = 0;
		memcpy(&band, data, sizeof(int));

		SETTING_TRACE("tel_get_network_mode() callback received, band=%d", band);

		temp_net_mode = band;

		/*	***BEGIN***	 Fixed the problem of word twinkle	SAMSUNG 2010/7/21 modify */
		ad->net_mode = temp_net_mode;

		const char *sub_desc = NULL;
		/* Need to fix */
		char *szCscFeatureValue = "all_numeric";

		switch (ad->net_mode) {
		case TAPI_NETWORK_MODE_AUTO:
		case TAPI_NETWORK_MODE_LTE|TAPI_NETWORK_MODE_WCDMA|TAPI_NETWORK_MODE_GSM:
			if (is_lte_on_feature(ad)) {
				if (strcmp(szCscFeatureValue, "all_numeric") == 0)
					sub_desc = _("IDS_ST_OPT_4G_3G_2G_HAUTO_CONNECT");
				else
					sub_desc = _("IDS_ST_OPT_LTE_WCDMA_GSM_HAUTO_CONNECT");
			}
			break;
		case TAPI_NETWORK_MODE_WCDMA|TAPI_NETWORK_MODE_GSM:
			if (strcmp(szCscFeatureValue, "all_numeric") == 0)
				sub_desc = _("IDS_ST_OPT_3G_2G_HAUTO_CONNECT");
			else
				sub_desc = _("IDS_ST_OPT_WCDMA_GSM_HAUTO_CONNECT");
			break;
		case TAPI_NETWORK_MODE_WCDMA:
			if (strcmp(szCscFeatureValue, "all_numeric") == 0)
				sub_desc = _("IDS_ST_BODY_3G_ONLY");
			else
				sub_desc = _("IDS_ST_MBODY_WCDMA_ONLY");
			break;
		case TAPI_NETWORK_MODE_GSM:
			if (strcmp(szCscFeatureValue, "all_numeric") == 0)
				sub_desc = _("IDS_ST_OPT_2G_ONLY");
			else
				sub_desc = _("IDS_ST_MBODY_GSM_ONLY");
			break;
		/* for docomo */
		case TAPI_NETWORK_MODE_LTE|TAPI_NETWORK_MODE_WCDMA:
			sub_desc = _(Keystr_NetBrand_LTE_WCDMA);
			break;
		default:
			break;
		}

		if (ad->data_net_mode) {
			ad->data_net_mode->sub_desc = (char *)g_strdup(sub_desc);
			elm_object_item_data_set(ad->data_net_mode->item, ad->data_net_mode);
			elm_genlist_item_update(ad->data_net_mode->item);
			elm_genlist_item_expanded_set(ad->data_net_mode->item, FALSE);
		}

		vconf_set_int(VCONFKEY_SETAPPL_NETWORK_MODE_INT, ad->net_mode);
		/*} */
		/*	****END****	 Fixed the problem of word twinkle	SAMSUNG 2010/7/21 modify */
	} else {
		SETTING_TRACE("tel_get_network_mode() callback error(%d)", result);
	}
}

/**
 * @see also tel_set_network_mode : registers this callback function
 */
void setting_tapi_set_band_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x	 , %d", result, result);
	ret_if(!user_data);
	SettingNetworkUG *ad = user_data;
	ret_if(NULL == ad->data_net_mode);
	if (ad->network_ug_pop) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}

	if (tel_get_network_mode(ad->handle, setting_tapi_get_band_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("*** [ERR] tel_get_network_band. ***");
	}
}
#endif
void __back_to_previous_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}

	/*go back to previous view if set successfully */
	setting_view_change(
		&setting_view_network_select_network,
		&setting_view_network_main, ad);
}

/**
 * @see also __register_network
 */
void setting_tapi_set_plmn_mode_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x", result);
	ret_if(!user_data);
	SettingNetworkUG *ad = user_data;

	setting_network_popup_delete(ad);

	if ((TAPI_NETWORK_SELECTIONMODE_AUTOMATIC == ad->sel_net
		 && !ad->b_set_auto_network) ||
		(TAPI_NETWORK_SELECTIONMODE_MANUAL == ad->sel_net
		 && !ad->b_set_manul_network)) {
		/*The requeset has already been canceled, return directly*/
		SETTING_TRACE("The requeset has already been canceled, return directly");
		return;
	}

	if (result != TAPI_NETWORK_NO_ERR) { /*roolback..*/
		if (TAPI_NETWORK_SELECTIONMODE_MANUAL == ad->sel_net
			&& ad->data_auto_network_item && !ad->data_auto_network_item->chk_status) {
			SETTING_TRACE("Need refreshing");
			setting_create_popup(ad, ad->win_get, NULL, keyStr_Failed_Select_Network, NULL, 2 * POPUP_INTERVAL, FALSE, FALSE, 0);
			elm_radio_value_set(ad->chk_sel, -2);
		}

		return;
	}

	if (0 != vconf_set_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, ad->sel_net)) {
		SETTING_TRACE_ERROR("vconf[%s] set failed");
	}
	if (0 != vconf_set_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT, ad->sel_act)) {
		SETTING_TRACE_ERROR("vconf[%s] set failed");
	}

	ad->b_set_auto_network = FALSE;
	ad->b_set_manul_network = FALSE;
	setting_network_update_sel_network(ad);

	Evas_Object *popup = elm_popup_add(ad->win_get);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
									 EVAS_HINT_EXPAND);
	char r_str_text[128] = { 0, };
	snprintf(r_str_text, 128, "<align=left>");
	g_strlcat(r_str_text, _("IDS_ST_POP_REGISTERED"), 128);
	g_strlcat(r_str_text, "</align>", 128);
	elm_object_text_set(popup, r_str_text);
	elm_popup_timeout_set(popup, 2.0);
	evas_object_smart_callback_add(popup, "timeout", __back_to_previous_cb, ad);
	evas_object_show(popup);
}

void setting_tapi_get_plmn_mode_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x", result);
	ret_if(!user_data);
	SettingNetworkUG *ad = user_data;

	if (result == TAPI_NETWORK_NO_ERR) {
		TelNetworkSelectionMode_t sel;
		memcpy(&sel, data, sizeof(TelNetworkSelectionMode_t));
		SETTING_TRACE_DEBUG("ad->sel_net[%d] %s sel[%d]", ad->sel_net, ad->sel_net == sel ? "==" : "!=", sel);
		if (ad->sel_net != sel) {
			ad->sel_net = sel;
			vconf_set_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, ad->sel_net);
			setting_network_update_sel_network(ad);
		}
	} else {
	}
}

/**
 *
 */
void ___sort_merge_handle(void *usrdata, TelNetworkPlmnList_t *list)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = usrdata;
	int cnt = 0, tempcount=0;
	ad->plmn_info.networks_count = list->networks_count;
	SETTING_TRACE("list->networks_count:%d", list->networks_count);
	int idx = 0;

	tempcount = list->networks_count;
	for (idx = 0; idx < tempcount; idx++) {
		SETTING_TRACE("[%d].network_name=%s, spn=%s, plmn=%s, plmn_id=%d, plmn_type=%d, access_technology=%d", idx, list->network_list[idx].network_name,
					  list->network_list[idx].service_provider_name,
					  list->network_list[idx].plmn, list->network_list[idx].plmn_id,
					  list->network_list[idx].type_of_plmn,
					  list->network_list[idx].access_technology);
	}
	SETTING_TRACE("*****************************************************");

	int j = 0;
	bool alread_exist = FALSE;
	tempcount = list->networks_count;
	for (idx = 0; idx < tempcount; idx++) {
		alread_exist = FALSE;
		for (j = 0; j < cnt; j++) {
			if (0 == safeStrCmp(ad->plmn_info.network_list[j].network_name, list->network_list[idx].network_name)
				|| 0 == safeStrCmp(ad->plmn_info.network_list[j].plmn, list->network_list[idx].plmn)) {
				alread_exist = TRUE;
				break;
			}
		}
		if (alread_exist) {
			continue;
		}
		memcpy(&(ad->plmn_info.network_list[cnt++]), &(list->network_list[idx]), sizeof(TelNetworkIdentity_t));
	}

	SETTING_TRACE("cnt:%d, list->networks_count=%d", cnt, list->networks_count);
	SETTING_TRACE("*****************************************************");
	SETTING_TRACE("After Sorted:");
	ad->plmn_info.networks_count = list->networks_count = (char)cnt;
	for (idx = 0; idx < cnt; idx++) {
		SETTING_TRACE("[%d]:network_name=%s,plmn=%s,plmn_id=%d,access_technology=%d", idx,
					  ad->plmn_info.network_list[idx].network_name, ad->plmn_info.network_list[idx].plmn, ad->plmn_info.network_list[idx].plmn_id, ad->plmn_info.network_list[idx].access_technology);
	}
}

/*special process for char '&', such as AT&T */
void __process_special_char(char *dst, const char *src)
{
	char *temp1 = (char *)src;
	char *temp2 = strchr(temp1, '&');
	char *temp = NULL;

	while (NULL != temp2) {
		if (0 == safeStrNCmp(temp2, "&amp;", strlen("&amp;"))) {
			temp = temp2 + 1;
			if ('\0' == temp[0]) {
				break;
			}
			temp2 = strchr(temp, '&');
		} else {
			g_strlcat(dst, temp1, temp2 - temp1 + 1);
			g_strlcat(dst, "&amp;", MAX_SPECIALIZITION_LEN);
			temp1 = temp2 + 1;
			if ('\0' == temp1[0]) {
				break;
			}
			temp2 = strchr(temp1, '&');
		}
	}
	if ('\0' != temp1[0]) {
		g_strlcat(dst, temp1, MAX_SPECIALIZITION_LEN);
	}
}

/**
 * async api call - searching network list
 *
 * @see also tel_search_network
 */
void setting_tapi_search_network_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	ret_if(!data || !user_data);
	SettingNetworkUG *ad = user_data;
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = %d", result);
	setting_network_popup_delete(ad);

	if (!ad->b_searching_network) {
		/*The requeset has already been canceled, don't do detail process*/
		return;
	}

	if (ad->data_search_network_item) {
		/* [ UI UPDATE ]*/
		elm_object_item_del(ad->data_search_network_item->item);
		ad->data_search_network_item = NULL;
	}

	ad->b_searching_network = FALSE;
	int valid_cnt = 0;
	if (result == TAPI_NETWORK_NO_ERR) {
		int cnt = 0;

		___sort_merge_handle(ad, data);

		Setting_GenGroupItem_Data *item_data = NULL;

		const char *pa_net_name = get_pa_select_network();
		char *cur_plmn = NULL;
		tel_get_property_string(ad->handle, TAPI_PROP_NETWORK_PLMN, &cur_plmn);
		SETTING_TRACE("VCONFKEY_TELEPHONY_NWNAME:pa_net_name:%s", pa_net_name);
		SETTING_TRACE("cur_plmn:%s", cur_plmn);

		char name[MAX_COMMON_BUFFER_LEN] = {0, };
		while (cnt < ad->plmn_info.networks_count) {
			SETTING_TRACE("network name from TAPI[%d]:%s --%s --(plmn=%s,plmn_id=%d,at=%s)", cnt,
						  ad->plmn_info.network_list[cnt].network_name,
						  ad->plmn_info.network_list[cnt].service_provider_name,
						  ad->plmn_info.network_list[cnt].plmn,
						  ad->plmn_info.network_list[cnt].plmn_id,
						  setting_network_get_act_str(ad->plmn_info.network_list[cnt].access_technology));

			if (isSpaceStr(ad->plmn_info.network_list[cnt].network_name)) {
				/*skip empty string. */
				cnt++;
				continue;
			}

			memset(name, 0, MAX_COMMON_BUFFER_LEN);

			snprintf(name, MAX_COMMON_BUFFER_LEN, "%s",
					 ad->plmn_info.network_list[cnt].network_name);
			/*SETTING_TRACE("name:%s", name); */
			char speciliztion[MAX_COMMON_BUFFER_LEN] = {0, };
			__process_special_char(speciliztion, name);

#if 1
			/* check the code */
			SETTING_TRACE("name:%s", speciliztion);
			item_data = setting_create_Gendial_field_1radio(
							ad->genlist_sel_network,
							&itc_multiline_1text_1icon,
							setting_network_Gendial_select_plmn_cb,
							ad,
							SWALLOW_TYPE_1RADIO_RIGHT_PROPAGATE_EVENTS_SET,
							ad->chk_sel,
							TAPI_NETWORK_SELECTIONMODE_MANUAL + valid_cnt + 1,
							speciliztion,
							setting_network_select_network_chk_changed);
			if (NULL == item_data) {
				SETTING_TRACE_ERROR("Failed to calloc memory");
				cnt++;
				continue;
			}
			item_data->userdata = ad;
#endif
#if 1
			if (TAPI_NETWORK_SELECTIONMODE_AUTOMATIC != ad->sel_net
				&& 0 == safeStrCmp(pa_net_name, ad->plmn_info.network_list[cnt].network_name)
				&& ad->plmn_info.network_list[cnt].access_technology == ad->sel_act) {
				/* [ UI UPDATE ]*/
				elm_radio_value_set(ad->chk_sel, item_data->chk_status);/*let select nothing */
			}
#endif
			cnt++;
			valid_cnt++;
		}
		FREE(pa_net_name);

		SETTING_TRACE("Found %d valid networks in total[%d]", valid_cnt, cnt);
		/*valid_cnt = 0; for test.. */
		if (valid_cnt > 0) {
			/* [ UI UPDATE ]*/
			setting_create_popup(ad, ad->win_get, NULL, keyStr_Searched, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		} else { /*there is no invalid plmn nearby */
			/* [ UI UPDATE ]*/
			setting_create_popup(ad, ad->win_get, NULL, keyStr_No_Other_Network, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		}
	} else {
		/* [ UI UPDATE ]*/
		setting_create_popup(ad, ad->win_get, NULL, keyStr_Failed_Searched, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
	}

#if 1
	if (0 == valid_cnt && TAPI_NETWORK_SELECTIONMODE_AUTOMATIC == ad->sel_net && ad->data_auto_network_item) {

		/* [ UI UPDATE ]*/
		ad->data_auto_network_item->chk_status = TRUE;
		char *sub_text = setting_customize_text(ad->data_auto_network_item->sub_desc, 0, BLUE_COLOR, NULL);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(sub_text);
		FREE(sub_text);
		elm_object_item_data_set(ad->data_auto_network_item->item, ad->data_auto_network_item);
		elm_genlist_item_update(ad->data_auto_network_item->item);
	}
#endif
}


void setting_tapi_get_serving_network_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x", result);
	ret_if(!user_data || !data);
	SettingNetworkUG *ad = user_data;
	TelNetworkServing_t *srv_nt = data;

	if (result == TAPI_NETWORK_NO_ERR) {
		ad->sel_act = srv_nt->act;
		vconf_set_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT, ad->sel_act);

		FREE(ad->sel_network);
		/*ad->sel_network = vconf_get_str(VCONFKEY_TELEPHONY_NWNAME); */
		ad->sel_network = get_pa_select_network();
		if (!ad->sel_network) {
			ad->sel_network = strndup(Keystr_UNKNOWN_NETWORK, strlen(Keystr_UNKNOWN_NETWORK) + 1);
		}
		setting_network_update_sel_network(ad);
	} else {
		/*if failed, do nothing.. */
	}
	SETTING_TRACE_END;

}


void setting_tapi_cancel_manual_search_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(" - result = 0x%x", result);
	ret_if(!user_data);
	/*SettingNetworkUG *ad = user_data; */
}

void __register_network(Setting_GenGroupItem_Data *list_item)
{
	SETTING_TRACE_BEGIN;
	ret_if(list_item == NULL);
	SETTING_TRACE("process item [%s]", list_item->keyStr);
	SettingNetworkUG *ad = list_item->userdata;
	ret_if(ad == NULL);

	int tapi_ret = TAPI_API_INVALID_INPUT;

	/*cancel the searching operation */

	if (ad->data_search_network_item) {
		elm_object_item_del(ad->data_search_network_item->item);
		ad->data_search_network_item = NULL;
	}

	if (ad->b_searching_network) {
		SETTING_TRACE("Sending tel_cancel_network_manual_search..");
		tapi_ret = tel_cancel_network_manual_search(ad->handle, setting_tapi_cancel_manual_search_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_ERROR("*** [ERR] tel_cancel_network_manual_search. "\
								"tapi_ret=%d ***", tapi_ret);
		} else {
			ad->b_searching_network = FALSE;
		}
	}

	if (0 == safeStrCmp(list_item->keyStr, "IDS_ST_BODY_SELECT_AUTOMATICALLY")) {
		ad->sel_net = TAPI_NETWORK_SELECTIONMODE_AUTOMATIC;
		SETTING_TRACE("Sending tel_select_network_automatic..");
		tapi_ret = tel_select_network_automatic(ad->handle, setting_tapi_set_plmn_mode_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG("*** [ERR] tel_select_network_automatic."\
								" tapi_ret=%d ***", tapi_ret);
		} else {

			/* Display 'Registering' popup when automatic */
			SETTING_TRACE("Automatic selection, Registering..")

			if (ad->network_select_registering_pop != NULL) {
				evas_object_del(ad->network_select_registering_pop);
				ad->network_select_registering_pop = NULL;
			}

			ad->network_select_registering_pop = __create_registering_popup(list_item);
			if (ad->network_select_registering_pop) {
				SETTING_TRACE("Registering popup appeared");
			} else {
				SETTING_TRACE("Registering popup failed");
			}

			if (ad->data_auto_network_item) {
				/*ad->data_auto_network_item->sub_desc = (char *)g_strdup("...."); */
				/*elm_genlist_item_item_class_update(ad->data_auto_network_item->item, &(itc_2text_1icon_6)); */
				char *sub_text = setting_customize_text("....", 0, BLUE_COLOR, NULL);
				ad->data_auto_network_item->sub_desc = (char *)g_strdup(sub_text);
				FREE(sub_text);
				elm_object_item_data_set(ad->data_auto_network_item->item, ad->data_auto_network_item);
				elm_genlist_item_update(ad->data_auto_network_item->item);
			}

			ad->b_set_manul_network = FALSE;
			ad->b_set_auto_network = TRUE;
			ad->sel_act = -1;/*means "Automatic" */
		}
	} else {
		ad->sel_net = TAPI_NETWORK_SELECTIONMODE_MANUAL;
		ad->b_set_manul_network = TRUE;
		ad->b_set_auto_network = FALSE;

		int cnt = 0;
		SETTING_TRACE("ad->plmn_info.networks_count:%d",
					  ad->plmn_info.networks_count);
		char name[MAX_COMMON_BUFFER_LEN] = {0, };
		for (; cnt < ad->plmn_info.networks_count; cnt++) {
			SETTING_TRACE("ad->plmn_info.network_list[cnt].network_name:%s",
						  ad->plmn_info.network_list[cnt].network_name);
			memset(name, 0, MAX_COMMON_BUFFER_LEN);
			snprintf(name, MAX_COMMON_BUFFER_LEN, "%s",
					 ad->plmn_info.network_list[cnt].network_name);
			/*SETTING_TRACE("name:%s", name); */

			char speciliztion[MAX_COMMON_BUFFER_LEN] = {0, };
			__process_special_char(speciliztion, name);

			SETTING_TRACE("	 name:%s", speciliztion);

			if (!safeStrCmp(list_item->keyStr, speciliztion)) {
				SETTING_TRACE("find the right item");
				break;
			}
		}

		SETTING_TRACE_DEBUG("*** [Selected PLMN] name=%s, "\
							"srv provider=%s, PLMN ID=%s, type=%d, "\
							"access tech=%d ***",
							ad->plmn_info.network_list[cnt].network_name,
							ad->plmn_info.network_list[cnt].service_provider_name,
							ad->plmn_info.network_list[cnt].plmn,
							(int)(ad->plmn_info.network_list[cnt].type_of_plmn),
							(int)(ad->plmn_info.network_list[cnt].access_technology));

		/*ASYNC API - TAPI_EVENT_NETWORK_SELECT_CNF */
		SETTING_TRACE("Sending tel_select_network_manual..");
		tapi_ret = tel_select_network_manual(ad->handle,
											 ad->plmn_info.network_list[cnt].plmn,
											 ad->plmn_info.network_list[cnt].access_technology,
											 setting_tapi_set_plmn_mode_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG("%s*** [ERR] "\
								"tel_select_network_manual. "\
								"tapi_ret=%d ***%s", SETTING_FONT_RED,
								tapi_ret, SETTING_FONT_BLACK);
			setting_create_popup(ad, ad->win_get, NULL, "IDS_ST_BODY_FAILED_TO_SELECT_NETWORK", NULL, POPUP_INTERVAL, FALSE, FALSE, 0);

			return;
		} else {
			/* Display 'Registering' popup when manual */
			SETTING_TRACE("Manual selection, Registering..")

			if (ad->network_select_registering_pop != NULL) {
				evas_object_del(ad->network_select_registering_pop);
				ad->network_select_registering_pop = NULL;
			}

			ad->network_select_registering_pop = __create_registering_popup(list_item);
			if (ad->network_select_registering_pop) {
				SETTING_TRACE("Registering popup appeared");
			} else {
				SETTING_TRACE("Registering popup failed");
			}

			ad->sel_act = ad->plmn_info.network_list[cnt].access_technology;
			FREE(ad->sel_network);
			ad->sel_network = strdup(ad->plmn_info.network_list[cnt].network_name);
		}
	}
}

static void __ignore_back_key_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	return;
}

static Evas_Object *__create_registering_popup(void *data)
{
	SETTING_TRACE_BEGIN;

	retv_if(!data, NULL);

	Setting_GenGroupItem_Data	*list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG			*ad = list_item->userdata;

	Evas_Object *popup = elm_popup_add(ad->win_get);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, __ignore_back_key_cb, list_item);

	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object *box = elm_box_add(popup);

	elm_box_horizontal_set(box, EINA_TRUE);
	elm_box_align_set(box, 0.5, 0.5);
	elm_box_padding_set(box, 16, 0);

	Evas_Object *progressbar = elm_progressbar_add(popup);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	elm_box_pack_end(box, progressbar);

	Evas_Object *label = elm_label_add(popup);
	elm_object_text_set(label, _("IDS_ST_POP_REGISTERING_ING"));
	evas_object_show(label);
	elm_box_pack_end(box, label);

	/*evas_object_show(box); */
	/*elm_object_content_set(popup, box); */

	Evas_Object *table = elm_table_add(popup);
	evas_object_show(table);
	elm_table_homogeneous_set(table, EINA_FALSE);

	Evas_Object *rect_up;
	rect_up = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_up, ELM_SCALE_SIZE(170), ELM_SCALE_SIZE(34));

	Evas_Object *rect_down;
	rect_down = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_down, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(34));

	elm_table_pack(table, rect_up, 0, 0, 2, 1);/* rect as a padding */
	elm_table_pack(table, box, 1, 1, 1, 1);
	elm_table_pack(table, rect_down, 2, 2, 2, 1);/* rect as a padding */

	evas_object_show(box);

	elm_object_content_set(popup, table);

	evas_object_show(popup);

	return popup;
}

/*
 * Cancel for searching network
 */
static void __search_network_cancel_cb(void *data, Evas_Object *obj,
									   void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG *ad = list_item->userdata;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (ad->b_searching_network) {
		SETTING_TRACE("Sending tel_cancel_network_manual_search..");
		int tapi_ret = tel_cancel_network_manual_search(ad->handle, setting_tapi_cancel_manual_search_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_ERROR("*** [ERR] tel_cancel_network_manual_search. "\
								"tapi_ret=%d ***", tapi_ret);
		} else {
			ad->b_searching_network = FALSE;
		}
	}

	/* Restore selection automatically */
	int sel_net;

	vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &sel_net);
	SETTING_TRACE("Previous selection after searching network:%d", sel_net);

	if (sel_net == TAPI_NETWORK_SELECTIONMODE_AUTOMATIC) {
		list_item->chk_status = 1;
		elm_genlist_item_update(list_item->item);
	}
}

Evas_Object *__create_searching_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG *ad = list_item->userdata;

	Evas_Object *popup = elm_popup_add(ad->ly_main);
	elm_object_domain_translatable_part_text_set(popup, "title,text", SETTING_PACKAGE, _("IDS_ST_POP_SEARCHING_NETWORK_ING"));

	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, __search_network_cancel_cb, list_item);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
									 EVAS_HINT_EXPAND);

	Evas_Object *box = elm_box_add(popup);

	elm_box_horizontal_set(box, EINA_TRUE);
	elm_box_align_set(box, 0.5, 0.5);
	elm_box_padding_set(box, 16, 0);

	Evas_Object *progressbar = elm_progressbar_add(popup);
	elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	evas_object_show(progressbar);
	elm_box_pack_end(box, progressbar);

	Evas_Object *label = elm_label_add(popup);
	elm_object_text_set(label, _("IDS_ST_POP_NETWORKSERVICES_SEARCHING"));
	evas_object_show(label);
	elm_box_pack_end(box, label);

	/*evas_object_show(box); */
	/*elm_object_content_set(popup, box); */

	Evas_Object *table = elm_table_add(popup);
	evas_object_show(table);
	elm_table_homogeneous_set(table, EINA_FALSE);

	Evas_Object *rect_up;
	rect_up = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_up, ELM_SCALE_SIZE(170), ELM_SCALE_SIZE(34));

	Evas_Object *rect_down;
	rect_down = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_down, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(34));

	elm_table_pack(table, rect_up, 0, 0, 2, 1);/* rect as a padding */
	elm_table_pack(table, box, 1, 1, 1, 1);
	elm_table_pack(table, rect_down, 2, 2, 2, 1);/* rect as a padding */

	evas_object_show(box);

	elm_object_content_set(popup, table);

	/* 'cancel' button */
	Evas_Object *btn = elm_button_add(popup);
	/*msgid "IDS_ST_BUTTON_CANCEL_ABB" */
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, _("IDS_ST_BUTTON_CANCEL_ABB"));
	elm_object_part_content_set(popup, "button1", btn);
	evas_object_smart_callback_add(btn, "clicked", __search_network_cancel_cb, list_item);

	evas_object_show(popup);

	return popup;
}

static Eina_Bool __search_net_on_timer(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, EINA_FALSE);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG *ad = list_item->userdata;
	retvm_if(ad == NULL, ECORE_CALLBACK_CANCEL, "ad is NULL");

	/*
	if (ad->popup)
	{
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	*/

	int tapi_ret;
	/*searching list */
	tapi_ret = tel_search_network(ad->handle, setting_tapi_search_network_cb, ad); /*ASYNC API - TAPI_EVENT_NETWORK_SEARCH_CNF */
	if (tapi_ret != TAPI_API_SUCCESS) { /* error handling.. */
		SETTING_TRACE_ERROR("%s*** [ERR] tel_search_network. tapi_ret=%d ***%s", SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup(ad, ad->win_get, NULL, STR_SETTING_OPERATION_FAILED, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);

		/* put error handler on the end of this function. */
		/*rollback.. */
		setting_update_gl_item_chk_status(ad->data_auto_network_item, 1);
		return ECORE_CALLBACK_CANCEL;
	}

	ad->b_searching_network = TRUE;
	/*
	ad->data_search_network_item = setting_create_Gendial_field_1radio(
							ad->genlist_sel_network,
							&itc_1icon,
							NULL, NULL,
							SWALLOW_Type_1RADIO_1SEARCH,
							ad->chk_sel, -1, NULL, NULL);
	elm_genlist_item_select_mode_set(ad->data_search_network_item->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	*/
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	ad->popup = __create_searching_popup(list_item);
	if (ad->data_auto_network_item) {
		/*elm_genlist_item_item_class_update(ad->data_auto_network_item->item, &(itc_2text_1icon_7)); */

		char *sub_text = setting_customize_text(ad->data_auto_network_item->sub_desc, 0, GRAY_COLOR, NULL);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(sub_text);
		FREE(sub_text);
		elm_object_item_data_set(ad->data_auto_network_item->item, ad->data_auto_network_item);
		elm_genlist_item_update(ad->data_auto_network_item->item);
	}

	/*ecore_timer_del(ad->timer); */
	ad->timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void __switch_automatic_on_resp_cb(void *data, Evas_Object *obj,
								   void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	SettingNetworkUG *ad = list_item->userdata;
	int response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK == response_type) {

		/* Not used, Searching network directly.
		//changing of USE_PACKET_DATA was trigged by betwork searching
		ad->b_trigged_by_searching = TRUE;

		//for delay..
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win_get,
					 PROGRESSBAR_STYLE,
					 NULL, NULL, NULL, 0, TRUE, FALSE, 0);

		if (ad->timer) {
			ecore_timer_del(ad->timer);
			ad->timer = NULL;
		}
		ad->timer = ecore_timer_add(TIME_TO_TURN_OFF_3G, __search_net_on_timer, list_item);
		*/

		__search_net_on_timer(list_item);

	} else if (POPUP_RESPONSE_CANCEL == response_type) {

		/* Restore selection automatically */
		int sel_net, chk_status;

		vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &sel_net);
		SETTING_TRACE("Previous selection after searching network:%d", sel_net);

		if (sel_net == TAPI_NETWORK_SELECTIONMODE_AUTOMATIC)
			chk_status = 1;
		else
			chk_status = 0;

		setting_update_gl_item_chk_status(ad->data_auto_network_item, chk_status);
	}
}

void __switch_automatic_network(Setting_GenGroupItem_Data *list_item)
{
	SETTING_TRACE_BEGIN;
	ret_if(!list_item || !list_item->userdata || !list_item->keyStr);
	ret_if(0 != safeStrCmp(list_item->keyStr, "IDS_ST_BODY_SELECT_AUTOMATICALLY"));
	SettingNetworkUG *ad = list_item->userdata;
	SETTING_TRACE("The value of toogle changes to be %d", list_item->chk_status);
	if (list_item->chk_status) {
		/* Automatic selected */
		Elm_Object_Item *last_item = elm_genlist_last_item_get(ad->genlist_sel_network);
		SETTING_TRACE("ad->data_auto_network_item->item:%p", ad->data_auto_network_item->item);
		for (;;) {
			last_item = elm_genlist_last_item_get(ad->genlist_sel_network);
			SETTING_TRACE("last_item:%p", last_item);
			if (last_item == ad->data_auto_network_item->item) {
				break;
			}
			elm_object_item_del(last_item);
		}

		__register_network(list_item);
	} else {
		setting_network_searching_network(list_item);
	}
}

/*
 * Network selection / Searching network
 */
void
setting_network_searching_network(Setting_GenGroupItem_Data *list_item)
{
	SETTING_TRACE_BEGIN;

	SettingNetworkUG *ad = list_item->userdata;

	/* Manual selected */
	/*__register_network(list_item); */

	int value_use_packet;
	vconf_get_int(VCONFKEY_DNET_STATE, &value_use_packet);

	SETTING_TRACE("ad->sel_net:%d", ad->sel_net);
	if (value_use_packet != VCONFKEY_DNET_OFF) {
		ad->popup =
			setting_create_popup(list_item, ad->ly_main,
								 IDS_ST_BODY_UNABLE_TO_SCAN_FOR_NETWORKS, _(SETTING_NETWORK_SEARCH_3G_ON_DESC),
								 __switch_automatic_on_resp_cb,
								 0, FALSE, FALSE,
								 2, _("IDS_ST_BUTTON_OK"), _("IDS_ST_BUTTON_CANCEL_ABB"));
		return;
	}

	int tapi_ret;
	/*searching list */
	tapi_ret = tel_search_network(ad->handle, setting_tapi_search_network_cb, ad); /*ASYNC API - TAPI_EVENT_NETWORK_SEARCH_CNF */
	if (tapi_ret != TAPI_API_SUCCESS) { /* error handling.. */
		SETTING_TRACE_ERROR("%s*** [ERR] tel_search_network. tapi_ret=%d ***%s", SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup(ad, ad->win_get, NULL, STR_SETTING_OPERATION_FAILED, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);

		/* put error handler on the end of this function. */
		/*rollback.. */
		setting_update_gl_item_chk_status(list_item, 1);
		return;
	}

	ad->b_searching_network = TRUE;
	/*latest UI: show progress popup instead of progress item
	ad->data_search_network_item = setting_create_Gendial_field_1radio(
							ad->genlist_sel_network,
							&itc_1icon,
							NULL, NULL,
							SWALLOW_Type_1RADIO_1SEARCH,
							ad->chk_sel, -1, NULL, NULL);
	elm_genlist_item_select_mode_set(ad->data_search_network_item->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	*/
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	ad->popup = __create_searching_popup(list_item);
	if (ad->data_auto_network_item) {
		/*elm_genlist_item_item_class_update(ad->data_auto_network_item->item, &(itc_2text_1icon_7)); */

		char *sub_text = setting_customize_text(ad->data_auto_network_item->sub_desc, 0, GRAY_COLOR, NULL);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(sub_text);
		FREE(sub_text);
		elm_object_item_data_set(ad->data_auto_network_item->item, ad->data_auto_network_item);
		elm_genlist_item_update(ad->data_auto_network_item->item);
	}

	return;
}

void setting_network_select_network_chk_changed(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL || obj == NULL, "Parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;
	if (0 == safeStrCmp(list_item->keyStr, "IDS_ST_BODY_SELECT_AUTOMATICALLY")) {
		list_item->chk_status = elm_check_state_get(obj);
		__switch_automatic_network(list_item);
	} else { /*"IDS_ST_BODY_SELECT_AUTOMATICALLY" */
		list_item->chk_status = elm_radio_value_get(obj);
		SETTING_TRACE("[Radio index:%d] Radio checked", list_item->chk_status);

		__register_network(list_item);
	}

}


void setting_network_Gendial_select_plmn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL || event_info == NULL, "Parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SETTING_TRACE("list_item->keyStr:%s", list_item->keyStr);
	if (0 == safeStrCmp(list_item->keyStr, "IDS_ST_BODY_SELECT_AUTOMATICALLY")) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
		__switch_automatic_network(list_item);
		return;
	} else {
		if (ad->b_searching_network) {
			SETTING_TRACE("Searching poup displaying, Cannot select");
			return;
		}

		int chk_status = elm_radio_value_get(ad->chk_sel);
		if (chk_status != list_item->chk_status) {
			SETTING_TRACE("[List index:%d] List selected (radio index:%d)", list_item->chk_status, chk_status);
			elm_radio_value_set(ad->chk_sel, list_item->chk_status);
			__register_network(list_item);
		} else {
			SETTING_TRACE("[List index:%d] Skip becuase radio checked (radio index:%d)", list_item->chk_status, chk_status);
		}
	}
}

/**
 * [UI] update
 */
void setting_network_update_sel_network(void *data)
{
	/*SETTING_TRACE_BEGIN; */
	SettingNetworkUG *ad = data;
	/*SETTING_TRACE("ad->sel_net:%d", ad->sel_net); */
	/*SETTING_TRACE("ad->sel_network:%s", ad->sel_network); */
	/*SETTING_TRACE("sel_act = [%s(%d)]", setting_network_get_act_str(ad->sel_act),ad->sel_act); */

	if (ad->data_auto_network_item) {
		if (TAPI_NETWORK_SELECTIONMODE_AUTOMATIC == ad->sel_net) {
			/*SETTING_TRACE_DEBUG("After set as Automatic, update " */
			/*		   "ad->data_auto_network_item to be [%s]", */
			/*		   ad->sel_network); */
			/*ad->data_auto_network_item->sub_desc = (char *)g_strdup(_(ad->sel_network)); */
			char *sub_text = setting_customize_text(_(ad->sel_network), 0, BLUE_COLOR, NULL);
			SETTING_TRACE("sub_text:%s", sub_text);
			G_FREE(ad->data_auto_network_item->sub_desc);
			ad->data_auto_network_item->sub_desc = (char *)g_strdup(sub_text);
			elm_object_item_data_set(ad->data_auto_network_item->item, ad->data_auto_network_item);
			elm_genlist_item_update(ad->data_auto_network_item->item);
			elm_genlist_item_fields_update(ad->data_auto_network_item->item, "*", ELM_GENLIST_ITEM_FIELD_TEXT);
			FREE(sub_text);
		} else {
		}
	}

	if (ad->data_sel_net) { /*it indicates TAPI is not responsed in time */
		char sel_network_desc[MAX_COMMON_BUFFER_LEN] = { 0, };
		if (TAPI_NETWORK_SELECTIONMODE_AUTOMATIC == ad->sel_net) {
			snprintf(sel_network_desc, MAX_COMMON_BUFFER_LEN,
					 "%s, %s", _("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"), _(ad->sel_network));

		} else {
			snprintf(sel_network_desc, MAX_COMMON_BUFFER_LEN,
					 "%s, %s", _("IDS_ST_BODY_MANUAL"), _(ad->sel_network));
		}

		/*SETTING_TRACE("sel_network_desc:%s", sel_network_desc); */

		G_FREE(ad->data_sel_net->sub_desc);
		ad->data_sel_net->sub_desc = (char *)g_strdup(_(sel_network_desc));

		elm_object_item_data_set(ad->data_sel_net->item, ad->data_sel_net);
		elm_genlist_item_update(ad->data_sel_net->item);
	}
}

const char *setting_network_get_act_str(TelNetworkSystemType_t type)
{
	switch (type) {
	case TAPI_NETWORK_SYSTEM_NO_SRV: /*0 */
		return "NO SERVICE";
	case TAPI_NETWORK_SYSTEM_GSM: /*1 */
		return "GSM";
	case TAPI_NETWORK_SYSTEM_GPRS: /*2 */
		return "GPRS";
	case TAPI_NETWORK_SYSTEM_EGPRS: /*3 */
		return "EGPRS";
	case TAPI_NETWORK_SYSTEM_PCS1900: /*4 */
		return "PCS1900";
	case TAPI_NETWORK_SYSTEM_UMTS: /*5 */
		return "WCDMA";
	case TAPI_NETWORK_SYSTEM_GSM_AND_UMTS:
		return "GSM/WCDMA";
	case TAPI_NETWORK_SYSTEM_HSDPA:
		return "HSDPA";
	case TAPI_NETWORK_SYSTEM_IS95A:
		return "IS95A";
	case TAPI_NETWORK_SYSTEM_IS95B:
		return "IS95B";
	case TAPI_NETWORK_SYSTEM_CDMA_1X:
		return "CDMA 1X";
	case TAPI_NETWORK_SYSTEM_EVDO_REV_0:
		return "EV-DO rev0";
	case TAPI_NETWORK_SYSTEM_1X_EVDO_REV_0_HYBRID:
		return "1X and EV-DO rev0";
	case TAPI_NETWORK_SYSTEM_EVDO_REV_A:
		return "EV-DO revA";
	case TAPI_NETWORK_SYSTEM_1X_EVDO_REV_A_HYBRID:
		return "1X and EV-DO revA";
	case TAPI_NETWORK_SYSTEM_EVDV:
		return "EV-DV";
	case TAPI_NETWORK_SYSTEM_LTE:
		return "LTE";
	default:
		return "NO SERVICE";
	}
}

bool setting_network_equal_profile(connection_profile_h pf1, connection_profile_h pf2)
{
	if (!pf1 && !pf2) {
		SETTING_TRACE("Both profile are NULL, return TRUE");
		return TRUE;
	} else if (!pf1 || !pf2) {
		SETTING_TRACE("One profile is NULL, one is not, return FALSE");
		return FALSE;
	}

	connection_profile_type_e type1, type2;
	connection_cellular_service_type_e srvtype1, srvtype2;
	char *apn1, *apn2, *name1, *name2;
	type1 = type2 = srvtype1 = srvtype2 = 0; /*intialize */
	apn1 = apn2 = name1 = name2 = NULL;

	(void)connection_profile_get_type(pf1, &type1);
	(void)connection_profile_get_type(pf2, &type2);
	(void)connection_profile_get_cellular_service_type(pf1, &srvtype1);
	(void)connection_profile_get_cellular_service_type(pf2, &srvtype2);

	(void)connection_profile_get_cellular_apn(pf1, &apn1);
	(void)connection_profile_get_cellular_apn(pf2, &apn2);
	(void)connection_profile_get_name(pf1, &name1);
	(void)connection_profile_get_name(pf2, &name2);

	if (type1 == type2
		&& srvtype1 == srvtype2
		&& 0 == safeStrCmp(apn1, apn2)
		&& 0 == safeStrCmp(name1, name1)) {
		SETTING_TRACE("They have same key fields, return TRUE.");
		FREE(apn1);
		FREE(apn2);
		return TRUE;
	} else {
		SETTING_TRACE("They don't have same key fields, return FALSE.");
		FREE(apn1);
		FREE(apn2);
		return FALSE;
	}
}

#define MAX_ID_LEN 512
static int __profile_item_sort_cb(const void *d1, const void *d2)
{
	SETTING_TRACE_BEGIN;
	connection_profile_h pf1 = (connection_profile_h)d1;
	connection_profile_h pf2 = (connection_profile_h)d2;

	if (!pf1) {
		return (-1);
	}
	if (!pf2) {
		return (1);
	}
	char *id_str1 = NULL;
	char *id_str2 = NULL;
	connection_profile_get_id(pf1, &id_str1);
	connection_profile_get_id(pf2, &id_str2);

	char *name1 = NULL;
	char *name2 = NULL;
	connection_profile_get_name(pf1, &name1);
	connection_profile_get_name(pf2, &name2);

	if (isEmptyStr(id_str1)) {
		FREE(id_str1);
		FREE(id_str2);
		return (-1);
	}

	if (isEmptyStr(id_str2)) {
		FREE(id_str1);
		FREE(id_str2);
		return (1);
	}

	char str1[MAX_ID_LEN] = {0,};
	char str2[MAX_ID_LEN] = {0,};
	safeCopyStr(str1, id_str1, MAX_ID_LEN);
	safeCopyStr(str2, id_str2, MAX_ID_LEN);
	G_FREE(id_str1);
	G_FREE(id_str2);
	/*SETTING_TRACE("str1:%s, name1:%s", str1, name1); */
	/*SETTING_TRACE("str2:%s, name2:%s", str2, name2); */

	char *p = NULL;
	/*for id1.. */
	p = safeStrRStr(str1, "_");
	if (!p) {
		return (-1);
	}
	*p = '\0';
	p = safeStrRStr(str1, "_");
	if (!p) {
		return (-1);
	}
	p++;
	int id1 = atoi(p);
	/*SETTING_TRACE("id1:%d", id1); */

	/*for id2.. */
	p = safeStrRStr(str2, "_");
	if (!p) {
		return (1);
	}
	*p = '\0';
	p = safeStrRStr(str2, "_");
	if (!p) {
		return (1);
	}
	p++;
	int id2 = atoi(p);

	/*SETTING_TRACE("id2:%d", id2); */
	return id1 - id2;
}

void setting_network_reget_profile_list(void *cb)
{
	SETTING_TRACE_BEGIN;
	ret_if(cb == NULL);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (ad->profile_list != NULL) {
		eina_list_free(ad->profile_list);
		ad->profile_list = NULL;
	}
	connection_profile_iterator_h profile_iter = NULL;
	connection_profile_h profile_h = NULL;
	int rv = connection_get_profile_iterator(ad->connection, CONNECTION_ITERATOR_TYPE_REGISTERED, &profile_iter);
	if (rv != CONNECTION_ERROR_NONE) {
		SETTING_TRACE_ERROR("Fail to get profile iterator [%d]", rv);
		/*elm_object_disabled_set(del_btn, EINA_TRUE); */
		return;
	}

	int cnt = 0;
	connection_profile_type_e profile_type = CONNECTION_PROFILE_TYPE_CELLULAR;
	connection_cellular_service_type_e service_type = CONNECTION_CELLULAR_SERVICE_TYPE_UNKNOWN;

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

		cnt++;
		if (CONNECTION_PROFILE_TYPE_CELLULAR != profile_type || ad->profile_service_type != service_type) {
			continue;
		}
		ad->profile_list = eina_list_sorted_insert(ad->profile_list, __profile_item_sort_cb, profile_h);
		/*if (cnt > 1) break;//for test */
	}
	SETTING_TRACE_END;
}

int is_lte_on_feature(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, 0);

	int ret = 0;
	vconf_get_int(VCONF_KEY_PRIVATE_LTE_ON, &ret);
	SETTING_TRACE("LTE feature status: %d", ret);
	return ret;
}

static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_ST_MBODY_MOBILE_DATA", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_MBODY_MOBILE_DATA", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_DATA_ROAMING", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_DATA_ROAMING", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_COM_BODY_NETWORK_OPERATORS", NULL, "viewtype:frontpage;tab:first;keyword:IDS_COM_BODY_NETWORK_OPERATORS", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_NETWORK_MODE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_NETWORK_MODE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_CONNECTIONS", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_CONNECTIONS", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
};

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-network-efl DB search code");

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, IMG_Network, s_cfg_node_array[i].item_type,  s_cfg_node_array[i].data, "Network");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

int setting_network_get_state_data_roaming(int *value)
{
	SETTING_TRACE_BEGIN;
	int err;
	int temp;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &temp, &err);
	*value = temp;
	return err;
}

/**
 * "Data Romaing"
 * VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL
BOOL_SLP_SETTING_DATA_ROAMING
 */
void setting_network_set_state_data_roaming(ButtonState state)
{
	SETTING_TRACE_BEGIN;
	int err;
	if (state == SETTING_ON_OFF_BTN_ON) {
		setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_ON, &err);
		/*		setting_set_event_system(SYS_EVENT_DATA_ROAMING_STATE, EVT_KEY_DATA_ROAMING_STATE, EVT_VAL_DATA_ROAMING_ON); */
	} else {
		setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_OFF, &err);
		/*		setting_set_event_system(SYS_EVENT_DATA_ROAMING_STATE, EVT_KEY_DATA_ROAMING_STATE, EVT_VAL_DATA_ROAMING_OFF); */
	}
}

/**
 * "Mobile Data"
 * setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
 * value : SETTING_ON_OFF_BTN_OFF or SETTING_ON_OFF_BTN_ON
 */
void setting_network_set_state_mobile_data(ButtonState state)
{
	SETTING_TRACE_BEGIN;
	if (state == SETTING_ON_OFF_BTN_ON) {
		system_settings_set_value_bool(SYSTEM_SETTINGS_KEY_3G_DATA_NETWORK_ENABLED, true);
	} else if (state == SETTING_ON_OFF_BTN_OFF) {
		system_settings_set_value_bool(SYSTEM_SETTINGS_KEY_3G_DATA_NETWORK_ENABLED, false);
	} else {
		SETTING_TRACE_ERROR("do not reach here!!!");
	}
}

int setting_network_get_state_mobile_data(int *value)
{
	SETTING_TRACE_BEGIN;
	bool val;
	int ret = system_settings_get_value_bool(SYSTEM_SETTINGS_KEY_3G_DATA_NETWORK_ENABLED, &val);
	if (ret == SYSTEM_SETTINGS_ERROR_NONE) {
		int temp = (int)val;
		*value = temp;
		return	SYSTEM_SETTINGS_ERROR_NONE;
	} else {
		return -1;
	}
}
