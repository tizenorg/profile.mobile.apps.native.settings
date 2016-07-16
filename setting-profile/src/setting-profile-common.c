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
#include <sys/stat.h>
#include <dirent.h>
#include <iniparser.h>
#include <app_manager.h>
#include <metadata_extractor.h>
#include <player.h>
#include <sound_manager_internal.h>

#define PLAY_FEEDBACK(f_type, f_pattern) {\
		if (feedback_initialize() == FEEDBACK_ERROR_NONE) {\
			feedback_play_type(f_type, f_pattern);\
			feedback_deinitialize();\
		} \
	}

typedef struct _SliderIcons {
	int index;
	char mute[256];
	char volume[256];
} SliderIcons;

SliderIcons slider_icons[SND_SLIDER_MAX] = {
	{SND_SLIDER_CALL,	ICON_CALL_MUTE,		ICON_CALL_NORMAL},
	{SND_SLIDER_NOTI,	ICON_NOTI_MUTE,		ICON_NOTI_NORMAL},
	{SND_SLIDER_MEDIA,	ICON_MEDIA_MUTE,	ICON_MEDIA_NORMAL},
	{SND_SLIDER_SYSTEM,	ICON_SYSTEM_MUTE,	ICON_SYSTEM_NORMAL},
	/*{SND_SLIDER_MAX,	"",				""} */
};

void setting_sound_update_do_not_disturb_item(void *data)
{
	SettingProfileUG *ad = (SettingProfileUG *) data;
	const char *sub_desc = NULL;

	if (!ad || !ad->data_do_not_disturb)
		return;

	sub_desc = setting_do_not_disturb_is_enable(data);
	g_free(ad->data_do_not_disturb->sub_desc);
	ad->data_do_not_disturb->sub_desc = g_strdup(sub_desc);
	elm_genlist_item_update(ad->data_do_not_disturb->item);
}

char *setting_sound_get_slider_icon(int type, int volume)
{
	if (type < SND_SLIDER_CALL || type >= SND_SLIDER_MAX)
		return NULL;

	SETTING_TRACE("type : %d, volume : %d", type, volume);
	/* 1.Call ringtone has 3 icon status */
	if (SND_SLIDER_CALL == type) {
		/*special process */
		int sound = 0;
		int vibration = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &sound);
		vconf_get_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
				&vibration);

		if (sound && !vibration)
			return ICON_CALL_NORMAL;
		else if (vibration && !sound)
			return ICON_CALL_VIB;
		else if (!vibration && !sound)
			return ICON_CALL_MUTE;

		return ICON_CALL_NORMAL;
	}

	/* 2.Other types, just have two icon status */
	if (volume > 0)
		return slider_icons[type].volume;
	else
		return slider_icons[type].mute;
}

void setting_sound_update_slider_icon(Setting_GenGroupItem_Data *item_data,
		int type)
{
	SETTING_TRACE_BEGIN;
	ret_if(!item_data || !item_data->eo_check);

	Evas_Object *icon = elm_object_content_get(item_data->eo_check);
	if (icon) {
		char *new_file = setting_sound_get_slider_icon(type,
				item_data->chk_status);
		elm_image_file_set(icon, new_file, NULL);
	}
}

char *setting_media_basename(char *path)
{
	retv_if(NULL == path || '\0' == path[0], NULL);	/* invalid arguement */

	if (safeStrCmp(path, "silent") == 0)
		return (char *)g_strdup(_("IDS_ST_BODY_PHONEPROFILES_SILENT"));

	metadata_extractor_h metadata = NULL;
	char *title = NULL;
	int ret = metadata_extractor_create(&metadata);
	if (ret == METADATA_EXTRACTOR_ERROR_NONE && metadata) {
		ret = metadata_extractor_set_path(metadata, path);
		if (ret == METADATA_EXTRACTOR_ERROR_NONE) {
			ret = metadata_extractor_get_metadata(metadata,
					METADATA_TITLE, &title);
			metadata_extractor_destroy(metadata);
			if (title) {
				char *strdup_title = (char *)g_strdup(title);
				FREE(title);
				return strdup_title;
			} else {
				return setting_file_basename(path);
			}
		} else {
			metadata_extractor_destroy(metadata);
			return setting_file_basename(path);
		}
	} else {
		return setting_file_basename(path);
	}
}

/* ------------------------------------------------------------------------- */
/* |		| elm.swallow.icon.0 | elm.text | elm.swallow.icon.1 |	   | */
/* | elm.swallow.icon |----------------------------------| elm.swallow.end | */
/* |		  |	elm.text.sub	  | elm.text.sub.end   |	   | */
/* ------------------------------------------------------------------------- */
static Evas_Object *__sound_slider_icon_get(
		void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	/*appcore_measure_start(); */
	retv_if(data == NULL, NULL);

	SETTING_TRACE(" -----------------> EDC part [%s]", part);
	if (safeStrCmp(part, "elm.swallow.content"))
		return NULL;

	Evas_Object *layout;
	/* Set custom layout style */
	layout = elm_layout_add(obj);

	Setting_GenGroupItem_Data *item_data =
			(Setting_GenGroupItem_Data *)data;
	Evas_Object *slider = setting_create_slider(obj, item_data);

	if (slider == NULL)
		return NULL;

	char *gl_style = "gl_custom_item";
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME, gl_style);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	if (0 == safeStrCmp(item_data->keyStr, "IDS_ST_BODY_MEDIA")) {
		elm_object_style_set(slider, "warning");

		Edje_Message_Float_Set *msg =
				alloca(sizeof(Edje_Message_Float_Set)
						+ (sizeof(double)));
		msg->count = 1;
		/* Warning area point has to be calculated considering rounding
		 * off. For example, value 10's area will be 9.5~10.4.
		 * So, if warning area has to be started at 10, we need to
		 * calculate the start point with 9.5.
		 * Warning start point = (Warning min value - 0.5) /
		 *				(Max_Value - Min_Value) */
		msg->val[0] = 0.633333;
		edje_object_message_send(
				_EDJ(slider), EDJE_MESSAGE_FLOAT_SET, 0, msg);
		elm_slider_indicator_format_set(slider, "%1.0f");
		elm_slider_indicator_show_set(slider, 1);
	}

	item_data->eo_check = slider;

	evas_object_pass_events_set(slider, EINA_TRUE);
	evas_object_propagate_events_set(slider, EINA_FALSE);

	if (item_data->userdata == NULL)
		return NULL;

	/* Set text into layout */
	/*if (0 == safeStrCmp(item_data->keyStr, "IDS_ST_BODY_MEDIA") */
	/*	|| 0 == safeStrCmp(item_data->keyStr, "IDS_ST_BODY_SYSTEM"))
	 * { */
	elm_object_part_text_set(layout, "elm.text", _(item_data->keyStr));
	/*} */
	elm_object_part_content_set(layout, "elm.swallow.content", slider);
	return layout;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
/*resolve P130805-7070 */
#define SETTING_DEFAULT_FOLDER_PATH	_TZ_SYS_SHARE"/settings/"
#define SDCARD_PATH	_TZ_SYS_MEDIA"/sdcard/"

int setting_sound_check_file_exist(void *data, const char *file_path)
{
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(file_path == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*resolve P130805-7070 */
	if (0 == safeStrNCmp(
			file_path,
			SETTING_DEFAULT_FOLDER_PATH,
			safeStrLen(SETTING_DEFAULT_FOLDER_PATH))) {
		SETTING_TRACE("Default file[%s]!", file_path);
		return SETTING_RETURN_SUCCESS;
	}

	if (!ecore_file_exists(file_path)) {
		if (safeStrCmp(file_path, "silent") == 0) {
			SETTING_TRACE("%s", file_path);
			return SETTING_RETURN_SUCCESS;
		}
		SETTING_TRACE_ERROR(
				"Cannot find the ringtone file[%s]!",
				file_path);
		if (0 == safeStrNCmp(
				file_path, SDCARD_PATH,
				safeStrLen(SDCARD_PATH))) {
			char *strdup_file_path = strdup(file_path);
			/*sdcard file*/
			int ret = vconf_set_str(
					VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_STR,
					strdup_file_path);
			if (ret < 0)
				SETTING_TRACE_DEBUG("failed to set vconf");

			FREE(strdup_file_path);
		}
		return SETTING_RETURN_FAIL;
	}

	int mmc_status = VCONFKEY_SYSMAN_MMC_REMOVED;
	vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &mmc_status);
	if (VCONFKEY_SYSMAN_MMC_MOUNTED != mmc_status) {
		if (!safeStrNCmp(
				SETTING_PROFILE_MMC_PATH,
				file_path,
				safeStrLen(SETTING_PROFILE_MMC_PATH))) {
			return SETTING_RETURN_FAIL;
		}
	}
	return SETTING_RETURN_SUCCESS;
}

static void __enable_sound_menu(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingProfileUG *ad = data;
	ad->sound_on = TRUE;
	Setting_GenGroupItem_Data *item_to_update = NULL;


	/*1.Enable "Vibrate when ringing" */
	item_to_update = ad->data_sound_when_ring;
	setting_genlist_item_disabled_set(item_to_update, EINA_FALSE);

	elm_object_item_disabled_set(ad->data_call_volume->item, EINA_FALSE);
	elm_object_disabled_set(ad->data_call_volume->eo_check, EINA_FALSE);

	/*2.Enable "Notification", via Sound Manager */
	item_to_update = ad->data_noti_volume;

	// @todo make sure data_msg_alert_tone is correct for 'Silent' set.
	//if (item_to_update && safeStrCmp(item_to_update->sub_desc, "Silent")) {
	if (ad->data_msg_alert_tone && safeStrCmp(ad->data_msg_alert_tone->sub_desc, "Silent")) {
		//Enable
		if (item_to_update
				&& item_to_update->item
				&& item_to_update->eo_check) {
			int mm_value = 0;
			int ret =  sound_manager_get_volume(
					SOUND_TYPE_NOTIFICATION, &mm_value);
			SETTING_TRACE("ret:%d", ret);
			item_to_update->chk_status = mm_value;
			elm_slider_value_set(
					item_to_update->eo_check,
					item_to_update->chk_status);
			elm_object_item_disabled_set(item_to_update->item, EINA_FALSE);
			elm_object_disabled_set(item_to_update->eo_check, EINA_FALSE);

			setting_sound_update_slider_icon(
					item_to_update, SND_SLIDER_NOTI);
		}
	}

	/*3.Enable "System", via Sound Manager */
	item_to_update = ad->data_touch_volume;
	if (item_to_update && item_to_update->item
			&& item_to_update->eo_check) {
		int mm_value = 0;
		int ret =  sound_manager_get_volume(
				SOUND_TYPE_SYSTEM, &mm_value);
		SETTING_TRACE("ret:%d", ret);
		item_to_update->chk_status = mm_value;
		elm_slider_value_set(
				item_to_update->eo_check,
				item_to_update->chk_status);
		elm_object_item_disabled_set(
				item_to_update->item, EINA_FALSE);
		elm_object_disabled_set(item_to_update->eo_check, EINA_FALSE);

		setting_sound_update_slider_icon(
				item_to_update, SND_SLIDER_SYSTEM);
	}


	/*4.Enable "Call ringtone", via "Backuped" value */
	item_to_update = ad->data_call_volume;
	if (item_to_update && item_to_update->eo_check) {
		SETTING_TRACE("....ad->is_ringtone_slidering:%d, "
				"ad->data_call_volume->chk_status:%d",
				ad->is_ringtone_slidering,
				item_to_update->chk_status);
		/* do need to restore from BACKUP VOLUME */
		if (ad->is_ringtone_slidering) return;

		int mm_value = 0;
		sound_manager_get_volume(SOUND_TYPE_RINGTONE, &mm_value);
		if (mm_value <= 0) mm_value = 1;
		/*SETTING_TRACE("ret:%d", ret); */
		sound_manager_set_volume(SOUND_TYPE_RINGTONE, mm_value);
		item_to_update->chk_status = mm_value;
		elm_slider_value_set(
				item_to_update->eo_check,
				item_to_update->chk_status);
		SETTING_TRACE("ad->data_call_volume->chk_status : %d",
				item_to_update->chk_status);
	}

}

static void __disable_sound_menu(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingProfileUG *ad = data;
	ad->sound_on = FALSE;

	__close_player(ad, SND_SLIDER_NOTI);
	__close_player(ad, SND_SLIDER_MEDIA);

	if (ad->mp_ringtone) {
		SETTING_TRACE_DEBUG("player_set_mute");
		player_set_mute(*(ad->mp_ringtone), 1);
	}
	Setting_GenGroupItem_Data *item_to_update = NULL;

	/*1.Disable	 "Vibrate when ringing" */
	item_to_update = ad->data_sound_when_ring;
	setting_genlist_item_disabled_set(item_to_update, EINA_TRUE);
	/* Disable Call Volume slider if Silent ringtone is selected */
	if (!safeStrCmp(ad->data_call_alert_tone && ad->data_call_alert_tone->sub_desc, "Silent")) {
		elm_object_item_disabled_set(ad->data_call_volume->item, EINA_TRUE);
		elm_object_disabled_set(ad->data_call_volume->eo_check, EINA_TRUE);
	}

	/*2.Disable "Notification" */
	item_to_update = ad->data_noti_volume;
	if (item_to_update && item_to_update->item
			&& item_to_update->eo_check) {
		item_to_update->chk_status = 0;
		elm_slider_value_set(item_to_update->eo_check,
				item_to_update->chk_status);
		elm_object_item_disabled_set(item_to_update->item, EINA_TRUE);
		elm_object_disabled_set(item_to_update->eo_check, EINA_TRUE);

		/* Change notification slider's icon as mute style */
		setting_sound_update_slider_icon(item_to_update,
				SND_SLIDER_NOTI);
	}

	/*3.Disable "System" */
	item_to_update = ad->data_touch_volume;
	if (item_to_update && item_to_update->item
			&& item_to_update->eo_check) {
		item_to_update->chk_status = 0;
		elm_slider_value_set(item_to_update->eo_check,
				item_to_update->chk_status);
		elm_object_item_disabled_set(item_to_update->item, EINA_TRUE);
		elm_object_disabled_set(item_to_update->eo_check, EINA_TRUE);

		/* Change system slider's icon as mute style */
		setting_sound_update_slider_icon(item_to_update,
				SND_SLIDER_SYSTEM);
	}

	/*4.Disable "Call ringtone" */
	item_to_update = ad->data_call_volume;
	if (item_to_update && item_to_update->eo_check) {
		/*DO NOT CHANGE Backup Values(
		 * VCONFKEY_SETAPPL_RINGTONE_BACKUP_VOLUME) */
		item_to_update->chk_status = 0;
		elm_slider_value_set(item_to_update->eo_check,
				item_to_update->chk_status);
		SETTING_TRACE("item_to_update->chk_status : %d",
				item_to_update->chk_status);
	}

}

static void ___sound_vconf_change_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);
	SettingProfileUG *ad = data;
	int status = 0;
	int ret = 0;
	char *pa_ringtone = NULL;
	char *sub_desc = NULL;
	char *vconf_name = vconf_keynode_get_name(key);
	Setting_GenGroupItem_Data *item_to_update = NULL;

	if (!safeStrCmp(vconf_name, VCONFKEY_SYSMAN_MMC_STATUS)) {
		status = vconf_keynode_get_int(key);
		SETTING_TRACE("status:%d", status);
		if (VCONFKEY_SYSMAN_MMC_MOUNTED != status) {
			/* when remove mmc card, change default tone */
			char *call_tone = vconf_get_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
			if (call_tone && !safeStrNCmp(
					SETTING_PROFILE_MMC_PATH,
					call_tone,
					safeStrLen(SETTING_PROFILE_MMC_PATH))) {
				if (isEmulBin()) {
					ret = vconf_set_str(
							VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
							SETTING_DEFAULT_CALL_TONE_SDK);
				} else {
					ret = vconf_set_str(
							VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
							SETTING_DEFAULT_CALL_TONE);
				}
				if (ret < 0)
					SETTING_TRACE_DEBUG("failed to set vconf");
			}
			FREE(call_tone);

			ret_if((!ad->mp_ringtone && !ad->mp_noti
					&& !ad->mp_media));
			SETTING_TRACE("song_playing:%s", ad->song_playing);

			if (!safeStrNCmp(SETTING_PROFILE_MMC_PATH,
					ad->song_playing,
					safeStrLen(SETTING_PROFILE_MMC_PATH))) {
				/*	close mm plaer */
				setting_sound_close_all_mm_players(ad);
				/*	close vibration */
			}
		}
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_SOUND_STATUS_BOOL)) {
		status = vconf_keynode_get_bool(key);
		SETTING_TRACE_DEBUG("sound status is changed. %d", status);
		if (status)
			__enable_sound_menu(ad);
		else
			__disable_sound_menu(ad);

		setting_sound_update_slider_icon(ad->data_call_volume,
				SND_SLIDER_CALL);
		/*FIX P141016-02659 Only In sound mode should enable
		 * "vibrate when ringing" */
		setting_genlist_item_disabled_set(ad->data_sound_when_ring,
				!status);
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL)) {
		status = vconf_keynode_get_bool(key);
		SETTING_TRACE_DEBUG("vibration status is changed. %d", status);
		if (status) {
			PLAY_FEEDBACK(FEEDBACK_TYPE_VIBRATION,
					FEEDBACK_PATTERN_VIBRATION_ON)
		}

		setting_sound_update_slider_icon(ad->data_call_volume,
				SND_SLIDER_CALL);
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_VIBRATE_WHEN_RINGING_BOOL)) {
		item_to_update = ad->data_sound_when_ring;
		status = vconf_keynode_get_bool(key);
		if (item_to_update && item_to_update->eo_check) {
			item_to_update->chk_status = status;
			elm_check_state_set(item_to_update->eo_check,
					item_to_update->chk_status);
		}
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_BUTTON_SOUNDS_BOOL)) {
		item_to_update = ad->data_button_sounds;
		status = vconf_keynode_get_bool(key);
		if (item_to_update && item_to_update->eo_check) {
			item_to_update->chk_status = status;
			elm_check_state_set(item_to_update->eo_check,
					item_to_update->chk_status);
		}
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_TOUCH_SOUNDS_BOOL)) {
		item_to_update = ad->data_touch_sounds;
		status = vconf_keynode_get_bool(key);
		if (item_to_update && item_to_update->eo_check) {
			item_to_update->chk_status = status;
			elm_check_state_set(item_to_update->eo_check,
					item_to_update->chk_status);
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_SOUND_LOCK_BOOL)) {
		item_to_update = ad->data_lock_sound;
		status = vconf_keynode_get_bool(key);
		if (item_to_update && item_to_update->eo_check) {
			item_to_update->chk_status = status;
			elm_check_state_set(item_to_update->eo_check,
					item_to_update->chk_status);
		}
	}
#if 0
	else if (!safeStrCmp(vconf_name, VCONFKEY_ISE_KEYSOUND)) {
		item_to_update = ad->data_keyboard_sound;
		status = vconf_keynode_get_bool(key);
		if (item_to_update && item_to_update->eo_check) {
			item_to_update->chk_status = status;
			elm_check_state_set(item_to_update->eo_check,
					item_to_update->chk_status);
		}
	}
#endif
	else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR)) {
		item_to_update = ad->data_call_alert_tone;

		pa_ringtone = (char *)g_strdup(vconf_keynode_get_str(key));
		if (setting_sound_check_file_exist(ad, pa_ringtone) ==
				SETTING_RETURN_FAIL) {
			if (isEmulBin()) {
				G_FREE(pa_ringtone);
				pa_ringtone = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			} else {
				G_FREE(pa_ringtone);
				pa_ringtone = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE);
			}
			ret = vconf_set_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
					pa_ringtone);
			if (ret < 0)
				SETTING_TRACE_DEBUG("failed to set vconf");
		}
		sub_desc = setting_media_basename(pa_ringtone);
		if (item_to_update) {
			item_to_update->sub_desc = (char *)g_strdup(sub_desc);
			elm_genlist_item_update(item_to_update->item);
		}
		G_FREE(pa_ringtone);
		G_FREE(sub_desc);
	} else if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR)) {
		item_to_update = ad->data_msg_alert_tone;
		pa_ringtone = (char *)g_strdup(vconf_keynode_get_str(key));
		if (setting_sound_check_file_exist(ad, pa_ringtone) ==
				SETTING_RETURN_FAIL) {
			G_FREE(pa_ringtone);
			pa_ringtone = (char *)g_strdup(
					SETTING_DEFAULT_MSG_TONE);
			ret = vconf_set_str(
					VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
					pa_ringtone);
			if (ret < 0)
				SETTING_TRACE_DEBUG("failed to set vconf");
		}
		sub_desc = setting_media_basename(pa_ringtone);
		if (item_to_update) {
			item_to_update->sub_desc = (char *)g_strdup(sub_desc);
			elm_genlist_item_update(item_to_update->item);
		}
		G_FREE(pa_ringtone);
		G_FREE(sub_desc);
	}
}

int setting_sound_listen_vconf_change(void *data)
{
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*Ints */
	(void)vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			___sound_vconf_change_cb, data);

	/*Bools */
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
			___sound_vconf_change_cb, data);
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
			___sound_vconf_change_cb, data);

	(void)vconf_notify_key_changed(
			VCONFKEY_SETAPPL_VIBRATE_WHEN_RINGING_BOOL,
			___sound_vconf_change_cb, data);
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_BUTTON_SOUNDS_BOOL,
			___sound_vconf_change_cb, data);
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_TOUCH_SOUNDS_BOOL,
			___sound_vconf_change_cb, data);
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_SOUND_LOCK_BOOL,
			___sound_vconf_change_cb, data);
	/*	(void)vconf_notify_key_changed(VCONFKEY_ISE_KEYSOUND,
	 *  ___sound_vconf_change_cb, data); */

	/*Strings */
	(void)vconf_notify_key_changed(VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
			___sound_vconf_change_cb, data);
	(void)vconf_notify_key_changed(
			VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
			___sound_vconf_change_cb, data);

	return 0;
}

int setting_sound_unlisten_vconf_change(void *data)
{
	/* unused listen */
	/*Ints */
	(void)vconf_ignore_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			___sound_vconf_change_cb);

	/*Bools */
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
			___sound_vconf_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
			___sound_vconf_change_cb);

	(void)vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_VIBRATE_WHEN_RINGING_BOOL,
			___sound_vconf_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_BUTTON_SOUNDS_BOOL,
			___sound_vconf_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_TOUCH_SOUNDS_BOOL,
			___sound_vconf_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_SOUND_LOCK_BOOL,
			___sound_vconf_change_cb);
	/*(void)vconf_ignore_key_changed(VCONFKEY_ISE_KEYSOUND,
	 *  ___sound_vconf_change_cb); */

	/*Strings */
	(void)vconf_ignore_key_changed(VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
			___sound_vconf_change_cb);
	(void)vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
			___sound_vconf_change_cb);
	return 0;
}

static Eina_Bool __volume_up_timer_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, EINA_FALSE);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	if (ad->is_pressing == FALSE) {
		ad->updown_timer = NULL;
		return EINA_FALSE;
	}

	sound_type_e sound_type = 0;
	int volume = 0;
	sound_manager_get_current_sound_type(&sound_type);
	if (sound_type == SOUND_TYPE_MEDIA) {
		int ret =  sound_manager_get_volume(sound_type, &volume);
		SETTING_TRACE("ret:%d", ret);
		if (volume < 15) {
			sound_manager_set_volume(sound_type, ++volume);
			if (ad->data_media_volume
					&& ad->data_media_volume->eo_check) {
				ad->data_media_volume->chk_status = volume;
				elm_slider_value_set(
						ad->data_media_volume->eo_check,
						volume);
			}
		}
	} else {
		int ret =  sound_manager_get_volume(SOUND_TYPE_RINGTONE,
				&volume);
		SETTING_TRACE("ret:%d", ret);
		if (volume < 15) {
			sound_manager_set_volume(SOUND_TYPE_RINGTONE, ++volume);
			if (ad->data_call_volume
					&& ad->data_call_volume->eo_check) {
				ad->data_call_volume->chk_status = volume;
				elm_slider_value_set(
						ad->data_call_volume->eo_check,
						volume);
				SETTING_TRACE("ad->data_call_volume->chk_status : %d",
						ad->data_call_volume->chk_status);
			}
		}
	}

	return EINA_TRUE;
}

static Eina_Bool __volume_down_timer_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, EINA_FALSE);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	if (ad->is_pressing == FALSE) {
		ad->updown_timer = NULL;
		return EINA_FALSE;
	}

	sound_type_e sound_type = 0;
	int volume = 0;
	sound_manager_get_current_sound_type(&sound_type);
	if (sound_type == SOUND_TYPE_MEDIA) {
		int ret =  sound_manager_get_volume(sound_type, &volume);
		SETTING_TRACE("ret:%d", ret);
		if (volume > 0) {
			sound_manager_set_volume(sound_type, --volume);
			if (ad->data_media_volume
					&& ad->data_media_volume->eo_check) {
				ad->data_media_volume->chk_status = volume;
				elm_slider_value_set(
						ad->data_media_volume->eo_check,
						volume);
			}
		}
	} else {
		int ret =  sound_manager_get_volume(SOUND_TYPE_RINGTONE,
				&volume);
		SETTING_TRACE("ret:%d", ret);
		if (volume > 0) {
			sound_manager_set_volume(SOUND_TYPE_RINGTONE,
					--volume);
			if (volume == 0) {
				/* change to Vibration mode */
				vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
						FALSE);
				vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
						TRUE);
				PLAY_FEEDBACK(FEEDBACK_TYPE_VIBRATION,
						FEEDBACK_PATTERN_VIBRATION_ON);
			}
			if (ad->data_call_volume
					&& ad->data_call_volume->eo_check) {
				ad->data_call_volume->chk_status = volume;
				elm_slider_value_set(
						ad->data_call_volume->eo_check,
						volume);
				SETTING_TRACE("ad->data_call_volume->chk_status : %d",
						ad->data_call_volume->chk_status);
				setting_sound_update_slider_icon(
						ad->data_call_volume,
						SND_SLIDER_CALL);
			}
		}
	}

	return EINA_TRUE;
}

static Eina_Bool __volume_key_up_cb(void *data, int type, void *event)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !event, EINA_TRUE);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	Ecore_Event_Key *ev = event;
	char *key = (char *)ev->key;
	if (!key)
		return EINA_TRUE;

	if (!safeStrCmp(key, "XF86AudioRaiseVolume")
			|| !safeStrCmp(key, "XF86AudioLowerVolume")) {
		ad->is_pressing = FALSE;
		if (ad->updown_timer) {
			ecore_timer_del(ad->updown_timer);
			ad->updown_timer = NULL;
		}
	}
	return EINA_TRUE;
}

static Eina_Bool __volume_key_down_cb(void *data, int type, void *event)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !event, EINA_TRUE);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	Ecore_Event_Key *ev = event;
	char *key = (char *)ev->key;
	if (!key)
		return EINA_TRUE;

	/* get current sound type */
	sound_type_e sound_type;
	int volume = 0;

	if (!safeStrCmp(key, "XF86AudioRaiseVolume")) {
		SETTING_TRACE("Press KEY_VOLUMEUP");
		/*
		 * S5 concept:
		 * On the volume popup,
		 *	1) if user controls the slider, don't play any sounds
		 *	just change volume.
		 *	2) if user controls the HW volume key for ringtone
		 *	sound, the beep sound is supported

		 * On Setting > Sound
		 *	1) if user controls the slider, change volume &
		 *	play sound
		 *	2) if user controls the HW volume key, do nothing
		 *	(DO NOT change volume,DO NOT play sound)
		*/
		if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
			return EINA_TRUE;

		/*if (ad->view_type != SETTING_PROF_VIEW_VOL)
		 * return EINA_TRUE; */

		ad->is_pressing = TRUE;

		sound_manager_get_current_sound_type(&sound_type);
		if (sound_type == SOUND_TYPE_MEDIA) {
			SETTING_TRACE("sound_type == SOUND_TYPE_MEDIA");
			int ret =  sound_manager_get_volume(sound_type,
					&volume);
			SETTING_TRACE("ret:%d", ret);
			if (volume < 15) {
				++volume;
				int ret = sound_manager_set_volume(sound_type,
						volume);

				if (/*ret == SOUND_MANAGER_ERROR_NO_OPERATION ||*/
						ret == SOUND_MANAGER_ERROR_INTERNAL) {
					SETTING_TRACE_DEBUG("didn't set volume");
					if (volume > 10) {
						ad->data_media_volume->chk_status = 10;
						elm_slider_value_set(
								ad->data_media_volume->eo_check,
								10);
						return EINA_TRUE;
					}
				}

				if (ad->data_media_volume
						&& ad->data_media_volume->eo_check) {
					ad->data_media_volume->chk_status =
							volume;
					elm_slider_value_set(
							ad->data_media_volume->eo_check,
							volume);
				}
			}
		} else {
			SETTING_TRACE("sound_type == SOUND_TYPE_RINGTONE");
			sound_manager_get_volume(SOUND_TYPE_RINGTONE, &volume);
			/*ad->backup_ringtong_volume = volume; */
			/*vconf_set_int(VCONFKEY_SETAPPL_RINGTONE_BACKUP_VOLUME,
			 *  ad->backup_ringtong_volume); */
			/*SETTING_TRACE("ret:%d, volume:%d,
			 * ad->backup_ringtong_volume:%d", ret, volume,
			 * ad->backup_ringtong_volume); */
			if (volume < 15) {
				sound_manager_set_volume(SOUND_TYPE_RINGTONE,
						++volume);

				int sound = 0;
				vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
						&sound);
				if (volume > 0 && !sound) {
					vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
							TRUE);
					vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
							FALSE);
					ad->data_call_volume->chk_status =
							volume;
					setting_sound_update_slider_icon(
							ad->data_call_volume,
							SND_SLIDER_CALL);
				}
				if (ad->data_call_volume
						&& ad->data_call_volume->eo_check) {
					ad->data_call_volume->chk_status =
							volume;
					elm_slider_value_set(
							ad->data_call_volume->eo_check,
							volume);
					SETTING_TRACE("chk_status : %d",
							ad->data_call_volume->chk_status);
				}
			}

			SETTING_TRACE("ad->mp_ringtone:%p, volume:%d",
					ad->mp_ringtone, volume);
			if (ad->mp_ringtone == NULL) {
				setting_sound_close_other_mm_players_but_type(
						ad, SOUND_TYPE_RINGTONE);
				if (volume > 0) {
					setting_sound_play_sound_origin(
							&(ad->mp_ringtone),
							ad,
							NULL,
							_TZ_SYS_SHARE"/settings/Sys/Volume_control.ogg",
							volume,
							SOUND_TYPE_RINGTONE);
				}
			} else {
				player_state_e state;
				player_get_state(*(ad->mp_ringtone), &state);
				if (state == PLAYER_STATE_PAUSED) {
					SETTING_TRACE("state == PLAYER_STATE_PAUSED");
					player_start(*(ad->mp_ringtone));
				} else if (state == PLAYER_STATE_PLAYING) {
					SETTING_TRACE("state == PLAYER_STATE_PLAYING");
					player_set_mute(*(ad->mp_ringtone),
							FALSE);
				}
			}
		}
		ad->updown_timer = ecore_timer_add(0.5, __volume_up_timer_cb,
				ad);
	} else if (!safeStrCmp(key, "XF86AudioLowerVolume")) {
		SETTING_TRACE("Press KEY_VOLUMEDOWN");
		/*
		 * S5 concept:
		 * On the volume popup,
		 *	1) if user controls the slider, don't play any sounds
		 *	just change volume.
		 *	2) if user controls the HW volume key for ringtone
		 *	sound, the beep sound is supported

		 * On Setting > Sound
		 *	1) if user controls the slider, change volume & play
		 *	sound
		 *	2) if user controls the HW volume key, do nothing
		 *	(DO NOT change volume,DO NOT play sound)
		*/
		if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
			return EINA_TRUE;

		/*if (ad->view_type != SETTING_PROF_VIEW_VOL)
		 * return EINA_TRUE; */
		ad->is_pressing = TRUE;
		sound_manager_get_current_sound_type(&sound_type);
		if (sound_type == SOUND_TYPE_MEDIA) {
			SETTING_TRACE("sound_type == SOUND_TYPE_MEDIA");
			int ret =  sound_manager_get_volume(sound_type,
					&volume);
			SETTING_TRACE("ret:%d", ret);
			if (volume > 0) {
				sound_manager_set_volume(sound_type, --volume);
				if (ad->data_media_volume
						&& ad->data_media_volume->eo_check) {
					ad->data_media_volume->chk_status =
							volume;
					elm_slider_value_set(
							ad->data_media_volume->eo_check,
							volume);
				}
			}
		} else {

			SETTING_TRACE("sound_type == SOUND_TYPE_RINGTONE");
			int sound = 0;
			vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
					&sound);
			if (!sound) return EINA_TRUE;
			int ret =  sound_manager_get_volume(SOUND_TYPE_RINGTONE,
					&volume);
			SETTING_TRACE("ret:%d, volume:%d", ret, volume);
			if (volume > 0) {
				sound_manager_set_volume(SOUND_TYPE_RINGTONE,
						--volume);
				if (volume == 0) {
					/* change to Vibration mode */
					vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
							FALSE);
					vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
							TRUE);

					PLAY_FEEDBACK(FEEDBACK_TYPE_VIBRATION,
							FEEDBACK_PATTERN_VIBRATION_ON);

					ad->data_call_volume->chk_status =
							volume;
					setting_sound_update_slider_icon(
							ad->data_call_volume,
							SND_SLIDER_CALL);
				}
				if (ad->data_call_volume
						&& ad->data_call_volume->eo_check) {
					ad->data_call_volume->chk_status =
							volume;
					elm_slider_value_set(
							ad->data_call_volume->eo_check,
							volume);
					SETTING_TRACE("ad->data_call_volume->chk_status : %d",
							ad->data_call_volume->chk_status);
				}
			}
			if (ad->mp_ringtone == NULL) {
				setting_sound_close_other_mm_players_but_type(
						ad, SOUND_TYPE_RINGTONE);
				if (volume > 0) {
					setting_sound_play_sound_origin(
							&(ad->mp_ringtone),
							ad,
							NULL,
							_TZ_SYS_SHARE"/settings/Sys/Volume_control.ogg",
							volume,
							SOUND_TYPE_RINGTONE);
				}
			} else {
				player_state_e state;
				player_get_state(*(ad->mp_ringtone), &state);
				SETTING_TRACE("ringtone player status : %d",
						state);
				if (state == PLAYER_STATE_PAUSED)
					player_start(*(ad->mp_ringtone));
			}
		}
		ad->updown_timer = ecore_timer_add(0.5, __volume_down_timer_cb,
				ad);
	} else if (!safeStrCmp(key, "XF86Menu")) {
		SETTING_TRACE("KEY_MENU CLICK.");
	} else if (!safeStrCmp(key, "XF86Home")) { /*XF86Phone*/
		SETTING_TRACE("KEY_HOME is pressed");
		if (!safeStrCmp(ad->viewtype, "org.tizen.volume"))
			ug_destroy_me(ad->ug);
	} else if (!safeStrCmp(key, "XF86Back")) { /*Back hard key*/
		SETTING_TRACE(" Back is pressed");
		/*SETTING_TRACE(" ad->viewtype %s", ad->viewtype);*/
		/*if (!safeStrCmp(ad->viewtype, "org.tizen.volume"))*/
		if (!safeStrCmp(ad->viewtype, "org.tizen.setting.volume")) {
			/*ug_destroy_me(ad->ug);*/
			elm_exit();
		}
	}

	return EINA_TRUE;
}

void __setting_sound_ug_key_grab(SettingProfileUG *ad)
{
	SETTING_TRACE_BEGIN;
#ifdef ECORE_X
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;
#endif

	if (ad == NULL)
		return;

	/* To prevent conflict with volume app */
	vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 2);

#ifdef ECORE_X
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get(ad->win_get);

	ret = eext_win_keygrab_set(xwin, "XF86AudioRaiseVolume");
	if (ret)
		SETTING_TRACE_DEBUG("KEY_VOLUMEUP grab failed");

	ret = eext_win_keygrab_set(xwin, "XF86AudioLowerVolume");
	if (ret)
		SETTING_TRACE_DEBUG("KEY_VOLUMEDOWN grab failed");
#endif
	if (ad->keydown_handler) {
		ecore_event_handler_del(ad->keydown_handler);
		ad->keydown_handler = NULL;
	}
	if (ad->keyup_handler) {
		ecore_event_handler_del(ad->keyup_handler);
		ad->keyup_handler = NULL;
	}
	ad->keydown_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
			__volume_key_down_cb, ad);
	if (!ad->keydown_handler)
		SETTING_TRACE_DEBUG("ecore_event_handler_add() failed");

	ad->keyup_handler = ecore_event_handler_add(ECORE_EVENT_KEY_UP,
			__volume_key_up_cb, ad);
	if (!ad->keyup_handler)
		SETTING_TRACE_DEBUG("ecore_event_handler_add() failed");

	SETTING_TRACE_END;
}

void __setting_sound_ug_key_ungrab(SettingProfileUG *ad)
{
	SETTING_TRACE_BEGIN;
#ifdef ECORE_X
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;
#endif

	if (ad == NULL)
		return;

	/* To prevent conflict with volume app */
	vconf_set_int(VCONFKEY_STARTER_USE_VOLUME_KEY, 0);

#ifdef ECORE_X
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get(ad->win_get);

	ret = eext_win_keygrab_unset(xwin, "XF86AudioRaiseVolume");
	if (ret)
		SETTING_TRACE_DEBUG("KEY_VOLUMEUP ungrab failed");

	ret = eext_win_keygrab_unset(xwin, "XF86AudioLowerVolume");
	if (ret)
		SETTING_TRACE_DEBUG("KEY_VOLUMEDOWN ungrab failed");
#endif

	if (ad->updown_timer) {
		ecore_timer_del(ad->updown_timer);
		ad->updown_timer = NULL;
	}
	if (ad->keydown_handler) {
		ecore_event_handler_del(ad->keydown_handler);
		ad->keydown_handler = NULL;
	}
	if (ad->keyup_handler) {
		ecore_event_handler_del(ad->keyup_handler);
		ad->keyup_handler = NULL;
	}
}

Evas_Object *setting_sound_init(void *data)
{
	SETTING_TRACE_BEGIN;
	/*appcore_measure_start(); */
	retv_if(data == NULL, NULL);
	SettingProfileUG *ad = data;

	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);

	setting_create_Gendial_itc(SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE,
			&(ad->itc_layout));

	ad->itc_layout.func.content_get = __sound_slider_icon_get;

	setting_create_Gendial_itc(SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE,
			&(ad->itc_layout_1icon));

	ad->itc_layout_1icon.func.content_get = __sound_slider_icon_get;

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_sound_main, NULL);

	if (!safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		elm_win_wm_rotation_preferred_rotation_set(ad->win_get, -1);
	else
		elm_win_wm_rotation_preferred_rotation_set(ad->win_get, 0);

	/* creating a view. */
	setting_view_node_set_cur_view(&setting_view_sound_main);
	setting_view_create(&setting_view_sound_main, (void *)ad);

	setting_sound_listen_vconf_change(ad);

	return ad->ly_main;
}

void setting_sound_deinit(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingProfileUG *ad = data;
	elm_win_wm_rotation_preferred_rotation_set(ad->win_get, -1);

	setting_sound_unlisten_vconf_change(ad);
	setting_sound_close_all_mm_players(ad);

	if (ad->calling_popup) {
		evas_object_del(ad->calling_popup);
		ad->calling_popup = NULL;
	}

	if (ad->updown_timer) {
		ecore_timer_del(ad->updown_timer);
		ad->updown_timer = NULL;
	}
	if (ad->keydown_handler) {
		ecore_event_handler_del(ad->keydown_handler);
		ad->keydown_handler = NULL;
	}
	if (ad->keyup_handler) {
		ecore_event_handler_del(ad->keyup_handler);
		ad->keyup_handler = NULL;
	}
	FREE(ad->viewtype);

	/* it is the view manage strategy to invoke view's destory operation */
	setting_view_destroy(&setting_view_sound_main, ad);

	SETTING_TRACE_END;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
static void __calling_popup_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL);
	SettingProfileUG *ad = (SettingProfileUG *) data;

	if (ad->calling_popup) {
		evas_object_del(ad->calling_popup);
		ad->calling_popup = NULL;
	}
}

/* Show guide popup, when drag or tap slider if during call.
	Do not provide pre-listening. */
void setting_sound_create_warning_popup_during_call(void *data)
{
	ret_if(data == NULL);
	SettingProfileUG *ad = (SettingProfileUG *) data;

	cm_call_status_e call_status = CM_CALL_STATUS_IDLE;
	cm_client_h cm_handle = NULL;
	cm_init(&cm_handle);
	cm_get_call_status(cm_handle, &call_status);
	cm_deinit(cm_handle);
	if (CM_CALL_STATUS_IDLE != call_status) {
		SETTING_TRACE("Call status is not OFF (%d)", call_status);
		if (!ad->calling_popup) {
			ad->calling_popup = setting_create_popup(
					ad,
					ad->win_get,
					"IDS_ST_HEADER_UNABLE_TO_PLAY_SAMPLE_ABB",
					"IDS_ST_POP_SAMPLES_CANNOT_BE_PLAYED_DURING_CALLS",
					__calling_popup_cb,
					0,
					false,
					false,
					1,
					"IDS_ST_BUTTON_OK");
		}
	}
}

#define EXP 0.001

static void __mm_player_msg_cb(void *user_param)
{
	ret_if(user_param == NULL);
	SettingProfileUG *ad = (SettingProfileUG *)user_param;

	SETTING_TRACE("completed_cb is called.");
	setting_sound_close_all_mm_players(ad);
}

static void __mm_player_interrupted_cb(player_interrupted_code_e code,
		void *user_data)
{
	ret_if(user_data == NULL);
	SettingProfileUG *ad = (SettingProfileUG *)user_data;

	SETTING_TRACE("interrupted code : %d", code);

	if (ad->mp_ringtone) {
		player_state_e state;
		player_get_state(*(ad->mp_ringtone), &state);
		SETTING_TRACE("ringtone player state : %d", state);
	} else if (ad->mp_noti) {
		player_state_e state;
		player_get_state(*(ad->mp_noti), &state);
		SETTING_TRACE("noti player state : %d", state);
	} else if (ad->mp_media) {
		player_state_e state;
		player_get_state(*(ad->mp_media), &state);
		SETTING_TRACE("media player state : %d", state);
	}
}

static Eina_Bool __play_timer_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, EINA_FALSE);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	if (ad->mp_prepare_async) {
		SETTING_TRACE("player (%x)", ad->mp_prepare_async);
		int err;
		player_state_e state = -1;
		err = player_get_state(*(ad->mp_prepare_async), &state);
		SETTING_TRACE("state:%d", state);

		if (state != PLAYER_STATE_READY) {
			SETTING_TRACE_ERROR("state of player is invalid %d",
					err);
			player_unprepare(*(ad->mp_prepare_async));
			player_destroy(*(ad->mp_prepare_async));
			if (ad->mp_prepare_async == ad->mp_ringtone) {
				free(ad->mp_ringtone);
				ad->mp_ringtone = NULL;
			} else if (ad->mp_prepare_async == ad->mp_noti) {
				free(ad->mp_noti);
				ad->mp_noti = NULL;
			} else if (ad->mp_prepare_async == ad->mp_media) {
				free(ad->mp_media);
				ad->mp_media = NULL;
			}
			ad->mp_prepare_async = NULL;
			return EINA_FALSE;
		}

		player_set_completed_cb(*(ad->mp_prepare_async),
				__mm_player_msg_cb, ad);
		player_set_interrupted_cb(*(ad->mp_prepare_async),
				__mm_player_interrupted_cb, ad);

		err = player_start(*(ad->mp_prepare_async));
		if (err != PLAYER_ERROR_NONE) {	/* if directly return error. */
			SETTING_TRACE_ERROR("player_start [%d]", err);
			player_unset_completed_cb(*(ad->mp_prepare_async));
			player_unset_interrupted_cb(*(ad->mp_prepare_async));
			player_unprepare(*(ad->mp_prepare_async));
			player_destroy(*(ad->mp_prepare_async));
			if (ad->mp_prepare_async == ad->mp_ringtone) {
				free(ad->mp_ringtone);
				ad->mp_ringtone = NULL;
			} else if (ad->mp_prepare_async == ad->mp_noti) {
				free(ad->mp_noti);
				ad->mp_noti = NULL;
			} else if (ad->mp_prepare_async == ad->mp_media) {
				free(ad->mp_media);
				ad->mp_media = NULL;
			}
			ad->mp_prepare_async = NULL;
		}
	}
	return EINA_FALSE;
}

/* Async cb : to improve slider's performance when creating player */
static void __mm_player_prepared_cb(void *user_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!user_data);
	SettingProfileUG *ad = (SettingProfileUG *)user_data;

	/* Do not call mm API in mm cb : Add timer to avoid it */
	/* ecore_timer_add(0.0, (Ecore_Task_Cb)__play_timer_cb, ad); */
	__play_timer_cb(ad);
}

player_h *setting_sound_play_sound(
		void *data,
		void *cb,
		char *ringtone_file,
		int vol,
		sound_type_e sound_type)
{
	/* error check */
	retv_if(data == NULL, NULL);
	SettingProfileUG *ad = (SettingProfileUG *) data;

	int is_existing = setting_sound_check_file_exist(ad, ringtone_file);
	if (is_existing == SETTING_RETURN_FAIL || NULL == ringtone_file) {
		switch (sound_type) {
		case SOUND_TYPE_RINGTONE:
			if (isEmulBin())
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE);
			break;
		case SOUND_TYPE_MEDIA:
			if (isEmulBin())
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_MEDIA_TONE);
			break;
		case SOUND_TYPE_NOTIFICATION:
			ad->song_playing = (char *)g_strdup(
					SETTING_DEFAULT_MSG_TONE);
			break;
		default:
			if (isEmulBin())
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				ad->song_playing = (char *)g_strdup(
						SETTING_DEFAULT_CALL_TONE);
			break;
		}

	} else {
		if (safeStrCmp(ringtone_file, "Silent") == 0)
			return NULL;

		ad->song_playing = (char *)g_strdup(ringtone_file);
	}

	player_h *player = calloc(1, sizeof(player_h));
	if (!player) {
		SETTING_TRACE_ERROR("failed to calloc player_h");
		return NULL;
	}

	int err = 0;

	err = player_create(player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("creating the player handle failed[%d]",
				err);
		free(player);
		return NULL;
	}

	err = player_set_sound_type(*player, sound_type);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("error to set sound_type[%d]", err);
		player_destroy(*player);
		free(player);
		return NULL;
	}

	SETTING_TRACE_DEBUG("player_set_sound_type [%d]", sound_type);

	err = player_set_uri(*player, ad->song_playing);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("error to set attribute---profile_uri[%d]",
				err);
		player_destroy(*player);
		free(player);
		return NULL;
	}

	if (sound_type != SOUND_TYPE_MEDIA) {
#if 0
		/*err = player_set_session_prelistening(*player); */
		/*player_prelistening_mode_e mode; */

		if (sound_type == SOUND_TYPE_NOTIFICATION)
			mode = PLAYER_PRELISTENING_MODE_NOTIFICATION;
		else
			mode = PLAYER_PRELISTENING_MODE_CALL;

		err = player_set_prelistening_mode(*player, mode);
		if (err != PLAYER_ERROR_NONE) {
			SETTING_TRACE_ERROR("error to player_set_session_prelistening[%d]",
					err);
			player_destroy(*player);
			free(player);
			return NULL;
		}
#endif
	} else {
#if 0
		err = player_set_safety_volume(*player);
		if (err != PLAYER_ERROR_NONE) {
			SETTING_TRACE_ERROR("error to player_set_volume[%d]",
					err);
			player_destroy(*player);
			free(player);
			return NULL;
		}
#endif
	}
#if SUPPORT_PLAYER_ASYNC
	err = player_prepare_async(*player, __mm_player_prepared_cb, ad);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("realizing the player handle failed[%d]",
				err);
		ad->mp_prepare_async = NULL;
		player_destroy(*player);
		free(player);
		return NULL;
	}
	ad->mp_prepare_async = player;
	SETTING_TRACE("waiting..player (%x)", ad->mp_prepare_async);
#else
	err = player_prepare(*player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("player_prepare() error %d", err);
		player_destroy(*player);
		free(player);
		return NULL;
	}

	player_state_e state = -1;
	err = player_get_state(*player, &state);
	SETTING_TRACE("state:%d", state);
	if (state != PLAYER_STATE_READY) {
		SETTING_TRACE_ERROR("state of player is invalid %d", err);
		player_unprepare(*player);
		player_destroy(*player);
		free(player);
		return NULL;

	}

	player_set_completed_cb(*player, __mm_player_msg_cb, ad);
	player_set_interrupted_cb(*player, __mm_player_interrupted_cb, ad);

	err = player_start(*player);
	if (err != PLAYER_ERROR_NONE) {
		/* if directly return error.. */
		SETTING_TRACE_ERROR("player_start [%d]", err);
		player_unset_completed_cb(*player);
		player_unset_interrupted_cb(*player);
		player_unprepare(*player);
		player_destroy(*player);
		free(player);
		return NULL;
	}
#endif

	return player;
}

void setting_sound_play_sound_origin(
		player_h **mp_handle,
		void *data,
		void *cb,
		char *ringtone_file,
		/* int vol, */
		float vol,
		sound_type_e sound_type)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	ret_if(data == NULL);

	if (vol < EXP) {
		SETTING_TRACE_DEBUG("vol < EXP");
		if (*mp_handle) {
			if (setting_sound_stop_sound(data, *mp_handle) !=
					SETTING_RETURN_SUCCESS) {
				/* error */
			} else {
				*mp_handle = NULL;
			}
		}
		return;
	}

	int vconf_value = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TURN_OFF_ALL_SOUNDS,
			&vconf_value);
	if (vconf_value == TRUE)
		return;

	SettingProfileUG *ad = (SettingProfileUG *) data;
	if (*mp_handle == NULL) {
		*mp_handle = setting_sound_play_sound(ad, cb, ringtone_file,
				vol, sound_type);
		SETTING_TRACE("mp_handle (%x)", *mp_handle);
	}
}

int setting_sound_stop_sound(void *data, player_h *player)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL || player == NULL,
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SETTING_TRACE("player (%x)", player);

	SettingProfileUG *ad = (SettingProfileUG *) data;
	if (ad->song_playing) {
		g_free(ad->song_playing);
		ad->song_playing = NULL;
	}

	player_state_e state = -1;
	int ret = SETTING_RETURN_SUCCESS;
	int prev_behavior = 0;	/*	instead of do_while(0) and break */

	player_get_state(*player, &state);
	if (state == PLAYER_STATE_PLAYING) {
		if (player_stop(*player) != PLAYER_ERROR_NONE) {
			SETTING_TRACE("mm player stop failed");
			ret = SETTING_MMPLAYER_STOP_ERR;
			/* instead of do_while(0) and break */
			prev_behavior++;
		}
	}

	player_unset_completed_cb(*player);
	player_unset_interrupted_cb(*player);

	if ((prev_behavior == 0) && (player_unprepare(*player) !=
			PLAYER_ERROR_NONE)) {
		SETTING_TRACE("mm player unrealize failed");
		ret = SETTING_MMPLAYER_UNREALIZE_ERR;
		/* instead of do_while(0) and break */
		prev_behavior++;
	}

	if ((prev_behavior == 0) && (player_destroy(*player) !=
			PLAYER_ERROR_NONE)) {
		SETTING_TRACE("mm player destroy failed");
		ret = SETTING_MMPLAYER_DESTROY_ERR;
	}

	FREE(player);
	return ret;
}

int __close_player(void *data, SoundType type)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) data;
	player_h *tmp = NULL;

	switch (type) {
	case SND_SLIDER_CALL:
		retv_if(ad->mp_ringtone == NULL,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
		SETTING_TRACE("ringtone player (%x)", ad->mp_ringtone);
		if (ad->mp_prepare_async == ad->mp_ringtone)
			ad->mp_prepare_async = NULL;

		/*player_unprepare take so much time, so if __mm_player_msg_cb
		 * was invokeing(it will trigger invoking
		 * setting_sound_stop_sound), */
		/*in this time user click Back key to exit(setting_sound_deinit
		 *  will trigger setting_sound_stop_sound() too) */
		/*if not set ad->mp_ringtone to NULL in advance,
		 * ad->mp_ringtone will be repeatly released */
		tmp = ad->mp_ringtone;
		ad->mp_ringtone = NULL;
		setting_sound_stop_sound(ad, tmp);
		break;
	case SND_SLIDER_NOTI:
		retv_if(ad->mp_noti == NULL,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
		SETTING_TRACE("noti player (%x)", ad->mp_noti);
		if (ad->mp_prepare_async == ad->mp_noti)
			ad->mp_prepare_async = NULL;

		tmp = ad->mp_noti;
		ad->mp_noti = NULL;
		setting_sound_stop_sound(ad, tmp);
		break;
	case SND_SLIDER_MEDIA:
		retv_if(ad->mp_media == NULL,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
		SETTING_TRACE("media player (%x)", ad->mp_media);
		if (ad->mp_prepare_async == ad->mp_media)
			ad->mp_prepare_async = NULL;

		tmp = ad->mp_media;
		ad->mp_media = NULL;
		setting_sound_stop_sound(ad, tmp);
		break;
	case SND_SLIDER_SYSTEM:
	default:
		return SETTING_RETURN_FAIL;
	}

	return SETTING_RETURN_SUCCESS;
}

int
setting_sound_close_other_mm_players_but_type(void *data, sound_type_e type)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) data;

	int ret = 0;

	switch (type) {
	/* close noti, media and system players */
	case SOUND_TYPE_RINGTONE:
		if (__close_player(ad, SND_SLIDER_NOTI) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		if (__close_player(ad, SND_SLIDER_MEDIA) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		break;
	/* close ringtone, media and system players */
	case SOUND_TYPE_NOTIFICATION:
		if (__close_player(ad, SND_SLIDER_CALL) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		if (__close_player(ad, SND_SLIDER_MEDIA) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		break;
	/* close ringtone, noti and system players */
	case SOUND_TYPE_MEDIA:
		if (__close_player(ad, SND_SLIDER_CALL) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		if (__close_player(ad, SND_SLIDER_NOTI) !=
				SETTING_RETURN_SUCCESS)
			ret++;
		break;
	/* if unknown type, dont close any player */
	default:
		break;
	}

	SETTING_TRACE_DEBUG("ret: %d", ret);

	if (ret == 0)
		return SETTING_RETURN_SUCCESS;
	else
		return SETTING_RETURN_FAIL;
}


int setting_sound_close_all_mm_players(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) data;

	int ret = 0;

	if (__close_player(ad, SND_SLIDER_CALL) != SETTING_RETURN_SUCCESS)
		ret++;
	if (__close_player(ad, SND_SLIDER_NOTI) != SETTING_RETURN_SUCCESS)
		ret++;
	if (__close_player(ad, SND_SLIDER_MEDIA) != SETTING_RETURN_SUCCESS)
		ret++;

	if (ret == 0)
		return SETTING_RETURN_SUCCESS;
	else
		return SETTING_RETURN_FAIL;
}
