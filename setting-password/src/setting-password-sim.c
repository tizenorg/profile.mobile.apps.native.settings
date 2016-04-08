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
#include <setting-password-sim.h>
#include <setting-common-data-slp-setting.h>
#include <setting-debug.h>
#include <dd-haptic.h>
#include <Ecore_IMF.h>
#include <Ecore_IMF_Evas.h>
#include <Ecore.h>
#include <time.h>
#include <notification.h>

static int setting_password_sim_create(void *cb);
static int setting_password_sim_destroy(void *cb);
static int setting_password_sim_update(void *cb);
static int setting_password_sim_cleanup(void *cb);

setting_view setting_view_password_sim = {
	.create = setting_password_sim_create,
	.destroy = setting_password_sim_destroy,
	.update = setting_password_sim_update,
	.cleanup = setting_password_sim_cleanup,
};

static void setting_password_sim_done(void *data);


/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void
__password_sim_gl_mouse_up(void *data, Evas *e, Evas_Object *obj,
                           void *event)
{
	ret_if(!data);
	/*SettingPasswordUG *ad = (SettingPasswordUG *) data; */

	/* P131121-05156 : do not hide input panel when genlist scrolls,
						plz compare with [about device > device name] item */
}

static char *setting_password_sim_get_title_str(void *data)
{
	retvm_if(data == NULL, NULL,
	         "[Setting > Password] Data parameter is NULL");
	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
		case SETTING_PW_TYPE_SIM_LOCK_ON:
		case SETTING_PW_TYPE_SIM_LOCK_OFF:
			return "IDS_ST_BODY_PASSWORD";
			break;
		case SETTING_PW_TYPE_PIN_LOCK_ON:
		case SETTING_PW_TYPE_PIN_LOCK_OFF:
			return _("IDS_ST_BODY_PIN");
			break;
		case SETTING_PW_TYPE_CHANGE_PIN:
			return _("IDS_ST_HEADER_CHANGE_PIN_ABB");
			break;

		case SETTING_PW_TYPE_CHANGE_PIN2:
			return _("IDS_ST_BODY_CHANGE_PIN2");
			break;

		case SETTING_PW_TYPE_PIN_BLOCKED:
			return _("IDS_ST_BODY_PUK1_CODE");
			break;
		case SETTING_PW_TYPE_PIN2_BLOCKED:

			return _("IDS_ST_BODY_PUK2");
			break;
		default:
			SETTING_TRACE_DEBUG("%s*** Unknown Password Type. ***%s",
			                    SETTING_FONT_BGREEN, SETTING_FONT_BLACK);
			break;
	}
	return NULL;

}

static Eina_Bool setting_password_sim_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
#if 0
	/* if IME is displayed, hide IME. or Destroy password ug. */
	if (ad->ed_pw1 && ad->ed_pw1->isFocusFlag == TRUE && elm_object_focus_get(ad->ed_pw1->eo_check)) {
		SETTING_TRACE_DEBUG("wait to hide IME");
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_FALSE);
		return EINA_FALSE;
	} else if (ad->ed_pw2 && ad->ed_pw2->isFocusFlag == TRUE && elm_object_focus_get(ad->ed_pw2->eo_check)) {
		SETTING_TRACE_DEBUG("wait to hide IME");
		elm_object_focus_set(ad->ed_pw2->eo_check, EINA_FALSE);
		return EINA_FALSE;
	} else if (ad->ed_pw3 && ad->ed_pw3->isFocusFlag == TRUE && elm_object_focus_get(ad->ed_pw3->eo_check)) {
		SETTING_TRACE_DEBUG("wait to hide IME");
		elm_object_focus_set(ad->ed_pw3->eo_check, EINA_FALSE);
		return EINA_FALSE;
	}
#endif
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

static int __create_sim_layout(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;
	static int count = 0;

	count++;
	/* CONTENT */
	if (ad->scroller != NULL) {
		evas_object_del(ad->scroller);
		ad->scroller = NULL;
	}

	/* PIN related views have 2 toolbar buttons. CANCEL and DONE button */
	ad->ly_main = setting_create_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get, setting_password_sim_get_title_str(ad),
	                                                     _("IDS_ST_BUTTON_BACK"),
	                                                     NULL,
	                                                     (setting_call_back_func)setting_password_sim_click_softkey_cancel_cb,
	                                                     NULL,
	                                                     ad,
	                                                     &(ad->scroller),
	                                                     &(ad->navi_bar));

	ad->navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	ad->bottom_btn = elm_button_add(ad->navi_bar);

	Evas_Object *btn_cancel = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn_cancel, "naviframe/title_cancel");
	evas_object_smart_callback_add(btn_cancel, "clicked", setting_password_sim_click_softkey_cancel_cb, ad);
	elm_object_item_part_content_set(ad->navi_it, "title_left_btn", btn_cancel);

	Evas_Object *btn_done = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn_done, "naviframe/title_done");
	evas_object_smart_callback_add(btn_done, "clicked", setting_password_sim_click_softkey_done_cb, ad);
	elm_object_item_part_content_set(ad->navi_it, "title_right_btn", btn_done);

	if (!ad->scroller) {
		SETTING_TRACE("genlist is NULL");
		return SETTING_RETURN_FAIL;
	}

	/* Disable Done button if exist */
	elm_object_disabled_set(ad->bottom_btn, EINA_TRUE);
	elm_object_focus_allow_set(ad->bottom_btn, EINA_FALSE);
	elm_object_item_signal_emit(ad->navi_it, "elm,state,sip,shown", "");

	elm_genlist_mode_set(ad->scroller, ELM_LIST_COMPRESS);
	/*evas_object_smart_callback_add(ad->scroller, "drag", __password_main_gl_drag, ad); */
	evas_object_event_callback_add(ad->scroller, EVAS_CALLBACK_MOUSE_UP,
	                               __password_sim_gl_mouse_up, ad);

	return SETTING_RETURN_SUCCESS;
}

static Eina_Bool __setting_password_sim_rotate_cb(void *data)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, FALSE);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->view_type == SETTING_PW_TYPE_PIN_LOCK_OFF
	    || ad->view_type == SETTING_PW_TYPE_PIN_LOCK_ON
	    || ((ad->view_type == SETTING_PW_TYPE_CHANGE_PIN) && !ad->cur_pwd)
	    || ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED) {
		if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_SC, setting_get_pin_lock_info_cb, ad) != 0)
			SETTING_TRACE_DEBUG("failed to call tel_get_sim_lock_info()");
	} else if (((ad->view_type == SETTING_PW_TYPE_CHANGE_PIN2) && !ad->cur_pwd)
	           || ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
		if (tel_get_sim_lock_info(ad->handle, TAPI_SIM_LOCK_FD, setting_get_pin_lock_info_cb, ad) != 0)
			SETTING_TRACE_DEBUG("failed to call tel_get_sim_lock_info()");
	}

	/* it can manage the cursor self */
	if (ad->ed_pw1 && ad->ed_pw1->isFocusFlag) {
		if (ad->ed_pw1->eo_check) {
			SETTING_TRACE_DEBUG("set focus on first entry");
			/* no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw1->eo_check);
		}
	} else if (ad->ed_pw2 && ad->ed_pw2->isFocusFlag) {
		if (ad->ed_pw2->eo_check) {
			SETTING_TRACE_DEBUG("set focus on second entry");
			/*no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw2->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw2->eo_check);
		}
	} else if (ad->ed_pw3 && ad->ed_pw3->isFocusFlag) {
		if (ad->ed_pw3->eo_check) {
			SETTING_TRACE_DEBUG("set focus on third entry");
			/*no matter how many edifileds, it only need focus on 1st editfiled */
			elm_object_focus_set(ad->ed_pw3->eo_check, EINA_TRUE);
			elm_entry_cursor_end_set(ad->ed_pw3->eo_check);
		}
	}

	ad->focus_timer = NULL;

	return ECORE_CALLBACK_CANCEL;
}

static void __init_tapi(SettingPasswordUG *ad)
{
	ad->handle =  tel_init(NULL);
	if (!ad->handle) {
		SETTING_TRACE_DEBUG("%s*** [ERR] tel_init. ***%s",
		                    SETTING_FONT_RED, SETTING_FONT_BLACK);
		setting_password_ug_create_popup_notitle_nobtn(ad,
		                                               _("IDS_ST_POP_ERROR"),
		                                               FALSE);
	}
}

static void __deinit_tapi(SettingPasswordUG *ad)
{
	if (ad->handle) {
		if (tel_deinit(ad->handle) != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG("%s*** [ERR] setting_network_unsubscribe_tapi_events. ***%s",
			                    SETTING_FONT_RED, SETTING_FONT_BLACK);
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_POP_ERROR"),
			                                               FALSE);
		}
		ad->handle = NULL;
	}
}

static int setting_password_sim_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;
	int ret = 0;

	ret = __create_sim_layout(ad);

	if (ret < 0)
		return SETTING_RETURN_FAIL;

	__init_tapi(ad);

	setting_password_sim_create_ui(ad);

	ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);

	setting_view_password_sim.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_sim_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	__deinit_tapi(ad);

	if (ad->remove_timer) {
		ecore_timer_del(ad->remove_timer);
		ad->remove_timer = NULL;
	}
	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}
	/*FREE(ad->guide_str); */

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		setting_view_password_sim.is_create = 0;
	}
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_sim_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}
	if (ad->puk_verified_flag == TRUE && ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED) {
		ad->puk_verified_flag = FALSE;
		SETTING_TRACE_DEBUG("view type : PIN1_BLOCKED, change view");
		if (ad->scroller) {
			elm_genlist_clear(ad->scroller);
			if (ad->cur_pwd) {
				setting_password_sim_draw_2line_entry(ad, NULL);
			} else {
				setting_password_sim_draw_1line_entry(ad, NULL);
				ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);
			}
		}
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_sim_cleanup(void *cb)
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

int setting_password_sim_create_ui(void *data)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	SETTING_TRACE_DEBUG("ad->view_type=%d ***", ad->view_type);

	switch (ad->view_type) {
		case SETTING_PW_TYPE_SIM_LOCK_OFF:
		case SETTING_PW_TYPE_PIN_LOCK_ON:
		case SETTING_PW_TYPE_PIN_LOCK_OFF:
		case SETTING_PW_TYPE_PIN_BLOCKED:
		case SETTING_PW_TYPE_CHANGE_PIN:
		case SETTING_PW_TYPE_CHANGE_PIN2:
			setting_password_sim_draw_1line_entry(ad, NULL);
			break;
		case SETTING_PW_TYPE_SIM_LOCK_ON:
			setting_password_sim_draw_2line_entry(ad, NULL);
			break;
		case SETTING_PW_TYPE_PIN2_BLOCKED:
			setting_password_sim_draw_3line_entry(ad, NULL);
			break;
		default:
			SETTING_TRACE_DEBUG("%s*** Unknown Password Type. ***%s",
			                    SETTING_FONT_BGREEN, SETTING_FONT_BLACK);
			break;
	}

	/** @todo this function always return 0, not verifying the function flow with the value */
	return 0;
}

static void
setting_password_sim_entry_changed_cb(void *data, Evas_Object *obj,
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
				elm_object_part_text_set(obj, "elm.guide", _("IDS_ST_HEADER_ENTER_PIN_ABB3"));
				elm_object_item_signal_emit(ad->ed_pw1->item, "elm,state,eraser,hide", "");
				break;
			} else if (ad->ed_pw1->isFocusFlag) {
				elm_object_item_signal_emit(ad->ed_pw1->item, "elm,state,eraser,show", "");
				break;
			}
		}
		if (ad->ed_pw2) {
			entry_str = ad->ed_pw2->sub_desc;
			if (NULL == entry_str || '\0' == entry_str[0]) {
				isFoundEmptyEntry = TRUE;
				elm_object_part_text_set(obj, "elm.guide", _("IDS_ST_HEADER_ENTER_PIN_ABB3"));
				elm_object_item_signal_emit(ad->ed_pw2->item, "elm,state,eraser,hide", "");
				break;
			} else if (ad->ed_pw2->isFocusFlag) {
				elm_object_item_signal_emit(ad->ed_pw2->item, "elm,state,eraser,show", "");
				break;
			}
		}
		if (ad->ed_pw3) {
			entry_str = ad->ed_pw3->sub_desc;
			if (NULL == entry_str || '\0' == entry_str[0]) {
				isFoundEmptyEntry = TRUE;
				elm_object_part_text_set(obj, "elm.guide", _("IDS_ST_HEADER_ENTER_PIN_ABB3"));
				elm_object_item_signal_emit(ad->ed_pw3->item, "elm,state,eraser,hide", "");
				break;
			} else if (ad->ed_pw3->isFocusFlag) {
				elm_object_item_signal_emit(ad->ed_pw3->item, "elm,state,eraser,show", "");
				break;
			}
		}

	} while (0);

	Eina_Bool item_disabled = elm_object_item_disabled_get(list_item->item);
	if (item_disabled)
		return;

	/* For disable toolbar (done) item */
	bool disableFlag = isFoundEmptyEntry;
	if (ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED
	    || ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
		if (ad->ed_pw1) {
			if (!ad->cur_pwd && ad->ed_pw1->eo_check && (safeStrLen(elm_entry_entry_get(ad->ed_pw1->eo_check)) < 8)) {
				SETTING_TRACE_DEBUG("disable flag becomes TRUE");
				disableFlag = TRUE;
			}
		}
	} else {
		if (ad->ed_pw1 && ad->ed_pw1->eo_check && (safeStrLen(elm_entry_entry_get(ad->ed_pw1->eo_check)) < 4))
			disableFlag = TRUE;
		if (ad->ed_pw2 && ad->ed_pw2->eo_check && (safeStrLen(elm_entry_entry_get(ad->ed_pw2->eo_check)) < 4))
			disableFlag = TRUE;
		if (ad->ed_pw3 && ad->ed_pw3->eo_check && (safeStrLen(elm_entry_entry_get(ad->ed_pw3->eo_check)) < 4))
			disableFlag = TRUE;
	}

	if (disableFlag) {
		elm_object_disabled_set(ad->bottom_btn, EINA_TRUE);
	} else {
		elm_object_disabled_set(ad->bottom_btn, EINA_FALSE);
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

	if (elm_object_disabled_get(ad->bottom_btn) == EINA_FALSE) {
		elm_object_focus_set(obj, EINA_FALSE);
		/* call done cb. */
		setting_password_sim_done(ad);
	}

	if (obj == ad->ed_pw1->eo_check) {
		if (ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED
		    || ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
			/* PUK should be 8 chars always */
			if (safeStrLen(elm_entry_entry_get(ad->ed_pw1->eo_check)) < SETTING_PW_UG_PUK_MAX_LENGTH) {
				SETTING_TRACE_DEBUG("do not move focus to next entry.");
				ad->ed_pw1->isFocusFlag = TRUE;
				elm_object_focus_set(ad->ed_pw1->eo_check, EINA_TRUE);
				return;
			}
		}
		if (ad->ed_pw2 && ad->ed_pw2->eo_check) {
			elm_object_focus_set(ad->ed_pw2->eo_check, EINA_TRUE);
			ad->ed_pw2->isFocusFlag = TRUE;
		}
	} else if (obj == ad->ed_pw2->eo_check) {
		if (ad->ed_pw3 && ad->ed_pw3->eo_check) {
			elm_object_focus_set(ad->ed_pw3->eo_check, EINA_TRUE);
			ad->ed_pw3->isFocusFlag = TRUE;
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
	} else if (ad->ed_pw2 && obj == ad->ed_pw2->eo_check) {
		ad->ed_pw2->isFocusFlag = TRUE;
		if (ad->ed_pw1)
			ad->ed_pw1->isFocusFlag = FALSE;
		if (ad->ed_pw3)
			ad->ed_pw3->isFocusFlag = FALSE;
	} else if (ad->ed_pw3 && obj == ad->ed_pw3->eo_check) {
		ad->ed_pw3->isFocusFlag = TRUE;
		if (ad->ed_pw1)
			ad->ed_pw1->isFocusFlag = FALSE;
		if (ad->ed_pw2)
			ad->ed_pw2->isFocusFlag = FALSE;
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

int setting_password_sim_draw_1line_entry(void *data, void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
		case SETTING_PW_TYPE_SIM_LOCK_OFF:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_SIM_PASSWORD",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           NULL,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_SIM_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			break;

		case SETTING_PW_TYPE_PIN_LOCK_ON:
		case SETTING_PW_TYPE_PIN_LOCK_OFF:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_PIN", NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			if (ad->ed_pw1)
				ad->ed_pw1->guide_text = (char *)g_strdup(_("IDS_ST_HEADER_ENTER_PIN_ABB3"));
			break;
		case SETTING_PW_TYPE_PIN_BLOCKED:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_PUK1_CODE",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PUK_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			break;
		case SETTING_PW_TYPE_CHANGE_PIN:
		case SETTING_PW_TYPE_CHANGE_PIN2:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           PW_SHORT_GUIDE_CURRENT,
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			if (ad->ed_pw1)
				ad->ed_pw1->guide_text = (char *)g_strdup(_("IDS_ST_HEADER_ENTER_PIN_ABB3"));
			break;
		default:
			break;
	}

	ad->ed_pw2 = NULL;
	ad->ed_pw3 = NULL;
	ad->cur_pwd = NULL;

	return 0;
}

int setting_password_sim_clear_1line_entry(void *data)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	retv_if(NULL == ad->ed_pw1, -1);

	ad->ed_pw1->sub_desc = NULL;
	ad->ed_pw1->isFocusFlag = FALSE;

	elm_entry_entry_set(ad->ed_pw1->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide", _("Enter PIN"));

	return 0;
}

int setting_password_sim_draw_2line_entry(void *data, void *cb)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
		case SETTING_PW_TYPE_SIM_LOCK_ON:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           PW_SHORT_GUIDE_NEW,
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_SIM_MAX_LENGTH,
			                                           0, "0123456789", NULL);

			ad->ed_pw2 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           PW_SHORT_GUIDE_CONFIRM,
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           NULL,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_SIM_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			break;
		case SETTING_PW_TYPE_PIN_BLOCKED:
		case SETTING_PW_TYPE_CHANGE_PIN:
		case SETTING_PW_TYPE_CHANGE_PIN2:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           PW_SHORT_GUIDE_NEW,
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			if (ad->ed_pw1)
				ad->ed_pw1->guide_text = (char *)g_strdup(_("IDS_ST_HEADER_ENTER_PIN_ABB3"));

			ad->ed_pw2 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           PW_SHORT_GUIDE_CONFIRM,
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			if (ad->ed_pw2)
				ad->ed_pw2->guide_text = (char *)g_strdup(_("IDS_ST_HEADER_ENTER_PIN_ABB3"));
			break;
		default:
			break;
	}

	ad->ed_pw3 = NULL;
	return 0;
}

int setting_password_sim_clear_2line_entry(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	retv_if(NULL == ad->ed_pw1, -1);
	ad->ed_pw1->sub_desc = NULL;
	ad->ed_pw1->isFocusFlag = FALSE;
	elm_entry_entry_set(ad->ed_pw1->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide", _("Enter PIN"));

	retv_if(NULL == ad->ed_pw2, -1);
	ad->ed_pw2->sub_desc = NULL;
	ad->ed_pw2->isFocusFlag = FALSE;
	elm_entry_entry_set(ad->ed_pw2->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide", _("Enter PIN"));
	return 0;
}

int setting_password_sim_draw_3line_entry(void *data, void *cb)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	switch (ad->view_type) {
		case SETTING_PW_TYPE_PIN_BLOCKED:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_PUK1_CODE",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PUK_MAX_LENGTH,
			                                           0, "0123456789", NULL);

			ad->ed_pw2 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_NEW_PIN1",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);

			ad->ed_pw3 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_CONFIRM_NEW_PIN1",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           NULL,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			break;
		case SETTING_PW_TYPE_PIN2_BLOCKED:
			ad->ed_pw1 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_PUK2", NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, TRUE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);

			ad->ed_pw2 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_NEW_PIN2",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           __entry_activated_cb,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);

			ad->ed_pw3 =
			    setting_create_Gendial_field_editfield(ad->scroller,
			                                           &(itc_editfield),
			                                           NULL, ad,
			                                           SWALLOW_Type_LAYOUT_EDITFIELD,
			                                           "IDS_ST_BODY_CONFIRM_NEW_PIN2",
			                                           NULL,
			                                           setting_password_sim_entry_changed_cb,
			                                           __entry_focused_cb,
			                                           __entry_unfocused_cb,
			                                           NULL,
			                                           NULL,
			                                           ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
			                                           TRUE, FALSE,
			                                           SETTING_PW_UG_PIN_MAX_LENGTH,
			                                           0, "0123456789", NULL);
			break;

		default:
			break;
	}
	return 0;
}

int setting_password_sim_clear_3line_entry(void *data)
{
	/* error check */
	retvm_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
	         "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	/*Evas_Object *layout = NULL; */

	retv_if(NULL == ad->ed_pw1, -1);
	ad->ed_pw1->sub_desc = NULL;
	ad->ed_pw1->isFocusFlag = FALSE;
	elm_entry_entry_set(ad->ed_pw1->eo_check, "");
	elm_object_part_text_set(ad->ed_pw1->eo_check, "elm.guide", _("Enter PUK"));

	retv_if(NULL == ad->ed_pw2, -1);
	ad->ed_pw2->sub_desc = NULL;
	ad->ed_pw2->isFocusFlag = FALSE;
	elm_entry_entry_set(ad->ed_pw2->eo_check, "");
	elm_object_part_text_set(ad->ed_pw2->eo_check, "elm.guide", _("Enter PIN"));

	retv_if(NULL == ad->ed_pw3, -1);
	ad->ed_pw3->sub_desc = NULL;
	ad->ed_pw3->isFocusFlag = FALSE;
	elm_entry_entry_set(ad->ed_pw3->eo_check, "");
	elm_object_part_text_set(ad->ed_pw3->eo_check, "elm.guide", _("Enter PIN"));

	return 0;
}

int setting_password_sim_check_1line_entry(void *data)
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
					setting_password_sim_warning_entry_added_byte_popup
					(ad, SETTING_PW_UG_PIN_MIN_LENGTH, SETTING_PW_UG_PIN_MAX_LENGTH);
					return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
				}
			}
			break;
		case SETTING_PW_TYPE_SIM_LOCK_OFF: {
				if (entry_str_len < SETTING_PW_UG_SIM_MIN_LENGTH
				    || entry_str_len > SETTING_PW_UG_SIM_MAX_LENGTH) {
					setting_password_sim_warning_entry_added_byte_popup
					(ad, SETTING_PW_UG_SIM_MIN_LENGTH, SETTING_PW_UG_SIM_MAX_LENGTH);
					return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
				}
			}
		default:
			break;
	}
	return SETTING_RETURN_SUCCESS;
}

int setting_password_sim_check_2line_entry(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;
	if (ad->ed_pw1 == NULL || ad->ed_pw2 == NULL)
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;

	const char *entry_str1 = ad->ed_pw1->sub_desc;
	const char *entry_str2 = ad->ed_pw2->sub_desc;
	ad->focus_data = ad->ed_pw1;

	/*  Match Check */
	if (0 != safeStrCmp(entry_str1, entry_str2)) {
		setting_password_sim_clear_2line_entry(ad);
		if (ad->view_type == SETTING_PW_TYPE_PIN_BLOCKED
		    || ad->view_type == SETTING_PW_TYPE_CHANGE_PIN
		    || ad->view_type == SETTING_PW_TYPE_PIN2_BLOCKED) {
			setting_password_ug_display_desc(ad, _("IDS_IDLE_BODY_NEW_PIN_AND_CONFIRM_PIN_DO_NOT_MATCH"), FALSE);
		} else if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN2) {
			setting_password_ug_display_desc(ad, _("IDS_ST_BODY_NEW_PIN2_AND_CONFIRM_PIN2_DO_NOT_MATCH"), FALSE);
		}

		return SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW;
	}

#if SUPPORT_SIMLOCK
	/*  Length Check */
	int entry_str_len = safeStrLen(entry_str1);
	if (ad->view_type == SETTING_PW_TYPE_SIM_LOCK_ON) {
		if (entry_str_len < SETTING_PW_UG_SIM_MIN_LENGTH
		    || entry_str_len > SETTING_PW_UG_SIM_MAX_LENGTH) {
			setting_password_sim_warning_entry_added_byte_popup
			(ad, SETTING_PW_UG_SIM_MIN_LENGTH, SETTING_PW_UG_SIM_MAX_LENGTH);
			return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
		}
	}
#endif

	return SETTING_RETURN_SUCCESS;
}

void setting_password_sim_warning_entry_added_byte_popup(SettingPasswordUG *ad,
                                                         int min, int max)
{
	char str[MAX_SPECIALIZITION_LEN + 1] = { 0 };

	safeCopyStr(str, _("IDS_COM_BODY_TYPE_A_PIN_THAT_IS_4_TO_8_NUMBERS"), MAX_SPECIALIZITION_LEN);
	setting_password_ug_display_desc(ad, str, FALSE);
}

int setting_password_sim_check_3line_entry(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	const char *entry_str1 = ad->ed_pw1->sub_desc;
	const char *entry_str2 = ad->ed_pw2->sub_desc;
	const char *entry_str3 = ad->ed_pw3->sub_desc;
	ad->focus_data = ad->ed_pw1;

	/*  Length Check */
	int entry_str1_len = safeStrLen(entry_str1);
	int entry_str2_len = safeStrLen(entry_str2);
	int entry_str3_len = safeStrLen(entry_str3);

	/* PUK code always should be 8 chars. */
	if (entry_str1_len != SETTING_PW_UG_PUK_MAX_LENGTH) {
		setting_password_sim_warning_entry_added_byte_popup
		(ad, SETTING_PW_UG_PUK_MAX_LENGTH, SETTING_PW_UG_PUK_MAX_LENGTH);
		return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
	}
	if (entry_str2_len < SETTING_PW_UG_PIN_MIN_LENGTH
	    || entry_str2_len > SETTING_PW_UG_PIN_MAX_LENGTH) {
		ad->focus_data = ad->ed_pw2;
		setting_password_sim_warning_entry_added_byte_popup
		(ad, SETTING_PW_UG_PIN_MIN_LENGTH, SETTING_PW_UG_PIN_MAX_LENGTH);
		return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
	}
	if (entry_str3_len < SETTING_PW_UG_PIN_MIN_LENGTH
	    || entry_str3_len > SETTING_PW_UG_PIN_MAX_LENGTH) {
		ad->focus_data = ad->ed_pw3;
		setting_password_sim_warning_entry_added_byte_popup
		(ad, SETTING_PW_UG_PIN_MIN_LENGTH, SETTING_PW_UG_PIN_MAX_LENGTH);
		return SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW;
	}

	return SETTING_RETURN_SUCCESS;
}

static void setting_password_sim_done(void *data)
{
	SETTING_TRACE_BEGIN;
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
		case SETTING_PW_TYPE_PIN_LOCK_ON: {
				SETTING_TRACE("case SETTING_PW_TYPE_PIN_LOCK_ON");

				ret = setting_password_sim_check_1line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					TelSimFacilityPw_t sim_sec_data = { 0, };
					int tapi_ret = TAPI_API_SUCCESS;
					const char *entry_str;

					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] elm_entry_entry_get(ad->ed_pw1) return NULL!");
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					sim_sec_data.lock_type = TAPI_SIM_LOCK_SC;/*TAPI_SIM_PTYPE_PIN1 */
					sim_sec_data.pw = (unsigned char *)entry_str;
					sim_sec_data.pw_len = safeStrLen(entry_str);

					tapi_ret = tel_enable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_enable_cb, ad);
					setting_password_sim_clear_1line_entry(ad);

					if (TAPI_API_SUCCESS != tapi_ret) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_DEBUG("%s*** [ERR] tel_disable_sim_facility err=%d ***%s",
						                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
					} else {
						/*  Success. Send Message to Caller for Modify Pin Lock slider status                                           */
					}
				} else {
					setting_password_sim_clear_1line_entry(ad);
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_PIN_LOCK_OFF: {
				SETTING_TRACE("case SETTING_PW_TYPE_PIN_LOCK_OFF");

				ret = setting_password_sim_check_1line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					TelSimFacilityPw_t sim_sec_data = { 0, };
					int tapi_ret = TAPI_API_SUCCESS;
					const char *entry_str;

					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] elm_entry_entry_get(ad->ed_pw1) return NULL!");
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					sim_sec_data.lock_type = TAPI_SIM_LOCK_SC;/*TAPI_SIM_PTYPE_PIN1 */
					sim_sec_data.pw = (unsigned char *)entry_str;
					sim_sec_data.pw_len = safeStrLen(entry_str);

					tapi_ret = tel_disable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_disable_cb, ad);
					setting_password_sim_clear_1line_entry(ad);

					if (TAPI_API_SUCCESS != tapi_ret) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_DEBUG("%s*** [ERR] tel_disable_sim_facility err=%d ***%s",
						                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
					} else {
						/*  Success. Send Message to Caller for Modify Pin Lock slider status                                           */
					}
				} else {
					setting_password_sim_clear_1line_entry(ad);
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_PIN_BLOCKED: {
				/*  PIN1 Status is "Blocked": User should input puk code and new pin1 code. */
				/*  If puk code is correct and success to modify pin1 code, display pin1 code change popup */
				/*  But, if this menu is "Pin Lock", pinlock status will not be changed. */
				SETTING_TRACE("case SETTING_PW_TYPE_PIN_BLOCKED");
				if (ad->cur_pwd == NULL) {
					ret = setting_password_sim_check_1line_entry(ad);
					if (ret == SETTING_RETURN_SUCCESS) {
						ad->cur_pwd = (char *)strdup(ad->ed_pw1->sub_desc);
						elm_genlist_clear(ad->scroller);
						setting_password_sim_draw_2line_entry(ad, NULL);
					} else {
						setting_password_sim_clear_1line_entry(ad);
					}
				} else {
					ret = setting_password_sim_check_2line_entry(ad);

					if (ret == SETTING_RETURN_SUCCESS) {
						TelSimSecPw_t puk1_data = { 0, };
						TelSimSecPw_t pin1_data = { 0, };
						int tapi_ret = TAPI_API_SUCCESS;
						const char *puk;
						const char *new_pin1;

						puk = ad->cur_pwd;
						new_pin1 = ad->ed_pw1->sub_desc;

						puk1_data.type = TAPI_SIM_PTYPE_PUK1;
						puk1_data.pw = (unsigned char *)puk;
						puk1_data.pw_len = safeStrLen(puk);

						pin1_data.type = TAPI_SIM_PTYPE_PIN1;
						pin1_data.pw = (unsigned char *)new_pin1;
						pin1_data.pw_len = safeStrLen(new_pin1);

						tapi_ret = tel_verify_sim_puks(ad->handle, &puk1_data, &pin1_data, setting_sim_verify_puks_cb, ad);
						FREE(ad->cur_pwd);
						ad->cur_pwd = NULL;

						setting_password_sim_clear_2line_entry(ad);

						if (TAPI_API_SUCCESS != tapi_ret) {
							ad->focus_data = ad->ed_pw1;
							SETTING_TRACE_DEBUG("%s*** [ERR] tel_verify_sim_puks err=%d ***%s",
							                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
							setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						} else {
							/*  Success. Send Message to Caller for Modify Pin Lock slider status */

						}
					} else {
						setting_password_sim_clear_2line_entry(ad);
					}
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_PIN2_BLOCKED: {
				SETTING_TRACE("case SETTING_PW_TYPE_PIN2_BLOCKED");
				ret = setting_password_sim_check_3line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					TelSimSecPw_t puk2_data = { 0, };
					TelSimSecPw_t pin2_data = { 0, };
					int tapi_ret = TAPI_API_SUCCESS;
					const char *puk;
					const char *new_pin2;

					puk = ad->ed_pw1->sub_desc;
					new_pin2 = ad->ed_pw2->sub_desc;
					if (isEmptyStr(puk)) {
						ad->focus_data = ad->ed_pw1;
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					if (isEmptyStr(new_pin2)) {
						ad->focus_data = ad->ed_pw2;
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					puk2_data.type = TAPI_SIM_PTYPE_PUK2;
					puk2_data.pw = (unsigned char *)puk;
					puk2_data.pw_len = safeStrLen(puk);

					pin2_data.type = TAPI_SIM_PTYPE_PIN2;
					pin2_data.pw = (unsigned char *)new_pin2;
					pin2_data.pw_len = safeStrLen(new_pin2);

					tapi_ret = tel_verify_sim_puks(ad->handle, &puk2_data, &pin2_data, setting_sim_verify_puks_cb, ad);
					setting_password_sim_clear_3line_entry(ad);

					if (TAPI_API_SUCCESS != tapi_ret) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_DEBUG("%s*** [ERR] tel_verify_sim_puks err=%d ***%s",
						                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
					} else {
						/*  Success. Send Message to Caller for Modify Pin Lock slider status */

					}
				} else {
					setting_password_sim_clear_3line_entry(ad);
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_CHANGE_PIN: {
				SETTING_TRACE("case SETTING_PW_TYPE_CHANGE_PIN");
				if (ad->cur_pwd) {
					SETTING_TRACE("step #2. new pins");
					ret = setting_password_sim_check_2line_entry(ad);
					if (ret == SETTING_RETURN_SUCCESS) {
						const char *old_pw;
						const char *new_pw;
						TelSimSecPw_t sim_old_sec_data = { 0, };
						TelSimSecPw_t sim_new_sec_data = { 0, };
						int tapi_ret = TAPI_API_SUCCESS;

						old_pw = ad->cur_pwd;
						new_pw = ad->ed_pw1->sub_desc;
						sim_old_sec_data.type = TAPI_SIM_PTYPE_PIN1;
						sim_new_sec_data.type = TAPI_SIM_PTYPE_PIN1;
						sim_old_sec_data.pw = (unsigned char *)old_pw;
						sim_old_sec_data.pw_len = safeStrLen(old_pw);
						sim_new_sec_data.pw = (unsigned char *)new_pw;
						sim_new_sec_data.pw_len = safeStrLen(new_pw);

						tapi_ret = tel_change_sim_pins(ad->handle, &sim_old_sec_data, &sim_new_sec_data, setting_sim_change_pins_cb, ad);
						setting_password_sim_clear_2line_entry(ad);

						if (TAPI_API_SUCCESS != tapi_ret) {
							ad->focus_data = ad->ed_pw1;
							SETTING_TRACE_DEBUG("%s*** [ERR] TelTapiSimChangePIN err=%d ***%s",
							                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
							setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						}
					} else {
						setting_password_sim_clear_2line_entry(ad);
					}
				} else {
					SETTING_TRACE("step #1. current pin");
					/* keep cur_pwd */
					ret = setting_password_sim_check_1line_entry(ad);
					if (ret == SETTING_RETURN_SUCCESS) {
						ad->cur_pwd = (char *)strdup(ad->ed_pw1->sub_desc);
						/*ad->update_change_pin1_flag = TRUE; */
						/*setting_view_update(&setting_view_password_main, ad); */

						Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(ad->ed_pw1->eo_check);
						if (imf_context) {
							SETTING_TRACE_DEBUG("imf_context (0x%.8x) : clear event callbacks", imf_context);
							ecore_imf_context_input_panel_event_callback_clear(imf_context);
						}

						elm_genlist_clear(ad->scroller);
						setting_password_sim_draw_2line_entry(ad, NULL);
						ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);
					} else {
						setting_password_sim_clear_1line_entry(ad);
					}
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_CHANGE_PIN2: {
				SETTING_TRACE("case SETTING_PW_TYPE_CHANGE_PIN2");
				if (ad->cur_pwd) {
					SETTING_TRACE("step #2. new pin2s");
					ret = setting_password_sim_check_2line_entry(ad);
					if (ret == SETTING_RETURN_SUCCESS) {
						const char *old_pw;
						const char *new_pw;
						TelSimSecPw_t sim_old_sec_data = { 0, };
						TelSimSecPw_t sim_new_sec_data = { 0, };
						int tapi_ret = TAPI_API_SUCCESS;

						old_pw = ad->cur_pwd;
						new_pw = ad->ed_pw1->sub_desc;

						sim_old_sec_data.type = TAPI_SIM_PTYPE_PIN2;
						sim_new_sec_data.type = TAPI_SIM_PTYPE_PIN2;
						sim_old_sec_data.pw = (unsigned char *)old_pw;
						sim_old_sec_data.pw_len = safeStrLen(old_pw);
						sim_new_sec_data.pw = (unsigned char *)new_pw;
						sim_new_sec_data.pw_len = safeStrLen(new_pw);

						tapi_ret = tel_change_sim_pins(ad->handle, &sim_old_sec_data, &sim_new_sec_data, setting_sim_change_pins_cb, ad);
						setting_password_sim_clear_2line_entry(ad);

						if (TAPI_API_SUCCESS != tapi_ret) {
							ad->focus_data = ad->ed_pw1;
							SETTING_TRACE_DEBUG("%s*** [ERR] TelTapiSimChangePIN err=%d ***%s",
							                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
							setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						}
					} else {
						setting_password_sim_clear_2line_entry(ad);
					}
				} else {
					SETTING_TRACE("step #1. current pin2");
					/* keep cur_pwd */
					ret = setting_password_sim_check_1line_entry(ad);
					if (ret == SETTING_RETURN_SUCCESS) {
						ad->cur_pwd = (char *)strdup(ad->ed_pw1->sub_desc);

						Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(ad->ed_pw1->eo_check);
						if (imf_context) {
							SETTING_TRACE_DEBUG("imf_context (0x%.8x) : clear event callbacks", imf_context);
							ecore_imf_context_input_panel_event_callback_clear(imf_context);
						}

						elm_genlist_clear(ad->scroller);
						setting_password_sim_draw_2line_entry(ad, NULL);
						ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);
					} else {
						setting_password_sim_clear_1line_entry(ad);
					}
				}
			}
			app_control_destroy(svc);
			return;
#if SUPPORT_SIMLOCK
		case SETTING_PW_TYPE_SIM_LOCK_ON: {
				SETTING_TRACE("case SETTING_PW_TYPE_SIM_LOCK_ON");

				ret = setting_password_sim_check_2line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					TelSimFacilityPw_t sim_sec_data = { 0, };
					int tapi_ret = TAPI_API_SUCCESS;
					const char *entry_str;
					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					sim_sec_data.lock_type = TAPI_SIM_LOCK_PS;/*TAPI_SIM_PTYPE_SIM */
					sim_sec_data.pw = (unsigned char *)entry_str;
					sim_sec_data.pw_len = safeStrLen(entry_str);

					tapi_ret = tel_enable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_enable_cb, ad);
					setting_password_sim_clear_2line_entry(ad);

					SETTING_TRACE_SECURE_DEBUG("SIM Lock password : %s\n", (char *)(sim_sec_data.pw));
					if (TAPI_API_SUCCESS != tapi_ret) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_DEBUG("%s*** [ERR] tel_enable_sim_facility err=%d ***%s",
						                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
					} else {
						/*  Success. Send Message to Caller for Modify Pin Lock slider status */

					}
				} else {
					setting_password_sim_clear_2line_entry(ad);
				}
			}
			app_control_destroy(svc);
			return;
		case SETTING_PW_TYPE_SIM_LOCK_OFF: {
				SETTING_TRACE("case SETTING_PW_TYPE_SIM_LOCK_OFF");

				ret = setting_password_sim_check_1line_entry(ad);
				if (ret == SETTING_RETURN_SUCCESS) {
					TelSimFacilityPw_t sim_sec_data = { 0, };
					int tapi_ret = TAPI_API_SUCCESS;
					const char *entry_str;

					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					sim_sec_data.lock_type = TAPI_SIM_LOCK_PS;/*TAPI_SIM_PTYPE_SIM */
					sim_sec_data.pw = (unsigned char *)entry_str;
					sim_sec_data.pw_len = safeStrLen(entry_str);

					tapi_ret = tel_disable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_disable_cb, ad);
					setting_password_sim_clear_1line_entry(ad);

					if (TAPI_API_SUCCESS != tapi_ret) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_DEBUG("%s*** [ERR] tel_disable_sim_facility err=%d ***%s",
						                    SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
					} else {
						/*  Success. Send Message to Caller for Modify Pin Lock slider status */

					}
				} else {
					setting_password_sim_clear_1line_entry(ad);
				}
			}
			app_control_destroy(svc);
			return;
#endif
#if SUPPORT_FDN
		case SETTING_PW_TYPE_FDN_MODE_ON: {
				SETTING_TRACE("case SETTING_PW_TYPE_FDN_MODE_ON");
				ret = setting_password_sim_check_1line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					const char *entry_str;
					int tapi_ret = TAPI_API_INVALID_INPUT;

					TelSimFacilityPw_t sim_sec_data = {0,};
					int ReqId;

					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] elm_entry_entry_get(ad->ed_pw1) return NULL!");
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}
					sim_sec_data.lock_type = TAPI_SIM_LOCK_FD;/*TAPI_SIM_PTYPE_PIN2; */
					sim_sec_data.pw_len = safeStrLen(entry_str);
					sim_sec_data.pw = (unsigned char *)entry_str;

					tapi_ret = tel_enable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_enable_cb, ad);
					setting_password_sim_clear_1line_entry(ad);

					if (tapi_ret != TAPI_API_SUCCESS) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] Failed to enable FDN. Tapi return %d", tapi_ret);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					app_control_add_extra_data(svc, "result", ad->view_type_string);
					ug_send_result(ad->ug, svc);
					SETTING_TRACE("Send Result : %s\n", ad->view_type_string);
					app_control_destroy(svc);
					/*ug_destroy_me(ad->ug); */
				} else {
					/* check failed */
					setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), TRUE);
				}
			}
			break;
		case SETTING_PW_TYPE_FDN_MODE_OFF: {
				SETTING_TRACE("case SETTING_PW_TYPE_FDN_MODE_OFF");
				ret = setting_password_sim_check_1line_entry(ad);

				if (ret == SETTING_RETURN_SUCCESS) {
					const char *entry_str;
					int tapi_ret = TAPI_API_INVALID_INPUT;

					TelSimFacilityPw_t sim_sec_data = {0,};

					entry_str = ad->ed_pw1->sub_desc;
					if (isEmptyStr(entry_str)) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] elm_entry_entry_get(ad->ed_pw1) return NULL!");
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}
					sim_sec_data.lock_type = TAPI_SIM_LOCK_FD;/*TAPI_SIM_PTYPE_PIN2 */
					sim_sec_data.pw_len = safeStrLen(entry_str);
					sim_sec_data.pw = (unsigned char *)entry_str;

					tapi_ret = tel_disable_sim_facility(ad->handle, &sim_sec_data, setting_sim_facility_disable_cb, ad);
					setting_password_sim_clear_1line_entry(ad);

					if (tapi_ret != TAPI_API_SUCCESS) {
						ad->focus_data = ad->ed_pw1;
						SETTING_TRACE_ERROR("[ERR] Failed to disable FDN. Tapi return %d", tapi_ret);
						setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), FALSE);
						app_control_destroy(svc);
						return;
					}

					app_control_add_extra_data(svc, "result", ad->view_type_string);
					ug_send_result(ad->ug, svc);
					SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

					app_control_destroy(svc);
					/*ug_destroy_me(ad->ug); */
				} else {
					/* check failed */
					setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_ERROR"), TRUE);
				}
			}
			break;
#endif
		default:
			app_control_destroy(svc);
			return;
	}

#if SUPPORT_FDN
	app_control_add_extra_data(svc, "result", ad->view_type_string);
	ug_send_result(ad->ug, svc);
	SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

	app_control_destroy(svc);
	/* Send destroy request */
	ug_destroy_me(ad->ug);
#endif
}

void setting_password_sim_click_softkey_done_cb(void *data, Evas_Object *obj,
                                                void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");

	SettingPasswordUG *ad = (SettingPasswordUG *) data;

	if (ad->ed_pw1 && ad->ed_pw1->eo_check) {
		elm_object_focus_set(ad->ed_pw1->eo_check, EINA_FALSE);
	}
	if (ad->ed_pw2 && ad->ed_pw2->eo_check) {
		elm_object_focus_set(ad->ed_pw2->eo_check, EINA_FALSE);
	}
	if (ad->ed_pw3 && ad->ed_pw3->eo_check) {
		elm_object_focus_set(ad->ed_pw3->eo_check, EINA_FALSE);
	}

	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}

	setting_password_sim_done(ad);
}

void setting_get_sim_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL || user_data == NULL);

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;

	SettingPasswordUG *ad = (SettingPasswordUG *)user_data;

	SETTING_TRACE_SECURE_DEBUG("sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]", sec_rt, lock->lock_type, lock->lock_status, lock->retry_count);

	char temp[256] = {0,};
	if (lock->retry_count > 1)
		snprintf(temp, 256, _(ATTEMPTS_DESC), lock->retry_count);
	else if (lock->retry_count > 0)
		snprintf(temp, 256, _(ATTEMPT_DESC), lock->retry_count);

	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}
	ad->err_desc = setting_create_Gendial_field_helpitem_without_bottom_separator(
	                   ad->scroller,
	                   &(itc_multiline_text),
	                   SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
	                   temp);
}

void setting_get_pin_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	TelSimPinOperationResult_t sec_rt = result;
	TelSimLockInfo_t *lock = data;


	SettingPasswordUG *ad = (SettingPasswordUG *)user_data;

	SETTING_TRACE_SECURE_DEBUG("sec_ret[%d], lock_type[%d], lock_status[%d], retry_count[%d]", sec_rt, lock->lock_type, lock->lock_status, lock->retry_count);

	char temp[256] = {0,};
	if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN) {
		char rtemp[SETTING_STR_SLP_LEN] = {0,};
		if (ad->incorrect_pin1_flag) {
			if (lock->retry_count > 1) {
				snprintf(rtemp, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_BODY_INCORRECT_PIN_E"), _(ATTEMPTS_DESC));
				snprintf(temp, SETTING_STR_SLP_LEN, rtemp, lock->retry_count);
			} else {
				snprintf(rtemp, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_BODY_INCORRECT_PIN_E"), _(ATTEMPT_DESC));
				snprintf(temp, SETTING_STR_SLP_LEN, rtemp, lock->retry_count);
			}
		} else {
			if (lock->retry_count > 1)
				snprintf(temp, 256, _(ATTEMPTS_DESC), lock->retry_count);
			else if (lock->retry_count > 0)
				snprintf(temp, 256, _(ATTEMPT_DESC), lock->retry_count);
		}
	} else if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN2) {
		char rtemp[SETTING_STR_SLP_LEN] = {0,};
		if (ad->incorrect_pin2_flag) {
			if (lock->retry_count > 1) {
				snprintf(rtemp, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PIN2"), _(ATTEMPTS_DESC));
				snprintf(temp, SETTING_STR_SLP_LEN, rtemp, lock->retry_count);
			} else {
				snprintf(rtemp, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PIN2"), _(ATTEMPT_DESC));
				snprintf(temp, SETTING_STR_SLP_LEN, rtemp, lock->retry_count);
			}
		} else {
			if (lock->retry_count > 1)
				snprintf(temp, 256, _(ATTEMPTS_DESC), lock->retry_count);
			else if (lock->retry_count > 0)
				snprintf(temp, 256, _(ATTEMPT_DESC), lock->retry_count);
		}
	} else {
		if (lock->retry_count > 1)
			snprintf(temp, 256, _(ATTEMPTS_DESC), lock->retry_count);
		else if (lock->retry_count > 0)
			snprintf(temp, 256, _(ATTEMPT_DESC), lock->retry_count);
	}

	if (ad->err_desc && ad->err_desc->item) {
		elm_object_item_del(ad->err_desc->item);
		ad->err_desc = NULL;
	}
	ad->err_desc = setting_create_Gendial_field_helpitem_without_bottom_separator(
	                   ad->scroller,
	                   &(itc_multiline_text),
	                   SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
	                   temp);
}

void setting_sim_change_pins_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data || !data);
	TelSimPinOperationResult_t sec_rt = result;
	tapi_receive_info result_info = { 0, };
	TelSimSecResult_t *sim_event_data = (TelSimSecResult_t *) data;
	SETTING_TRACE_SECURE_DEBUG("sec_rt[%d]", sec_rt);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->type:%d", sim_event_data->type);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->retry_count[%d]", sim_event_data->retry_count);

	if (TAPI_SIM_PIN_OPERATION_SUCCESS == sec_rt) {
		if (TAPI_SIM_PTYPE_PIN1 == sim_event_data->type)
			result_info.stat = SIM_PIN1_CHANGE_SUCCESS;
		else if (TAPI_SIM_PTYPE_PIN2 == sim_event_data->type)
			result_info.stat = SIM_PIN2_CHANGE_SUCCESS;
	} else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == sec_rt) {
		if (TAPI_SIM_PTYPE_PIN1 == sim_event_data->type) {
			result_info.stat = SIM_INCORRECT_PIN1_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
		} else if (TAPI_SIM_PTYPE_PIN2 == sim_event_data->type) {
			result_info.stat = SIM_INCORRECT_PIN2_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
		}
	} else if (TAPI_SIM_PUK_REQUIRED == sec_rt) {
		if (TAPI_SIM_PTYPE_PIN1 == sim_event_data->type)
			result_info.stat = SIM_PIN1_BLOCKED;
		else if (TAPI_SIM_PTYPE_PIN2 == sim_event_data->type)
			result_info.stat = SIM_PIN2_BLOCKED;
	} else {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] INCORRECTED ***%s",
		 SETTING_FONT_RED, SETTING_FONT_BLACK);
		return;
	}
	setting_password_ug_display_result_popup(&result_info, user_data);
}

void setting_sim_verify_puks_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;

	ret_if(!user_data || !data);
	SettingPasswordUG *ad = user_data;
	TelSimPinOperationResult_t sec_rt = result;
	tapi_receive_info result_info = { 0, };
	TelSimSecResult_t *sim_event_data = (TelSimSecResult_t *) data;
	ad->verify_puks_result = sim_event_data;
	SETTING_TRACE_SECURE_DEBUG("sec_rt[%d]", sec_rt);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->type:%d", sim_event_data->type);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->retry_count[%d]", sim_event_data->retry_count);

	if (TAPI_SIM_PTYPE_PUK1 == sim_event_data->type) {
		/*if (TAPI_SIM_PIN_REQUIRED == sec_rt){ */
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == sec_rt) {
			SETTING_TRACE_DEBUG("SIM_PIN1_UNBLOCKED");
			safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PIN1_UNBLOCKED", sizeof("SETTING_PW_TYPE_PIN1_UNBLOCKED") + 1);

			char tmp_str[SETTING_STR_SLP_LEN] = {0,};
			snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_PUK1_VERIFIED"), _("IDS_ST_POP_PIN_UNBLOCKED"));
			setting_password_ug_create_popup_notitle_nobtn(ad, tmp_str, TRUE);
			return;
		} else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == sec_rt) {
			result_info.stat = SIM_INCORRECT_PUK1_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
			ad->puk_verified_flag = TRUE;
			setting_view_update(&setting_view_password_main, ad);
		} else if (TAPI_SIM_PUK_INCORRECT_PASSWORD == sec_rt) {
			if (sim_event_data->retry_count <= 0) {
				result_info.stat = SIM_PUK1_BLOCKED;
				result_info.retry_cnt = 0;
			} else {
				result_info.stat = SIM_INCORRECT_PUK1_CODE;
				result_info.retry_cnt = sim_event_data->retry_count;
				ad->puk_verified_flag = TRUE;
				setting_view_update(&setting_view_password_main, ad);
			}
		} else if (TAPI_SIM_PERM_BLOCKED == sec_rt) {
			result_info.stat = SIM_PUK1_BLOCKED;
			result_info.retry_cnt = 0;
		} else {
			result_info.stat = SIM_UNKNOWN_ERROR;
		}
	}

	else if (TAPI_SIM_PTYPE_PUK2 == sim_event_data->type) {
		SETTING_TRACE_DEBUG("sec_rt : %d", sec_rt);
		/*if (TAPI_SIM_PUK_REQUIRED == sec_rt) */
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == sec_rt) {
			SETTING_TRACE_DEBUG("SIM_PIN2_UNBLOCKED");
			safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PIN2_UNBLOCKED", sizeof("SETTING_PW_TYPE_PIN2_UNBLOCKED") + 1);
			/*char tmp_str[SETTING_STR_SLP_LEN] = {0,}; */
			/*snprintf(tmp_str, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_PUK2_VERIFIED"), _("IDS_ST_POP_PIN2_UNBLOCKED")); */
			setting_password_ug_create_popup_notitle_nobtn(ad, _("IDS_ST_POP_PIN2_UNBLOCKED"), TRUE);
			return;
		} else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == sec_rt) {
			SETTING_TRACE_DEBUG("stat : SIM_INCORRECT_PUK2_CODE");
			result_info.stat = SIM_INCORRECT_PUK2_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
		} else if (TAPI_SIM_PUK_INCORRECT_PASSWORD == sec_rt) {
			SETTING_TRACE_DEBUG("stat : SIM_INCORRECT_PUK2_CODE");
			if (sim_event_data->retry_count <= 0) {
				result_info.stat = SIM_PUK2_BLOCKED;
				result_info.retry_cnt = 0;
			} else {
				result_info.stat = SIM_INCORRECT_PUK2_CODE;
				result_info.retry_cnt = sim_event_data->retry_count;
			}
		} else if (TAPI_SIM_PERM_BLOCKED == sec_rt) {
			result_info.stat = SIM_PUK2_BLOCKED;
			result_info.retry_cnt = 0;
		} else {
			result_info.stat = SIM_UNKNOWN_ERROR;
		}
	} else {
		result_info.stat = SIM_UNKNOWN_ERROR;
	}
	setting_password_ug_display_result_popup(&result_info, user_data);
}

void setting_sim_facility_enable_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);

	SETTING_TRACE_SECURE_DEBUG("result[%d]", result);
	if (result == TAPI_API_SIM_LOCKED) {	/* -503 error */
		setting_password_ug_create_popup_notitle_nobtn(user_data, _("IDS_ST_HEADER_UNAVAILABLE"), TRUE);
		return;
	}
	ret_if(!data);
	tapi_receive_info result_info = { 0, };
	TelSimFacilityResult_t *sim_event_data = (TelSimFacilityResult_t *) data;

	SETTING_TRACE_SECURE_DEBUG("sim_event_data->type:%d", sim_event_data->type);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->retry_count[%d]", sim_event_data->retry_count);

	/*  Sim Lock  */
	if (TAPI_SIM_LOCK_PS == sim_event_data->type) {
		SETTING_TRACE("CASE TAPI_SIM_PTYPE_SIM == sim_event_data->type");
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == result)
			result_info.stat = SIM_LOCK_ON_SUCCESS;
		else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == result)
			result_info.stat = SIM_LOCK_INCORRECT_PASSWORD;
		else {
			SETTING_TRACE_DEBUG("%s*** [ERR] INCORRECTED ***%s",
			                    SETTING_FONT_RED,
			                    SETTING_FONT_BLACK);
			return;
		}
	}
	/*  Pin Lock */
	else if (TAPI_SIM_LOCK_SC == sim_event_data->type) {
		SETTING_TRACE("CASE TAPI_SIM_PTYPE_PIN1 == sim_event_data->type");
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == result)
			result_info.stat = SIM_PIN_LOCK_ON_SUCCESS;
		else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == result) {
			result_info.stat = SIM_INCORRECT_PIN1_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
		} else if (TAPI_SIM_PUK_REQUIRED == result)
			result_info.stat = SIM_PIN1_BLOCKED;

		/*  Pre CCF */
		else if (TAPI_SIM_INCOMPATIBLE_PIN_OPERATION == result
		         || TAPI_SIM_CARD_ERROR == result) {
			/*  popup */
			result_info.stat = SIM_OPERATION_UNAVAILABLE;
		} else {
			SETTING_TRACE_DEBUG
			("%s*** [ERR] INCORRECTED ***%s",
			 SETTING_FONT_RED,
			 SETTING_FONT_BLACK);
			return;
		}
	}
	setting_password_ug_display_result_popup(&result_info, user_data);
}

void setting_sim_facility_disable_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);

	SETTING_TRACE_SECURE_DEBUG("result[%d]", result);
	if (result == TAPI_API_SIM_LOCKED) { /* -503 error */
		setting_password_ug_create_popup_notitle_nobtn(user_data, _("IDS_ST_HEADER_UNAVAILABLE"), TRUE);
		return;
	}
	ret_if(!data);
	tapi_receive_info result_info = { 0, };
	TelSimFacilityResult_t *sim_event_data = (TelSimFacilityResult_t *) data;

	SETTING_TRACE_SECURE_DEBUG("sim_event_data->type:%d", sim_event_data->type);
	SETTING_TRACE_SECURE_DEBUG("sim_event_data->retry_count[%d]", sim_event_data->retry_count);

	/*  Sim Lock  */
	if (TAPI_SIM_LOCK_PS == sim_event_data->type) {
		SETTING_TRACE_DEBUG("SIM lOCK");
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == result)
			result_info.stat = SIM_LOCK_OFF_SUCCESS;
		else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == result)
			result_info.stat = SIM_LOCK_INCORRECT_PASSWORD;
		else {
			SETTING_TRACE_DEBUG
			("%s*** [ERR] INCORRECTED ***%s",
			 SETTING_FONT_RED,
			 SETTING_FONT_BLACK);
			return;
		}
	}
	/*  Pin Lock */
	else if (TAPI_SIM_LOCK_SC == sim_event_data->type) {
		SETTING_TRACE_DEBUG("PIN LOCK");
		if (TAPI_SIM_PIN_OPERATION_SUCCESS == result)
			result_info.stat = SIM_PIN_LOCK_OFF_SUCCESS;
		else if (TAPI_SIM_PIN_INCORRECT_PASSWORD == result) {
			result_info.stat = SIM_INCORRECT_PIN1_CODE;
			result_info.retry_cnt = sim_event_data->retry_count;
		} else if (TAPI_SIM_PUK_REQUIRED == result)
			result_info.stat = SIM_PIN1_BLOCKED;

		/*  Pre CCF */
		else if (TAPI_SIM_INCOMPATIBLE_PIN_OPERATION == result
		         || TAPI_SIM_CARD_ERROR == result) {
			/*  popup */
			result_info.stat = SIM_OPERATION_UNAVAILABLE;
		} else {
			SETTING_TRACE_DEBUG
			("%s*** [ERR] INCORRECTED ***%s",
			 SETTING_FONT_RED,
			 SETTING_FONT_BLACK);
			return;
		}
	}
	setting_password_ug_display_result_popup(&result_info, user_data);
}

static void setting_password_ug_display_result_popup(tapi_receive_info *result,
                                                     SettingPasswordUG *ad)
{
	SETTING_TRACE_BEGIN;
	switch (result->stat) {
		case SIM_OPERATION_OK:
		case SIM_LOCK_ON_SUCCESS:
		case SIM_LOCK_OFF_SUCCESS:
		case SIM_PIN_LOCK_ON_SUCCESS:
		case SIM_PIN_LOCK_OFF_SUCCESS: {
				if (ad->view_type ==
				    SETTING_PW_TYPE_SIM_LOCK_OFF) {
					int err = SETTING_RETURN_SUCCESS;
					/*  reset VCONF */
					setting_set_int_slp_key
					(INT_SLP_SETTING_SIM_LOCK_ATTEMPTS_LEFT,
					 SIM_LOCK_ATTEMPS_MAX, &err);
					setting_retm_if(err == SETTING_RETURN_FAIL,
					                "[Error] set value of vconf fail.");
				}
				/*  Success to Operate */
				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result", ad->view_type_string);
				ug_send_result(ad->ug, svc);
				SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

				app_control_destroy(svc);
				/* Send destroy request */
				ug_destroy_me(ad->ug);
			}
			break;

		case SIM_PIN1_BLOCKED: {
				SETTING_TRACE_DEBUG("SIM_PIN1_BLOCKED");
				/*ad->view_type = SETTING_PW_TYPE_PIN_BLOCKED; */
				/*  Success to Operate */
				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result", "SETTING_PW_TYPE_PIN_BLOCKED");
				ug_send_result(ad->ug, svc);
				SETTING_TRACE("Send Result : %s\n", "SETTING_PW_TYPE_PIN_BLOCKED");

				app_control_destroy(svc);
				/* Send destroy request */
				ug_destroy_me(ad->ug);
			}
			break;
		case SIM_PIN2_BLOCKED: {
				SETTING_TRACE_DEBUG("SIM_PIN2_BLOCKED");
				/*ad->view_type = SETTING_PW_TYPE_PIN2_BLOCKED; */
				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result", "SETTING_PW_TYPE_PIN2_BLOCKED");
				ug_send_result(ad->ug, svc);
				SETTING_TRACE("Send Result : %s\n", "SETTING_PW_TYPE_PIN2_BLOCKED");

				app_control_destroy(svc);
				/* Send destroy request */
				ug_destroy_me(ad->ug);
				/*safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PIN2_BLOCKED", safeStrLen("SETTING_PW_TYPE_PIN2_BLOCKED")+1); */
				/*setting_password_ug_create_popup_notitle_nobtn(ad, */
				/*					       _("IDS_ST_POP_PIN2_BLOCKED"), TRUE); */
			}
			break;
		case SIM_LOCK_INCORRECT_PASSWORD: {
				SETTING_TRACE_DEBUG("SIM LOCK INCORRECT PASSWORD");
				ad->focus_data = ad->ed_pw1;
				setting_password_ug_display_desc(ad, PW_ERR_DESC, FALSE);
			}
			break;
		case SIM_INCORRECT_PIN1_CODE: {
				if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN) {
					ad->incorrect_pin1_flag = TRUE;
					FREE(ad->cur_pwd);
					elm_genlist_clear(ad->scroller);
					setting_password_sim_draw_1line_entry(ad, NULL);
					ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);
					return;
				}

				const char *ret_str = NULL;
				char tmp_str[SETTING_STR_SLP_LEN] = {0,};

				if (result->retry_cnt > 1) {
					/* there has '%d' in 'IDS_ST_POP_INCORRECT_PIN_PD_ATTEMPTS_LEFT', must transform */
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_BODY_INCORRECT_PIN_E"), _(ATTEMPTS_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
				} else {
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_BODY_INCORRECT_PIN_E"), _(ATTEMPT_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
					/*ret_str = _("IDS_ST_POP_INCORRECT_PIN1_ATTEMPT_LEFT"); */
				}

				ad->focus_data = ad->ed_pw1;
#ifdef SUPPORT_POPUP
				setting_password_ug_create_popup_notitle_nobtn(ad, (char *)ret_str, FALSE);
#else
				setting_password_ug_display_desc(ad, (char *)ret_str, FALSE);
#endif
			}
			break;
		case SIM_INCORRECT_PIN2_CODE: {
				if (ad->view_type == SETTING_PW_TYPE_CHANGE_PIN2) {
					ad->incorrect_pin2_flag = TRUE;
					FREE(ad->cur_pwd);
					elm_genlist_clear(ad->scroller);
					setting_password_sim_draw_1line_entry(ad, NULL);
					ad->focus_timer = ecore_timer_add(0.5, __setting_password_sim_rotate_cb, ad);
					return;
				}

				char *ret_str = NULL;
				char tmp_str[SETTING_STR_SLP_LEN] = {0,};

				if (result->retry_cnt > 1) {
					/* there has '%d' in 'IDS_ST_POP_INCORRECT_PIN_PD_ATTEMPTS_LEFT', must transform */
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PIN2"), _(ATTEMPTS_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
				} else {
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PIN2"), _(ATTEMPT_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
					/*ret_str = _("IDS_ST_POP_INCORRECT_PIN1_ATTEMPT_LEFT"); */
				}

				ad->focus_data = ad->ed_pw1;
#ifdef SUPPORT_POPUP
				setting_password_ug_create_popup_notitle_nobtn(ad, ret_str, FALSE);
#else
				setting_password_ug_display_desc(ad, ret_str, FALSE);
#endif
			}
			break;
		case SIM_INCORRECT_PUK1_CODE: {
				char *ret_str = NULL;
				char tmp_str[SETTING_STR_SLP_LEN] = {0,};

				if (result->retry_cnt > 1) {
					/* there has '%d' in 'IDS_ST_POP_INCORRECT_PUK1_PD_ATTEMPTS_LEFT', must transform */
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, PUK1_ERR_DESC, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
				} else {
					ret_str = _("IDS_ST_POP_INCORRECT_PUK1_CODE");
				}

				ad->focus_data = ad->ed_pw1;
#ifdef SUPPORT_POPUP
				setting_password_ug_create_popup_notitle_nobtn(ad, ret_str, FALSE);
#else
				setting_password_ug_display_desc(ad, ret_str, FALSE);
#endif
			}
			break;
		case SIM_INCORRECT_PUK2_CODE: {
				char *ret_str = NULL;
				char tmp_str[SETTING_STR_SLP_LEN] = {0,};

				if (result->retry_cnt > 1) {
					/* there has '%d' in 'IDS_ST_POP_INCORRECT_PIN_PD_ATTEMPTS_LEFT', must transform */
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PUK2"), _(ATTEMPTS_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
				} else {
					char tmp_str2[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str2, SETTING_STR_SLP_LEN, "%s. %s.", _("IDS_ST_POP_INCORRECT_PUK2"), _(ATTEMPT_DESC));
					int ret = snprintf(tmp_str, SETTING_STR_SLP_LEN, tmp_str2, result->retry_cnt);
					setting_retm_if(ret < 0, "snprintf fail");
					ret_str = tmp_str;
					ret_str = _("IDS_ST_POP_INCORRECT_PIN1_ATTEMPT_LEFT");
				}
				ad->focus_data = ad->ed_pw1;
#ifdef SUPPORT_POPUP
				setting_password_ug_create_popup_notitle_nobtn(ad, ret_str, FALSE);
#else
				setting_password_ug_display_desc(ad, ret_str, FALSE);
#endif
			}
			break;

		case SIM_PIN1_CHANGE_SUCCESS: {
				SETTING_TRACE_DEBUG("SIM_PIN1_CHANGE_SUCCESS");
				/*ad->view_type = SETTING_PW_TYPE_PIN_BLOCKED; */
				/*  Success to Operate */
				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result", ad->view_type_string);
				ug_send_result(ad->ug, svc);
				SETTING_TRACE("Send Result : %s\n", ad->view_type_string);

				app_control_destroy(svc);
				/* Send destroy request */
				ug_destroy_me(ad->ug);
			}
			/*setting_password_ug_create_popup_notitle_nobtn(ad, */
			/*					       _("IDS_ST_POP_PIN1_CHANGED"), TRUE); */
			break;
		case SIM_PIN2_CHANGE_SUCCESS:
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_POP_PIN2_CHANGED"), TRUE);
			break;
		case SIM_PIN1_UNBLOCKED:
			SETTING_TRACE_DEBUG("SIM_PIN1_UNBLOCKED");
			safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PIN_LOCK_ON", safeStrLen("SETTING_PW_TYPE_PIN_LOCK_ON") + 1);
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_POP_PIN_UNBLOCKED"), TRUE);
			break;
		case SIM_PIN2_UNBLOCKED:
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_POP_PIN2_UNBLOCKED"), TRUE);
			break;
		case SIM_PUK1_BLOCKED: {
				char *ret_str = NULL;
				ret_str = PUK1_BLOCKED;

				safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PUK1_BLOCKED", safeStrLen("SETTING_PW_TYPE_PUK1_BLOCKED") + 1);
				setting_password_ug_create_popup_notitle_nobtn(ad,
				                                               ret_str, TRUE);
				/*  Phone blocked. Call Permernent UG */
			}
			break;
		case SIM_PUK2_BLOCKED: {
				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result", "SETTING_PW_TYPE_PUK2_BLOCKED");
				ug_send_result(ad->ug, svc);
				SETTING_TRACE("Send Result : %s\n", "SETTING_PW_TYPE_PUK2_BLOCKED");

				app_control_destroy(svc);
				/* Send destroy request */
				ug_destroy_me(ad->ug);
			}
			/*  Call Permernent UG */
			break;
		case SIM_REQUIRED_PUK_CODE:
			SETTING_TRACE_DEBUG("SIM_REQUIRED_PUK_CODE");
			safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_PIN_BLOCKED", safeStrLen("SETTING_PW_TYPE_PIN_BLOCKED") + 1);
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_POP_PIN1_BLOCKED"), TRUE);
			break;
		case SIM_OPERATION_UNAVAILABLE:
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_ST_HEADER_UNAVAILABLE"), TRUE);
			break;
		case SIM_UNKNOWN_ERROR:
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("IDS_CST_POP_UNKNOWN_OPERATION"), FALSE);
			break;
		case SIM_ERROR:
		case SIM_REQ_PIN:
		case SIM_OPERATION_OFF:
		case SIM_LOCK_ON_FAIL:
			SETTING_TRACE_DEBUG("SIM_LOCK_ON_FAIL");
			{
				if (result->retry_cnt > 0) {
					setting_password_ug_display_desc(ad, PW_ERR_DESC, FALSE);
				} else {
					char tmp_str[SETTING_STR_SLP_LEN] = {0,};
					snprintf(tmp_str, SETTING_STR_SLP_LEN, PW_ERR_DELAY_DESC, PW_ERR_DELAY_TIME);
					safeCopyStr(ad->view_type_string, "SETTING_PW_TYPE_SIM_LOCK_DISABLE", safeStrLen("SETTING_PW_TYPE_SIM_LOCK_DISABLE") + 1);
					setting_password_ug_create_popup_notitle_nobtn(ad, tmp_str, TRUE);
				}
			}
			break;
		case SIM_PIN1_CHANGE_FAIL:
		case SIM_PIN2_CHANGE_FAIL:
			setting_password_ug_create_popup_notitle_nobtn(ad,
			                                               _("Change Failed"), TRUE);
			break;
		default:
			break;
	}

	SETTING_TRACE_END;
}
