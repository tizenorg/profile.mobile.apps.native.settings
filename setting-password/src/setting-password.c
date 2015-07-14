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
#include <setting-password.h>
#include <setting-password-main.h>
#include <security-server.h>

extern void setting_get_pin_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data);

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

struct _pw_item pw_its[] = {
	{ 0, NULL, 0, 0},
	{ SETTING_PW_TYPE_PASSWORD, "SETTING_PW_TYPE_PASSWORD", SETTING_PW_CATEGORY_LOCK, 1 },
	{ SETTING_PW_TYPE_SET_PASSWORD, "SETTING_PW_TYPE_SET_PASSWORD", SETTING_PW_CATEGORY_LOCK, 2 },
	{ SETTING_PW_TYPE_SIMPLE_PASSWORD, "SETTING_PW_TYPE_SIMPLE_PASSWORD", SETTING_PW_CATEGORY_LOCK + SETTING_PW_SUB_CATEGORY_SIMPLE, 1 },
	{ SETTING_PW_TYPE_SET_SIMPLE_PASSWORD, "SETTING_PW_TYPE_SET_SIMPLE_PASSWORD", SETTING_PW_CATEGORY_LOCK + SETTING_PW_SUB_CATEGORY_SIMPLE, 2 },
	{ SETTING_PW_TYPE_ENTER_LOCK_TYPE, "SETTING_PW_TYPE_ENTER_LOCK_TYPE", SETTING_PW_CATEGORY_LOCK, 1 },
	{ SETTING_PW_TYPE_SIM_LOCK_ON, "SETTING_PW_TYPE_SIM_LOCK_ON", SETTING_PW_CATEGORY_SIM, 2 },
	{ SETTING_PW_TYPE_SIM_LOCK_OFF, "SETTING_PW_TYPE_SIM_LOCK_OFF", SETTING_PW_CATEGORY_SIM, 1 },
	{ SETTING_PW_TYPE_PIN_LOCK_ON, "SETTING_PW_TYPE_PIN_LOCK_ON", SETTING_PW_CATEGORY_SIM, 1 },
	{ SETTING_PW_TYPE_PIN_LOCK_OFF, "SETTING_PW_TYPE_PIN_LOCK_OFF", SETTING_PW_CATEGORY_SIM, 1 },
	{ SETTING_PW_TYPE_CHANGE_PIN, "SETTING_PW_TYPE_CHANGE_PIN", SETTING_PW_CATEGORY_SIM, 3 },
	{ SETTING_PW_TYPE_CHANGE_PIN2, "SETTING_PW_TYPE_CHANGE_PIN2", SETTING_PW_CATEGORY_SIM, 3 },
	{ SETTING_PW_TYPE_PIN_BLOCKED, "SETTING_PW_TYPE_PIN_BLOCKED", SETTING_PW_CATEGORY_SIM, 3 },
	{ SETTING_PW_TYPE_PIN2_BLOCKED, "SETTING_PW_TYPE_PIN2_BLOCKED", SETTING_PW_CATEGORY_SIM, 3 },
	{ SETTING_PW_TYPE_MAX, NULL }
};

static void setting_password_ug_cb_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SettingPasswordUG *ad = (SettingPasswordUG *) data; */
	/*setting_view_update(&setting_view_password_main, ad); */
}

void __chk_cur_pw_status(SettingPasswordUG *ad, app_control_h service)
{
	int ret = 0;
	unsigned int attempt = 0;
	unsigned int max_attempt = 0;
	unsigned int expire_sec = 0;

	ad->pw_status = SETTING_PW_STATUS_DEFAULT;

	ret = security_server_is_pwd_valid(&attempt, &max_attempt, &expire_sec);
	SETTING_TRACE_DEBUG("status of password : %d (cur attempt %d, max %d, expire %d", ret, attempt, max_attempt, expire_sec);

	if (ret == SECURITY_SERVER_API_ERROR_NO_PASSWORD) {
		SETTING_TRACE("security-server has no password!");
		ad->pw_status = SETTING_PW_STATUS_EMPTY;
	}
}

void __get_extra_data(SettingPasswordUG *ad, app_control_h service)
{
	SETTING_TRACE_BEGIN;

	char *encryption = NULL;

	/* #1. viewtype */
	app_control_get_extra_data(service, "viewtype", &(ad->view_type_string));
	SETTING_TRACE("viewtype:%s", ad->view_type_string);
	/* #2. current pw */
	app_control_get_extra_data(service, "current", &(ad->cur_pwd));
	if (ad->cur_pwd)
		SETTING_TRACE_SECURE_DEBUG("Receive current : %s", ad->cur_pwd);
}

void __get_password_view_type(SettingPasswordUG *ad, app_control_h service)
{
	SETTING_TRACE_BEGIN;

	setting_pw_type ret_pw_type = 0;
	int i;

	for (i = 1; i < SETTING_PW_TYPE_MAX; i++) {
		if (0 == safeStrCmp(ad->view_type_string, pw_its[i].pw_type_string)) {
			SETTING_TRACE_DEBUG("%s %d", ad->view_type_string, pw_its[i].pw_type_num);
			ret_pw_type = pw_its[i].pw_type_num;
			break;
		}
	}

	if (pw_its[ret_pw_type].category == SETTING_PW_CATEGORY_SIM) {
		SETTING_TRACE_DEBUG("SIM or Fingerprint do not need to check pw status. %d", pw_its[ret_pw_type].category);
		ad->view_type = ret_pw_type;
		return;
	}

	__chk_cur_pw_status(ad, service);

	/* Some pw types should be changed, according to pw status and current lock type */
	if (ret_pw_type == SETTING_PW_TYPE_ENTER_LOCK_TYPE) {
		if (ad->pw_status == SETTING_PW_STATUS_EMPTY) {
			if (ad->screen_lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD)
				ret_pw_type = SETTING_PW_TYPE_SET_PASSWORD;
			else if (ad->screen_lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD)
				ret_pw_type = SETTING_PW_TYPE_SET_SIMPLE_PASSWORD;
		} else {
			if (ad->screen_lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD)
				ret_pw_type = SETTING_PW_TYPE_PASSWORD;
			else if (ad->screen_lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD)
				ret_pw_type = SETTING_PW_TYPE_SIMPLE_PASSWORD;
		}
	}
	/* end */

	ad->view_type = ret_pw_type;
}

static setting_view *__initialize_view_node_table(int category)
{
	setting_view_node_table_intialize();

	if (category == SETTING_PW_CATEGORY_SIM) {
		setting_view_node_table_register(&setting_view_password_sim, NULL);
		return &setting_view_password_sim;
	} else if (category & SETTING_PW_SUB_CATEGORY_SIMPLE) {
		setting_view_node_table_register(&setting_view_password_simple, NULL);
		return &setting_view_password_simple;
	} else {
		setting_view_node_table_register(&setting_view_password_main, NULL);
		return &setting_view_password_main;
	}
}

static void __destroy_view(SettingPasswordUG *ad)
{
	ret_if(!ad);

	if (pw_its[ad->view_type].category == SETTING_PW_CATEGORY_SIM) {
		setting_view_destroy(&setting_view_password_sim, ad);
	}
	if (pw_its[ad->view_type].category & SETTING_PW_SUB_CATEGORY_SIMPLE) {
		setting_view_destroy(&setting_view_password_simple, ad);
	} else {
		/*  delete the allocated objects. */
		setting_view_destroy(&setting_view_password_main, ad);
	}
}

static char *__gl_err_desc_text_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, NULL);
	char buf[256] = {0,};

	snprintf(buf, 256, "<font color=#ff0000>%s</font>", (char *)data);

	SETTING_TRACE_DEBUG("buf is %s", buf);

	return (char *)g_strdup(buf);
}

static void __gl_err_desc_del(void *data, Evas_Object *obj)
{
	SETTING_TRACE_BEGIN;
	char *desc = data;
	if (desc) {
		G_FREE(desc);
		SETTING_TRACE_DEBUG("free err_desc");
	}
}

static void setting_password_rotated_cb(void *data, Evas_Object *obj, void *event)
{
	SETTING_TRACE_BEGIN;
	SettingPasswordUG *ad = (SettingPasswordUG *)data;

	if (ad == NULL || ad->win_get == NULL || obj == NULL)
		return;

	/*int changed_ang = 0;
	changed_ang = elm_win_rotation_get(obj);*/

	if (ad->view_type == SETTING_PW_TYPE_PIN_LOCK_OFF
	    || ad->view_type == SETTING_PW_TYPE_PIN_LOCK_ON
	    || ad->view_type == SETTING_PW_TYPE_CHANGE_PIN
	    || ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED) {
		if (ad->handle) {
			if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_SC, setting_get_pin_lock_info_cb, ad) != 0)
				SETTING_TRACE_DEBUG("failed to call tel_get_sim_lock_info()");
		}
	} else if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN2
	           || ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
		if (ad->handle) {
			if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_FD, setting_get_pin_lock_info_cb, ad) != 0)
				SETTING_TRACE_DEBUG("failed to call tel_get_sim_lock_info()");
		}
	}
	/* it can manage the cursor self */
	if (ad->ed_pw1 && (ad->ed_pw1->isFocusFlag == TRUE)) {
		if (ad->ed_pw1->eo_check) {
			SETTING_TRACE_DEBUG("set focus on first entry");
			/* no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw1->eo_check);
		}
	} else if (ad->ed_pw2 && (ad->ed_pw2->isFocusFlag == TRUE)) {
		if (ad->ed_pw2->eo_check) {
			SETTING_TRACE_DEBUG("set focus on second entry");
			/*no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw2->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw2->eo_check);
		}
	} else if (ad->ed_pw3 && (ad->ed_pw3->isFocusFlag == TRUE)) {
		if (ad->ed_pw3->eo_check) {
			SETTING_TRACE_DEBUG("set focus on third entry");
			/*no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw3->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw3->eo_check);
		}
	} else {
		SETTING_TRACE_DEBUG("no entry to set focus");
	}
}

static void *setting_password_ug_on_create(ui_gadget_h ug,
                                           enum ug_mode mode, app_control_h service,
                                           void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingPasswordUG *passwordUG = (SettingPasswordUG *)priv;
	passwordUG->ug = ug;
	passwordUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	passwordUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(passwordUG->win_main_layout);
	passwordUG->evas = evas_object_evas_get(passwordUG->win_main_layout);

	setting_retvm_if(passwordUG->win_main_layout == NULL, NULL,
	                 "cannot get main window ");

	/* set launguage */
	/*setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR); */
	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);
	evas_object_smart_callback_add(passwordUG->win_get, "wm,rotation,changed", setting_password_rotated_cb, passwordUG);

	setting_create_Gendial_itc("groupindex", &(passwordUG->itc_title));
	setting_create_Gendial_itc("1icon", &(passwordUG->itc_variable_height));

	passwordUG->itc_err_desc.item_style = "multiline_sub";
	passwordUG->itc_err_desc.func.text_get = __gl_err_desc_text_get;
	passwordUG->itc_err_desc.func.content_get = NULL;
	passwordUG->itc_err_desc.func.state_get = NULL;
	passwordUG->itc_err_desc.func.del = __gl_err_desc_del;

	/* get screen lock type */
	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &(passwordUG->screen_lock_type));
	SETTING_TRACE_DEBUG("screen lock type : %d", passwordUG->screen_lock_type);

	/* init */
	__get_extra_data(passwordUG, service);
	__get_password_view_type(passwordUG, service);
	passwordUG->step1_str = NULL;

	setting_view *main_view = __initialize_view_node_table(pw_its[passwordUG->view_type].category);

	/*  creating a view. */
	setting_view_create(main_view, (void *)passwordUG);
	evas_object_event_callback_add(passwordUG->win_main_layout,
	                               EVAS_CALLBACK_RESIZE,
	                               setting_password_ug_cb_resize,
	                               passwordUG);

	return passwordUG->ly_main;
}

static void setting_password_ug_on_start(ui_gadget_h ug, app_control_h service, void *priv)
{
}

static void setting_password_ug_on_pause(ui_gadget_h ug, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingPasswordUG *passwordUG = (SettingPasswordUG *)priv;

	if (pw_its[passwordUG->view_type].category & SETTING_PW_SUB_CATEGORY_SIMPLE) {
		/* If pause, simple password's SIP should be focused out.
			If not, Our SIP will not be shown after using Other apps with independent SIP. */
		if (passwordUG->imf_context) {
			ecore_imf_context_reset(passwordUG->imf_context);
			ecore_imf_context_focus_out(passwordUG->imf_context);
		}
	}
}

static void setting_password_ug_on_resume(ui_gadget_h ug, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingPasswordUG *passwordUG = (SettingPasswordUG *)priv;

	if (pw_its[passwordUG->view_type].category & SETTING_PW_SUB_CATEGORY_SIMPLE) {
		/* If resume, simple password's SIP should be focused in.
		If not, Our SIP will not be shown after using Other apps with independent SIP. */
		if (passwordUG->imf_context) {
			/*ecore_imf_context_input_panel_show(passwordUG->imf_context); */
			ecore_imf_context_focus_in(passwordUG->imf_context);
		}
	}
}

static void setting_password_ug_on_destroy(ui_gadget_h ug, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingPasswordUG *passwordUG = (SettingPasswordUG *)priv;

	evas_object_event_callback_del(passwordUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_password_ug_cb_resize);	/* fix flash issue for gallery */
	evas_object_smart_callback_del(passwordUG->win_get, "wm,rotation,changed", setting_password_rotated_cb);
	passwordUG->ug = ug;

	if (passwordUG->view_type_string)
		FREE(passwordUG->view_type_string);

	if (passwordUG->cur_pwd)
		FREE(passwordUG->cur_pwd);

	if (passwordUG->t_info) {
		FREE(passwordUG->t_info);
		passwordUG->t_info = NULL;
	}

	if (pw_its[passwordUG->view_type].category & SETTING_PW_SUB_CATEGORY_SIMPLE) {
		if (passwordUG->imf_context) {
			ecore_imf_context_del(passwordUG->imf_context);
			passwordUG->imf_context = NULL;
		}
	}

	__destroy_view(passwordUG);

	if (NULL != ug_get_layout(passwordUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(passwordUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(passwordUG->ug));
	}
	SETTING_TRACE_END;
}

static void setting_password_ug_on_message(ui_gadget_h ug, app_control_h msg,
                                           app_control_h service, void *priv)
{

}

static void setting_password_ug_on_event(ui_gadget_h ug,
                                         enum ug_event event, app_control_h service,
                                         void *priv)
{
	SETTING_TRACE_BEGIN;
	if (!priv)
		return;

	SettingPasswordUG *ad = (SettingPasswordUG *)priv;
	/*static int old_event = -1; */
	switch (event) {
		case UG_EVENT_LOW_MEMORY:
			break;
		case UG_EVENT_LOW_BATTERY:
			break;
		case UG_EVENT_LANG_CHANGE: {
#if 0
				/* update toolbar items */
				Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it, "toolbar");
				if (toolbar != NULL) {
					Elm_Object_Item *last_item = elm_toolbar_last_item_get(toolbar);
					Elm_Object_Item *first_item = elm_toolbar_last_item_get(toolbar);

					if (last_item)
						elm_object_item_text_set(last_item, _("IDS_SA_BUTTON_DONE_ABB"));
					if (first_item && (first_item != last_item))
						elm_object_item_text_set(last_item, _("IDS_ST_BUTTON_CANCEL_ABB"));
				}
#endif
			}
			break;
		case UG_EVENT_ROTATE_PORTRAIT:
		case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		case UG_EVENT_ROTATE_LANDSCAPE:
		case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
			break;
		case UG_EVENT_REGION_CHANGE:
			break;
		default:
			break;
	}
}

static void setting_password_ug_on_key_event(ui_gadget_h ug,
                                             enum ug_key_event event,
                                             app_control_h service, void *priv)
{
	if (!priv)
		return;

	SettingPasswordUG *ad = (SettingPasswordUG *)priv;

	switch (event) {
		case UG_KEY_EVENT_END:
			ug_destroy_me(ug);
			break;
		default:
			break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SettingPasswordUG *passwordUG = calloc(1, sizeof(SettingPasswordUG));
	setting_retvm_if(!passwordUG, -1, "Create SettingPasswordUG obj failed");

	ops->create = setting_password_ug_on_create;
	ops->start = setting_password_ug_on_start;
	ops->pause = setting_password_ug_on_pause;
	ops->resume = setting_password_ug_on_resume;
	ops->destroy = setting_password_ug_on_destroy;
	ops->message = setting_password_ug_on_message;
	ops->event = setting_password_ug_on_event;
	ops->key_event = setting_password_ug_on_key_event;
	ops->priv = passwordUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	memset(passwordUG, 0x00, sizeof(SettingPasswordUG));

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingPasswordUG *passwordUG;
	setting_retm_if(!ops, "ops == NULL");

	passwordUG = ops->priv;
	if (passwordUG)
		FREE(passwordUG);
}

void setting_password_ug_popup_resp_cb(void *data, Evas_Object *obj,
                                       void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");

	app_control_h svc;
	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	if (ad->destroy) {
		/*  Success to Operate */

		if (app_control_create(&svc))
			return;

		app_control_add_extra_data(svc, "result", ad->view_type_string);
		ug_send_result(ad->ug, svc);
		SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

		app_control_destroy(svc);
		/* Send destroy request */
		ug_destroy_me(ad->ug);
	} else {
		if (ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED) {
			ug_destroy_me(ad->ug);
			return;
		} else if (ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
			ug_destroy_me(ad->ug);
			return;
		}
#if SUPPORT_SIMLOCK
		char *diable_view_type = NULL;
		if (ad->disable_item_type == SETTING_PW_TYPE_SIM_LOCK_OFF) {
			diable_view_type = "SETTING_PW_TYPE_SIM_LOCK_DISABLE";
		}

		if (diable_view_type) {
			/*  Success to Operate */

			if (app_control_create(&svc))
				return;

			app_control_add_extra_data(svc, "result", diable_view_type);
			ug_send_result(ad->ug, svc);

			app_control_destroy(svc);
			/* Send destroy request */
			ug_destroy_me(ad->ug);
		}
#endif
		if (ad->focus_data) {
			ad->focus_data->isFocusFlag = TRUE;
			elm_object_item_data_set(ad->focus_data->item, ad->focus_data);
			elm_genlist_item_update(ad->focus_data->item);
		}
	}
}

void setting_password_ug_display_desc(void *data, char *desc, int destroy)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL || desc == NULL);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;

	ad->destroy = destroy;

	if (ad->scroller == NULL)
		return;

	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}

	ad->err_desc = setting_create_Gendial_field_helpitem_without_bottom_separator(
	                   ad->scroller,
	                   &(itc_multiline_text),
	                   SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
	                   desc);

	/* if pw incorrect, show keyboard again. */
	/*if(ad->ed_pw1 && ad->ed_pw1->eo_check) */
	/*{ */
	/*	elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE); */
	/*} */
}

void
setting_password_ug_create_popup_notitle_nobtn(void *data, char *str,
                                               int destroy)
{
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	ad->destroy = destroy;
	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}
	ad->notify = setting_create_popup_without_btn(ad, ad->ly_main, NULL, str,
	                                              setting_password_ug_popup_resp_cb,
	                                              POPUP_INTERVAL, FALSE, FALSE);
}

void setting_password_ug_check_attemps_left(void *data)
{
	ret_if(!data);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	setting_int_slp_list item_attempts_left;
	setting_str_slp_list item_lock_timestamp;
	int attemps_left_max = 5;

	if (ad->view_type == SETTING_PW_TYPE_PASSWORD) {
		item_attempts_left = INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT;
		item_lock_timestamp = STR_SLP_SETTING_PHONE_LOCK_TIMESTAMP;
		attemps_left_max = PHONE_LOCK_ATTEMPS_MAX;
	} else if (ad->view_type == SETTING_PW_TYPE_SIM_LOCK_OFF) {
		item_attempts_left = INT_SLP_SETTING_SIM_LOCK_ATTEMPTS_LEFT;
		item_lock_timestamp = STR_SLP_SETTING_SIM_LOCK_TIMESTAMP;
		attemps_left_max = SIM_LOCK_ATTEMPS_MAX;
	} else {
		return;
	}

	int value = -1;
	int err = SETTING_RETURN_SUCCESS;
	setting_get_int_slp_key(item_attempts_left, &value, &err);
	setting_retm_if(err == SETTING_RETURN_FAIL,
	                "[Error] get value of vconf fail.");

	value--;

	if (value > 0 && value <= attemps_left_max) {
		setting_set_int_slp_key(item_attempts_left, value, &err);
		setting_retm_if(err == SETTING_RETURN_FAIL, "[Error] set value of vconf fail.");
		char temp_str[MAX_SPECIALIZITION_LEN] = {0,};
		char temp[MAX_SPECIALIZITION_LEN] = {0,};

		if (value > 1)
			snprintf(temp, sizeof(temp), "%s. %s.", PW_ERR_DESC, ATTEMPTS_DESC);
		else
			snprintf(temp, sizeof(temp), "%s. %s.", PW_ERR_DESC, ATTEMPT_DESC);

		int ret = snprintf(temp_str, MAX_SPECIALIZITION_LEN, temp, value);

		ret_if(ret < 0);

		setting_password_ug_display_desc(ad, temp_str, FALSE);
	} else if (value == 0) {
		/*  store the lock timestamp */
		time_t cur_time = time(NULL);
		char cur_timestamp[LOCK_TIMESTAMP_LEN] = { 0, };
		int ret = snprintf(cur_timestamp, sizeof(cur_timestamp), "%ld", cur_time);
		ret_if(ret < 0);

		setting_set_string_slp_key(item_lock_timestamp, cur_timestamp, &err);
		setting_retm_if(err == SETTING_RETURN_FAIL,
		                "[Error] set value of vconf fail.");

		setting_set_int_slp_key(item_attempts_left, value, &err);
		setting_retm_if(err == SETTING_RETURN_FAIL,
		                "[Error] set value of vconf fail.");

		ad->disable_item_type = ad->view_type;

		char temp[MAX_SPECIALIZITION_LEN] = {0,};
		snprintf(temp, sizeof(temp), PW_ERR_DELAY_DESC, PW_ERR_DELAY_TIME);

		setting_password_ug_create_popup_notitle_nobtn(ad, temp, FALSE);
	} else {
		setting_password_ug_create_popup_notitle_nobtn(ad,
		                                               _("IDS_ST_POP_ERROR"), FALSE);
	}

}

int setting_password_check_password(const char *challenge, unsigned int *remain_attempt, unsigned int *valid_sec)
{
	SETTING_TRACE_BEGIN;
	retv_if(challenge == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	int inner_ret = 0;
	int ret = 0;
	unsigned int current_attempt = 0;
	unsigned int max_attempt = 0;
	unsigned int valid_secs = 0;

	SETTING_TRACE_DEBUG("check pwd : %s", challenge);
	inner_ret = security_server_chk_pwd(challenge, &current_attempt, &max_attempt, &valid_secs);

	SETTING_TRACE_DEBUG("chk password : %d", inner_ret);
	SETTING_TRACE_SECURE_DEBUG("current_attempt : %d, max_attempt : %d, valid_secs : %d secs", current_attempt, max_attempt, valid_secs);

	if (valid_sec != NULL)
		*valid_sec = valid_secs;

	if (inner_ret == SECURITY_SERVER_API_SUCCESS) {
		ret = SETTING_RETURN_SUCCESS;
	} else {
		if (remain_attempt != NULL) {
			if ((max_attempt > 0) && (max_attempt >= current_attempt))
				*remain_attempt = max_attempt - current_attempt;
			else
				*remain_attempt = ATTEMPT_INFINITE;	/* infinite */
		}
		ret = SETTING_RETURN_FAIL;
	}

	return ret;
}

int setting_password_set_password(const char *cur_pwd, const char *new_pwd, void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(new_pwd == NULL || data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;
	int ret = 0;
	int err;

	/* To support key manager */
	uid_t user = 5000;
	/*int ckmc_ret = CKMC_ERROR_NONE; */

	/* max attempt count will be handled in passwordug for a while. */
	if (ad->pw_status == SETTING_PW_STATUS_EMPTY) {
		ret = security_server_set_pwd(NULL, new_pwd, 0, 0);
		SETTING_TRACE_DEBUG("[security_server_set_pwd() + empty current] returns %d, INFINITE", ret);
		if (ret == SECURITY_SERVER_API_SUCCESS) {
#if 0
			ckmc_ret = ckmc_reset_user_password(user, new_pwd);
			SETTING_TRACE("ckmc_reset_user_password() returns %d", ckmc_ret);
			setting_set_int_slp_key(INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT, PHONE_LOCK_ATTEMPS_MAX, &err);
#endif
			return SETTING_RETURN_SUCCESS;
		} else {
			return SETTING_PW_ERROR_UNKNOWN;
		}
	} else {
		ret = security_server_set_pwd(cur_pwd, new_pwd, 0, 0);
		SETTING_TRACE_DEBUG("[security_server_set_pwd()] returns %d, INFINITE", ret);
		if (ret == SECURITY_SERVER_API_SUCCESS) {
#if 0
			ckmc_ret = ckmc_change_user_password(user, cur_pwd, new_pwd);
			SETTING_TRACE("ckmc_change_user_password() returns %d", ckmc_ret);
			setting_set_int_slp_key(INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT, PHONE_LOCK_ATTEMPS_MAX, &err);
#endif
			return SETTING_RETURN_SUCCESS;
		} else {
			return SETTING_PW_ERROR_UNKNOWN;
		}
	}
}


