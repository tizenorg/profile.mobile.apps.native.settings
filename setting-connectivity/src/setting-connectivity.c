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

#include <setting-connectivity.h>
#include <setting-cfg.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#define USB_BTN_NO	0

/**
  * This function checks the USB connection status
  */
int check_usb_jack_status()
{
	SETTING_TRACE_BEGIN;
	bool usb_connection = false;
	int ret = runtime_info_get_value_bool(RUNTIME_INFO_KEY_USB_CONNECTED, &usb_connection);
	if (ret == RUNTIME_INFO_ERROR_NONE && usb_connection == true) {
		return USB_CONNECTED;   /*  USB is connected */
	} else {
		return USB_DISCONNECTED;       /*  USB is disconnected */
	}
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
		return NULL;	/* &setting_view_connective_main; */
	}

}

Evas_Object *__get_connective_layout_to_return(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *connectiveUG = priv;
	char *viewtype = NULL;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "usb")) {
		FREE(viewtype);
		return connectiveUG->ly_usb;
	} else {
		FREE(viewtype);
		return NULL;	/* &setting_view_connective_main; */
	}

}

static void setting_connective_ug_cb_resize(void *data, Evas *e,
											Evas_Object *obj, void *event_info)
{
	SettingConnectivityUG *ad = (SettingConnectivityUG *) data;
	/* setting_view_update(&setting_view_about_main, ad); */
	setting_view_update(ad->view_to_load, ad);
}

static void *setting_connective_ug_on_create(ui_gadget_h ug,
											 enum ug_mode mode, app_control_h service,
											 void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingConnectivityUG *connectiveUG = priv;
	connectiveUG->ug = ug;

	connectiveUG->win_main_layout =
		(Evas_Object *) ug_get_parent_layout(ug);
	connectiveUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(connectiveUG->win_main_layout);
	connectiveUG->evas =
		evas_object_evas_get(connectiveUG->win_main_layout);

	setting_retvm_if(connectiveUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_connectivity_usb, NULL);
	setting_view_node_table_register(&setting_view_connectivity_usb_help,
									 &setting_view_connectivity_usb);

	/*  creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(connectiveUG->itc_title));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(connectiveUG->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(connectiveUG->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(connectiveUG->itc_1text_1icon_gen));	/* toggle */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(connectiveUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc("dialogue/2text.3", &(connectiveUG->itc_2text_3));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(connectiveUG->itc_2text_3_parent));
	setting_create_Gendial_itc("multiline_sub", &(connectiveUG->itc_help_style));

	connectiveUG->view_to_load = __get_connective_view_to_load(service);
	setting_retvm_if(NULL == connectiveUG->view_to_load, NULL,
					 "NULL == connectiveUG->view_to_load");
	setting_view_node_set_cur_view(connectiveUG->view_to_load);
	setting_view_create(connectiveUG->view_to_load, (void *)connectiveUG);
	evas_object_event_callback_add(connectiveUG->win_main_layout,
								   EVAS_CALLBACK_RESIZE,
								   setting_connective_ug_cb_resize,
								   connectiveUG);
	return __get_connective_layout_to_return(service, connectiveUG);
}

static void setting_connective_ug_on_start(ui_gadget_h ug, app_control_h service,
										   void *priv)
{
}

static void setting_connective_ug_on_pause(ui_gadget_h ug, app_control_h service,
										   void *priv)
{
}

static void setting_connective_ug_on_resume(ui_gadget_h ug, app_control_h service,
											void *priv)
{
}

static void setting_connective_ug_on_destroy(ui_gadget_h ug,
											 app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingConnectivityUG *connectiveUG = priv;

	evas_object_event_callback_del(connectiveUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_connective_ug_cb_resize);	/* fix flash issue for gallery */
	connectiveUG->ug = ug;

	/*  called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_connectivity_usb == connectiveUG->view_to_load) {
		setting_view_destroy(&setting_view_connectivity_usb_help,
							 connectiveUG);
		setting_view_destroy(&setting_view_connectivity_usb,
							 connectiveUG);
	}

	if (NULL != ug_get_layout(connectiveUG->ug)) {
		evas_object_hide((Evas_Object *)
						 ug_get_layout(connectiveUG->ug));
		evas_object_del((Evas_Object *)
						ug_get_layout(connectiveUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_connective_ug_on_message(ui_gadget_h ug, app_control_h msg,
											 app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_connective_ug_on_event(ui_gadget_h ug,
										   enum ug_event event, app_control_h service,
										   void *priv)
{
	SETTING_TRACE_BEGIN;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_connective_ug_on_key_event(ui_gadget_h ug,
											   enum ug_key_event event,
											   app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *ad = (SettingConnectivityUG *) priv;

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

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *connectiveUG =
		calloc(1, sizeof(SettingConnectivityUG));
	setting_retvm_if(!connectiveUG, -1,
					 "Create SettingConnectivityUG obj failed");

	ops->create = setting_connective_ug_on_create;
	ops->start = setting_connective_ug_on_start;
	ops->pause = setting_connective_ug_on_pause;
	ops->resume = setting_connective_ug_on_resume;
	ops->destroy = setting_connective_ug_on_destroy;
	ops->message = setting_connective_ug_on_message;
	ops->event = setting_connective_ug_on_event;
	ops->key_event = setting_connective_ug_on_key_event;
	ops->priv = connectiveUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingConnectivityUG *connectiveUG;
	setting_retm_if(!ops, "ops == NULL");

	connectiveUG = ops->priv;
	if (connectiveUG) {
		FREE(connectiveUG);
	}
}

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv,
											 char **applocale)
{
	int i, size;
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = priv;
	const Setting_Cfg_Node_T search_configs[] = {
		{DEVOPTION_STR_USB_DEBUGGING, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
		{DEVOPTION_STR_SHOW_CPU_USAGE, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
		{DEVOPTION_STR_RENDERTING_ENGINE, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
		{DEVOPTION_STR_LIMIT_BACKGROUND_PRECESS, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
		{DEVOPTION_STR_CRASH_VIEWER, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL}
	};

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

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

