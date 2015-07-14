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
#include <setting-password-fingerprint.h>
#include <setting-common-data-slp-setting.h>
#include <setting-debug.h>
#include <dd-haptic.h>
#include <Ecore_IMF.h>
#include <Ecore_IMF_Evas.h>
#include <Ecore.h>
#include <fingerprint_manager.h>
#include <fingerprint_event.h>

static int setting_password_fingerprint_create(void *cb);
static int setting_password_fingerprint_destroy(void *cb);
static int setting_password_fingerprint_update(void *cb);
static int setting_password_fingerprint_cleanup(void *cb);

setting_view setting_view_password_fingerprint = {
	.create = setting_password_fingerprint_create,
	.destroy = setting_password_fingerprint_destroy,
	.update = setting_password_fingerprint_update,
	.cleanup = setting_password_fingerprint_cleanup,
};

static void setting_password_fingerprint_done(void *data);


/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static char *setting_password_fingerprint_get_title_str(void *data)
{
	retvm_if(data == NULL, NULL,
	         "[Setting > Password] Data parameter is NULL");
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
		case SETTING_PW_TYPE_SET_FP_ALT_PASSWORD:
			return "Create password";
			break;
		case SETTING_PW_TYPE_VERIFY_FP_ALT_PASSWORD:
			return "Confirm password";
			break;
		case SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD:
			return "Change password";
			break;
		default:
			SETTING_TRACE_DEBUG("%s*** Unknown Password Type. ***%s",
			                    SETTING_FONT_BGREEN, SETTING_FONT_BLACK);
			break;
	}
	return NULL;

}

static Eina_Bool setting_password_fingerprint_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/* if IME is displayed, hide IME. or Destroy password ug. */
	if (ad->ed_pw1 && ad->ed_pw1->isFocusFlag == TRUE && elm_object_focus_get(ad->ed_pw1->eo_check)) {
		SETTING_TRACE_DEBUG("wait to hide IME");
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_FALSE);
		return EINA_FALSE;
	}

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

		app_control_destroy(svc);
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);

	return EINA_TRUE;
}

static int __create_fingerprint_layout(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;

	/* CONTENT */
	if (ad->scroller != NULL) {
		evas_object_del(ad->scroller);
		ad->scroller = NULL;
	}

	/* PIN related views have 2 toolbar buttons. CANCEL and DONE button */
	ad->ly_main = setting_create_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get, setting_password_fingerprint_get_title_str(ad),
	                                                     _("IDS_ST_BUTTON_CANCEL_ABB"),
	                                                     _("IDS_SA_BUTTON_DONE_ABB"),
	                                                     (setting_call_back_func)setting_password_fingerprint_click_softkey_cancel_cb,
	                                                     setting_password_fingerprint_click_softkey_done_cb,
	                                                     ad,
	                                                     &(ad->scroller),
	                                                     &(ad->navi_bar));

	if (!ad->scroller) {
		SETTING_TRACE("genlist is NULL");
		return SETTING_RETURN_FAIL;
	}

	/* Disable Done button if exist */
	if (ad->navi_bar) {
		ad->navi_it = elm_naviframe_top_item_get(ad->navi_bar);
		Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it, "toolbar");
		if (toolbar) {
			elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
			evas_object_show(toolbar);
		}
		elm_object_item_signal_emit(ad->navi_it, "elm,state,sip,shown", "");
	}

	elm_genlist_mode_set(ad->scroller, ELM_LIST_COMPRESS);

	ADD_GL_SEPARATOR(ad->scroller)

	return SETTING_RETURN_SUCCESS;
}

static Eina_Bool __setting_password_fingerprint_rotate_cb(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, FALSE);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	/* it can manage the cursor self */
	if (ad->ed_pw1 && ad->ed_pw1->isFocusFlag) {
		if (ad->ed_pw1->eo_check) {
			SETTING_TRACE_DEBUG("set focus on first entry");
			/* no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw1->eo_check);
		}
	}

	ad->focus_timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}

static int setting_password_fingerprint_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;
	int ret = 0;

	ret = __create_fingerprint_layout(ad);

	if (ret < 0)
		return SETTING_RETURN_FAIL;

	ad->cur_step = 1;
	setting_password_fingerprint_draw_1line_entry(ad, NULL);

	/*ad->focus_timer = ecore_timer_add(0.5, __setting_password_fingerprint_rotate_cb, ad); */

	setting_view_password_fingerprint.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_fingerprint_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}
	FREE(ad->cur_pwd);

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		setting_view_password_fingerprint.is_create = 0;
	}
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_fingerprint_update(void *cb)
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

static int setting_password_fingerprint_cleanup(void *cb)
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

static void
setting_password_fingerprint_entry_changed_cb(void *data, Evas_Object *obj,
                                              void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");
	retm_if(obj == NULL, "[Setting > Password] Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;
	const char *entry_str = elm_entry_entry_get(obj);
	list_item->sub_desc = (char *)g_strdup(entry_str);
	bool isFoundEmptyEntry = FALSE;

	elm_object_part_text_set(obj, "elm.guide", "");

	do {
		if (ad->ed_pw1) {
			entry_str = ad->ed_pw1->sub_desc;
			if (NULL == entry_str || '\0' == entry_str[0]) {
				isFoundEmptyEntry = TRUE;
				elm_object_part_text_set(obj, "elm.guide", _("Enter password"));
				elm_object_item_signal_emit(ad->ed_pw1->item, "elm,state,eraser,hide", "");
				break;
			} else if (ad->ed_pw1->isFocusFlag) {
				elm_object_item_signal_emit(ad->ed_pw1->item, "elm,state,eraser,show", "");
				break;
			}
		}
	} while (0);

	Eina_Bool item_disabled = elm_object_item_disabled_get(list_item->item);
	if (item_disabled)
		return;

	/* For disable toolbar (done) item */
	bool disableFlag = isFoundEmptyEntry;
	if (ad->ed_pw1 && ad->ed_pw1->eo_check && (safeStrLen(elm_entry_entry_get(ad->ed_pw1->eo_check)) < 4))
		disableFlag = TRUE;

	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it, "toolbar");
	if (disableFlag) {
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_TRUE);
	} else {
		elm_object_item_disabled_set(elm_toolbar_last_item_get(toolbar), EINA_FALSE);
		if (ad->err_desc && ad->err_desc->item) {
			elm_object_item_del(ad->err_desc->item);
			ad->err_desc = NULL;
		}
	}
}

#if 0
static void __reached_max_pwlength_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");/*notify only when entry is being focused on. */

	int err = 0;

	/*Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data; */
	/*SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata; */

	int haptic_return = 0;
	haptic_device_h haptic_handle;
	haptic_return = haptic_open(HAPTIC_DEVICE_0, &haptic_handle);
	SETTING_TRACE("haptic_handle : %d", haptic_handle);
	if (haptic_return < 0) {
		SETTING_TRACE_DEBUG("%s*** [ERR] Failed haptic_open ***%s",
		                    SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	err = haptic_vibrate_file_with_detail(haptic_handle, "/usr/share/feedback/haptic/touch/touch.ivt", 1, HAPTIC_FEEDBACK_AUTO, HAPTIC_PRIORITY_MIN, NULL);

	if (err < 0)
		SETTING_TRACE("Failed to play haptic");

	haptic_close(haptic_handle);
}
#endif

static void __entry_activated_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;

	Evas_Object *toolbar = elm_object_item_part_content_get(ad->navi_it, "toolbar");
	if (toolbar) {
		Elm_Object_Item *done_item = elm_toolbar_last_item_get(toolbar);
		if (done_item) {
			if (elm_object_item_disabled_get(done_item) == EINA_FALSE) {
				elm_object_focus_set(obj, EINA_FALSE);
				/* call done cb. */
				setting_password_fingerprint_done(ad);
			}
		}
	}
}

static void __entry_focused_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *)list_item->userdata;

	if (!elm_entry_is_empty(obj)) {
		elm_object_item_signal_emit(list_item->item, "elm,state,eraser,show", "");
	}
	elm_object_item_signal_emit(list_item->item, "elm,state,rename,hide", "");

	if (ad == NULL)
		return;

	if (ad->ed_pw1 && obj == ad->ed_pw1->eo_check) {
		ad->ed_pw1->isFocusFlag = TRUE;
		if (ad->ed_pw2)
			ad->ed_pw2->isFocusFlag = FALSE;
		if (ad->ed_pw3)
			ad->ed_pw3->isFocusFlag = FALSE;
	}
}

static void __entry_unfocused_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(obj == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	list_item->isFocusFlag = FALSE;

	if (list_item->item) {
		elm_object_item_signal_emit(list_item->item, "elm,state,eraser,hide", "");
		elm_object_item_signal_emit(list_item->item, "elm,state,rename,show", "");
	}
}

static void __entry_max_length_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");/*notify only when entry is being focused on. */

	int err = 0;

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingPasswordUG *ad = (SettingPasswordUG *) list_item->userdata;

	/* Display help text. */
	char temp[128] = {0,};
	snprintf(temp, 128, _(PW_ST_PW_MAX_LENGTH_HELP), list_item->limit_filter_data->max_char_count);
	setting_password_ug_display_desc(ad, temp, FALSE);

	int haptic_return = 0;
	haptic_device_h haptic_handle;
	haptic_return = haptic_open(HAPTIC_DEVICE_0, &haptic_handle);
	SETTING_TRACE("haptic_handle : %d", haptic_handle);
	if (haptic_return < 0) {
		SETTING_TRACE_DEBUG("%s*** [ERR] Failed haptic_open ***%s",
		                    SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	err = haptic_vibrate_file_with_detail(haptic_handle, "/usr/share/feedback/haptic/touch/touch.ivt", 1, HAPTIC_FEEDBACK_AUTO, HAPTIC_PRIORITY_MIN, NULL);

	if (err < 0)
		SETTING_TRACE("Failed to play haptic");

	haptic_close(haptic_handle);
}

static char *__get_title_of_entry(int viewtype, int step)
{
	if ((viewtype == SETTING_PW_TYPE_SET_FP_ALT_PASSWORD && step == 1)
	    || (viewtype == SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD && step == 2)) {
		return "Enter password";
	} else {
		return "Confirm password";
	}
}

int setting_password_fingerprint_draw_1line_entry(void *data, void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->scroller)
		elm_genlist_clear(ad->scroller);

	ADD_GL_SEPARATOR(ad->scroller);

	ad->ed_pw1 = setting_create_Gendial_field_editfield(ad->scroller,
	                                                    &(itc_editfield),
	                                                    NULL, ad,
	                                                    SWALLOW_Type_LAYOUT_EDITFIELD,
	                                                    __get_title_of_entry(ad->view_type, ad->cur_step),
	                                                    NULL,
	                                                    setting_password_fingerprint_entry_changed_cb,
	                                                    __entry_focused_cb,
	                                                    __entry_unfocused_cb,
	                                                    __entry_activated_cb,
	                                                    __entry_max_length_cb,
	                                                    ELM_INPUT_PANEL_LAYOUT_PASSWORD,
	                                                    TRUE, TRUE,
	                                                    SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH,
	                                                    0, NULL, NULL);

	if (ad->view_type == SETTING_PW_TYPE_SET_FP_ALT_PASSWORD && ad->cur_step == 1)
		ADD_GL_LABLE(ad->scroller, "Enter a password to use as an alternative to your fingerprints.");

	ad->ed_pw2 = NULL;
	ad->ed_pw3 = NULL;
	/*ad->cur_pwd = NULL; */

	ad->focus_timer = ecore_timer_add(0.5, __setting_password_fingerprint_rotate_cb, ad);

	return 0;
}

int setting_password_fingerprint_clear_1line_entry(void *data)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	retv_if(NULL == ad->ed_pw1, -1);

	ad->ed_pw1->sub_desc = NULL;
	ad->ed_pw1->isFocusFlag = FALSE;

	elm_entry_entry_set(ad->ed_pw1->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide", _("Enter password"));

	elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);

	return 0;
}

int setting_password_fingerprint_check_1line_entry(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	const char *entry_str = ad->ed_pw1->sub_desc;
	ad->focus_data = ad->ed_pw1;

	/*  Length Check */
	int entry_str_len = safeStrLen(entry_str);
	switch (ad->view_type) {
		case SETTING_PW_TYPE_PIN_LOCK_ON:
		case SETTING_PW_TYPE_PIN_LOCK_OFF:
		case SETTING_PW_TYPE_CHANGE_PIN:
		case SETTING_PW_TYPE_CHANGE_PIN2: {
				if (entry_str_len < SETTING_PW_UG_PIN_MIN_LENGTH
				    || entry_str_len > SETTING_PW_UG_PIN_MAX_LENGTH) {
					setting_password_fingerprint_warning_entry_added_byte_popup
					(ad, SETTING_PW_UG_PIN_MIN_LENGTH, SETTING_PW_UG_PIN_MAX_LENGTH);
					return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
				}
			}
			break;
		case SETTING_PW_TYPE_SIM_LOCK_OFF: {
				if (entry_str_len < SETTING_PW_UG_SIM_MIN_LENGTH
				    || entry_str_len > SETTING_PW_UG_SIM_MAX_LENGTH) {
					setting_password_fingerprint_warning_entry_added_byte_popup
					(ad, SETTING_PW_UG_SIM_MIN_LENGTH, SETTING_PW_UG_SIM_MAX_LENGTH);
					return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
				}
			}
		default:
			break;
	}
	return SETTING_RETURN_SUCCESS;
}

void setting_password_fingerprint_warning_entry_added_byte_popup(SettingPasswordUG *ad,
                                                                 int min, int max)
{
	char str[MAX_SPECIALIZITION_LEN + 1] = { 0 };

	safeCopyStr(str, _("IDS_COM_BODY_TYPE_A_PIN_THAT_IS_4_TO_8_NUMBERS"), MAX_SPECIALIZITION_LEN);
	setting_password_ug_display_desc(ad, str, FALSE);
}

static void setting_password_fingerprint_display_desc(void *data, char *desc)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL || desc == NULL);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;

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
	if (ad->ed_pw1 && ad->ed_pw1->eo_check) {
		SETTING_TRACE_DEBUG("FOCUS IN");
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
	}
}

static void setting_password_fingerprint_done(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	SETTING_TRACE("ad->view_type:%d, ad->cur_step:%d", ad->view_type, ad->cur_step);

	char *input = ad->ed_pw1->sub_desc;

	/* case 1 : verify current */
	if (ad->view_type == SETTING_PW_TYPE_VERIFY_FP_ALT_PASSWORD
	    || (ad->view_type == SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD && ad->cur_step == 1)) {
		SETTING_TRACE_DEBUG("Verify");
		fpResult_t result = FINGERPRINT_RESULT_ERROR;
		result = fp_verify_password(input);
		setting_password_fingerprint_clear_1line_entry(ad);
		if (result != FINGERPRINT_RESULT_OK) {
			SETTING_TRACE_DEBUG("fp_verify_password() failed. %d", result);
			/* help text : enter again. etc */
			setting_password_fingerprint_display_desc(ad, "Verify failed.");
			return;
		}
		/* change entry's title */
		if (ad->view_type == SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD) {
			ad->cur_step++;
			setting_password_fingerprint_draw_1line_entry(ad, NULL);
			return;
		}
	}
	/* case 2 : enter new */
	else if ((ad->view_type == SETTING_PW_TYPE_SET_FP_ALT_PASSWORD && ad->cur_step == 1)
	         || (ad->view_type == SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD && ad->cur_step == 2)) {
		SETTING_TRACE_DEBUG("New");
		/* check */
		if (setting_password_fingerprint_check_1line_entry(ad) == SETTING_RETURN_SUCCESS) {
			ad->cur_pwd = (char *)strdup(ad->ed_pw1->sub_desc);
			SETTING_TRACE_DEBUG("input1 : %s", ad->cur_pwd);
			ad->cur_step++;
			setting_password_fingerprint_draw_1line_entry(ad, NULL);
			return;
		} else {
			setting_password_fingerprint_clear_1line_entry(ad);
			setting_password_fingerprint_display_desc(ad, "Input check failed.");
		}
	}
	/* case 3 : enter new again */
	else if ((ad->view_type == SETTING_PW_TYPE_SET_FP_ALT_PASSWORD && ad->cur_step == 2)
	         || (ad->view_type == SETTING_PW_TYPE_CHANGE_FP_ALT_PASSWORD && ad->cur_step == 3)) {
		SETTING_TRACE_DEBUG("Confirm New, input1 %s, input2 %s", ad->cur_pwd, input);
		/* check match with ad->cur_pwd */
		if (!safeStrCmp(ad->cur_pwd, input)) {
			fpResult_t result = FINGERPRINT_RESULT_ERROR;
			result = fp_set_password(input);
			setting_password_fingerprint_clear_1line_entry(ad);
			if (result != FINGERPRINT_RESULT_OK) {
				SETTING_TRACE_DEBUG("fp_set_password() failed. %d", result);
				setting_password_fingerprint_display_desc(ad, "Set failed.");
				return;
			}
			FREE(ad->cur_pwd);
		} else {
			setting_password_fingerprint_clear_1line_entry(ad);
			/* help text : new and confirm mismatched etc */
			setting_password_fingerprint_display_desc(ad, "New and Confirm do not mismatched.");

			return;
		}
	}

	/* send result to caller */
	app_control_h svc;
	if (app_control_create(&svc)) {
		SETTING_TRACE_ERROR("app_control_create() failed");
		return;
	}

	app_control_add_extra_data(svc, "result", ad->view_type_string);
	ug_send_result(ad->ug, svc);
	SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

	app_control_destroy(svc);
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

void setting_password_fingerprint_click_softkey_done_cb(void *data, Evas_Object *obj,
                                                        void *event_info)
{
	SETTING_TRACE_BEGIN;
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

	setting_password_fingerprint_done(ad);
}
