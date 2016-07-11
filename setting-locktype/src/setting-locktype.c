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

/**
 *@defgroup setting-security
 *UG creation code for setting-security
 */

#include <setting-locktype.h>
#include <efl_extension.h>

#include <auth-passwd.h>

#define Max_Passwd_View_Type_Len	64
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

void setting_locktype_ug_popup_resp_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingLocktypeUG *ad = (SettingLocktypeUG *)data;
	if (ad->save_popup) {
		evas_object_del(ad->save_popup);
		ad->save_popup = NULL;
	}
	ug_destroy_me(ad->ug);
}

int pwd_handler_do_nothing(SettingLocktypeUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	/*SettingLocktypeUG *ad = (SettingLocktypeUG *)data; */
	/* DO NOTHING */
	return 0;
}

int pwd_handler_sec_pw_passwd(SettingLocktypeUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingLocktypeUG *ad = (SettingLocktypeUG *)data;

	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
			SETTING_SCREEN_LOCK_TYPE_PASSWORD) == 0) {
		/* set radio */
		if (ad->lock_type_rd)
			elm_radio_value_set(ad->lock_type_rd,
					SETTING_SCREEN_LOCK_TYPE_PASSWORD);

		ad->save_popup = setting_create_popup(ad, ad->win_get, NULL,
				"IDS_ST_POP_NEW_PASSWD_SAVED",
				setting_locktype_ug_popup_resp_cb,
				POPUP_INTERVAL, FALSE, FALSE, 0);
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}

	return 0;
}

int pwd_handler_sec_pw_simple_passwd(SettingLocktypeUG *data, void *arg)
{
	SETTING_TRACE_BEGIN;
	SettingLocktypeUG *ad = (SettingLocktypeUG *)data;

	SETTING_TRACE("ad->selected_lock_type:%s", ad->selected_lock_type);
	/*'face and voice ' and 'simple password' share the same PASSWD_TYPE
	 * and callback function */
	int lock_type = SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD;
	if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type) ==
			0) {
		/* set radio */
		if (ad->lock_type_rd)
			elm_radio_value_set(ad->lock_type_rd, lock_type);

		ad->save_popup = setting_create_popup(ad, ad->win_get, NULL,
				"IDS_ST_POP_NEW_PASSWD_SAVED",
				setting_locktype_ug_popup_resp_cb,
				POPUP_INTERVAL, FALSE, FALSE, 0);
	} else {
		SETTING_TRACE_ERROR(
				"setting vconf value failed : screen_lock_type");
	}
	return 0;
}

static struct _locktype_item locktype_table[] = { {
	SETTING_LOCKTYPE_PW_PASSWORD,
	"SETTING_PW_TYPE_SET_PASSWORD",
	pwd_handler_sec_pw_passwd }, {
	SETTING_LOCKTYPE_PW_SIMPLE_PASSWD,
	"SETTING_PW_TYPE_SET_SIMPLE_PASSWORD",
	pwd_handler_sec_pw_simple_passwd }, {
	SETTING_LOCKTYPE_PW_CHANGE_PASSWD,
	"SETTING_PW_TYPE_SET_PASSWORD",
	pwd_handler_sec_pw_passwd }, {
	SETTING_LOCKTYPE_PW_CHANGE_SIMPLE_PASSWD,
	"SETTING_PW_TYPE_SET_SIMPLE_PASSWORD",
	pwd_handler_sec_pw_simple_passwd },
/*--------------------------------------------------------------------------- */
{ SETTING_LOCKTYPE_PW_MAX, NULL, pwd_handler_do_nothing } };

static void setting_locktype_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);

	/* ad is point to data */
	SettingLocktypeUG *ad = (SettingLocktypeUG *)data;
	setting_view_update(&setting_view_locktype_main, ad);
}

static void *setting_locktype_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingLocktypeUG *locktypeUG = priv;
	locktypeUG->ug = ug;

	locktypeUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	locktypeUG->win_get = (Evas_Object *)ug_get_window();
	evas_object_show(locktypeUG->win_main_layout);
	locktypeUG->evas = evas_object_evas_get(locktypeUG->win_main_layout);

	setting_retvm_if(locktypeUG->win_main_layout == NULL, NULL,
			"[Setting >> About] cannot get main window ");

	/*setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR); */
	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_locktype_main, NULL);

	setting_create_Gendial_itc("dialogue/1text", &(locktypeUG->itc_1text));
	setting_create_Gendial_itc("multiline/1text",
			&(locktypeUG->itc_bg_1icon));
	setting_create_Gendial_itc("dialogue/2text.3",
			&(locktypeUG->itc_2text_2));
	setting_create_Gendial_itc("dialogue/1text.1icon",
			&(locktypeUG->itc_1text_1icon));
	setting_create_Gendial_itc("dialogue/title",
			&(locktypeUG->itc_group_item));
	setting_create_Gendial_itc("dialogue/2text.3/expandable",
			&(locktypeUG->itc_2text_3_parent));

	locktypeUG->input_pwd = NULL;
	locktypeUG->notify = NULL;

	app_control_get_extra_data(service, "current",
			&(locktypeUG->input_pwd));
	app_control_get_extra_data(service, "caller", &(locktypeUG->caller));
	if (locktypeUG->caller && !safeStrCmp(locktypeUG->caller,
			"encryption")) {
		locktypeUG->viewtype = SETTING_LOCKTYPE_VIEWTYPE_ENCRYPTION;
	} else if (locktypeUG->caller && !safeStrCmp(locktypeUG->caller,
			"lockscreen")) {
		locktypeUG->viewtype = SETTING_LOCKTYPE_VIEWTYPE_RECOVERY;
		vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
				SETTING_SCREEN_LOCK_TYPE_SWIPE);
#if SECURITY_SERVER
//		int result = security_server_reset_pwd("0000", 0, 0);
//??		int result = auth_passwd_reset_passwd(AUTH_PWD_NORMAL,uid,
//		"0000");
//		SETTING_TRACE_DEBUG("reset_pwd result : %d", result);
#endif
	} else {
		locktypeUG->viewtype = SETTING_LOCKTYPE_VIEWTYPE_MAIN;
	}
	FREE(locktypeUG->caller);

	setting_view_node_set_cur_view(&setting_view_locktype_main);
	setting_view_create(&setting_view_locktype_main, (void *)locktypeUG);
	evas_object_event_callback_add(locktypeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_locktype_ug_cb_resize,
			locktypeUG);

	return locktypeUG->ly_main;
}

static void setting_locktype_ug_on_start(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_locktype_ug_on_pause(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_locktype_ug_on_resume(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_locktype_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingLocktypeUG *locktypeUG = priv;
	locktypeUG->ug = ug;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(locktypeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_locktype_ug_cb_resize);

	if (locktypeUG->save_popup) {
		evas_object_del(locktypeUG->save_popup);
		locktypeUG->save_popup = NULL;
	}

	setting_view_destroy(&setting_view_locktype_main, locktypeUG);

	FREE(locktypeUG->input_pwd);

	if (NULL != ug_get_layout(locktypeUG->ug)) {
		evas_object_hide((Evas_Object *)ug_get_layout(locktypeUG->ug));
		evas_object_del((Evas_Object *)ug_get_layout(locktypeUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_locktype_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{

}

static void setting_locktype_ug_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
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

static void setting_locktype_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	if (!priv)
		return;
	/* ad is point to priv */
	SettingLocktypeUG *ad = (SettingLocktypeUG *)priv;
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
	SettingLocktypeUG *locktypeUG = calloc(1, sizeof(SettingLocktypeUG));
	setting_retvm_if(!locktypeUG, -1,
			"Create SettingLocktypeUG obj failed");

	memset(locktypeUG, 0x00, sizeof(SettingLocktypeUG));

	ops->create = setting_locktype_ug_on_create;
	ops->start = setting_locktype_ug_on_start;
	ops->pause = setting_locktype_ug_on_pause;
	ops->resume = setting_locktype_ug_on_resume;
	ops->destroy = setting_locktype_ug_on_destroy;
	ops->message = setting_locktype_ug_on_message;
	ops->event = setting_locktype_ug_on_event;
	ops->key_event = setting_locktype_ug_on_key_event;
	ops->priv = locktypeUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	struct SettingLocktypeUG *locktypeUG;
	setting_retm_if(!ops, "ops == NULL");

	locktypeUG = ops->priv;
	if (locktypeUG)
		FREE(locktypeUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_locktype_layout_passwd_ug_cb(ui_gadget_h ug, enum ug_mode mode,
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

void setting_locktype_destroy_password_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingLocktypeUG *ad = (SettingLocktypeUG *)priv;

	if (ad->ly_main)
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);
	if (ug)
		ug_destroy(ug);
}

void setting_locktype_end_password_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingLocktypeUG *ad = (SettingLocktypeUG *)priv;
	ad->ug_passwd = NULL;
}

gboolean setting_locktype_create_password_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	/* ad is point to data */
	SettingLocktypeUG *ad = (SettingLocktypeUG *)data;

	/* prevent the ug from being loaded again due to window event queuing */
	/* added by JTS: CQ H0100135346 */
	if (ad->ug_passwd) {
		SETTING_TRACE("Password UG is already loaded.");
		return FALSE;
	}

	if ((ad->pw_type < 0) || (ad->pw_type > SETTING_LOCKTYPE_PW_MAX))
		return FALSE;

	char str[Max_Passwd_View_Type_Len + 1] = { 0, };
	safeCopyStr(str, locktype_table[ad->pw_type].pw_type_string,
			Max_Passwd_View_Type_Len);

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_locktype_layout_passwd_ug_cb;
	cbs->result_cb = setting_locktype_result_password_ug_cb;
	cbs->destroy_cb = setting_locktype_destroy_password_ug_cb;
	cbs->end_cb = setting_locktype_end_password_ug_cb;
	cbs->priv = (void *)ad;

	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return FALSE;
	}

	app_control_add_extra_data(svc, "viewtype", str);
	if (ad->viewtype == SETTING_LOCKTYPE_VIEWTYPE_ENCRYPTION)
		app_control_add_extra_data(svc, "encryption", "TRUE");

	if (ad->input_pwd)
		app_control_add_extra_data(svc, "current", ad->input_pwd);

	if (ad->caller && !safeStrCmp(ad->caller, "DPM")) 
		app_control_add_extra_data(svc, "caller", "DPM");

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);
	ad->ug_passwd = setting_ug_create(ad->ug, "setting-password-efl",
			UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_passwd) { /* error handling */
		evas_object_show(ad->ly_main);
	}

	app_control_destroy(svc);
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
int _get_locktype_table_index(char *name)
{
	int i = 0;

	for (i = 0; i < SETTING_LOCKTYPE_PW_MAX; i++) {
		if (0 == safeStrCmp(name, locktype_table[i].pw_type_string))
			return i;
	}
	return -1;
}

void setting_locktype_result_password_ug_cb(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data paremeter is NULL");

	/* ad is point to priv */
	SettingLocktypeUG *ad = (SettingLocktypeUG *)priv;

	char *result = NULL;

	app_control_get_extra_data(service, "result", &result);

	/* divided pw view type */
	SETTING_TRACE("ad->pw_type: %d result :%s", ad->pw_type, result);
	ret_if(NULL == result);

	if (safeStrCmp(result, "Cancel")) {
		int index = _get_locktype_table_index(result);

		if (index != -1) {
			locktype_table[index].passwd_handler(ad, NULL);
			ad->pw_type = locktype_table[index].pw_type_num;
		} else {
			SETTING_TRACE_ERROR("no table entry");
		}
	}

	evas_object_show(ad->ly_main);
	FREE(result);
	SETTING_TRACE_END;
}

