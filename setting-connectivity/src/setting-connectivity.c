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

/**
 *@defgroup setting-connectivity
 *UG creation code for setting-connectivity
 */

#include <setting-cfg.h>

#include "setting-connectivity.h"

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#define SETTING_CONNECTIVITY_PACKAGE_NAME "org.tizen.setting-connectivity"
#define CONNECTIVITY_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-connectivity/res/locale"
#define USB_BTN_NO 0

/**
 * This function checks the USB connection status
 */
int check_usb_jack_status()
{
	SETTING_TRACE_BEGIN;
	bool usb_connection = false;
	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED,
			&usb_connection);
	if (ret == RUNTIME_INFO_ERROR_NONE && usb_connection == true)
		return USB_CONNECTED; /*	USB is connected */
	else
		return USB_DISCONNECTED; /*  USB is disconnected */
}

setting_view *__get_connective_view_to_load(app_control_h service)
{
	SETTING_TRACE_BEGIN;
	char *viewtype = NULL;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "usb")) {
		FREE(viewtype);
		return &setting_view_connectivity_usb;
	} else {
		FREE(viewtype);
		return NULL; /* &setting_view_connective_main; */
	}

}

bool __get_connective_layout(SettingConnectivityUG *connectiveUG,
		app_control_h service)
{
	SETTING_TRACE_BEGIN;
	char *viewtype = NULL;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	retv_if(!viewtype, false);

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "usb")) {
		FREE(viewtype);
		return connectiveUG->ly_usb;
	} else {
		FREE(viewtype);
		return false; /* &setting_view_connective_main; */
	}
}

static void setting_connective_ug_cb_resize(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;
	/* setting_view_update(&setting_view_about_main, ad); */
	setting_view_update(ad->view_to_load, ad);
}

bool on_app_create(void *priv)
{
	SettingConnectivityUG *ad = priv;
	retv_if((!ad), false);

	if (app_init(&ad->md, SETTING_CONNECTIVITY_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	setting_set_i18n(SETTING_PACKAGE, CONNECTIVITY_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_connectivity_usb, NULL);
	setting_view_node_table_register(&setting_view_connectivity_usb_help,
			&setting_view_connectivity_usb);

	/*	creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_title));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon_gen)); /* toggle */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1icon_1text_sub));
	setting_create_Gendial_itc("dialogue/2text.3",
			&(ad->itc_2text_3));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_3_parent));
	setting_create_Gendial_itc("multiline_sub",
			&(ad->itc_help_style));

	return true;
}

static void on_app_pause(void *priv)
{
}

static void on_app_resume(void *priv)
{
}

static void on_app_control(app_control_h service, void *priv)
{
	SettingConnectivityUG *ad = priv;
	ret_if(!ad);

	ad->view_to_load = __get_connective_view_to_load(service);
	ret_if(!ad->view_to_load);

	setting_view_node_set_cur_view(ad->view_to_load);
	setting_view_create(ad->view_to_load, (void *)ad);
	evas_object_event_callback_add(ad->md.ly_main,
			EVAS_CALLBACK_RESIZE, setting_connective_ug_cb_resize,
			ad);
	ret_if(!__get_connective_layout(ad, service));
}

static void on_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingConnectivityUG *ad = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.ly_main,
			EVAS_CALLBACK_RESIZE, setting_connective_ug_cb_resize);

	/* called when this shared gadget is terminated. similar with
	 * app_exit */
	if (&setting_view_connectivity_usb == ad->view_to_load) {
		setting_view_destroy(&setting_view_connectivity_usb_help, ad);
		setting_view_destroy(&setting_view_connectivity_usb, ad);
	}

	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}

	SETTING_TRACE_END;
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

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = on_app_create,
		.pause = on_app_pause,
		.resume = on_app_resume,
		.terminate = on_app_terminate,
		.app_control = on_app_control,
	};
	SettingConnectivityUG app_data;
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	memset(&app_data, 0x0, sizeof(app_data));
	return ui_app_main(argc, argv, &ops, &app_data);
}

/*
UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv,
		char **applocale)
{
	int i, size;
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = priv;
	const Setting_Cfg_Node_T search_configs[] = { {
		DEVOPTION_STR_USB_DEBUGGING,
		NULL,
		NULL,
		0,
		Cfg_Item_unResetable,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL }, {
		DEVOPTION_STR_SHOW_CPU_USAGE,
		NULL,
		NULL,
		0,
		Cfg_Item_unResetable,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL }, {
		DEVOPTION_STR_RENDERTING_ENGINE,
		NULL,
		NULL,
		0,
		Cfg_Item_unResetable,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL }, {
		DEVOPTION_STR_LIMIT_BACKGROUND_PRECESS,
		NULL,
		NULL,
		0,
		Cfg_Item_unResetable,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL }, {
		DEVOPTION_STR_CRASH_VIEWER,
		NULL,
		NULL,
		0,
		Cfg_Item_unResetable,
		0,
		Cfg_Item_View_Node,
		NULL,
		NULL,
		NULL,
		NULL } };

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:"_TZ_SYS_RO_APP\
			"/org.tizen.setting/res/locale");

	size = sizeof(search_configs) / sizeof(Setting_Cfg_Node_T);

	for (i = 0; i < size; i++) {
		node = setting_plugin_search_item_subindex_add(
				search_configs[i].key_name,
				"viewtype:usb",
				IMG_USBconnection,
				search_configs[i].item_type,
				search_configs[i].data,
				"Developer options");

		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}
*/

