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
#include <setting-phone-display-language.h>

#include <setting-common-draw-widget.h>
#include <eventsystem.h>

static int setting_phone_display_language_create(void *cb);
static int setting_phone_display_language_destroy(void *cb);
static int setting_phone_display_language_cleanup(void *cb);


setting_view setting_view_phone_display_language = {
	.create = setting_phone_display_language_create,
	.destroy = setting_phone_display_language_destroy,
	.update = NULL,
	.cleanup = setting_phone_display_language_cleanup,
};




/**
 * @brief Callback of cancel button by caller
 *
 * @param data The view data passed between all callbacks
 */
static void setting_phone_display_language_caller_exist_right_cb(void *data,
                                                                 Evas_Object *obj,
                                                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingPhoneUG *ad = (SettingPhoneUG *) data;
	/*  Create Bundle and send message */
	app_control_h svc;
	if (app_control_create(&svc)) {
		return;
	}

	app_control_add_extra_data(svc, "result", "rbutton_click");
	ug_send_result(ad->ug, svc);

	app_control_destroy(svc);

	ug_destroy_me(ad->ug);
}



/**
 * @brief Callback of cancel button
 *
 * @param data The view data passed between all callbacks
 * @param it Naviframe item
 */
static Eina_Bool setting_phone_display_language_click_softkey_cancel_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;
	setting_view_change(&setting_view_phone_display_language, &setting_view_phone_language_region, ad);

	return EINA_TRUE;
}



/**
 * @brief Callback of pop_progress_lang timeout
 *
 * @param data The view data passed between all callbacks
 */
void setting_phone_display_language_done_popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;

	if (ad->pop_progress_lang) {
		evas_object_del(ad->pop_progress_lang);
		ad->pop_progress_lang = NULL;
	}

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_phone_display_language.is_create = 0;
}


/**
 * @brief function of item click
 *
 * @param data The view data passed between all callbacks
 */
static void setting_phone_display_language_close_popup_ex(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;

	if (ad->selected_locale == NULL) {
		return;
	}

	if (safeStrCmp(ad->selected_locale, "IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC") == 0) {

		/* [control] set automatic - TRUE */
		vconf_set_bool(VCONFKEY_SETAPPL_LANG_AUTOMATIC_BOOL, TRUE);

		/* [control] set vconf language */
		if (ad->sim_lang != NULL) {
			set_language_helper(ad->sim_lang);
			/*vconf_set_str(VCONFKEY_LANGSET, ad->sim_lang); */
			elm_language_set(ad->sim_lang);
			SETTING_TRACE_DEBUG("select Automatic : sim_lang is %s", ad->sim_lang);
		}
	} else {
		/* create loop operation here */
		Eina_List *elist = NULL;
		/*Eina_List* tmplist = NULL; */
		setting_lang_entry *pnode = NULL;

		/* load language table from XML file */
		Eina_List *langlist = setting_get_language_list();

		EINA_LIST_FOREACH(langlist, elist, pnode) {
			if (safeStrCmp(ad->selected_locale, pnode->title) == 0) {
				/* [control] set automatic - TRUE */
				vconf_set_bool(VCONFKEY_SETAPPL_LANG_AUTOMATIC_BOOL, FALSE);

				char temp[32] = {0,};
				snprintf(temp, 32, "%s.UTF-8", pnode->locale);
				/* [control] set vconf language */
				set_language_helper(temp);
				/*vconf_set_str(VCONFKEY_LANGSET, temp); */
				elm_language_set(temp);

				int region_automatic = 1;
				vconf_get_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, &region_automatic);
				if (region_automatic == 1) {
					/* [control] region format set - if 'automatic region' */
					set_regionformat_helper(temp);
					/*vconf_set_str(VCONFKEY_REGIONFORMAT, temp); */

					int ret = setting_phone_region_format_set_dateformat(pnode->locale, ad);
					if (ret == SETTING_RETURN_FAIL) {
						SETTING_TRACE_ERROR("[Error] set date format fail");
					}
				}
				SETTING_TRACE_DEBUG("selectd language is %s, its locale is %s", ad->selected_locale, pnode->locale);
			}
		}
	}

	FREE(ad->selected_locale);

	/* 2. GET SELECTED LANG */
	const char *pa_lang = vconf_get_str(VCONFKEY_LANGSET);
	if (pa_lang) {	/* remove the */
		char *q = strchr(pa_lang, '.');
		if (q) {
			*q = '\0';
		}
	}
	SETTING_TRACE("pa_lang:%s", pa_lang);
	int err = -1;

	/* 3. SET DEFAULT */
	/* default UI language */
	uloc_setDefault(pa_lang, &err);

	char *caller = NULL;
	app_control_h service = ad->bundle_data;

	app_control_get_extra_data(service, "caller", &caller);

	if (caller) {
		int ret = setting_phone_region_format_set_dateformat(pa_lang, ad);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("[Error] set date format fail");
		}

		const char *lang = vconf_get_str(VCONFKEY_LANGSET);
		if (lang) {
			ret = set_regionformat_helper(lang);
			/*ret = vconf_set_str(VCONFKEY_REGIONFORMAT, lang); */
			if (ret != 0) {
				SETTING_TRACE_ERROR("[Error] set value of VCONFKEY_REGIONFORMAT fail");
			}
			FREE(lang);
		}

		/*  Create Bundle and send message */
		app_control_h svc;
		if (app_control_create(&svc)) {
			return;
		}

		app_control_add_extra_data(svc, "result", "rbutton_click");
		ug_send_result(ad->ug, svc);

		app_control_destroy(svc);

		ug_destroy_me(ad->ug);
		FREE(caller);

	} else {
		ad->pop_progress_lang = setting_create_popup_with_progressbar_withobject(ad, ad->pop_progress_lang, ad->ly_language,
		                                                                         PROGRESSBAR_STYLE,
		                                                                         NULL, KeyStr_Loading, setting_phone_display_language_done_popup_resp_cb, 3/*0*/, TRUE, TRUE);	/* 3 seconds to wait in maximum */
	}

	FREE(pa_lang);
}



/**
 * @brief Callback of language item click event
 *
 * @param data The view data passed between all callbacks
 * @param event_info The select genlist item
 */
static void setting_phone_display_language_mouse_up_Gendial_list_radio_cb(void *data,
                                                                          Evas_Object *obj,
                                                                          void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	ad->selected_locale = (char *)strdup(list_item->keyStr);
	setting_phone_display_language_close_popup_ex(ad);
}




/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

/**
 * @brief Callback of view creating
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_display_language_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhoneUG *ad = (SettingPhoneUG *) cb;
	/*char *title = "IDS_ST_HEADER_LANGUAGE_AND_INPUT"; */
	char *title = "IDS_ST_HEADER_DISPLAY_LANGUAGE";

	Evas_Object *scroller = NULL;
	setting_call_back_func gl_sel_cb = NULL;

	if (&setting_view_phone_display_language == ad->view_to_load) {
		char *caller = NULL;

		app_control_h service = ad->bundle_data;

		app_control_get_extra_data(service, "caller", &caller);

		if (caller) {
			ad->ly_language = setting_create_layout_navi_bar_genlist(ad->win_main_layout,
			                                                         ad->win_get,
			                                                         title,
			                                                         "IDS_ST_BUTTON_BACK", NULL,
			                                                         setting_phone_display_language_caller_exist_right_cb, NULL,
			                                                         ad, &scroller, &ad->navi_bar);
			gl_sel_cb = setting_phone_display_language_mouse_up_Gendial_list_radio_cb;
			FREE(caller);
		} else {
			SETTING_TRACE_ERROR("[ERROR] caller is empty");
			return SETTING_RETURN_FAIL;
		}
	} else {
		/* add basic layout */
		Elm_Object_Item *navi_it = setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
		                                                                title, "IDS_ST_BUTTON_BACK",
		                                                                NULL,
		                                                                NULL,
		                                                                NULL, ad, &scroller, ad->navi_bar);
		elm_naviframe_item_pop_cb_set(navi_it, setting_phone_display_language_click_softkey_cancel_cb, ad);
		gl_sel_cb = setting_phone_display_language_mouse_up_Gendial_list_radio_cb;
	}

	ad->gl_lang = scroller;
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);

	evas_object_smart_callback_add(ad->gl_lang, "realized", __gl_realized_cb, NULL);

	Elm_Object_Item *item = NULL;
	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* scroller is a genlist */
	ad->chk_lang = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->chk_lang, -1);

	/* create loop operation here */
	Eina_List *elist = NULL;
	setting_lang_entry *pnode = NULL;
	int idx = SETTING_LANG_KOREA;		/* 2 */
	int item_idx = 0;
	Setting_GenGroupItem_Data *last_item = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;

	/* load language table from XML file */
	Eina_List *langlist = setting_get_language_list();

	EINA_LIST_FOREACH(langlist, elist, pnode) {
		item_data =
		    setting_create_Gendial_field_def(scroller,
		                                     &(ad->itc_1text),
		                                     gl_sel_cb,
		                                     ad,
		                                     SWALLOW_Type_INVALID, NULL, NULL,
		                                     /*idx,			// <<< WARNING */
		                                     0,
		                                     pnode->title, NULL, NULL);
		if (item_data) {
			item_data->userdata = ad;
			if (item_idx == 0) {
				item_data->group_style = SETTING_GROUP_STYLE_TOP;
			} else {
				item_data->group_style = SETTING_GROUP_STYLE_CENTER;
			}

			if (item_data->item) {
				if (item_data->group_style == SETTING_GROUP_STYLE_TOP) {
					elm_object_item_signal_emit(item_data->item, "elm,state,top", "");
				} else if (item_data->group_style == SETTING_GROUP_STYLE_BOTTOM) {
					elm_object_item_signal_emit(item_data->item, "elm,state,bottom", "");
				} else if (item_data->group_style == SETTING_GROUP_STYLE_CENTER) {
					elm_object_item_signal_emit(item_data->item, "elm,state,center", "");
				} else {
					elm_object_item_signal_emit(item_data->item, "elm,state,normal", "");
				}
			}
			item_idx++;
			last_item = item_data;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
		idx++;
	}
	if (last_item) {
		last_item->group_style = SETTING_GROUP_STYLE_BOTTOM;
	}

	/*item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL, NULL,
				ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	setting_view_phone_display_language.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}



/**
 * @brief Callback of view destroy
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_display_language_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhoneUG *ad = (SettingPhoneUG *) cb;

	evas_object_smart_callback_del(ad->gl_lang, "realized", __gl_realized_cb);

	/*  FIXED : destroy only if it was created. */
	if (setting_view_phone_display_language.is_create) {
		if (ad->pop_progress_lang) {
			evas_object_del(ad->pop_progress_lang);
			ad->pop_progress_lang = NULL;
		}
		elm_naviframe_item_pop(ad->navi_bar);
		setting_view_phone_display_language.is_create = 0;

		setting_get_language_list_destroy();
	}

	return SETTING_RETURN_SUCCESS;
}



/**
 * @brief Callback of view cleanup
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_display_language_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_phone_display_language_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

