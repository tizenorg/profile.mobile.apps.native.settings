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
 *@defgroup setting-ringtone
 *UG creation code for setting-ringtone
 * html file viewer
 */

#include <setting-ringtone.h>
#include <setting-ringtone-util.h>

#define SETTING_RINGTONE_PACKAGE_NAME "org.tizen.setting-ringtone"
#define RINGTONE_SETTING_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-ringtone/res/locale"
#define SETTING_RINGTONE_EDJEDIR _TZ_SYS_RO_APP"/org.tizen.setting-ringtone/res/edje"

bool __parse_ug_argument(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	char *default_str = NULL;
	SettingRingtoneUG *ad = priv;
	char *silent = NULL;
	ad->file_path = NULL;
	ad->dir_path = NULL;

	app_control_get_extra_data(service, "marked_mode", &(ad->file_path));
	setting_retvm_if(!ad->file_path, FALSE,
			"no arguement to specialize file");

	app_control_get_extra_data(service, "path", &(ad->dir_path));
	setting_retvm_if(!ad->dir_path, FALSE,
			"no arguement to specialize file");

	app_control_get_extra_data(service, "silent", &silent);
	if (!safeStrCmp(silent, "silent show"))
		ad->is_show_silent = 1;

	app_control_get_extra_data(service, "default", &default_str);
	if (!safeStrCmp(default_str, "default show"))
		ad->is_show_def = 1;

	SETTING_TRACE("dir path: %s", ad->dir_path);
	SETTING_TRACE("current file path: %s", ad->file_path);
	SETTING_TRACE("show silent: %d", ad->is_show_silent);
	return TRUE;
}

/**
 * Event process when the sizeof UG view changes
 *
 * @param data
 * @param e
 * @param obj
 * @param event_info
 */
static void setting_ringtone_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingRingtoneUG *ad = (SettingRingtoneUG *)data;
	setting_view_update(&setting_view_ringtone_main, ad);
}

/**
 * on_create function of the UG
 *
 * @param ug
 * @param mode
 * @param data
 * @param priv
 *
 * @return
 */
static bool _setting_ringtone_app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	bindtextdomain(SETTING_PACKAGE, RINGTONE_SETTING_LOCALEDIR);

	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingRingtoneUG *ringtoneUG = priv;
//ringtoneUG->ug = ug;

	if (app_init(&ringtoneUG->md, SETTING_RINGTONE_PACKAGE_NAME)
	        != SETTING_RETURN_SUCCESS) {
	    SETTING_TRACE_ERROR("Cannot initialize application");
	    return false;
	}

	elm_theme_extension_add(NULL, SETTING_RINGTONE_EDJEDIR"/setting-genlist.edj");
	elm_theme_extension_add(NULL, SETTING_RINGTONE_EDJEDIR"/setting-theme.edj");

	/*	creating a view. */
	//if (!__parse_ug_argument(service, priv)) {
		//SETTING_TRACE_ERROR("Invalid arguement");
		//return false;
	//}
	ringtoneUG->is_show_silent = 1;
	ringtoneUG->is_show_def = 1;
	ringtoneUG->dir_path = strdup(_TZ_SYS_SHARE"/settings/Ringtones");
//if (0 != app_control_clone(&(ringtoneUG->source_svc), service))
	//SETTING_TRACE_ERROR("failed to clone svc");

	setting_view_create(&setting_view_ringtone_main, (void *)ringtoneUG);
	evas_object_event_callback_add(ringtoneUG->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_ringtone_ug_cb_resize,
			ringtoneUG);
	return true;
}

static void _setting_ringtone_app_on_pause(void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void _setting_ringtone_app_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void _setting_ringtone_app_controll(app_control_h service, void *priv)
{

}

static void _setting_ringtone_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingRingtoneUG *ringtoneUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ringtoneUG->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_ringtone_ug_cb_resize);

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_ringtone_main, ringtoneUG);

/*
	if (ringtoneUG->source_svc) {
		app_control_destroy(ringtoneUG->source_svc);
		ringtoneUG->source_svc = NULL;
	}
*/

	evas_object_del(ringtoneUG->md.win_main);
	ringtoneUG->md.win_main = NULL;

	SETTING_TRACE_END;
}

static void _lang_changed(app_event_info_h event_info, void *priv)
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
        .create = _setting_ringtone_app_create,
        .pause = _setting_ringtone_app_on_pause,
        .resume = _setting_ringtone_app_resume,
        .terminate = _setting_ringtone_app_terminate,
        .app_control = _setting_ringtone_app_controll
    };

    SettingRingtoneUG app_data;
    memset(&app_data, 0, sizeof(SettingRingtoneUG));

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

    return ui_app_main(argc, argv, &ops, &app_data);

}
