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

#include <setting-security.h>
#include <setting-cfg.h>

#include <efl_extension.h>
#include <app_manager.h>
#if SUPPORT_ENCRYPTION
#include <ode.h>
#endif
#include <sys/wait.h>
#include <bundle_internal.h>


#define Max_Passwd_View_Type_Len	64
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

enum {
    RESULT_OK,
    RESULT_FAILED,
    RESULT_CANCELED,
    RESULT_NETWORK_ERROR
};

void setting_security_ug_popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (ad->save_popup) {
		evas_object_del(ad->save_popup);
		ad->save_popup = NULL;
	}
	ug_destroy_me(ad->ug);
}

int pwd_handler_do_nothing(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	/*SettingSecurityUG *ad = (SettingSecurityUG *)data; */
	/* DO NOTHING */
	return 0;
}

int pwd_handler_sec_pw_passwd(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
#if SUPPORT_ENCRYPTION
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (setting_view_security_encryption.is_create == TRUE && ad->pw_type == SETTING_SEC_PW_ENCRYPTION) {
		if (setting_view_get_topview(&setting_view_security_confirm_encryption) != &setting_view_security_encryption) {
			SETTING_TRACE("view stack is missed. initialize again.");
			setting_view_node_table_intialize();
			/*SETTING_TRACE("viewtype 0x%x", ad->viewtype); */
			if (ad->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
				setting_view_node_table_register(&setting_view_security_encryption, NULL);
				setting_view_node_table_register(&setting_view_security_confirm_encryption, &setting_view_security_encryption);
			} else {
				setting_view_node_table_register(&setting_view_security_main, NULL);
				setting_view_node_table_register(&setting_view_security_sim_settings, &setting_view_security_main);
				setting_view_node_table_register(&setting_view_security_update, &setting_view_security_main);
				setting_view_node_table_register(&setting_view_security_encryption, &setting_view_security_main);
				setting_view_node_table_register(&setting_view_security_confirm_encryption, &setting_view_security_encryption);
			}
		}
		setting_view_change(&setting_view_security_encryption, &setting_view_security_confirm_encryption, ad);
		return 0;
	}
#endif

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, SETTING_SCREEN_LOCK_TYPE_PASSWORD) == 0) {
		/* Success to save */
	} else {
		SETTING_TRACE_ERROR("setting vconf value failed : screen_lock_type");
	}

	return 0;
}

static void
__face_voice_done_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	retm_if(data == NULL, "[Setting > Security] Data parameter is NULL");
	SettingSecurityUG *ad = (SettingSecurityUG *) data;

	/*int i = 0; */
	while (elm_naviframe_top_item_get(ad->navi_bar) != ad->screen_lock_main_item) {
		elm_naviframe_item_pop(ad->navi_bar);
		/*SETTING_TRACE("i:%d",i); */
		/*i++; */
		/*sleep(2); */
		if (elm_naviframe_top_item_get(ad->navi_bar) == elm_naviframe_bottom_item_get(ad->navi_bar)) {
			break;
		}
	}
}

int pwd_handler_sec_pw_simple_passwd(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	SETTING_TRACE("ad->selected_lock_type:%s", ad->selected_lock_type);
	/*'face and voice ' and 'simple password' share the same PASSWD_TYPE and callback function */
	int lock_type = SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD;
	if (!safeStrCmp(ad->selected_lock_type, Keystr_FaceAndVoice)) {
		lock_type = SETTING_SCREEN_LOCK_TYPE_FACE_AND_VOICE;
		setting_create_guild_layout(ad->navi_bar, Setup_Face_Unlock_Str,
		                            _("IDS_SA_BUTTON_DONE_ABB"), NULL, NULL,
		                            __face_voice_done_cb, NULL, NULL,
		                            _(Finish_Setup_Face),
		                            NULL, NULL, NULL, NULL, ad);
	}

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type) == 0) {
		/* Success to save */
	} else {
		SETTING_TRACE_ERROR("setting vconf value failed : screen_lock_type");
	}
	return 0;
}

int pwd_handler_sec_pw_sim_lock_on(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_PS, setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}


int pwd_handler_sec_pw_sim_lock_off(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_PS, setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}


int pwd_handler_sec_pw_pin_lock_on(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, -1);
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_SC, setting_security_pin_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}


int pwd_handler_sec_pw_pin_lock_off(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_SC, setting_security_pin_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}


int pwd_handler_sec_pw_sim_lock_disabled(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	/* [in] Evas_Object *checkbtn */
	if (ad->data_sim_lk)
		setting_disable_genlist_item(ad->data_sim_lk->item);

	return 0;
}


int pwd_handler_sec_pw_change_pin1(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	setting_create_popup_without_btn(ad, ad->ly_main, NULL, _("IDS_ST_POP_PIN_CHANGED"),
	                                 NULL,
	                                 POPUP_INTERVAL, FALSE, FALSE);
	return 0;
}

#if SUPPORT_FDN
int pwd_handler_sec_pw_fdn_mode_on(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD, setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}


int pwd_handler_sec_pw_fdn_mode_off(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD, setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}
#endif

int pwd_handler_sec_pw_change_pin2(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	return 0;
}

static void setting_security_pin1_blocked_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingSecurityUG *ad = data;

	int ret = 0;
	/* Call pwlock. */
	bundle *b = bundle_create();
	if (b) {
		bundle_add(b, "after_bootup", "1");
		ret = aul_launch_app("org.tizen.pwlock", b);
		SETTING_TRACE("aul_launch_app() returns %d", ret);
		bundle_free(b);
	}

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
}

int pwd_handler_sec_pw_pin1_blocked(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}

	/* To do : draw popup, */
	/*         content - PIN Blocked\n The PIN is now blocked. Enter your PUK code to continue. */
	/*		   button - OK */
	char popup_text[2048] = {0,};
	snprintf(popup_text, 2048, "%s", _(PIN_BLOCKED_ENTER_PUK_DESC));

	int ret = 0;

#if 0
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;

	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get(ad->win_get);

	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(disp, xwin, UTILX_NOTIFICATION_LEVEL_LOW);

	ret = utilx_grab_key(disp, xwin, KEY_BACK, EXCLUSIVE_GRAB);
	if (ret)
		SETTING_TRACE("utilx_grab_key() failed.");
	ret = utilx_grab_key(disp, xwin, KEY_HOME, EXCLUSIVE_GRAB);
	if (ret)
		SETTING_TRACE("utilx_grab_key() failed.");
	ret = utilx_grab_key(disp, xwin, KEY_VOLUMEUP, TOP_POSITION_GRAB);
	if (ret)
		SETTING_TRACE("utilx_grab_key() failed.");
	ret = utilx_grab_key(disp, xwin, KEY_VOLUMEDOWN, TOP_POSITION_GRAB);
	if (ret)
		SETTING_TRACE("utilx_grab_key() failed.");
	unsigned int val[3];
	val[0] = 0; 	/* always enable F */
	val[1] = 0;		/* quickpanel enable F */
	val[2] = 0; 	/* apptray enable F */
	/* set quickpanel disable */
	Ecore_X_Atom ATOM_PANEL_SCROLLABLE_STATE = 0;
	ATOM_PANEL_SCROLLABLE_STATE = ecore_x_atom_get("_E_MOVE_PANEL_SCROLLABLE_STATE");
	ecore_x_window_prop_card32_set(xwin, ATOM_PANEL_SCROLLABLE_STATE, val, 3);
#endif

	ad->sim_popup = setting_create_popup_with_btn(ad, ad->win_get, NULL,
	                                              popup_text,
	                                              setting_security_pin1_blocked_resp_cb,
	                                              0, 1,
	                                              "IDS_ST_BUTTON_OK");
	eext_object_event_callback_del(ad->sim_popup, EEXT_CALLBACK_BACK, eext_popup_back_cb_2);

	/* End. */
	return 0;
}

int pwd_handler_sec_pw_pin2_blocked(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
#if SUPPORT_FDN
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD, setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
#endif

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
	char popup_text[2048] = {0,};
	snprintf(popup_text, 2048, "%s", _("IDS_ST_POP_PIN2_BLOCKED"));

	setting_create_popup_without_btn(ad, ad->win_get, NULL,
	                                 popup_text,
	                                 NULL,
	                                 2, FALSE, FALSE);

	ad->pin2_blocked_flag = TRUE;
	setting_view_update(&setting_view_security_sim_settings, ad);

	return 0;
}

int pwd_handler_sec_pw_pin2_unblocked(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;

	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	ad->pin2_blocked_flag = FALSE;
	setting_view_update(&setting_view_security_sim_settings, ad);

	return 0;
}

int pwd_handler_sec_pw_puk1_blocked(SettingSecurityUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (setting_view_security_sim_settings.is_create == TRUE) {
		setting_view_change(&setting_view_security_sim_settings, &setting_view_security_main, ad);
	} else if (setting_view_security_main.is_create == TRUE) {
		setting_view_update(&setting_view_security_main, ad);
	}

	return 0;
}

static struct _security_item security_table[] = {
	{ SETTING_SEC_PW_PASSWORD, 		"SETTING_PW_TYPE_PASSWORD", 			pwd_handler_sec_pw_passwd },
	{ SETTING_SEC_PW_SIMPLE_PASSWD, 	"SETTING_PW_TYPE_SIMPLE_PASSWORD", 		pwd_handler_sec_pw_simple_passwd },
	{ SETTING_SEC_PW_ENTER_LOCK_TYPE, "SETTING_PW_TYPE_ENTER_LOCK_TYPE", 		pwd_handler_do_nothing},
	{ SETTING_SEC_PW_ENCRYPTION, "SETTING_PW_TYPE_ENCRYPTION", 		pwd_handler_sec_pw_passwd},
	{ SETTING_SEC_PW_SIM_LOCK_ON, 		"SETTING_PW_TYPE_SIM_LOCK_ON", 			pwd_handler_sec_pw_sim_lock_on },
	{ SETTING_SEC_PW_SIM_LOCK_OFF, 		"SETTING_PW_TYPE_SIM_LOCK_OFF",			pwd_handler_sec_pw_sim_lock_off },
	{ SETTING_SEC_PW_SIM_LOCK_DISABLED, "SETTING_PW_TYPE_SIM_LOCK_DISABLE",		pwd_handler_sec_pw_sim_lock_disabled},
	{ SETTING_SEC_PW_PIN_LOCK_ON, 		"SETTING_PW_TYPE_PIN_LOCK_ON",			pwd_handler_sec_pw_pin_lock_on },
	{ SETTING_SEC_PW_PIN_LOCK_OFF, 		"SETTING_PW_TYPE_PIN_LOCK_OFF",			pwd_handler_sec_pw_pin_lock_off },
	{ SETTING_SEC_PW_CHANGE_PIN1, 		"SETTING_PW_TYPE_CHANGE_PIN",			pwd_handler_sec_pw_change_pin1},
#if SUPPORT_FDN
	{ SETTING_SEC_PW_FDN_MODE_ON, 		"SETTING_PW_TYPE_FDN_MODE_ON",			pwd_handler_sec_pw_fdn_mode_on},
	{ SETTING_SEC_PW_FDN_MODE_OFF, 		"SETTING_PW_TYPE_FDN_MODE_OFF",			pwd_handler_sec_pw_fdn_mode_off},
#endif
	{ SETTING_SEC_PW_CHANGE_PIN2, 		"SETTING_PW_TYPE_CHANGE_PIN2",			pwd_handler_sec_pw_change_pin2},
	{ SETTING_SEC_PW_PIN1_BLOCKED, 		"SETTING_PW_TYPE_PIN_BLOCKED",			pwd_handler_sec_pw_pin1_blocked},
	{ SETTING_SEC_PW_PIN2_BLOCKED, 		"SETTING_PW_TYPE_PIN2_BLOCKED",			pwd_handler_sec_pw_pin2_blocked},
	{ SETTING_SEC_PW_PIN1_UNBLOCKED, 		"SETTING_PW_TYPE_PIN1_UNBLOCKED",		pwd_handler_sec_pw_pin_lock_on},
	{ SETTING_SEC_PW_PIN2_UNBLOCKED, 		"SETTING_PW_TYPE_PIN2_UNBLOCKED",		pwd_handler_sec_pw_pin2_unblocked},
	{ SETTING_SEC_PW_PUK1_BLOCKED, 		"SETTING_PW_TYPE_PUK1_BLOCKED",		pwd_handler_sec_pw_puk1_blocked},
	{ SETTING_SEC_PW_PUK2_BLOCKED, 		"SETTING_PW_TYPE_PUK2_BLOCKED",		pwd_handler_do_nothing},
	/*------------------------------------------------------------------------------------------------------------*/
	{ SETTING_SEC_PW_MAX, 				NULL, 									pwd_handler_do_nothing}
};

static void setting_security_ug_cb_resize(void *data, Evas *e,
                                          Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;	/* ad is point to data */
	if (ad->viewtype == SETTING_SEC_VIEWTYPE_MAIN)
		setting_view_update(&setting_view_security_main, ad);
}

setting_view *__get_security_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	char *viewtype = NULL;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	/*if(!viewtype) */
	/*	return NULL; */
	if (viewtype) {
		if (!safeStrCmp(viewtype, "encryption"))
			ad->viewtype = SETTING_SEC_VIEWTYPE_ENCRYPTION;
		else if (!safeStrCmp(viewtype, "update"))
			ad->viewtype = SETTING_SEC_VIEWTYPE_UPDATE;
		else
			ad->viewtype = SETTING_SEC_VIEWTYPE_MAIN;
	} else {
		ad->viewtype = SETTING_SEC_VIEWTYPE_MAIN;
	}
	FREE(viewtype);

	if (ad->viewtype == SETTING_SEC_VIEWTYPE_MAIN) {
		setting_view_node_table_register(&setting_view_security_main, NULL);
		setting_view_node_table_register(&setting_view_security_sim_settings, &setting_view_security_main);
		setting_view_node_table_register(&setting_view_security_update, &setting_view_security_main);
#if SUPPORT_ENCRYPTION
		setting_view_node_table_register(&setting_view_security_encryption, &setting_view_security_main);
		setting_view_node_table_register(&setting_view_security_confirm_encryption, &setting_view_security_encryption);
#endif
		return &setting_view_security_main;
	} else if (ad->viewtype == SETTING_SEC_VIEWTYPE_UPDATE) {
		setting_view_node_table_register(&setting_view_security_update, NULL);
		return &setting_view_security_update;
	}
#if SUPPORT_ENCRYPTION
	else if (ad->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
		setting_view_node_table_register(&setting_view_security_encryption, NULL);
		setting_view_node_table_register(&setting_view_security_confirm_encryption, &setting_view_security_encryption);
		/*setting_view_node_table_register(&setting_view_security_enc_processing, &setting_view_security_confirm_encryption); */
		return &setting_view_security_encryption;
	}
#endif
	else {
		/* unknown viewtype */
		return NULL;
	}
}

static void *setting_security_ug_on_create(ui_gadget_h ug,
                                           enum ug_mode mode, app_control_h service,
                                           void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingSecurityUG *securityUG = priv;
	securityUG->ug = ug;

	securityUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	securityUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(securityUG->win_main_layout);
	securityUG->evas = evas_object_evas_get(securityUG->win_main_layout);

	setting_retvm_if(securityUG->win_main_layout == NULL, NULL,
	                 "[Setting >> About] cannot get main window ");

	/*setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR); */
	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc("1line", &(securityUG->itc_1text));
	setting_create_Gendial_itc("multiline_sub", &(securityUG->itc_bg_1icon));
	setting_create_Gendial_itc("2line.top", &(securityUG->itc_2text_2));
	setting_create_Gendial_itc("1line", &(securityUG->itc_1text_1icon));
	setting_create_Gendial_itc("groupindex", &(securityUG->itc_group_item));
	setting_create_Gendial_itc("2line.top", &(securityUG->itc_2text_3_parent));

	securityUG->update_view_timer = NULL;
	securityUG->remove_sim_popup_timer = NULL;
	securityUG->tapi_async_cb_check_timer = NULL;
	securityUG->input_pwd = NULL;
	/*securityUG->cur_enc_mode = 0; */
	/*  creating a view. */
	securityUG->handle =  tel_init(NULL);
	if (!securityUG->handle) {
		SETTING_TRACE_ERROR("*** [ERR] tel_init. ***");
		/*return NULL; */
	}
	securityUG->view_to_load = __get_security_view_to_load(securityUG, service);
	setting_retvm_if(NULL == securityUG->view_to_load, NULL,
	                 "NULL == securityUG->view_to_load");

	app_control_get_extra_data(service, "current", &(securityUG->input_pwd));

	setting_view_node_set_cur_view(securityUG->view_to_load);
	setting_view_create(securityUG->view_to_load, (void *)securityUG);
	evas_object_event_callback_add(securityUG->win_main_layout,
	                               EVAS_CALLBACK_RESIZE,
	                               setting_security_ug_cb_resize, securityUG);

	return securityUG->ly_main;
}

static void setting_security_ug_on_start(ui_gadget_h ug, app_control_h service,
                                         void *priv)
{
}

static void setting_security_ug_on_pause(ui_gadget_h ug, app_control_h service,
                                         void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingSecurityUG *securityUG = priv;

	if (setting_view_security_sim_settings.is_create == TRUE) {
		bool is_running = FALSE;
		app_manager_is_running("org.tizen.pwlock", &is_running);
		if (is_running) {
			SETTING_TRACE_DEBUG("pwlock is running");

#if 0
			Ecore_X_Window xwin = 0;
			Ecore_X_Display *disp = NULL;
			disp = ecore_x_display_get();
			xwin = elm_win_xwindow_get(securityUG->win_get);

			ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NORMAL);
			int ret;
			ret = utilx_ungrab_key(disp, xwin, KEY_BACK);
			if (ret)
				SETTING_TRACE("utilx_ungrab_key() failed.");
			ret = utilx_ungrab_key(disp, xwin, KEY_HOME);
			if (ret)
				SETTING_TRACE("utilx_ungrab_key() failed.");
			ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEUP);
			if (ret)
				SETTING_TRACE("utilx_ungrab_key() failed.");
			ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEDOWN);
			if (ret)
				SETTING_TRACE("utilx_ungrab_key() failed.");
			unsigned int val[3];
			val[0] = 1; 	/* always enable */
			val[1] = 1;		/* quickpanel enable */
			val[2] = 1; 	/* apptray enable */
			/* set quickpanel enable */
			Ecore_X_Atom ATOM_PANEL_SCROLLABLE_STATE = 0;
			ATOM_PANEL_SCROLLABLE_STATE = ecore_x_atom_get("_E_MOVE_PANEL_SCROLLABLE_STATE");
			ecore_x_window_prop_card32_set(xwin, ATOM_PANEL_SCROLLABLE_STATE, val, 3);
#endif
		}
	}
}

static void setting_security_ug_on_resume(ui_gadget_h ug, app_control_h service,
                                          void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingSecurityUG *securityUG = priv;

	if (setting_view_security_sim_settings.is_create == TRUE) {
		if (tel_get_sim_facility(securityUG->handle, TAPI_SIM_LOCK_SC, setting_security_pin_get_facility_cb, securityUG) != TAPI_API_SUCCESS) {
			SETTING_TRACE_ERROR("tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
		}
	}
}

static void setting_security_ug_on_destroy(ui_gadget_h ug, app_control_h service,
                                           void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingSecurityUG *securityUG = priv;
	securityUG->ug = ug;

	evas_object_event_callback_del(securityUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_security_ug_cb_resize);	/* fix flash issue for gallery */

	if (securityUG->handle && tel_deinit(securityUG->handle) != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] setting_network_unsubscribe_tapi_events. ***%s",
		 SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	FREE(securityUG->input_pwd);
#if SUPPORT_ENCRYPTION
	if (securityUG->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
		setting_view_destroy(&setting_view_security_confirm_encryption, securityUG);
		setting_view_destroy(&setting_view_security_encryption, securityUG);
	} else {
#endif
		setting_view_destroy(&setting_view_security_sim_settings, securityUG);
		setting_view_destroy(&setting_view_security_update, securityUG);
		setting_view_destroy(&setting_view_security_main, securityUG);
#if SUPPORT_ENCRYPTION
		setting_view_destroy(&setting_view_security_confirm_encryption, securityUG);
		setting_view_destroy(&setting_view_security_encryption, securityUG);
	}
#endif

	if (NULL != ug_get_layout(securityUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(securityUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(securityUG->ug));
	}
}

static void setting_security_ug_on_message(ui_gadget_h ug, app_control_h msg,
                                           app_control_h service, void *priv)
{

}

static void setting_security_ug_on_event(ui_gadget_h ug,
                                         enum ug_event event, app_control_h service,
                                         void *priv)
{
	if (!priv)
		return;
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;

	switch (event) {
		case UG_EVENT_LOW_MEMORY:
			break;
		case UG_EVENT_LOW_BATTERY:
			break;
		case UG_EVENT_LANG_CHANGE:
			if (ad->navi_bar) {
				setting_navi_items_update(ad->navi_bar);
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

static void setting_security_ug_on_key_event(ui_gadget_h ug,
                                             enum ug_key_event event,
                                             app_control_h service, void *priv)
{
	if (!priv)
		return;
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;	/* ad is point to priv */
	switch (event) {
		case UG_KEY_EVENT_END: {
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
	SettingSecurityUG *securityUG = calloc(1, sizeof(SettingSecurityUG));
	setting_retvm_if(!securityUG, -1,
	                 "Create SettingSecurityUG obj failed");

	memset(securityUG, 0x00, sizeof(SettingSecurityUG));

	ops->create = setting_security_ug_on_create;
	ops->start = setting_security_ug_on_start;
	ops->pause = setting_security_ug_on_pause;
	ops->resume = setting_security_ug_on_resume;
	ops->destroy = setting_security_ug_on_destroy;
	ops->message = setting_security_ug_on_message;
	ops->event = setting_security_ug_on_event;
	ops->key_event = setting_security_ug_on_key_event;
	ops->priv = securityUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	struct SettingSecurityUG *securityUG;
	setting_retm_if(!ops, "ops == NULL");

	securityUG = ops->priv;
	if (securityUG)
		FREE(securityUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
void setting_security_sim_get_facility_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityInfo_t *fi = data;
	SettingSecurityUG *ad = user_data;
	ad->enter_tapi_async_cb_flag++;

	SETTING_TRACE("sec_rt[%d]", sec_rt);
	SETTING_TRACE("fi->type[%d]", fi->type);
	SETTING_TRACE("fi->f_status[%d]", fi->f_status);
	if (sec_rt == TAPI_SIM_PIN_OPERATION_SUCCESS) {
		SETTING_TRACE_DEBUG("PIN_OPERATION_SUCCESS");
		switch (fi->type) {
			case TAPI_SIM_LOCK_PS:/*sim status */
				ad->sim_status = fi->f_status;
				break;
			default:
				break;
		}
	} else {
		/*if failed, do nothing.. */
	}

	/* Update SIM Settings view if created. */
	if (setting_view_security_sim_settings.is_create == 1) {
		SETTING_TRACE_DEBUG("SIM settings view is already created. [%d]", setting_view_security_sim_settings.is_create);
		setting_view_update(&setting_view_security_sim_settings, ad);
	} else {
		SETTING_TRACE_DEBUG("Change view");
		setting_view_change(&setting_view_security_main, &setting_view_security_sim_settings, ad);
	}

	SETTING_TRACE_END;
}

void setting_security_pin2_get_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	SETTING_TRACE_DEBUG("sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]", sec_rt, lock->lock_type, lock->lock_status, lock->retry_count);
	if (lock->lock_type == TAPI_SIM_LOCK_FD
	    && lock->lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		ad->pin2_blocked_flag = TRUE;
	}
}

void setting_security_pin_get_facility_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityInfo_t *fi = data;
	SettingSecurityUG *ad = user_data;

	SETTING_TRACE("sec_rt[%d]", sec_rt);
	SETTING_TRACE("fi->type[%d]", fi->type);
	SETTING_TRACE("fi->f_status[%d]", fi->f_status);
	if (sec_rt == TAPI_SIM_PIN_OPERATION_SUCCESS) {
		SETTING_TRACE_DEBUG("PIN_OPERATION_SUCCESS");
		switch (fi->type) {
			case TAPI_SIM_LOCK_SC:/*pin1 status */
				ad->pin1_status = fi->f_status;
				break;
			case TAPI_SIM_LOCK_FD:/*pin2/fdn status */
				ad->pin2_status = fi->f_status;
				break;
			default:
				break;
		}
	} else {
		/*if failed, do nothing.. */
	}

	if (ad->tapi_async_cb_check_timer) {
		ecore_timer_del(ad->tapi_async_cb_check_timer);
		ad->tapi_async_cb_check_timer = NULL;
	}
	ad->enter_tapi_async_cb_flag = 1;

	/* Update SIM Settings view if created. */
	if (setting_view_security_sim_settings.is_create == 1) {
		SETTING_TRACE_DEBUG("SIM settings view is already created. [%d]", setting_view_security_sim_settings.is_create);
		setting_view_update(&setting_view_security_sim_settings, ad);
	} else {
		SETTING_TRACE_DEBUG("Change view");
		setting_view_change(&setting_view_security_main, &setting_view_security_sim_settings, ad);
	}

	SETTING_TRACE_END;
}

void setting_security_layout_passwd_ug_cb(ui_gadget_h ug,
                                          enum ug_mode mode, void *priv)
{
	if (!priv)
		return;

	Evas_Object *base = ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_show(base);
			break;
		default:
			break;
	}

	return;
}

void setting_security_destroy_password_ug_cb(ui_gadget_h ug,
                                             void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);

	if (ug) {
		setting_ug_destroy(ug);
	}

	SETTING_TRACE_DEBUG("[TEST] current : %s", ad->input_pwd);
}

void setting_security_end_password_ug_cb(ui_gadget_h ug,
                                         void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;
	ad->ug_passwd = NULL;

	if (setting_view_security_sim_settings.is_create == TRUE && ad->pw_type == SETTING_SEC_PW_PIN1_UNBLOCKED) {
		SETTING_TRACE("ungrab keys");
#if 0
		/* ungrab keys. */
		Ecore_X_Window xwin = 0;
		Ecore_X_Display *disp = NULL;

		int ret = 0;

		disp = ecore_x_display_get();
		xwin = elm_win_xwindow_get((Evas_Object *)ug_get_window());

		ret = utilx_ungrab_key(disp, xwin, KEY_BACK);
		if (ret)
			SETTING_TRACE("KEY_BACK ungrab failed.");
		ret = utilx_ungrab_key(disp, xwin, KEY_SELECT);
		if (ret)
			SETTING_TRACE("KEY_SELECT ungrab failed.");
		ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEUP);
		if (ret)
			SETTING_TRACE("KEY_VOLUMEUP ungrab failed.");
		ret = utilx_ungrab_key(disp, xwin, KEY_VOLUMEDOWN);
		if (ret)
			SETTING_TRACE("KEY_VOLUMEDOWN ungrab failed.");
#endif
	}
#if SUPPORT_ENCRYPTION
	if (setting_view_security_confirm_encryption.is_create == TRUE) {
		SETTING_TRACE_DEBUG("[TEST] current : %s", ad->input_pwd);
		setting_view_update(&setting_view_security_confirm_encryption, ad);
	} else if (setting_view_security_encryption.is_create == TRUE) {
		SETTING_TRACE_DEBUG("[TEST] current : %s", ad->input_pwd);
		setting_view_update(&setting_view_security_encryption, ad);
	}
#endif
}

gboolean setting_security_create_password_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;	/* ad is point to data */

	/* prevent the ug from being loaded again due to window event queuing */
	/* added by JTS: CQ H0100135346 */
	if (ad->ug_passwd) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}

	if ((ad->pw_type < 0) || (ad->pw_type > SETTING_SEC_PW_MAX))
		return FALSE;

	char str[Max_Passwd_View_Type_Len + 1] = { 0, };
	safeCopyStr(str, security_table[ad->pw_type].pw_type_string, Max_Passwd_View_Type_Len);

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_security_layout_passwd_ug_cb;
	cbs->result_cb = setting_security_result_password_ug_cb;
	cbs->destroy_cb = setting_security_destroy_password_ug_cb;
	cbs->end_cb = setting_security_end_password_ug_cb;
	cbs->priv = (void *)ad;

	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return FALSE;
	}

	app_control_add_extra_data(svc, "viewtype", str);

	SETTING_TRACE_DEBUG("send viewtype to password : %s", str);

	if ((ad->pw_type != SETTING_SEC_PW_ENTER_LOCK_TYPE) && ad->input_pwd) {
		SETTING_TRACE_SECURE_DEBUG("Send current : %s", ad->input_pwd);
		app_control_add_extra_data(svc, "current", ad->input_pwd);
	}

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_removed = FALSE;
	ad->ug_passwd = setting_ug_create(ad->ug, "setting-password-efl", UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_passwd) {	/* error handling */
		evas_object_show(ad->ly_main);
	}

	app_control_destroy(svc);
	FREE(cbs);

	return TRUE;
}

void setting_security_layout_lockscreen_options_ug_cb(ui_gadget_h ug,
                                                      enum ug_mode mode, void *priv)
{
	if (!priv)
		return;

	Evas_Object *base = ug_get_layout(ug);
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

	return;
}

void setting_security_destroy_lockscreen_options_ug_cb(ui_gadget_h ug,
                                                       void *priv)
{
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_lockscreen = NULL;
	}
}

gboolean setting_security_create_lockscreen_options_sg(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;	/* ad is point to data */

	if (ad->ug_lockscreen) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_security_layout_lockscreen_options_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_security_destroy_lockscreen_options_ug_cb;
	cbs->priv = (void *)ad;

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_lockscreen = setting_ug_create(ad->ug, "lockscreen-options", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_lockscreen) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_lockscreen");
		evas_object_show(ad->ly_main);
	}

	FREE(cbs);

	return TRUE;
}
#if SUPPORT_ENCRYPTION
void setting_security_layout_mmc_encryption_ug_cb(ui_gadget_h ug,
                                                  enum ug_mode mode, void *priv)
{
	if (!priv)
		return;

	Evas_Object *base = ug_get_layout(ug);
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

	return;
}

void setting_security_destroy_mmc_encryption_ug_cb(ui_gadget_h ug,
                                                   void *priv)
{
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_mmc_encryption = NULL;
	}

}

gboolean setting_security_create_mmc_encryption_sg(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;	/* ad is point to data */

	if (ad->ug_mmc_encryption) {
		SETTING_TRACE("mmc encryption UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_security_layout_mmc_encryption_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_security_destroy_mmc_encryption_ug_cb;
	cbs->priv = (void *)ad;

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_mmc_encryption = setting_ug_create(ad->ug, "setting-mmc-encryption-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_mmc_encryption) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_mmc_encryption");
		evas_object_show(ad->ly_main);
	}

	FREE(cbs);

	return TRUE;
}
#endif

void setting_security_layout_firewall_ug_cb(ui_gadget_h ug,
                                            enum ug_mode mode, void *priv)
{
	Evas_Object *base = ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
			                                 EVAS_HINT_EXPAND);
			/*elm_win_resize_object_add(ad->win_get, base); */
			evas_object_show(base);
			break;
		default:
			break;
	}

	return;
}

void setting_security_destroy_firewall_ug_cb(ui_gadget_h ug,
                                             void *priv)
{
	ret_if(priv == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) priv;

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_firewall = NULL;
	}
}

gboolean setting_security_create_firewall_sg(void *data)
{
	retv_if(data == NULL, FALSE);

	SettingSecurityUG *ad = (SettingSecurityUG *) data;
	if (ad->ug_firewall) {
		SETTING_TRACE("firewall UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_security_layout_firewall_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_security_destroy_firewall_ug_cb;
	cbs->priv = (void *)ad;

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_firewall = setting_ug_create(ad->ug, "setting-firewall-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_firewall) {
		SETTING_TRACE_ERROR("NULL == ad->ug_firewall");
		evas_object_show(ad->ly_main);
	}

	FREE(cbs);
	return TRUE;
}


/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
/* -1 	on fail */
/* 0~n 	on success */
int _get_security_table_index(char *name)
{
	int i = 0;

	for (i = 0; i < SETTING_SEC_PW_MAX; i++) {
		if (0 == safeStrCmp(name, security_table[i].pw_type_string))
			return i;
	}
	return -1;
}

void
setting_security_result_password_ug_cb(ui_gadget_h ug, app_control_h service,
                                       void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data paremeter is NULL");

	SettingSecurityUG *ad = (SettingSecurityUG *) priv;	/* ad is point to priv */

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);

	char *result = NULL;
	char *current = NULL;

	app_control_get_extra_data(service, "result", &result);
	app_control_get_extra_data(service, "current", &current);

	/* divided pw view type */
	SETTING_TRACE("ad->pw_type: %d result :%s", ad->pw_type, result);
	ret_if(NULL == result);

	if (safeStrCmp(result, "Cancel") == 0) {
		ad->pw_type = -1;
		if (setting_view_security_sim_settings.is_create == 1) {
			setting_view_update(&setting_view_security_sim_settings, ad);
		}
		FREE(result);
		return;
		/* To do : rollback previouse status. */
	} else if (safeStrCmp(result, "SETTING_PW_TYPE_ENTER_LOCK_TYPE") == 0) {
		/*ug_destroy(ug); */
		/*ad->ug_passwd = NULL; */
		/*setting_view_update(&setting_view_security_locktype, ad); */
		ad->pw_type = SETTING_SEC_PW_ENTER_LOCK_TYPE;
	} else if (safeStrCmp(result, "SETTING_PW_TYPE_ENTER_LOCK_TYPE:Fail") == 0) {
		ad->pw_type = -1;
		ug_destroy_me(ad->ug);
		FREE(result);
		if (current)
			FREE(current);
		return;
	} else {
		int index =  _get_security_table_index(result);

		if (index != -1) {
			security_table[index].passwd_handler(ad, NULL);
			ad->pw_type = security_table[index].pw_type_num;
		} else {
			SETTING_TRACE_ERROR("no table entry");
		}
		if (ad->input_pwd)
			FREE(ad->input_pwd);
	}

	if (current) {
		SETTING_TRACE_SECURE_DEBUG("current : %s", current);
		ad->input_pwd = (char *)strdup(current);
		FREE(current);
	}

	evas_object_show(ad->ly_main);
	FREE(result);
	SETTING_TRACE_END;
}

#ifdef Status
#undef Status
#endif

static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_JAVA_OPT_SIM_SETTINGS", NULL, "viewtype:frontpage;tab:first;keyword:IDS_JAVA_OPT_SIM_SETTINGS", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_DECRYPT_DEVICE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_DECRYPT_DEVICE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_HEADER_ENCRYPT_DEVICE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_HEADER_ENCRYPT_DEVICE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_DN_BODY_ENCRYPT_SD_CARD", NULL, "viewtype:frontpage;tab:first;keyword:IDS_DN_BODY_ENCRYPT_SD_CARD", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_FIREWALL", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_FIREWALL", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_ANTIVIRUS_SOFTWARE_ABB", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_ANTIVIRUS_SOFTWARE_ABB", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_EMAIL_POP_SECURITY_UPDATE_ABB", NULL, "viewtype:frontpage;tab:first;keyword:IDS_EMAIL_POP_SECURITY_UPDATE_ABB", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
};

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-security-efl DB search code");

	*applocale = strdup("setting:/usr/apps/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, IMG_Security, s_cfg_node_array[i].item_type, s_cfg_node_array[i].data, "Security");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

#if SUPPORT_SECURITY_FIREWALL
int setting_security_system_command(const char *command, const char *commandex)
{
	SETTING_TRACE_BEGIN;
	int pid = 0 , status = 0;
	char *argv[2] = {NULL,};
	/*int ret; */

	if (command == 0 && commandex == 0) {
		/*not exist input */
	}

	pid = fork();

	if (pid > 0) {
		SETTING_TRACE("parent");
		wait(&status);
		return WEXITSTATUS(status);
	}

	if (pid == 0) {
		char *args[100] = {
			argv[0] = (char *)command,
			argv[1] = (char *)commandex,
			NULL,
		};

		SETTING_TRACE_DEBUG("[%s]:%s", argv[0], argv[1]);
		execv("/usr/apps/org.tizen.setting/bin/firewall", args);
	}
	return 1;
}

int setting_security_firewall_reset()
{
	SETTING_TRACE_BEGIN;

	struct dirent *next_file;
	DIR *theFolder;

	char filepath[256];

	theFolder = opendir(SEC_FIREWALL_DIR);
	next_file = readdir(theFolder);
	while (next_file) {
		/* build the full path for each file in the folder */
		sprintf(filepath, "%s/%s", SEC_FIREWALL_DIR, next_file->d_name);
		SETTING_TRACE_DEBUG("[%s]", filepath);
		/*(void) remove(filepath); */
		if (remove(filepath) != 0) { /*delete the file */
			perror("remove");
		}
		next_file = readdir(theFolder);
	}

	closedir(theFolder);
	return 0;
}
#endif

