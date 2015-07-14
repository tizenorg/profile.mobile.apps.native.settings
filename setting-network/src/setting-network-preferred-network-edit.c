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
#include <setting-network-preferred-network-edit.h>

static int setting_network_preferred_network_edit_create(void *cb);
static int setting_network_preferred_network_edit_destroy(void *cb);
static int setting_network_preferred_network_edit_update(void *cb);
static int setting_network_preferred_network_edit_cleanup(void *cb);

setting_view setting_view_network_preferred_network_edit = {
	.create = setting_network_preferred_network_edit_create,
	.destroy = setting_network_preferred_network_edit_destroy,
	.update = setting_network_preferred_network_edit_update,
	.cleanup = setting_network_preferred_network_edit_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_network_preferred_network_edit_create(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *scroller = NULL;
	/* Evas_Object *outer_box = NULL; */
	setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
	                                     _(ad->
	                                       pref_list.PrefPlmnRecord
	                                       [ad->sel_pref_idx].network_name),
	                                     dgettext("sys_string",
	                                              "IDS_ST_BUTTON_CANCEL_ABB"),
	                                     dgettext("sys_string",
	                                              "IDS_SA_BUTTON_DONE_ABB"),
	                                     setting_network_preferred_network_edit_click_softkey_cancel_cb,
	                                     setting_network_preffered_network_edit_click_softkey_save_cb,
	                                     ad, &scroller, ad->navi_bar);
	Elm_Object_Item *item = NULL;;

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	char mcc[SETTING_NETWORK_PREFERRED_NETWORK_CODE_LEN] = { 0, };
	char mnc[SETTING_NETWORK_PREFERRED_NETWORK_CODE_LEN] = { 0, };
	int network_type = 0;
	/* SETTING_TRACE("Plmn:%s", ad->pref_list.PrefPlmnRecord[ad->sel_pref_idx].Plmn); */
	memcpy(&mcc, ad->pref_list.PrefPlmnRecord[ad->sel_pref_idx].Plmn, 3);
	memcpy(&mnc, ad->pref_list.PrefPlmnRecord[ad->sel_pref_idx].Plmn + 3,
	       3);

	if (ad->pref_list.PrefPlmnRecord[ad->sel_pref_idx].SystemType ==
	    TAPI_NETWORK_SYSTEM_GSM) {
		network_type = 0;
	} else {
		network_type = 1;
	}
	ad->data_edit_mcc =
	    setting_create_Gendial_field_def(scroller,
	                                     &itc_editfield, NULL,
	                                     NULL, SWALLOW_Type_LAYOUT_EDITFIELD,
	                                     NULL, NULL, 0,
	                                     "IDS_ST_BODY_COUNTRY_CODE", _(mcc),
	                                     setting_network_preffered_network_edit_entry_changed_cb);
	if (ad->data_edit_mcc) {
		ad->data_edit_mcc->userdata = ad;
		ad->data_edit_mcc->input_type = ELM_INPUT_PANEL_LAYOUT_NUMBERONLY;
	} else {
		SETTING_TRACE_ERROR("ad->data_edit_mcc is NULL");
	}

	ad->data_edit_mnc =
	    setting_create_Gendial_field_def(scroller,
	                                     &itc_editfield, NULL,
	                                     NULL, SWALLOW_Type_LAYOUT_EDITFIELD,
	                                     NULL, NULL, 0,
	                                     "IDS_ST_BODY_NETWORK_CODE", _(mnc),
	                                     setting_network_preffered_network_edit_entry_changed_cb);
	if (ad->data_edit_mnc) {
		ad->data_edit_mnc->userdata = ad;
		ad->data_edit_mnc->input_type = ELM_INPUT_PANEL_LAYOUT_NUMBERONLY;
	} else {
		SETTING_TRACE_ERROR("ad->data_edit_mnc is NULL");
	}
	ADD_GL_SEPARATOR(scroller);

	(void)setting_create_Gendial_field_titleItem(scroller,
	                                             &itc_group_item,
	                                             "IDS_ST_BODY_NETWORK_TYPE",
	                                             NULL);
	ad->chk_edit_type = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->chk_edit_type, -1);

	setting_create_Gendial_field_1radio(scroller, &itc_1text_1icon_2,
	                                    setting_mouse_up_Gendial_list_radio_cb,
	                                    ad->chk_edit_type,
	                                    SWALLOW_Type_1RADIO,
	                                    ad->chk_edit_type, 0, "IDS_ST_BODY_GSM", NULL);

	setting_create_Gendial_field_1radio(scroller, &itc_1text_1icon_2,
	                                    setting_mouse_up_Gendial_list_radio_cb,
	                                    ad->chk_edit_type,
	                                    SWALLOW_Type_1RADIO,
	                                    ad->chk_edit_type, 1, "UMTS", NULL);
	elm_radio_value_set(ad->chk_edit_type, network_type);
	setting_view_network_preferred_network_edit.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_edit_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_preferred_network_edit.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_edit_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_edit_cleanup(void *cb)
{
	return setting_network_preferred_network_edit_destroy(cb);
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
setting_network_preffered_network_edit_entry_changed_cb(void *data,
                                                        Evas_Object *obj,
                                                        void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	const char *entry_str = elm_entry_entry_get(obj);
	/* SETTING_TRACE("entry_str:%s", entry_str); */
	list_item->sub_desc = (char *)g_strdup(entry_str);
}

static void
setting_network_preffered_network_edit_click_softkey_save_cb(void *data,
                                                             Evas_Object *obj,
                                                             void *event_info)
{
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter for setting_network_preffered_network_edit_click_softkey_save_cb is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int tapi_ret = TAPI_API_SUCCESS;
	TelNetworkPreferredPlmnInfo_t pref_plmn_info = { 0, };

	int network_type = 0;
	const char *mcc = NULL;
	const char *mnc = NULL;
	if (ad->data_edit_mcc) {
		mcc = ad->data_edit_mcc->sub_desc;
	}
	if (ad->data_edit_mnc) {
		mnc = ad->data_edit_mnc->sub_desc;
	}

	setting_retm_if(isEmptyStr(mcc)
	                || isEmptyStr(mcc),
	                "mcc or mnc got from editfiled error");
	SETTING_TRACE("mcc:%s, mnc:%s", mcc, mnc);
	int mcc_len = safeStrLen(mcc);
	int mnc_len = safeStrLen(mnc);
	if (COUNTRY_CODE_LEN != mcc_len) {
		if ((NETWORK_CODE_LEN < mnc_len) || (mnc_len <= 0)) {
			setting_create_popup_without_btn(ad, ad->win_get,
			                                 NULL, _("please input country code made up of 3 digital number and network code made up of 1 ~ 3 digital number"),
			                                 NULL, POPUP_INTERVAL, FALSE, FALSE);
		} else {
			setting_create_popup_without_btn(ad, ad->win_get,
			                                 NULL, _("please input country code made up of 3 digital number"),
			                                 NULL, POPUP_INTERVAL, FALSE, FALSE);
		}
		return;
	}

	if ((NETWORK_CODE_LEN < mnc_len) || (mnc_len <= 0)) {
		setting_create_popup_without_btn(ad, ad->win_get,
		                                 NULL, _("please input network code made up of 1 ~ 3 digital number"),
		                                 NULL, POPUP_INTERVAL, FALSE, FALSE);
		return;
	}

	network_type = elm_radio_value_get(ad->chk_edit_type);

	memset(&pref_plmn_info, 0x00, sizeof(TelNetworkPreferredPlmnInfo_t));
	memcpy(&pref_plmn_info,
	       &ad->pref_list.PrefPlmnRecord[ad->sel_pref_idx],
	       sizeof(TelNetworkPreferredPlmnInfo_t));

	safeCopyStr((char *)pref_plmn_info.Plmn, mcc, 3);
	safeCopyStr((char *)pref_plmn_info.Plmn + 3, mnc, 3);

	/* save network type */
	if (network_type == 0) {
		pref_plmn_info.SystemType = TAPI_NETWORK_SYSTEM_GSM;
	} else {
		pref_plmn_info.SystemType = TAPI_NETWORK_SYSTEM_UMTS;
	}
	ad->network_ug_pop =
	    setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_ST_POP_SAVING"),
	                                     NULL, 0.0, TRUE, TRUE);

	tapi_ret =
	    tel_set_network_preferred_plmn(ad->handle, TAPI_NETWORK_PREF_PLMN_EDIT,
	                                   &pref_plmn_info, setting_tapi_set_preferred_plmn_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_set_network_preferred_plmn(PLMN_EDIT) err=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("Save Failed!"), NULL, POPUP_INTERVAL, FALSE, FALSE);
	}

	ad->op_type = SETTING_NETWORK_PREFERRED_NETWORK_EDIT;
}

static void
setting_network_preferred_network_edit_click_softkey_cancel_cb(void *data,
                                                               Evas_Object *
                                                               obj,
                                                               void *event_info)
{
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter for setting_network_preffered_network_edit_click_softkey_save_cb is NULL");
	ad->op_type = SETTING_NETWORK_PREFERRED_NETWORK_NONE;
	setting_view_change(&setting_view_network_preferred_network_edit,
	                    &setting_view_network_preferred_network, ad);
}
