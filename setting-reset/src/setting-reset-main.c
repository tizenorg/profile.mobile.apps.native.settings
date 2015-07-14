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

#include <setting-reset-main.h>
#include <sysman.h>


#define RESULT_FONT_SIZE 28//18
#define ITEM_NAME_FONT_SIZE 35//22

static int setting_reset_main_create(void *cb);
static int setting_reset_main_destroy(void *cb);
static int setting_reset_main_update(void *cb);
static int setting_reset_main_cleanup(void *cb);

setting_view setting_view_reset_main = {
	.create = setting_reset_main_create,
	.destroy = setting_reset_main_destroy,
	.update = setting_reset_main_update,
	.cleanup = setting_reset_main_cleanup,
};

static void setting_reset_main_click_softkey_cancel_cb(void *data,
						       Evas_Object *obj,
						       void *event_info);
#if SUPPORT_RESET_DETAILS
static void setting_reset_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info);
#endif

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_reset_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;
	Evas_Object *scroller = NULL;
#if SIMPLY_SUPPORT_ALL_RESET
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _("IDS_COM_BODY_RESET"),
						   _("IDS_COM_BODY_BACK"), NULL,
						   setting_reset_main_click_softkey_cancel_cb,
						   NULL, ad, &scroller,
						   &(ad->navi_bar));
	/* Description */
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text), NULL,
					 ad, SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, RESET_ALL_DESC,
					 NULL, NULL);
	elm_genlist_item_select_mode_set(elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL),
					 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	/* Button */
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) calloc(1,
						 sizeof
						 (Setting_GenGroupItem_Data));
	setting_retvm_if(!item_data, SETTING_RETURN_FAIL, "calloc failed");
	item_data->userdata = ad;
	item_data->keyStr = (char *)g_strdup(RESET_ALL);
	item_data->swallow_type = SWALLOW_Type_1BUTTON;

	item_data->chk_change_cb = __reset_all_button_cb;
	item_data->item = elm_genlist_item_append(scroller, &(ad->itc_1icon), item_data, NULL,
						    ELM_GENLIST_ITEM_NONE,
						    NULL, NULL);
	setting_view_reset_main.is_create = 1;
	ad->main_scroller = scroller;
	return SETTING_RETURN_SUCCESS;
#endif

#if SUPPORT_RESET_DETAILS
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _("IDS_COM_BODY_RESET"),
						   _("IDS_COM_BODY_BACK"), NULL,
						   setting_reset_main_click_softkey_cancel_cb,
						   NULL, ad, &scroller,
						   &(ad->navi_bar));

	Elm_Object_Item *item = NULL;;

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* 1. Reset settings */
	setting_create_Gendial_field_def(scroller, &itc_1text,
								setting_reset_main_mouse_up_Gendial_list_cb,ad,
								SWALLOW_Type_INVALID,
								NULL, NULL,
								0, RESET_SETTINGS_STR, NULL, NULL);
#if SUPPORT_SIM_CARD_PROFILE
	/* 2. SIM card profile settings */
	#ifdef TARGET
	int is_sim_profile_enabled = csc_svc_sim_auto_setting_chk_feature_enable();
	int err = 0;
	int value = VCONFKEY_TELEPHONY_SIM_UNKNOWN;
	setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	if (is_sim_profile_enabled && VCONFKEY_TELEPHONY_SIM_INSERTED == value) {
		setting_create_Gendial_field_def(scroller, &itc_1text,
								setting_reset_main_mouse_up_Gendial_list_cb,ad,
								SWALLOW_Type_INVALID,
								NULL, NULL,
								0, SIM_CARD_PROFILE_SETTINGS_STR, NULL, NULL);
	} else {
		SETTING_TRACE("is_sim_profile_enabled:%d", is_sim_profile_enabled);
		SETTING_TRACE("value:%d", value);
	}
	#else
	SETTING_TRACE("TARGET MACRO disabled");
	#endif

#endif

#if DISABLED_CODE
	/* Description */
	setting_create_Gendial_field_def(scroller, &itc_multiline_text, NULL,
					 ad, SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, RESET_ALL_DESC,
					 NULL, NULL);
#endif
	setting_view_reset_main.is_create = 1;
	ad->main_scroller = scroller;
	return SETTING_RETURN_SUCCESS;

#endif

#if SUPPORT_RESET_OLD
	Evas_Object *navi_bar = NULL;

	Evas_Object *controllbar = NULL;

	Evas_Object *view_layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(view_layout, SETTING_THEME_EDJ_NAME,
			    "selinfo_bottom");
	evas_object_size_hint_weight_set(view_layout, EVAS_HINT_EXPAND, 0.0);

	/*  Evas_Object * win_layout, Evas_Object * win_obj */
	ad->ly_main =
	    setting_create_layout_navi_bar(ad->win_main_layout, ad->win_get,
					   _("IDS_COM_BODY_RESET"),
					   _("IDS_COM_SK_CANCEL"),
					   _("IDS_COM_BODY_RESET"), NULL,
					   setting_reset_main_click_softkey_cancel_cb,
					   setting_reset_main_click_softkey_reset_cb,
					   NULL, ad, view_layout, &navi_bar,
					   &controllbar);
	elm_object_item_disabled_set(elm_toolbar_last_item_get
					 (controllbar), EINA_TRUE);

	/* ******************************************create frame */
	Setting_Done_List_Data *list_data = &(ad->list_data);	/* define a handle */
	int idx = 0;
	list_data->win_main = ad->ly_main;
	list_data->UG_data = ad;
	list_data->ly_parent = view_layout;
	list_data->controllbar = controllbar;


	list_data->chk_items[idx++].item_name = KeyStr_Select_All;
	char *ug_args;
	char *item_name;

	int i = 0;
	int j = 0;

	for (; i < setting_cfg_get_category_length(); i++) {
		SETTING_TRACE("group is:%s", _(setting_cfg_get_category_name(i)));

		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			/*SETTING_TRACE("i:%d\tj:%d", i, j);*/
			if (Cfg_Item_unResetable ==
			    setting_cfg_get_resetable_flag_idx(i, j))
			{
			    continue;
			}
			list_data->chk_items[idx].ug_args = ug_args =
					setting_cfg_get_ug_args_idx(i, j);
			list_data->chk_items[idx].item_name = item_name =
					setting_cfg_get_keyname_idx(i, j);

			SETTING_TRACE("process %s:\t%s",_(item_name), ug_args);
			idx++;
		}
	}

	list_data->cur_item_num = idx;
	SETTING_TRACE("list_data->cur_item_num:%d", list_data->cur_item_num);
	scroller = setting_create_done_list(list_data);
	elm_object_part_content_set(view_layout, "elm.swallow.contents", scroller);

	ad->view_layout = view_layout;
	ad->navi_bar = navi_bar;
	ad->controllbar = controllbar;
	/* *******************************************update info */

	setting_view_reset_main.is_create = 1;
	ad->itc_1text.func.text_get = __result_gl_text_get;
	return SETTING_RETURN_SUCCESS;
#endif
}

static int setting_reset_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;
	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}
	if (ad->old_notify) {
		evas_object_del(ad->old_notify);
		ad->old_notify = NULL;
	}
	if (ad->pop_progress)
	{
		evas_object_del(ad->pop_progress);
		ad->pop_progress = NULL;
	}

	if (ad->ask_popup)
	{
		evas_object_del(ad->ask_popup);
		ad->ask_popup = NULL;
	}
	if (ad->animator)
	{
		ecore_animator_del(ad->animator);
		ad->animator = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		setting_view_reset_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_reset_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_reset_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_hide(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

static void setting_reset_reboot_resp_cb(void *data, Evas_Object *obj,
					void *event_info)
{
	SETTING_TRACE_BEGIN;
	int response_type = btn_type(obj);
	if (POPUP_RESPONSE_OK == response_type) { 			// OK cliked
		csc_svc_sim_auto_setting_clear_active_operator();
		// reboot here
		sysman_request_reboot();

	} else if (POPUP_RESPONSE_CANCEL == response_type) { // cancel clicked
		// DO NOTHING
	}

	SettingResetUG *ad = data;
	if (ad->pop)
	{
		evas_object_del(ad->pop);
		ad->pop = NULL;
	}
}

#if SUPPORT_RESET_DETAILS
static void
setting_reset_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingResetUG *ad = (SettingResetUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(RESET_SETTINGS_STR, list_item->keyStr))
	{
		setting_view_change(&setting_view_reset_main, &setting_view_reset_settings, ad);
	}
#if SUPPORT_SIM_CARD_PROFILE
	else if (!safeStrCmp(SIM_CARD_PROFILE_SETTINGS_STR, list_item->keyStr))
	{
		SETTING_TRACE("SIM-CARD PROFILE");
		#ifdef TARGET
		ad->pop = setting_create_popup_with_btn(ad, ad->win_get, NULL,
					 _("IDS_ST_BODY_RESET_SIM_CARD_PROFILE_SETTINGS_TO_DEFAULT_AND_SELECT_ANOTHER_SERVICE_PROVIDER_MSG"),
					 //setting_time_reboot_resp_cb,
					 setting_reset_reboot_resp_cb,
					 0, 2,
			         dgettext("sys_string", "IDS_COM_SK_OK"),
			         _("IDS_COM_SK_CANCEL"));
		#endif
	}
#endif
	/* to do : */
}
#endif

static void
setting_reset_main_click_softkey_cancel_cb(void *data, Evas_Object *obj,
					   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingResetUG *ad = (SettingResetUG *) data;
	if (ad->list_data.selInfoPop) {
		evas_object_del(ad->list_data.selInfoPop);
		ad->list_data.selInfoPop = NULL;
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

