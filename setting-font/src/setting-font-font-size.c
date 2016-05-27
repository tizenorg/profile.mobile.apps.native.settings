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

#include <setting-font-font-size.h>


#define STR_FONT_SIZE_CHANGING "IDS_ST_BODY_CHANGING_THE_FONT_SIZE_IN_ACCESSIBILITY_SETTINGS_WILL_OVERRIDE_THE_FONT_SIZE_IN_EACH_APPLICATION"

static Eina_Bool __setting_font_font_size_click_softkey_back_cb(void *data, Elm_Object_Item *it);

static int setting_font_font_size_create(void *cb);
static int setting_font_font_size_destroy(void *cb);
static int setting_font_font_size_update(void *cb);
static int setting_font_font_size_cleanup(void *cb);

setting_view setting_view_font_font_size = {
	.create = setting_font_font_size_create,
	.destroy = setting_font_font_size_destroy,
	.update = setting_font_font_size_update,
	.cleanup = setting_font_font_size_cleanup,
};

static char *result_str_arr[] = {"Small", "Normal", "Large", "Huge", "Giant"};

static int convert_font_size(int size)
{
	int font_size;
	if (SYSTEM_SETTINGS_FONT_SIZE_SMALL == size) {
		font_size = LABEL_FONT_SIZE_SMALL;
	} else if (SYSTEM_SETTINGS_FONT_SIZE_LARGE == size) {
		font_size = LABEL_FONT_SIZE_LARGE;
	} else if (SYSTEM_SETTINGS_FONT_SIZE_HUGE == size) {
		font_size = LABEL_FONT_SIZE_HUGE;
	} else if (SYSTEM_SETTINGS_FONT_SIZE_GIANT == size) {
		font_size = LABEL_FONT_SIZE_GIANT;
	} else {
		font_size = LABEL_FONT_SIZE_NORMAL;
	}
	return font_size;
}

char *get_font_size_name_by_id(char *font_size_id)
{
	SETTING_TRACE_BEGIN;
	int i = 0;
	while (font_size_table[i].key_font_name) {
		if (!safeStrCmp(font_size_table[i].key_font_name, font_size_id)) {
			return setting_customize_text(_(font_size_table[i].key_font_name), convert_font_size(font_size_table[i].font_size), NULL, NULL);
		}
		i++;
	}
	return NULL;

}
char *_item_text_font_size_keystr2_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (!strcmp(part, "elm.text.main.left")) {
		if (item_data->keyStr2) {
			/*SETTING_TRACE("gl update item_data->keyStr2[%s]",item_data->keyStr2); */
			return get_font_size_name_by_id(item_data->keyStr2);
		}
	}
	SETTING_TRACE_END;
	return NULL;
}

/* ***************************************************
**
** basic func
**
** **************************************************/

static int setting_font_font_size_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/*error check */
	if (!cb) {
		SETTING_TRACE_ERROR("cb == NULL");
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;
	}


	SettingFontUG *ad = (SettingFontUG *) cb;
	Evas_Object *scroller;
	setting_create_Gendial_itc("dialogue/1text.1icon.3.tb", &(ad->itc_1text_1icon_2_font_size));
	ad->itc_1text_1icon_2_font_size.func.text_get = _item_text_font_size_keystr2_get;
	retvm_if(ad->win_main_layout == NULL, SETTING_DRAW_ERR_FAIL_LOAD_EDJ,
			 "win_main_layout is NULL");

	/* create a navigation bar */
	if (ad->view_to_load == &setting_view_font_font_size) {
		ad->ly_main = setting_create_layout_navi_bar_genlist(ad->win_main_layout,
															 ad->win_get,
															 "IDS_ST_MBODY_FONT_SIZE",
															 _("IDS_ST_BUTTON_BACK"), NULL,
															 (setting_call_back_func)__setting_font_font_size_click_softkey_back_cb, NULL,
															 ad, &scroller, &ad->navibar);
		ad->navi_it_font_size = elm_naviframe_top_item_get(ad->navibar);
	} else {
		ad->navi_it_font_size = setting_push_layout_navi_bar_genlist(ad->win_main_layout,
																	 ad->win_get,
																	 "IDS_ST_MBODY_FONT_SIZE",
																	 _("IDS_ST_BUTTON_BACK"),
																	 NULL,
																	 (setting_call_back_func)__setting_font_font_size_click_softkey_back_cb,
																	 NULL,
																	 ad, &scroller, ad->navibar);
	}

	/* [UI] create the font list */
	ad->size_rdg = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->size_rdg, -1);

	int idx = 0;

	while (font_size_table[idx].key_font_name) {
		Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
		setting_retvm_if(!item_data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "calloc failed");
		item_data->keyStr2 = (char *)g_strdup(font_size_table[idx].key_font_name);/*setting_customize_text(_(font_size_table[idx].key_font_name), convert_font_size(font_size_table[idx].font_size),NULL,NULL);//for display */
		item_data->keyStr = (char *)g_strdup(_(font_size_table[idx].key_font_name));/*for tts feature; */
		item_data->swallow_type = SWALLOW_Type_1CHECK_RIGHT;

		item_data->chk_status = font_size_table[idx].font_size;
		item_data->chk_change_cb = NULL;
		item_data->rgd = ad->size_rdg;
		item_data->userdata = ad;

		item_data->item = elm_genlist_item_append(scroller, &(ad->itc_1text_1icon_2_font_size), item_data, NULL,
												  ELM_GENLIST_ITEM_NONE, setting_font_font_size_list_mouse_up_cb, ad->size_rdg);

		idx++;
	}


	/* [UI] help text */
	setting_add_gl_help(scroller, STR_FONT_SIZE_CHANGING);

	/* update check status - init values */
	setting_update_chk_status(ad->size_rdg, INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE);
	setting_view_font_font_size.is_create = 1;
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, ad);
	return SETTING_RETURN_SUCCESS;
}

static int setting_font_font_size_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/*error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingFontUG *ad = (SettingFontUG *) cb;


	if (ad->view_to_load == &setting_view_font_font_size) {
		if (ad->ly_main) {
			evas_object_del(ad->ly_main);
			ad->ly_main = NULL;
		}
	} else {
		elm_naviframe_item_pop(ad->navibar);
	}
	if (ad->navi_it_font_size) {
		ad->navi_it_font_size = NULL;
	}
	if (ad->size_rdg != NULL) {
		ad->size_rdg = NULL;
	}
	setting_view_font_font_size.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_font_font_size_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* do nothing here */
	return SETTING_RETURN_SUCCESS;
}

static int setting_font_font_size_cleanup(void *cb)
{
	/*error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_font_font_size_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
**
** general func
**
** **************************************************/

/* ***************************************************
**
** call back func
**
** **************************************************/

static Eina_Bool __setting_font_font_size_click_softkey_back_cb(void *data, Elm_Object_Item *it)
{
	/*error check */
	retv_if(data == NULL, EINA_FALSE);
	SettingFontUG *ad = (SettingFontUG *) data;

	if (ad->view_to_load == &setting_view_font_font_size) {

		int value = 0;
		int ret = system_settings_get_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE, &value);
		setting_retvm_if(ret != 0, EINA_FALSE, "fail to get vconf");

		app_control_h svc;
		if (app_control_create(&svc)) {
			return EINA_FALSE;
		}

		app_control_add_extra_data(svc, "category", "FontSize");
		app_control_add_extra_data(svc, "FontSize", result_str_arr[value]);

		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "category", "FontSize");
		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "FontSize", result_str_arr[value]);

		ug_send_result(ad->ug, svc);
		app_control_destroy(svc);

		ug_destroy_me(ad->ug);
	} else {
		setting_view_change(&setting_view_font_font_size, &setting_view_font_main, ad);
	}
	return EINA_TRUE;
}
static void __setting_font_size_progress_popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *ad = (SettingFontUG *) data;
	if (ad->size_popup) {
		evas_object_del(ad->size_popup);
		ad->size_popup = NULL;
	}
}

static Eina_Bool __font_change_call(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *ad = (SettingFontUG *) data;

	/* logic */
	int ret;
	ret = system_settings_set_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE, ad->ret_font_size);
	if (ret != 0) {
		SETTING_TRACE_ERROR("system call failed with error code %d", ret);
	}

	/* finalize */
	ad->font_size_idler = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void setting_font_font_size_list_mouse_up_cb(void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingFontUG *ad = list_item->userdata;
	ad->cur_focus_item = item;
	Evas_Object *radio = data;
	elm_radio_value_set(radio, list_item->chk_status);

	if (ad->main_popup) {
		evas_object_del(ad->main_popup);
		ad->main_popup = NULL;
	}

	int ret = 0;
	int old_value = -1;
	ret = system_settings_get_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE, &old_value);
	if (ret != 0) {
		SETTING_TRACE_ERROR("system call failed with error code %d", ret);
	}
	if (old_value == list_item->chk_status) {
		return;
	}

	ad->ret_font_size = list_item->chk_status;

	/* original popup */
	ad->size_rdg = NULL;
	ad->size_popup = setting_create_popup_with_progressbar(ad, ad->win_get,
														   PROGRESSBAR_STYLE,
														   NULL, KeyStr_Loading, __setting_font_size_progress_popup_cb, 3/*0*/, TRUE, TRUE, 0);	/* 3 seconds to wait in maximum */
	ad->font_size_idler = ecore_timer_add(1, (Ecore_Task_Cb)__font_change_call, ad);
}

