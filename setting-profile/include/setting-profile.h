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
#ifndef __SETTING_PROFILE_H__
#define __SETTING_PROFILE_H__

#include <player.h>
#include <sound_manager.h>
#include <setting-common-general-func.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <efl_extension.h>
#include <feedback.h>
#include <call-manager.h>
#include <system_settings.h>

#define VOLUME_APP_NAME "org.tizen.setting.volume"
#define MAX_SAFETY_VOLUME_LEVEL 10
#define SETTING_PROFILE_SOUND_ALERTS_PATH \
	_TZ_SYS_SHARE"/settings/Alerts"
#define SETTING_PROFILE_SOUND_RINGTONES_PATH \
	_TZ_SYS_SHARE"/settings/Ringtones"
#define SETTING_PROFILE_MMC_PATH \
	_TZ_SYS_MEDIA"/sdcard"
#define SETTING_DEFAULT_MSG_TONE \
	_TZ_SYS_SHARE"/settings/Alerts/Whistle.ogg"
#define SETTING_DEFAULT_MEDIA_TONE \
	_TZ_SYS_SHARE"/settings/Ringtones/ringtone_sdk.mp3"
#define SUPPORT_PLAYER_ASYNC 1

/*Call ringtone:3 icon status */
#define ICON_CALL_VIB \
	SETTING_ICON_PATH_CFG"B04_slider_icon_vibrate.png"
#define ICON_CALL_NORMAL \
	SETTING_ICON_PATH_CFG"B04_slider_icon_ring.png"
#define ICON_CALL_MUTE \
	SETTING_ICON_PATH_CFG"B04_volume_icon_mute.png"

/*Notification: 2 icon status */
#define ICON_NOTI_MUTE \
	SETTING_ICON_PATH_CFG"B04_slider_icon_notification_mute.png"
#define ICON_NOTI_NORMAL \
	SETTING_ICON_PATH_CFG"B04_slider_icon_notification.png"

/*Media: 2 icon status */
#define ICON_MEDIA_MUTE		SETTING_ICON_PATH_CFG"B04_volume_icon_mute.png"
#define ICON_MEDIA_NORMAL	SETTING_ICON_PATH_CFG"00_volume_icon.png"

/*System:2 icon status */
#define ICON_SYSTEM_MUTE \
	SETTING_ICON_PATH_CFG"B04_slider_icon_system_mute.png"
#define ICON_SYSTEM_NORMAL \
	SETTING_ICON_PATH_CFG"B04_slider_icon_system.png"

typedef enum {
	SND_SLIDER_CALL = 0,
	SND_SLIDER_NOTI,
	SND_SLIDER_MEDIA,
	SND_SLIDER_SYSTEM,
	SND_SLIDER_MAX,
} SoundType;

typedef struct _SettingProfileUG SettingProfileUG;

/**
 * Setting Profile UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingProfileUG and the functions access app context.
 */
struct _SettingProfileUG {
	ui_gadget_h ug;
	ui_gadget_h ug_loading;
	char *viewtype;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *navi_bar;
	Evas_Object *ly_main;
	Evas_Object *gl_lite_main;

	Elm_Genlist_Item_Class itc_layout;
	Elm_Genlist_Item_Class itc_layout_1icon;

	/* Main view */
	Setting_GenGroupItem_Data *data_call_alert_tone;
	Setting_GenGroupItem_Data *data_call_volume;
	Setting_GenGroupItem_Data *data_msg_alert_tone;
	Setting_GenGroupItem_Data *data_noti_volume;
	Setting_GenGroupItem_Data *data_touch_volume;
	Setting_GenGroupItem_Data *data_media_volume;
	Setting_GenGroupItem_Data *data_sound_when_ring;
	Setting_GenGroupItem_Data *data_do_not_disturb;

	/* Feedback */
	Setting_GenGroupItem_Data *data_lock_sound;
	Setting_GenGroupItem_Data *data_keyboard_sound;
	Setting_GenGroupItem_Data *data_button_sounds;
	Setting_GenGroupItem_Data *data_touch_sounds;

	player_h *mp_ringtone;
	player_h *mp_noti;
	player_h *mp_media;
	player_h *mp_prepare_async;

	bool sound_on;
	char *song_playing;
	char *ringtone_type;
	char *sdcard_ringtone_path;

	/* for performance of incoming call ringtone slider */
	Ecore_Idler *play_ringtone_idler;

	Ecore_Event_Handler *keydown_handler;
	Ecore_Event_Handler *keyup_handler;bool is_pressing;
	Ecore_Timer *updown_timer;
	Evas_Object *calling_popup; /* Unable to pre-listening during a call */
	int backup_ringtong_volume;bool is_ringtone_slidering;
};

extern setting_view setting_view_sound_main;

extern void setting_sound_play_sound_origin(player_h **mp_handle, void *data,
		void *cb, char *ringtone_file, float vol,
		sound_type_e sound_type);
extern int setting_sound_check_file_exist(void *data, const char *file_path);

extern void system_settings_changed_silent_mode(system_settings_key_e key,
		void *user_data);

extern player_h *setting_sound_play_sound(void *data, void *cb,
		char *ringtone_file, int vol, sound_type_e sound_type);

extern int __close_player(void *data, SoundType type);

extern int setting_sound_stop_sound(void *data, player_h *player);
extern int setting_sound_close_other_mm_players_but_type(void *data,
		sound_type_e type);
extern int setting_sound_close_all_mm_players(void *data);
extern void __setting_sound_ug_key_grab(SettingProfileUG *ad);
extern void __setting_sound_ug_key_ungrab(SettingProfileUG *ad);
extern char *setting_media_basename(char *path);
extern void setting_sound_create_warning_popup_during_call(void *data);
extern char *setting_sound_get_slider_icon(int type, int volume);
extern void setting_sound_update_slider_icon(
		Setting_GenGroupItem_Data *item_data, int type);
extern Evas_Object *setting_sound_init(void *data);
extern void setting_sound_deinit(void *data);
extern char *setting_do_not_disturb_is_enable(void *data);
extern void setting_sound_update_do_not_disturb_item(void *data);

#endif
