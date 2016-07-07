/*
 * accessibility
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Contact: Tomasz Olszak <t.olszak@samsung.com>
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

#include <app.h>
#include <appcore-common.h>

#include "setting-cfg.h"
#include "setting-debug.h"
#include "setting-accessibility.h"

#define SETTING_ACCESSIBILITY_PACKAGE_NAME "org.tizen.setting-accessibility"
#define ACCESSIBILITY_LOCALEDIR _TZ_SYS_RO_APP"/"\
	SETTING_ACCESSIBILITY_PACKAGE_NAME"/res/locale"

static bool on_app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingAccessibility *ad = priv;

	setting_set_i18n(SETTING_PACKAGE, ACCESSIBILITY_LOCALEDIR);

	if (app_init(&ad->md, SETTING_ACCESSIBILITY_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_accessibility_main,
			NULL);

	setting_view_node_set_cur_view(&setting_view_accessibility_main);

	/* creating a view. */
	setting_view_create(&setting_view_accessibility_main, (void *)ad);

	return true;
}

static void on_app_pause(void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void on_app_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void on_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_view_destroy(&setting_view_accessibility_main, priv);
	SETTING_TRACE_END;
}

static void on_app_control(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
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
	SettingAccessibility app_data;

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
