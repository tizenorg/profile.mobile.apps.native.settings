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

#include <feedback.h>
#include <mm_sound_private.h>
#include <app_manager.h>
#include <setting-profile-sound-main.h>
#include <setting-profile.h>
#include <setting-common-general-func.h>

#include <bundle_internal.h>
#include <notification_setting.h>
#include <notification_setting_internal.h>

#define DO_NOT_DISTURB_OP "http://tizen.org/appcontrol/operation/setting/"\
	"do_not_disturb"
#define APP_NOTIFICATIONS_OP "http://tizen.org/appcontrol/operation/setting/"\
	"app_notifications"

#define SETTING_SOUND_VOL_MAX 15
#define SETTING_DEFAULT_RINGTONE_VOL_INT	11
#define SETTING_DEFAULT_NOTI_VOL_INT		11
#define SETTING_DEFAULT_MEDIA_VOL_INT		9
#define SETTING_DEFAULT_SYSTEM_VOL_INT		9

static int setting_sound_main_create(void *cb);
static int setting_sound_main_destroy(void *cb);
static int setting_sound_main_update(void *cb);
static int setting_sound_main_cleanup(void *cb);

setting_view setting_view_sound_main = {
	.create = setting_sound_main_create,
	.destroy = setting_sound_main_destroy,
	.update = setting_sound_main_update,
	.cleanup = setting_sound_main_cleanup,
};

/* should be called in function to return int */
#define ADD_GENLIST(genlist, parent) \
	{\
		genlist = elm_genlist_add(parent);\
		retvm_if(genlist == NULL, SETTING_RETURN_FAIL, \
				"Cannot set genlist object as content of layout");\
		elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);\
		elm_object_style_set(genlist, "dialogue");\
		elm_genlist_clear(genlist);\
		evas_object_smart_callback_add(genlist, "realized", \
				__gl_realized_cb, NULL);\
		elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, \
				ELM_SCROLLER_POLICY_AUTO);\
	}


/* should be called in function to return void */
#define ADD_GENLIST2(genlist, parent) \
	{\
		genlist = elm_genlist_add(parent);\
		retm_if(genlist == NULL, \
				"Cannot set genlist object as content of layout");\
		elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);\
		elm_object_style_set(genlist, "dialogue");\
		elm_genlist_clear(genlist);\
		evas_object_smart_callback_add(genlist, "realized", \
				__gl_realized_cb, NULL);\
		elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, \
				ELM_SCROLLER_POLICY_AUTO);\
	}

/* keystr, toggle */
#define ADD_TOGGLE_MENU2(item, genlist, item_style, keystr, value, data) \
	{\
		item = setting_create_Gendial_field_def( \
				genlist, \
				&(item_style), \
				setting_sound_main_mouse_up_Gendial_list_cb, \
				data, \
				SWALLOW_Type_1ICON_1RADIO, \
				NULL, \
				NULL, \
				value, \
				keystr, \
				NULL, \
				__sound_chk_cb); \
		if (item) { \
			item->userdata = data; \
		} else { \
			SETTING_TRACE_ERROR("%s item is NULL", keystr); \
		} \
	}


/* keystr, multiline, toggle */
/* should be called in function to return void */
#define ADD_TOGGLE_MENU( \
		_item, genlist, item_style, keystr, substr, value, data) \
	{\
		_item = setting_create_Gendial_field_def( \
				genlist, \
				&(item_style), \
				setting_sound_main_mouse_up_Gendial_list_cb, \
				data, \
				SWALLOW_Type_1ICON_1RADIO, \
				NULL, \
				NULL, \
				value, \
				keystr, \
				NULL, \
				__sound_chk_cb); \
		if (_item) { \
			_item->userdata = data;\
		} else { \
			SETTING_TRACE_ERROR("%s item is NULL", keystr);\
		} \
		if (substr) {\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def( \
					genlist, \
					&itc_multiline_text,\
					NULL,\
					NULL,\
					SWALLOW_Type_LAYOUT_SPECIALIZTION_X,\
					NULL, \
					NULL, \
					0, \
					substr, \
					NULL, \
					NULL);\
			setting_retm_if(NULL == item_data, \
					"item_data is NULL");\
			elm_genlist_item_select_mode_set( \
					item_data->item, \
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		} \
	}

#define ADD_SOUND_SLIDER(item_data, genlist, item_style, l_img, r_img, value, \
		keystr, data, sld_cb, sld_max, start_cb, stop_cb) \
	{ \
		item_data = setting_create_Gendial_field_def( \
				genlist, \
				&(item_style), \
				NULL,\
				NULL, \
				SWALLOW_Type_LAYOUT_SLIDER,\
				l_img, \
				r_img,\
				value,\
				keystr,\
				NULL,\
				sld_cb);\
		if (item_data) {\
			item_data->win_main = data->md.view_layout;\
			item_data->evas = data->md.evas;\
			item_data->isIndicatorVisible = FALSE;\
			item_data->slider_min = 0;\
			item_data->slider_max = sld_max;\
			item_data->userdata = data;\
			item_data->start_change_cb = start_cb;\
			item_data->stop_change_cb = stop_cb;\
			elm_genlist_item_select_mode_set( \
					item_data->item, \
					ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		} else {\
			SETTING_TRACE_ERROR("%s item is NULL", keystr);\
		} \
	}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static Eina_Bool __play_ringtone_sound_idle_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, EINA_FALSE);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;

	if (list_item->eo_check) {
		double current;
		current = elm_slider_value_get(list_item->eo_check);
		list_item->chk_status = (int)(current + 0.5);
	}

	/*
	 * S5 concept:
	 * On the volume popup,
	 *	1) if user controls the slider, don't play any sounds,
	 *	just change volume.
	 *	2) if user controls the HW volume key for ringtone sound,
	 *	the beep sound is supported

	 * On Setting > Sound
	 *	1) if user controls the slider, change volume & play sound
	 *	2) if user controls the HW volume key, do nothing
	 *	(DO NOT change volume,DO NOT play sound)
	*/
	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		return EINA_FALSE;

	if (ad->mp_ringtone == NULL) {
		setting_sound_close_other_mm_players_but_type(
				ad, SOUND_TYPE_RINGTONE);
		char	*pa_tone_path = NULL;
		pa_tone_path = vconf_get_str(
				VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		if (pa_tone_path == NULL) {
			if (isEmulBin())
				pa_tone_path = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				pa_tone_path = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE);

			if (vconf_set_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
					pa_tone_path) < 0) {
				FREE(pa_tone_path);
				return EINA_FALSE;
			}
		}
		if (list_item->chk_status > 0) {
			setting_sound_play_sound_origin(
					&(ad->mp_ringtone),
					ad,
					NULL,
					pa_tone_path,
					(float)list_item->chk_status,
					SOUND_TYPE_RINGTONE);
		}
	} else {
		player_state_e state;
		player_get_state(*(ad->mp_ringtone), &state);
		SETTING_TRACE("ringtone player status : %d", state);
		if (state == PLAYER_STATE_PAUSED) {
			player_start(*(ad->mp_ringtone));
		} else if (state == PLAYER_STATE_PLAYING) {
			/*bool is_muted = 0; */
			/*player_is_muted(*(ad->mp_ringtone), &is_muted); */
			/*SETTING_TRACE("ringtone player is muted : %d",
			 * is_muted); */
			if (/*is_muted &&*/ ad->sound_on)
				player_set_mute(*(ad->mp_ringtone), 0);
		}
	}

	ad->play_ringtone_idler = NULL;
	return EINA_FALSE;
}


void __start_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	ret_if(ad == NULL);
	setting_sound_create_warning_popup_during_call(ad);
	if (list_item == ad->data_call_volume) {
		ad->is_ringtone_slidering = TRUE;
		setting_sound_close_other_mm_players_but_type(
				ad, SOUND_TYPE_RINGTONE);
	} else if (list_item == ad->data_noti_volume) {
		setting_sound_close_other_mm_players_but_type(
				ad, SOUND_TYPE_NOTIFICATION);
	} else if (list_item == ad->data_media_volume) {
		setting_sound_close_other_mm_players_but_type(
				ad, SOUND_TYPE_MEDIA);
	} else if (list_item == ad->data_touch_volume) {
		setting_sound_close_all_mm_players(ad);
	}
}

static void __call_slider_change_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	ret_if(data == NULL);

	double val = elm_slider_value_get(obj);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;

	ad->is_ringtone_slidering = TRUE;

	if (list_item->chk_status != (int)(val + 0.5)) {
		list_item->chk_status = (int)(val + 0.5);
		sound_manager_set_volume(
				SOUND_TYPE_RINGTONE, list_item->chk_status);
		SETTING_TRACE_DEBUG(
				"ringtone volume is %d, %f",
				list_item->chk_status,
				val);

#if FUNCTION_SYSTEM_SETTING
		if (list_item->chk_status == 0) {
			/* change to Vibration mode */
			/* call system_setting */
			/*system_settings_set_value_bool(
			 * SYSTEM_SETTINGS_KEY_SOUND_SILENT_MODE, false); */
			vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
					FALSE);
			vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
					TRUE);

		} else {
			if (!ad->sound_on) {
				/* change to Sound mode */
				/* call system_setting */
				/*system_settings_set_value_bool(
				 * SYSTEM_SETTINGS_KEY_SOUND_SILENT_MODE,
				 * false); */
				vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
						TRUE);
				vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
						FALSE);
			}
		}
#else
		if (list_item->chk_status == 0) {
			/* change to Vibration mode */
			vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
					FALSE);
			vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
					TRUE);
		} else {
			if (!ad->sound_on) {
				vconf_set_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
						TRUE);
				vconf_set_bool(VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL,
						FALSE);
			}
		}
#endif

		setting_sound_update_slider_icon(list_item, SND_SLIDER_CALL);

		/*elm_genlist_item_update(list_item->item); */
		elm_slider_value_set(obj, list_item->chk_status);
		SETTING_TRACE_DEBUG("[TIME] change sound mode : %d msec ",
				appcore_measure_time());
	}

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	if (ad->play_ringtone_idler) {
		ecore_idler_del(ad->play_ringtone_idler);
		ad->play_ringtone_idler = NULL;
	}
	ad->play_ringtone_idler = ecore_idler_add(
			__play_ringtone_sound_idle_cb, list_item);

	/*SWIP_EFFECT_ENABLE(ad->md.ly_main, ad->ctrl_bar); */
	SETTING_TRACE("list_item->chk_status:%d", list_item->chk_status);
}

static void __noti_slider_change_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	ret_if(data == NULL);

	double val = elm_slider_value_get(obj);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	sound_type_e sound_type = SOUND_TYPE_NOTIFICATION;

	if (list_item->chk_status != (int)(val + 0.5)) {
		list_item->chk_status = (int)(val + 0.5);
		setting_sound_update_slider_icon(list_item, SND_SLIDER_NOTI);
		sound_manager_set_volume(sound_type, list_item->chk_status);
		SETTING_TRACE_DEBUG(
				"notification volume is %d, %f",
				list_item->chk_status,
				val);
	}

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	/*
	 * S5 concept:
	 * On the volume popup,
	 *	1) if user controls the slider, don't play any sounds
	 *	just change volume.
	 *	2) if user controls the HW volume key for ringtone sound,
	 *	the beep sound is supported

	 * On Setting > Sound
	 *	1) if user controls the slider, change volume & play sound
	 *	2) if user controls the HW volume key, do nothing
	 *	(DO NOT change volume,DO NOT play sound)
	*/
	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		return;

	if (ad->mp_noti == NULL) {
		setting_sound_close_other_mm_players_but_type(ad, sound_type);
		if (list_item->chk_status > 0) {
			char *pa_tone_path = vconf_get_str(
					VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
			if (pa_tone_path == NULL) {
				pa_tone_path = (char *)strdup(
						SETTING_DEFAULT_MSG_TONE);
				if (vconf_set_str(
						VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
						pa_tone_path) < 0) {
					FREE(pa_tone_path);
					return;
				}
			}
			setting_sound_play_sound_origin(
					&(ad->mp_noti),
					ad,
					NULL,
					pa_tone_path,
					(float)list_item->chk_status,
					sound_type);
			FREE(pa_tone_path);
		}
	} else {
		player_state_e state;
		player_get_state(*(ad->mp_noti), &state);
		SETTING_TRACE("noti player status : %d", state);
		if (state == PLAYER_STATE_PAUSED)
			player_start(*(ad->mp_noti));
	}
}


static void __media_slider_change_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	sound_type_e sound_type = SOUND_TYPE_MEDIA;

	double val = elm_slider_value_get(obj);
	int value = (int)(val + 0.5);
	if (list_item->chk_status != value) {

		list_item->chk_status = value;
		setting_sound_update_slider_icon(list_item, SND_SLIDER_MEDIA);
		int ret = sound_manager_set_volume(sound_type, value);
		SETTING_TRACE_DEBUG("ret : 0x%.8x", ret);

		SETTING_TRACE_DEBUG(
				"media volume is %d, %f",
				list_item->chk_status,
				val);
	}

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	/*
	 * S5 concept:
	 * On the volume popup,
	 *	1) if user controls the slider, don't play any sounds
	 *	just change volume.
	 *	2) if user controls the HW volume key for ringtone sound,
	 *	the beep sound is supported

	 * On Setting > Sound
	 *	1) if user controls the slider, change volume & play sound
	 *	2) if user controls the HW volume key, do nothing
	 *	(DO NOT change volume,DO NOT play sound)
	*/
	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		return;

	if (ad->mp_media == NULL) {
		setting_sound_close_other_mm_players_but_type(ad, sound_type);
		if (list_item->chk_status > 0) {
			char	*pa_tone_path = NULL;
			if (isEmulBin())
				pa_tone_path = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				pa_tone_path = (char *)strdup(
						SETTING_DEFAULT_MEDIA_TONE);

			sound_type_e type;
			sound_manager_get_current_sound_type(&type);
			setting_sound_play_sound_origin(
					&(ad->mp_media),
					ad,
					NULL,
					pa_tone_path,
					(float)list_item->chk_status,
					sound_type);
			FREE(pa_tone_path);
		}
	} else {
		player_state_e state;
		player_get_state(*(ad->mp_media), &state);
		SETTING_TRACE("ringtone player status : %d", state);
		if (state == PLAYER_STATE_PAUSED)
			player_start(*(ad->mp_media));
	}
}

static void __sys_slider_change_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	/* error check */
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	ret_if(ad == NULL);
	setting_sound_close_all_mm_players(ad);

	/*Get the current real value of slider */
	double val = elm_slider_value_get(obj);
	int value = (int)(val + 0.5);
	sound_manager_set_volume(SOUND_TYPE_SYSTEM, value);
	SETTING_TRACE_DEBUG("system volume is %d, %f", value, val);

	setting_sound_create_warning_popup_during_call(ad);
	if (ad->calling_popup)
		return;

	if (list_item->chk_status != value) {

		/*
		 * S5 concept:
		 * On the volume popup,
		 *	1) if user controls the slider, don't play any sounds
		 *	just change volume.
		 *	2) if user controls the HW volume key for ringtone
		 *	sound, the beep sound is supported

		 * On Setting > Sound
		 *	1) if user controls the slider, change
		 *	volume & play sound
		 *	2) if user controls the HW volume key, do nothing
		 *	(DO NOT change volume,DO NOT play sound)
		*/
		if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
			/*Play Tap Sound,then stop at once */
			if (feedback_initialize() == FEEDBACK_ERROR_NONE) {
				/*sound_manager_session_set_state_others(
				 * SOUND_STATE_PAUSE); */
				feedback_play_type(
						FEEDBACK_TYPE_SOUND,
						FEEDBACK_PATTERN_TAP);
				feedback_deinitialize();
			}
		}
		list_item->chk_status = value;
		setting_sound_update_slider_icon(list_item, SND_SLIDER_SYSTEM);
	}
}


void __stop_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SETTING_TRACE("chk_status : %d, slider : %f", list_item->chk_status,
			elm_slider_value_get(obj));
	if (list_item->chk_status == 0)
		elm_slider_value_set(obj, 0.0);
	else if (list_item->chk_status == 15)
		elm_slider_value_set(obj, 15.0);
	SETTING_TRACE_END;
}

char *setting_do_not_disturb_is_enable(void *data)
{
	int err = NOTIFICATION_ERROR_NONE;
	bool do_not_disturb;
	notification_system_setting_h system_setting = NULL;

	err = notification_system_setting_load_system_setting(&system_setting);
	setting_retvm_if(NULL == system_setting
			|| NOTIFICATION_ERROR_NONE != err,
			"NULL parameter",
			"notification_system_setting_load_system_Setting failed");
	err = notification_system_setting_get_do_not_disturb(
			system_setting, &do_not_disturb);
	SETTING_TRACE("do_not_disturb: %d", do_not_disturb);

	if (1 == do_not_disturb) {
		notification_system_setting_free_system_setting(system_setting);
		return "IDS_ST_BODY_ON";
	} else {
		notification_system_setting_free_system_setting(system_setting);
		return "IDS_ST_BODY_ALERTTYPE_OFF";
	}
}

static void
__sound_chk_cb(void *data, Evas_Object *obj,
			   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	SettingProfileUG *ad = list_item->userdata;
	ret_if(ad == NULL);

	setting_sound_close_all_mm_players(ad);
	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);
	/* rollback */
	setting_update_gl_item_chk_status(list_item, !list_item->chk_status);
	setting_sound_main_mouse_up_Gendial_list_cb(ad, NULL, list_item->item);
}

#define INIT_PATH	_TZ_SYS_SHARE"/settings/Vibrations/"
static void __get_lite_main_list(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingProfileUG *ad = (SettingProfileUG *)data;

	int vconf_value = 0;
	char *sub_desc = NULL;
	char *left_icon = NULL;
	int mm_value = 0;
	int sound_value = 0;
	int vconf_ret = 0;
	char *pa_ringtone = NULL;

	Evas_Object *genlist = ad->md.genlist;
	elm_genlist_block_count_set(genlist, 3);
	elm_genlist_homogeneous_set(genlist, 1);

	if (vconf_get_bool(
			VCONFKEY_SETAPPL_SOUND_STATUS_BOOL,
			&sound_value) < 0)
		sound_value = TRUE;	/*	default value of sound : on */

	Elm_Genlist_Item_Class *itc_slider = NULL;


	/*------------------------------------------------------------------ */
	/*------------------------------------------------------------------ */
	/*------------------------------------------------------------------ */
	itc_slider = &(ad->itc_layout_1icon);

	if (0 == safeStrCmp(ad->viewtype, VOLUME_APP_NAME))
		itc_slider = &(ad->itc_layout);
	/*------------------------------------------------------------------ */
	/*------------------------------------------------------------------ */
	/*------------------------------------------------------------------ */

	/* 1. Ringtones */
	SETTING_TRACE(" ???????????? ad->viewtype : %s ", ad->viewtype);

	if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {

		int sd_status = VCONFKEY_SYSMAN_MMC_REMOVED;
		if (vconf_get_int(
				VCONFKEY_SYSMAN_MMC_STATUS,
				&sd_status) != 0) {
			SETTING_TRACE_ERROR("fail to get mmc status");
		}
		char *sdcard_filepath = vconf_get_str(
				VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_STR);
		if (0 == safeStrNCmp(
				sdcard_filepath,
				INIT_PATH,
				safeStrLen(INIT_PATH))) {
			vconf_set_str(VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_STR,
					"NONE");
			sdcard_filepath = vconf_get_str(
					VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_STR);
		}
		if (safeStrCmp(sdcard_filepath, "NONE") != 0
				&& sd_status == VCONFKEY_SYSMAN_MMC_MOUNTED) {
			/*check sdcard filepath*/
			if (sdcard_filepath)
				pa_ringtone = strdup(sdcard_filepath);
			FREE(sdcard_filepath);
			vconf_set_str(VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_STR,
					"NONE");
		} else {
			pa_ringtone = vconf_get_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		}
		if (setting_sound_check_file_exist(ad, pa_ringtone) ==
				SETTING_RETURN_FAIL) {
			if (isEmulBin()) {
				pa_ringtone = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			} else {
				pa_ringtone = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE);
			}
		}
		vconf_ret = vconf_set_str(
				VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
				pa_ringtone);
		if (vconf_ret < 0)
			SETTING_TRACE_DEBUG("failed to set vconf");

		sub_desc = setting_media_basename(pa_ringtone);
		ad->data_call_alert_tone =
			setting_create_Gendial_field_def(
					genlist,
					&itc_2text_2,
					setting_sound_main_mouse_up_Gendial_list_cb,
					ad,
					SWALLOW_Type_INVALID,
					NULL,
					NULL,
					0,
					"IDS_ST_MBODY_RINGTONE",
					sub_desc,
					NULL);
		if (ad->data_call_alert_tone) {
			__BACK_POINTER_SET(ad->data_call_alert_tone);
			ad->data_call_alert_tone->userdata = ad;
		}

		FREE(pa_ringtone);
		G_FREE(sub_desc);
	}


	/* 2. call volumn. */
	if (sound_manager_get_volume(SOUND_TYPE_RINGTONE, &mm_value) < 0)
		mm_value = SETTING_DEFAULT_RINGTONE_VOL_INT;
	if (sound_value == FALSE)
		mm_value = 0;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_CALL, mm_value);
	ADD_SOUND_SLIDER(
			ad->data_call_volume,
			genlist,
			(*itc_slider),
			left_icon,
			NULL,
			/*"IDS_ST_HEADER_RINGTONES" is for volume App,in
			 * Setting > Sound,it has no title */
			mm_value,
			"IDS_ST_MBODY_RINGTONE",
			ad,
			__call_slider_change_cb,
			SETTING_SOUND_VOL_MAX,
			__start_change_cb,
			__stop_change_cb);
	__BACK_POINTER_SET(ad->data_call_volume);

	/* Disable Call Volume slider if Silent ringtone is selected */
	if (ad->data_call_volume
			&& ad->data_call_alert_tone
			&& !safeStrCmp(ad->data_call_alert_tone->sub_desc, "Silent")) {
		elm_object_item_disabled_set(ad->data_call_volume->item, EINA_TRUE);
		elm_object_disabled_set(ad->data_call_volume->eo_check, EINA_TRUE);
	}

	/* 3. Notification alert */
	if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		pa_ringtone = vconf_get_str(
				VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
		if (setting_sound_check_file_exist(ad, pa_ringtone)
				== SETTING_RETURN_FAIL) {
			pa_ringtone = (char *)strdup(SETTING_DEFAULT_MSG_TONE);
			vconf_ret = vconf_set_str(
					VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
					SETTING_DEFAULT_MSG_TONE);
			if (vconf_ret < 0)
				SETTING_TRACE_DEBUG("failed to set vconf");
		}
		sub_desc = setting_media_basename(pa_ringtone);
		ad->data_msg_alert_tone =
			setting_create_Gendial_field_def(
					genlist,
					&itc_2text_2,
					setting_sound_main_mouse_up_Gendial_list_cb,
					ad,
					SWALLOW_Type_INVALID,
					NULL,
					NULL,
					0,
					/*SETTING_SOUND_TEXT_MSG_ALERT*/
					"IDS_ST_BODY_NOTIFICATION",
					sub_desc,
					NULL);
		if (ad->data_msg_alert_tone) {
			__BACK_POINTER_SET(ad->data_msg_alert_tone);
			ad->data_msg_alert_tone->userdata = ad;
		}

		FREE(pa_ringtone);
		G_FREE(sub_desc);
	}
	/* 4.noti volumn */
	if (sound_manager_get_volume(SOUND_TYPE_NOTIFICATION, &mm_value) < 0)
		mm_value = SETTING_DEFAULT_NOTI_VOL_INT;
	if (sound_value == FALSE)
		mm_value = 0;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_NOTI, mm_value);
	ADD_SOUND_SLIDER(
			ad->data_noti_volume,
			genlist,
			(*itc_slider),
			left_icon,
			NULL,
			mm_value,
			"IDS_ST_BODY_NOTIFICATION",
			ad,
			__noti_slider_change_cb,
			SETTING_SOUND_VOL_MAX,
			__start_change_cb,
			__stop_change_cb);
	__BACK_POINTER_SET(ad->data_noti_volume);

	/* Disable Notification slider if Silent ringtone is selected */
	if (ad->data_noti_volume
			&& ad->data_msg_alert_tone
			&& !safeStrCmp(ad->data_msg_alert_tone->sub_desc, "Silent")) {
		elm_object_item_disabled_set(ad->data_noti_volume->item, EINA_TRUE);
		elm_object_disabled_set(ad->data_noti_volume->eo_check, EINA_TRUE);
	}

	/* 5.media volume */
	if (sound_manager_get_volume(SOUND_TYPE_MEDIA, &mm_value) < 0)
		mm_value = SETTING_DEFAULT_MEDIA_VOL_INT;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_MEDIA, mm_value);
	ADD_SOUND_SLIDER(
			ad->data_media_volume,
			genlist,
			ad->itc_layout,
			left_icon, /*IMG_SLIDER_BTN_VOLUME02*/
			NULL,
			mm_value,
			"IDS_ST_BODY_MEDIA",
			ad,
			__media_slider_change_cb,
			SETTING_SOUND_VOL_MAX,
			__start_change_cb,
			__stop_change_cb);

	__BACK_POINTER_SET(ad->data_media_volume);

	/* 6.system volume */
	if (sound_manager_get_volume(SOUND_TYPE_SYSTEM, &mm_value) < 0) {
		SETTING_TRACE_DEBUG("Fail to get volume");
		mm_value = SETTING_DEFAULT_SYSTEM_VOL_INT;
	}
	if (sound_value == FALSE)
		mm_value = 0;

	left_icon = setting_sound_get_slider_icon(SND_SLIDER_SYSTEM, mm_value);
	ADD_SOUND_SLIDER(
			ad->data_touch_volume, genlist, ad->itc_layout,
			left_icon, NULL/*IMG_SLIDER_BTN_SYSTEM_VOLUME*/,
			mm_value, "IDS_ST_BODY_SYSTEM", ad,
			__sys_slider_change_cb,
			SETTING_SOUND_VOL_MAX,
			__start_change_cb, __stop_change_cb);

	__BACK_POINTER_SET(ad->data_touch_volume);

	if (0 != safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		/* 7. Vibrate when ringing */
		if (vconf_get_bool(
				VCONFKEY_SETAPPL_VIBRATE_WHEN_RINGING_BOOL,
				&vconf_value) < 0) {
			/* default value of vibrate sound : on */
			vconf_value = TRUE;
		}
		ADD_TOGGLE_MENU2(
				ad->data_sound_when_ring,
				genlist,
				itc_1text_1icon,
				"IDS_ST_BODY_VIBRATE_WHEN_RINGING",
				vconf_value,
				ad);
		__BACK_POINTER_SET(ad->data_sound_when_ring);
		setting_genlist_item_disabled_set(
				ad->data_sound_when_ring, !sound_value);

		/* 8. Feedback */
		setting_create_Gendial_field_def(genlist, &itc_1text,
				setting_sound_main_mouse_up_Gendial_list_cb,
				data, SWALLOW_Type_INVALID, NULL,
				NULL, 0,
				"IDS_ST_HEADER_FEEDBACK", NULL, NULL);

		/* Notifications */
		setting_create_Gendial_field_titleItem(
				genlist,
				&itc_group_item,
				_("IDS_ST_BODY_NOTIFICATIONS"),
				NULL);
		/* 9. Notifications - Do not disturb */
		char *sub_desc = setting_do_not_disturb_is_enable(data);
		ad->data_do_not_disturb = setting_create_Gendial_field_def(
				genlist,
				&itc_2text_2,
				setting_sound_main_mouse_up_Gendial_list_cb,
				data,
				SWALLOW_Type_INVALID,
				NULL,
				NULL,
				0,
				"IDS_ST_MBODY_DO_NOT_DISTURB_ABB",
				sub_desc,
				NULL);
		/* 10. Notifications - App notifications */
		setting_create_Gendial_field_def(
				genlist,
				&itc_2text_2,
				setting_sound_main_mouse_up_Gendial_list_cb,
				data,
				SWALLOW_Type_INVALID,
				NULL,
				NULL,
				0,
				"IDS_ST_MBODY_APP_NOTIFICATIONS",
				"IDS_ST_BODY_ALLOW_OR_BLOCK_NOTIFICATIONS_FROM_"
				"INDIVIDUAL_APPLICATIONS",
				NULL);

		ad->md.genlist = genlist;

	}
	if (!sound_value) {
		setting_genlist_item_disabled_set(ad->data_noti_volume, 1);
		setting_genlist_item_disabled_set(ad->data_touch_volume, 1);
	}
}

static void __volume_popup_del_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/*SETTING_TRACE(" REMOVE THE POPUP OBJECT BY THIS FUNCTION "); */
	elm_exit();
}

unsigned int vibration_state_reg_id;

static Eina_Bool
setting_soudn_main_click_softkey_back_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
			"[Setting > Display] Data parameter is NULL");

	SettingProfileUG *ad = (SettingProfileUG *) data;

	/* Send destroy request */
//ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;

}


static int setting_sound_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingProfileUG *ad = (SettingProfileUG *) cb;

	int ret = view_init(&ad->md, _("VOLUME_APP_NAME"));

	if (ret != SETTING_RETURN_SUCCESS)
	    return ret;

	/* 1.Grab Hard Key */
	__setting_sound_ug_key_grab(ad);
	SETTING_TRACE("viewtype: %s", ad->viewtype);
	if (!safeStrCmp(ad->viewtype, VOLUME_APP_NAME)) {
		//ad->md.ly_main = setting_create_popup_with_list(
				//&ad->md.genlist, ad, ad->md.win_main,
				//"IDS_ST_BODY_VOLUME", __volume_popup_del_cb,
				//0 , false, false, 1, "IDS_ST_SK3_DONE");
		__get_lite_main_list(ad);
	} else {
		//ad->md.ly_main = setting_create_win_layout(ad->md.win_main);
		//ad->md.navibar_main = setting_create_navi_bar(ad->md.ly_main);

		ADD_GENLIST(ad->md.genlist, ad->md.navibar_main);

		__get_lite_main_list(ad);

		Evas_Object *lbtn = setting_create_button(ad->md.navibar_main, _(NULL),
				NAVI_BACK_ARROW_BUTTON_STYLE,
				&setting_soudn_main_click_softkey_back_cb, ad);

		Elm_Object_Item *navi_it = elm_naviframe_item_push(
				ad->md.navibar_main,
				KeyStr_Sounds,
				lbtn,
				NULL,
				ad->md.genlist,
				NULL);
		elm_naviframe_item_pop_cb_set(
				navi_it,
				(Elm_Naviframe_Item_Pop_Cb)setting_sound_main_click_softkey_cancel_cb,
				ad);
		/*elm_object_item_part_content_set(
		 * navi_it, "title_left_btn", lbtn); */
		elm_object_item_domain_text_translatable_set(
				navi_it, SETTING_PACKAGE, EINA_TRUE);
	}
	setting_view_sound_main.is_create = TRUE;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	retv_if(!setting_view_sound_main.is_create,
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	/* 1.Ungrab Key */
	__setting_sound_ug_key_ungrab(ad);

	if (ad->play_ringtone_idler) {
		ecore_idler_del(ad->play_ringtone_idler);
		ad->play_ringtone_idler = NULL;
	}

	evas_object_del(ad->md.ly_main);
	ad->md.ly_main = NULL;

	setting_view_sound_main.is_create = FALSE;
	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_update(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	if (ad->md.ly_main != NULL)
		evas_object_show(ad->md.ly_main);

	return SETTING_RETURN_SUCCESS;
}

static int setting_sound_main_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingProfileUG *ad = (SettingProfileUG *) cb;

	if (ad->md.ly_main != NULL)
		evas_object_hide(ad->md.ly_main);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static Eina_Bool setting_sound_main_click_softkey_cancel_cb(
		void *data, Elm_Object_Item *it)
{
	/* error check */
	retv_if(data == NULL, EINA_FALSE);

	SettingProfileUG *ad = (SettingProfileUG *) data;
	SETTING_TRACE_BEGIN;

	/* close mm plaer */
	setting_sound_close_all_mm_players(ad);

	/* Send destroy request */
	//ug_destroy_me(ad->ug);

	return EINA_FALSE;
}

static void setting_sound_main_destroy_myfile_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingProfileUG *ad = (SettingProfileUG *) priv;

	int sound_value = 0, volume = 0;

	__setting_sound_ug_key_grab(ad);

	if (ug) {
		//setting_ug_destroy(ug);
		//ad->ug_loading = NULL;
	}

	if (vconf_get_bool(
			VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &sound_value) < 0)
		sound_value = TRUE;

	if (sound_value == FALSE)
		volume = 0;
	else
		(void)sound_manager_get_volume(SOUND_TYPE_RINGTONE, &volume);

	if (ad->data_call_volume && ad->data_call_volume->eo_check) {
		ad->data_call_volume->chk_status = volume;
		elm_slider_value_set(ad->data_call_volume->eo_check, volume);
	}

	if (sound_value == FALSE)
		volume = 0;
	else
		(void)sound_manager_get_volume(
				SOUND_TYPE_NOTIFICATION, &volume);

	if (ad->data_noti_volume && ad->data_noti_volume->eo_check) {
		ad->data_noti_volume->chk_status = volume;
		elm_slider_value_set(ad->data_noti_volume->eo_check, volume);
	}

	setting_sound_update_slider_icon(ad->data_call_volume, SND_SLIDER_CALL);
	setting_sound_update_slider_icon(ad->data_noti_volume, SND_SLIDER_NOTI);
}

static void
setting_sound_main_result_myfile_ug_cb(ui_gadget_h ug,
		app_control_h result, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);

	SettingProfileUG *ad = (SettingProfileUG *) priv;
	char *ringtone_file = NULL;
	char *ringtone_path = NULL;

	app_control_get_extra_data(result, "result", &ringtone_path);
	if (!ringtone_path)
		return;

	char *recommend_time = NULL;
	app_control_get_extra_data(result, "position", &recommend_time);

	SETTING_TRACE("ringtone_path:%s", ringtone_path);

	if (ringtone_path != NULL) {
		ringtone_file = setting_media_basename(ringtone_path);
		SETTING_TRACE("ringtone_file:%s", ringtone_file);

		if (0 == safeStrCmp(
				ad->ringtone_type, "IDS_ST_MBODY_RINGTONE")) {
			if (vconf_set_str(
					VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR,
					ringtone_path) == 0) {
				if (ad->data_call_alert_tone) {
					ad->data_call_alert_tone->sub_desc =
							(char *)g_strdup(ringtone_file);

					elm_object_item_data_set(
							ad->data_call_alert_tone->item,
							ad->data_call_alert_tone);
					elm_genlist_item_update(
							ad->data_call_alert_tone->item);
				}
			}
			if (recommend_time) {
				char recommend_info[512] = {0,};
				snprintf(recommend_info, 512, "%s; %smsec",
						ringtone_path, recommend_time);
				SETTING_TRACE_DEBUG("%s", recommend_info);
				vconf_set_str(VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_WITH_RECOMMENDATION_TIME_STR,
						recommend_info);
			}
		} else if (0 == safeStrCmp(ad->ringtone_type,
				"IDS_ST_BODY_NOTIFICATION")) {
			if (vconf_set_str(
					VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR,
					ringtone_path) == 0) {
				if (ad->data_msg_alert_tone) {
					ad->data_msg_alert_tone->sub_desc =
							(char *)g_strdup(ringtone_file);

					elm_object_item_data_set(
							ad->data_msg_alert_tone->item,
							ad->data_msg_alert_tone);
					elm_genlist_item_update(
							ad->data_msg_alert_tone->item);
				}
			}
			if (!safeStrCmp(ringtone_file, "Silent")) {
				/* Disable */
				Setting_GenGroupItem_Data *item_to_update = NULL;
				item_to_update = ad->data_noti_volume;
				if (item_to_update && item_to_update->item
						&& item_to_update->eo_check) {
					item_to_update->chk_status = 0;
					elm_slider_value_set(item_to_update->eo_check,
							item_to_update->chk_status);

					sound_manager_set_volume(SOUND_TYPE_NOTIFICATION, 0);

					elm_object_item_disabled_set(item_to_update->item, EINA_TRUE);
					elm_object_disabled_set(item_to_update->eo_check, EINA_TRUE);

					/* Change notification slider's icon as mute style */
					setting_sound_update_slider_icon(item_to_update,
							SND_SLIDER_NOTI);
				}
			} else {
				/* Enable */
				Setting_GenGroupItem_Data *item_to_update = NULL;
				item_to_update = ad->data_noti_volume;
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
				sound_manager_set_volume(SOUND_TYPE_NOTIFICATION,
						SETTING_DEFAULT_NOTI_VOL_INT);
				setting_sound_update_slider_icon(
						item_to_update, SND_SLIDER_NOTI);
			}
		}
	}

	FREE(ringtone_path);
	FREE(recommend_time);
	G_FREE(ringtone_file);
	return;
}

static void setting_sound_main_create_myfile_ug(
		SettingProfileUG *ad, char *titleID)
{
	ret_if(ad == NULL);

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs)
		return;

	cbs->layout_cb = setting_layout_ug_cb;
	cbs->result_cb = setting_sound_main_result_myfile_ug_cb;
	cbs->destroy_cb = setting_sound_main_destroy_myfile_ug_cb;
	cbs->priv = (void *)ad;

	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return;
	}

	const char *pa_cur_ringtone = NULL;
	const char *dir_path = NULL;
	if (0 == safeStrCmp(ad->ringtone_type, "IDS_ST_MBODY_RINGTONE")) {
		pa_cur_ringtone = vconf_get_str(
				VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		if (pa_cur_ringtone == NULL) {
			if (isEmulBin())
				pa_cur_ringtone = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE_SDK);
			else
				pa_cur_ringtone = (char *)strdup(
						SETTING_DEFAULT_CALL_TONE);
		}
		dir_path = SETTING_PROFILE_SOUND_RINGTONES_PATH;
	} else if (0 == safeStrCmp(
			ad->ringtone_type,
			"IDS_ST_BODY_NOTIFICATION")) {
		pa_cur_ringtone = vconf_get_str(
				VCONFKEY_SETAPPL_NOTI_MSG_RINGTONE_PATH_STR);
		if (pa_cur_ringtone == NULL)
			pa_cur_ringtone = (char *)strdup(
					SETTING_DEFAULT_MSG_TONE);
		dir_path = SETTING_PROFILE_SOUND_ALERTS_PATH;
	}
	SETTING_TRACE("pa_cur_ringtone:\t%s", pa_cur_ringtone);
	SETTING_TRACE("dir_path:\t%s", dir_path);

	app_control_add_extra_data(svc, "title", titleID);
	app_control_add_extra_data(svc, "domain", SETTING_PACKAGE);
	app_control_add_extra_data(svc, "path", dir_path);
	app_control_add_extra_data(svc, "select_type", "SINGLE_FILE");
	app_control_add_extra_data(svc, "file_type", "SOUND");
	app_control_add_extra_data(svc, "drm_type", "DRM_ALL");
	/* Show Silent item in the list */
	app_control_add_extra_data(svc, "silent", "silent show");
	if (safeStrCmp(pa_cur_ringtone, "silent") == 0)
		/* Set radio on Silent item */
		app_control_add_extra_data(svc, "marked_mode", "silent");
	else
		app_control_add_extra_data(svc, "marked_mode", pa_cur_ringtone);

	elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);

/*	ad->ug_loading = setting_ug_create(
			ad->ug,
			"setting-ringtone-efl",
			UG_MODE_FULLVIEW,
			svc,
			cbs);

	if (NULL == ad->ug_loading) {
		SETTING_TRACE_ERROR("NULL == ad->ug_loading");
	}
*/
	app_control_destroy(svc);
	FREE(cbs);

	/** @todo verify it's valid */
	if (pa_cur_ringtone) {
		free((void *)pa_cur_ringtone);
		pa_cur_ringtone = NULL;
	}

	/*__setting_sound_ug_key_ungrab(ad); */
	/*__setting_sound_ug_menu_key_grab(ad); */
	return;
}

static Eina_Bool __feedback_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, EINA_TRUE);
	SettingProfileUG *ad = (SettingProfileUG *) data;
	/*setting_view_change(&setting_view_feedback_main,
	 *  &setting_view_sound_main, ad); */
	elm_naviframe_item_pop(ad->md.navibar_main);
	SETTING_TRACE_END;
	return EINA_TRUE;
}

#if 0
void setting_sound_notifications_layout_ug_cb(ui_gadget_h ug,
		enum ug_mode mode,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *base;

	if (!priv)
		return;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		evas_object_show(base);
		break;
	default:
		break;
	}
	SETTING_TRACE_END;
}
#endif

static void
setting_sound_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj,
		void *event_info)
{
	/* error check */
	ret_if(data == NULL);
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = elm_object_item_data_get(item);

	SettingProfileUG *ad = data;

	if (SETTING_RETURN_SUCCESS != setting_sound_close_all_mm_players(ad))
		SETTING_TRACE_ERROR("close player fail.");

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (!safeStrCmp("IDS_ST_MBODY_RINGTONE", list_item->keyStr)
		|| !safeStrCmp("IDS_ST_BODY_NOTIFICATION", list_item->keyStr)) {
		ad->ringtone_type = list_item->keyStr;
		setting_sound_main_create_myfile_ug(ad, list_item->keyStr);
		__setting_sound_ug_key_ungrab(ad);
	} else if (!safeStrCmp("IDS_ST_MBODY_DO_NOT_DISTURB_ABB",
			list_item->keyStr)) {
#if 0
		SettingProfileUG *ad = (SettingProfileUG *) data;
		struct ug_cbs *cbs = (struct ug_cbs *)calloc(
				1, sizeof(struct ug_cbs));
		setting_retm_if(!cbs, "calloc failed");
		cbs->layout_cb = setting_sound_notifications_layout_ug_cb;
		cbs->result_cb = NULL;
		cbs->destroy_cb = setting_sound_notifications_destroy_ug_cb;
		cbs->priv = (void *)ad;

		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);

		SETTING_TRACE("To load ug[%s]",
				"ug-setting-notification-do-not-disturb-efl");
		ug_create(ad->ug, "ug-setting-notification-do-not-disturb-efl",
				UG_MODE_FULLVIEW, NULL, cbs);
		FREE(cbs);
#else
		app_control_h service = 0;
		app_control_create(&service);
		app_control_set_app_id(service,
				"org.tizen.setting-notification");
		app_control_set_operation(service, DO_NOT_DISTURB_OP);
		app_control_send_launch_request(service, NULL, NULL);
		app_control_destroy(service);
#endif
	} else if (!safeStrCmp("IDS_ST_MBODY_APP_NOTIFICATIONS",
			list_item->keyStr)) {
#if 0
		SettingProfileUG *ad = (SettingProfileUG *) data;
		struct ug_cbs *cbs = (struct ug_cbs *)calloc(
				1, sizeof(struct ug_cbs));
		setting_retm_if(!cbs, "calloc failed");
		cbs->layout_cb = setting_sound_notifications_layout_ug_cb;
		cbs->result_cb = NULL;
		cbs->destroy_cb = setting_sound_notifications_destroy_ug_cb;
		cbs->priv = (void *)ad;

		elm_object_tree_focus_allow_set(ad->md.ly_main, EINA_FALSE);

		SETTING_TRACE("To load ug[%s]",
				"ug-setting-notification-app-notifications-efl");
		ug_create(ad->ug,
				"ug-setting-notification-app-notifications-efl",
				UG_MODE_FULLVIEW, NULL, cbs);
		FREE(cbs);
#else
		app_control_h service = 0;
		app_control_create(&service);
		app_control_set_app_id(service,
				"org.tizen.setting-notification");
		app_control_set_operation(service, APP_NOTIFICATIONS_OP);
		app_control_send_launch_request(service, NULL, NULL);
		app_control_destroy(service);

#endif

	}
	/* additional */
	else if (!safeStrCmp(
			"IDS_ST_BODY_VIBRATE_WHEN_RINGING",
			list_item->keyStr)) {
		/* vibrate when ring*/
		/*elm_check_state_get(list_item->eo_check); */
		int old_status = list_item->chk_status;
		if (vconf_set_bool(VCONFKEY_SETAPPL_VIBRATE_WHEN_RINGING_BOOL,
				!old_status) == 0) {
			/* new status */
			setting_update_gl_item_chk_status(list_item,
					!old_status);
		}
	} else if (!safeStrCmp("IDS_ST_BODY_SCREEN_LOCK_SOUND",
			list_item->keyStr)) {
		/*elm_check_state_get(list_item->eo_check); */
		int old_status = list_item->chk_status;
		if (vconf_set_bool(VCONFKEY_SETAPPL_SOUND_LOCK_BOOL,
				!old_status) == 0) {
			/* new status */
			setting_update_gl_item_chk_status(list_item,
					!old_status);
		}
	} else if (!safeStrCmp("IDS_ST_MBODY_DIALLING_KEYPAD_TONE",
			list_item->keyStr)) {
		/*elm_check_state_get(list_item->eo_check); */
		int old_status = list_item->chk_status;
		/* To do : set vconfkey for touch sounds */
		if (vconf_set_bool(VCONFKEY_SETAPPL_BUTTON_SOUNDS_BOOL,
				!old_status) == 0) {
			/* new status */
			setting_update_gl_item_chk_status(list_item,
					!old_status);
		}
	} else if (!safeStrCmp("IDS_ST_MBODY_TOUCH_SOUND", list_item->keyStr)) {
		/*elm_check_state_get(list_item->eo_check); */
		int old_status = list_item->chk_status;
		/* To do : set vconfkey for touch sounds */
		if (vconf_set_bool(VCONFKEY_SETAPPL_TOUCH_SOUNDS_BOOL,
				!old_status) == 0) {
			/* new status */
			setting_update_gl_item_chk_status(list_item,
					!old_status);
		}
	} else if (!safeStrCmp("IDS_ST_HEADER_FEEDBACK", list_item->keyStr)) {
		__setting_sound_ug_key_ungrab(ad);

		int vconf_value = 0;
		Evas_Object *genlist = NULL;
		ADD_GENLIST2(genlist, ad->md.navibar_main);
		/* 1. Touch sounds */
		if (vconf_get_bool(VCONFKEY_SETAPPL_TOUCH_SOUNDS_BOOL,
				&vconf_value) < 0)
			/* default value of touch sounds : on */
			vconf_value = TRUE;
		ADD_TOGGLE_MENU(ad->data_touch_sounds, genlist, itc_1text_1icon,
				"IDS_ST_MBODY_TOUCH_SOUND",
				"IDS_ST_BODY_PLAY_A_SOUND_WHEN_BUTTONS_ICONS_"
					"AND_MENU_ITEMS_ARE_TAPPED",
				vconf_value, ad)

		/* 2. Keytones */
		if (vconf_get_bool(VCONFKEY_SETAPPL_BUTTON_SOUNDS_BOOL,
				&vconf_value) < 0)
			/* default value of touch sounds : on */
			vconf_value = TRUE;
		ADD_TOGGLE_MENU(ad->data_button_sounds, genlist,
				itc_1text_1icon,
				"IDS_ST_MBODY_DIALLING_KEYPAD_TONE",
				"IDS_ST_BODY_PLAY_TONE_WHEN_THE_DIALLING_"
					"KEYPAD_IS_TAPPED",
				vconf_value, ad)

		/*	3. Screen lock sound */
		if (vconf_get_bool(VCONFKEY_SETAPPL_SOUND_LOCK_BOOL,
				&vconf_value) < 0)
			/* default value of lock/unlock sound : off */
			vconf_value = 0;
		ADD_TOGGLE_MENU(ad->data_lock_sound, genlist, itc_1text_1icon,
				"IDS_ST_BODY_SCREEN_LOCK_SOUND",
				"IDS_ST_BODY_PLAY_SOUNDS_WHEN_LOCKING_AND_UNLOCKING_SCREEN",
				vconf_value, ad)

		Elm_Object_Item *navi_it = elm_naviframe_item_push(ad->md.navibar_main,
				"IDS_ST_HEADER_FEEDBACK", NULL, NULL, genlist,
				NULL);
		elm_object_item_domain_text_translatable_set(navi_it,
				SETTING_PACKAGE, EINA_TRUE);

		elm_naviframe_item_pop_cb_set(
				navi_it,
				(Elm_Naviframe_Item_Pop_Cb)__feedback_back_cb,
				ad);
	}
}
