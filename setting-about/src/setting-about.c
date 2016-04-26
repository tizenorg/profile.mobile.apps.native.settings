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

#include <setting-about.h>
#include <setting-cfg.h>


void setting_about_ug_on_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingAboutUG *aboutUG = priv;
	aboutUG->pause_flag = FALSE;

	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };

	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_wifi) {
		//setting_about_main_get_wifi_mac_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		aboutUG->item_data_wifi->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(aboutUG->item_data_wifi->item, aboutUG->item_data_wifi);
		elm_genlist_item_update(aboutUG->item_data_wifi->item);
	}
	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_bt) {
		//setting_about_main_get_bluetooth_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		aboutUG->item_data_bt->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(aboutUG->item_data_bt->item, aboutUG->item_data_bt);
		elm_genlist_item_update(aboutUG->item_data_bt->item);
	}

	if (aboutUG->item_dev_name) {
		if (elm_object_focus_get(aboutUG->item_dev_name->eo_check)) {
			elm_object_focus_set(aboutUG->item_dev_name->eo_check, EINA_TRUE);
			Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(aboutUG->item_dev_name->eo_check);
			if (imf_context) {
				ecore_imf_context_input_panel_show(imf_context);
			}
		}
	}
}

/**
* @brief ug_event processing function
*
* @param ug
* @param event
* @param service
* @param priv
*
* @return
*/

void setting_about_ug_on_event(app_event_info_h event_info, void *priv)
{
	int ret;
	app_device_orientation_e ori;
	SettingAboutUG *ad = (SettingAboutUG *)priv;
	setting_retm_if(NULL == ad, "ad is NULL");

	ret = app_event_get_device_orientation(event_info, &ori);
	if (APP_ERROR_NONE != ret) {
		SETTING_TRACE("setting_about_ug_on_event %s", get_error_message(ret));
		return;
	}
	if (APP_DEVICE_ORIENTATION_180 == ori) {
			/*restore the focus status */
			if (!ad || !ad->is_dev_name_focus || !ad->item_dev_name) {
				return;
			}
			SETTING_TRACE("ad->is_dev_name_focus:%d", ad->is_dev_name_focus);

			Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(ad->item_dev_name->eo_check);
			if (imf_context) {
				/*ecore_imf_context_input_panel_hide(imf_context); */
				ecore_imf_context_input_panel_show(imf_context);
			}
			elm_object_focus_set(ad->item_dev_name->eo_check, ad->is_dev_name_focus);
			elm_entry_cursor_pos_set(ad->item_dev_name->eo_check, ad->cursor_pos);
			ad->is_dev_name_focus = FALSE;
		}
}

/**
* @brief ug_key_event processing function
*
* @param ug
* @param event
* @param service
* @param priv
*
* @return
*/
static void setting_about_ug_on_key_event(ui_gadget_h ug,
										  enum ug_key_event event,
										  app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *) priv;
	if (!ug) {
		return;
	}

	switch (event) {
	case UG_KEY_EVENT_END: {
			if (elm_naviframe_top_item_get(ad->navi_bar) ==
				elm_naviframe_bottom_item_get(ad->navi_bar)) {
				ug_destroy_me(ug);
			} else {
				/* elm_naviframe_item_pop(ad->navi_bar); */
				setting_view_cb_at_endKey(ad);
			}
		}
		break;
	default:
		break;
	}
}


/**
 * @brief aboutUG searchable nodes array
 *
 * @see the struct Setting_Cfg_Node_T
 */
static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_ST_BODY_IMEI", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_IMEI", 0, 0, 0, Cfg_Item_View_Node, NULL,  NULL, NULL, NULL},
	{"IDS_ST_BODY_MY_NUMBER", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_MY_NUMBER", 0, 0, 0, Cfg_Item_View_Node, NULL,  NULL, NULL, NULL},
	{"IDS_CAM_POP_MODEL", NULL, "viewtype:frontpage;tab:first;keyword:IDS_CAM_POP_MODEL", 0, 0, 0, Cfg_Item_View_Node,  NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_VERSION", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_VERSION", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_BLUETOOTH", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_BLUETOOTH", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_HEADER_WI_FI", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_HEADER_WI_FI", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_SERIAL_NUMBER", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_SERIAL_NUMBER", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_BATTERY", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_BATTERY", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_CPU_USAGE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_CPU_USAGE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_MBODY_DEVICE_STATUS", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_MBODY_DEVICE_STATUS", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_LICENCE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_LICENCE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_MBODY_SOFTWARE_UPDATE", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_MBODY_SOFTWARE_UPDATE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_HEADER_MANAGE_CERTIFICATES_ABB", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_ADVANCEDOPTIONS_CERTIFICATES", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
};


