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

#include "setting-moreconnections.h"

#define SETTING_MORECONNECTIONS_PACKAGE_NAME "org.tizen.setting-moreconnections"
#define MORECONNECTIONS_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-moreconnections/res/locale"

bool on_app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingMoreConnections *ad = priv;
	retv_if(!ad, NULL);

	if (app_init(&ad->md, SETTING_MORECONNECTIONS_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	setting_set_i18n(SETTING_PACKAGE, MORECONNECTIONS_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_moreconnections_main,
			NULL);

	/*	creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text));

	setting_view_node_set_cur_view(&setting_view_moreconnections_main);
	setting_view_create(&setting_view_moreconnections_main, (void *)ad);

	return true;
}

static void on_app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void on_app_control(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
}

static void on_app_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	char *sub_desc;
	SettingMoreConnections *ad = priv;
	ret_if(!priv);

	setting_view_create(&setting_view_moreconnections_main, (void *)ad);

	if (ad->location_service) {
		sub_desc = setting_location_is_enabled(priv);
		ad->location_service->sub_desc = (char *)strdup(sub_desc);
		elm_object_item_data_set(ad->location_service->item,
				ad->location_service);
		elm_genlist_item_update(ad->location_service->item);
	}

	SETTING_TRACE_END;
}

static void on_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingMoreConnections *ad = priv;

	/* called when this shared gadget is terminated. similar with
	 * app_exit */
	setting_view_destroy(&setting_view_moreconnections_main, ad);

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
		.app_control = on_app_control,
		.pause = on_app_pause,
		.resume = on_app_resume,
		.terminate = on_app_terminate,
	};
	SettingMoreConnections app_data;
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

