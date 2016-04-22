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
#include <setting-font.h>
#include <system_settings.h>
#include <setting-cfg.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

setting_view *__default_view_state(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");
	SettingFontUG *fontUG = (SettingFontUG *)data;

	/* argument processing */
	char *category = NULL;
	app_control_get_extra_data(service, "category", &category);

	if (0 == safeStrCmp(category, "FontType")) {
		SETTING_TRACE("LAUNCHED BY APP-CONTROL MODE");
		fontUG->viewmode = FONT_SEL_VIEW_APPCONTROL;
		setting_view_node_table_register(&setting_view_font_font_size, &setting_view_font_main);
		setting_view_node_table_register(&setting_view_font_main, NULL);
		FREE(category);
		return &setting_view_font_main;
	} else if (0 == safeStrCmp(category, "FontSize")) {
		SETTING_TRACE("category = %s", category);
		fontUG->viewmode = FONT_SIZE_VIEW_APPCONTROL;
		setting_view_node_table_register(&setting_view_font_font_size, NULL);
		FREE(category);
		return &setting_view_font_font_size;
	} else {
		SETTING_TRACE("HERE ?? <<<<< ");
		char *viewtype = NULL;
		app_control_get_extra_data(service, "viewtype", &viewtype);
		if (!safeStrCmp(viewtype, "FontType_expand")) {
			fontUG->font_expand_state = FONT_EXPAND_STATE_FONT_TYPE;
		} else if (!safeStrCmp(viewtype, "FontSize_expand")) {
			fontUG->font_expand_state = FONT_EXPAND_STATE_FONT_SIZE;
		} else {
			fontUG->font_expand_state = FONT_EXPAND_STATE_NONE;
		}
		fontUG->viewmode = FONT_SIZE_AND_FONT_SEL_UG;	/* set default */
		setting_view_node_table_register(&setting_view_font_font_size, &setting_view_font_main);
		setting_view_node_table_register(&setting_view_font_main, NULL);
		FREE(category);
		FREE(viewtype);
		return &setting_view_font_main;
	}
}

/*------------------------------------------------------------- */
/* http://tizen.org/appcontrol/operation/configure/font/type */
/* http://tizen.org/appcontrol/operation/configure/font/size */
/*------------------------------------------------------------- */
setting_view *__app_control_get_operation_handler(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");
	setting_retvm_if((!service), NULL, "service is NULL");
	SettingFontUG *fontUG = (SettingFontUG *)data;
	char *output_url = NULL;

	int ret = app_control_get_operation(service, &output_url);

	if (ret == APP_CONTROL_ERROR_NONE) {
		/*SETTING_TRACE(" >>>>>>>>>>>>>>>>>>> (%s) ", output_url); */
		if (0 == safeStrCmp(output_url, "http://tizen.org/appcontrol/operation/configure/font/type")) {
			fontUG->viewmode = FONT_SEL_VIEW_APPCONTROL;
			setting_view_node_table_register(&setting_view_font_font_size, &setting_view_font_main);
			setting_view_node_table_register(&setting_view_font_main, NULL);

			FREE(output_url);
			return &setting_view_font_main;
		} else if (0 == safeStrCmp(output_url, "http://tizen.org/appcontrol/operation/configure/font/size")) {
			fontUG->viewmode = FONT_SIZE_VIEW_APPCONTROL;
			setting_view_node_table_register(&setting_view_font_font_size, NULL);

			FREE(output_url);
			return &setting_view_font_font_size;
		} else {
			SETTING_TRACE("No URL, default action - argument parsing");
			FREE(output_url);
			return __default_view_state(fontUG, service);
		}
	} else {
		SETTING_TRACE("failed to call app_control_get_operation with error code %d", ret);
		if (output_url) {
			FREE(output_url);
		}
		return NULL;
	}
}

setting_view *__get_font_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");
	SettingFontUG *fontUG = (SettingFontUG *)data;

	/*if ug is called by accessibility, view node table should not be initialized */
	char *caller = NULL;
	app_control_get_extra_data(service, "caller", &caller);
	if (0 != safeStrCmp(caller, "accessibility")) {
		/*need to initialize the node table when UG is re-created and it is NOT loaded by another UG*/
		setting_view_node_table_intialize();
	}
	FREE(caller);

	/* if argument is there, process it first */
	char *output_url = NULL;
	int ret = app_control_get_operation(service, &output_url);

	/* service OK, but there's no argument */
	if ((ret == APP_CONTROL_ERROR_NONE)
		&& (0 == safeStrCmp(output_url, "http://tizen.org/appcontrol/operation/default"))) {
		FREE(output_url);
		return __default_view_state(fontUG, service);

	} else if (ret == APP_CONTROL_ERROR_NONE) {
		FREE(output_url);
		return __app_control_get_operation_handler(fontUG, service);

	} else {
		if (output_url) {
			FREE(output_url);
		}
		/* service is not OK, but need to handle default action */
		return __default_view_state(fontUG, service);
	}
}

static void setting_font_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
									  void *event_info)
{
	SettingFontUG *ad = (SettingFontUG *) data;
	setting_view_update(&setting_view_font_main, ad);
}

static void *setting_font_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
									   app_control_h service, void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SettingFontUG *fontUG = priv;

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	fontUG->ug = ug;
	fontUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	fontUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(fontUG->win_main_layout);
	fontUG->evas = evas_object_evas_get(fontUG->win_main_layout);

	setting_retvm_if(fontUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(fontUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(fontUG->itc_1icon_1text_sub));
	/*setting_create_Gendial_itc("dialogue/2text.3", &(fontUG->itc_2text_2)); */
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(fontUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(fontUG->itc_group_item));
	setting_create_Gendial_itc("1icon", &(fontUG->itc_1icon));

	/* view type checking */
	fontUG->view_to_load = __get_font_view_to_load(fontUG, service);
	setting_retvm_if(NULL == fontUG->view_to_load, NULL, "NULL == fontUG->view_to_load");

	setting_view_node_set_cur_view(fontUG->view_to_load);
	setting_view_create(fontUG->view_to_load, (void *)fontUG);

	elm_theme_extension_add(NULL, SETTING_5STEP_SLIDER_EDJ_NAME);

	/*  creating a view. */
	/*setting_view_create(&setting_view_font_main, (void *)fontUG); */
	evas_object_event_callback_add(fontUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_font_ug_cb_resize, fontUG);
	return fontUG->ly_main;
}

static void setting_font_ug_on_start(ui_gadget_h ug, app_control_h service,
									 void *priv)
{
}

static void setting_font_ug_on_pause(ui_gadget_h ug, app_control_h service,
									 void *priv)
{
}

static void setting_font_ug_on_resume(ui_gadget_h ug, app_control_h service,
									  void *priv)
{
}

static void setting_font_ug_on_destroy(ui_gadget_h ug, app_control_h service,
									   void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingFontUG *fontUG = priv;

	evas_object_event_callback_del(fontUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_font_ug_cb_resize);	/* fix flash issue for gallery */
	fontUG->ug = ug;

	setting_view_destroy(&setting_view_font_font_size, fontUG);
	setting_view_destroy(&setting_view_font_main, fontUG);

	if (NULL != ug_get_layout(fontUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(fontUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(fontUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_font_ug_on_message(ui_gadget_h ug, app_control_h msg,
									   app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}


static void setting_font_ug_on_event(ui_gadget_h ug, enum ug_event event,
									 app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *ad = (SettingFontUG *)priv;
	setting_retm_if(NULL == ad, "ad is NULL");
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		setting_navi_items_update(ad->navibar);
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

static void setting_font_ug_on_key_event(ui_gadget_h ug,
										 enum ug_key_event event, app_control_h service,
										 void *priv)
{
	SETTING_TRACE_BEGIN;
	if (!ug) {
		return;
	}

	switch (event) {
	case UG_KEY_EVENT_END:
		ug_destroy_me(ug);
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *fontUG = calloc(1, sizeof(SettingFontUG));
	setting_retvm_if(!fontUG, -1, "Create SettingFontUG obj failed");

	ops->create = setting_font_ug_on_create;
	ops->start = setting_font_ug_on_start;
	ops->pause = setting_font_ug_on_pause;
	ops->resume = setting_font_ug_on_resume;
	ops->destroy = setting_font_ug_on_destroy;
	ops->message = setting_font_ug_on_message;
	ops->event = setting_font_ug_on_event;
	ops->key_event = setting_font_ug_on_key_event;
	ops->priv = fontUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	memset(fontUG, 0x00, sizeof(SettingFontUG));

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingFontUG *fontUG;
	setting_retm_if(!ops, "ops == NULL");

	fontUG = ops->priv;
	if (fontUG) {
		FREE(fontUG);
	}
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/**
* Reset function to 'reset' the settings of the UG, it will be invoked by 'Reset' UG
*/
UG_MODULE_API int setting_plugin_reset(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;

	/* reset font type, default is HelveticaNeue */
	/*font_config_set("HelveticaNeue"); */
	char *default_font_name = NULL;
	default_font_name = _get_default_font();
	if (NULL != default_font_name) {
		ret += system_settings_set_value_string(SYSTEM_SETTINGS_KEY_FONT_TYPE, default_font_name);
	} else {
		SETTING_TRACE_ERROR("Failed to get default font type");
	}
	ret += vconf_set_int(VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, SYSTEM_SETTINGS_FONT_SIZE_NORMAL);
	setting_retvm_if(ret == -1, ret, "Failed to set vconf");
	font_size_set();

	return ret;
}
static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_ST_BODY_FONT_TYPE", NULL, "viewtype:FontType_expand;tab:first;keyword:IDS_ST_BODY_FONT_TYPE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_MBODY_FONT_SIZE", NULL, "viewtype:FontSize_expand;tab:first;keyword:IDS_ST_MBODY_FONT_SIZE", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
};

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-font-efl DB search code");
	setting_retvm_if(!priv || !applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "pplist/applocale is NULL");

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, IMG_Font, s_cfg_node_array[i].item_type,  s_cfg_node_array[i].data, "Font");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

