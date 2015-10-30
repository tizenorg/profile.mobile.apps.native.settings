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
#include <setting-applications-main.h>
#include <dd-display.h>

#include <setting-common-draw-widget.h>

static int setting_applications_main_create(void *cb);
static int setting_applications_main_destroy(void *cb);
static int setting_applications_main_update(void *cb);
static int setting_applications_main_cleanup(void *cb);

setting_view setting_view_applications_main = {
	.create = setting_applications_main_create,
	.destroy = setting_applications_main_destroy,
	.update = setting_applications_main_update,
	.cleanup = setting_applications_main_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static void setting_applications_main_vconf_change_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);

	SettingApplicationsUG *ad = data;
	int status = 0;

	status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("status:%d", status);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL)) {
		if (ad->data_back) {
			G_FREE(ad->data_back->sub_desc);
			ad->data_back->sub_desc = get_pa_backlight_time_value_str();
			elm_object_item_data_set(ad->data_back->item, ad->data_back);
			elm_genlist_item_update(ad->data_back->item);
		}
	}
}


static int setting_applications_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	Evas_Object *scroller = NULL;

	int value = 0;
	int ret = 0;


	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           KeyStr_Applications, NULL,/* ARROW */
	                                           NULL,
	                                           (setting_call_back_func)setting_applications_main_click_softkey_back_cb,
	                                           NULL, ad, &scroller,
	                                           &(ad->navi_bar));

	ad->genlist = scroller;
	elm_genlist_mode_set(ad->genlist, ELM_LIST_COMPRESS);
	/*register vconf key */

	evas_object_smart_callback_add(ad->genlist, "realized", __gl_realized_cb, NULL);

	Elm_Object_Item *item = NULL;

	item =
	    elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	                            ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	setting_create_Gendial_field_def(ad->genlist, &itc_1text,
	                                 setting_applications_main_mouse_up_Gendial_list_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL,
	                                 NULL, 0,
	                                 KeyStr_ApplicationManager, NULL, NULL);

	setting_create_Gendial_field_def(ad->genlist, &itc_1text,
	                                 setting_applications_main_mouse_up_Gendial_list_cb,
	                                 ad, SWALLOW_Type_INVALID, NULL,
	                                 NULL, 0,
	                                 KeyStr_DefaultApplications, NULL, NULL);


	setting_view_applications_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_applications_main.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	if (ad->nf_it) {
		ad->nf_it = NULL;
	}
	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		/* if(ad->back_dialData) FREE(ad->back_dialData); */
	}
	setting_view_applications_main.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
		if (ad->data_br) {
			ad->data_br->sub_desc =
			    (char *)g_strdup(get_brightness_mode_str());
			elm_object_item_data_set(ad->data_br->item, ad->data_br);
			elm_genlist_item_update(ad->data_br->item);

		}
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

static Eina_Bool setting_applications_manage_apps_freeze_event_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	evas_object_freeze_events_set(ad->navi_bar, EINA_FALSE);

	ad->event_freeze_timer = NULL;
	SETTING_TRACE_END;
	return EINA_FALSE;
}



static void setting_applications_manage_apps_ug(SettingApplicationsUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (app_launcher("setting-manage-applications-efl|viewtype:manage-applications") == 0) {
		ad->event_freeze_timer = ecore_timer_add(1, setting_applications_manage_apps_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
}

static void
setting_applications_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
                                                   void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KeyStr_ApplicationManager, list_item->keyStr)) {
		setting_applications_manage_apps_ug(ad);
	} else if (!safeStrCmp(KeyStr_DefaultApplications, list_item->keyStr)) {
		setting_view_change(&setting_view_applications_main,
		                    &setting_view_applications_defaultapp, ad);
	}
}

static Eina_Bool setting_applications_main_click_softkey_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > Applications] Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;

}

