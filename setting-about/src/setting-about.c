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
#include <setting-about-main.h>
#include <setting-cfg.h>

#define SETTING_ABOUT_PACKAGE_NAME "org.tizen.setting-about"

void setting_about_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
								void *priv)
{
	/*SettingAboutUG *ad = (SettingAboutUG *) priv; */
	Evas_Object *base;

	if (!priv) {
		return;
	}
	SETTING_TRACE_BEGIN;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base) {
		return;
	}

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
										 EVAS_HINT_EXPAND);
		/* elm_win_resize_object_add(ad->win_get, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}

	SETTING_TRACE_END;
}

/**
* @brief Event process when the sizeof UG view changes
*
* @param data
* @param e
* @param obj
* @param event_info
*/
static void setting_about_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
									   void *event_info)
{
	SettingAboutUG *ad = (SettingAboutUG *) data;
	setting_view_update(&setting_view_about_main, ad);
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
static void setting_about_ug_on_event(ui_gadget_h ug, enum ug_event event,
									  app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)priv;
	setting_retm_if(NULL == ad, "ad is NULL");
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		setting_navi_items_update(ad->md.navibar_main);
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
	/*break; */
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
	/*break; */
	case UG_EVENT_ROTATE_LANDSCAPE:
	/*break; */
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN: {
			/*restore the focus status */
			SettingAboutUG *ad = (SettingAboutUG *) priv;
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
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
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
			if (elm_naviframe_top_item_get(ad->md.navibar_main) ==
				elm_naviframe_bottom_item_get(ad->md.navibar_main)) {
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
	{"IDS_ST_BODY_IMEI", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_IMEI", 0, 0, 0, Cfg_Item_View_Node, NULL,	NULL, NULL, NULL},
	{"IDS_ST_BODY_MY_NUMBER", NULL, "viewtype:frontpage;tab:first;keyword:IDS_ST_BODY_MY_NUMBER", 0, 0, 0, Cfg_Item_View_Node, NULL,  NULL, NULL, NULL},
	{"IDS_CAM_POP_MODEL", NULL, "viewtype:frontpage;tab:first;keyword:IDS_CAM_POP_MODEL", 0, 0, 0, Cfg_Item_View_Node,	NULL, NULL, NULL, NULL},
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

/**
 * @brief init search db for aboutUG searchable nodes
 *
 * @param service struct Setting_Cfg_Node_T
 * @param priv struct Setting_Cfg_Node_T
 * @param applocale struct Setting_Cfg_Node_T
 *
 * @return 0 for success
 */
int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-about-efl DB search code");

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, IMG_AboutDevice, s_cfg_node_array[i].item_type,  s_cfg_node_array[i].data, "About");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}
///////////////////////////////////////
static bool app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *) data;

	setting_retvm_if((!ad), NULL, "!priv");

	setting_set_i18n(SETTING_PACKAGE, ABOUT_LOCALEDIR);

	if(app_init(&ad->md, SETTING_ABOUT_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	setting_view_create(&setting_view_about_main, ad);

	evas_object_event_callback_add(ad->md.win_main,
					EVAS_CALLBACK_RESIZE,
					setting_about_ug_cb_resize, ad);

	SETTING_TRACE_END;
	return true;
}

static void app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!data), "!priv");
	SettingAboutUG *aboutUG = data;
	aboutUG->pause_flag = FALSE;

	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };

	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_wifi) {
		setting_about_main_get_wifi_mac_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		aboutUG->item_data_wifi->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(aboutUG->item_data_wifi->item, aboutUG->item_data_wifi);
		elm_genlist_item_update(aboutUG->item_data_wifi->item);
	}
	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_bt) {
		setting_about_main_get_bluetooth_address_string(str, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
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

static void app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *SettingAboutUG = data;
	SettingAboutUG->pause_flag = TRUE;
}

static void app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!data), "!data");
	SettingAboutUG *aboutUG = data;

	evas_object_event_callback_del(aboutUG->md.ly_main, EVAS_CALLBACK_RESIZE, setting_about_ug_cb_resize);	/* fix flash issue for gallery */

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_about_main, aboutUG);
	if (NULL != ug_get_layout(aboutUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(aboutUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(aboutUG->ug));
	}

	if (aboutUG->md.handle
		&& tel_deinit(aboutUG->md.handle) == TAPI_API_SUCCESS) {
		SETTING_TRACE("tel_deinit sucessed");
	}
	FREE(aboutUG);

	SETTING_TRACE_END;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = app_create,
		.pause = app_pause,
		.resume = app_resume,
		.terminate = app_terminate,
		.app_control = NULL
	};
	SettingAboutUG app_data;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}

