/*
 setting
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

SettingSecurity *g_ad; /* for tapi callback */
COMMAND *g_ipHead, *g_ipTail;
COMMAND *g_urlHead, *g_urlTail;
COMMAND *g_portHead, *g_portTail;


#define SETTING_SECURITY_PACKAGE_NAME "org.tizen.setting-security"
#define SETTING_SECURITY_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-security/res/locale"
#define SETTING_SECURITY_EDJEDIR _TZ_SYS_RO_APP"/org.tizen.setting-security/res/edje"

enum {
	RESULT_OK, RESULT_FAILED, RESULT_CANCELED, RESULT_NETWORK_ERROR
};

int pwd_handler_do_nothing(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	/*SettingSecurity *ad = (SettingSecurity *)data; */
	/* DO NOTHING */
	return 0;
}

int pwd_handler_sec_pw_passwd(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
#if SUPPORT_ENCRYPTION
	SettingSecurity *ad = (SettingSecurity *)data;

	if (setting_view_security_encryption.is_create == TRUE
			&& ad->pw_type == SETTING_SEC_PW_ENCRYPTION) {
		if (setting_view_get_topview(&setting_view_security_confirm_encryption)
				!= &setting_view_security_encryption) {
			SETTING_TRACE("view stack is missed. initialize again.");
			setting_view_node_table_intialize();
			/*SETTING_TRACE("viewtype 0x%x", ad->viewtype); */
			if (ad->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
				setting_view_node_table_register(
						&setting_view_security_encryption,
						NULL);
				setting_view_node_table_register(
						&setting_view_security_confirm_encryption,
						&setting_view_security_encryption);
			} else {
				setting_view_node_table_register(
						&setting_view_security_main,
						NULL);
				setting_view_node_table_register(
						&setting_view_security_sim_settings,
						&setting_view_security_main);
				setting_view_node_table_register(
						&setting_view_security_update,
						&setting_view_security_main);
				setting_view_node_table_register(
						&setting_view_security_encryption,
						&setting_view_security_main);
				setting_view_node_table_register(
						&setting_view_security_confirm_encryption,
						&setting_view_security_encryption);
			}
		}
		setting_view_change(&setting_view_security_encryption,
				&setting_view_security_confirm_encryption, ad);
		return 0;
	}
#endif

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
			SETTING_SCREEN_LOCK_TYPE_PASSWORD) == 0) {
		/* Success to save */
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}

	return 0;
}

static void __face_voice_done_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	retm_if(data == NULL, "[Setting > Security] Data parameter is NULL");
	SettingSecurity *ad = (SettingSecurity *)data;

	/*int i = 0; */
	while (elm_naviframe_top_item_get(ad->md.navibar_main)
			!= ad->screen_lock_main_item) {
		elm_naviframe_item_pop(ad->md.navibar_main);
		/*SETTING_TRACE("i:%d",i); */
		/*i++; */
		/*sleep(2); */
		if (elm_naviframe_top_item_get(ad->md.navibar_main)
				== elm_naviframe_bottom_item_get(ad->md.navibar_main)) {
			break;
		}
	}
}

int pwd_handler_sec_pw_simple_passwd(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;

	SETTING_TRACE("ad->selected_lock_type:%s", ad->selected_lock_type);
	/*'face and voice ' and 'simple password' share the same PASSWD_TYPE and
	 * callback function */
	int lock_type = SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD;
	if (!safeStrCmp(ad->selected_lock_type, Keystr_FaceAndVoice)) {
		lock_type = SETTING_SCREEN_LOCK_TYPE_FACE_AND_VOICE;
		setting_create_guild_layout(ad->md.navibar_main, Setup_Face_Unlock_Str,
				_("IDS_SA_BUTTON_DONE_ABB"), NULL, NULL,
				__face_voice_done_cb, NULL, NULL,
				_(Finish_Setup_Face),
				NULL, NULL, NULL, NULL, ad);
	}

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type) == 0) {
		/* Success to save */
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}
	return 0;
}

int pwd_handler_sec_pw_sim_lock_on(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_PS,
			setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}

int pwd_handler_sec_pw_sim_lock_off(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_PS,
			setting_security_sim_get_facility_cb, ad) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}

int pwd_handler_sec_pw_pin_lock_on(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, -1);
	SettingSecurity *ad = (SettingSecurity *)data;

	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_SC,
			setting_security_pin_get_facility_cb, ad)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}

int pwd_handler_sec_pw_pin_lock_off(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_SC,
			setting_security_pin_get_facility_cb, ad)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}

int pwd_handler_sec_pw_sim_lock_disabled(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	/* [in] Evas_Object *checkbtn */
	if (ad->data_sim_lk)
		setting_disable_genlist_item(ad->data_sim_lk->item);

	return 0;
}

int pwd_handler_sec_pw_change_pin1(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;

	setting_create_popup(ad, ad->md.ly_main, NULL, "IDS_ST_POP_PIN_CHANGED",
	NULL,
	POPUP_INTERVAL, FALSE, FALSE, 0);
	return 0;
}

#if SUPPORT_FDN
int pwd_handler_sec_pw_fdn_mode_on(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD,
					setting_security_sim_get_facility_cb, ad)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}

int pwd_handler_sec_pw_fdn_mode_off(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD,
					setting_security_sim_get_facility_cb, ad)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
	return 0;
}
#endif

int pwd_handler_sec_pw_change_pin2(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	return 0;
}

static void setting_security_pin1_blocked_resp_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingSecurity *ad = data;

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

int pwd_handler_sec_pw_pin1_blocked(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}

	/* To do : draw popup, */
	/*	   content - PIN Blocked\n The PIN is now blocked. Enter your
	 * PUK code to continue. */
	/*	   button - OK */
	char popup_text[2048] = { 0, };
	snprintf(popup_text, 2048, "%s", _(PIN_BLOCKED_ENTER_PUK_DESC));

#ifdef ECORE_X
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;

	int ret = 0;
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get(ad->md.win_main);

	ecore_x_netwm_window_type_set(xwin, ECORE_X_WINDOW_TYPE_NOTIFICATION);
	utilx_set_system_notification_level(disp, xwin,
			UTILX_NOTIFICATION_LEVEL_LOW);

	ret = eext_win_keygrab_set(xwin, "XF86Back");
	if (ret)
	SETTING_TRACE("eext_win_keygrab_set() failed.");
	ret = eext_win_keygrab_set(xwin, "XF86Home");
	if (ret)
	SETTING_TRACE("eext_win_keygrab_set() failed.");
	ret = eext_win_keygrab_set(xwin, "XF86AudioRaiseVolume");
	if (ret)
	SETTING_TRACE("eext_win_keygrab_set() failed.");
	ret = eext_win_keygrab_set(xwin, "XF86AudioLowerVolume");
	if (ret)
	SETTING_TRACE("eext_win_keygrab_set() failed.");
#else
	/* @todo : repace codes using X with codes tizen 3.0 API */
#endif

#ifdef ECORE_X
	unsigned int val[3];
	val[0] = 0; /* always enable F */
	val[1] = 0; /* quickpanel enable F */
	val[2] = 0; /* apptray enable F */
	/* set quickpanel disable */
	Ecore_X_Atom ATOM_PANEL_SCROLLABLE_STATE = 0;
	ATOM_PANEL_SCROLLABLE_STATE = ecore_x_atom_get(
			"_E_MOVE_PANEL_SCROLLABLE_STATE");
	ecore_x_window_prop_card32_set(xwin, ATOM_PANEL_SCROLLABLE_STATE, val,
			3);
#else
	/* @todo : repace codes using X with codes tizen 3.0 API */
#endif

	ad->sim_popup = setting_create_popup(ad, ad->md.win_main, NULL, popup_text,
			setting_security_pin1_blocked_resp_cb, 0, FALSE, FALSE,
			1, "IDS_ST_BUTTON_OK");
	eext_object_event_callback_del(ad->sim_popup, EEXT_CALLBACK_BACK,
			setting_popup_del_cb);

	/* End. */
	return 0;
}

int pwd_handler_sec_pw_pin2_blocked(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;

	SettingSecurity *ad = (SettingSecurity *)data;
#if SUPPORT_FDN
	if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_FD,
					setting_security_sim_get_facility_cb,
					ad)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR(
				"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
	}
#endif

	if (ad->sim_popup) {
		evas_object_del(ad->sim_popup);
		ad->sim_popup = NULL;
	}
	char popup_text[2048] = { 0, };
	snprintf(popup_text, 2048, "%s", _("IDS_ST_POP_PIN2_BLOCKED"));

	setting_create_popup(ad, ad->md.win_main, NULL, popup_text,
	NULL, 2, FALSE, FALSE, 0);

	ad->pin2_blocked_flag = TRUE;
	setting_view_update(&setting_view_security_sim_settings, ad);

	return 0;
}

int pwd_handler_sec_pw_pin2_unblocked(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;

	SettingSecurity *ad = (SettingSecurity *)data;
	ad->pin2_blocked_flag = FALSE;
	setting_view_update(&setting_view_security_sim_settings, ad);

	return 0;
}

int pwd_handler_sec_pw_puk1_blocked(SettingSecurity *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;
	if (setting_view_security_sim_settings.is_create == TRUE) {
		setting_view_change(&setting_view_security_sim_settings,
				&setting_view_security_main, ad);
	} else if (setting_view_security_main.is_create == TRUE) {
		setting_view_update(&setting_view_security_main, ad);
	}

	return 0;
}

static struct _security_item security_table[] = { {
	SETTING_SEC_PW_PASSWORD,
	"SETTING_PW_TYPE_PASSWORD",
	pwd_handler_sec_pw_passwd }, {
	SETTING_SEC_PW_SIMPLE_PASSWD,
	"SETTING_PW_TYPE_SIMPLE_PASSWORD",
	pwd_handler_sec_pw_simple_passwd }, {
	SETTING_SEC_PW_ENTER_LOCK_TYPE,
	"SETTING_PW_TYPE_ENTER_LOCK_TYPE",
	pwd_handler_do_nothing }, {
	SETTING_SEC_PW_ENCRYPTION,
	"SETTING_PW_TYPE_ENCRYPTION",
	pwd_handler_sec_pw_passwd }, {
	SETTING_SEC_PW_SIM_LOCK_ON,
	"SETTING_PW_TYPE_SIM_LOCK_ON",
	pwd_handler_sec_pw_sim_lock_on }, {
	SETTING_SEC_PW_SIM_LOCK_OFF,
	"SETTING_PW_TYPE_SIM_LOCK_OFF",
	pwd_handler_sec_pw_sim_lock_off }, {
	SETTING_SEC_PW_SIM_LOCK_DISABLED,
	"SETTING_PW_TYPE_SIM_LOCK_DISABLE",
	pwd_handler_sec_pw_sim_lock_disabled }, {
	SETTING_SEC_PW_PIN_LOCK_ON,
	"SETTING_PW_TYPE_PIN_LOCK_ON",
	pwd_handler_sec_pw_pin_lock_on }, {
	SETTING_SEC_PW_PIN_LOCK_OFF,
	"SETTING_PW_TYPE_PIN_LOCK_OFF",
	pwd_handler_sec_pw_pin_lock_off }, {
	SETTING_SEC_PW_CHANGE_PIN1,
	"SETTING_PW_TYPE_CHANGE_PIN",
	pwd_handler_sec_pw_change_pin1 },
#if SUPPORT_FDN
	{	SETTING_SEC_PW_FDN_MODE_ON, "SETTING_PW_TYPE_FDN_MODE_ON",
		pwd_handler_sec_pw_fdn_mode_on},
	{	SETTING_SEC_PW_FDN_MODE_OFF, "SETTING_PW_TYPE_FDN_MODE_OFF",
		pwd_handler_sec_pw_fdn_mode_off},
#endif
	{
		SETTING_SEC_PW_CHANGE_PIN2,
		"SETTING_PW_TYPE_CHANGE_PIN2",
		pwd_handler_sec_pw_change_pin2 }, {
		SETTING_SEC_PW_PIN1_BLOCKED,
		"SETTING_PW_TYPE_PIN_BLOCKED",
		pwd_handler_sec_pw_pin1_blocked }, {
		SETTING_SEC_PW_PIN2_BLOCKED,
		"SETTING_PW_TYPE_PIN2_BLOCKED",
		pwd_handler_sec_pw_pin2_blocked }, {
		SETTING_SEC_PW_PIN1_UNBLOCKED,
		"SETTING_PW_TYPE_PIN1_UNBLOCKED",
		pwd_handler_sec_pw_pin_lock_on }, {
		SETTING_SEC_PW_PIN2_UNBLOCKED,
		"SETTING_PW_TYPE_PIN2_UNBLOCKED",
		pwd_handler_sec_pw_pin2_unblocked }, {
		SETTING_SEC_PW_PUK1_BLOCKED,
		"SETTING_PW_TYPE_PUK1_BLOCKED",
		pwd_handler_sec_pw_puk1_blocked }, {
		SETTING_SEC_PW_PUK2_BLOCKED,
		"SETTING_PW_TYPE_PUK2_BLOCKED",
		pwd_handler_do_nothing },
	/*--------------------------------------------------------------------*/
	{ SETTING_SEC_PW_MAX, NULL, pwd_handler_do_nothing } };

static void _cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);

	/* ad is point to data */
	SettingSecurity *ad = (SettingSecurity *)data;
	if (ad->viewtype == SETTING_SEC_VIEWTYPE_MAIN)
		setting_view_update(&setting_view_security_main, ad);
}

setting_view *__get_security_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	SettingSecurity *ad = (SettingSecurity *)data;

	char *viewtype = NULL;
	app_control_get_extra_data(service, "viewtype", &viewtype);

	/*if(!viewtype) */
	/* return NULL; */
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
		setting_view_node_table_register(&setting_view_security_main,
		NULL);
		setting_view_node_table_register(
				&setting_view_security_sim_settings,
				&setting_view_security_main);
		setting_view_node_table_register(&setting_view_security_update,
				&setting_view_security_main);
#if SUPPORT_ENCRYPTION
		setting_view_node_table_register(
				&setting_view_security_encryption,
				&setting_view_security_main);
		setting_view_node_table_register(
				&setting_view_security_confirm_encryption,
				&setting_view_security_encryption);
#endif
		return &setting_view_security_main;
	} else if (ad->viewtype == SETTING_SEC_VIEWTYPE_UPDATE) {
		setting_view_node_table_register(&setting_view_security_update,
		NULL);

		return &setting_view_security_update;
	}
#if SUPPORT_ENCRYPTION
	else if (ad->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
		setting_view_node_table_register(
				&setting_view_security_encryption, NULL);
		setting_view_node_table_register(
				&setting_view_security_confirm_encryption,
				&setting_view_security_encryption);
		/*setting_view_node_table_register(
		 * &setting_view_security_enc_processing,
		 *  &setting_view_security_confirm_encryption); */
		return &setting_view_security_encryption;
	}
#endif
	else {
		/* unknown viewtype */
		return NULL;
	}
}

static bool _setting_security_app_create(void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingSecurity *ad = priv;

	if (app_init(&ad->md, SETTING_SECURITY_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	elm_theme_extension_add(NULL, SETTING_SECURITY_EDJEDIR"/setting-genlist.edj");
	elm_theme_extension_add(NULL, SETTING_SECURITY_EDJEDIR"/setting-theme.edj");
	elm_theme_extension_add(NULL, SETTING_SECURITY_EDJEDIR"/setting.edj");

	bindtextdomain(SETTING_PACKAGE, SETTING_SECURITY_LOCALEDIR);

	ad->current_rotation = elm_win_rotation_get(
			ad->md.win_main);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d",
			ad->current_rotation);
	if (elm_win_wm_rotation_supported_get(ad->md.win_main)) {
		/* rotation value that app may want */
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->md.win_main,
				rots, 4);
	}

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_group_item));
	setting_create_Gendial_itc("multiline_sub",
			&(ad->itc_bg_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_3_parent));

	ad->update_view_timer = NULL;
	ad->remove_sim_popup_timer = NULL;
	ad->tapi_async_cb_check_timer = NULL;
	ad->input_pwd = NULL;
	/*ad->cur_enc_mode = 0; */
	/* creating a view. */
	ad->handle = tel_init(NULL);
	if (!ad->handle) {
		SETTING_TRACE_ERROR("*** [ERR] tel_init. ***");
		/*return NULL; */
	}

	evas_object_event_callback_add(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, _cb_resize,
			ad);

	return true;
}

static void _setting_security_app_pause(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

	if (setting_view_security_sim_settings.is_create == TRUE) {
		bool is_running = FALSE;
		app_manager_is_running("org.tizen.pwlock", &is_running);
		if (is_running) {
			SETTING_TRACE_DEBUG("pwlock is running");
#ifdef ECORE_X
			SettingSecurity *ad = priv;
			Ecore_X_Window xwin = 0;
			Ecore_X_Display *disp = NULL;

			disp = ecore_x_display_get();
			xwin = elm_win_xwindow_get(ad->md.win_main);

			ecore_x_netwm_window_type_set(xwin,
					ECORE_X_WINDOW_TYPE_NORMAL);
			int ret;
			ret = eext_win_keygrab_unset(xwin, "XF86Back");
			if (ret)
			SETTING_TRACE("eext_win_keygrab_unset() failed.");
			ret = eext_win_keygrab_unset(xwin, "XF86Home");
			if (ret)
			SETTING_TRACE("eext_win_keygrab_unset() failed.");
			ret = eext_win_keygrab_unset(xwin,
					"XF86AudioRaiseVolume");
			if (ret)
			SETTING_TRACE("eext_win_keygrab_unset() failed.");
			ret = eext_win_keygrab_unset(xwin,
					"XF86AudioLowerVolume");
			if (ret)
			SETTING_TRACE("eext_win_keygrab_unset() failed.");
#else
			/* @todo : repace codes using X with codes tizen 3.0
			 * API */
#endif

#ifdef ECORE_X
			unsigned int val[3];
			val[0] = 1; /* always enable */
			val[1] = 1; /* quickpanel enable */
			val[2] = 1; /* apptray enable */
			/* set quickpanel enable */
			Ecore_X_Atom ATOM_PANEL_SCROLLABLE_STATE = 0;
			ATOM_PANEL_SCROLLABLE_STATE = ecore_x_atom_get(
					"_E_MOVE_PANEL_SCROLLABLE_STATE");
			ecore_x_window_prop_card32_set(xwin,
					ATOM_PANEL_SCROLLABLE_STATE, val, 3);
#else
			/* @todo : repace codes using X with codes tizen 3.0
			 * API */
#endif
		}
	}
}

static void _setting_security_app_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingSecurity *ad = priv;

	if (setting_view_security_sim_settings.is_create == TRUE) {
		if (tel_get_sim_facility(ad->handle, TAPI_SIM_LOCK_SC,
				setting_security_pin_get_facility_cb,
				ad) != TAPI_API_SUCCESS) {
			SETTING_TRACE_ERROR(
					"tel_get_sim_facility(TAPI_SIM_LOCK_PS) failed");
		}
	}
}

static void _setting_security_app_control(app_control_h service, void *priv)
{
	SettingSecurity *ad = priv;

	if (!ad)
		return;

	ad->view_to_load = __get_security_view_to_load(ad,
							service);

	if (!ad->view_to_load) {
		SETTING_TRACE_ERROR("NULL == ad->view_to_load");
		return;
	}

	app_control_get_extra_data(service, "current",
			&(ad->input_pwd));

	setting_view_node_set_cur_view(ad->view_to_load);
	setting_view_create(ad->view_to_load, ad);
}

static void _setting_security_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingSecurity *ad = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, _cb_resize);

	if (ad->handle && tel_deinit(ad->handle)
			!= TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG(
				"%s*** [ERR] setting_network_unsubscribe_tapi_events. ***%s",
				SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	FREE(ad->input_pwd);
#if SUPPORT_ENCRYPTION
	if (ad->viewtype == SETTING_SEC_VIEWTYPE_ENCRYPTION) {
		setting_view_destroy(&setting_view_security_confirm_encryption,
				ad);
		setting_view_destroy(&setting_view_security_encryption,
				ad);
	} else {
#endif
	setting_view_destroy(&setting_view_security_sim_settings, ad);
	setting_view_destroy(&setting_view_security_update, ad);
	setting_view_destroy(&setting_view_security_main, ad);
#if SUPPORT_ENCRYPTION
	setting_view_destroy(&setting_view_security_confirm_encryption,
			ad);
	setting_view_destroy(&setting_view_security_encryption, ad);
}
#endif

	if (ad->md.win_main) {
	    evas_object_del(ad->md.win_main);
	    ad->md.win_main = NULL;
	}
}

static void _lang_changed(app_event_info_h event_info, void *priv)
{
	char *lang = NULL;

	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = _setting_security_app_create,
		.pause = _setting_security_app_pause,
		.resume = _setting_security_app_resume,
		.terminate = _setting_security_app_terminate,
		.app_control = _setting_security_app_control
	};

	SettingSecurity app_data;

	memset(&app_data, 0, sizeof(app_data));

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	return ui_app_main(argc, argv, &ops, &app_data);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
void setting_security_sim_get_facility_cb(TapiHandle *handle, int result,
		void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityInfo_t *fi = data;
	SettingSecurity *ad = user_data;
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
		SETTING_TRACE_DEBUG(
				"SIM settings view is already created. [%d]",
				setting_view_security_sim_settings.is_create);
		setting_view_update(&setting_view_security_sim_settings, ad);
	} else {
		SETTING_TRACE_DEBUG("Change view");
		setting_view_change(&setting_view_security_main,
				&setting_view_security_sim_settings, ad);
	}

	SETTING_TRACE_END;
}

void setting_security_pin2_get_lock_info_cb(TapiHandle *handle, int result,
		void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingSecurity *ad = (SettingSecurity *)user_data;

	SETTING_TRACE_DEBUG(
			"sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]",
			sec_rt, lock->lock_type, lock->lock_status,
			lock->retry_count);
	if (lock->lock_type == TAPI_SIM_LOCK_FD
			&& lock->lock_status == TAPI_SIM_LOCK_KEY_PUK2) {
		ad->pin2_blocked_flag = TRUE;
	}
}

void setting_security_pin_get_facility_cb(TapiHandle *handle, int result,
		void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	ret_if(!user_data);
	SETTING_TRACE_DEBUG("user_data is not NULL");

	ret_if(!data);
	SETTING_TRACE_DEBUG("data is not NULL");

	TelSimPinOperationResult_t sec_rt = result;
	TelSimFacilityInfo_t *fi = data;
	SettingSecurity *ad = user_data;

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
		SETTING_TRACE_DEBUG(
				"SIM settings view is already created. [%d]",
				setting_view_security_sim_settings.is_create);
		setting_view_update(&setting_view_security_sim_settings, ad);
	} else {
		SETTING_TRACE_DEBUG("Change view");
		setting_view_change(&setting_view_security_main,
				&setting_view_security_sim_settings, ad);
	}

	SETTING_TRACE_END;
}

void setting_security_layout_passwd_ug_cb(ui_gadget_h ug, enum ug_mode mode,
		void *priv)
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

void setting_security_destroy_password_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingSecurity *ad = (SettingSecurity *)priv;

	if (ad->md.ly_main)
		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_TRUE);

	if (ug)
		setting_ug_destroy(ug);

	SETTING_TRACE_DEBUG("[TEST] current : %s", ad->input_pwd);
}

void setting_security_end_password_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingSecurity *ad = (SettingSecurity *)priv;
	ad->ug_passwd = NULL;

	if (setting_view_security_sim_settings.is_create == TRUE
			&& ad->pw_type == SETTING_SEC_PW_PIN1_UNBLOCKED) {
		SETTING_TRACE("ungrab keys");
		/* ungrab keys. */
#ifdef ECORE_X
		Ecore_X_Window xwin = 0;
		Ecore_X_Display *disp = NULL;

		int ret = 0;

		disp = ecore_x_display_get();
		xwin = elm_win_xwindow_get((Evas_Object *)ug_get_window());

		ret = eext_win_keygrab_unset(xwin, "XF86Back");
		if (ret)
		SETTING_TRACE("KEY_BACK ungrab failed.");
		ret = eext_win_keygrab_unset(xwin, "XF86Home");
		if (ret)
		SETTING_TRACE("XF86Home ungrab failed.");
		ret = eext_win_keygrab_unset(xwin, "XF86AudioRaiseVolume");
		if (ret)
		SETTING_TRACE("KEY_VOLUMEUP ungrab failed.");
		ret = eext_win_keygrab_unset(xwin, "XF86AudioLowerVolume");
		if (ret)
		SETTING_TRACE("KEY_VOLUMEDOWN ungrab failed.");
#else
		/* @todo : repace codes using X with codes tizen 3.0 API */
#endif
	}
#if SUPPORT_ENCRYPTION
	if (setting_view_security_confirm_encryption.is_create == TRUE) {
		SETTING_TRACE_DEBUG("[TEST] current : %s", ad->input_pwd);
		setting_view_update(&setting_view_security_confirm_encryption,
				ad);
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

	/* ad is point to data */
	SettingSecurity *ad = (SettingSecurity *)data;

	/* prevent the ug from being loaded again due to window event queuing */
	/* added by JTS: CQ H0100135346 */
	if (ad->ug_passwd) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}

	if ((ad->pw_type < 0) || (ad->pw_type > SETTING_SEC_PW_MAX))
		return FALSE;

	char str[Max_Passwd_View_Type_Len + 1] = { 0, };
	safeCopyStr(str, security_table[ad->pw_type].pw_type_string,
	Max_Passwd_View_Type_Len);

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

	if (ad->md.ly_main)
		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);
	ad->ug_removed = FALSE;
//TODO: appctl:
//ad->ug_passwd = setting_ug_create(ad->ug, "setting-password-efl",
//		UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_passwd) { /* error handling */
		evas_object_show(ad->md.ly_main);
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
	SettingSecurity *ad = (SettingSecurity *)priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_lockscreen = NULL;
	}
}

gboolean setting_security_create_lockscreen_options_sg(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	/* ad is point to data */
	SettingSecurity *ad = (SettingSecurity *)data;

	if (ad->ug_lockscreen) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}

	app_launcher("lockscreen-options", NULL, NULL);

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
	SettingSecurity *ad = (SettingSecurity *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_mmc_encryption = NULL;
	}

}

gboolean setting_security_create_mmc_encryption_sg(void *data)
{
	/* error check */
	retv_if(data == NULL, FALSE);

	/* ad is point to data */
	SettingSecurity *ad = (SettingSecurity *) data;

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

	if (ad->md.ly_main)
	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);
	ad->ug_mmc_encryption = setting_ug_create(ad->ug,
			"setting-mmc-encryption-efl", UG_MODE_FULLVIEW, NULL,
			cbs);
	if (NULL == ad->ug_mmc_encryption) { /* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_mmc_encryption");
		evas_object_show(ad->md.ly_main);
	}

	FREE(cbs);

	return TRUE;
}
#endif

void setting_security_layout_firewall_ug_cb(ui_gadget_h ug, enum ug_mode mode,
		void *priv)
{
	Evas_Object *base = ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		/*elm_win_resize_object_add(ad->md.win_main, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}

	return;
}

void setting_security_destroy_firewall_ug_cb(ui_gadget_h ug, void *priv)
{
	ret_if(priv == NULL);
	SettingSecurity *ad = (SettingSecurity *)priv;

	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_TRUE);
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_firewall = NULL;
	}
}

gboolean setting_security_create_firewall_sg(void *data)
{
	retv_if(data == NULL, FALSE);

	SettingSecurity *ad = (SettingSecurity *)data;
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

	if (ad->md.ly_main)
		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);
//TODO: appctl:
//ad->ug_firewall = setting_ug_create(ad->ug, "setting-firewall-efl",
		//UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_firewall) {
		SETTING_TRACE_ERROR("NULL == ad->ug_firewall");
		evas_object_show(ad->md.ly_main);
	}

	FREE(cbs);
	return TRUE;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/
/* -1	on fail */
/* 0~n	on success */
int _get_security_table_index(char *name)
{
	int i = 0;

	for (i = 0; i < SETTING_SEC_PW_MAX; i++) {
		if (0 == safeStrCmp(name, security_table[i].pw_type_string))
			return i;
	}
	return -1;
}

void setting_security_result_password_ug_cb(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data paremeter is NULL");

	/* ad is point to priv */
	SettingSecurity *ad = (SettingSecurity *)priv;

	if (ad->md.ly_main)
		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_TRUE);

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
			setting_view_update(&setting_view_security_sim_settings,
					ad);
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
		FREE(result);
		if (current)
			FREE(current);
		ui_app_exit();
		return;
	} else {
		int index = _get_security_table_index(result);

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

	evas_object_show(ad->md.ly_main);
	FREE(result);
	SETTING_TRACE_END;
}

#ifdef Status
#undef Status
#endif

static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{
		"IDS_JAVA_OPT_SIM_SETTINGS",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_JAVA_OPT_SIM_SETTINGS",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_ST_BODY_DECRYPT_DEVICE",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_DECRYPT_DEVICE",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_ST_HEADER_ENCRYPT_DEVICE",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_ST_HEADER_ENCRYPT_DEVICE",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_DN_BODY_ENCRYPT_SD_CARD",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_DN_BODY_ENCRYPT_SD_CARD",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_ST_BODY_FIREWALL",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_FIREWALL",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_ST_BODY_ANTIVIRUS_SOFTWARE_ABB",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_ANTIVIRUS_SOFTWARE_ABB",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL },
	{
		"IDS_EMAIL_POP_SECURITY_UPDATE_ABB",
		NULL,
		"viewtype:frontpage;tab:first;keyword:IDS_EMAIL_POP_SECURITY_UPDATE_ABB",
		0,
		0,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL }, };

int setting_plugin_search_init(app_control_h service, void *priv,
		char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-security-efl DB search code");

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(
				s_cfg_node_array[i].key_name,
				s_cfg_node_array[i].ug_args,
				IMG_Security,
				s_cfg_node_array[i].item_type,
				s_cfg_node_array[i].data,
				"Security");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

#if SUPPORT_SECURITY_FIREWALL
int setting_security_system_command(const char *command, const char *commandex)
{
	SETTING_TRACE_BEGIN;
	int pid = 0, status = 0;
	char *argv[2] = { NULL, };
	/*int ret; */

	/* if (command == 0 && commandex == 0) {
		not exist input
	} */

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
			NULL, };

		SETTING_TRACE_DEBUG("[%s]:%s", argv[0], argv[1]);
		execv(_TZ_SYS_RO_APP"/org.tizen.setting/bin/firewall", args);
	}
	return 1;
}

int setting_security_firewall_reset()
{
	SETTING_TRACE_BEGIN;

	struct dirent ent, *next_file;
	DIR *theFolder;

	char filepath[256];

	theFolder = opendir(SEC_FIREWALL_DIR);
	readdir_r(theFolder, &ent, &next_file);
	while (next_file) {
		/* build the full path for each file in the folder */
		snprintf(filepath, 256, "%s/%s", SEC_FIREWALL_DIR, ent.d_name);
		SETTING_TRACE_DEBUG("[%s]", filepath);
		/*(void) remove(filepath); */
		if (remove(filepath) != 0) { /*delete the file */
			perror("remove");
		}
		readdir_r(theFolder, &ent, &next_file);
	}

	closedir(theFolder);
	return 0;
}
#endif

