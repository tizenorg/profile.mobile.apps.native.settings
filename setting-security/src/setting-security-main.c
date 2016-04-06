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

#include <setting-security-main.h>
#include <setting-debug.h>
#include <app.h>
#if SUPPORT_ENCRYPTION
#include <ode.h>
#endif

static int setting_security_main_create(void *cb);
static int setting_security_main_destroy(void *cb);
static int setting_security_main_update(void *cb);
static int setting_security_main_cleanup(void *cb);

setting_view setting_view_security_main = {
	.create = setting_security_main_create,
	.destroy = setting_security_main_destroy,
	.update = setting_security_main_update,
	.cleanup = setting_security_main_cleanup,
};

static Eina_Bool setting_security_main_click_softkey_back_cb(void *data, Elm_Object_Item *it);
static void setting_security_main_mouse_up_Gendial_list_cb(void *data,
                                                           Evas_Object *obj,
                                                           void *event_info);

void __security_sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info);

#define SECURITY_ENGINE_NAME_MAX_LEN 128

#define SUPPORT_LOCKTYPE_IN_SECURITY_MENU 0
/* ***************************************************
 **
 **basic func
 **
 ****************************************************/

static int setting_security_main_create(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *) cb;

	Evas_Object *scroller;

	int ret = 0;
	int value = 0;
	int err = 0;

	gboolean b_status = FALSE;
	int tapi_ret;

	/* add basic layout */
	char setBtnStr[MAX_DISPLAY_NAME_LEN_ON_UI];
	snprintf(setBtnStr, sizeof(setBtnStr), "%s",
	         (char *)dgettext("sys_string", "IDS_ST_BUTTON_BACK"));
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           KeyStr_LockScreen,
	                                           setBtnStr, NULL,
	                                           (setting_call_back_func)setting_security_main_click_softkey_back_cb,
	                                           NULL, ad, &scroller,
	                                           &ad->navi_bar);

	/*setting_enable_expandable_genlist(scroller, ad, __security_exp_cb, NULL); */

	Elm_Object_Item *item = NULL;

	ADD_GL_SEPARATOR(scroller);

	/* Group List: Lock screen */
	(void)setting_create_Gendial_field_titleItem(scroller,
	                                             &itc_group_item,
	                                             KeyStr_LockScreen, NULL);
	item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* Lockscreen */
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
	                                 setting_security_main_mouse_up_Gendial_list_cb, ad,
	                                 SWALLOW_Type_INVALID,
	                                 NULL, NULL,
	                                 0, KeyStr_LockScreen, NULL, NULL);

	/*Seperator */
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL),
	                                 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/*Security update*/
	app_info_h app_info = NULL;
	/* replace deprecated API :
					from app_manager_get_app_info()
					to app_info_create() 			*/
	ret = app_info_create("org.tizen.msa", &app_info);
	if (ret != APP_MANAGER_ERROR_NONE) {
		SETTING_TRACE("No corresponding app_id for [%s]\n", "org.tizen.msa");
	} else {
		item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		ad->data_security_update = setting_create_Gendial_field_def(scroller, &(ad->itc_1text_1icon),
		                                                            setting_security_main_mouse_up_Gendial_list_cb, ad,
		                                                            SWALLOW_Type_INVALID,
		                                                            NULL, NULL,
		                                                            value, _(SECURITY_UPDATE_TEXT), NULL, NULL);

		if (app_info_destroy(app_info) != 0) {
			SETTING_TRACE_ERROR("app_info_destroy error for org.tizen.msa");
		}
	}
	ADD_GL_SEPARATOR(scroller);

	/* update info */
	if (ad->data_sim_settings) {
		if ((ad->handle == NULL) || (isEmulBin() == TRUE)) {
			setting_disable_genlist_item(ad->data_sim_settings->item);
		} else {
			ret = setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
			tapi_ret = tel_check_modem_power_status(ad->handle, &b_status);
			if ((tapi_ret != TAPI_API_SUCCESS)
			    || (b_status != TAPI_PHONE_POWER_STATUS_ON)
			    || (value != VCONFKEY_TELEPHONY_SIM_INSERTED)) {
				setting_disable_genlist_item(ad->data_sim_settings->item);
			}
			TelSimCardType_t cardInfo;
			tapi_ret = tel_get_sim_type(ad->handle, &cardInfo);
			if (tapi_ret == TAPI_API_SIM_PERM_BLOCKED)
				setting_disable_genlist_item(ad->data_sim_settings->item);
		}

		/* If flight mode is ON, can't use sim settings menu */
		int status = 0;
		vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &status);
		if (status == 1)
			setting_disable_genlist_item(ad->data_sim_settings->item);
	}

	setting_view_security_main.is_create = 1;

	g_ad = (SettingSecurityUG *) malloc(sizeof(SettingSecurityUG)*1);
	retv_if(!g_ad, -1);
	memcpy(g_ad, ad, sizeof(SettingSecurityUG));

	vconf_notify_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE,
	                         setting_security_main_vconf_change_cb, ad);
	vconf_notify_key_changed(VCONFKEY_WIFI_STATE,
	                         setting_security_main_vconf_change_cb, ad);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_security_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *) cb;
#if SUPPORT_ENCRYPTION
	if (ad->popup_sd_card_encryption) {
		evas_object_del(ad->popup_sd_card_encryption);
		ad->popup_sd_card_encryption = NULL;
	}
#endif
	if (ad->update_view_timer) {
		ecore_timer_del(ad->update_view_timer);
		ad->update_view_timer = NULL;
	}
	if (ad->get_security_list_timer) {
		ecore_timer_del(ad->get_security_list_timer);
		ad->get_security_list_timer = NULL;
	}
	if (ad->remove_sim_popup_timer) {
		ecore_timer_del(ad->remove_sim_popup_timer);
		ad->remove_sim_popup_timer = NULL;
	}
	if (ad->tapi_async_cb_check_timer) {
		ecore_timer_del(ad->tapi_async_cb_check_timer);
		ad->tapi_async_cb_check_timer = NULL;
	}

	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE,
	                         setting_security_main_vconf_change_cb);
	vconf_ignore_key_changed(VCONFKEY_WIFI_STATE,
	                         setting_security_main_vconf_change_cb);
	if (ad->secrity_engine_list) {
		g_list_free(ad->secrity_engine_list);
	}
	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);

		FREE(g_ad);
	}

	setting_view_security_main.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingSecurityUG *ad = (SettingSecurityUG *) cb;

	evas_object_show(ad->ly_main);

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/
static Eina_Bool
setting_security_main_click_softkey_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "[Setting > Security] Data parameter is NULL");

	SettingSecurityUG *ad = (SettingSecurityUG *) data;

	bool is_running = 0;
	app_manager_is_running("gr47by21a5.SamsungAccount", &is_running);
	if (is_running && ad->samsung_account_is_running) {	/* If samsung account is running from Find my mobile, security ug can't be destroyed. */
		return EINA_FALSE;
	}
	if (ad->ug_mt) {	/* If find my mobile ug is running, security ug can't be destroyed. */
		return EINA_FALSE;
	}

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	return EINA_TRUE;
}

int _handle_sim_exception(void *data, int sim_status)
{
	retv_if(data == NULL, SETTING_RETURN_FAIL);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;

	switch (sim_status) {
		case VCONFKEY_TELEPHONY_SIM_INSERTED:
			break;

		case VCONFKEY_TELEPHONY_SIM_NOT_PRESENT:

			setting_create_popup(NULL, ad->win_get,
			                            NULL, _(SECURITY_SIM_NOT_PRESENT_MSG), NULL, 0, false, false, 0);
			SETTING_TRACE_DEBUG
			("%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
			 SETTING_FONT_RED, sim_status, SETTING_FONT_BLACK);
			return SETTING_RETURN_FAIL;

			break;

		case VCONFKEY_TELEPHONY_SIM_CARD_ERROR:
		case VCONFKEY_TELEPHONY_SIM_UNKNOWN:

			setting_create_popup(NULL, ad->win_get,
			                            NULL, _("IDS_SIM_BODY_INVALID_SIM_CARD"), NULL, 0, false, false, 0);
			SETTING_TRACE_DEBUG
			("%s*** [ERR] INCORRECTED SIM. sim_slot_type=%d ***%s",
			 SETTING_FONT_RED, sim_status, SETTING_FONT_BLACK);
			return SETTING_RETURN_FAIL;

			break;
		default:
			break;
	}
	return SETTING_RETURN_SUCCESS;
}

static Eina_Bool __remove_sim_popup_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, EINA_FALSE);
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
	ad->remove_sim_popup_timer = NULL;
	return EINA_FALSE;
}

static Eina_Bool _check_tapi_async_cb_is_called(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, EINA_FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (!ad->enter_tapi_async_cb_flag) {
		ad->sim_popup = setting_create_popup(ad, ad->win_get,
		                                     NULL, KeyStr_Security_Waiting_Sim,
		                                     (setting_call_back_func)__remove_sim_popup_cb,
		                                     0, FALSE, FALSE, 0);
		ad->remove_sim_popup_timer = ecore_timer_add(1, __remove_sim_popup_cb, ad);
	}
	ad->tapi_async_cb_check_timer = NULL;

	return EINA_FALSE;
}

static Eina_Bool __freeze_event_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, EINA_FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;

	evas_object_freeze_events_set(ad->navi_bar, EINA_FALSE);
	ad->update_view_timer = NULL;
	return EINA_FALSE;
}

void setting_security_lockscreen_layout_ug_cb(ui_gadget_h ug,
                                              enum ug_mode mode,
                                              void *priv)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *base;

	if (!priv)
		return;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
			                                 EVAS_HINT_EXPAND);
			evas_object_show(base);
			break;
		default:
			break;
	}
	SETTING_TRACE_END;
}

static void setting_security_lockscreen_destroy_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
	}
	elm_genlist_realized_items_update(ad->genlist);
}

static void
setting_security_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
                                               void *event_info)
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

	SettingSecurityUG *ad = (SettingSecurityUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KeyStr_LockScreen, list_item->keyStr)) {
#if 1
		if (0 == app_launcher("lockscreen-options")) {
		//if (0 == app_launcher("setting-lockscreen-options-efl")) {
			ad->update_view_timer = ecore_timer_add(1, __freeze_event_timer_cb, ad);
			evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
		}
#else
		retm_if(data == NULL, "Data parameter is NULL");

		SettingSecurityUG *ad = (SettingSecurityUG *) data;

		struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
		setting_retm_if(!cbs, "calloc failed");
		cbs->layout_cb = setting_security_lockscreen_layout_ug_cb;
		cbs->result_cb = NULL;
		cbs->destroy_cb = setting_security_lockscreen_destroy_ug_cb;
		cbs->priv = (void *)ad;

		elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);

		SETTING_TRACE("To load ug[%s]", "lockscreen-options");
		setting_ug_create(ad->ug, "lockscreen-options", UG_MODE_FULLVIEW, NULL, cbs);
		FREE(cbs);
#endif
	}
}

void setting_security_main_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	/*ret_if(NULL == data); */
	/*SettingSecurityUG *ad = (SettingSecurityUG *) data; */
}
