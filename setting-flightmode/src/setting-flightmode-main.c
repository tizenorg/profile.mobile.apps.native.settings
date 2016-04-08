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
#include <setting-flightmode-main.h>

static int setting_flightmode_main_create(void *cb);
static int setting_flightmode_main_destroy(void *cb);
static int setting_flightmode_main_update(void *cb);
static int setting_flightmode_main_cleanup(void *cb);

setting_view setting_view_flightmode_main = {
	.create = setting_flightmode_main_create,
	.destroy = setting_flightmode_main_destroy,
	.update = setting_flightmode_main_update,
	.cleanup = setting_flightmode_main_cleanup,
};

static void setting_flightmode_main_tapi_event_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);
	SettingFlightModeUG *ad = (SettingFlightModeUG *) user_data;
	ad->b_fm_requesting = FALSE;
	SETTING_TRACE("result:%d", result);

	if (ad->popup_flight_mode) {
		evas_object_del(ad->popup_flight_mode);
		ad->popup_flight_mode = NULL;
	}
	int err = 0;
	switch (result) {
		case TAPI_POWER_FLIGHT_MODE_RESP_ON:

			setting_set_bool_slp_key(BOOL_SLP_SETTING_FLIGHT_MODE,
			                         SETTING_ON_OFF_BTN_ON, &err);

			/*If Setting has validate operation (fm_waiting_op == MODE_LEAVE) to process, process it. */
			/*Otherwise, do nothing */
			if (MODE_LEAVE == ad->fm_waiting_op) {
				/*Send the latest operation */
				ad->fm_waiting_op = MODE_INVALID;
				err = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_LEAVE, setting_flightmode_main_tapi_event_cb, ad);
				setting_retm_if(err != TAPI_API_SUCCESS,
				                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_LEAVE) ***");

				/*sucessfully sent, */
				ad->b_fm_requesting = TRUE;

			}
			break;

		case TAPI_POWER_FLIGHT_MODE_RESP_OFF:

			setting_set_bool_slp_key(BOOL_SLP_SETTING_FLIGHT_MODE,
			                         SETTING_ON_OFF_BTN_OFF, &err);

			/*If Setting has validate operation (here, fm_waiting_op == MODE_ENTER) to process,process it. */
			/*Otherwise, do nothing */
			if (MODE_ENTER == ad->fm_waiting_op) {
				/*Send the latest operation */
				ad->fm_waiting_op = MODE_INVALID;
				err = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_ENTER, setting_flightmode_main_tapi_event_cb, ad);
				setting_retm_if(err != TAPI_API_SUCCESS,
				                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_ENTER) ***");

				/*sucessfully sent, */
				ad->b_fm_requesting = TRUE;
			}

			break;

		case TAPI_POWER_FLIGHT_MODE_RESP_FAIL:
			/*Setting has a validate operation to process, Send the operation request */
			if (MODE_ENTER == ad->fm_waiting_op) {
				ad->fm_waiting_op = MODE_INVALID;
				err = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_ENTER, setting_flightmode_main_tapi_event_cb, ad);

				setting_retm_if(err != TAPI_API_SUCCESS,
				                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_ENTER) ***");
				ad->b_fm_requesting = TRUE;
			} else if (MODE_LEAVE == ad->fm_waiting_op) {
				ad->fm_waiting_op = MODE_INVALID;
				err = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_LEAVE, setting_flightmode_main_tapi_event_cb, ad);

				setting_retm_if(err != TAPI_API_SUCCESS,
				                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_LEAVE) ***");
				ad->b_fm_requesting = TRUE;
			} else { /*Current requset is the last one, Setting needs to notify user */
				setting_create_popup(ad, ad->win_get,
									 "IDS_ST_POP_ERROR",
									 "IDS_ST_POP_UNABLE_TO_TURN_ON_FLIGHT_MODE_VODA",
									 NULL, POPUP_INTERVAL,
									 FALSE, FALSE, 0);

				/*It is need to rollback the status, */
				setting_update_gl_item_chk_status(ad->data_flightmode, !(ad->data_flightmode->chk_status));
				return;
			}

			/*sucessfully sent, */
			ad->b_fm_requesting = TRUE;

			break;

		case TAPI_POWER_FLIGHT_MODE_RESP_MAX:
			setting_create_popup(ad, ad->win_get,
								 "IDS_ST_POP_ERROR",
								 "IDS_IM_POP_UNEXPECTED_ERROR",
								 NULL, POPUP_INTERVAL, FALSE, FALSE, 0);

			/*It is need to rollback the status, */
			setting_update_gl_item_chk_status(ad->data_flightmode, !(ad->data_flightmode->chk_status));
			break;
		default:
			/* do nothing */
			break;
	}
	return;
}

void __alternate_flight_mode(Evas_Object *check, void *data)
{
	SETTING_TRACE_BEGIN;
	int ret;
	Eina_Bool status =  elm_check_state_get(check);
	SETTING_TRACE("flight mode status : %d", status);
	/*SettingFlightModeUG *ad = (SettingFlightModeUG *) data; */
	ret_if(!data);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingFlightModeUG *ad = list_item->userdata;

	if (!ad->handle) {
		SETTING_TRACE("ad->handle is NULL, something error happended in TAPI");
		return;
	}

	/*Check whether some requestion is processing by TAPI */
	if (ad->b_fm_requesting) {
		/*Do nothing, just mark the lastest operation.. */
		SETTING_TRACE("Some requestion is processing by TAPI, wait to process");
		ad->fm_waiting_op = status ? MODE_ENTER : MODE_LEAVE;
		return;
	}
	/*otherwise, invalid waiting operation and send requsetion to TAPI: */
	ad->fm_waiting_op = MODE_INVALID;
	if (status) {
		ret = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_ENTER, setting_flightmode_main_tapi_event_cb, ad);
		setting_retm_if(ret != TAPI_API_SUCCESS,
		                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_ENTER) ***");
	} else {
		ret = tel_set_flight_mode(ad->handle, TAPI_POWER_FLIGHT_MODE_LEAVE, setting_flightmode_main_tapi_event_cb, ad);
		setting_retm_if(ret != TAPI_API_SUCCESS,
		                "*** [ERR] tel_set_flight_mode(TAPI_POWER_FLIGHT_MODE_LEAVE) ***");
	}

	/*sucessfully sent, */
	ad->b_fm_requesting = TRUE;
	/*SETTING_TRACE_END; */
}

static void _flightmode_vconf_change_cb(keynode_t *key, void *data)
{
	setting_retm_if(NULL == data, "NULL == data");
	SettingFlightModeUG *ad = (SettingFlightModeUG *) data;
	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_FLIGHT_MODE)) {
		ad->data_flightmode->chk_status = status;
		elm_check_state_set(ad->data_flightmode->eo_check, status);
	} else {
		SETTING_TRACE_ERROR("vconf_name is wrong");
	}
}


/* ***************************************************
 **
 **basic func
 **
 ****************************************************/
static int setting_flightmode_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFlightModeUG *ad = (SettingFlightModeUG *) cb;

	ad->handle = tel_init(NULL);
	if (!ad->handle) {
		SETTING_TRACE_ERROR("*** [ERR] tel_init failed ***");
		setting_create_popup(ad, ad->win_main_layout, _("IDS_ST_BODY_FLIGHT_MODE"), _("tel_init() failed"), NULL, 0, false, false, 0);
		/*return SETTING_RETURN_FAIL; */
	} else {
		SETTING_TRACE("tel_init ok[handle:%p]", ad->handle);
	}

	Evas_Object *genlist;
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           "IDS_ST_BODY_FLIGHT_MODE",
	                                           _("IDS_ST_BUTTON_BACK"),
	                                           NULL,
	                                           setting_flightmode_main_click_softkey_back_cb,
	                                           NULL,
	                                           ad,
	                                           &genlist,
	                                           &(ad->navi_bar));

	/* add - flightmode setting menu list */
	int value, err, ret;
	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_FLIGHT_MODE, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	/* create flightmode_mode */
	ad->data_flightmode = setting_create_Gendial_field_def(genlist,
	                                                             &(itc_1text_1icon),
	                                                             setting_flightmode_main_list_Gendial_mouse_up_cb,
	                                                             ad,
	                                                             SWALLOW_Type_1ICON_1RADIO,
	                                                             NULL,
	                                                             NULL,
	                                                             value,
	                                                             "IDS_ST_BODY_FLIGHT_MODE",
	                                                             NULL,
	                                                             setting_flightmode_main_chk_btn_cb);

	if (ad->data_flightmode) {
		ad->data_flightmode->userdata = ad;
		__BACK_POINTER_SET(ad->data_flightmode);
	}

	ADD_GL_HELP_NO_SEP(genlist, "IDS_ST_BODY_FLIGHT_MODE_DISABLES_CALLING_AND_MESSAGING_FUNCTIONS_AND_TURNS_OFF_MOBILE_DATA_AND_CONNECTIVITY_FUNCTIONS_MSG");

	ret = vconf_notify_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, _flightmode_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	/* update info */
	setting_view_flightmode_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_flightmode_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingFlightModeUG *ad = (SettingFlightModeUG *) cb;

	if (ad->popup_flight_mode) {
		evas_object_del(ad->popup_flight_mode);
		ad->popup_flight_mode = NULL;
	}

	int ret = vconf_ignore_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE, _flightmode_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	if (ad->handle) {
		int tapi_ret = tel_deinit(ad->handle);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_DEBUG("*** [ERR] tel_deinit. [%d] ***", tapi_ret);
		} else {
			SETTING_TRACE("***  tel_deinit OK ");
		}
		ad->handle = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		setting_view_flightmode_main.is_create = 0;
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_flightmode_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_flightmode_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFlightModeUG *ad = (SettingFlightModeUG *) cb;
	return setting_flightmode_main_destroy(ad);
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/
static Eina_Bool setting_flightmode_main_click_softkey_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, FALSE, "Data parameter is NULL");
	SettingFlightModeUG *ad = (SettingFlightModeUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;
}

static void setting_flightmode_main_list_Gendial_mouse_up_cb(void *data, Evas_Object *obj,
                                                             void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);

	if (list_item) {
		list_item->chk_status = !list_item->chk_status;
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		setting_flightmode_main_chk_btn_cb(list_item, list_item->eo_check, NULL);
	}
}

static void setting_flightmode_main_chk_btn_cb(void *data, Evas_Object *obj,
                                               void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingFlightModeUG *ad = list_item->userdata;
	retm_if(ad == NULL, "ad parameter is NULL");
	list_item->chk_status = elm_check_state_get(obj);/*  for genlist update status */

	if (list_item->chk_status) {
		list_item->chk_status = EINA_TRUE;
		elm_check_state_set(list_item->eo_check, EINA_TRUE);
		__alternate_flight_mode(list_item->eo_check, data);

	} else {
		list_item->chk_status = EINA_FALSE;
		elm_check_state_set(list_item->eo_check, EINA_FALSE);
		__alternate_flight_mode(list_item->eo_check, data);
	}
}


