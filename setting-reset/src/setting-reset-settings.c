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
#include <setting-cfg.h>
#include <setting-reset-settings.h>

#define RESULT_FONT_SIZE 28//18
#define ITEM_NAME_FONT_SIZE 35//22

static int setting_reset_settings_create(void *cb);
static int setting_reset_settings_destroy(void *cb);
static int setting_reset_settings_update(void *cb);
static int setting_reset_settings_cleanup(void *cb);

setting_view setting_view_reset_settings = {
	.create = setting_reset_settings_create,
	.destroy = setting_reset_settings_destroy,
	.update = setting_reset_settings_update,
	.cleanup = setting_reset_settings_cleanup,
};

static void setting_reset_settings_click_softkey_reset_cb(void *data,
						      Evas_Object *obj,
						      void *event_info);
static void setting_reset_settings_click_softkey_cancel_cb(void *data,
						       Evas_Object *obj,
						       void *event_info);

static bool setting_reset_settings_create_password_sg(void *data);
static void setting_reset_settings_check_state(Setting_Done_List_Data *list_data);
static void setting_reset_settings_result_password_ug_cb(ui_gadget_h ug,
						     service_h result,
						     void *priv);

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static char *__result_gl_text_get(void *data, Evas_Object *obj,
				   const char *part)
{
	retv_if(!data, NULL);
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) data;
	//SETTING_TRACE("part:%s", part);

	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {	/* title */
		char label[HELP_MSG_BUFF_SIZE + 1] = { 0, };
		if (item_data->keyStr)
		{
			const char *key = setting_customize_text(_(item_data->keyStr), ITEM_NAME_FONT_SIZE, NULL, NULL);
			safeStrNCat(label, key,HELP_MSG_BUFF_SIZE);
			FREE(key);
		}
		if (item_data->sub_desc)
		{
			char desc[SETTING_STR_SLP_LEN + 1] = { 0, };
			snprintf(desc, SETTING_STR_SLP_LEN, ": %s", _(item_data->sub_desc));
			const char *sub_desc = setting_customize_text(desc, RESULT_FONT_SIZE, NULL, NULL);
			safeStrNCat(label, sub_desc,HELP_MSG_BUFF_SIZE);
			FREE(sub_desc);
		}
		return (char *)g_strdup(label);
	}
	return NULL;
}
#if SIMPLY_SUPPORT_ALL_RESET
static void __processing_popup_response_cb(void *data, Evas_Object *obj,
				     void *event_info)
{
	ret_if(!data);
	SettingResetUG *ad = data;
	setting_reset_special_process(ad);
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
	ad->notify= NULL;

#if 0
	// key ungrab: after showing the result popup
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get((Evas_Object*)ug_get_window());
	int ret = utilx_ungrab_key(disp, xwin, KEY_HOME);
	if (ret) {
		SETTING_TRACE_ERROR("KEY_HOME ungrab error ret[%d]", ret);
	}
#endif
}
bool __reset_all_idler(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, FALSE);
	SettingResetUG *ad = data;

	int i = 0;
	int j = 0;
	int idx = 0;
	const char *result_str = RESET_SUCESS_STR;
	char ug_file[PATH_MAX + 1];
	char *path = NULL;
	service_h svc;
	int ret = -1;
	char *ug_args;
	char *item_name;

	for (; i < setting_cfg_get_category_length(); i++) {
		//SETTING_TRACE("group is:%s", _(setting_cfg_get_category_name(i)));

		result_str = RESET_SUCESS_STR;
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			/*SETTING_TRACE("i:%d\tj:%d", i, j);*/
			if (Cfg_Item_unResetable ==
			    setting_cfg_get_resetable_flag_idx(i, j))
			{
				//SETTING_TRACE("process %s:%s skipped",_(item_name), ug_args);
				continue;
			}
			ug_args = setting_cfg_get_ug_args_idx(i, j);
			item_name = setting_cfg_get_keyname_idx(i, j);
			if (item_name)
				SETTING_TRACE(">>> process %s:%s",_(item_name), ug_args);

			idx++;
			if (NULL == ug_args) /*special process*/
			{
				ret = -1;
				if (!safeStrCmp(KeyStr_FlightMode, item_name)) {
					//Invoke aync API and ignore the response message
					ret = setting_reset_flight_mode();
				} else if (!safeStrCmp(KeyStr_Landscape, item_name)) {
					ret = setting_reset_rotation_lock();
				}
				//..
				if (0 > ret)
				{
					result_str = RESET_FAILED_STR;
				}


			}
			else
			{
				path = get_ug_path_from_ug_args((void *)ug_args);
				svc = get_bundle_from_ug_args((void *)ug_args);

				//if (snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH, path) < 0)
				//	return FALSE;

				ret = setting_invoke_reset_function(path, svc, NULL);
				//SETTING_TRACE("load %s[plugin_ret:%d]", ug_file, ret);

				if (OPERATE_LIB_SUCESS > ret)
				{
					result_str = RESET_FAILED_STR;

				}

				service_destroy(svc);
				FREE(path);
				memset(ug_file, 0, PATH_MAX);
			}
			SETTING_TRACE("process %s:%s %s",_(item_name), ug_args, _(result_str));
		}
	}

	//list_data->cur_item_num = idx;
	SETTING_TRACE_DEBUG("Reset totally %d items. result_str:%s", idx, result_str);
	//setting_create_popup_with_notitle(NULL, ad->win_get, _(result_str));
	elm_popup_desc_set(ad->notify, _(result_str));
	elm_popup_mode_set(ad->notify, POPUP_INTERVAL);
	elm_popup_timeout_set(ad->notify, 2);

	if (ad->reset_all_idler) {
		ecore_idler_del(ad->reset_all_idler);
		ad->reset_all_idler = NULL;
	}
	return FALSE;
}

void __reset_all_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	Setting_GenGroupItem_Data *item_Data = data;
	ret_if(!item_Data->userdata);
	SettingResetUG *ad = item_Data->userdata;


	char pw_pass[SETTING_STR_SLP_LEN] = { 0, };
	int err = 0;

	/*  Check Password Setting status */
	int ret =
	    setting_get_string_slp_key(STR_SLP_LIST_PHONE_PASSWORD,
				       pw_pass, &err);
	SETTING_TRACE("pw_pass:%s", pw_pass);
	if (SETTING_RETURN_SUCCESS == ret && '\0' != pw_pass[0]) {
		setting_reset_settings_create_password_sg(ad);
		return;
	}

	// key grab: before showing processing popup
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get((Evas_Object*)ug_get_window());
	ret = utilx_grab_key(disp, xwin, KEY_HOME, TOP_POSITION_GRAB);
	if (ret) {
		SETTING_TRACE_ERROR("KEY_HOME grab error ret[%d]", ret);
	}

	ad->notify =
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_COM_POP_PROCESSING"), __processing_popup_response_cb, 0, 1, 1);
	if (ad->reset_all_idler) {
		ecore_idler_del(ad->reset_all_idler);
		ad->reset_all_idler = NULL;
	}

	ad->reset_all_idler =
	    ecore_idler_add((Ecore_Task_Cb) __reset_all_idler, ad);
}
#endif
static int setting_reset_settings_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;
	Evas_Object *scroller = NULL;

	Evas_Object *view_layout = elm_layout_add(ad->win_main_layout);
	elm_layout_file_set(view_layout, SETTING_THEME_EDJ_NAME,
			    "selinfo_bottom");
	evas_object_size_hint_weight_set(view_layout, EVAS_HINT_EXPAND, 0.0);

	/*  Evas_Object * win_layout, Evas_Object * win_obj */
	Elm_Object_Item *navi_it = setting_push_layout_navi_bar(_(RESET_SETTINGS_STR),
				   _("IDS_COM_BODY_RESET"),
				   _("IDS_COM_SK_CANCEL"), NULL,
				   setting_reset_settings_click_softkey_reset_cb,
				   setting_reset_settings_click_softkey_cancel_cb,
				   NULL, ad, view_layout, ad->navi_bar,
				   NULL);

	/* ******************************************create frame */
	Setting_Done_List_Data *list_data = &(ad->list_data);	/* define a handle */
	int idx = 0;
	list_data->win_main = ad->win_main_layout;
	list_data->UG_data = ad;
	list_data->ly_parent = view_layout;
	list_data->select_all_checked = EINA_FALSE;
	list_data->navi_it = navi_it;

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
			ug_args = setting_cfg_get_ug_args_idx(i, j);
			if (!is_ug_installed_by_ug_args(ug_args))
			{
				continue;
			}

			item_name = setting_cfg_get_keyname_idx(i, j);
			if (isEmulBin())
			{
				if (!safeStrCmp(KeyStr_FlightMode, item_name)) {
					continue; /* hide Flight Mode in Emulator*/
				} else if (!safeStrCmp(KeyStr_WiFi, item_name)) {
					continue; /* hide WIFI in Emulator*/
				} else if (!safeStrCmp(KeyStr_MobileAP, item_name)) {
					continue; /* hide Tethering in Emulator*/
				} else if (!safeStrCmp(KeyStr_WiFiDirect, item_name)) {
					continue; /* hide WiFiDirect in Emulator*/
				} else if (!safeStrCmp(KeyStr_Network, item_name)) {
					continue; /* hide Network in Emulator*/
				} else if (!safeStrCmp(KeyStr_Bluetooth, item_name)) {
					continue; /* hide Bluetooth in Emulator*/
				} else if (!safeStrCmp(KeyStr_DateTime, item_name)) {
					continue; /* hide Date&Time in Emulator*/
				} else if (!safeStrCmp(KeyStr_Security, item_name)) {
					continue; /* hide Security in Emulator*/
				} else if (!safeStrCmp(KeyStr_Memory, item_name)) {
					continue; /* hide Memory in Emulator*/
				} else if (!safeStrCmp(KeyStr_Powersaving, item_name)) {
					continue; /* hide Powersaving in Emulator*/
				} else if (!safeStrCmp(KeyStr_Email, item_name)) {
					continue;
				}
			}

			list_data->chk_items[idx].ug_args = ug_args;
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

	if(idx == 0 || !setting_done_list_is_some_item_selected(list_data)) {
		Evas_Object *reset_btn = elm_object_item_part_content_get(navi_it, "toolbar_button1");
		elm_object_disabled_set(reset_btn, EINA_TRUE);
	}

	ad->view_layout = view_layout;
	ad->reset_list = elm_object_part_content_get(scroller, "search_list");
	/* *******************************************update info */

	setting_view_reset_settings.is_create = 1;
	ad->itc_1text.func.text_get = __result_gl_text_get;
	return SETTING_RETURN_SUCCESS;
}

static int setting_reset_settings_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingResetUG *ad = (SettingResetUG *) cb;

	if (ad->list_data.selInfoPop) {
		evas_object_del(ad->list_data.selInfoPop);
		ad->list_data.selInfoPop = NULL;
	}
	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}
	if (ad->old_notify) {
		evas_object_del(ad->old_notify);
		ad->old_notify = NULL;
	}
	if (ad->pop)
	{
		evas_object_del(ad->pop);
		ad->pop = NULL;
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

	if (ad->reset_all_idler) {
		ecore_idler_del(ad->reset_all_idler);
		ad->reset_all_idler = NULL;
	}

	ad->pop_btn = NULL;

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_reset_settings.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_reset_settings_update(void *cb)
{
	SETTING_TRACE_BEGIN;

	return SETTING_RETURN_SUCCESS;
}

static int setting_reset_settings_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	//SettingResetUG *ad = (SettingResetUG *) cb;

	setting_reset_settings_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
bool __delay_reset_location(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, FALSE);
	Setting_GenGroupItem_Data *item_data = data;
	int plugin_ret = setting_invoke_reset_function("setting-location-efl", NULL, NULL);
	SETTING_TRACE("load libug-%s.so[plugin_ret:%d]", "setting-location-efl", plugin_ret);
	const char *result_str = NULL;
	if (LOAD_LIB_FAILED == plugin_ret) // -1
	{
		result_str = LOAD_LIB_FAILED_STR;

	} else if (UNDEFINED_LIB_SYMBOL == plugin_ret) // -2
	{
		result_str = UNDEFINE_STR;
	} else if (OPERATE_LIB_SUCESS > plugin_ret){ // 0
		result_str = RESET_FAILED_STR;

	} else {
		return FALSE;
	}

	item_data->sub_desc = (char *) g_strdup(_(result_str));
	elm_object_item_data_set(item_data->item, item_data);
	elm_genlist_item_update(item_data->item);
	return FALSE;
}

static void __reset_an_item(Setting_Done_List_Data *list_data,
				const char *item_name,
				const char *ug_args)
{
	SettingResetUG *ad = (SettingResetUG *) list_data->UG_data;
	if(ad == NULL)
		return;

	Evas_Object *genlist = elm_object_content_get(ad->notify);
	Setting_GenGroupItem_Data *item_data = NULL;
	const char *result_str = RESET_SUCESS_STR;
	if (NULL == ug_args) /*special process*/
	{
		int ret = -1;
		if (!safeStrCmp(KeyStr_FlightMode, item_name)) {
			//Invoke aync API and ignore the response message
			ret = setting_reset_flight_mode();
		} else if (!safeStrCmp(KeyStr_Landscape, item_name)) {
			ret = setting_reset_rotation_lock();
		}
		//..
		if (0 > ret)
		{
			result_str = VCONF_RESET_FAILED_STR;
		}

		item_data = setting_create_Gendial_field_def(genlist, &(ad->itc_1text),
						 NULL, ad,
						 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
						 NULL, NULL, 0,
						 (char *)item_name,
						 (char *)result_str, NULL);
	} else
	{
		char *path = get_ug_path_from_ug_args((void *)ug_args);
		service_h svc = get_bundle_from_ug_args((void *)ug_args);
		int plugin_ret = OPERATE_LIB_SUCESS;
		if (!isEmulBin() || 0 != safeStrCmp(KeyStr_Location, item_name)) //KeyStr_Location is too slow to display in emulator
		{
			plugin_ret = setting_invoke_reset_function(path, svc, NULL);
			SETTING_TRACE("load libug-%s.so[plugin_ret:%d]", path, plugin_ret);
		}
		else
		{
			item_data = setting_create_Gendial_field_def(genlist, &(ad->itc_1text),
							 NULL, ad,
							 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
							 NULL, NULL, 0,
							 (char *)item_name,
							 (char *)result_str, NULL);
			ecore_idler_add((Ecore_Task_Cb) __delay_reset_location, item_data);

			service_destroy(svc);
			FREE(path);
			return;
		}

		if (LOAD_LIB_FAILED == plugin_ret) // -1
		{
			result_str = LOAD_LIB_FAILED_STR;

		} else if (UNDEFINED_LIB_SYMBOL == plugin_ret) // -2
		{
			result_str = UNDEFINE_STR;
		} else if (OPERATE_LIB_SUCESS <= plugin_ret){ // 0
			result_str = RESET_SUCESS_STR;

		} else {
			result_str = RESET_FAILED_STR;
		}

		service_destroy(svc);
		if (path){
			FREE(path);
			path = NULL;
		}

		item_data = setting_create_Gendial_field_def(genlist, &(ad->itc_1text),
						 NULL, ad,
						 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
						 NULL, NULL, 0,
						 (char *)item_name,
						 (char *)result_str, NULL);
	}

	if (item_data)
		elm_genlist_item_show(item_data->item, ELM_GENLIST_ITEM_SCROLLTO_IN);//show the last item
	else
		SETTING_TRACE_ERROR("error : item_data is NULL");
}

static Eina_Bool __reset_animator_cb(void *data)
{
	SettingResetUG *ad = (SettingResetUG *) data;

	SETTING_TRACE("Processing the %dth of %d",
		      ad->gl_index, ad->list_data.cur_item_num);
	if (ad->gl_index >=  ad->list_data.cur_item_num) {
		ecore_animator_del(ad->animator);
		ad->animator = NULL;
		if (ad->pop_progress) {
			evas_object_del(ad->pop_progress);
			ad->pop_progress = NULL;
		}
		//Evas_Object *btn = elm_object_part_content_get(ad->notify, "button1");
		if (ad->pop_btn)
		{
			setting_enable_evas_object(ad->pop_btn);
			setting_undo_dim_evas_object(ad->pop_btn, TRUE);
		}

		Evas_Object *genlist = elm_object_content_get(ad->notify);
		if(genlist)
			elm_genlist_item_show(elm_genlist_first_item_get(genlist), ELM_GENLIST_ITEM_SCROLLTO_TOP);
		if(ad->reset_list)
			elm_genlist_item_show(elm_genlist_first_item_get(ad->reset_list), ELM_GENLIST_ITEM_SCROLLTO_TOP);

		if(!setting_done_list_is_some_item_selected(&ad->list_data)) {
			if (ad->list_data.navi_it) {
				Evas_Object *button = elm_object_item_part_content_get(ad->list_data.navi_it, "toolbar_button1");
				elm_object_disabled_set(button, EINA_TRUE);
			}
		}
		return ECORE_CALLBACK_CANCEL;
	}
	if (ad->list_data.chk_items[ad->gl_index].data_GenItem->chk_status) {
		elm_genlist_item_show(ad->list_data.chk_items[ad->gl_index].data_GenItem->item, ELM_GENLIST_ITEM_SCROLLTO_IN);//show the last item
		__reset_an_item(&(ad->list_data),
				ad->list_data.chk_items[ad->gl_index].item_name,
				ad->list_data.chk_items[ad->gl_index].ug_args);
		/*unselect item*/
		elm_check_state_set(ad->list_data.chk_items[ad->gl_index].data_GenItem->eo_check, 0);
		ad->list_data.chk_items[ad->gl_index].data_GenItem->chk_status = 0;
	}

	ad->gl_index++;
	return ECORE_CALLBACK_RENEW;
}

bool __reset_delay_idler(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, FALSE);
	SettingResetUG *ad = data;
	ad->gl_index = 0;
	ad->animator = ecore_animator_add(__reset_animator_cb, ad);
	return FALSE;
}

static void setting_reset_settings_check_state(Setting_Done_List_Data *list_data)
{
	/* reset */
	SETTING_TRACE_DEBUG("********resetting*******");

	SettingResetUG *ad = (SettingResetUG *) list_data->UG_data;
	ad->notify = elm_popup_add(ad->win_get);
	elm_object_style_set(ad->notify,"menustyle");
	elm_object_part_text_set(ad->notify, "title,text", _(RESET_RESULT_STR));
	evas_object_size_hint_weight_set(ad->notify,
				EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
	Evas_Object *btn = setting_create_button(ad->notify, _("IDS_COM_SK_OK"), NULL, setting_reset_result_popup_resp_cb, ad);
	elm_object_part_content_set(ad->notify, "button1", btn);
	elm_object_style_set(btn, "popup_button/default");
	setting_disable_evas_object(btn);
	setting_dim_evas_object(btn, TRUE);
	ad->pop_btn = btn;

	Evas_Object *genlist = elm_genlist_add(ad->notify);
	elm_object_style_set(genlist, "dialogue");
	evas_object_size_hint_weight_set(genlist,
				EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist,
				EVAS_HINT_FILL,
				EVAS_HINT_FILL);
	/*  add separator */
	setting_create_Gendial_field_titleItem(genlist,
					     &itc_group_item,
					     RESET_RESULT_STR,
					     NULL);
	//ad->gl_index = 0;
	//ad->animator = ecore_animator_add(__reset_animator_cb, ad);
	//Don't append any item until the popup is fully showed up
	ecore_idler_add((Ecore_Task_Cb) __reset_delay_idler, ad);
	elm_object_content_set(ad->notify, genlist);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	evas_object_show(ad->notify);

	/* reset sel_all item */
	elm_check_state_set(list_data->select_all_checkbox, 0);
	list_data->select_all_checked = EINA_FALSE;

}

static void
setting_reset_settings_result_password_ug_cb(ui_gadget_h ug, service_h result,
					 void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data parameter is NULL");

	SettingResetUG *ad = (SettingResetUG *) priv;

	char *ret_str = NULL;

	service_get_extra_data(result, "result", &ret_str);

	retm_if(ret_str == NULL, "result paremeter is NULL");
	SETTING_TRACE("result:%s", ret_str);

	if (ret_str) {
		if (0 == safeStrCmp("Cancel", ret_str)) {
			/* Do not reset */
		}
		else if (0 == safeStrCmp("SETTING_PW_TYPE_RESET", ret_str)) {
			/*  Reset */
#if SIMPLY_SUPPORT_ALL_RESET
			ad->notify =
				setting_create_popup_witout_btn(ad, ad->win_get, _("IDS_COM_POP_PROCESSING"), NULL, __processing_popup_response_cb, 0);
			if (ad->reset_all_idler) {
				ecore_idler_del(ad->reset_all_idler);
				ad->reset_all_idler = NULL;
			}
			ad->reset_all_idler =
			    ecore_idler_add((Ecore_Task_Cb) __reset_all_idler, ad);
#else
			/* if return value from passwordUG, create popup for resetting. */
			setting_reset_settings_check_state(&(ad->list_data));
#endif
		}
	}
}

static void
setting_reset_settings_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
				void *priv)
{
	SettingResetUG *ad = (SettingResetUG *) priv;
	Evas_Object *base;

	if (!priv)
		return;
	SETTING_TRACE_BEGIN;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
						 EVAS_HINT_EXPAND);
		//elm_win_resize_object_add(ad->win_get, base);
		evas_object_show(base);
		break;
	default:
		break;
	}
	SETTING_TRACE_END;
}

static void setting_reset_settings_destroy_password_ug_cb(ui_gadget_h ug,
						      void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(!priv);
	SettingResetUG *ad = (SettingResetUG *) priv;
	if (ug) {
		ug_destroy(ug);
		ad->ug_loading = NULL;
	}

}

static bool setting_reset_settings_create_password_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingResetUG *ad = (SettingResetUG *) data;

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs)
		return FALSE;
	cbs->layout_cb = setting_reset_settings_layout_ug_cb;
	cbs->result_cb = setting_reset_settings_result_password_ug_cb;
	cbs->destroy_cb = setting_reset_settings_destroy_password_ug_cb;
	cbs->priv = (void *)ad;

	service_h svc;
	if (service_create(&svc)) {
		FREE(cbs);
		return FALSE;
	}

	service_add_extra_data(svc, "viewtype", "SETTING_PW_TYPE_RESET");

	ad->ug_loading =
	    ug_create(ad->ug, "setting-password-efl", UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_loading) {	/* error handling */
	}

	service_destroy(svc);
	FREE(cbs);

	return TRUE;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void
setting_reset_settings_click_softkey_cancel_cb(void *data, Evas_Object *obj,
					   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingResetUG *ad = (SettingResetUG *) data;

	setting_view_change(&setting_view_reset_settings, &setting_view_reset_main, ad);
}

static void
setting_reset_settings_click_softkey_reset_cb(void *data, Evas_Object *obj,
					  void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingResetUG *ad = (SettingResetUG *) data;
	//int screen_lock_type = 0;

	//every time clicking 'Reset' button, restore following VCONFs
	//vconf_set_bool(VCONFKEY_SETAPPL_FONT_CHANGED, FALSE);

	if (ad->list_data.selInfoPop) {
		evas_object_del(ad->list_data.selInfoPop);
		ad->list_data.selInfoPop = NULL;
	}

	if (!setting_done_list_is_some_item_selected(&(ad->list_data))) {
		setting_create_simple_popup(NULL, ad->win_get, NULL, "No item selected");
	} else {

		setting_reset_settings_check_state(&(ad->list_data));

	}
}
