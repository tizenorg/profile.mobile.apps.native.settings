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

#include <setting-time.h>
#include <setting-cfg.h>
#include <unicode/uloc.h>

#define SETTING_TIME_PACKAGE_NAME "org.tizen.setting-time"
#define SETTING_TIME_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-time/res/locale"

/**
* @callback handler for EVAS_CALLBACK_RESIZE
*
* @param data user data (SettingTimeUG *)
*/
static void _setting_time_resize_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) data;
	setting_view_update(&setting_view_time_main, ad);
}

/**
* @callback function called first while loading setting-time UG.
*
* @param ug ug object itself.
* @param mode
* @param data bundle (key, value pair) data as arguments into UG.
* @param priv SettingTimeUG context
*
* @return
*/
static bool _setting_time_app_create(void *priv)
{
	SettingTimeUG *timeUG = (SettingTimeUG *)priv;
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	if (app_init(&timeUG->md, SETTING_TIME_PACKAGE_NAME)
	        != SETTING_RETURN_SUCCESS) {
	    SETTING_TRACE_ERROR("Cannot initialize application");
	    return false;
	}

	timeUG->pop_progress = NULL;
	/*	Initialize Caller value */
	timeUG->caller = NULL;

	/* set launguage */
	setting_set_i18n(SETTING_PACKAGE, SETTING_TIME_LOCALEDIR);

	/* register view node table */
	elm_theme_extension_add(NULL, SETTING_TIME_EDJEDIR"/setting-genlist.edj");
	//elm_theme_extension_add(NULL, SETTING_TIME_EDJEDIR"/setting-theme.edj");

	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_time_main, NULL);

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(timeUG->itc_1text_1icon));

	/*date and time buttons item calss: */
	setting_create_Gendial_itc("entry.main", &(timeUG->itc_layout));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_1icon_1text_sub));

	timeUG->is_expand_time_format_list = 0;

	//char *viewtype = NULL;
	//app_control_get_extra_data(service, "viewtype", &viewtype);
	//if (!safeStrCmp(viewtype, "format_expand"))
		//timeUG->is_expand_time_format_list = 1;

	/*	creating a view. */
	setting_view_node_set_cur_view(&setting_view_time_main);
	setting_view_create(&setting_view_time_main, (void *)timeUG);
	evas_object_event_callback_add(timeUG->md.view_layout,
			EVAS_CALLBACK_RESIZE,
			_setting_time_resize_cb, timeUG);

	return true;
}

static void _setting_time_app_pause(void *data)
{
}

static void _setting_time_app_resume(void *data)
{
}

static void _setting_time_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingTimeUG *timeUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(timeUG->md.view_layout,
			EVAS_CALLBACK_RESIZE, _setting_time_resize_cb);

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_time_main, timeUG);

	FREE(timeUG);

	SETTING_TRACE_END;
}


static void _setting_time_app_controll(app_control_h service, void *data)
{
}

static void _setting_time_lang_changed(app_event_info_h event_info, void *data)
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
	SettingTimeUG app_data;
	app_event_handler_h handlers[5] = {NULL, };

	ui_app_lifecycle_callback_s ops = {
		.create = _setting_time_app_create,
		.pause = _setting_time_app_pause,
		.resume = _setting_time_app_resume,
		.terminate = _setting_time_app_terminate,
		.app_control = _setting_time_app_controll,
	};

	memset(&app_data, 0, sizeof(app_data));

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);

	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _setting_time_lang_changed, &app_data);

	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);

	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	return ui_app_main(argc, argv, &ops, &app_data);
}

int setting_plugin_search_init(app_control_h service, void *priv,
		char **applocale)
{
	int i, size;
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = priv;

	const Setting_Cfg_Node_T search_configs[] = {
		{DATE_TIME_STR_AUTOMATIC_UPDATE, NULL, NULL, 0,
				Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
				NULL, DATE_TIME_STR_DATE_AND_TIME, NULL, NULL },
		{DATE_TIME_STR_TIME_ZONE, NULL, NULL, 0,
				Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
				NULL, DATE_TIME_STR_DATE_AND_TIME, NULL, NULL },

		/* Duplicated title name : 'Date and time' with
		 * UG name : 'Date and time' */
		{DATE_TIME_STR_DATE_AND_TIME, NULL, NULL,  0,
				Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
				NULL, DATE_TIME_STR_DATE_AND_TIME, NULL, NULL },
		{DATE_TIME_STR_TIME_FORMAT, NULL, "viewtype:format_expand", 0,
				Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
				NULL, DATE_TIME_STR_DATE_AND_TIME, NULL, NULL }
	};

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting"
			"/res/locale");

	size = sizeof(search_configs) / sizeof(Setting_Cfg_Node_T);

	for (i = 0; i < size; i++) {
		node = setting_plugin_search_item_subindex_add(
				   search_configs[i].key_name,
				   search_configs[i].ug_args,
				   IMG_DateTime,
				   search_configs[i].item_type,
				   search_configs[i].data,
				   search_configs[i].sub_index);

		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}
