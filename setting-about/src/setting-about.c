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
* @brief on_create function of the UG
*
* @param ug
* @param mode
* @param data
* @param priv
*
* @return
*/

bool setting_about_ug_on_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingAboutUG *aboutUG = priv;

	aboutUG->win_get = elm_win_util_standard_add("Setting about", "Setting about");
	aboutUG->win_main_layout = elm_layout_add(aboutUG->win_get);
	elm_win_resize_object_add(aboutUG->win_get, aboutUG->win_main_layout);

	evas_object_show(aboutUG->win_get);
	evas_object_show(aboutUG->win_main_layout);
	aboutUG->evas = evas_object_evas_get(aboutUG->win_main_layout);

	setting_retvm_if(aboutUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_about_main, NULL);

	/*  creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(aboutUG->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(aboutUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(aboutUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(aboutUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(aboutUG->itc_group_item));
	setting_create_Gendial_itc("multiline/1text", &(aboutUG->itc_help_style));

	setting_view_node_set_cur_view(&setting_view_about_main);
	setting_view_create(&setting_view_about_main, (void *)aboutUG);

	evas_object_event_callback_add(aboutUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_about_ug_cb_resize, aboutUG);

	aboutUG->popup_showed_flag = FALSE;

	return true;
}

void setting_about_ug_on_pause(void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *SettingAboutUG = priv;
	SettingAboutUG->pause_flag = TRUE;
}

void setting_about_ug_on_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingAboutUG *aboutUG = priv;
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

/**
* on_destroy function of the UG
*
* @param ug
* @param data
* @param priv
*/

void setting_about_ug_on_destroy(void *priv)
{
	SettingAboutUG *aboutUG = priv;

	evas_object_event_callback_del(aboutUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_about_ug_cb_resize);	/* fix flash issue for gallery */

	/*  delete the allocated objects. */
	setting_view_destroy(&setting_view_about_main, aboutUG);
	if (NULL != ug_get_layout(aboutUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(aboutUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(aboutUG->ug));
	}

	SETTING_TRACE_END;
}
/*
static void setting_about_ug_on_message(ui_gadget_h ug, app_control_h msg,
										app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	if (aboutUG) {
		if (aboutUG->handle
		    && tel_deinit(aboutUG->handle) == TAPI_API_SUCCESS) {
			SETTING_TRACE("tel_deinit sucessed");
		}
		FREE(aboutUG);
	}
}
*/
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
* @brief aboutUG init
*
* @param ops
*/
int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	//ops->create = setting_about_ug_on_create;

	//ops->pause = setting_about_ug_on_pause;
	//ops->resume = setting_about_ug_on_resume;
	//ops->destroy = setting_about_ug_on_destroy;

	//ops->event = setting_about_ug_on_event;
	ops->key_event = setting_about_ug_on_key_event;

	return 0;
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


