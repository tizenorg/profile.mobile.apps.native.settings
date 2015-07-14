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
#include <efl_assist.h>
#include <eventsystem.h>
#include <bundle_internal.h>

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

#if SUPPORT_TETHERING
static void __enabled_tethering(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is enabled -- callback\n");

	if (ad && ad->data_connection)
		setting_disable_genlist_item(ad->data_connection->item);
}

static void __disabled_tethering(void *data)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	SETTING_TRACE("Tethering is disabled -- callback\n");

	if (ad && ad->data_connection)
		setting_enable_genlist_item(ad->data_connection->item);
}

static void __enabled_cb(tethering_error_e error, tethering_type_e type, bool is_requested, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	__enabled_tethering(ad);
}

static void __disabled_cb(tethering_error_e error, tethering_type_e type, tethering_disabled_cause_e code, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

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

	ad->th_main = th;

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
#ifdef NETWORK_MODE
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
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _(Invok_API_Failed_Desc), NULL, POPUP_INTERVAL, FALSE, FALSE);
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
	ad->network_ug_pop = setting_create_popup_without_btn(ad, ad->win_get, NULL,
	                                                      _("IDS_ST_BUTTON2_PROCESSING_ING"), NULL,
	                                                      0.0, TRUE, FALSE);

	/*ecore_timer_del(ad->timer); */
	ad->timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

static Eina_Bool __set_net_mode_on_delay(void *data)
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

	SETTING_TRACE("tel_set_network_mode(data_subItem->chk_status=%d) run", data_subItem->chk_status);
	int tapi_ret = tel_set_network_mode(ad->handle, data_subItem->chk_status, setting_tapi_set_band_cb, ad);

	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_mode. tapi_ret=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _(Invok_API_Failed_Desc), NULL, POPUP_INTERVAL, FALSE, FALSE);
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
		                                                  NULL, NULL, NULL, 0, TRUE, FALSE);
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

	if (value_use_packet != VCONFKEY_DNET_OFF) {
		ad->popup =
		    setting_create_popup_with_btn(subitem, ad->win_get,
		                                  NULL, _(SETTING_NETWORK_CHANGE_3G_ON_DESC),
		                                  __change_3g_on_resp_cb,
		                                  0, 2, _("IDS_ST_BUTTON_OK"), _("IDS_ST_BUTTON_CANCEL_ABB"));
		return;
	}

#if 0
	if (data_subItem->chk_status == elm_radio_value_get(ad->data_net_mode->rgd)) {
		SETTING_TRACE("chk_status EQUALS radio check -- return ");
		return;
	}
#endif
	/*it will be deleted in callback set by tel_set_network_mode(that is setting_tapi_set_band_cb.) */
	ad->network_ug_pop = setting_create_popup_without_btn(ad, ad->win_get, NULL,
	                                                      _("IDS_ST_BUTTON2_PROCESSING_ING"), NULL,
	                                                      0.0, TRUE, FALSE);

	elm_radio_value_set(ad->data_net_mode->rgd, data_subItem->chk_status);
	ad->net_mode = data_subItem->chk_status;

	/*for delay.. */
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}
	ad->timer = ecore_timer_add(0.5, __set_net_mode_on_delay, data_subItem->item);

	/*int tapi_ret = tel_set_network_mode(ad->handle, data_subItem->chk_status, setting_tapi_set_band_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		    ("%s*** [ERR] tel_set_network_mode. tapi_ret=%d ***%s",
		     SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _(Invok_API_Failed_Desc), NULL, POPUP_INTERVAL, FALSE, FALSE);
		//rollback
		int err = 0;
		int ret = setting_get_int_slp_key(ad->data_net_mode->int_slp_setting_binded,
					&(ad->net_mode), &err);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("failed to get vconf");
		}

		if (ad->net_mode == (TAPI_NETWORK_MODE_GSM|TAPI_NETWORK_MODE_WCDMA)) {
			ad->net_mode = TAPI_NETWORK_MODE_AUTO;
		}
		elm_radio_value_set(ad->data_net_mode->rgd, ad->net_mode);
		return;
	}*/
}

EXPORT_PUBLIC
void __network_default_rd_change(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	/*Setting_GenGroupItem_Data *list_item = */
	/*    (Setting_GenGroupItem_Data *) data; */
	/*list_item->chk_status = elm_radio_value_get(obj);	/*  for update * / */

	/*Elm_Object_Item *subItem = list_item->item; */
	/*Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subItem); */

	/*Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subItem);	/* subItem data * / */
	/*Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data * / */

	/*ret_if(NULL == data_subItem || NULL == data_parentItem); */
	/*int err; */
}

/**
 * expandable list handler - expand / collaps
 */
static void
__setting_network_main_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	int status = VCONFKEY_CALL_OFF;
	vconf_get_int(VCONFKEY_CALL_STATE, &status);
	if (VCONFKEY_CALL_OFF != status) {
		setting_create_simple_popup(ad, ad->win_get, NULL,
		                            _("IDS_CST_POP_OPERATION_NOT_ALLOWED_DURING_CALLS"));
		elm_genlist_item_expanded_set(event_info, FALSE);
		return;
	}

	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	int value = 0;
	int err = 0;

	Evas_Object *rgd;
	if (data_parentItem == ad->data_net_mode) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);
		data_parentItem->rgd = rgd;

		/*Need to fix */
		char	*szCscFeatureValue = "all_numeric";
		char	*pszStrId = NULL;

		/* LTE/WCDMA/GSM */
		if (is_lte_on_feature(ad)) {
			if (strcmp(szCscFeatureValue, "all_numeric") == 0)
				pszStrId = _("IDS_ST_OPT_4G_3G_2G_HAUTO_CONNECT");
			else
				pszStrId = _("IDS_ST_BODY_LTE_WCDMA_GSM_NHAUTO_CONNECT");

			ad->lte_wcdma_gsm = setting_create_Gendial_exp_sub_field(scroller,
			                                                         &itc_multiline_1text_1icon,
			                                                         __network_sub_list_sel_cb,
			                                                         ad,
			                                                         parentItem,
			                                                         SWALLOW_Type_1RADIO,
			                                                         rgd,
			                                                         TAPI_NETWORK_MODE_LTE | TAPI_NETWORK_MODE_WCDMA | TAPI_NETWORK_MODE_GSM,	/* chk_status */
			                                                         pszStrId,
			                                                         __network_default_rd_change);
			if (ad->lte_wcdma_gsm) {
				setting_genlist_item_groupstyle_set(ad->lte_wcdma_gsm, SETTING_GROUP_STYLE_CENTER);
			} else
				SETTING_TRACE_ERROR("ad->lte_wcdma_gsm is NULL");
		}

		/* WCDMA/GSM */
		if (strcmp(szCscFeatureValue, "all_numeric") == 0)
			pszStrId = _("IDS_ST_OPT_3G_2G_HAUTO_CONNECT");
		else
			pszStrId = _("IDS_ST_BODY_WCDMA_GSM_NHAUTO_CONNECT");

		ad->wcdma_gsm = setting_create_Gendial_exp_sub_field(scroller,
		                                                     &itc_multiline_1text_1icon,
		                                                     __network_sub_list_sel_cb,
		                                                     ad,
		                                                     parentItem,
		                                                     SWALLOW_Type_1RADIO,
		                                                     rgd,
		                                                     TAPI_NETWORK_MODE_WCDMA | TAPI_NETWORK_MODE_GSM,	/* chk_status */
		                                                     pszStrId,
		                                                     __network_default_rd_change);
		if (ad->wcdma_gsm) {
			setting_genlist_item_groupstyle_set(ad->wcdma_gsm, SETTING_GROUP_STYLE_CENTER);
		} else
			SETTING_TRACE_ERROR("ad->wcdma_gsm is NULL");

		/* WCDMA */
		if (strcmp(szCscFeatureValue, "all_numeric") == 0)
			pszStrId = _("IDS_ST_BODY_3G_ONLY");
		else
			pszStrId = _("IDS_ST_MBODY_WCDMA_ONLY");

		ad->wcdma_only = setting_create_Gendial_exp_sub_field(scroller,
		                                                      &itc_multiline_1text_1icon,
		                                                      __network_sub_list_sel_cb,
		                                                      ad,
		                                                      parentItem,
		                                                      SWALLOW_Type_1RADIO,
		                                                      rgd,
		                                                      TAPI_NETWORK_MODE_WCDMA,	/* chk_status */
		                                                      pszStrId,
		                                                      __network_default_rd_change);
		if (ad->wcdma_only) {
			setting_genlist_item_groupstyle_set(ad->wcdma_only, SETTING_GROUP_STYLE_CENTER);
		} else
			SETTING_TRACE_ERROR("ad->wcdma_only is NULL");

		/* GSM */
		if (strcmp(szCscFeatureValue, "all_numeric") == 0)
			pszStrId = _("IDS_ST_OPT_2G_ONLY");
		else
			pszStrId = _("IDS_ST_MBODY_GSM_ONLY");

		ad->gsm_only = setting_create_Gendial_exp_sub_field(scroller,
		                                                    &itc_multiline_1text_1icon,
		                                                    __network_sub_list_sel_cb,
		                                                    ad,
		                                                    parentItem,
		                                                    SWALLOW_Type_1RADIO,
		                                                    rgd,
		                                                    TAPI_NETWORK_MODE_GSM,	/* chk_status */
		                                                    pszStrId,
		                                                    __network_default_rd_change);
		if (ad->gsm_only) {
			setting_genlist_item_groupstyle_set(ad->gsm_only, SETTING_GROUP_STYLE_CENTER);
		} else
			SETTING_TRACE_ERROR("ad->gsm_only is NULL");

		setting_get_int_slp_key(data_parentItem->int_slp_setting_binded, &value, &err);
		SETTING_TRACE("value: %d, err: %d", value, err);

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
		if (ad->data_use_packet) {
			/* change 'use packet data' with the vconf value */
			ad->data_use_packet->chk_status = status;

			/* set toggle state */

			elm_object_item_data_set(ad->data_use_packet->item, ad->data_use_packet);
			elm_genlist_item_update(ad->data_use_packet->item);
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
		setting_update_gl_item_chk_status(ad->data_roaming, status);
		if (status) {
			int value = 0;
			int err = 0;
			setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, &value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (!value) {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, SETTING_ON_OFF_BTN_ON, &err);
			}
		}
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
			if (ad->data_use_packet)
				setting_disable_genlist_item(ad->data_use_packet->item);

			if (ad->data_roaming)
				setting_disable_genlist_item(ad->data_roaming->item);
		} else {
			if (ad->data_use_packet)
				setting_enable_genlist_item(ad->data_use_packet->item);

			if (ad->data_roaming)
				setting_enable_genlist_item(ad->data_roaming->item);
		}
	}

	return;
}

#ifdef NETWORK_MODE
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

void __check_sim_card(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	int err = 0;
	int value;
	setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	SETTING_TRACE("sim: %d", value);
	switch (value) {
		case VCONFKEY_TELEPHONY_SIM_NOT_PRESENT:
			ad->popup = setting_create_popup_with_btn(ad, ad->win_get,
			                                          NULL, _("IDS_ST_POP_INSERT_SIM_CARD_TO_ACCESS_NETWORK_SERVICES"),
			                                          __popup_cb, 0, 1, keyStr_OK);
			SETTING_TRACE("%s*** [ERR] No SIM. sim_slot_type=%d ***%s",
			              SETTING_FONT_RED, value, SETTING_FONT_BLACK);
			return;
			break;
		case VCONFKEY_TELEPHONY_SIM_CARD_ERROR:
		case VCONFKEY_TELEPHONY_SIM_UNKNOWN:
			ad->popup = setting_create_popup_with_btn(ad, ad->win_get,
			                                          NULL, _("IDS_ST_POP_INSERT_SIM_CARD_TO_ACCESS_NETWORK_SERVICES"),
			                                          __popup_cb, 0, 1, keyStr_OK);
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

void __check_flight_mode(void *cb)
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
		ad->popup = setting_create_popup_with_btn(ad, ad->win_get,
		                                          NULL, "IDS_ST_BODY_FLIGHT_MODE_HAS_BEEN_ENABLED_NETWORK_SETTINGS_WILL_CLOSE",
		                                          __popup_cb2, 0, 1, keyStr_OK);
	}
}

void __check_net_restriction_mode(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	int status = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &status);

	if (status) {
		if (ad->data_use_packet)
			setting_disable_genlist_item(ad->data_use_packet->item);

		if (ad->data_roaming)
			setting_disable_genlist_item(ad->data_roaming->item);
	} else {
		if (ad->data_use_packet)
			setting_enable_genlist_item(ad->data_use_packet->item);

		if (ad->data_roaming)
			setting_enable_genlist_item(ad->data_roaming->item);
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
	                                           dgettext("sys_string", "IDS_ST_BUTTON_BACK"),
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

#ifdef NETWORK_MODE
	setting_enable_expandable_genlist(scroller, ad, __setting_network_main_exp_cb, NULL);
#endif

	Elm_Object_Item *item = NULL;;

	item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	/*elm_object_item_access_unregister(item); */

	/* [UI] Use packet data */
	int value_use_packet;
	int err_use_packet;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
	                         &value_use_packet, &err_use_packet);

	ad->data_use_packet = setting_create_Gendial_field_def(scroller, &itc_1text_1icon,
	                                                       setting_network_main_item_Gendial_mouse_up_cb,
	                                                       ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                                       NULL, value_use_packet,
	                                                       KeyStr_UsePacketData, NULL,
	                                                       setting_network_main_chk_btn_cb);
	if (ad->data_use_packet) {
		ad->data_use_packet->userdata = ad;
		__BACK_POINTER_SET(ad->data_use_packet);
	} else {
		SETTING_TRACE_ERROR("ad->data_use_packet is NULL");
	}

	/*const char *title = "Enable data access by Mobile network"; feature */
	const char *title = Data_packet_Sub_desc;
	/* [UI] packet data descrition */
	ADD_GL_HELP(scroller, title);
	/* [UI] data roaming */
	int value_data_roaming = 0;
	int err_data_roaming = 0;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING,
	                         &value_data_roaming, &err_data_roaming);
	if (err_data_roaming != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->data_roaming = setting_create_Gendial_field_def(scroller, &(itc_1text_1icon),
	                                                    setting_network_main_item_Gendial_mouse_up_cb,
	                                                    ad, SWALLOW_Type_1ICON_1RADIO, NULL,
	                                                    NULL, value_data_roaming,
	                                                    KeyStr_DataRoaming, NULL,
	                                                    setting_network_main_chk_btn_cb);
	if (ad->data_roaming) {
		ad->data_roaming->userdata = ad;
		__BACK_POINTER_SET(ad->data_roaming);
	} else {
		SETTING_TRACE_ERROR("ad->data_roaming is NULL");
	}

	ADD_GL_HELP(scroller, SETTING_NETWORK_DATA_ROMING_DESC);

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
		ad->data_sel_net->group_style = SETTING_GROUP_STYLE_TOP;
	} else {
		SETTING_TRACE_ERROR("ad->data_sel_net is NULL");
	}

#ifdef NETWORK_MODE
	/* [UI] Network Mode */
	/* Not appeared in advance.
	char* sub_des  = _get_network_mode_sub_desc();
	int value = TAPI_NETWORK_MODE_AUTO;
	vconf_get_int(VCONFKEY_SETAPPL_NETWORK_MODE_INT, &value);
	if (value == TAPI_NETWORK_MODE_AUTO && is_lte_on_feature(ad))
	{
		sub_des = _("IDS_BR_OPT_LTE_GSM_WCDMA_HAUTO_CONNECT");
	}

	*/

	ad->data_net_mode = setting_create_Gendial_exp_parent_field(scroller,
	                                                            &itc_2text_3_parent,
	                                                            NULL, NULL,
	                                                            SWALLOW_Type_INVALID,
	                                                            "IDS_ST_BODY_NETWORK_MODE",
	                                                            NULL,/*sub_des, */
	                                                            SETTING_GROUP_STYLE_CENTER,
	                                                            SETTING_GROUP_STYLE_CENTER);
	if (ad->data_net_mode) {
		__BACK_POINTER_SET(ad->data_net_mode);
		ad->data_net_mode->int_slp_setting_binded =
		    INT_SLP_SETTING_NETWORK_MODE;
		ad->data_net_mode->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_back is NULL");
	}
#endif

	/* [UI] Connection mode description */
	ad->data_connection = setting_create_Gendial_field_def(scroller, &itc_1text,
	                                                       setting_network_main_item_Gendial_mouse_up_cb,
	                                                       ad, SWALLOW_Type_INVALID, NULL, NULL,
	                                                       0, CONNECTION_DSC,
	                                                       NULL, NULL);
	if (ad->data_connection) {
		__BACK_POINTER_SET(ad->data_connection);
		ad->data_connection->userdata = ad;
		ad->data_connection->group_style = SETTING_GROUP_STYLE_BOTTOM;
	} else {
		SETTING_TRACE_ERROR("ad->data_connection is NULL");
	}

	vconf_notify_key_changed(VCONFKEY_3G_ENABLE, __3g_change_cb, ad);
	vconf_notify_key_changed(VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL, __data_roaming_change_cb, ad);
	vconf_notify_key_changed(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, __net_restriction_mode_vconf_change_cb, ad);

#if SUPPORT_TETHERING
	is_tethering_enabled(ad);
#endif
	setting_view_network_main.is_create = 1;
	setting_view_update(&setting_view_network_main, ad);

	/*check sim card */
	__check_sim_card(ad);
	__check_flight_mode(ad);
	__check_net_restriction_mode(ad);

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

#if SUPPORT_TETHERING
	if (ad->th_main) {
		SETTING_TRACE("tethering destruction ");
		tethering_destroy(ad->th_main);
		ad->th_main = NULL;
	}
#endif

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

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	setting_view_change(&setting_view_network_main,
	                    &setting_view_network_main_help, ad);
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
		                                                  NULL, NULL, NULL, 0, TRUE, FALSE);

		if (ad->timer) {
			ecore_timer_del(ad->timer);
			ad->timer = NULL;
		}
		ad->timer = ecore_timer_add(TIME_TO_TURN_OFF_3G, __change_search_view_on_timer, ad);

	} else if (POPUP_RESPONSE_CANCEL == response_type) {
	}
}

Evas_Object *_create_popup(Evas_Object *parent, char *content, setting_call_back_func response_cb, void *data)
{
	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, response_cb, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);

	elm_object_domain_translatable_text_set(popup, SETTING_PACKAGE, content);

	Evas_Object *btn = elm_button_add(popup);
	elm_object_domain_translatable_text_set(btn, SYSTEM_PACKAGE, "IDS_ST_BUTTON_OK");
	if (response_cb) {
		evas_object_smart_callback_add(btn, "clicked",
		                               response_cb, data);
	}
	evas_object_show(btn);
	elm_object_style_set(btn, "popup");
	elm_object_part_content_set(popup, "button1", btn);

	evas_object_show(popup);
	return popup;
}

Evas_Object *_create_popup_with_btn(void *data,
                                    Evas_Object *parent,
                                    char *title,
                                    char *text,
                                    setting_call_back_func response_cb,
                                    int timeout,
                                    int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, NULL);
	setting_add_hardkey_features(popup, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	if (text) {
		elm_object_domain_translatable_text_set(popup, SETTING_PACKAGE, text);
	}
	if (title) {
		elm_object_domain_translatable_part_text_set(popup, SETTING_PACKAGE,
		                                             "title,text", title);
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	va_list args;
	va_start(args, btn_num);

	char *btn_str[3];
	int argno = 0;

	char *btn_part_str[] = {
		_("button1"), _("button2"), _("button3")
	};

	int i = 0;
	for (; i < btn_num; i++) {
		btn_str[i] = va_arg(args, char *);
	}

	for (; argno < btn_num; argno++) {
		if (btn_str[btn_num - 1 - argno]) {
			Evas_Object *btn = setting_create_button(popup, btn_str[btn_num - 1 - argno], NULL, response_cb, data);
			elm_object_style_set(btn, "popup");
			char *p = dgettext(SETTING_PACKAGE, btn_str[btn_num - 1 - argno]);
			if (safeStrCmp(p, btn_str[btn_num - 1 - argno])) {
				elm_object_domain_translatable_text_set(btn, SETTING_PACKAGE, btn_str[btn_num - 1 - argno]);
			} else {
				elm_object_domain_translatable_text_set(btn, SYSTEM_PACKAGE, btn_str[btn_num - 1 - argno]);
			}
			elm_object_part_content_set(popup, btn_part_str[argno], btn);
		}
	}
	va_end(args);

	evas_object_show(popup);
	SETTING_TRACE_END;
	return popup;
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

	if (!safeStrCmp("IDS_COM_BODY_NETWORK_OPERATORS", list_item->keyStr)) {
		int status = VCONFKEY_CALL_OFF;
		vconf_get_int(VCONFKEY_CALL_STATE, &status);
		if (VCONFKEY_CALL_OFF != status) {
			setting_create_simple_popup(ad, ad->win_get, NULL,
			                            _("IDS_CST_POP_OPERATION_NOT_ALLOWED_DURING_CALLS"));
			return;
		}

		int value_use_packet;
		vconf_get_int(VCONFKEY_DNET_STATE, &value_use_packet);

		vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &ad->sel_net);/*retrieve here */
		SETTING_TRACE("ad->sel_net:%d", ad->sel_net);

		/* Not used, Enter menu directly.
		if (value_use_packet != VCONFKEY_DNET_OFF && TAPI_NETWORK_SELECTIONMODE_MANUAL == ad->sel_net)
		{
			ad->popup =
			    setting_create_popup_with_btn(ad, ad->win_get,
						 NULL, _(SETTING_NETWORK_SEARCH_3G_ON_DESC),
						 __change_search_view_on_resp_cb,
						 0, 2, _("IDS_ST_BUTTON_OK"),_("IDS_ST_BUTTON_CANCEL_ABB"));
		}
		else
		*/
		{
			ret = setting_view_change(&setting_view_network_main,
			                          &setting_view_network_select_network,
			                          ad);
		}

	} else if (!safeStrCmp(KeyStr_UsePacketData, list_item->keyStr)) {
		/* new status */
		setting_update_gl_item_chk_status(ad->data_use_packet, !(list_item->chk_status));
		if (!list_item->chk_status) {
			/* On -> Off */
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			if (reminder_flag) {
				ad->popup_data_off =
				    setting_create_popup_with_label_check(ad, ad->win_get,
				                                          _(KeyStr_Network_Turn_Off_Mobile_Data),
				                                          _(KeyStr_Network_Mobile_Data_Has_Been_Disabled_Msg),
				                                          "IDS_ST_POP_DO_NOT_ASK_AGAIN",
				                                          &(ad->popup_chk),
				                                          setting_network_use_packet_resp_cb,
				                                          0, 2, "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_DISABLE");
				/*0, 2,"IDS_ST_BUTTON_CANCEL_ABB","IDS_ST_BUTTON_OK"); */
			} else {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
				                         SETTING_ON_OFF_BTN_OFF, &err);
				setting_retm_if(err != 0, "set vconf failed");

				setting_update_gl_item_chk_status(ad->data_use_packet, 0);


				/* should set data_roming as same as status of use_packet_data */
				int data_roaming_value = 0;

				setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
				setting_retm_if(err != 0, "get vconf failed");
				if (data_roaming_value) {
					setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING,
					                         SETTING_ON_OFF_BTN_OFF, &err);
					setting_retm_if(err != 0, "set vconf failed");

					setting_update_gl_item_chk_status(ad->data_roaming, 0);
				}
			}
		} else {
			/* Off -> On */
			/*
			int err = 0;
			setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
						 SETTING_ON_OFF_BTN_ON, &err);*/

			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			if (reminder_flag) {
				ad->popup_data_on =
				    setting_create_popup_with_label_check(ad, ad->win_get,
				                                          _(KeyStr_Network_Turn_On_Mobile_Data),
				                                          _(Data_packet_Popup_desc),
				                                          "IDS_ST_POP_DO_NOT_ASK_AGAIN",
				                                          &(ad->popup_chk),
				                                          setting_network_use_packet_resp_cb,
				                                          0, 2, "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_ENABLE");
				/*0, 2,"IDS_ST_BUTTON_CANCEL_ABB","IDS_ST_BUTTON_CONTINUE"); */
			} else {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
				                         SETTING_ON_OFF_BTN_ON, &err);
				setting_retm_if(err != 0, "set vconf failed");

				setting_update_gl_item_chk_status(ad->data_use_packet, 1);

			}
		}

	} else if (!safeStrCmp(KeyStr_DataRoaming, list_item->keyStr)) {
		setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));

		if (list_item->chk_status) {
			ad->popup =  _create_popup_with_btn(ad, ad->win_get,
			                                    NULL, _("IDS_ST_POP_ROAMING_WILL_INCUR_EXTRA_CHARGES_CONTINUE_Q") ,
			                                    __setting_network_data_roaming_resp_cb,
			                                    0, 2, "IDS_ST_BUTTON_ENABLE", "IDS_ST_BUTTON_CANCEL_ABB");
			/*0, 2, "IDS_ST_BUTTON_CONTINUE","IDS_ST_BUTTON_CANCEL_ABB"); */
		} else {
			ad->popup =  _create_popup_with_btn(ad, ad->win_get,
			                                    NULL, _("IDS_ST_BODY_WHEN_DATA_ROAMING_IS_DISABLED_APPLICATIONS_THAT_USE_THE_DATA_CONNECTION_MAY_NOT_WORK") ,
			                                    __setting_network_data_roaming_resp_cb,
			                                    0, 2, "IDS_ST_BUTTON_DISABLE", "IDS_ST_BUTTON_CANCEL_ABB");
			/*0, 2, "IDS_ST_BUTTON_OK","IDS_ST_BUTTON_CANCEL_ABB"); */
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
	if (obj == ad->data_use_packet->eo_check) {
		/* On -> Off */
		int reminder_flag = TRUE;
		int ret = 0;
		if (!list_item->chk_status) {
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			if (reminder_flag) {
				ad->popup_data_off =
				    setting_create_popup_with_label_check(ad, ad->win_get,
				                                          _(KeyStr_Network_Turn_Off_Mobile_Data),
				                                          _(KeyStr_Network_Mobile_Data_Has_Been_Disabled_Msg),
				                                          "IDS_ST_POP_DO_NOT_ASK_AGAIN",
				                                          &(ad->popup_chk),
				                                          setting_network_use_packet_resp_cb,
				                                          0, 2, "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_DISABLE");
				/*0, 2, "IDS_ST_BUTTON_CANCEL_ABB","IDS_ST_BUTTON_OK"); */
			} else {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
				                         SETTING_ON_OFF_BTN_OFF, &err);
				if (0 != err) {
					SETTING_TRACE_ERROR("set vconf failed");
					return;
				}
				setting_update_gl_item_chk_status(ad->data_use_packet, 0);

				/* should set data_roming as same as status of use_packet_data */
				int data_roaming_value = 0;

				setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
				setting_retm_if(err != 0, "get vconf failed");
				if (data_roaming_value) {
					setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING,
					                         SETTING_ON_OFF_BTN_OFF, &err);
					setting_retm_if(err != 0, "set vconf failed");

					setting_update_gl_item_chk_status(ad->data_roaming, 0);
				}

			}

		} else {
			/* Off -> On */
			ret = vconf_get_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, &reminder_flag);
			if (0 != ret) {
				SETTING_TRACE_ERROR("Failed to get vconf value");
			}
			if (reminder_flag) {
				ad->popup_data_on =
				    setting_create_popup_with_label_check(ad, ad->win_get,
				                                          _(KeyStr_Network_Turn_On_Mobile_Data),
				                                          _(Data_packet_Popup_desc),
				                                          "IDS_ST_POP_DO_NOT_ASK_AGAIN",
				                                          &(ad->popup_chk),
				                                          setting_network_use_packet_resp_cb,
				                                          0, 2, "IDS_ST_BUTTON_CANCEL_ABB", "IDS_ST_BUTTON_ENABLE");
				/*0, 2, "IDS_ST_BUTTON_CANCEL_ABB","IDS_ST_BUTTON_CONTINUE"); */
			} else {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
				                         SETTING_ON_OFF_BTN_ON, &err);
				setting_retm_if(err != 0, "set vconf failed");
				setting_update_gl_item_chk_status(ad->data_use_packet, 1);
			}
		}
	} else if (obj == ad->data_roaming->eo_check) {
		if (list_item->chk_status) {
			ad->popup =  _create_popup_with_btn(ad, ad->win_get,
			                                    NULL, _("IDS_ST_POP_ROAMING_WILL_INCUR_EXTRA_CHARGES_CONTINUE_Q") ,
			                                    __setting_network_data_roaming_resp_cb,
			                                    0, 2, "IDS_ST_BUTTON_ENABLE", "IDS_ST_BUTTON_CANCEL_ABB");
			/*0, 2, "IDS_ST_BUTTON_CONTINUE","IDS_ST_BUTTON_CANCEL_ABB"); */
		} else {
			ad->popup =  _create_popup_with_btn(ad, ad->win_get,
			                                    NULL, _("IDS_ST_BODY_WHEN_DATA_ROAMING_IS_DISABLED_APPLICATIONS_THAT_USE_THE_DATA_CONNECTION_MAY_NOT_WORK") ,
			                                    __setting_network_data_roaming_resp_cb,
			                                    0, 2, "IDS_ST_BUTTON_DISABLE", "IDS_ST_BUTTON_CANCEL_ABB");
			/*0, 2, "IDS_ST_BUTTON_OK","IDS_ST_BUTTON_CANCEL_ABB"); */
		}
	}
}

static void
setting_network_use_packet_resp_cb(void *data, Evas_Object *obj,
                                   void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int err = 0;		/* error check   */
	int response_type = btn_type(obj);

	int value = 0;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, &value, &err);
	setting_retm_if(err != 0, "get vconf failed");
	SETTING_TRACE("value = %d", value);
	if (POPUP_RESPONSE_OK == response_type) {
		if (value == 0) {
			setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
			                         SETTING_ON_OFF_BTN_ON, &err);
			setting_set_event_system(SYS_EVENT_MOBILE_DATA_STATE, EVT_KEY_MOBILE_DATA_STATE, EVT_VAL_MOBILE_DATA_ON);

			setting_retm_if(err != 0, "set vconf failed");

			setting_update_gl_item_chk_status(ad->data_use_packet, 1);
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				(void)vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_ON_REMINDER, 0);
			}
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
			setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
			                         SETTING_ON_OFF_BTN_OFF, &err);
			setting_set_event_system(SYS_EVENT_MOBILE_DATA_STATE, EVT_KEY_MOBILE_DATA_STATE, EVT_VAL_MOBILE_DATA_OFF);

			setting_retm_if(err != 0, "set vconf failed");

			setting_update_gl_item_chk_status(ad->data_use_packet, 0);
			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				(void)vconf_set_bool(VCONFKEY_SETAPPL_MOBILE_DATA_OFF_REMINDER, 0);
			}

			/* should set data_roming as same as status of use_packet_data */
			int data_roaming_value = 0;
			setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &data_roaming_value, &err);
			setting_retm_if(err != 0, "get vconf failed");
			if (data_roaming_value) {
				setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING,
				                         SETTING_ON_OFF_BTN_OFF, &err);
				setting_retm_if(err != 0, "set vconf failed");

				setting_update_gl_item_chk_status(ad->data_roaming, 0);
			}
		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		/* rollback */
		setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA,
		                         value, &err);
		setting_retm_if(err != 0, "set vconf failed");
		setting_update_gl_item_chk_status(ad->data_use_packet, value);
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
 * @see also setting_create_popup_with_btn
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
		setting_update_gl_item_chk_status(ad->data_roaming, !(ad->data_roaming->chk_status));

		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);

		/* data roaming */
		/* 0 --> 1 */
		/* 1 --> 0 */
		if (value == 0) {
			setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_ON, &err);
			setting_set_event_system(SYS_EVENT_DATA_ROAMING_STATE, EVT_KEY_DATA_ROAMING_STATE, EVT_VAL_DATA_ROAMING_ON);

			setting_retm_if(err != 0, "set vconf failed");

			setting_update_gl_item_chk_status(ad->data_roaming, 1);

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
			setting_set_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, SETTING_ON_OFF_BTN_OFF, &err);
			setting_set_event_system(SYS_EVENT_DATA_ROAMING_STATE, EVT_KEY_DATA_ROAMING_STATE, EVT_VAL_DATA_ROAMING_OFF);

			setting_retm_if(err != 0, "set vconf failed");

			setting_update_gl_item_chk_status(ad->data_roaming, 0);
		}
	} else {
		/*do nothing */
		/* rollback */
		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_DATA_ROAMING, &value, &err);
		setting_retm_if(err != 0, "set vconf failed");
		setting_update_gl_item_chk_status(ad->data_roaming, value);
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

