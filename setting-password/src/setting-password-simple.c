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
#include <setting-common-data-slp-setting.h>
#include <setting-debug.h>
#include <dd-haptic.h>
#include <Ecore_IMF.h>
#include <Ecore_IMF_Evas.h>
#include <Ecore.h>
#include <time.h>
#include <notification.h>


int setting_password_simple_check_simple_password(const char *collection);
int setting_password_simple_draw_next_simple_password(void *data, int title_index);

static int setting_password_simple_create(void *cb);
static int setting_password_simple_destroy(void *cb);
static int setting_password_simple_update(void *cb);
static int setting_password_simple_cleanup(void *cb);

setting_view setting_view_password_simple = {
	.create = setting_password_simple_create,
	.destroy = setting_password_simple_destroy,
	.update = setting_password_simple_update,
	.cleanup = setting_password_simple_cleanup,
};


#define SETTING_SP_TITLE_INDEX_START 0
#define SETTING_SP_TITLE_INDEX_MAX 3
const char *sp_titles[5] = { PW_ST_ENTER_PW,
							 PW_ST_NEW_PW,
							 PW_ST_ENTER_PW_AGAIN,
							 PW_ST_ENTER_CUR_PW,
							 NULL,
						   };


extern struct _pw_item pw_its[];

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static Eina_Bool setting_password_simple_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->focus_timer) {
		ecore_timer_del(ad->focus_timer);
		ad->focus_timer = NULL;
	}

	if (ad->imf_handler) {
		ecore_event_handler_del(ad->imf_handler);
		ad->imf_handler = NULL;
	}
	if (ad->imf_context) {
		ecore_imf_context_del(ad->imf_context);
		ad->imf_context = NULL;
	}

	/* send result : Cancel */
	app_control_h svc;
	if (app_control_create(&svc) == 0) {
		app_control_add_extra_data(svc, "result", "Cancel");
		ug_send_result(ad->ug, svc);
		SETTING_TRACE("Send Result : %s\n", "Cancel");

		app_control_destroy(svc);
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);

	return EINA_TRUE;
}

static void setting_password_simple_add_description(Evas_Object *layout, char *desc)
{
	ret_if(!layout || !desc);
	elm_object_part_text_set(layout, "help.txt", _(desc));
}

static void _copy_chars_from_sp_entries(void *data, char *collection)
{
	ret_if(data == NULL || collection == NULL);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->sp_entry1 == NULL || ad->sp_entry2 == NULL ||
		ad->sp_entry3 == NULL || ad->sp_entry4 == NULL)
		return;

	strncpy(collection, ad->sp_chars, 4);
	memset(ad->sp_chars, 0x00, 5);
}

/* Verify simple password */
static int _handle_step1(void *data, char *collection)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_RETURN_FAIL);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	int ret = 0;
	int err = 0;

	_copy_chars_from_sp_entries(ad, collection);

	ret = setting_password_simple_check_simple_password(collection);
	SETTING_TRACE("ret:%d", ret);
	if (ret < 0) {
		SETTING_TRACE_ERROR("Invalid Password formatting")
		setting_password_simple_draw_next_simple_password(ad, 3);
		return SETTING_RETURN_FAIL;
	}

	unsigned int remain_attempt;
	unsigned int valid_sec;
	ret = setting_password_check_password(collection, &remain_attempt, &valid_sec);
	SETTING_TRACE("ret:%d", ret);
	if (ret != SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Failed to invoke security_server_chk_pwd")
		setting_password_simple_draw_next_simple_password(ad, 3);
		ad->remain_attempt = remain_attempt;
		ad->valid_sec = valid_sec;
		return SETTING_PW_ERROR_DO_NOT_MATCH;
	}

	int value = 0;
	ecore_imf_context_input_panel_hide(ad->imf_context);

	if (0 == safeStrCmp(ad->view_type_string, "SETTING_PW_TYPE_SIMPLE_PASSWORD")) {
		setting_get_bool_slp_key(BOOL_SLP_SETTING_POWER_ON_LOCK, &value, &err);
		SETTING_TRACE_DEBUG("get VCONF POWER_ON_LOCK : %d", value);

		if (value == 0)
			ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_POWER_ON_LOCK,
										   SETTING_ON_OFF_BTN_ON, &err);
		else
			ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_POWER_ON_LOCK,
										   SETTING_ON_OFF_BTN_OFF, &err);

		/* reset VCONF */
		setting_set_int_slp_key(INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT,
								PHONE_LOCK_ATTEMPS_MAX, &err);
		setting_retvm_if(err == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL, "[Error] set value of vconf fail.");

		/* Correct Password. Send Bundle to caller for status change */
	}

	app_control_h svc;
	if (app_control_create(&svc)) {
		setting_password_simple_draw_next_simple_password(ad, 3);
		return SETTING_RETURN_FAIL;
	}

	app_control_add_extra_data(svc, "result", ad->view_type_string);

	if (0 == safeStrCmp(ad->view_type_string, "SETTING_PW_TYPE_ENTER_LOCK_TYPE"))
		app_control_add_extra_data(svc, "current", collection);

	/* reset static variables */
	memset(collection, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);

	ug_send_result(ad->ug, svc);
	SETTING_TRACE("Send Result : %s\n", ad->view_type_string);
	app_control_destroy(svc);
	/* Send destroy request */
	ug_destroy_me(ad->ug);

	return SETTING_RETURN_SUCCESS;
}

/* Set new simple password */
static int _handle_step2(void *data, int *step, char *collection1, char *collection2)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_RETURN_FAIL);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	int ret = 0;


	if (*step == 0) {	/* Enter new password */
		_copy_chars_from_sp_entries(ad, collection1);

		ret = setting_password_simple_check_simple_password(collection1);
		if (ret < 0) {
			setting_password_simple_draw_next_simple_password(ad, 1);
			return SETTING_RETURN_FAIL;
		}

		(*step)++;
		setting_password_simple_draw_next_simple_password(ad, 2);
	} else if (*step == 1) {	/* Enter password again */
		_copy_chars_from_sp_entries(ad, collection2);

		ret = setting_password_simple_check_simple_password(collection2);
		if (ret < 0) {
			setting_password_simple_draw_next_simple_password(ad, 2);
			return SETTING_RETURN_FAIL;
		}

		/* compare two input passwords : collection_step1, collection_step2 */
		if (safeStrNCmp(collection1, collection2, 4) == 0) {
			ecore_imf_context_input_panel_hide(ad->imf_context);
			ret = setting_password_set_password(ad->cur_pwd, collection1, ad);

			if (SETTING_RETURN_SUCCESS == ret) {
				FREE(ad->step1_str);

				app_control_h svc;
				if (!app_control_create(&svc)) {
					app_control_add_extra_data(svc, "result", ad->view_type_string);

					/* reset static variables */
					*step = 0;
					/* reset static variables */
					memset(collection1, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);
					memset(collection2, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);

					ug_send_result(ad->ug, svc);
					SETTING_TRACE("Send Result : %s\n", ad->view_type_string);
					app_control_destroy(svc);
					/* Send destroy request */
					ug_destroy_me(ad->ug);
					return SETTING_RETURN_SUCCESS;
				}
				/* Display popup */
				setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_NEW_PASSWD_SAVED"), TRUE);
			} else if (ret == SETTING_PW_ERROR_REUSED) {
				setting_password_simple_draw_next_simple_password(ad, 1);
				setting_password_simple_add_description(ad->simple_layout, PW_ERR_REUSED);
				*step = 0;
				/* reset static variables */
				memset(collection1, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);
				memset(collection2, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);
				return SETTING_PW_ERROR_REUSED;
			} else {
				setting_password_simple_draw_next_simple_password(ad, 1);
				char temp[256] = {0,};
				snprintf(temp, 256, "Unable to save password (%d)", ret);
				setting_password_ug_create_popup_notitle_nobtn(ad, temp, FALSE);

				*step = 0;
				/* reset static variables */
				memset(collection1, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);
				memset(collection2, 0x00, SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1);
				return SETTING_PW_ERROR_UNKNOWN;
			}
		} else {
			setting_password_simple_draw_next_simple_password(ad, 2);
			return SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW;
		}
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_simple_done_simple_password(void *data, int on_locking)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_RETURN_FAIL);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	static int step = 0;

	static char collection_step1[SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1] = {0,};
	static char collection_step2[SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH + 1] = {0,};

	switch (ad->view_type) {
	case SETTING_PW_TYPE_SIMPLE_PASSWORD: {
			SETTING_TRACE_DEBUG("case SIMPLE_PASSWORD");
			return _handle_step1(ad, collection_step1);
		}
	case SETTING_PW_TYPE_SET_SIMPLE_PASSWORD: {
			SETTING_TRACE_DEBUG("case SET_SIMPLE_PASSWORD");
			return _handle_step2(ad, &step, collection_step1, collection_step2);
		}

	default:
		break;
	}
	return SETTING_RETURN_SUCCESS;
}

static Eina_Bool setting_password_simple_check_4entries(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, EINA_FALSE);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	int ret = 0;

	ret = setting_password_simple_done_simple_password(ad, TRUE);

	memset(ad->sp_chars, 0x00, 5);

	if (ret == SETTING_RETURN_FAIL) {
		setting_password_simple_add_description(ad->simple_layout, PW_ST_INCORRECT_PW_TRY_AGAIN);
	} else if (ret == SETTING_PW_ERROR_DO_NOT_MATCH) {
		if (0 == safeStrCmp(ad->view_type_string, "SETTING_PW_TYPE_ENTER_LOCK_TYPE")) {

			setting_password_simple_add_description(ad->simple_layout, PW_ERR_DESC);
			return EINA_FALSE;
		}
		/** @todo setting_password_ug_check_attemps_left */
		setting_int_slp_list item_attempts_left;
		setting_str_slp_list item_lock_timestamp;
		int attemps_left_max = 5;
		item_attempts_left = INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT;
		item_lock_timestamp = STR_SLP_SETTING_PHONE_LOCK_TIMESTAMP;
		attemps_left_max = PHONE_LOCK_ATTEMPS_MAX;

		int value = -1;
		int err = SETTING_RETURN_SUCCESS;
		setting_get_int_slp_key(item_attempts_left, &value, &err);
		retvm_if(err == SETTING_RETURN_FAIL, EINA_FALSE, "[Error] get value of vconf fail.");
		value--;
		if (value > 0 && value <= attemps_left_max) {
			setting_set_int_slp_key(item_attempts_left, value, &err);
			setting_retvm_if(err == SETTING_RETURN_FAIL, EINA_FALSE, "[Error] set value of vconf fail.");
			char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
			char temp[MAX_SPECIALIZITION_LEN] = {0,};
			if (value > 1)
				snprintf(temp, sizeof(temp), "%s. %s.", PW_ERR_DESC, ATTEMPTS_DESC);
			else
				snprintf(temp, sizeof(temp), "%s. %s.", PW_ERR_DESC, ATTEMPT_DESC);
			snprintf(speciliztion, sizeof(speciliztion), temp, value);
			setting_password_simple_add_description(ad->simple_layout, speciliztion);
		} else if (value == 0) {
			/*  store the lock timestamp */
			elm_object_focus_set(ad->sp_entry1, EINA_FALSE);
			time_t cur_time = time(NULL);
			char cur_timestamp[LOCK_TIMESTAMP_LEN] = { 0, };
			int ret = snprintf(cur_timestamp, sizeof(cur_timestamp), "%ld",	cur_time);
			retv_if(ret < 0, EINA_FALSE);
			setting_set_string_slp_key(item_lock_timestamp, cur_timestamp, &err);
			setting_retvm_if(err == SETTING_RETURN_FAIL, EINA_FALSE,
							 "[Error] set value of vconf fail.");

			setting_set_int_slp_key(item_attempts_left, value, &err);
			setting_retvm_if(err == SETTING_RETURN_FAIL, EINA_FALSE,
							 "[Error] set value of vconf fail.");

			ad->disable_item_type = ad->view_type;

			char temp[MAX_SPECIALIZITION_LEN] = {0,};
			snprintf(temp, sizeof(temp), PW_ERR_DELAY_DESC, PW_ERR_DELAY_TIME);
			setting_password_ug_create_popup_notitle_nobtn(ad, temp, TRUE);
		}
	} else if (ret == SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW) {
		setting_password_simple_add_description(ad->simple_layout, PW_ST_PW_DO_NOT_MATCH);
	} else {
		/* if you have more cases, modify here. */
	}

	ad->simple_done_timer = NULL;
	return EINA_FALSE;
}

Ecore_IMF_Context *__imf_context_create(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, NULL);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	Ecore_IMF_Context *imf_context = NULL;
	const char *ctx_id = ecore_imf_context_default_id_get();

	if (ctx_id) {
		imf_context = ecore_imf_context_add(ctx_id);

		if (ad->evas && imf_context) {
			ecore_imf_context_client_window_set(imf_context, (void *)ecore_evas_window_get(ecore_evas_ecore_evas_get(ad->evas)));
			ecore_imf_context_client_canvas_set(imf_context, ad->evas);
		}
	}
	return imf_context;
}

static Eina_Bool __imf_event_key_down_cb(void *data, int type, void *event)
{
	SETTING_TRACE_BEGIN;
	SettingPasswordUG *ad = (SettingPasswordUG *)data;
	Ecore_Event_Key *ev = event;

	char *commit_str = (char *)ev->key;
	char *digit = (char *)ev->string;
	SETTING_TRACE_DEBUG("ev->keyname is %s\n", commit_str); /*here you get the Input String from ISE */

	if (!safeStrCmp(commit_str, "Return"))
		return EINA_TRUE;
	else if (!safeStrCmp(commit_str, "XF86Back"))
		return EINA_TRUE;

	if (!safeStrCmp(commit_str, "BackSpace")) {
		if (ad->sp_focused_num > 0)
			elm_entry_entry_set(ad->sp_entry[--ad->sp_focused_num], "");
		if (ad->sp_focused_num < 0)
			ad->sp_focused_num = 0;
	} else {
		if (!digit)
			return EINA_TRUE;

		if (ad->sp_focused_num >= 4) {
			SETTING_TRACE_DEBUG("all entries are filled.");
			return EINA_TRUE;
		} else if (ad->sp_focused_num < 0) {
			ad->sp_focused_num = 0;
		}
		char key = digit[0];
		SETTING_TRACE_DEBUG("digit is %c", key);
		if (key >= '0' && key <= '9') {
			ad->sp_chars[ad->sp_focused_num] = key;
			elm_entry_entry_set(ad->sp_entry[ad->sp_focused_num], "<align=center>•</align>");
			ad->sp_focused_num++;
			if (ad->sp_focused_num == 4)
				/*call done cb */
				ad->simple_done_timer = ecore_timer_add(0.2, setting_password_simple_check_4entries, ad);
		}
	}

	return EINA_TRUE;
};

void __imf_show_for_simple_password(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->imf_context) {
		ecore_imf_context_input_panel_show(ad->imf_context);
	} else {
		Ecore_IMF_Context *imf_context = NULL;
		imf_context = __imf_context_create(ad);

		ad->imf_context = imf_context;

		ecore_imf_context_reset(imf_context);
		ecore_imf_context_input_panel_imdata_set(imf_context, (void *)"type=password", 13); /* for password read out with TTS */
		ecore_imf_context_input_panel_layout_set(imf_context, ECORE_IMF_INPUT_PANEL_LAYOUT_DATETIME);
		ecore_imf_context_input_panel_return_key_disabled_set(imf_context, EINA_TRUE);
		ecore_imf_context_focus_in(imf_context);
		ad->imf_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, __imf_event_key_down_cb, ad);
		if (!ad->imf_handler)
			SETTING_TRACE_DEBUG("handler is NULL");
	}
	SETTING_TRACE_END;
}

static void __enable_imf_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	__imf_show_for_simple_password(ad);
}

static int setting_password_simple_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	ad->sp_focused_num = 0;

	/* Create layout : layout/simple_password in setting-theme.edc */
	ad->simple_layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(ad->simple_layout, SETTING_THEME_EDJ_NAME, "layout/simple_password");

	ad->sp_entry1  = elm_entry_add(ad->simple_layout);
	elm_object_style_set(ad->sp_entry1 , "popup");
	elm_entry_single_line_set(ad->sp_entry1 , EINA_TRUE);
	elm_entry_editable_set(ad->sp_entry1 , EINA_FALSE);
	ad->sp_entry[0] = ad->sp_entry1;
	elm_object_part_content_set(ad->simple_layout, "entry1", ad->sp_entry1);

	ad->sp_entry2  = elm_entry_add(ad->simple_layout);
	elm_object_style_set(ad->sp_entry2 , "popup");
	elm_entry_single_line_set(ad->sp_entry2 , EINA_TRUE);
	elm_entry_editable_set(ad->sp_entry2 , EINA_FALSE);
	ad->sp_entry[1] = ad->sp_entry2;
	elm_object_part_content_set(ad->simple_layout, "entry2", ad->sp_entry2);

	ad->sp_entry3  = elm_entry_add(ad->simple_layout);
	elm_object_style_set(ad->sp_entry3 , "popup");
	elm_entry_single_line_set(ad->sp_entry3 , EINA_TRUE);
	elm_entry_editable_set(ad->sp_entry3 , EINA_FALSE);
	ad->sp_entry[2] = ad->sp_entry3;
	elm_object_part_content_set(ad->simple_layout, "entry3", ad->sp_entry3);

	ad->sp_entry4  = elm_entry_add(ad->simple_layout);
	elm_object_style_set(ad->sp_entry4 , "popup");
	elm_entry_single_line_set(ad->sp_entry4 , EINA_TRUE);
	elm_entry_editable_set(ad->sp_entry4 , EINA_FALSE);
	ad->sp_entry[3] = ad->sp_entry4;
	elm_object_part_content_set(ad->simple_layout, "entry4", ad->sp_entry4);

	if (ad->view_type == SETTING_PW_TYPE_SIMPLE_PASSWORD)
		elm_object_part_text_set(ad->simple_layout, "title.txt", _(sp_titles[3]));
	else
		elm_object_part_text_set(ad->simple_layout, "title.txt", _(sp_titles[1]));
	/* End */

	ad->ly_main = setting_create_layout_navi_bar(ad->win_main_layout, ad->win_get, "IDS_ST_BODY_SIMPLE_PASSWORD",
												 _("IDS_ST_BUTTON_BACK"),
												 (setting_call_back_func)setting_password_simple_click_softkey_cancel_cb,
												 ad,
												 ad->simple_layout,
												 &(ad->navi_bar), NULL);
	evas_object_event_callback_add(ad->sp_entry1, EVAS_CALLBACK_MOUSE_DOWN, __enable_imf_cb, ad);
	evas_object_event_callback_add(ad->sp_entry2, EVAS_CALLBACK_MOUSE_DOWN, __enable_imf_cb, ad);
	evas_object_event_callback_add(ad->sp_entry3, EVAS_CALLBACK_MOUSE_DOWN, __enable_imf_cb, ad);
	evas_object_event_callback_add(ad->sp_entry4, EVAS_CALLBACK_MOUSE_DOWN, __enable_imf_cb, ad);

	evas_object_show(ad->simple_layout);

	__imf_show_for_simple_password(ad);

	setting_view_password_simple.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_simple_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->imf_handler) {
		ecore_event_handler_del(ad->imf_handler);
		ad->imf_handler = NULL;
	}
	if (ad->imf_context) {
		ecore_imf_context_del(ad->imf_context);
		ad->imf_context = NULL;
	}

	if (ad->simple_done_timer) {
		ecore_timer_del(ad->simple_done_timer);
		ad->simple_done_timer = NULL;
	}

	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}

	setting_view_password_simple.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_simple_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_simple_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_hide(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

int setting_password_simple_draw_next_simple_password(void *data, int title_index)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			 "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	elm_object_part_text_set(ad->simple_layout, "title.txt", _(sp_titles[title_index]));
	elm_entry_entry_set(ad->sp_entry1, "");
	elm_entry_entry_set(ad->sp_entry2, "");
	elm_entry_entry_set(ad->sp_entry3, "");
	elm_entry_entry_set(ad->sp_entry4, "");

	memset(ad->sp_chars, 0x00, 5);
	ad->sp_focused_num = 0;

	return SETTING_RETURN_SUCCESS;
}

int setting_password_simple_check_simple_password(const char *collection)
{
	retv_if(collection == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	int sp_len = safeStrLen(collection);
	int index = 0;
	char temp;

	SETTING_TRACE_SECURE_DEBUG("collection length is %d", sp_len);

	if (sp_len != 4)
		return SETTING_RETURN_FAIL;

	/* check if digit. */
	for (index = 0; index < sp_len; index++) {
		temp = (char)(collection[index]);
		if ((temp < '0') || (temp > '9'))
			return SETTING_RETURN_FAIL;
		else
			SETTING_TRACE_SECURE_DEBUG("temp %c", temp);
	}

	return SETTING_RETURN_SUCCESS;
}
