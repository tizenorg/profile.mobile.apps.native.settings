/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <setting-powersaving-main.h>
#include <devman.h>

static int setting_powersaving_main_create(void *cb);
static int setting_powersaving_main_destroy(void *cb);
static int setting_powersaving_main_update(void *cb);
static int setting_powersaving_main_cleanup(void *cb);
static void setting_powersaving_view_disable(void *data, bool disable);

setting_view setting_view_powersaving_main = {
	.create = setting_powersaving_main_create,
	.destroy = setting_powersaving_main_destroy,
	.update = setting_powersaving_main_update,
	.cleanup = setting_powersaving_main_cleanup,
};

static void __setting_powersaving_main_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingPowersavingUG *ad = (SettingPowersavingUG *)data;

	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS)) {
		SETTING_TRACE("status:%d", status);
		setting_powersaving_view_disable(ad, !status);
		ad->data_powersaving_active->chk_status = status;
		elm_check_state_set(ad->data_powersaving_active->eo_check, ad->data_powersaving_active->chk_status);
		int ret = 0;
		if (status) {//powersaving is on
			int pwrsv_vib = 0;
			ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &pwrsv_vib);
			setting_retm_if(ret != 0, "get vconf failed");
			if (pwrsv_vib){
				//touch feedback virbration
				int value = 0;
				ret = vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
				setting_retm_if(ret != 0, "get vconf failed");
				ret = vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
				setting_retm_if(ret != 0, "set vconf failed");
				ret = vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
				setting_retm_if(ret != 0, "set vconf failed");
			}
		} else {//powersaving is off
			//touch feedback virbration
			int value = 0;
			ret = vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
			setting_retm_if(ret != 0, "get vconf failed");
			ret = vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
			setting_retm_if(ret != 0, "set vconf failed");
		}

	} else {
		SETTING_TRACE_ERROR("vconf_name is wrong");
	}
}


/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_powersaving_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;
	Evas_Object *scroller = NULL;

	/* add basic layout */
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _(KeyStr_Powersaving),
						   _("IDS_COM_BODY_BACK"), NULL,
						   setting_powersaving_main_click_softkey_back_cb,
						   NULL, ad, &scroller,
						   &(ad->navi_bar));
	Elm_Object_Item *item = NULL;;
	/*  add separator */
	item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* Powersaving Mode Activation */
	int ret;
	int value = 1;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	int active_value = value;

	ad->data_powersaving_active = setting_create_Gendial_field_def(scroller,
								       &itc_1text_1icon,
								       setting_powersaving_main_mouse_up_Gendial_list_cb,
								       ad,
								       SWALLOW_Type_1TOGGLE,
								       NULL,
								       NULL,
								       value,
								       KeyStr_Powersaving_Mode,
								       NULL,
								       setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_powersaving_active) {
		__BACK_POINTER_SET(ad->data_powersaving_active);
		ad->data_powersaving_active->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_powersaving_active is NULL");
	}
#if SUPPORT_UI0_4
	(void)setting_create_Gendial_field_titleItem(scroller,
						     &itc_group_item,
						     KeyStr_PS_SETTINGS,
						     NULL);
	//A.
	value = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	ad->data_cpu_ps = setting_create_Gendial_field_def(scroller,
						       &itc_1text_1icon,
						       setting_powersaving_main_mouse_up_Gendial_list_cb,
						       ad,
						       SWALLOW_Type_1TOGGLE,
						       NULL,
						       NULL,
						       value,
						       KeyStr_CPU_PS,
						       NULL,
						       setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_cpu_ps) {
		__BACK_POINTER_SET(ad->data_cpu_ps);
		ad->data_cpu_ps->userdata = ad;
	}

	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_CPU_PS_DSC, NULL,
					 NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

#if DISABLED_CODE
	//B.
	value = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DISPLAY, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	ad->data_display_ps = setting_create_Gendial_field_def(scroller,
						       &itc_1text_1icon,
						       setting_powersaving_main_mouse_up_Gendial_list_cb,
						       ad,
						       SWALLOW_Type_1TOGGLE,
						       NULL,
						       NULL,
						       value,
						       KeyStr_DISPLAY_PS,
						       NULL,
						       setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_display_ps) {
		__BACK_POINTER_SET(ad->data_display_ps);
		ad->data_display_ps->userdata = ad;
	}

	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_DISPLAY_PS_DSC, NULL,
					 NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	//C.
	value = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BG_COLOR, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	ad->data_bg_color = setting_create_Gendial_field_def(scroller,
						       &itc_1text_1icon,
						       setting_powersaving_main_mouse_up_Gendial_list_cb,
						       ad,
						       SWALLOW_Type_1TOGGLE,
						       NULL,
						       NULL,
						       value,
						       KeyStr_BG_COLOR,
						       NULL,
						       setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_bg_color) {
		__BACK_POINTER_SET(ad->data_bg_color);
		ad->data_bg_color->userdata = ad;
	}

	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_BG_COLOR_DSC, NULL,
					 NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
#endif

	//D.
	value = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	ad->data_screen_vib = setting_create_Gendial_field_def(scroller,
						       &itc_1text_1icon,
						       setting_powersaving_main_mouse_up_Gendial_list_cb,
						       ad,
						       SWALLOW_Type_1TOGGLE,
						       NULL,
						       NULL,
						       value,
						       KeyStr_SCREEN_VIB,
						       NULL,
						       setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_screen_vib) {
		__BACK_POINTER_SET(ad->data_screen_vib);
		ad->data_screen_vib->userdata = ad;
	}

	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_SCREEN_VIB_DSC, NULL,
					 NULL);
	/****Powersaving Tips*/
	(void)setting_create_Gendial_field_titleItem(scroller,
						     &itc_group_item,
						     KeyStr_Powersaving_Tips,
						     NULL);

	/****Customed Setting*/
	ad->data_powersaving_about =
	    setting_create_Gendial_field_def(scroller, &itc_1text,
					     setting_powersaving_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, KeyStr_Powersaving_About,
					     NULL, NULL);
	__BACK_POINTER_SET(ad->data_powersaving_about);

	item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	setting_powersaving_view_disable(ad, !active_value);
#else
#if SUPPORT_UI0_3
	/****Powersaving Mode description*/
	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad, SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_Powersaving_Mode_DSC, NULL,
					 NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	/****customed Setting Mode active*/
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_STATUS, &value);
	if (0 != ret) {
		SETTING_TRACE_ERROR("Failed to get vconf value");
	}
	bool customed_flag = (bool) value;
	ad->data_customed_active = setting_create_Gendial_field_def(scroller,
						&itc_1text_1icon,
						setting_powersaving_main_mouse_up_Gendial_list_cb,
						ad,
						SWALLOW_Type_1TOGGLE,
						NULL, NULL,
						value,
						KeyStr_Powersaving_Customed_Mode,
						NULL,
						setting_powersaving_main_motion_active_chk_btn_cb);
	if (ad->data_customed_active) {
		__BACK_POINTER_SET(ad->data_customed_active);
		ad->data_customed_active->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_powersaving_active is NULL");
	}
#else
	setting_enable_expandable_genlist(scroller, ad, setting_powersaving_ug_exp_cb,NULL);
	char *str_value = get_pa_powersaving_at_str();
	ad->data_saving_at =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &itc_2text_3_parent,
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    KeyStr_Powersaving_AT,
						    str_value);

	if (ad->data_saving_at) {
		ad->data_saving_at->int_slp_setting_binded =
		    INT_SLP_SETTING_POWERSAVING_AT;
	} else {
		SETTING_TRACE_ERROR("ad->data_back is NULL");
	}
	__BACK_POINTER_SET(ad->data_saving_at);
	G_FREE(str_value);
#endif
	/****Customed Setting*/
	ad->data_customed_setting =
	    setting_create_Gendial_field_def(scroller, &itc_1text,
					     setting_powersaving_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0,
					     KeyStr_Powersaving_Customed, NULL,
					     NULL);
	__BACK_POINTER_SET(ad->data_customed_setting);

	/* Powersaving and battery level Description */
	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0,
					 KeyStr_Powersaving_Level_DSC, NULL,
					 NULL);

	/****Powersaving Tips*/
	(void)setting_create_Gendial_field_titleItem(scroller,
						     &itc_group_item,
						     KeyStr_Powersaving_Tips,
						     NULL);

	/****Customed Setting*/
	ad->data_powersaving_about =
	    setting_create_Gendial_field_def(scroller, &itc_1text,
					     setting_powersaving_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, KeyStr_Powersaving_About,
					     NULL, NULL);
	__BACK_POINTER_SET(ad->data_powersaving_about);
#if SUPPORT_UI0_3
	if (!customed_flag && ad->data_customed_setting) {
		setting_disable_genlist_item(ad->data_customed_setting->item);
	}
#else
	if (!active_value)
	{
		if (ad->data_saving_at) {
			setting_disable_genlist_item(ad->data_saving_at->item);
		}

		if (ad->data_customed_setting) {
			setting_disable_genlist_item(ad->data_customed_setting->item);
		}
	}
#endif
#endif

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS,
			__setting_powersaving_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	setting_view_powersaving_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}


static int setting_powersaving_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;

	int ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS,
			__setting_powersaving_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		setting_view_powersaving_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
static void
__powersaving_on_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "NULL == data");
	int response_type = btn_type(obj);
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;
	if (POPUP_RESPONSE_OK == response_type) {	//need set vconf
		if (ad->data_powersaving_active) {
			int err =
				vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS,
						ad->data_powersaving_active->chk_status);
			if (0 != err) {	//if set failed, need rollback
				SETTING_TRACE_ERROR("Set vconf Failed");
				ad->data_powersaving_active->chk_status =
					!ad->data_powersaving_active->chk_status;
				elm_check_state_set(ad->
						data_powersaving_active->
						eo_check,
						ad->
						data_powersaving_active->
						chk_status);
				return;
			}
			if (ad->data_powersaving_active->chk_status) {//powersaving is on
				int pwrsv_vib = 0;
				vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &pwrsv_vib);
				if (pwrsv_vib){
					//touch feedback virbration
					int value = 0;
					vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
					vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
					vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
				}
			} else {//powersaving is off
				//touch feedback virbration
				int value = 0;
				vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
				vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
			}

			if (ad->popup_chk && elm_check_state_get(ad->popup_chk)) {
				(void)
					vconf_set_bool
					(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS_REMINDER,
					 FALSE);
			}
		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {	//need rollback
		if (ad->data_powersaving_active) {
			ad->data_powersaving_active->chk_status =
			    !ad->data_powersaving_active->chk_status;
			elm_check_state_set(ad->data_powersaving_active->
					    eo_check,
					    ad->data_powersaving_active->
					    chk_status);
			if (ad->data_saving_at) {
				setting_disable_genlist_item(ad->data_saving_at->item);
			}
			if (ad->data_customed_setting) {
				setting_disable_genlist_item(ad->data_customed_setting->item);
			}
			if (ad->data_cpu_ps) {
				setting_disable_genlist_item(ad->data_cpu_ps->item);
			}
			if (ad->data_display_ps) {
				setting_disable_genlist_item(ad->data_display_ps->item);
			}
			if (ad->data_bg_color) {
				setting_disable_genlist_item(ad->data_bg_color->item);
			}
			if (ad->data_screen_vib) {
				setting_disable_genlist_item(ad->data_screen_vib->item);
			}
		}
	}

	evas_object_del(ad->popup);
	ad->popup = NULL;
	ad->popup_chk = NULL;
}

static void
setting_powersaving_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
						  void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KeyStr_Powersaving_Mode, list_item->keyStr)) {
		list_item->chk_status =
		    !elm_check_state_get(list_item->eo_check);
		elm_check_state_set(list_item->eo_check, list_item->chk_status);

		setting_powersaving_view_disable(ad, !list_item->chk_status);

		int reminder_flag = TRUE;
		int ret =
		    vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS_REMINDER,
				   &reminder_flag);
		if (0 != ret) {
			SETTING_TRACE_ERROR("Failed to get vconf value");
		}
		SETTING_TRACE("reminder_flag:%d", reminder_flag);
		if (list_item->chk_status && reminder_flag) {
			ad->popup =
			    setting_create_popup_with_image_check(ad, ad->win_get,
							    NULL,
							    _(POWERSAVING_REMINDER), NULL,NULL,
							    _(NEVER_SHOW_AGAIN),
							    &(ad->popup_chk),
							    __powersaving_on_resp_cb,
							    0,
							    2, _("IDS_COM_SK_OK"),
							    _("IDS_COM_SK_CANCEL"));
			return;
		}
		ret =
		    vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS, list_item->chk_status);
		if (0 != ret) {	/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(list_item->eo_check,
					    list_item->chk_status);
			return;
		}
		if (list_item->chk_status) {//powersaving is on
			int pwrsv_vib = 0;
			vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &pwrsv_vib);
			if (pwrsv_vib){
				//touch feedback virbration
				int value = 0;
				vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
				vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
				vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
			}
		} else {//powersaving is off
			//touch feedback virbration
			int value = 0;
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
		}

	}
	else if (!safeStrCmp(KeyStr_Powersaving_Customed_Mode, list_item->keyStr)) {
		int old_status = elm_check_state_get(list_item->eo_check);
		int ret = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_STATUS, !old_status);
		setting_retm_if(0 != ret, "Failed to set vconf [%s]",
			VCONFKEY_SETAPPL_PWRSV_CUSTMODE_STATUS);
		/* new status */
		list_item->chk_status = !old_status;
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		if (0 == list_item->chk_status) {
			//not active
			if (ad->data_customed_setting) {
				setting_disable_genlist_item(ad->data_customed_setting->item);
				}
		} else {
			//Powersavings actived
			if (ad->data_customed_setting) {
				setting_enable_genlist_item(ad->data_customed_setting->item);
			}
		}
	}
	else if (!safeStrCmp(KeyStr_Powersaving_Customed, list_item->keyStr)) {
		setting_view_change(&setting_view_powersaving_main,
				    &setting_view_powersaving_customed, ad);
	} else if (!safeStrCmp(KeyStr_Powersaving_About, list_item->keyStr)) {
		setting_view_change(&setting_view_powersaving_main,
				    &setting_view_powersaving_about, ad);
	}
	else if(!safeStrCmp(KeyStr_CPU_PS, list_item->keyStr))
	{
		list_item->chk_status =
		    !elm_check_state_get(list_item->eo_check);
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
	}
	else if(!safeStrCmp(KeyStr_DISPLAY_PS, list_item->keyStr))
	{
		list_item->chk_status =
		    !elm_check_state_get(list_item->eo_check);
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DISPLAY, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
	}
	else if(!safeStrCmp(KeyStr_BG_COLOR, list_item->keyStr))
	{
		list_item->chk_status =
		    !elm_check_state_get(list_item->eo_check);
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BG_COLOR, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
	}
	else if(!safeStrCmp(KeyStr_SCREEN_VIB, list_item->keyStr))
	{
		list_item->chk_status =
		    !elm_check_state_get(list_item->eo_check);
		elm_check_state_set(list_item->eo_check, list_item->chk_status);
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}

		//take it into effect
		int value = 0;
		if (list_item->chk_status){
			//touch feedback virbration
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
		} else {
			//touch feedback virbration
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
		}
	}
}

static void __setting_powersaving_main_popup_rsp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
	ad->popup = NULL;

	int ret = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS, 0);
	setting_retm_if(0 != ret, "Failed to set vconf value");
}

static void
setting_powersaving_main_click_softkey_back_cb(void *data, Evas_Object *obj,
					       void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "[Setting > Powersaving] Data parameter is NULL");
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;

	int ret = 0;
	int value = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS, &value);
	setting_retm_if(0 != ret, "Failed to get vconf value");

	if (value) {
		int sub_item_value = 0;

		/* check VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU */
		ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, &sub_item_value);
		setting_retm_if(0 != ret, "Failed to get vconf value");
		if (sub_item_value) {
			ug_destroy_me(ad->ug);
			return;
		}

		/* check VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DISPLAY */
		ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DISPLAY, &sub_item_value);
		setting_retm_if(0 != ret, "Failed to get vconf value");
		if (sub_item_value) {
			ug_destroy_me(ad->ug);
			return;
		}

		/* check VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BG_COLOR */
		ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BG_COLOR, &sub_item_value);
		setting_retm_if(0 != ret, "Failed to get vconf value");
		if (sub_item_value) {
			ug_destroy_me(ad->ug);
			return;
		}

		/* check VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB */
		ret = vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &sub_item_value);
		setting_retm_if(0 != ret, "Failed to get vconf value");
		if (sub_item_value) {
			ug_destroy_me(ad->ug);
			return;
		}

		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get, NULL, _(SETTING_POWERSAVING_ALL_SUB_ITEMS_OFF),
				     __setting_powersaving_main_popup_rsp_cb, POPUP_INTERVAL, TRUE, TRUE);
	} else {
		/* Send destroy request */
		ug_destroy_me(ad->ug);
	}
}

static void
setting_powersaving_main_motion_active_chk_btn_cb(void *data, Evas_Object *obj,
						  void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingPowersavingUG *ad = list_item->userdata;
	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	if (!safeStrCmp(KeyStr_Powersaving_Mode, list_item->keyStr)) {
		setting_powersaving_view_disable(ad, !list_item->chk_status);
		int reminder_flag = TRUE;
		int ret =
		    vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS_REMINDER,
				   &reminder_flag);
		if (0 != ret) {
			SETTING_TRACE_ERROR("Failed to get vconf value");
		}
		SETTING_TRACE("reminder_flag:%d", reminder_flag);
		if (list_item->chk_status && reminder_flag) {
			ad->popup =
			    setting_create_popup_with_image_check(ad, ad->win_get,
			    				    NULL,
							    _(POWERSAVING_REMINDER), NULL,NULL,
							    _(NEVER_SHOW_AGAIN),
							    &(ad->popup_chk),
							    __powersaving_on_resp_cb,
							    0,
							    2, _("IDS_COM_SK_OK"),
							    _("IDS_COM_SK_CANCEL"));
			return;
		}

		ret =
		    vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_SYSMODE_STATUS,
				   list_item->chk_status);
		if (0 != ret) {	/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
		if (list_item->chk_status) {//powersaving is on
			int pwrsv_vib = 0;
			vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, &pwrsv_vib);
			if (pwrsv_vib){
				//touch feedback virbration
				int value = 0;
				vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
				vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
				vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
			}
		} else {//powersaving is off
			//touch feedback virbration
			int value = 0;
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
		}
	}
	else if(!safeStrCmp(KeyStr_Powersaving_Customed_Mode, list_item->keyStr))
	{
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_STATUS,
			list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
		if (0 == list_item->chk_status) {
			//not active
			if (ad->data_customed_setting)
			{
				setting_disable_genlist_item(ad->data_customed_setting->item);
			}
		} else {
			//Powersavings actived
			if (ad->data_customed_setting) {
				setting_enable_genlist_item(ad->data_customed_setting->item);
			}
		}
	}
	else if(!safeStrCmp(KeyStr_CPU_PS, list_item->keyStr))
	{
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_CPU, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
	}
	else if(!safeStrCmp(KeyStr_DISPLAY_PS, list_item->keyStr))
	{
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_DISPLAY, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
	}
	else if(!safeStrCmp(KeyStr_BG_COLOR, list_item->keyStr))
	{
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BG_COLOR, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}
		if (list_item->chk_status)
		{
			// do nothing
		}
	}
	else if(!safeStrCmp(KeyStr_SCREEN_VIB, list_item->keyStr))
	{
		int err = vconf_set_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_SCREEN_VIB, list_item->chk_status);
		if (0 != err) {
			/* rollback */
			SETTING_TRACE_ERROR("Set vconf Failed");
			list_item->chk_status = !list_item->chk_status;
			elm_check_state_set(obj, list_item->chk_status);
			return;
		}

		//take it into effect
		int value = 0;
		if (list_item->chk_status){
			//touch feedback virbration
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, value);//backup
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, 0);
		} else {
			//touch feedback virbration
			vconf_get_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_BAK_INT, &value);
			vconf_set_int(VCONFKEY_SETAPPL_TOUCH_FEEDBACK_VIBRATION_LEVEL_INT, value);
		}
	}


	return;
}
static void setting_powersaving_view_disable(void *data, bool disable)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	ret_if(data == NULL);
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;
	if (disable)
	{
		if (ad->data_cpu_ps) setting_disable_genlist_item(ad->data_cpu_ps->item);
		if (ad->data_display_ps) setting_disable_genlist_item(ad->data_display_ps->item);
		if (ad->data_bg_color) setting_disable_genlist_item(ad->data_bg_color->item);
		if (ad->data_screen_vib) setting_disable_genlist_item(ad->data_screen_vib->item);
	}
	else
	{
		if (ad->data_cpu_ps) setting_enable_genlist_item(ad->data_cpu_ps->item);
		if (ad->data_display_ps) setting_enable_genlist_item(ad->data_display_ps->item);
		if (ad->data_bg_color) setting_enable_genlist_item(ad->data_bg_color->item);
		if (ad->data_screen_vib) setting_enable_genlist_item(ad->data_screen_vib->item);
	}
}

