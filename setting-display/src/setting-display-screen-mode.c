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
#include <setting-display-screen-mode.h>

static int setting_display_screen_mode_create(void *cb);
static int setting_display_screen_mode_destroy(void *cb);
static int setting_display_screen_mode_update(void *cb);
static int setting_display_screen_mode_cleanup(void *cb);

/* Don't support Natural mode in redwood. */
const char *str_in_arr[4] = {"Dynamic", "Standard", "Natural", "Movie"};
const char *str_out_arr[4] = {Keystr_Dynamic, Keystr_Standard, Keystr_Natural, Keystr_Movie};

static void  setting_display_screen_mode_mouse_up_Gendial_list_radio_cb(void *data, Evas_Object *obj, void *event_info);

setting_view setting_view_display_screen_mode = {
	.create = setting_display_screen_mode_create,
	.destroy = setting_display_screen_mode_destroy,
	.update = setting_display_screen_mode_update,
	.cleanup = setting_display_screen_mode_cleanup,
};


static Evas_Object *__gl_content_get(void *data, Evas_Object *obj, const char *part)
{
	if (!strcmp(part, "elm.icon")) {
		Evas_Object *image;
		image = elm_image_add(obj);
		elm_image_file_set(image, IMG_SCREEN_MODE, NULL);

		/*int w = 668; /* left padding and right padding is 26, so the width = 720 - 26 -26 * / */
		int w = 720 - 36 * 2; /*enlarge the padding to 36 to fix issue P130319-9042 */
		int h = 0;
		elm_image_object_size_get(image, NULL, &h);
		evas_object_size_hint_min_set(image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
		evas_object_size_hint_max_set(image, w * WIDGET_SCALE_FACTOR, h * WIDGET_SCALE_FACTOR);
		evas_object_resize(image, w * WIDGET_SCALE_FACTOR, h * WIDGET_SCALE_FACTOR);

		return image;
	}
	return NULL;
}

static void __gl_item_del(void *data, Evas_Object *obj)
{
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	if (item_data) {
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		FREE(item_data);
	}
}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_display_screen_mode_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	setting_call_back_func gl_sel_cb = NULL;
	Evas_Object *genlist = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;


	if (&setting_view_display_screen_mode == ad->view_to_load) {
		ad->ly_main =
		    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
		                                           ad->win_get,
		                                           "IDS_ST_BODY_SCREEN_MODE",
		                                           _("IDS_ST_BUTTON_BACK"),
		                                           NULL,
		                                           (setting_call_back_func)setting_display_screen_mode_click_softkey_back_cb,
		                                           NULL, ad,
		                                           &genlist,
		                                           &ad->navi_bar);
	} else {
		genlist = elm_genlist_add(ad->navi_bar);
		retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "scroller == NULL");
		elm_object_style_set(genlist, "dialogue");
		elm_genlist_clear(genlist);	/* first to clear list */
		setting_push_layout_navi_bar("IDS_ST_BODY_SCREEN_MODE", _("IDS_ST_BUTTON_BACK"),
		                             NULL, NULL,
		                             (setting_call_back_func)setting_display_screen_mode_click_softkey_back_cb,
		                             NULL, NULL, ad, genlist, ad->navi_bar, NULL);
	}
	ad->genlist = genlist;

	evas_object_smart_callback_add(ad->genlist, "realized", __gl_realized_cb, ad);

	Elm_Object_Item *item = elm_genlist_item_append(genlist,
	                                                &(itc_seperator),
	                                                NULL, NULL,
	                                                ELM_GENLIST_ITEM_NONE,
	                                                NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->itc_screen_mode.item_style = "1icon/with_no_line";
	ad->itc_screen_mode.func.text_get = NULL;
	ad->itc_screen_mode.func.content_get = __gl_content_get;
	ad->itc_screen_mode.func.state_get = NULL;
	ad->itc_screen_mode.func.del = __gl_item_del;

	Setting_GenGroupItem_Data *screen_mode = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retvm_if(!screen_mode, SETTING_RETURN_FAIL, "calloc failed");

	if (screen_mode) {
		screen_mode->userdata = ad;
		screen_mode->item = elm_genlist_item_append(genlist, &(ad->itc_screen_mode), screen_mode, NULL,
		                                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(screen_mode->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("screen_mode is NULL");
		return SETTING_RETURN_FAIL;
	}

	item = elm_genlist_item_append(genlist,
	                               &(itc_sep_line),
	                               NULL, NULL,
	                               ELM_GENLIST_ITEM_NONE,
	                               NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	gl_sel_cb = setting_display_screen_mode_mouse_up_Gendial_list_radio_cb;

	Evas_Object *radio = elm_radio_add(genlist);
	elm_radio_state_value_set(radio, -1);

	int i = 0;

	for (i = 0 ; i < SCREENMODE_MAX ; i++) {
#if !SUPPORT_AMOLED		/* Natural mode can be supported on AMOLED only. So, skip this menu in other cases */
		if (safeStrCmp(str_in_arr[i], "Natural") == 0) {
			continue;
		}
#endif
		item_data = setting_create_Gendial_field_1radio(genlist,
		                                                &(itc_1text_1icon_2),
		                                                gl_sel_cb,
		                                                radio,	/* sel data */
		                                                SWALLOW_Type_1RADIO,
		                                                radio, i,
		                                                str_out_arr[i], NULL);
		if (item_data) {
			if (i == 0) {
				item_data->group_style = SETTING_GROUP_STYLE_TOP;
			} else if (i == (sizeof(str_in_arr) / sizeof(str_in_arr[0]) - 1)) {
				item_data->group_style = SETTING_GROUP_STYLE_BOTTOM;
			} else {
				item_data->group_style = SETTING_GROUP_STYLE_CENTER;
			}
			item_data->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}

	/* set toggle */
	char *curmode = vconf_get_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME);
	SETTING_TRACE(">>> CUR SCREEN MODE : %s ", curmode);
	int j;
	if (!curmode) {
		/*set defaule screen mode if get the cur mode failed from VCONF*/
		elm_radio_value_set(radio, 0);
	} else {
		for (j = 0; j < sizeof(str_in_arr) / sizeof(str_in_arr[0]) ; j++) {
			if (0 == strcmp(str_in_arr[j], curmode)) {
				elm_radio_value_set(radio, j);
				break;
			}
		}
		FREE(curmode);
	}

	item = elm_genlist_item_append(genlist, &itc_bottom_seperator, NULL, NULL,
	                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	setting_view_display_screen_mode.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_screen_mode_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_display_screen_mode.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	evas_object_smart_callback_del(ad->genlist, "realized", __gl_realized_cb);

	if (&setting_view_display_screen_mode == ad->view_to_load) {
		if (ad->ly_main != NULL) {
			evas_object_del(ad->ly_main);
			ad->ly_main = NULL;
		}
		setting_view_display_screen_mode.is_create = 0;
	} else {
		setting_view_display_screen_mode.is_create = 0;
		elm_naviframe_item_pop(ad->navi_bar);
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_screen_mode_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_screen_mode_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;

	setting_display_screen_mode_destroy(ad);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

static Eina_Bool
setting_display_screen_mode_click_softkey_back_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > Display] Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	if (&setting_view_display_screen_mode == ad->view_to_load) {
		/* Send destroy request */
		ug_destroy_me(ad->ug);
		return EINA_FALSE;
	}

	setting_view_change(&setting_view_display_screen_mode, &setting_view_display_main, ad);
	SETTING_TRACE_END;
	return EINA_TRUE;
}


static void
setting_display_screen_mode_mouse_up_Gendial_list_radio_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Evas_Object *radio = (Evas_Object *) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	/*SettingDisplayUG *ad = (SettingDisplayUG *) list_item->userdata; */

	elm_radio_value_set(radio, list_item->chk_status);
	vconf_set_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME, str_in_arr[list_item->chk_status]);
	/* Set status to device manager */
	display_set_image_enhance(ENHANCE_MODE, list_item->chk_status);
	/*SETTING_TRACE(">>> AFTER VCONF SET : %s ", str_in_arr[list_item->chk_status]); */
}
