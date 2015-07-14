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

#include "setting-moreconnections-main.h"

static int setting_moreconnections_main_create(void *cb);
static int setting_moreconnections_main_destroy(void *cb);
static int setting_moreconnections_main_update(void *cb);
static int setting_moreconnections_main_cleanup(void *cb);

setting_view setting_view_moreconnections_main = {
	.create = setting_moreconnections_main_create,
	.destroy = setting_moreconnections_main_destroy,
	.update = setting_moreconnections_main_update,
	.cleanup = setting_moreconnections_main_cleanup,
};

/************************************************
 * @brief Do process when clicking '<-' button
 *
 * @param data application context
 * @param obj evas object
 * @param event_info event type
 ************************************************/
static Eina_Bool
setting_moreconnections_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                                   void *event_info)
{
	setting_retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingMoreConnectionsUG *ad = (SettingMoreConnectionsUG *) data;
	if (ad->empty_flag) {
		return EINA_FALSE;
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;
}

static void setting_moreconnections_main_mouse_up_Gendial_list_cb(void *data,
                                                                  Evas_Object *obj, void *event_info)
{
	setting_retm_if(NULL == data, "data is NULL");
	setting_retm_if(NULL == event_info, "event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingMoreConnectionsUG *ad = (SettingMoreConnectionsUG *)data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp(KeyStr_LocationService, list_item->keyStr)) {
		app_launcher("org.tizen.setting-location");
	}
}

char *setting_location_is_enable(void *data)
{
	int is_enable;

	if (0 != vconf_get_int(VCONFKEY_LOCATION_USE_MY_LOCATION, &is_enable))
		SETTING_TRACE_ERROR("Fail to get vconf");

	if (1 == is_enable)
		return KeyStr_On;
	else if (0 == is_enable)
		return KeyStr_Off;
	else
		setting_retvm_if(data == NULL, SETTING_VCONF_ERR_RETURN_STR_NULL, "vconf error");
}

/**************************************************
 * @brief create main view genlist items
 * @param data application data
 * @return FALSE for call it once and then destory the timer, TRUE for always call it when the timer is reached.
 **************************************************/
int setting_moreconnections_main_generate_genlist(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingMoreConnectionsUG *ad = (SettingMoreConnectionsUG *) data;

	Evas_Object *scroller = ad->genlist;
	Setting_GenGroupItem_Data *item_data = NULL;

	char *sub_desc = setting_location_is_enable(data);
	ad->location_service = setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2),
	                                                        setting_moreconnections_main_mouse_up_Gendial_list_cb,
	                                                        ad, SWALLOW_Type_INVALID,
	                                                        NULL, NULL, 0,
	                                                        KeyStr_LocationService, sub_desc, NULL);
	if (ad->location_service) {
		ad->location_service->sub_desc = (char *)strdup(sub_desc);
		ad->location_service->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->location_service is NULL");
	}

	/* VPN */
	ad->VPN = setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
	                                           setting_moreconnections_main_mouse_up_Gendial_list_cb,
	                                           ad, SWALLOW_Type_INVALID,
	                                           NULL, NULL, 0,
	                                           KeyStr_VPN, NULL, NULL);
	if (ad->VPN) {
		ad->VPN->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->VPN is NULL");
	}

	SETTING_TRACE_END;
}

/***************************************************
 *
 * basic func
 *
 ***************************************************/
static int setting_moreconnections_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingMoreConnectionsUG *ad = (SettingMoreConnectionsUG *) cb;
	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	setting_retvm_if(NULL == scroller, SETTING_DRAW_ERR_FAIL_SCROLLER,
	                 "Scroller is NULL");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller);

	ad->genlist = scroller;
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                           ad->win_get,
	                                           KeyStr_MoreConnectionSetting,
	                                           KeyStr_Back,
	                                           NULL,
	                                           (setting_call_back_func)setting_moreconnections_main_click_softkey_back_cb,
	                                           NULL, ad, &scroller,
	                                           &(ad->navi_bar));
	ad->genlist = scroller;

	setting_moreconnections_main_generate_genlist((void *)ad);
	setting_view_moreconnections_main.is_create = 1;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_moreconnections_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_moreconnections_main.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMoreConnectionsUG *ad = (SettingMoreConnectionsUG *) cb;
	setting_view_moreconnections_main.is_create = 0;

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_moreconnections_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

static int setting_moreconnections_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}
