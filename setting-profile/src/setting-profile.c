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

#include <setting-profile.h>

#define SETTING_PROFILE_PACKAGE_NAME "org.tizen.setting-profile"


static void setting_sound_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);

	SettingProfileUG *ad = (SettingProfileUG *)data;
	setting_view_update(&setting_view_sound_main, ad);
}


static bool _setting_sound_app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	/*appcore_measure_start(); */
	retv_if(priv == NULL, NULL);

	SettingProfileUG *profileUG = priv;

	if (app_init(&profileUG->md, VOLUME_APP_NAME)
	        != SETTING_RETURN_SUCCESS) {
	    SETTING_TRACE_ERROR("Cannot initialize application");
	    return false;
	}

//profileUG->md.view_layout = (Evas_Object *)ug_get_parent_layout(ug);
//profileUG->win_get = (Evas_Object *)ug_get_window();
//evas_object_show(profileUG->md.view_layout);

//app_control_get_caller(service, &(profileUG->viewtype));
	if (setting_sound_init(profileUG) == NULL)
		return false;

	evas_object_event_callback_add(profileUG->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_sound_ug_cb_resize,
			profileUG);
	return true;
}

static void _setting_sound_app_pause(void *priv)
{
	ret_if(priv == NULL);
	SettingProfileUG *profileUG = (SettingProfileUG *)priv;

	if (profileUG->calling_popup) {
		evas_object_del(profileUG->calling_popup);
		profileUG->calling_popup = NULL;
	}

	/* if click home key, players have to be closed */
	setting_sound_close_all_mm_players(profileUG);

	/* If sound ug is paused, others can use volume HW key popup (set 0) */
	vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 0);
}

static void _setting_sound_app_resume(void *priv)
{
	ret_if(priv == NULL);
	SettingProfileUG *profileUG = (SettingProfileUG *)priv;
	/* If current view is sound main, block volume app */
	/* To prevent conflict with volume app */
	if (!safeStrCmp(profileUG->viewtype, "com.samsung.volume"))
		vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 2);
	else
		vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 0);

	/* update path of tones */
	char *pa_tone_path = NULL;

	/* 1. incoming call ringtone */
	if (profileUG->data_call_alert_tone) {
		SETTING_TRACE_DEBUG("change call_alert_tone %x",
				profileUG->data_call_alert_tone);
		pa_tone_path = vconf_get_str(
				VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		if ((pa_tone_path == NULL) || (setting_sound_check_file_exist(
				profileUG, pa_tone_path) == SETTING_RETURN_FAIL)) {
			if (isEmulBin())
			pa_tone_path = (char *)strdup(SETTING_DEFAULT_CALL_TONE_SDK);
			else
			pa_tone_path = (char *)strdup(SETTING_DEFAULT_CALL_TONE);
			if (vconf_set_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
					pa_tone_path) < 0) {
				FREE(pa_tone_path);
				return;
			}
			profileUG->data_call_alert_tone->sub_desc = setting_media_basename(
					pa_tone_path);
			elm_genlist_item_update(
					profileUG->data_call_alert_tone->item);
		}
		FREE(pa_tone_path);
	}

	/* 2. message ringtone */
	if (profileUG->data_msg_alert_tone) {
		SETTING_TRACE_DEBUG("change msg_alert_tone %x",
				profileUG->data_msg_alert_tone);
		pa_tone_path = vconf_get_str(
				VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
		if ((pa_tone_path == NULL) || (setting_sound_check_file_exist(
				profileUG, pa_tone_path) == SETTING_RETURN_FAIL)) {
			pa_tone_path = (char *)strdup(SETTING_DEFAULT_MSG_TONE);
			if (vconf_set_str(
					VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
					SETTING_DEFAULT_MSG_TONE) < 0) {
				FREE(pa_tone_path);
				return;
			}

			profileUG->data_msg_alert_tone->sub_desc = setting_media_basename(
					pa_tone_path);
			elm_genlist_item_update(
					profileUG->data_msg_alert_tone->item);
		}
		FREE(pa_tone_path);
	}

	int sound_value = 0, volume = 0;

	if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
			&sound_value) < 0)
		sound_value = TRUE;

	(void)sound_manager_get_volume(SOUND_TYPE_RINGTONE, &volume);
	if (sound_value == FALSE)
		volume = 0;
	if (profileUG->data_call_volume
			&& profileUG->data_call_volume->eo_check) {
		profileUG->data_call_volume->chk_status = volume;
		elm_slider_value_set(profileUG->data_call_volume->eo_check,
				volume);
	}

	(void)sound_manager_get_volume(SOUND_TYPE_NOTIFICATION, &volume);
	if (sound_value == FALSE)
		volume = 0;
	if (profileUG->data_noti_volume
			&& profileUG->data_noti_volume->eo_check) {
		profileUG->data_noti_volume->chk_status = volume;
		elm_slider_value_set(profileUG->data_noti_volume->eo_check,
				volume);
	}

	(void)sound_manager_get_volume(SOUND_TYPE_MEDIA, &volume);
	if (profileUG->data_media_volume
			&& profileUG->data_media_volume->eo_check) {
		profileUG->data_media_volume->chk_status = volume;
		elm_slider_value_set(profileUG->data_media_volume->eo_check,
				volume);
	}

	(void)sound_manager_get_volume(SOUND_TYPE_SYSTEM, &volume);
	if (sound_value == FALSE)
		volume = 0;
	if (profileUG->data_touch_volume
			&& profileUG->data_touch_volume->eo_check) {
		profileUG->data_touch_volume->chk_status = volume;
		elm_slider_value_set(profileUG->data_touch_volume->eo_check,
				volume);
	}

	setting_sound_update_slider_icon(profileUG->data_call_volume,
			SND_SLIDER_CALL);
	setting_sound_update_slider_icon(profileUG->data_noti_volume,
			SND_SLIDER_NOTI);
	setting_sound_update_slider_icon(profileUG->data_media_volume,
			SND_SLIDER_MEDIA);
	setting_sound_update_slider_icon(profileUG->data_touch_volume,
			SND_SLIDER_SYSTEM);
	setting_sound_update_do_not_disturb_item(profileUG);
}

static void _setting_sound_app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);

	SettingProfileUG *ad = priv;
	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE, setting_sound_ug_cb_resize);
	setting_sound_deinit(ad);

    evas_object_del(ad->md.win_main);
    ad->md.win_main = NULL;

	SETTING_TRACE_END;
}

static void _setting_sound_app_controll(app_control_h service, void *data)
{

}

static void _setting_sound_lang_changed(app_event_info_h event_info, void *data)
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
        .create = _setting_sound_app_create,
        .pause = _setting_sound_app_pause,
        .resume = _setting_sound_app_resume,
        .terminate = _setting_sound_app_terminate,
        .app_control = _setting_sound_app_controll,
    };

    SettingProfileUG app_data;
    memset(&app_data, 0, sizeof(SettingProfileUG));

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
            APP_EVENT_LOW_MEMORY, NULL, NULL);

    ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
            APP_EVENT_LOW_BATTERY, NULL, NULL);

    ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
            APP_EVENT_LANGUAGE_CHANGED, _setting_sound_lang_changed, &app_data);

    ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
            APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);

    ui_app_add_event_handler(
            &handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
            APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

    return ui_app_main(argc, argv, &ops, &app_data);
}
