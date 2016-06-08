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

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_sound_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	ret_if(data == NULL);

	SettingProfileUG *ad = (SettingProfileUG *)data;
	setting_view_update(&setting_view_sound_main, ad);
}

static void *setting_sound_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	/*appcore_measure_start(); */
	retv_if(priv == NULL, NULL);

	SettingProfileUG *profileUG = priv;
	profileUG->ug = ug;

	profileUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	profileUG->win_get = (Evas_Object *)ug_get_window();
	evas_object_show(profileUG->win_main_layout);
	profileUG->evas = evas_object_evas_get(profileUG->win_main_layout);
	setting_retvm_if(profileUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	app_control_get_caller(service, &(profileUG->viewtype));
	setting_sound_init(profileUG);
	evas_object_event_callback_add(profileUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_sound_ug_cb_resize,
			profileUG);
	return profileUG->ly_main;
}

static void setting_sound_ug_on_start(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_sound_ug_on_pause(ui_gadget_h ug, app_control_h service,
		void *priv)
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

static void setting_sound_ug_on_resume(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	ret_if(priv == NULL);
	SettingProfileUG *profileUG = (SettingProfileUG *)priv;
	/* If current view is sound main, block volume app */
	/* To prevent conflict with volume app */
	if (!safeStrCmp(profileUG->viewtype, "com.samsung.volume")) {
		vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 2);
	} else {
		vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 0);
	}

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

static void setting_sound_ug_on_destroy(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);

	SettingProfileUG *ad = priv;
	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_sound_ug_cb_resize);
	setting_sound_deinit(ad);

	if (NULL != ug_get_layout(ad->ug)) {
		evas_object_hide((Evas_Object *)ug_get_layout(ad->ug));
		evas_object_del((Evas_Object *)ug_get_layout(ad->ug));
	}
	SETTING_TRACE_END;
}

static void setting_sound_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
}

static void setting_sound_ug_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	/*SettingProfileUG *ad = (SettingProfileUG *)priv; */

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_sound_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	ret_if(priv == NULL);

	/*SettingProfileUG *ad = (SettingProfileUG *) priv; */

	switch (event) {
	case UG_KEY_EVENT_END:
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	/*appcore_measure_start(); */
	SettingProfileUG *profileUG = calloc(1, sizeof(SettingProfileUG));
	setting_retvm_if(!profileUG, -1, "Create SettingProfileUG obj failed");

	ops->create = setting_sound_ug_on_create;
	ops->start = setting_sound_ug_on_start;
	ops->pause = setting_sound_ug_on_pause;
	ops->resume = setting_sound_ug_on_resume;
	ops->destroy = setting_sound_ug_on_destroy;
	ops->message = setting_sound_ug_on_message;
	ops->event = setting_sound_ug_on_event;
	ops->key_event = setting_sound_ug_on_key_event;
	ops->priv = profileUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	memset(profileUG, 0x00, sizeof(SettingProfileUG));

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	ret_if(ops == NULL);

	struct SettingProfileUG *profileUG;
	profileUG = ops->priv;
	if (profileUG)
		FREE(profileUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

