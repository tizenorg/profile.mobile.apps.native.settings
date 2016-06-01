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

static int setting_view_security_update_create(void *cb);
static int setting_view_security_update_destroy(void *cb);
static int setting_view_security_update_update(void *cb);
static int setting_view_security_update_cleanup(void *cb);

static Eina_Bool setting_security_update_click_softkey_back_cb(void *data,
		Evas_Object *obj, void *event_info);
static void setting_security_update_mouse_up_Gendial_chk_cb(void *data,
		Evas_Object *obj, void *event_info);
static void setting_security_update_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info);
static void __get_toggle_status_form_msa(void *data);
/*static void __get_toggle_status_form_msa_wifi_only(void *data); */
static void setting_security_update_gl_item_chk_status(
		Setting_GenGroupItem_Data *item_data, int status);

setting_view setting_view_security_update = {
	.create = setting_view_security_update_create,
	.destroy = setting_view_security_update_destroy,
	.update = setting_view_security_update_update,
	.cleanup = setting_view_security_update_cleanup, };

/* ***************************************************
 **
 **basic func
 **
 ****************************************************/
static int setting_view_security_update_create(void *cb)
{
	if (!cb) {
		SETTING_TRACE_ERROR("cb == NULL");
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;
	}
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)cb;

	Evas_Object *scroller;
	/*Elm_Object_Item *item = NULL;*/

	/* add basic layout */
	char setBtnStr[MAX_DISPLAY_NAME_LEN_ON_UI];
	snprintf(setBtnStr, sizeof(setBtnStr), "%s",
			(char *)dgettext("sys_string", "IDS_ST_BUTTON_BACK"));

	if (&setting_view_security_update == ad->view_to_load) {
		ad->ly_main = setting_create_layout_navi_bar_genlist(
				ad->win_main_layout, ad->win_get,
				"IDS_EMAIL_POP_SECURITY_UPDATE_ABB", setBtnStr,
				NULL,
				(setting_call_back_func)setting_security_update_click_softkey_back_cb,
				NULL, ad, &scroller, &(ad->navi_bar));
	} else {

		setting_push_layout_navi_bar_genlist(ad->win_main_layout,
				ad->win_get,
				"IDS_EMAIL_POP_SECURITY_UPDATE_ABB", setBtnStr,
				NULL,
				(setting_call_back_func)setting_security_update_click_softkey_back_cb,
				NULL, ad, &scroller, ad->navi_bar);
	}

	ad->genlist = scroller;
	evas_object_smart_callback_add(ad->genlist, "realized",
			__gl_realized_cb, ad);

	/* separator */
	/*em = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 * ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/* Update button */
	ad->data_security_update_bt = setting_create_Gendial_field_def(scroller,
			&(ad->itc_1text_1icon),
			setting_security_update_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_INVALID,
			NULL, NULL, 1, _(UPDATE_TEXT), NULL, NULL);
	if (ad->data_security_update_bt) {
		__BACK_POINTER_SET(ad->data_security_update_bt);
	}
	/* Update button desc*/
	/*D_GL_HELP(scroller,CHECK_UPDATES); */
	/* separator */
	/*em = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 * ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/*button with toggle*/
	ad->data_security_update_chk = setting_create_Gendial_field_def(
			scroller, &itc_1text_1icon,
			setting_security_update_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1ICON_1RADIO, NULL,
			NULL, 1, _(AUTO_UPDATE),
			NULL, setting_security_update_mouse_up_Gendial_chk_cb);
	if (ad->data_security_update_chk) {
		__BACK_POINTER_SET(ad->data_security_update_chk);
		ad->data_security_update_chk->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_security_update_chk is NULL");
	}
	setting_add_gl_help(scroller, CHECK_UPDATE_AUTO_MSG);

	/* separator */
	/*em = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 * ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/*via wifi only*/
	ad->data_security_wifi_update_chk = setting_create_Gendial_field_def(
			scroller, &itc_1text_1icon,
			setting_security_update_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1ICON_1RADIO, NULL,
			NULL, 1, _(SECURITY_VIA_WIFI),
			NULL, setting_security_update_mouse_up_Gendial_chk_cb);
	if (ad->data_security_wifi_update_chk) {
		__BACK_POINTER_SET(ad->data_security_wifi_update_chk);
		ad->data_security_wifi_update_chk->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_security_update_chk is NULL");
	}
	setting_add_gl_help(scroller, SECURITY_VIA_WIFI_HELP_MSG);

	/*get auto update & via wifi value */
	__get_toggle_status_form_msa(cb);

	setting_view_security_update.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_view_security_update_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *)cb;
	if (&setting_view_security_update == ad->view_to_load) {
		if (ad->ly_main) {
			evas_object_del(ad->ly_main);
			ad->ly_main = NULL;
		}
	} else {
		elm_naviframe_item_pop(ad->navi_bar);
	}

	/*evas_object_smart_callback_del(ad->genlist, "realized",
	 * __gl_realized_cb); */

	setting_view_security_update.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_view_security_update_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_view_security_update_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingSecurityUG *ad = (SettingSecurityUG *)cb;
	return setting_view_security_update_destroy(ad);
	/*return SETTING_RETURN_SUCCESS; */
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/
static Eina_Bool setting_security_update_click_softkey_back_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, EINA_FALSE,
			"[Setting > Security update] Data parameter is NULL");
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	if (&setting_view_security_update == ad->view_to_load) {
		/* Send destroy request */
		ug_destroy_me(ad->ug);
	} else
		setting_view_change(&setting_view_security_update,
				&setting_view_security_main, ad);
	SETTING_TRACE_END;
	return EINA_TRUE;
}

static void setting_security_update_gl_item_chk_status(
		Setting_GenGroupItem_Data *item_data, int status)
{
	SETTING_TRACE_BEGIN;
	if (item_data && item_data->chk_status != status) {
		item_data->chk_status = status;
		elm_genlist_item_update(item_data->item);
	}
	SETTING_TRACE_END;
}

static void security_update_get_status_service_reply_cb(app_control_h request,
		app_control_h reply, app_control_result_e result,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	retm_if(request == NULL, "request is NULL");
	retm_if(reply == NULL, "reply is NULL");
	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;
	char *resultMsa = NULL;

	SETTING_TRACE_DEBUG("result:[%d]", result);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_SUCCEEDED");
		/*for auto update value */
		app_control_get_extra_data(reply, "AUTO", &resultMsa);
		SETTING_TRACE_DEBUG("resultCode:[%s]", resultMsa);
		if (!safeStrCmp(_("AUTO_ON"), resultMsa)) {
			SETTING_TRACE_DEBUG("auto update - on");
			setting_security_update_gl_item_chk_status(
					ad->data_security_update_chk, 1);
			if (ad->data_security_wifi_update_chk) {
				elm_object_item_disabled_set(
						ad->data_security_wifi_update_chk->item,
						EINA_FALSE);
			}
		} else if (!safeStrCmp(_("AUTO_OFF"), resultMsa)) {
			SETTING_TRACE_DEBUG("auto update - off");
			setting_security_update_gl_item_chk_status(
					ad->data_security_update_chk, 0);
			if (ad->data_security_wifi_update_chk) {
				elm_object_item_disabled_set(
						ad->data_security_wifi_update_chk->item,
						EINA_TRUE);
			}
		}
		FREE(resultMsa);
		/*for via wifi value */
		app_control_get_extra_data(reply, "WIFI", &resultMsa);
		SETTING_TRACE_DEBUG("resultCode:[%s]", resultMsa);
		if (!safeStrCmp(_("WIFI_ON"), resultMsa)) {
			SETTING_TRACE_DEBUG(" wifi only - on");
			setting_security_update_gl_item_chk_status(
					ad->data_security_wifi_update_chk, 1);
		} else if (!safeStrCmp(_("WIFI_OFF"), resultMsa)) {
			SETTING_TRACE_DEBUG("wifi only - off");
			setting_security_update_gl_item_chk_status(
					ad->data_security_wifi_update_chk, 0);
		}
		FREE(resultMsa);
	} else if (result == APP_CONTROL_RESULT_FAILED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_FAILED");
	} else if (result == APP_CONTROL_RESULT_CANCELED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_CANCELED");
	}

	return;
}

static void __get_toggle_status_form_msa(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);

	int ret = 0;
	app_control_h service = NULL;

	ret = app_control_create(&service);
	if (ret != APP_CONTROL_ERROR_NONE) {
		SETTING_TRACE_DEBUG("service_create fail!! ret=%d", ret);
		return;
	}

	app_control_set_app_id(service, "org.tizen.msa");
	app_control_set_operation(service,
			"http://org.tizen.msa/appcontrol/operation/query");

	ret = app_control_add_extra_data(service, "KEY", "STATUS");
	SETTING_TRACE_DEBUG("add extra data (KEY,STATUS) to service ret=%d",
			ret);

	ret = app_control_send_launch_request(service,
			security_update_get_status_service_reply_cb,
			(void *)data);
	SETTING_TRACE_DEBUG("send launch service (KEY,STATUS) request ret=%d",
			ret);
	if (ret != APP_CONTROL_ERROR_NONE) {
		SETTING_TRACE_ERROR("Cannot launch app");
	}
	app_control_destroy(service);
}

static void security_update_on_service_reply_cb(app_control_h request,
		app_control_h reply, app_control_result_e result,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	retm_if(request == NULL, "request is NULL");
	retm_if(reply == NULL, "reply is NULL");
	retm_if(user_data == NULL, "user_data is NULL");
	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	SETTING_TRACE_DEBUG("result:[%d]", result);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_SUCCEEDED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				1);
	} else if (result == APP_CONTROL_RESULT_FAILED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_FAILED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				0);
	} else if (result == APP_CONTROL_RESULT_CANCELED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_CANCELED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				0);
	}
	return;
}

static void security_update_off_service_reply_cb(app_control_h request,
		app_control_h reply, app_control_result_e result,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	retm_if(request == NULL, "request is NULL");
	retm_if(reply == NULL, "reply is NULL");
	retm_if(user_data == NULL, "user_data is NULL");
	SettingSecurityUG *ad = (SettingSecurityUG *)user_data;

	SETTING_TRACE_DEBUG("result:[%d]", result);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_SUCCEEDED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				0);
		if (ad->data_security_wifi_update_chk) {
			elm_object_item_disabled_set(
					ad->data_security_wifi_update_chk->item,
					EINA_TRUE);
			elm_object_disabled_set(
					ad->data_security_wifi_update_chk->eo_check,
					EINA_TRUE);
		}
	} else if (result == APP_CONTROL_RESULT_FAILED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_FAILED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				1);
	} else if (result == APP_CONTROL_RESULT_CANCELED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_CANCELED");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				1);
	}
	return;
}

static void security_wifi_update_on_service_reply_cb(app_control_h request,
		app_control_h reply, app_control_result_e result,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	retm_if(request == NULL, "request is NULL");
	retm_if(reply == NULL, "reply is NULL");
	retm_if(user_data == NULL, "user_data is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)user_data;

	SETTING_TRACE_DEBUG("result:[%d]", result);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_SUCCEEDED");
		setting_update_gl_item_chk_status(list_item, 1);
	} else if (result == APP_CONTROL_RESULT_FAILED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_FAILED");
		setting_update_gl_item_chk_status(list_item, 0);
	} else if (result == APP_CONTROL_RESULT_CANCELED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_CANCELED");
		setting_update_gl_item_chk_status(list_item, 0);
	}
	return;
}

static void security_wifi_update_off_service_reply_cb(app_control_h request,
		app_control_h reply, app_control_result_e result,
		void *user_data)
{
	SETTING_TRACE_BEGIN;
	retm_if(request == NULL, "request is NULL");
	retm_if(reply == NULL, "reply is NULL");
	retm_if(user_data == NULL, "user_data is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)user_data;

	SETTING_TRACE_DEBUG("result:[%d]", result);

	if (result == APP_CONTROL_RESULT_SUCCEEDED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_SUCCEEDED");
		setting_update_gl_item_chk_status(list_item, 0);
	} else if (result == APP_CONTROL_RESULT_FAILED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_FAILED");
		setting_update_gl_item_chk_status(list_item, 1);
	} else if (result == APP_CONTROL_RESULT_CANCELED) {
		SETTING_TRACE_DEBUG("APP_CONTROL_RESULT_CANCELED");
		setting_update_gl_item_chk_status(list_item, 1);
	}
	return;
}

static void __ask_create_auto_update_pop_on_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	ret_if(NULL == data);
	int response_type = btn_type(obj);

	if (POPUP_RESPONSE_OK == response_type) {
		SETTING_TRACE("OK");
		int ret = 0;
		app_control_h service = NULL;
		/*if(ad->data_security_update_chk->chk_status == 0) { */
		/* SETTING_TRACE_DEBUG("set auto update state to 1"); */
		/* setting_update_gl_item_chk_status(
		 * ad->data_security_update_chk, 1); */
		/*} */
		ret = app_control_create(&service);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_DEBUG("service_create fail!! ret=%d",
					ret);
			SETTING_TRACE_ERROR("Cannot launch app");
		} else {
			app_control_set_app_id(service, "org.tizen.msa");
			app_control_set_operation(service,
					"http://org.tizen.msa/appcontrol/operation/query");

			ret = app_control_add_extra_data(service, "KEY",
					"AUTO_ON");
			SETTING_TRACE_DEBUG(
					"add extra data (KEY,AUTO_ON) to service ret=%d",
					ret);

			ret = app_control_send_launch_request(service,
					security_update_on_service_reply_cb,
					(void *)data);
			SETTING_TRACE_DEBUG(
					"send launch service (KEY,AUTO_ON) request ret=%d",
					ret);
			if (ret != APP_CONTROL_ERROR_NONE) {
				SETTING_TRACE_ERROR("Cannot launch app");
			}
			if (ad->data_security_wifi_update_chk) {
				elm_object_item_disabled_set(
						ad->data_security_wifi_update_chk->item,
						EINA_FALSE);
				elm_object_disabled_set(
						ad->data_security_wifi_update_chk->eo_check,
						EINA_FALSE);
			}
		}
		app_control_destroy(service);
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		SETTING_TRACE("CANCEL");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				0);
	}

	evas_object_del(ad->pop_auto_update_on);
	ad->pop_auto_update_on = NULL;
}

static void __ask_create_auto_update_pop_off_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	ret_if(NULL == data);
	int response_type = btn_type(obj);

	if (POPUP_RESPONSE_OK == response_type) {
		SETTING_TRACE("OK");
		int ret = 0;
		app_control_h service = NULL;
		/*if(ad->data_security_update_chk->chk_status == 1) */
		/* setting_update_gl_item_chk_status(
		 * ad->data_security_update_chk, 0); */
		ret = app_control_create(&service);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_DEBUG("service_create fail!! ret=%d",
					ret);
			SETTING_TRACE_ERROR("Cannot launch app");
		} else {
			app_control_set_app_id(service, "org.tizen.msa");
			app_control_set_operation(service,
					"http://org.tizen.msa/appcontrol/operation/query");

			ret = app_control_add_extra_data(service, "KEY",
					"AUTO_OFF");
			SETTING_TRACE_DEBUG(
					"add extra data (KEY,AUTO_OFF) to service ret=%d",
					ret);

			ret = app_control_send_launch_request(service,
					security_update_off_service_reply_cb,
					(void *)data);
			SETTING_TRACE_DEBUG(
					"send launch service (KEY,AUTO_OFF) request ret=%d",
					ret);
			if (ret != APP_CONTROL_ERROR_NONE) {
				SETTING_TRACE_ERROR("Cannot launch app");
			}
			/*if (ad->data_security_wifi_update_chk) { */
			/* elm_object_item_disabled_set(
			 * ad->data_security_wifi_update_chk->item,
			 * EINA_TRUE); */
			/* elm_object_disabled_set(
			 * ad->data_security_wifi_update_chk->eo_check,
			 * EINA_TRUE); */
			/*} */
		}
		app_control_destroy(service);
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		SETTING_TRACE("CANCEL");
		setting_update_gl_item_chk_status(ad->data_security_update_chk,
				1);
	}

	evas_object_del(ad->pop_auto_update_off);
	ad->pop_auto_update_off = NULL;
}

static void __ask_create_manual_update_pop_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;
	ret_if(NULL == data);
	int response_type = btn_type(obj);
	if (POPUP_RESPONSE_OK == response_type) {
		SETTING_TRACE("OK");

		/*request manual update */
		int ret = 0;
		app_control_h service = NULL;
		ret = app_control_create(&service);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_DEBUG("service_create fail!! ret=%d",
					ret);
			evas_object_del(ad->pop_manual_update);
			ad->pop_manual_update = NULL;
			SETTING_TRACE_ERROR("Cannot launch app");
			return;
		} else {
			app_control_set_app_id(service, "org.tizen.msa");
			app_control_set_operation(service,
					"http://org.tizen.msa/appcontrol/operation/query");
			ret = app_control_add_extra_data(service, "KEY",
					"MANUAL_UPDATE");
			SETTING_TRACE_DEBUG("add extra data to service ret=%d",
					ret);
			ret = app_control_send_launch_request(service, NULL,
					NULL);
			SETTING_TRACE_DEBUG(
					"send launch service request ret=%d",
					ret);
			if (ret != APP_CONTROL_ERROR_NONE) {
				evas_object_del(ad->pop_manual_update);
				ad->pop_manual_update = NULL;
				SETTING_TRACE_ERROR("Cannot launch app");
				app_control_destroy(service);
				service = NULL;
				return;
			}
		}
		app_control_destroy(service);
		service = NULL;

		/*POP_UP */
		ad->pop_progress = elm_popup_add(ad->win_get);
		eext_object_event_callback_add(ad->pop_progress,
				EEXT_CALLBACK_BACK, setting_popup_del_cb, NULL);
		evas_object_size_hint_weight_set(ad->pop_progress,
				EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		Evas_Object *box = NULL;
		Evas_Object *progressbar = NULL;
		Evas_Object *label = NULL;

		/*label */
		label = elm_label_add(ad->pop_progress);
		elm_object_text_set(label, _(CONNECT_TO_SERVER));
		evas_object_show(label);
		box = elm_box_add(ad->pop_progress);

		/*progressbar */
		progressbar = elm_progressbar_add(ad->pop_progress);
		elm_object_style_set(progressbar, PROGRESSBAR_STYLE);
		elm_progressbar_pulse(progressbar, EINA_TRUE);
		evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL,
				0.5);
		evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		elm_progressbar_pulse(progressbar, EINA_TRUE);
		evas_object_show(progressbar);

		/*box */
		Evas_Object *blank = setting_create_blank_rect_customize(
				ad->pop_progress, 10, 30);
		elm_box_pack_end(box, blank);
		elm_box_pack_end(box, label);
		elm_box_pack_end(box, blank);
		elm_box_pack_end(box, progressbar);
		elm_box_pack_end(box, blank);
		/*elm_box_pack_end(box, btn); */
		evas_object_show(box);
		elm_object_content_set(ad->pop_progress, box);
		evas_object_show(ad->pop_progress);

	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		SETTING_TRACE("CANCEL");
		/* do nothing */
	}

	evas_object_del(ad->pop_manual_update);
	ad->pop_manual_update = NULL;
}

static void setting_security_update_mouse_up_Gendial_list_cb(void *data,
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

	if (!safeStrCmp(_(AUTO_UPDATE), list_item->keyStr)) {
		SETTING_TRACE_DEBUG("There is auto update");
		if (ad->data_security_update_chk->chk_status == 1) {
			SETTING_TRACE_DEBUG("set auto update state to 0");
			/* TOGGLE OFF */
			SETTING_TRACE_DEBUG(
					"There is auto update toggle on->off");
			ad->pop_auto_update_off = setting_create_popup(ad,
					ad->win_get,
					NULL, SECURITY_UPDATE_TOGGLE_OFF,
					__ask_create_auto_update_pop_off_cb, 0,
					FALSE, FALSE, 2, "IDS_ST_BODY_TURN_OFF",
					"IDS_ST_BUTTON_CANCEL_ABB");
		} else {
			SETTING_TRACE_DEBUG("set auto update state to 1");
			/* TOGGLE ON */
			SETTING_TRACE_DEBUG(
					"There is auto update toggle off->on");
			ad->pop_auto_update_on = setting_create_popup(ad,
					ad->win_get,
					NULL, SECURITY_UPDATE_TOGGLE_ON,
					__ask_create_auto_update_pop_on_cb, 0,
					FALSE, FALSE, 2, "IDS_ST_BUTTON_OK",
					"IDS_ST_BUTTON_CANCEL_ABB");
		}
	} else if (!safeStrCmp(_(UPDATE_TEXT), list_item->keyStr)) {
		SETTING_TRACE_DEBUG("There is manual update");
		ad->pop_manual_update = setting_create_popup(ad, ad->win_get,
		NULL, SECURITY_MANUAL_UPDATE_TEXT,
				__ask_create_manual_update_pop_cb, 0, FALSE,
				FALSE, 2, "IDS_ST_BUTTON_OK",
				"IDS_ST_BUTTON_CANCEL_ABB");
	} else if (!safeStrCmp(_(SECURITY_VIA_WIFI), list_item->keyStr)) {
		int ret = 0;
		app_control_h service = NULL;
		int chk_status = !list_item->chk_status;
		setting_update_gl_item_chk_status(list_item, chk_status);
		ret = app_control_create(&service);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_DEBUG("service_create fail!! ret=%d",
					ret);
		} else {
			app_control_set_app_id(service, "org.tizen.msa");
			app_control_set_operation(service,
					"http://org.tizen.msa/appcontrol/operation/query");
			char *type = NULL;
			if (chk_status) {
				type = "WIFI_ON";
			} else {
				type = "WIFI_OFF";
			}
			ret = app_control_add_extra_data(service, "KEY", type);

			if (chk_status == 1)
				ret = app_control_send_launch_request(service,
						security_wifi_update_on_service_reply_cb,
						list_item);
			else
				ret = app_control_send_launch_request(service,
						security_wifi_update_off_service_reply_cb,
						list_item);
			SETTING_TRACE_DEBUG(
					"send launch service (MSA_QUERY, %s) request ret=%d",
					type, ret);
			if (ret != APP_CONTROL_ERROR_NONE) {
				SETTING_TRACE_ERROR("Cannot launch app");
			}
		}
		app_control_destroy(service);
	}
}

static void __setting_security_update_toggle_automatic_chk(void *data,
		int chk_status, char *keyStr)
{
	SETTING_TRACE_BEGIN;
	SettingSecurityUG *ad = (SettingSecurityUG *)data;

	if (chk_status) {
		/* TOGGLE ON */
		SETTING_TRACE_DEBUG("There is auto update toggle off->on");
		ad->pop_auto_update_on = setting_create_popup(ad, ad->win_get,
		NULL, SECURITY_UPDATE_TOGGLE_ON,
				__ask_create_auto_update_pop_on_cb, 0, FALSE,
				FALSE, 2, "IDS_ST_BUTTON_OK",
				"IDS_ST_BUTTON_CANCEL_ABB");
	} else {
		/* TOGGLE OFF */
		SETTING_TRACE_DEBUG("There is auto update toggle on->off");
		ad->pop_auto_update_off = setting_create_popup(ad, ad->win_get,
		NULL, SECURITY_UPDATE_TOGGLE_OFF,
				__ask_create_auto_update_pop_off_cb, 0, FALSE,
				FALSE, 2, "IDS_ST_BODY_TURN_OFF",
				"IDS_ST_BUTTON_CANCEL_ABB");
	}
	return;
}

static void setting_security_update_mouse_up_Gendial_chk_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;
	SettingSecurityUG *ad = list_item->userdata;
	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);
	SETTING_TRACE_DEBUG("auto update toggle cb");

	/* enable /disable toggle button */
	if (!safeStrCmp(_(AUTO_UPDATE), list_item->keyStr)) {
		__setting_security_update_toggle_automatic_chk(ad,
				list_item->chk_status, list_item->keyStr);
		list_item->chk_status = !list_item->chk_status;
		elm_check_state_set(obj, list_item->chk_status);
	} else if (!safeStrCmp(_(SECURITY_VIA_WIFI), list_item->keyStr)) {
		int ret = 0;
		app_control_h service = NULL;
		int chk_status = list_item->chk_status;
		/*setting_update_gl_item_chk_status(
		 * ad->data_security_wifi_update_chk, chk_status); */
		ret = app_control_create(&service);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_DEBUG("service_create fail!! ret=%d",
					ret);
			SETTING_TRACE_ERROR("Cannot launch app");
		} else {
			app_control_set_app_id(service, "org.tizen.msa");
			app_control_set_operation(service,
					"http://org.tizen.msa/appcontrol/operation/query");
			char *type = NULL;
			if (chk_status) {
				type = "WIFI_ON";
			} else {
				type = "WIFI_OFF";
			}
			ret = app_control_add_extra_data(service, "KEY", type);

			if (chk_status == 1)
				ret = app_control_send_launch_request(service,
						security_wifi_update_on_service_reply_cb,
						list_item);
			else
				ret = app_control_send_launch_request(service,
						security_wifi_update_off_service_reply_cb,
						list_item);
			SETTING_TRACE_DEBUG(
					"send launch service (KEY, %s) request ret=%d",
					type, ret);
			if (ret != APP_CONTROL_ERROR_NONE) {
				SETTING_TRACE_ERROR("Cannot launch app");
			}
		}
		list_item->chk_status = !list_item->chk_status;
		elm_check_state_set(obj, list_item->chk_status);
		app_control_destroy(service);
	}
	return;
}
