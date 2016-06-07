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
#include <setting-common-data-slp-setting.h>
#include <setting-debug.h>
#include <dd-haptic.h>
#if SUPPORT_ENCRYPTION
#include <ode.h>
#endif
#include <Ecore_IMF.h>
#include <Ecore_IMF_Evas.h>
#include <Ecore.h>
#include <time.h>
#include <notification.h>
#include <dpm/password.h>

void __add_help_of_password_fails(int ret, SettingPasswordUG *ad);

static int setting_password_main_create(void *cb);
static int setting_password_main_destroy(void *cb);
static int setting_password_main_update(void *cb);
static int setting_password_main_cleanup(void *cb);

setting_view setting_view_password_main = {
		.create = setting_password_main_create,
		.destroy = setting_password_main_destroy,
		.update = setting_password_main_update,
		.cleanup = setting_password_main_cleanup, };

static void setting_password_main_done_password(void *data);
static int __count_string(const char *str, int *cnt_letter, int *cnt_digit,
		int *cnt_sym);

extern struct _pw_item pw_its[];

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/


int set_password_status_handler(SettingPasswordUG *ad, int status)
{
	if(!ad->caller_string || safeStrCmp(ad->caller_string,"DPM") != 0){
		SETTING_TRACE("Failed : caller is NOT DPM");
		return -1;
	}

	dpm_context_h handle;
	dpm_password_policy_h password_policy_handle;

	handle = dpm_context_create();
	if (handle == NULL) {
		SETTING_TRACE("Failed to create client handle");
		return -1;
	}

	password_policy_handle = dpm_context_acquire_password_policy(handle);
	if (password_policy_handle == NULL) {
		SETTING_TRACE("Failed to create password policy handle");
		return -1;
	}

	if (dpm_password_set_status(password_policy_handle, status) == 0) {
		dpm_context_release_password_policy(handle, password_policy_handle);
		dpm_context_destroy(handle);
		return 0;
	}

	dpm_context_release_password_policy(handle, password_policy_handle);
	dpm_context_destroy(handle);
	return -1;
}



static void setting_password_main_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(NULL == data,
			"[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/* if IME is displayed, hide IME. or Destroy password ug. */
	elm_object_focus_set(ad->ed_pw1->eo_check, EINA_FALSE);

	if (ad->focus_timer) {
		ecore_timer_del(ad->focus_timer);
		ad->focus_timer = NULL;
	}

	/* send result : Cancel */
	app_control_h svc;
	if (app_control_create(&svc) == 0) {
		app_control_add_extra_data(svc, "result", "Cancel");
		ug_send_result(ad->ug, svc);
		SETTING_TRACE("Send Result : %s\n", "Cancel");

		set_password_status_handler(ad, 2);  /* temporiry codes only for dpm */
		app_control_destroy(svc);
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

Eina_Bool __rotate_cb(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, FALSE);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/*SETTING_TRACE_DEBUG("category : 0x%x",
	 * pw_its[ad->view_type].category);*/

	/* it can manage the cursor self */
	if (ad->ed_pw1 && ad->ed_pw1->eo_check) {
		SETTING_TRACE_DEBUG("set focus on first entry");
		/* no matter how many edifileds, it only need focus on 1st
		 * editfiled */
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
		elm_entry_cursor_end_set(ad->ed_pw1->eo_check);
	}

	ad->focus_timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}

static int setting_password_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	ad->ly_main = setting_create_layout_navi_bar_genlist(
			ad->win_main_layout, ad->win_get,
			"IDS_ST_BODY_PASSWORD", _("IDS_ST_BUTTON_BACK"),
			NULL,
			(setting_call_back_func) setting_password_main_click_softkey_cancel_cb,
			NULL, ad, &(ad->scroller), &(ad->navi_bar));

	ad->navi_it = elm_naviframe_top_item_get(ad->navi_bar);

	ad->bottom_btn = elm_box_add(ad->navi_bar);
	elm_box_padding_set(ad->bottom_btn, ELM_SCALE_SIZE(10),
			ELM_SCALE_SIZE(10));
	elm_box_horizontal_set(ad->bottom_btn, EINA_TRUE);
	evas_object_size_hint_weight_set(ad->bottom_btn, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->bottom_btn, EVAS_HINT_FILL,
	EVAS_HINT_FILL);
	evas_object_show(ad->bottom_btn);
	elm_box_pack_end(ad->navi_bar, ad->bottom_btn);

	/* button 1 */
	Evas_Object *btn = elm_button_add(ad->bottom_btn);
	elm_object_style_set(btn, "bottom");
	elm_object_text_set(btn, _("IDS_ST_BUTTON_CANCEL"));
	evas_object_smart_callback_add(btn, "clicked",
			setting_password_main_click_softkey_cancel_cb, ad);
	evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
	evas_object_show(btn);
	elm_box_pack_end(ad->bottom_btn, btn);

	/* button 2 */
	ad->bottom_btn1 = elm_button_add(ad->bottom_btn);
	elm_object_style_set(ad->bottom_btn1, "bottom");
	if (ad->view_type == SETTING_PW_TYPE_SET_PASSWORD) {
		elm_object_text_set(ad->bottom_btn1,
				_("IDS_ST_BUTTON_CONTINUE"));
		evas_object_smart_callback_add(ad->bottom_btn1, "clicked",
				setting_password_main_click_softkey_continue_cb,
				ad);
	} else {
		elm_object_text_set(ad->bottom_btn1,
				_("IDS_SA_BUTTON_DONE_ABB"));
		evas_object_smart_callback_add(ad->bottom_btn1, "clicked",
				setting_password_main_click_softkey_done_cb,
				ad);
	}
	evas_object_size_hint_weight_set(ad->bottom_btn1, EVAS_HINT_EXPAND,
	EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(ad->bottom_btn1, EVAS_HINT_FILL, 0.5);
	evas_object_show(ad->bottom_btn1);
	elm_box_pack_end(ad->bottom_btn, ad->bottom_btn1);

	elm_object_item_part_content_set(ad->navi_it, "toolbar",
			ad->bottom_btn);

	ad->cur_step = 1;
	setting_password_main_draw_1line_password(ad, NULL);

	/* Disable Done button if exist */
	elm_object_disabled_set(ad->bottom_btn1, EINA_TRUE);
	elm_object_focus_allow_set(ad->bottom_btn1, EINA_FALSE);

	ad->focus_timer = ecore_timer_add(0.5, __rotate_cb, ad);

	setting_view_password_main.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	FREE(ad->step1_str);

	if (ad->remove_timer) {
		ecore_timer_del(ad->remove_timer);
		ad->remove_timer = NULL;
	}

	if (ad->focus_timer) {
		ecore_timer_del(ad->focus_timer);
		ad->focus_timer = NULL;
	}

	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}

	setting_view_password_main.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_main_update(void *cb)
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

static int setting_password_main_cleanup(void *cb)
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

static void setting_password_main_entry_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;

	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");
	retm_if(obj == NULL, "[Setting > Password] Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;
	const char *entry_str = elm_entry_entry_get(obj);
	list_item->sub_desc = (char *) g_strdup(entry_str);
	bool isFoundEmptyEntry = FALSE;

	do {
		if (ad->ed_pw1) {
			entry_str = ad->ed_pw1->sub_desc;
			if (NULL == entry_str || '\0' == entry_str[0]) {
				isFoundEmptyEntry = TRUE;
				elm_object_item_signal_emit(ad->ed_pw1->item,
						"elm,state,eraser,hide", "");
				break;
			} else {
				elm_object_item_signal_emit(ad->ed_pw1->item,
						"elm,state,eraser,show", "");
				break;
			}
		}
	} while (0);

	Eina_Bool item_disabled = elm_object_item_disabled_get(list_item->item);
	if (item_disabled)
		return;

	/* For disable toolbar (done) item */
	bool disableFlag = isFoundEmptyEntry;
	if (ad->ed_pw1 && ad->ed_pw1->eo_check
			&& safeStrLen(elm_entry_entry_get(
					ad->ed_pw1->eo_check)) < 4)
		disableFlag = TRUE;

#if SUPPORT_BOTTOM_BTNS
	if (disableFlag) {
		elm_object_disabled_set(ad->bottom_btn1, EINA_TRUE);
		elm_entry_input_panel_return_key_disabled_set(obj, EINA_TRUE);
	} else {
		elm_object_disabled_set(ad->bottom_btn1, EINA_FALSE);
		elm_entry_input_panel_return_key_disabled_set(obj, EINA_FALSE);
	}
#else
	elm_object_item_disabled_set(elm_toolbar_last_item_get(ad->controllbar),
			disableFlag);
#endif

}

static void __reached_max_pwlength_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	/*notify only when entry is being focused on. */
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");

	/* int err = 0; */

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;

	/* Display help text. */
	char temp[128] = { 0, };
	snprintf(temp, 128, _(PW_ST_PW_MAX_LENGTH_HELP),
			list_item->limit_filter_data->max_char_count);
	setting_password_ug_display_desc(ad, temp, FALSE);
}

static void __entry_activated_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;

	if (ad->bottom_btn) {
		if (elm_object_disabled_get(ad->bottom_btn1) == EINA_FALSE) {
			/* call done cb. */
			if (ad->view_type == SETTING_PW_TYPE_SET_PASSWORD
					&& ad->cur_step == 1) {
				/* Save first input */
				ad->step1_str = (char *) strdup(
						ad->ed_pw1->sub_desc);

				/* Check step1_str : more than 4, not all
				 * digits */
				int letter = 0;
				int digit = 0;
				int symbol = 0;
				__count_string(ad->step1_str, &letter, &digit,
						&symbol);
				if (safeStrLen(ad->step1_str) == digit) {
					__add_help_of_password_fails(
							SETTING_PW_ERROR_INCLUDE_NO_LETTER,
							ad);
					FREE(ad->step1_str);
					return;
				}

				setting_password_main_clear_1line_password(ad);
				if (ad->err_desc && ad->err_desc->item) {
					elm_object_item_del(ad->err_desc->item);
					ad->err_desc = NULL;
				}

				/* Change entry title :
				 * PW_SHORT_GUIDE_CONFIRM */
				FREE(ad->ed_pw1->keyStr);
				ad->ed_pw1->keyStr = (char *) strdup(
				PW_SHORT_GUIDE_CONFIRM);
				elm_genlist_item_fields_update(ad->ed_pw1->item,
						"elm.text.main",
						ELM_GENLIST_ITEM_FIELD_TEXT);

				/* Change button */
				elm_object_text_set(ad->bottom_btn1,
						_("IDS_SA_BUTTON_DONE_ABB"));
				evas_object_smart_callback_add(ad->bottom_btn1,
						"clicked",
						setting_password_main_click_softkey_done_cb,
						ad);
				elm_object_disabled_set(ad->bottom_btn1,
				EINA_TRUE);
				elm_entry_input_panel_return_key_disabled_set(
						obj,
						EINA_TRUE);
				ad->cur_step++;
			} else {
				setting_password_main_done_password(ad);
			}
		}
	}
}

static void __entry_focused_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	/* SettingPasswordUG *ad = (SettingPasswordUG *)list_item->userdata; */

	if (!elm_entry_is_empty(obj)) {
		elm_object_item_signal_emit(list_item->item,
				"elm,state,eraser,show", "");
	}
	elm_object_item_signal_emit(list_item->item, "elm,state,rename,hide",
			"");
}

static void __entry_unfocused_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(obj == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	if (list_item->item) {
		elm_object_item_signal_emit(list_item->item,
				"elm,state,eraser,hide", "");
		elm_object_item_signal_emit(list_item->item,
				"elm,state,rename,show", "");
	}
}

int setting_password_main_draw_1line_password(void *data, void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
	case SETTING_PW_TYPE_PASSWORD:
		ad->ed_pw1 = setting_create_Gendial_field_editfield(
				ad->scroller,
				&(itc_editfield),
				NULL,
				ad,
				SWALLOW_Type_LAYOUT_EDITFIELD,
				"IDS_ST_BODY_PASSWORD",
				NULL,
				setting_password_main_entry_changed_cb,
				__entry_focused_cb,
				__entry_unfocused_cb,
				__entry_activated_cb,
				__reached_max_pwlength_cb,
				ELM_INPUT_PANEL_LAYOUT_PASSWORD,
				TRUE,
				TRUE,
				SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH,
				0,
				NULL,
				NULL);
		/* change return key type */
		if (ad->ed_pw1) {
			ad->ed_pw1->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
			elm_entry_input_panel_return_key_type_set(
					ad->ed_pw1->eo_check,
					ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
			elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
		} else {
			SETTING_TRACE_ERROR("Cannot create editfield");
		}

		break;
	case SETTING_PW_TYPE_SET_PASSWORD:
		ad->ed_pw1 = setting_create_Gendial_field_editfield(
				ad->scroller,
				&(itc_editfield),
				NULL,
				ad,
				SWALLOW_Type_LAYOUT_EDITFIELD,
				PW_SHORT_GUIDE_NEW,
				NULL,
				setting_password_main_entry_changed_cb,
				__entry_focused_cb,
				__entry_unfocused_cb,
				__entry_activated_cb,
				__reached_max_pwlength_cb,
				ELM_INPUT_PANEL_LAYOUT_PASSWORD,
				TRUE,
				TRUE,
				SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH,
				0,
				NULL,
				NULL);

		ad->err_desc = setting_create_Gendial_field_helpitem_without_bottom_separator(
				ad->scroller, &(itc_multiline_text),
				SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
				PW_NORMAL_AND_CONTAIN_ALPHANUMER_DESC);

		/* change return key type */
		if (ad->ed_pw1) {
			ad->ed_pw1->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT;
			elm_entry_input_panel_return_key_type_set(
					ad->ed_pw1->eo_check,
					ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT);
			elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
		} else {
			SETTING_TRACE_ERROR("Cannot create editfield");
		}

		break;
	default:
		break;
	}

	ad->ed_pw2 = NULL;
	ad->ed_pw3 = NULL;

	return 0;
}

int setting_password_main_clear_1line_password(void *data)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
			"[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	retv_if(NULL == ad->ed_pw1, -1);

	ad->ed_pw1->sub_desc = NULL;
	ad->ed_pw1->isFocusFlag = FALSE;

	elm_entry_entry_set(ad->ed_pw1->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide",
			_("IDS_ST_BODY_ENTER_PASSWORD"));

	return 0;
}

/* This help is shown on entering new/confirm password view */
void __add_help_of_password_policy(SettingPasswordUG *ad)
{
	ret_if(ad == NULL);

	ad->err_desc = setting_create_Gendial_field_helpitem_without_bottom_separator(
			ad->scroller, &(itc_multiline_text),
			SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
			PW_NORMAL_AND_CONTAIN_ALPHANUMER_DESC);

	if (ad->ed_pw1 && ad->ed_pw1->eo_check) {
		SETTING_TRACE_DEBUG("FOCUS IN");
		ad->ed_pw1->isFocusFlag = TRUE;
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
	}
}

/* This help is shown after verifying entered new/confirm password with
 * constraint */
void __add_help_of_password_fails(int ret, SettingPasswordUG *ad)
{
	ret_if(ad == NULL);

	switch (ret) {
	case SETTING_PW_ERROR_INVALID_LENGTH:
		setting_password_ug_display_desc(
				ad,
				"Invalid length",
				FALSE);
		break;
	case SETTING_PW_ERROR_INCLUDE_NO_LETTER:
		setting_password_ug_display_desc(
				ad,
				"IDS_ST_BODY_PASSWORD_MUST_CONTAIN_AT_LEAST_1_LETTER",
				FALSE);
		break;
	case SETTING_PW_ERROR_NO_MATCH_WITH_POLICY:
		setting_password_ug_display_desc(
				ad,
				"No match with policy",
				FALSE);
		break;
	case SETTING_PW_ERROR_NO_MATCH_MIN_COMPLEX:
		setting_password_ug_display_desc(
				ad,
				"No match with min complex",
				FALSE);
		break;
	case SETTING_PW_ERROR_EXIST_FORBIDDEN_STRING:
		setting_password_ug_display_desc(
				ad,
				"Exist forbidden string",
				FALSE);
		break;
	case SETTING_PW_ERROR_NO_MATCH_MAX_REPEAT_COUNT:
		setting_password_ug_display_desc(
				ad,
				"No match with max repeat count",
				FALSE);
		break;
	case SETTING_PW_ERROR_SIMPLE_STYLE:
		setting_password_ug_display_desc(
				ad,
				"Exist simple sequence",
				FALSE);
		break;
	case SETTING_PW_ERROR_INCLUDE_DIGIT:
		setting_password_ug_display_desc(
				ad,
				"Password should not include any digit or symbol",
				FALSE);
		break;
	case SETTING_PW_ERROR_NO_MATCH_WITH_PATTERN:
		setting_password_ug_display_desc(
				ad,
				"No match with pattern",
				FALSE);
		break;
	case SETTING_PW_ERROR_NO_DIGIT:
		setting_password_ug_display_desc(
				ad,
				"IDS_ST_BODY_PASSWORD_MUST_CONTAIN_AT_LEAST_1_NUMBER",
				FALSE);
		break;
	default:
		setting_password_ug_display_desc(
				ad,
				PW_ERR_DESC,
				FALSE);
		break;
	}
}

int setting_password_main_check_1line_password(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/*const char *entry_str = ad->ed_pw1->sub_desc; */
	const char *entry_str = elm_entry_markup_to_utf8(ad->ed_pw1->sub_desc);
	/*char* plain_str = elm_entry_markup_to_utf8(ad->ed_pw1->sub_desc); */
	ad->focus_data = ad->ed_pw1;

	/*	Empty Check */
	if (isEmptyStr(entry_str)) {
		setting_password_ug_display_desc(ad,
				_("IDS_ST_BODY_PASSWORD_EMPTY"),
				FALSE);
		FREE(entry_str);
		return SETTING_ENTRY_RETURN_NULL_STR_ERR;
	}

	/*	Length Check */
	int entry_str_len = safeStrLen(entry_str);
	if (entry_str_len < SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH
			|| entry_str_len > SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH) {
		setting_password_main_warning_entry_added_byte_popup(
				ad,
				SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH,
				SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH);
		FREE(entry_str);
		return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
	}
	FREE(entry_str);
	return SETTING_RETURN_SUCCESS;
}

int setting_password_main_check_2line_password(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	if (ad->ed_pw1 == NULL)
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;

	const char *entry_str1 = ad->step1_str;
	const char *entry_str2 = ad->ed_pw1->sub_desc;

	/*SETTING_TRACE(" entry_str1 : %s ", entry_str1); */
	/*SETTING_TRACE(" entry_str2 : %s ", entry_str2); */

	/*char* plain_str = elm_entry_markup_to_utf8(str); */
	const char *entry_str1_mk = elm_entry_markup_to_utf8(entry_str1);
	const char *entry_str2_mk = elm_entry_markup_to_utf8(entry_str2);

	ad->focus_data = ad->ed_pw1;

	/*	Empty Check */
	if (isEmptyStr(entry_str1_mk)) {
		setting_password_ug_display_desc(
				ad,
				_("IDS_ST_BODY_PASSWORD_EMPTY"),
				FALSE);
		FREE(entry_str1_mk);
		FREE(entry_str2_mk);
		return SETTING_ENTRY_RETURN_NULL_STR_ERR;
	}

	/*	Match Check */
	if (0 != safeStrCmp(entry_str1_mk, entry_str2_mk)) {
		setting_password_main_clear_1line_password(ad);
		setting_password_ug_display_desc(
				ad,
				_("IDS_ST_POP_PASSWORDS_DO_NOT_MATCH"),
				FALSE);
		FREE(entry_str1_mk);
		FREE(entry_str2_mk);
		return SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW;
	}

	/*	Length Check */
	int entry_str_len = safeStrLen(entry_str1_mk);
	if (entry_str_len < SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH
			|| entry_str_len > SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH) {
		setting_password_main_warning_entry_added_byte_popup(
				ad,
				SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH,
				SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH);
		FREE(entry_str1_mk);
		FREE(entry_str2_mk);
		return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
	}

	FREE(entry_str1_mk);
	FREE(entry_str2_mk);
	return SETTING_RETURN_SUCCESS;
}

void setting_password_main_warning_entry_added_byte_popup(SettingPasswordUG *ad,
		int min, int max)
{
	char str[MAX_SPECIALIZITION_LEN + 1] = { 0 };

	snprintf(str, MAX_SPECIALIZITION_LEN,
			_("IDS_ST_POP_PASSWORD_MUST_BE_MORE_THAN_P1SD_CHARACTERS_AND_LESS_THAN_P2SD"),
			min, max);
	setting_password_ug_display_desc(ad, str, FALSE);
}

static void setting_password_main_done_password(void *data)
{
	ret_if(data == NULL);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	app_control_h svc;
	if (app_control_create(&svc)) {
		SETTING_TRACE_ERROR("app_control_create() failed");
		return;
	}

	SETTING_TRACE("ad->view_type:%d", ad->view_type);
	int ret = 0;

	switch (ad->view_type) {
	case SETTING_PW_TYPE_PASSWORD:
		ret = setting_password_main_check_1line_password(ad);

		if (ret == SETTING_RETURN_SUCCESS) {
			const char *entry_str;
			entry_str = ad->ed_pw1->sub_desc;
			/*SETTING_TRACE(" ----> entry_str : %s ", entry_str); */
			/*SETTING_TRACE(" ----> ad->view_type_string : %s ",
			 * ad->view_type_string); */
			unsigned int remain_attempt = ATTEMPT_INFINITE;
			unsigned int valid_seconds = 0;

			ret = setting_password_check_password(entry_str,
					&remain_attempt, &valid_seconds);

			if (ret == SETTING_RETURN_SUCCESS) {
				int err;

				if (safeStrCmp(ad->view_type_string,
						"SETTING_PW_TYPE_PASSWORD")
						== 0) {

					/*	reset VCONF */
					setting_set_int_slp_key(
							INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT,
							PHONE_LOCK_ATTEMPS_MAX,
							&err);
					if (err == SETTING_RETURN_FAIL) {
						SETTING_TRACE_ERROR(
								"[Error] set value of vconf fail.");
						app_control_destroy(svc);
						return;
					}
				} else if (0
						== safeStrCmp(
								ad->view_type_string,
								"SETTING_PW_TYPE_ENTER_LOCK_TYPE")) {
					app_control_add_extra_data(svc,
							"current", entry_str);
				}
			} else {
				/*	Incorrect Password. Display Popup.	 */
				setting_password_main_clear_1line_password(ad);
				ad->focus_data = ad->ed_pw1;
				/*else */
				/*{ */
				/*	setting_password_ug_check_attemps_left(ad); */
				/*	return; */
				/*} */
				setting_password_ug_display_desc(ad,
				PW_ERR_DESC, FALSE);
				app_control_destroy(svc);
				return;
			}
		} else {
			setting_password_main_clear_1line_password(ad);
			app_control_destroy(svc);
			return;
		}
		break;

	case SETTING_PW_TYPE_SET_PASSWORD: {
		ret = setting_password_main_check_2line_password(ad);

		if (ret == SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW) {
			/* Nothing */
			app_control_destroy(svc);
			setting_password_main_clear_1line_password(ad);
			setting_password_ug_display_desc(ad,
					PW_ST_PW_DO_NOT_MATCH, FALSE);
			return;

		} else if (ret != SETTING_RETURN_SUCCESS) {
			SETTING_TRACE("error : %d", ret);
			app_control_destroy(svc);
			setting_password_main_clear_1line_password(ad);
			setting_password_ug_display_desc(ad, PW_ERR_DESC,
					FALSE);
			return;
		}

		const char *entry_str;
		entry_str = ad->ed_pw1->sub_desc;
		if (isEmptyStr(entry_str)) {
			ad->focus_data = ad->ed_pw1;
			SETTING_TRACE_DEBUG(
					"%s*** [ERR] empty entry_str ***%s",
					SETTING_FONT_RED,
					SETTING_FONT_BLACK);
			setting_password_ug_create_popup_notitle_nobtn(
					ad, _("IDS_ST_POP_ERROR"),
					FALSE);

			app_control_destroy(svc);
			return;
		}

		/* Modify Setting Password */
		ret = setting_password_set_password(ad->cur_pwd,
				entry_str, ad);
		SETTING_TRACE("---> set password returns: %d", ret);
		if (ret == SETTING_PW_ERROR_REUSED) {
			ad->focus_data = ad->ed_pw1;
			setting_password_main_clear_1line_password(ad);
			setting_password_ug_display_desc(ad,
					_(PW_ERR_REUSED),
					FALSE);
			app_control_destroy(svc);
			return;

		} else if (ret <= 0) {
			ad->focus_data = ad->ed_pw1;
			setting_password_main_clear_1line_password(ad);
			setting_password_ug_display_desc(ad,
					_("IDS_BT_POP_OPERATION_FAILED"),
					FALSE);
			app_control_destroy(svc);
			return;
		}

		/*	Display popup */
		/*service_add_extra_data(svc, "current", entry_str); */
		if (ad->set_history_timer) {
			setting_password_ug_create_popup_notitle_nobtn(
					ad,
					_("IDS_ST_POP_NEW_PASSWD_SAVED"),
					FALSE);
		} else {
			app_control_add_extra_data(svc,
					"result",
					ad->view_type_string);
			ug_send_result(ad->ug, svc);
			SETTING_TRACE("Send Result : %s\n",
					ad->view_type_string);

			set_password_status_handler(ad, 1);  /* temporiry codes only for dpm */
			app_control_destroy(svc);
			/* Send destroy request */
			ug_destroy_me(ad->ug);
			return;
		}
		app_control_destroy(svc);
		return;
	}

	default:
		app_control_destroy(svc);
		return;
	}

	app_control_add_extra_data(svc, "result", ad->view_type_string);
	ug_send_result(ad->ug, svc);
	SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

	set_password_status_handler(ad, 1);  /* temporiry codes only for dpm */
	app_control_destroy(svc);
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

static int __count_string(const char *str, int *cnt_letter, int *cnt_digit,
		int *cnt_sym)
{
	SETTING_TRACE_BEGIN;

	retv_if(
			str == NULL || cnt_letter == NULL || cnt_digit == NULL || cnt_sym == NULL,
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*char* plain_str = elm_entry_markup_to_utf8(str); */
	char *plain_str = (char *) str;

	int length = strlen(plain_str);
	int index = 0;
	char temp = 0;
	int letter = 0;
	int digit = 0;
	int symbol = 0;

	if (length <= 0) {
		SETTING_TRACE_DEBUG("[DEBUG] invalid input length");
		return SETTING_RETURN_FAIL;
	}

	for (index = 0; index < length; index++) {
		temp = (char) (plain_str[index]);
		SETTING_TRACE_SECURE_DEBUG("temp %c", temp);

		if ((temp >= '0') && (temp <= '9')) { /* digit */
			digit++;
		} else if ((temp >= 'a') && (temp <= 'z')) { /* lower letter */
			letter++;
		} else if ((temp >= 'A') && (temp <= 'Z')) { /* upper letter */
			letter++;
		} else if ((temp >= ' ') && (temp <= '~')) {
			/* symbol means special char that enables to input from IME. */
			symbol++;
		}
	}
	SETTING_TRACE_SECURE_DEBUG(
			"%s includes %d letters, %d digits, and %d symbols",
			plain_str, letter, digit, symbol);
	*cnt_letter = letter;
	*cnt_digit = digit;
	*cnt_sym = symbol;
	return SETTING_RETURN_SUCCESS;
}

void setting_password_main_click_softkey_continue_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	/* error check */
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/* Save first input */
	ad->step1_str = (char *) strdup(ad->ed_pw1->sub_desc);

	/* Check step1_str : more than 4, not all digits */
	int letter = 0;
	int digit = 0;
	int symbol = 0;
	char *plain_str = elm_entry_markup_to_utf8(ad->step1_str);
	__count_string(plain_str, &letter, &digit, &symbol);

	if (safeStrLen(plain_str) == digit) {
		setting_password_main_clear_1line_password(ad);
		__add_help_of_password_fails(SETTING_PW_ERROR_INCLUDE_NO_LETTER,
				ad);
		FREE(ad->step1_str);
		FREE(plain_str);
		return;
	}

	setting_password_main_clear_1line_password(ad);
	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}

	/* Change entry title : PW_SHORT_GUIDE_CONFIRM */
	FREE(ad->ed_pw1->keyStr);
	ad->ed_pw1->keyStr = (char *) strdup(PW_SHORT_GUIDE_CONFIRM);
	elm_genlist_item_fields_update(ad->ed_pw1->item, "elm.text.main",
			ELM_GENLIST_ITEM_FIELD_TEXT);

	/* change return key type */
	ad->ed_pw1->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;
	elm_entry_input_panel_return_key_type_set(ad->ed_pw1->eo_check,
			ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE);
	elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);

	/* Change button */
	if (ad->bottom_btn) {
		ad->bottom_btn = elm_box_add(ad->navi_bar);
		elm_box_padding_set(ad->bottom_btn, ELM_SCALE_SIZE(10),
				ELM_SCALE_SIZE(10));
		elm_box_horizontal_set(ad->bottom_btn, EINA_TRUE);
		evas_object_size_hint_weight_set(ad->bottom_btn,
		EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(ad->bottom_btn, EVAS_HINT_FILL,
		EVAS_HINT_FILL);
		evas_object_show(ad->bottom_btn);
		elm_box_pack_end(ad->navi_bar, ad->bottom_btn);

		/* button 1 */
		Evas_Object *btn = elm_button_add(ad->bottom_btn);
		elm_object_style_set(btn, "bottom");
		elm_object_text_set(btn, _("IDS_ST_BUTTON_CANCEL"));
		evas_object_smart_callback_add(btn, "clicked",
				setting_password_main_click_softkey_cancel_cb,
				ad);
		evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
		evas_object_show(btn);
		elm_box_pack_end(ad->bottom_btn, btn);
		elm_object_disabled_set(btn, EINA_FALSE);

		/* button 2 */
		ad->bottom_btn1 = elm_button_add(ad->bottom_btn);
		elm_object_style_set(ad->bottom_btn1, "bottom");
		elm_object_text_set(ad->bottom_btn1,
				_("IDS_SA_BUTTON_DONE_ABB"));
		evas_object_smart_callback_del(ad->bottom_btn1, "clicked",
				setting_password_main_click_softkey_continue_cb);
		evas_object_smart_callback_add(ad->bottom_btn1, "clicked",
				setting_password_main_click_softkey_done_cb,
				ad);
		elm_object_disabled_set(ad->bottom_btn1, EINA_TRUE);
		evas_object_size_hint_weight_set(ad->bottom_btn1,
		EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(ad->bottom_btn1,
		EVAS_HINT_FILL, 0.5);
		evas_object_show(ad->bottom_btn1);
		elm_box_pack_end(ad->bottom_btn, ad->bottom_btn1);
		elm_object_item_part_content_set(ad->navi_it, "toolbar",
				ad->bottom_btn);
	}

	ad->cur_step++;
	FREE(plain_str);
}

void setting_password_main_click_softkey_done_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	/* error check */
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->ed_pw1 && ad->ed_pw1->eo_check) {
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_FALSE);
	}

	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}

	setting_password_main_done_password(ad);
}
