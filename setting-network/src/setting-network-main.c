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
#include <setting-network-main.h>
#include <app_preference.h>
#include <efl_extension.h>

#define MOBILE_DATA_WITHOUT_WIFI_STR 			"IDS_ST_BODY_MOBILE_DATA_WILL_BE_USED_IF_YOU_DOWNLOAD_OR_UPLOAD_DATA_WITHOUT_WI_FI_CONNECTION"
#define UMTS_MODE_WORK_OUTSIDE_STR 					"IDS_ST_BODY_UMTS_MODE_ONLY_WORKS_INSIDE_3G_COVERAGE_AREA"
#define BAND_OPERATE_OUTSIDE_USA_CANADA_STR	  	"IDS_ST_BODY_THIS_BAND_MAY_NOT_WORK_OUTSIDE_THE_USA_AND_CANADA"

#define __switch_radio_state(cur_data, dst_state, dst_key) do { \
		if (cur_data) {\
			old_status = cur_data->chk_status;\
			cur_data->chk_status = dst_state;\
			elm_radio_state_value_set(cur_data->eo_check, cur_data->chk_status);\
			if (sel_mode == old_status) {\
				sel_item_data = cur_data;\
			} \
			\
			G_FREE(cur_data->keyStr);\
			cur_data->keyStr = g_strdup(dst_key);\
			elm_genlist_item_fields_update(cur_data->item, "*", ELM_GENLIST_ITEM_FIELD_TEXT);\
		} \
	} while (0);

static int setting_network_main_create(void *cb);
static int setting_network_main_destroy(void *cb);
static int setting_network_main_update(void *cb);
static int setting_network_main_cleanup(void *cb);

setting_view setting_view_network_main = {
	.create = setting_network_main_create,
	.destroy = setting_network_main_destroy,
	.update = setting_network_main_update,
	.cleanup = setting_network_main_cleanup,
};

static void setting_network_main_item_Gendial_mouse_up_cb(void *data,
                                                          Evas_Object *obj,
                                                          void *event_info);
static void setting_network_main_click_softkey_help_cb(void *data,
                                                       Evas_Object *obj,
                                                       void *event_info);
static void setting_network_main_click_softkey_back_cb(void *data,
                                                       Evas_Object *obj,
                                                       void *event_info);
static void setting_network_main_chk_btn_cb(void *data, Evas_Object *obj,
                                            void *event_info);
static void setting_network_use_packet_resp_cb(void *data, Evas_Object *obj,
                                               void *event_info);
static void __setting_network_data_roaming_resp_cb(void *data, Evas_Object *obj,
                                                   void *event_info);
static void __net_restriction_mode_vconf_change_cb(keynode_t *key, void *data);


/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
#ifdef UI_NETWORK_MODE
static Eina_Bool __set_net_mode_on_timer(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, ECORE_CALLBACK_CANCEL, "Data parameter is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) data;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	retvm_if(parentItem == NULL, ECORE_CALLBACK_CANCEL, "parentItem is NULL");

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subitem);	/* parent data */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	retvm_if(!data_subItem || !data_parentItem, ECORE_CALLBACK_CANCEL, "!data_subItem || !data_parentItem");

	SettingNetworkUG *ad = data_parentItem->userdata;
	retvm_if(ad == NULL, ECORE_CALLBACK_CANCEL, "ad is NULL");
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	elm_radio_value_set(ad->data_net_mode->rgd, data_subItem->chk_status);

	ad->net_mode = data_subItem->chk_status;
	int tapi_ret = tel_set_network_mode(ad->handle, data_subItem->chk_status, setting_tapi_set_band_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_mode. tapi_ret=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup(ad, ad->win_get, NULL, _(Invok_API_Failed_Desc), NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		/*rollback */
		int err = 0;
		int ret = setting_get_int_slp_key(ad->data_net_mode->int_slp_setting_binded,
		                                  &(ad->net_mode), &err);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("failed to get vconf");
		}

		if (ad->net_mode == (TAPI_NETWORK_MODE_GSM | TAPI_NETWORK_MODE_WCDMA)) {
			ad->net_mode = TAPI_NETWORK_MODE_AUTO;
		}
		elm_radio_value_set(ad->data_net_mode->rgd, ad->net_mode);
		return ECORE_CALLBACK_CANCEL;
	}

	/*it will be deleted in callback set by tel_set_network_mode(that is setting_tapi_set_band_cb.) */
	ad->network_ug_pop = setting_create_popup(ad, ad->win_get, NULL,
	                                                      "IDS_ST_BUTTON2_PROCESSING_ING", NULL,
	                                                      0.0, TRUE, FALSE, 0);

	/*ecore_timer_del(ad->timer); */
	ad->timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

#if 0
static Eina_Bool __set_net_mode_on_delay(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;

	retvm_if(data == NULL, ECORE_CALLBACK_CANCEL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	//Elm_Object_Item *subitem = (Elm_Object_Item *) data;
	//Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	//retvm_if(parentItem == NULL, ECORE_CALLBACK_CANCEL, "parentItem is NULL");

	//Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subitem);	/* parent data */
	//Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	//retvm_if(!data_subItem || !data_parentItem, ECORE_CALLBACK_CANCEL, "!data_subItem || !data_parentItem");

	retvm_if(ad == NULL, ECORE_CALLBACK_CANCEL, "ad is NULL");

	SETTING_TRACE("tel_set_network_mode(data_subItem->chk_status=%d) run", data_subItem->chk_status);
	int tapi_ret = tel_set_network_mode(ad->handle, data_subItem->chk_status, setting_tapi_set_band_cb, ad);

	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_mode. tapi_ret=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup(ad, ad->win_get, NULL, Invok_API_Failed_Desc, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		/*rollback */
		int err = 0;
		int ret = setting_get_int_slp_key(ad->data_net_mode->int_slp_setting_binded, &(ad->net_mode), &err);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("failed to get vconf");
		}
	} else {
		SETTING_TRACE("tel_set_network_mode()=TAPI_API_SUCCESS");
	}

	ad->timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}
#endif

static void __change_3g_on_resp_cb(void *data, Evas_Object *obj,
                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) data;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	retm_if(parentItem == NULL, "parentItem is NULL");

	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	retm_if(!data_parentItem, "!data_parentItem");

	SettingNetworkUG *ad = data_parentItem->userdata;
	retm_if(ad == NULL, "ad is NULL");

	int response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (POPUP_RESPONSE_OK == response_type) {
		/*changing of USE_PACKET_DATA was trigged by betwork searching */
		ad->b_trigged_by_modechanging = TRUE;
		/*for delay.. */
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win_get,
		                                                  PROGRESSBAR_STYLE,
		                                                  NULL, NULL, NULL, 0, TRUE, FALSE, 0);
		if (ad->timer) {
			ecore_timer_del(ad->timer);
			ad->timer = NULL;
		}
		ad->timer = ecore_timer_add(TIME_TO_TURN_OFF_3G, __set_net_mode_on_timer, subitem);

	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		/*rollback.. */
		int value = 0;
		vconf_get_int(VCONFKEY_SETAPPL_NETWORK_MODE_INT, &value);
		SETTING_TRACE("vaule:%d", value);

		if ((value & TAPI_NETWORK_MODE_GSM) && (value & TAPI_NETWORK_MODE_WCDMA)) {
			SETTING_TRACE("has TAPI_NETWORK_MODE_GSM|TAPI_NETWORK_MODE_WCDMA");
			if (ad->lte_wcdma_gsm) elm_radio_value_set(ad->data_net_mode->rgd, ad->lte_wcdma_gsm->chk_status);
		} else if (value & TAPI_NETWORK_MODE_GSM) {
			SETTING_TRACE("has TAPI_NETWORK_MODE_WCDMA but not has TAPI_NETWORK_MODE_GSM");
			if (ad->gsm_only) elm_radio_value_set(ad->data_net_mode->rgd, ad->gsm_only->chk_status);
		} else {
			SETTING_TRACE("not have TAPI_NETWORK_MODE_WCDMA");
			if (ad->wcdma_only) elm_radio_value_set(ad->data_net_mode->rgd, ad->wcdma_only->chk_status);
		}
		/*elm_radio_value_set(ad->data_net_mode->rgd, ad->net_mode); */
	}
}

/**
 * item in genlist on the popup 'NETWORK MODE'
 */
void __network_sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	ret_if(NULL == data);
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(subitem, 0);

	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subitem);

	SETTING_TRACE("ad->net_mode=%d", ad->net_mode);
	SETTING_TRACE("elm_radio_value_get(ad->data_net_mode->rgd):%d", elm_radio_value_get(ad->data_net_mode->rgd));
	SETTING_TRACE("data_subItem->chk_status=%d", data_subItem->chk_status);

	if (ad->net_mode == data_subItem->chk_status) {
		SETTING_TRACE("No change");
		return;
	}

	int value_use_packet;

	vconf_get_int(VCONFKEY_DNET_STATE, &value_use_packet);

	/*it will be deleted in callback set by tel_set_network_mode(that is setting_tapi_set_band_cb.) */
	ad->network_ug_pop = setting_create_popup_with_progressbar(ad, ad->win_get, PROGRESSBAR_STYLE,
															   NULL, "IDS_ST_BUTTON2_PROCESSING_ING",
															   NULL, 0.0, TRUE, FALSE, 0);

	elm_radio_value_set(ad->data_net_mode->rgd, data_subItem->chk_status);
	ad->net_mode = data_subItem->chk_status;

	/*for delay.. */
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}

	SETTING_TRACE("tel_set_network_mode(data_subItem->chk_status=%d) run", data_subItem->chk_status);
	int tapi_ret = tel_set_network_mode(ad->handle, data_subItem->chk_status, setting_tapi_set_band_cb, ad);

	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_mode. tapi_ret=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup(ad, ad->win_get, NULL, Invok_API_Failed_Desc, NULL, POPUP_INTERVAL, FALSE, FALSE, 0);
		/*rollback */
		int err = 0;
		int ret = setting_get_int_slp_key(ad->data_net_mode->int_slp_setting_binded, &(ad->net_mode), &err);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("failed to get vconf");
		}
	} else {
		SETTING_TRACE("tel_set_network_mode()=TAPI_API_SUCCESS");
	}

	if (ad->network_mode_popup) {
		evas_object_del(ad->network_mode_popup);
		ad->network_mode_popup = NULL;
	}
}

/**
 * radio - chnage handler
 */
EXPORT_PUBLIC
void __network_default_rd_change(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
}

#endif

/**
 * VCONFKEY_3G_ENABLE : notification handler
 */
static void __3g_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);
	SettingNetworkUG *ad = data;
	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("vconf_name:%s, status:%d", vconf_name, status);

	if (!safeStrCmp(vconf_name, VCONFKEY_3G_ENABLE)) {
		if (ad->data_mobile_data) {
			/* change 'use packet data' with the vconf value */
			ad->data_mobile_data->chk_status = status;

			/* set toggle state */

			elm_object_item_data_set(ad->data_mobile_data->item, ad->data_mobile_data);
			elm_genlist_item_update(ad->data_mobile_data->item);
		}
	}
}

/**
 * mobile data ON  --> Data Roaming activate
 * mobile data OFF --> Data Roaming de-activate
 */
static void _setting_network_activate_state_data_roaming(void *data, bool state)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);
	SettingNetworkUG *ad = data;

	if (state == true) {
		if(ad->data_roaming && ad->data_roaming->item) {
			setting_enable_genlist_item(ad->data_roaming->item);
			SETTING_TRACE("AFTER enable data roaming");
		}
	} else {
		if(ad->data_roaming && ad->data_roaming->item) {
			setting_disable_genlist_item(ad->data_roaming->item);
			SETTING_TRACE("AFTER disable data roaming");
		}
	}
}


/**
 * if data roaming is on, set 'use packet data' to ON
 *
 * VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL : __data_roaming_change_cb
 *
 * @see also vconf_notify_key_changed
 */
static void __data_roaming_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);
	SettingNetworkUG *ad = data;
	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("vconf_name:%s, status:%d", vconf_name, status);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL) && ad->data_roaming) {
		#if 0
		setting_update_gl_item_chk_status(ad->data_roaming, status);
		if (status) {
			int value = 0;
			int err = 0;
			int err_mobile_data = setting_network_get_state_mobile_data(&value);
			setting_retm_if(err != 0, "get vconf failed");
			if (!value) {
				setting_network_set_state_mobile_data(SETTING_ON_OFF_BTN_ON);
			}
		}
		#endif
	}
}

/**
 * Vconf callback for network restriction mode (VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE)
 */
static void __net_restriction_mode_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;

	ret_if(NULL == data);

	SettingNetworkUG *ad = data;

	int		status = vconf_keynode_get_bool(key);
	char	*vconf_name = vconf_keynode_get_name(key);

	SETTING_TRACE("vconf_name:%s, status:%d", vconf_name, status);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE)) {
		if (status) {
			if (ad->data_mobile_data)
				setting_disable_genlist_item(ad->data_mobile_data->item);

			if (ad->data_roaming)
				setting_disable_genlist_item(ad->data_roaming->item);
		} else {
			if (ad->data_mobile_data)
				setting_enable_genlist_item(ad->data_mobile_data->item);

			if (ad->data_roaming)
				setting_enable_genlist_item(ad->data_roaming->item);
		}
	}

	return;
}

#ifdef UI_NETWORK_MODE
#if 0 /* DISABLED CODE */
/**
 * get network name by entwork_mode vconf key
 *
 * featuring : docomo
 * @return pointer of static variable (no free)
 */
static char *_get_network_mode_sub_desc()
{
	/*  ***BEGIN***  Fixed the problem of word twinkle  SAMSUNG 2010/7/21 add */
	/* hide NETWORK MODE in Emulator*/
	char *sub_des = NULL;
	int value = TAPI_NETWORK_MODE_AUTO;
	int k;
	k = vconf_get_int(VCONFKEY_SETAPPL_NETWORK_MODE_INT, &value);
	int status_3G = 0;
	vconf_get_bool(VCONFKEY_3G_ENABLE, &status_3G);
	int roam_status = 0;
	vconf_get_bool(VCONFKEY_TELEPHONY_SVC_ROAM, &roam_status);

	if (-1 != k) {
		SETTING_TRACE("VCONFKEY_SETAPPL_NETWORK_MODE_INT value:%d", value);
		switch (value) {
			case TAPI_NETWORK_MODE_AUTO:
			case TAPI_NETWORK_MODE_LTE|TAPI_NETWORK_MODE_GSM|TAPI_NETWORK_MODE_WCDMA: {
					SETTING_TRACE("_(Keystr_NetBrand_Auto):%s", _(Keystr_NetBrand_Auto));
					sub_des = _(Keystr_NetBrand_Auto);
					break;
				}

			case TAPI_NETWORK_MODE_GSM:
				sub_des = _(Keystr_NetBrand_GSM);
				break;
			case TAPI_NETWORK_MODE_WCDMA:
				sub_des = _(Keystr_NetBrand_WCDMA);
				break;
			case TAPI_NETWORK_MODE_LTE|TAPI_NETWORK_MODE_WCDMA:
				sub_des = _(Keystr_NetBrand_LTE_WCDMA);
				break;
			case TAPI_NETWORK_MODE_GSM|TAPI_NETWORK_MODE_WCDMA:
				sub_des = _(Keystr_NetBrand_Auto_WCDMA_GSM);
				break;
			default:
				break;
		}
	} else {
		SETTING_TRACE("VCONFKEY_SETAPPL_NETWORK_MODE_INT value:%d on success ", value);
	}
	return sub_des;
}
#endif
#endif

void _get_network_selected_desc(void *data, char *sel_network_desc, int size)
{
	/*	int value = 0; */
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	ad->sel_net = TAPI_NETWORK_SELECTIONMODE_AUTOMATIC;
	ad->sel_act = TAPI_NETWORK_SYSTEM_NO_SRV;
	vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &ad->sel_net);
	vconf_get_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT, &ad->sel_act);

	FREE(ad->sel_network);
	ad->sel_network = get_pa_select_network();
	SETTING_TRACE("pa_sel_net_name:%s", ad->sel_network);
	if (!ad->sel_network) {
		ad->sel_network = strndup(Keystr_UNKNOWN_NETWORK, strlen(Keystr_UNKNOWN_NETWORK) + 1);
	}
	SETTING_TRACE("pa_net_name:%s", ad->sel_network);
	SETTING_TRACE("old act:%d", ad->sel_act);
	SETTING_TRACE("old network mode:%d", ad->sel_net);

	/* TODO : value is always zero need to fix */
	/*	if (TAPI_NETWORK_SELECTIONMODE_MANUAL == value) { */
	/*	snprintf(sel_network_desc, size, */
	/*			 "%s, %s", _("IDS_ST_BODY_MANUAL"), _(ad->sel_network)); */
	/*	} */
	/*	else{ */
	snprintf(sel_network_desc, size,
	         "%s, %s", _("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"), _(ad->sel_network));
	/*	} */
}

static void __popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	const char *txt = elm_object_text_get(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (0 == safeStrCmp(txt, _(keyStr_OK))) {
		ug_destroy_me(ad->ug);
	}
}

static void __check_sim_card(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	int err = 0;
	int value;
	setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	SETTING_TRACE("sim: %d", value);
	switch (value) {
		case VCONFKEY_TELEPHONY_SIM_NOT_PRESENT:
			ad->popup = setting_create_popup(ad, ad->win_get,
			                                 NULL, _("IDS_ST_POP_INSERT_SIM_CARD_TO_ACCESS_NETWORK_SERVICES"),
			                                 __popup_cb, 0, true, true,
											 1, keyStr_OK);
			SETTING_TRACE("%s*** [ERR] No SIM. sim_slot_type=%d ***%s",
			              SETTING_FONT_RED, value, SETTING_FONT_BLACK);
			return;
			break;
		case VCONFKEY_TELEPHONY_SIM_CARD_ERROR:
		case VCONFKEY_TELEPHONY_SIM_UNKNOWN:
			ad->popup = setting_create_popup(ad, ad->win_get,
			                                 NULL, _("IDS_ST_POP_INSERT_SIM_CARD_TO_ACCESS_NETWORK_SERVICES"),
			                                  __popup_cb, 0, true, true,
											  1, keyStr_OK);
			SETTING_TRACE("%s*** [ERR] Invalid SIM. sim_slot_type=%d ***%s",
			              SETTING_FONT_RED, value, SETTING_FONT_BLACK);
			return;
			break;
		default:
			break;
	}
}

static void __popup_cb2(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	const char *txt = elm_object_text_get(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (0 == safeStrCmp(txt, _(keyStr_OK))) {
		ug_destroy_me(ad->ug);
	}
}

static void __check_flight_mode(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	/*check flight mode */
	int flight_mode = 0;
	vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight_mode);
	if (flight_mode) {
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup(ad, ad->win_get,
		                                 NULL, "IDS_ST_BODY_FLIGHT_MODE_HAS_BEEN_ENABLED_NETWORK_SETTINGS_WILL_CLOSE",
		                                 __popup_cb2, 0, true, true,
										 1, keyStr_OK);
	}
}


void data_roaming_state_event_handler(const char *event_name, bundle *data, void *user_data)
{
	const char *data_roaming_state_set = NULL;
	SETTING_TRACE("data roamings state set event(%s) received", event_name);

	data_roaming_state_set = bundle_get_val(data, EVT_KEY_DATA_ROAMING_STATE);
	SETTING_TRACE("data_roaming_state_set(%s)", data_roaming_state_set);
}

void mobile_data_state_event_handler(const char *event_name, bundle *data, void *user_data)
{
	const char *mobile_data_state_set = NULL;
	SETTING_TRACE("mobile data state set event(%s) received", event_name);

	mobile_data_state_set = bundle_get_val(data, EVT_KEY_MOBILE_DATA_STATE);
	SETTING_TRACE("mobile_data_state_set(%s)", mobile_data_state_set);
}

int data_roaming_event_reg_id;
int mobile_data_event_reg_id;



static void setting_network_mode_popup(void *data)
{

	ret_if(NULL == data);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	Evas_Object *scroller = NULL;
	ad->network_mode_popup = setting_create_popup_with_list(&scroller , ad, ad->win_get,
	                                                          "Network Mode", NULL, 0, false, false, 0);
	_P(ad->network_mode_popup);
	//Evas_Object *parentItem = ad->network_mode_popup;

	int call_status = CM_CALL_STATUS_IDLE;
	cm_client_h cm_handle = NULL;
	cm_init(&cm_handle);
	cm_get_call_status(cm_handle, &call_status);
	cm_deinit(cm_handle);
	if (CM_CALL_STATUS_IDLE != call_status) {
		setting_create_popup(ad, ad->win_get, NULL,
		                            _("IDS_CST_POP_OPERATION_NOT_ALLOWED_DURING_CALLS"), NULL, 0, false, false, 0);
		return;
	}

	int value = 0;
	int err = 0;
	Evas_Object *rdg;

	rdg = elm_radio_add(scroller);
	elm_radio_value_set(rdg, -1);

	/*Need to fix */
	char	*szCscFeatureValue = "all_numeric";
	char	*pszStrId = NULL;

	/* LTE/WCDMA/GSM */
	if (is_lte_on_feature(ad)) {
		if (strcmp(szCscFeatureValue, "all_numeric") == 0)
			pszStrId = _("IDS_ST_OPT_4G_3G_2G_HAUTO_CONNECT");
		else
			pszStrId = _("IDS_ST_BODY_LTE_WCDMA_GSM_NHAUTO_CONNECT");

		ad->lte_wcdma_gsm = setting_create_Gendial_field_1radio(scroller, &itc_multiline_1text_1icon,
	                                                              __network_sub_list_sel_cb, ad,
	                                                              SWALLOW_Type_1RADIO_RIGHT,
	                                                              rdg, TAPI_NETWORK_MODE_LTE | TAPI_NETWORK_MODE_WCDMA | TAPI_NETWORK_MODE_GSM/* chk_status */,
																  pszStrId,
	                                                              __network_default_rd_change);
	}

	/* WCDMA/GSM */
	if (strcmp(szCscFeatureValue, "all_numeric") == 0)
		pszStrId = _("IDS_ST_OPT_3G_2G_HAUTO_CONNECT");
	else
		pszStrId = _("IDS_ST_BODY_WCDMA_GSM_NHAUTO_CONNECT");



	ad->wcdma_gsm = setting_create_Gendial_field_1radio(scroller, &itc_multiline_1text_1icon,
	                                                              __network_sub_list_sel_cb, ad,
	                                                              SWALLOW_Type_1RADIO_RIGHT,
	                                                              rdg,
																  TAPI_NETWORK_MODE_WCDMA | TAPI_NETWORK_MODE_GSM,	/* chk_status */
																  pszStrId,
	                                                              __network_default_rd_change);

	/* WCDMA */
	if (strcmp(szCscFeatureValue, "all_numeric") == 0)
		pszStrId = _("IDS_ST_BODY_3G_ONLY");
	else
		pszStrId = _("IDS_ST_MBODY_WCDMA_ONLY");

	ad->wcdma_only = setting_create_Gendial_field_1radio(scroller, &itc_multiline_1text_1icon,
	                                                              __network_sub_list_sel_cb, ad,
	                                                              SWALLOW_Type_1RADIO_RIGHT,
	                                                              rdg,
																  TAPI_NETWORK_MODE_WCDMA,	/* chk_status */
																  pszStrId,
	                                                              __network_default_rd_change);

	/* GSM */
	if (strcmp(szCscFeatureValue, "all_numeric") == 0)
		pszStrId = _("IDS_ST_OPT_2G_ONLY");
	else
		pszStrId = _("IDS_ST_MBODY_GSM_ONLY");

	ad->gsm_only = setting_create_Gendial_field_1radio(scroller, &itc_multiline_1text_1icon,
	                                                              __network_sub_list_sel_cb, ad,
	                                                              SWALLOW_Type_1RADIO_RIGHT,
	                                                              rdg,
																  TAPI_NETWORK_MODE_GSM,	/* chk_status */
																  pszStrId,
	                                                              __network_default_rd_change);

	setting_get_int_slp_key(INT_SLP_SETTING_NETWORK_MODE, &value, &err);
	SETTING_TRACE("value: %d, err: %d", value, err);
	elm_radio_value_set(rdg, value);


	/* Selection */
	do {
		if (is_lte_on_feature(ad)) {
			if ((value & TAPI_NETWORK_MODE_LTE) && (value & TAPI_NETWORK_MODE_WCDMA) && (value & TAPI_NETWORK_MODE_GSM)) {
				SETTING_TRACE("TAPI_NETWORK_MODE_LTE|TAPI_NETWORK_MODE_WCDMA|TAPI_NETWORK_MODE_GSM selected");

				if (ad->lte_wcdma_gsm) {
					elm_radio_value_set(ad->data_net_mode->rgd, ad->lte_wcdma_gsm->chk_status);
					SETTING_TRACE("ad->lte_wcdma_gsm->chk_status=%d checked", ad->lte_wcdma_gsm->chk_status);
					break;
				} else
					SETTING_TRACE_ERROR("ad->lte_wcdma_gsm is NULL");
			}
		}

		if ((value & TAPI_NETWORK_MODE_WCDMA) && (value & TAPI_NETWORK_MODE_GSM)) {
			SETTING_TRACE("TAPI_NETWORK_MODE_WCDMA|TAPI_NETWORK_MODE_GSM selected");

			if (ad->wcdma_gsm) {
				elm_radio_value_set(ad->data_net_mode->rgd, ad->wcdma_gsm->chk_status);
				SETTING_TRACE("ad->wcdma_gsm->chk_status=%d checked", ad->wcdma_gsm->chk_status);
			} else
				SETTING_TRACE_ERROR("ad->wcdma_gsm is NULL");
		} else if (value & TAPI_NETWORK_MODE_WCDMA) {
			SETTING_TRACE("TAPI_NETWORK_MODE_WCDMA selected");

			if (ad->wcdma_only) {
				elm_radio_value_set(ad->data_net_mode->rgd, ad->wcdma_only->chk_status);
				SETTING_TRACE("ad->wcdma_only->chk_status=%d checked", ad->wcdma_only->chk_status);
			} else
				SETTING_TRACE_ERROR("ad->wcdma_only is NULL");
		} else if (value & TAPI_NETWORK_MODE_GSM) {
			SETTING_TRACE("TAPI_NETWORK_MODE_GSM selected");

			if (ad->gsm_only) {
				elm_radio_value_set(ad->data_net_mode->rgd, ad->gsm_only->chk_status);
				SETTING_TRACE("ad->gsm_only->chk_status=%d checked", ad->gsm_only->chk_status);
			} else
				SETTING_TRACE_ERROR("ad->gsm_only is NULL");
		} else
			SETTING_TRACE_ERROR("TAPI_NETWORK(%d) unknown", value);

	} while (0);

	SETTING_TRACE("after value set -- value: %d, err: %d", value, err);
}




static int setting_network_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	Evas_Object *scroller;

	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           _("IDS_ST_BODY_NETWORK"),
	                                           //dgettext("sys_string", "IDS_ST_BUTTON_BACK"),
	                                           NULL, /* ARROW STYLE */
	                                           NULL,/*_("IDS_ST_HEADER_HELP"), */
	                                           setting_network_main_click_softkey_back_cb,
	                                           setting_network_main_click_softkey_help_cb,
	                                           ad, &scroller,
	                                           &ad->navi_bar);

	if (scroller) {
		ad->genlist = scroller;
	} else {
		ad->genlist = NULL;
		SETTING_TRACE("genlist is NULL ERROR COND");
	}

	evas_object_smart_callback_add(ad->genlist, "realized", __gl_realized_cb, ad);

	Elm_Object_Item *item = NULL;;
	item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	/*elm_object_item_access_unregister(item); */

	// mobile data ON  --> Data Roaming activate
	// mobile data OFF --> Data Roaming de-activate

	/* [UI] TITLE : "Mobile Data" */
	int value_mobile_data;
	int err_mobile_data;
	err_mobile_data = setting_network_get_state_mobile_data(&value_mobile_data);

	ad->data_mobile_data = setting_create_Gendial_field_def(scroller, &itc_1text_1icon,
	                                                       setting_network_main_item_Gendial_mouse_up_cb,
	                                                       ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                                       NULL, value_mobile_data,
	                                                       KeyStr_UseMobileData, NULL,
	                                                       setting_network_main_chk_btn_cb);
	if (ad->data_mobile_data) {
		ad->data_mobile_data->userdata = ad;
		__BACK_POINTER_SET(ad->data_mobile_data);
	} else {
		SETTING_TRACE_ERROR("ad->data_mobile_data is NULL");
	}

	#ifdef OLD_GENLIST_STYLE
	/* [UI] "Mobile Data" descrition */
	const char *title = Data_packet_Sub_desc;
	ADD_GL_HELP(scroller, title);
	#endif

	/* [UI] Data Roaming - TITLE : "Data Roaming" */
	int err_data_roaming;
	int value_data_roaming;
	err_data_roaming = setting_network_get_state_data_roaming(&value_data_roaming);

	SETTING_TRACE_ERROR(" ---> data roaming value : %d", value_data_roaming);

	ad->data_roaming = setting_create_Gendial_field_def(scroller, &(itc_1text_1icon),
	                                                    setting_network_main_item_Gendial_mouse_up_cb,
	                                                    ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                                    NULL, value_data_roaming,
	                                                    KeyStr_DataRoaming, NULL,
	                                                    setting_network_main_chk_btn_cb);
	if (ad->data_roaming) {
		ad->data_roaming->userdata = ad;
		//ad->data_roaming->isItemDisableFlag = value_mobile_data;
		__BACK_POINTER_SET(ad->data_roaming);
	} else {
		SETTING_TRACE_ERROR("ad->data_roaming is NULL");
	}


	#ifdef OLD_GENLIST_STYLE
	ADD_GL_HELP(scroller, SETTING_NETWORK_DATA_ROMING_DESC);
	#endif
	/* [UI] Network Mode */
	ad->data_net_mode =
	    setting_create_Gendial_field_def(scroller,
	                                     &itc_2text_3_parent,
										 setting_network_main_item_Gendial_mouse_up_cb,
										 ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                     0, "IDS_ST_BODY_NETWORK_MODE",
	                                     NULL,
	                                     NULL);

	if (ad->data_net_mode) {
		ad->data_net_mode->int_slp_setting_binded = INT_SLP_SETTING_NETWORK_MODE;
		__BACK_POINTER_SET(ad->data_net_mode);
	} else {
		SETTING_TRACE_ERROR("ad->data_net_modeis NULL");
	}

	/*  Don't need to check flight mode,if flight mode is on, thw whole Network function will be disable*/
	/*  ***BEGIN***  Fixed the problem of word twinkle  SAMSUNG 2010/7/21 add */
	char sel_network_desc[MAX_COMMON_BUFFER_LEN] = {0,};
	_get_network_selected_desc(ad, sel_network_desc, MAX_COMMON_BUFFER_LEN);
	ad->data_sel_net = setting_create_Gendial_field_def(scroller,
	                                                    &itc_2text_3,
	                                                    setting_network_main_item_Gendial_mouse_up_cb,
	                                                    ad, SWALLOW_Type_INVALID,
	                                                    NULL, NULL, 0,
	                                                    "IDS_COM_BODY_NETWORK_OPERATORS",
	                                                    sel_network_desc, NULL);
	if (ad->data_sel_net) {
		__BACK_POINTER_SET(ad->data_sel_net);
		ad->data_sel_net->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_sel_net is NULL");
	}


	/* [UI] Connection mode description */
	ad->data_connection = setting_create_Gendial_field_def(scroller, &itc_1text,
	                                                       setting_network_main_item_Gendial_mouse_up_cb,
	                                                       ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                                       0, CONNECTION_DSC,
	                                                       NULL, NULL);
	if (ad->data_connection) {
		__BACK_POINTER_SET(ad->data_connection);
		ad->data_connection->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_connection is NULL");
	}

	vconf_notify_key_changed(VCONFKEY_3G_ENABLE, __3g_change_cb, ad);
	vconf_notify_key_changed(VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL, __data_roaming_change_cb, ad);
	vconf_notify_key_changed(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, __net_restriction_mode_vconf_change_cb, ad);

	setting_view_network_main.is_create = 1;
	setting_view_update(&setting_view_network_main, ad);

	/*check sim card */
	__check_sim_card(ad);
	__check_flight_mode(ad);

	/* eventsystem */
	if (ES_R_OK != eventsystem_register_event(SYS_EVENT_MOBILE_DATA_STATE,
	                                          &mobile_data_event_reg_id,
	                                          (eventsystem_handler)mobile_data_state_event_handler, cb)) {
		SETTING_TRACE_ERROR("error");
	}

	if (ES_R_OK != eventsystem_register_event(SYS_EVENT_DATA_ROAMING_STATE,
	                                          &data_roaming_event_reg_id,
	                                          (eventsystem_handler)data_roaming_state_event_handler, cb)) {
		SETTING_TRACE_ERROR("error");
	}

	// mobile data On -> data roaming activates
	// mobile data Off -> data roaming deactivates
	SETTING_TRACE(" ---> mobile-data value : %d", value_mobile_data);
	if (value_mobile_data)
		_setting_network_activate_state_data_roaming(ad, true);
	else
		_setting_network_activate_state_data_roaming(ad, false);

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	vconf_ignore_key_changed(VCONFKEY_3G_ENABLE, __3g_change_cb);
	vconf_ignore_key_changed(VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL, __data_roaming_change_cb);
	vconf_ignore_key_changed(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, __net_restriction_mode_vconf_change_cb);

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}
	if (ad->network_ug_pop) {
		evas_object_del(ad->network_ug_pop);
		ad->network_ug_pop = NULL;
	}
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}
	ad->data_sel_net = NULL;/*reset NULL after genlis destoryed with ly_main. */

	setting_view_network_main.is_create = 0;

	if (ES_R_OK != eventsystem_unregister_event(mobile_data_event_reg_id)) {
		SETTING_TRACE_ERROR("error");
	}
	if (ES_R_OK != eventsystem_unregister_event(data_roaming_event_reg_id)) {
		SETTING_TRACE_ERROR("error");
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_cleanup(void *cb)
{
	/* error check */
	/* retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER); */

	/* SettingNetworkUG *ad = (SettingNetworkUG *)cb; */

	/* all the view in view stack shared the ad->ly_main, it cannot be hided!!! */
	/* if(ad->ly_main != NULL) */
	/* { */
	/* evas_object_hide(ad->ly_main); */
	/* } */

	return SETTING_RETURN_SUCCESS;
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
setting_network_main_click_softkey_help_cb(void *data, Evas_Object *obj,
                                           void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
}

static void
setting_network_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                           void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	ug_destroy_me(ad->ug);
}

static Eina_Bool __change_search_view_on_timer(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = data;
	retvm_if(ad == NULL, ECORE_CALLBACK_CANCEL, "ad is NULL");
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	setting_view_change(&setting_view_network_main,
	                    &setting_view_network_select_network,
	                    ad);
	/*ecore_timer_del(ad->timer); */
	ad->timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void __change_search_view_on_resp_cb(void *data, Evas_Object *obj,
                                     void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK == response_type) {
		/*changing of USE_PACKET_DATA was trigged by betwork searching */
		ad->b_trigged_by_searching = TRUE;

		/*for delay.. */
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win_get,
		                                                  PROGRESSBAR_STYLE,
		                                                  NULL, NULL, NULL, 0, TRUE, FALSE, 0);

		if (ad->timer) {
			ecore_timer_del(ad->timer);
			ad->timer = NULL;
		}
		ad->timer = ecore_timer_add(TIME_TO_TURN_OFF_3G, __change_search_view_on_timer, ad);

	} else if (POPUP_RESPONSE_CANCEL == response_type) {
	}
}

static void _setting_network_popup_mobile_data_rollback(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	int err = 0;
	int value = 0;

	//int err_mobile_data = 
	setting_network_get_state_mobile_data(&value);
	SETTING_TRACE("value = %d", value);

	/* rollback */
	setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, value, &err);
	setting_retm_if(err != 0, "set vconf failed");
	setting_update_gl_item_chk_status(ad->data_mobile_data, value);
}

static void _setting_network_popup_data_roaming_rollback(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	int err = 0;
	int value = 0;

	//int err_data_romaing = 
	setting_network_get_state_data_roaming(&value);
	SETTING_TRACE("value = %d", value);

	/* rollback */
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);
	setting_update_gl_item_chk_status(ad->data_roaming, value);
}


static void _setting_network_popup_mobile_data_hide_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	_setting_network_popup_mobile_data_rollback(ad);
//	elm_popup_dismiss(obj);
}

static void _setting_network_popup_data_roaming_hide_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	/* rollback */
	_setting_network_popup_data_roaming_rollback(ad);
//	elm_popup_dismiss(obj);
}




static void
setting_network_main_item_Gendial_mouse_up_cb(void *data, Evas_Object *obj, void *event_info)
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

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	int ret;
	int reminder_flag = TRUE;
	int err;

	if (!safeStrCmp("IDS_ST_BODY_NETWORK_MODE", list_item->keyStr)) {
		setting_network_mode_popup(ad);
	} else if (!safeStrCmp("IDS_COM_BODY_NETWORK_OPERATORS", list_item->keyStr)) {
		int call_status = CM_CALL_STATUS_IDLE;
		cm_client_h cm_handle = NULL;
		cm_init(&cm_handle);
		cm_get_call_status(cm_handle, &call_status);
		cm_deinit(cm_handle);
		if (CM_CALL_STATUS_IDLE != call_status) {
			setting_create_popup(ad, ad->win_get, NULL,
			                            _("IDS_CST_POP_OPERATION_NOT_ALLOWED_DURING_CALLS"), NULL, 0, false, false, 0);
			return;
		}

		int value_use_packet;
		vconf_get_int(VCONFKEY_DNET_STATE, &value_use_packet);

		vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &ad->sel_net);/*retrieve here */
		SETTING_TRACE("ad->sel_net:%d", ad->sel_net);

		ret = setting_view_change(&setting_view_network_main,
				&setting_view_network_select_network,
				ad);

	} else if (!safeStrCmp(KeyStr_UseMobileData, list_item->keyStr)) {
		/* new status */
		setting_update_gl_item_chk_status(ad->data_mobile_data, !(list_item->chk_status));
		if (!list_item->chk_status) {
			/* On -> Off */
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			ad->popup_data_off = setting_create_popup2(ad,
													ad->win_get,
													"IDS_ST_HEADER_TURN_OFF_MOBILE_DATA",
													"IDS_ST_POP_UNLESS_YOU_CONNECT_TO_A_WI_FI_NETWORK_YOU_WILL_NOT_BE_ABLE_TO_USE_THE_INTERNET_EMAIL_OR_OTHER_APPS_MSG",
													setting_network_use_packet_resp_cb,
													_setting_network_popup_mobile_data_hide_cb,
													0,
													FALSE,
													FALSE,
													2,
													"IDS_ST_BUTTON_DISABLE",
													"IDS_ST_BUTTON_CANCEL_ABB");
			// Evas_Object *setting_create_popup2(void *data,

			/* should set data_roming as same as status of use_packet_data */
			int data_roaming_value = 0;
			setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (data_roaming_value) {
				setting_network_set_state_mobile_data(SETTING_ON_OFF_BTN_OFF);
				setting_update_gl_item_chk_status(ad->data_roaming, 0);
			}
		} else {
			/* Off -> On */
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			ad->popup_data_on = setting_create_popup2(ad,
													ad->win_get,
													"IDS_ST_HEADER_TURN_ON_MOBILE_DATA",
													"IDS_ST_POP_MOBILE_DATA_WILL_BE_TURNED_ON_CONNECTING_TO_MOBILE_NETWORKS_MAY_RESULT_IN_ADDITIONAL_CHARGES_DEPENDING_ON_YOUR_PAYMENT_PLAN",
													setting_network_use_packet_resp_cb,
													_setting_network_popup_mobile_data_hide_cb,
													0,
													FALSE,
													FALSE,
													2,
													"IDS_ST_BUTTON_ENABLE",
													"IDS_ST_BUTTON_CANCEL_ABB");
		}

	} else if (!safeStrCmp(KeyStr_DataRoaming, list_item->keyStr)) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));

		if (list_item->chk_status) {
			ad->popup =  setting_create_popup2(ad, ad->win_get,
											  "IDS_ST_HEADER_ENABLE_DATA_ROAMING",
											  "IDS_ST_POP_ROAMING_WILL_INCUR_EXTRA_CHARGES_CONTINUE_Q",
											  __setting_network_data_roaming_resp_cb,
											 _setting_network_popup_data_roaming_hide_cb,
											  0,
											  FALSE,
											  FALSE,
											  2,
											  "IDS_ST_BUTTON_ENABLE",
											  "IDS_ST_BUTTON_CANCEL_ABB");
		}
	} else if (!safeStrCmp(CONNECTION_DSC, list_item->keyStr)) {
		ret = setting_view_change(&setting_view_network_main,
		                          &setting_view_network_con, ad);
	}
}

static void
setting_network_main_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingNetworkUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */

	int err;

	if (obj == ad->data_mobile_data->eo_check) {
		SETTING_TRACE(" TOUCH HANDLING MOBILE DATA");

		/* On -> Off */
		int reminder_flag = TRUE;
		int ret = 0;
		if (!list_item->chk_status) {
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			ad->popup_data_off = setting_create_popup2(ad,
								ad->win_get,
								"IDS_ST_HEADER_TURN_OFF_MOBILE_DATA",
								"IDS_ST_POP_UNLESS_YOU_CONNECT_TO_A_WI_FI_NETWORK_YOU_WILL_NOT_BE_ABLE_TO_USE_THE_INTERNET_EMAIL_OR_OTHER_APPS_MSG",
								setting_network_use_packet_resp_cb,
								_setting_network_popup_mobile_data_hide_cb,
								0,
								FALSE,
								FALSE,
								2,
								"IDS_ST_BUTTON_DISABLE",
								"IDS_ST_BUTTON_CANCEL_ABB");

			/* should set data_roming as same as status of use_packet_data */
			int data_roaming_value = 0;

			setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (data_roaming_value) {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_OFF, &err);
				setting_retm_if(err != 0, "set vconf failed");

				setting_update_gl_item_chk_status(ad->data_roaming, 0);
			}

		} else {
			/* Off -> On */
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			ad->popup_data_on = setting_create_popup2(ad,
								ad->win_get,
								"IDS_ST_HEADER_TURN_ON_MOBILE_DATA",
								"IDS_ST_POP_MOBILE_DATA_WILL_BE_TURNED_ON_CONNECTING_TO_MOBILE_NETWORKS_MAY_RESULT_IN_ADDITIONAL_CHARGES_DEPENDING_ON_YOUR_PAYMENT_PLAN",
								setting_network_use_packet_resp_cb,
								_setting_network_popup_mobile_data_hide_cb,
								0,
								FALSE,
								FALSE,
								2,
								"IDS_ST_BUTTON_ENABLE",
								"IDS_ST_BUTTON_CANCEL_ABB");
		}
	} else if (obj == ad->data_roaming->eo_check) {
		SETTING_TRACE(" TOUCH HANDLING DATA ROAMING");
		if (list_item->chk_status) {
			SETTING_TRACE(" TOUCH HANDLING DATA ROAMING - set true");
			ad->popup =  setting_create_popup2(ad, ad->win_get,
											  "IDS_ST_HEADER_ENABLE_DATA_ROAMING",
											  "IDS_ST_POP_ROAMING_WILL_INCUR_EXTRA_CHARGES_CONTINUE_Q",
											  __setting_network_data_roaming_resp_cb,
											 _setting_network_popup_data_roaming_hide_cb,
											  0,
											  FALSE,
											  FALSE,
											  2,
											  "IDS_ST_BUTTON_ENABLE",
											  "IDS_ST_BUTTON_CANCEL_ABB");
		} else {
			SETTING_TRACE(" TOUCH HANDLING DATA ROAMING - set false");
			setting_network_set_state_data_roaming(SETTING_ON_OFF_BTN_OFF);
			setting_update_gl_item_chk_status(ad->data_roaming, SETTING_ON_OFF_BTN_OFF);
		}
	}
}


/**
 * mobile data ON  --> Data Roaming activate
 * mobile data OFF --> Data Roaming de-activate
 */
static void setting_network_use_packet_resp_cb(void *data, Evas_Object *obj,
                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int err = 0;		/* error check   */
	int response_type = btn_type(obj);
	int value = 0;

	//int err_mobile_data = 
	setting_network_get_state_mobile_data(&value);
	setting_retm_if(err != 0, "get vconf failed");
	SETTING_TRACE("value = %d", value);

	if (POPUP_RESPONSE_OK == response_type) {

		if (value == 0) {

			setting_network_set_state_mobile_data(SETTING_ON_OFF_BTN_ON);
			setting_retm_if(err != 0, "set vconf failed");
			setting_update_gl_item_chk_status(ad->data_mobile_data, 1);
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				(void)vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, 0);
			}
			_setting_network_activate_state_data_roaming(ad, true);

			/* if called by other apps, destroy ug */
			if (!safeStrCmp(ad->view_type_string, "DATA_ROAMING")) {
				if (ad->popup) {
					evas_object_del(ad->popup);
					ad->popup = NULL;
				}
				ug_destroy_me(ad->ug);
				return;
			}
		} else {	// (value != 0)

			setting_network_set_state_mobile_data(SETTING_ON_OFF_BTN_OFF);
			setting_retm_if(err != 0, "set vconf failed");
			setting_update_gl_item_chk_status(ad->data_mobile_data, 0);
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				(void)vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, 0);
			}
			_setting_network_activate_state_data_roaming(ad, false);

			/* should set data_roming as same as status of use_packet_data */
			int data_roaming_value = 0;
			setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (data_roaming_value) {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_OFF, &err);
				setting_retm_if(err != 0, "set vconf failed");

				setting_update_gl_item_chk_status(ad->data_roaming, 0);
			}
		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {

		/* rollback */
		_setting_network_popup_mobile_data_rollback(ad);
	}
	if (ad->popup_data_off) {
		evas_object_del(ad->popup_data_off);
		ad->popup_data_off = NULL;
	}
	if (ad->popup_data_on) {
		evas_object_del(ad->popup_data_on);
		ad->popup_data_on = NULL;
	}

}

/**
 * toggle 'Data Roamding' mode as a popup callback
 * @see also setting_create_popup
 */
static void __setting_network_data_roaming_resp_cb(void *data, Evas_Object *obj,
                                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int response_type = btn_type(obj);
	SETTING_TRACE("elm_object_parent_widget_get(obj):%p", elm_object_parent_widget_get(obj));
	SETTING_TRACE("obj:%p", obj);
	if (POPUP_RESPONSE_OK == response_type) {

		SETTING_TRACE(" >>>> ad->data_roaming->chk_status = %d", ad->data_roaming->chk_status);
		//setting_update_gl_item_chk_status(ad->data_roaming, !(ad->data_roaming->chk_status));

		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);

		/* data roaming */
		if (value == 0) {
			SETTING_TRACE("1 trying ON");
			setting_network_set_state_data_roaming(SETTING_ON_OFF_BTN_ON);
			setting_update_gl_item_chk_status(ad->data_roaming, SETTING_ON_OFF_BTN_ON);

			/* if called by other apps, destroy ug */
			if (!safeStrCmp(ad->view_type_string, "DATA_ROAMING")) {
				if (ad->popup) {
					evas_object_del(ad->popup);
					ad->popup = NULL;
				}
				ug_destroy_me(ad->ug);
				return;
			}
		} else {
			SETTING_TRACE("2 trying OFF");
			setting_network_set_state_data_roaming(SETTING_ON_OFF_BTN_OFF);
			setting_update_gl_item_chk_status(ad->data_roaming, SETTING_ON_OFF_BTN_OFF);
		}
	} else {
		/*do nothing */
		/* rollback */
		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);
		setting_update_gl_item_chk_status(ad->data_roaming, value);
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

