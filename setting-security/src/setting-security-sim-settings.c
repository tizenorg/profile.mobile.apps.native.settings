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
#include <setting-common-data-type.h>
#include <setting-common-data-slp-setting.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <setting-security-sim-settings.h>
#include <setting-security-main.h>
#include <setting-debug.h>

static int setting_security_sim_settings_create(void *cb);
static int setting_security_sim_settings_destroy(void *cb);
static int setting_security_sim_settings_update(void *cb);
static int setting_security_sim_settings_cleanup(void *cb);

setting_view setting_view_security_sim_settings = {
	.create = setting_security_sim_settings_create,
	.destroy = setting_security_sim_settings_destroy,
	.update = setting_security_sim_settings_update,
	.cleanup = setting_security_sim_settings_cleanup, };

/* ***************************************************
 **
 **basic func
 **
 ****************************************************/

static int setting_security_sim_settings_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;

	Evas_Object *scroller;
	/*Elm_Object_Item *item = NULL;*/

	/* add basic layout */
	setting_push_layout_navi_bar_genlist(ad->md.view_layout, ad->md.win_main,
	SECURITY_SIM_SETTINGS, _("IDS_ST_BUTTON_BACK"), NULL,
			(setting_call_back_func)setting_security_sim_settings_click_softkey_back_cb,
			NULL, ad, &scroller, ad->md.navibar_main);

	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			NULL);

	/* separator */
	/*em = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 * ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/* to do : sim settings menu */
	/* g. pin lock */
	/*if TAPI is responsed in time, ad->sim_status will have invalid value,
	 * or else ad->data_pin_lk will be updated by
	 * setting_security_sim_get_facility_cb */
	ad->data_pin_lk = setting_create_Gendial_field_def(scroller,
			&itc_1text_1icon,
			setting_security_sim_settings_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_1ICON_1RADIO,
			NULL, NULL, ad->pin1_status, "IDS_ST_BODY_PIN_LOCK",
			NULL, setting_security_sim_settings_chk_btn_cb);
	if (ad->data_pin_lk)
		ad->data_pin_lk->userdata = ad;

	/* h. change pin1 */
	ad->data_change_pin1 = setting_create_Gendial_field_def(scroller,
			&(ad->itc_1text),
			setting_security_sim_settings_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID,
			NULL, NULL, 0, "IDS_ST_HEADER_CHANGE_PIN_ABB", NULL,
			NULL);

	if (ad->data_change_pin1)
		ad->data_change_pin1->userdata = ad;

	if (!ad->pin1_status && ad->data_change_pin1)
		setting_disable_genlist_item(ad->data_change_pin1->item);

	/* i. change pin2 */
	if (ad->pin2_blocked_flag) {
		ad->data_change_pin2 = setting_create_Gendial_field_def(
				scroller, &(ad->itc_1text),
				setting_security_sim_settings_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_INVALID,
				NULL, NULL, 0, "IDS_CST_BODY_UNBLOCK_PIN2",
				NULL, NULL);
	} else {
		ad->data_change_pin2 = setting_create_Gendial_field_def(
				scroller, &(ad->itc_1text),
				setting_security_sim_settings_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_INVALID,
				NULL, NULL, 0, "IDS_ST_BODY_CHANGE_PIN2", NULL,
				NULL);
	}

	if (ad->data_change_pin2)
		ad->data_change_pin2->userdata = ad;

#if SUPPORT_FDN
	/* j. FDN */
	/*if TAPI is responsed in time, ad->sim_status will have invalid
	 * value, or else ad->data_fdn will be updated by
	 * setting_security_sim_get_facility_cb */
	ad->data_fdn = setting_create_Gendial_field_def(scroller,
			&(ad->itc_1text_1icon),
			setting_security_sim_settings_mouse_up_Gendial_list_cb,
			ad,
			SWALLOW_Type_1TOGGLE,
			NULL, NULL,
			ad->pin2_status, Keystr_FDN, NULL,
			setting_security_sim_settings_chk_btn_cb);
	if (ad->data_fdn)
	ad->data_fdn->userdata = ad;
#endif

	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			ad);

	setting_view_security_sim_settings.is_create = 1;

	return SETTING_RETURN_SUCCESS;

}

static int setting_security_sim_settings_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;
	if (ad->tapi_async_cb_check_timer) {
		ecore_timer_del(ad->tapi_async_cb_check_timer);
		ad->tapi_async_cb_check_timer = NULL;
	}
	/*elm_naviframe_item_pop(ad->md.navibar_main); */

	setting_view_security_sim_settings.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_sim_settings_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;
	SETTING_TRACE("Update PIN menu");
	if (ad->data_pin_lk) { /*it indicates TAPI is not responsed in time */
		/*setting_update_gl_item_chk_status(ad->data_pin_lk,
		 *  ad->pin1_status); */
		ad->data_pin_lk->chk_status = ad->pin1_status;
		elm_genlist_item_fields_update(ad->data_pin_lk->item, "*",
				ELM_GENLIST_ITEM_FIELD_CONTENT);

		if (ad->data_pin_lk->chk_status == 0)
			setting_disable_genlist_item(
					ad->data_change_pin1->item);
		else
			setting_enable_genlist_item(ad->data_change_pin1->item);
	}

	if (ad->data_change_pin2 && ad->data_change_pin2->item) {
		if (ad->pin2_blocked_flag) {
			if (!safeStrCmp(ad->data_change_pin2->keyStr,
					"IDS_ST_BODY_CHANGE_PIN2")) {
				ad->data_change_pin2->keyStr = (char *)g_strdup(
						"IDS_CST_BODY_UNBLOCK_PIN2");
				elm_object_item_data_set(
						ad->data_change_pin2->item,
						ad->data_change_pin2);
				elm_genlist_item_update(
						ad->data_change_pin2->item);
			}
		} else {
			if (!safeStrCmp(ad->data_change_pin2->keyStr,
					"IDS_CST_BODY_UNBLOCK_PIN2")) {
				ad->data_change_pin2->keyStr = (char *)g_strdup(
						"IDS_ST_BODY_CHANGE_PIN2");
				elm_object_item_data_set(
						ad->data_change_pin2->item,
						ad->data_change_pin2);
				elm_genlist_item_update(
						ad->data_change_pin2->item);
			}
		}
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_sim_settings_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;
	return setting_security_sim_settings_destroy(ad);
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/

static void get_pin_lock_info_cb(TapiHandle *handle, int result, void *data,
		void *user_data)
{
	SETTING_TRACE_BEGIN;

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	/* delete popup */
	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
	ad->enter_tapi_async_cb_flag = 1;

	int old_state = ad->data_pin_lk->chk_status;/*elm_check_state_get(
	ad->data_pin_lk->eo_check); */
	if (old_state) {
		SETTING_TRACE("Current status of PIN Lock is ON");
		ad->pw_type = SETTING_SEC_PW_PIN_LOCK_OFF;
	} else {
		SETTING_TRACE("Current status of PIN Lock is OFF");
		ad->pw_type = SETTING_SEC_PW_PIN_LOCK_ON;
	}

	SETTING_TRACE_DEBUG(
			"sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]",
			sec_rt, lock->lock_type, lock->lock_status,
			lock->retry_count);
	if (lock->lock_status == 2) { /* PUK */
		SETTING_TRACE("Current status of PIN Lock is Blocked");
		ad->pw_type = SETTING_SEC_PW_PIN1_BLOCKED;
	} else if (lock->lock_status == 5) { /* Blocked */
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("PUK is blocked. Can't use PIN Lock"), NULL, 0, false,
				false, 0);
		return;
	}

	setting_security_create_password_sg(ad);
}

static Eina_Bool _check_tapi_async_cb_is_called(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, EINA_FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (!ad->enter_tapi_async_cb_flag) {
		ad->sim_popup = setting_create_popup(ad, ad->md.win_main,
		NULL, KeyStr_Security_Waiting_Sim,
		NULL, 0, TRUE, TRUE, 0);
	}
	ad->tapi_async_cb_check_timer = NULL;

	return EINA_FALSE;
}

void _draw_pin_onoff_status(void *data, Evas_Object *check)
{
	SETTING_TRACE("PIN Lock");

	ret_if(data == NULL);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	int value = 0;
	int err = 0;
	int ret = 0;

	/* 0:click pin lock, 1:change pin1 code, 2:fdn, 3: change pin2 code */
	ad->sel_item = 0;

	ret = setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);

	ret = _handle_sim_exception(ad, value);
	if (ret == SETTING_RETURN_FAIL)
		return;

	/* check sim status */
	int tapi_ret = TAPI_API_INVALID_INPUT;
	TelSimCardType_t sim_card = TAPI_SIM_CARD_TYPE_UNKNOWN;

	tapi_ret = tel_get_sim_type(ad->handle, &sim_card);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_type call failed with error code %d",
				tapi_ret);
		return;
	}
	SETTING_TRACE("SIM Type is %d", (int)sim_card);
	if (sim_card == (TelSimCardType_t)TAPI_SIM_CARD_TYPE_UNKNOWN) {
		SETTING_TRACE_DEBUG(
				"%s*** [ERR] tel_get_sim_type. sim_card=%d ***%s",
				SETTING_FONT_RED, sim_card, SETTING_FONT_BLACK);
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("IDS_SIM_BODY_INVALID_SIM_CARD"), NULL, 0, false, false,
				0);
		return;
	}

	ad->enter_tapi_async_cb_flag = 0;
	ad->sim_popup = NULL;
	ad->tapi_async_cb_check_timer = ecore_timer_add(0.5,
			_check_tapi_async_cb_is_called, ad);

	if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_SC,
			get_pin_lock_info_cb, ad) != 0) {
		SETTING_TRACE_ERROR("failed to call tel_get_sim_lock_info()");
	}
}

static void get_sim_lock_info_cb(TapiHandle *handle, int result, void *data,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL || user_data == NULL);

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	/* delete popup */
	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}

	/*elm_check_state_get(ad->data_sim_lk->eo_check); */
	int old_state = ad->data_sim_lk->chk_status;
	if (old_state) {
		SETTING_TRACE("Current status of SIM Lock is ON");
		ad->pw_type = SETTING_SEC_PW_SIM_LOCK_OFF;
	} else {
		SETTING_TRACE("Current status of SIM Lock is OFF");
		ad->pw_type = SETTING_SEC_PW_SIM_LOCK_ON;
	}

	SETTING_TRACE_DEBUG(
			"sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]",
			sec_rt, lock->lock_type, lock->lock_status,
			lock->retry_count);

	if (lock->lock_status == 5) { /* Blocked */
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("SIM is blocked. Can't use SIM Lock"), NULL, 0, false,
				false, 0);
		return;
	}

	setting_security_create_password_sg(ad);
}

void _draw_sim_onoff_status(void *data, Evas_Object *check)
{
	ret_if(data == NULL);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	int value = 0;
	int err = 0;
	int ret = 0;

	ret = setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	ret = _handle_sim_exception(ad, value);
	if (ret == SETTING_RETURN_FAIL)
		return;

	/* create popup */
	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
	ad->sim_popup = setting_create_popup(ad, ad->md.win_main,
	NULL, KeyStr_Security_Waiting_Sim,
	NULL, 0, FALSE, FALSE, 0);

	if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_PS,
			get_sim_lock_info_cb, ad) != 0) {
		SETTING_TRACE_ERROR("failed to call tel_get_sim_lock_info()");
	}
}

static void get_change_pin_info_cb(TapiHandle *handle, int result, void *data,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL || user_data == NULL);

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	SETTING_TRACE_DEBUG(
			"sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]",
			sec_rt, lock->lock_type, lock->lock_status,
			lock->retry_count);
	/* PUK required : 0x02*/
	if (lock->lock_type == TAPI_SIM_LOCK_SC
			&& lock->lock_status == TAPI_SIM_LOCK_KEY_PUK) {
		SETTING_TRACE("Current status of PIN Lock is Blocked");
		ad->pw_type = SETTING_SEC_PW_PIN1_BLOCKED;

	} else if (lock->lock_type == TAPI_SIM_LOCK_SC
			&& (lock->lock_status == TAPI_SIM_LOCK_KEY_NOT_NEED
					|| lock->lock_status == TAPI_SIM_LOCK_KEY_PIN)) {
		/* PIN required : 0x00, 0x01 */
		ad->pw_type = SETTING_SEC_PW_CHANGE_PIN1;
	} else if (lock->lock_type == TAPI_SIM_LOCK_FD
			&& lock->lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		/* PUK2 required : 0x04 */
		SETTING_TRACE("Current status of Change PIN2 is Blocked");
		ad->pw_type = SETTING_SEC_PW_PIN2_BLOCKED;
	} else if (lock->lock_type == TAPI_SIM_LOCK_FD
			&& (lock->lock_status == TAPI_SIM_LOCK_KEY_NOT_NEED
					|| lock->lock_status == TAPI_SIM_LOCK_KEY_PIN2)) {
		/* PIN2 required : 0x00, 0x03 */
		ad->pw_type = SETTING_SEC_PW_CHANGE_PIN2;
	} else if (lock->lock_status == TAPI_SIM_LOCK_PERM_BLOCKED) {
		/* Blocked : 0x05 */
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("Permanent block SIM"), NULL, 0, false, false, 0);
		return;
	}

	setting_security_create_password_sg(ad);
}

void _mouse_up_change_pin(void *data, int sel_item)
{
	ret_if(data == NULL);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	/* 0:click pin lock, 1:change pin1 code, 2:fdn, 3: change pin2 code */
	ad->sel_item = sel_item;
	int ret;
	int value = 0;
	int err;

	ret = setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	setting_retm_if(ret != 0, "failed to read vconf value");
	switch (value) {
	case VCONFKEY_TELEPHONY_SIM_INSERTED:
		break;
	case VCONFKEY_TELEPHONY_SIM_NOT_PRESENT:
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _(SECURITY_SIM_NOT_PRESENT_MSG), NULL, 0, false, false,
				0);
		SETTING_TRACE_DEBUG(
				"%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
				SETTING_FONT_RED, value, SETTING_FONT_BLACK);
		return;
		break;
	case VCONFKEY_TELEPHONY_SIM_CARD_ERROR:
	case VCONFKEY_TELEPHONY_SIM_UNKNOWN:
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("IDS_SIM_BODY_INVALID_SIM_CARD"), NULL, 0, false, false,
				0);
		SETTING_TRACE_DEBUG(
				"%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
				SETTING_FONT_RED, value, SETTING_FONT_BLACK);
		return;
		break;
	default:
		break;
	}

	/* check sim status */
	TapiResult_t tapi_ret = TAPI_API_INVALID_INPUT;
	TelSimCardType_t sim_card = TAPI_SIM_CARD_TYPE_UNKNOWN;

	tapi_ret = tel_get_sim_type(ad->handle, &sim_card);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_type call failed with error code %d",
				tapi_ret);
		return;
	}
	if (sim_card == (TelSimCardType_t)SETTING_TAPI_UNKNOWN_SIM_TYPE) {
		SETTING_TRACE_DEBUG(
				"%s*** [ERR] tel_get_sim_type. sim_card=%d ***%s",
				SETTING_FONT_RED, sim_card, SETTING_FONT_BLACK);
		setting_create_popup(NULL, ad->md.win_main,
		NULL, _("IDS_SIM_BODY_INVALID_SIM_CARD"), NULL, 0, false, false,
				0);
		return;
	}
	if (sel_item == 1 && (ad->pin1_status == TAPI_SIM_FACILITY_ENABLED)) {
		if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_SC,
				get_change_pin_info_cb, ad) != 0) {
			SETTING_TRACE_ERROR(
					"failed to call tel_get_sim_lock_info()");
		}
	} else if (sel_item == 3) {
		if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_FD,
				get_change_pin_info_cb, ad) != 0) {
			SETTING_TRACE_ERROR(
					"failed to call tel_get_sim_lock_info()");
		}
		/*ad->pw_type = SETTING_SEC_PW_CHANGE_PIN2; */
		/*setting_security_create_password_sg(ad); */
	}
}

static void setting_security_sim_settings_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (!safeStrCmp("IDS_ST_BODY_PIN_LOCK", list_item->keyStr)) {
		_draw_pin_onoff_status(ad, list_item->eo_check);

	} else if (!safeStrCmp("IDS_ST_HEADER_CHANGE_PIN_ABB",
			list_item->keyStr)) {
		/* 0:click pin lock, 1:change pin1 code, 2:fdn,
		 * 3: change pin2 code */
		_mouse_up_change_pin(ad, 1);
	} else if (!safeStrCmp("IDS_ST_BODY_CHANGE_PIN2", list_item->keyStr)) {
		/* 0:click pin lock, 1:change pin1 code, 2:fdn,
		 * 3: change pin2 code */
		_mouse_up_change_pin(ad, 3);
	} else if (!safeStrCmp("IDS_CST_BODY_UNBLOCK_PIN2",
			list_item->keyStr)) {
		ad->pw_type = SETTING_SEC_PW_PIN2_BLOCKED;
		setting_security_create_password_sg(ad);
	}
}

static void setting_security_sim_settings_chk_btn_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "[Setting > Reset] Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;
	SettingSecurityUG *ad = list_item->userdata;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);
	if (!safeStrCmp("IDS_ST_BODY_PIN_LOCK", list_item->keyStr)) {
		/*Rollback state of toggle. */
		/*it invokes async TAPIs to enable/disable, so don't change the
		 * status at first to avoid rollback */
		/*setting_update_gl_item_chk_status(list_item,
		 * !(list_item->chk_status)); */
		list_item->chk_status = !(list_item->chk_status);
		_draw_pin_onoff_status(ad, obj);

	}
#if SUPPORT_FDN
	else if (!safeStrCmp(Keystr_FDN, list_item->keyStr)) {
		/*Rollback state of toggle. */
		/*it invokes async TAPIs to enable/disable, so don't change the
		 * status at first to avoid rollback */
		setting_update_gl_item_chk_status(list_item,
				!(list_item->chk_status));
		ad->chk_fdn = obj;
		_draw_fdn_onoff_status(ad, obj);
	}
#endif
}

/*static void */
/*setting_security_sim_settings_click_softkey_back_cb(void *data,
 *		Evas_Object *obj, void *event_info) */
Eina_Bool setting_security_sim_settings_click_softkey_back_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, EINA_FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (ad->ug_passwd)
		return EINA_FALSE;

	if (ad->sim_popup)
		return EINA_FALSE;

	setting_view_change(&setting_view_security_sim_settings,
			&setting_view_security_main, ad);

	return EINA_TRUE;
	SETTING_TRACE_END;
}
