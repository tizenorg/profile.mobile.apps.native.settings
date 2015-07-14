/*
 * setting
 * * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
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
#include <setting.h>
#include <app.h>
#include <sound_manager.h>

#include <appcore-common.h>
#include <sensor.h>
#include <aul.h>
#include <app_preference.h>

#include "setting-helper.h"
#include "setting-main.h"
#include <setting-cfg.h>

#include <setting-common-search.h>
#include <elm_object.h>
#include <appsvc.h>
/*#include <nfc.h> */
#include <signal.h>
#include <system_settings.h>
#include <bundle_internal.h>

#define KEY_END "XF86Stop"
/*#define SUPPORT_UG_MESSAGE */


#define SETTING_SOUND_VOL_MAX 15
#define SETTING_DEFAULT_RINGTONE_VOL_INT	11
#define SETTING_DEFAULT_NOTI_VOL_INT		11
#define SETTING_DEFAULT_MEDIA_VOL_INT		9

int g_geometry_x, g_geometry_y, g_geometry_w, g_geometry_h;
extern int aul_listen_app_dead_signal(int (*func)(int signal, void *data), void *data);

/* This API is defined in <app_service_private.h>. But, cannot include and it is not a managed API.
	The way to resolve : 1. Add extern.
						 2. Use bundle pointer originally
	At first, choose No.1 */
extern int app_control_create_request(bundle *data, app_control_h *service);


setting_main_appdata *g_main_ad;

void termination_handler(int signum)
{
	SETTING_TRACE_BEGIN;
	/* do something for signal handling */
	/*SETTING_TRACE_DEBUG(">>>>>>>>>>>>>>>>> SIGTERM >>>>>>>>>>>>>>> SETTING "); */

	elm_exit();
}


#if LOW_BATTERY_DO_NOTHING

/*  in case of low battery, don't terminate itself.*/

/**
* The event process when battery becomes low.
*/
static void setting_main_low_battery_cb(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data, "Invalid argument: data is NULL");
	setting_main_appdata *ad = data;

	if (ad->ug) {
		ug_send_event(UG_EVENT_LOW_BATTERY);
	}
}
#endif


#if SUPPORT_APP_ROATION
static void _rot_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *)data;
	if (ad == NULL || ad->win_main == NULL) {
		return;
	}
	int change_ang = elm_win_rotation_get(ad->win_main);
	SETTING_TRACE_DEBUG("....change_ang:%d", change_ang);
	SETTING_TRACE_DEBUG("current_rotation:%d", ad->current_rotation);
	/*Send the rotation event to UGs.. */
	enum ug_event event = UG_EVENT_ROTATE_PORTRAIT;
	switch (change_ang) {
		case APP_DEVICE_ORIENTATION_0:
			event = UG_EVENT_ROTATE_PORTRAIT;
			break;
		case APP_DEVICE_ORIENTATION_180:
			event = UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN;
			break;
		case APP_DEVICE_ORIENTATION_270:
			event = UG_EVENT_ROTATE_LANDSCAPE;
			break;
		case APP_DEVICE_ORIENTATION_90:
			event = UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN;
			break;
		default:
			return;
	}
	SETTING_TRACE_DEBUG("diff:%d", elm_win_rotation_get(ad->win_main) - ad->current_rotation);

	if (change_ang != ad->current_rotation) {
		int diff = change_ang - ad->current_rotation;
		if (diff < 0) {
			diff = -diff;
		}
		/**
		* @todo if app didn't launch UG, is the call required to invoke?
		*/
		ug_send_event(event);
		if (diff == 180) {
			/* do nothing */
		}
		ad->current_rotation = change_ang;
	}
}
#endif


/**
* The event process when region is changes.
*/
static void setting_main_region_changed_cb(app_event_info_h event_info, void *data)
{
	retm_if(!data, "Invalid argument: data is NULL");
	setting_main_appdata *ad = data;

	if (ad->ug) {
		ug_send_event(UG_EVENT_REGION_CHANGE);
	}
}

/**
* The event process when win object is destroyed
*/
static void setting_main_del_win(void *data, Evas_Object *obj, void *event)
{
	elm_exit();
}

void profile_changed_cb(void *data, Evas_Object *obj, void *event)
{
	const char *profile = elm_config_profile_get();

	if (strcmp(profile, "desktop") == 0) {
		elm_win_indicator_mode_set(obj, ELM_WIN_INDICATOR_HIDE);
	} else {
		elm_win_indicator_mode_set(obj, ELM_WIN_INDICATOR_SHOW);
	}
}

/**
* To create a win object, the win is shared between the App and all its UGs
*/
static Evas_Object *setting_main_create_win(const char *name)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	Evas_Object *eo;
	int w, h;

	eo = (Evas_Object *) elm_win_add(NULL, name, ELM_WIN_BASIC);
	if (!eo)
		eo = elm_win_util_standard_add(name, name);
	else {
		/* elm_win_util_standard_add creates bg inside */
		Evas_Object *bg;

		bg = elm_bg_add(eo);

		if (!bg) {
			evas_object_del(eo);
			return NULL;
		}
		evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_win_resize_object_add(eo, bg);
		evas_object_show(bg);
	}
	if (eo) {
		elm_win_title_set(eo, name);
		evas_object_smart_callback_add(eo, "delete,request", setting_main_del_win, NULL);
		evas_object_smart_callback_add(eo, "profile,changed", profile_changed_cb, NULL);
		/*elm_win_conformant_set(eo, EINA_TRUE); */
		//ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
		// evas_object_resize(eo, w, h);
	}
	LAUNCH_SETTING_OUT();
	return eo;
}


/**
* exceptional process, reset the env vars by Setting vconf VCONFKEY_LANGSET
*/
static void setting_main_lang_changed_cb(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data, "Invalid argument: data is NULL");
	/*FIX the crash when switch from Portrait mode to landscape mode, */
	/*It is so strange the data is no longer correct */
	/*and ....why it is invoked */
	/*setting_main_appdata *ad = data; */
	setting_main_appdata *ad = g_main_ad;

	char *tab_str[] = {
		Keystr_Connection,
		KeyStr_Device,
		KeyStr_MotionControl,
		"IDS_ST_BODY_GENERAL",
		KeyStr_DownloadedAPPs
	};
	int i = 0;

	for (; i < SETTING_TAB_VIEW_MAX; i++) {
		if (ad->ctrl_item[i])
			elm_object_item_text_set(ad->ctrl_item[i], _(tab_str[i]));
	}

	char *tmp = NULL;
	if (ad->data_bright) {
		tmp = ad->data_bright->sub_desc;
		ad->data_bright->sub_desc = (char *)g_strdup(get_brightness_mode_str());
		FREE(tmp)
	}
	if (ad->data_backlight) {
		tmp = ad->data_backlight->sub_desc;
		ad->data_backlight->sub_desc = get_pa_backlight_time_str();
		FREE(tmp)
	}
	if (ad->data_screenmode) {
		tmp = ad->data_screenmode->sub_desc;
		ad->data_screenmode->sub_desc = get_pa_screen_mode_str();
		FREE(tmp)
	}

	Eina_List *list, *l;
	Elm_Object_Item *it;
	i = SETTING_TAB_VIEW_INVALID;
	for (i = SETTING_TAB_VIEW_CONNECTION; i < SETTING_TAB_VIEW_MAX; i++) {
		if (ad->sub_view[i]) {
			list = elm_genlist_realized_items_get(ad->sub_view[i]);
			EINA_LIST_FOREACH(list, l, it) {
				__add_gl_tts_feature(it);
				elm_genlist_item_update(it);
			}
		}
	}

	/* update all tabs */
	for (i = SC_All_List; i < SC_Max; i++) {
		if (ad->sc_gl[i]) {
			list = elm_genlist_realized_items_get(ad->sc_gl[i]);
			EINA_LIST_FOREACH(list, l, it) {
				__add_gl_tts_feature(it);
				elm_genlist_item_update(it);
			}
		}
	}
	/* For UGs */

	elm_object_item_part_text_set(ad->main_navi_it, "elm.text.title", _("IDS_ST_OPT_SETTINGS"));
	setting_navi_items_update(ad->navibar_main);

	if (ad->isInUGMode && ad->ug) {
		Evas_Object *layout = (Evas_Object *)ug_get_layout(ad->ug);
		if (layout) {
			Evas_Object *navi_bar = NULL;
			navi_bar = elm_object_part_content_get(layout, "elm.swallow.content");
			setting_navi_items_update(navi_bar);
		}
		ug_send_event(UG_EVENT_LANG_CHANGE);
	}

}

/**
* the event process when other VCONFS changes
*/
static void setting_other_vconf_change_cb(keynode_t *key, void *data)
{
	setting_main_appdata *ad = data;
	retm_if(!data, "Invalid argument: data is NULL");

	Setting_GenGroupItem_Data *item_to_update = NULL;
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("the value of [ %s ] just changed", vconf_name);
	int i = 0;

	if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_FLIGHT_MODE)) {
		int flightmode = 0;
		vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flightmode);
		/*Flight mode is ON : Network menu dimmed */
		SETTING_TRACE("vconf flight [%d]", flightmode);
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_Network], flightmode);
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_PM], flightmode);
			item_to_update = ad->gl_data_item[i][GL_FlightMode];
			if (item_to_update) {
				item_to_update->chk_status = flightmode;
				const char *tmp = item_to_update->l_swallow_path;
				SETTING_TRACE("status:%d", flightmode);
				if (!flightmode) {
					item_to_update->l_swallow_path = (char *)g_strdup(SETTING_ICON_PATH_CFG"settings_flightoff1.png");
				} else {
					item_to_update->l_swallow_path = (char *)g_strdup(SETTING_ICON_PATH_CFG"A01-1_icon_Flight_Mode.png");
				}
				G_FREE(tmp);

				elm_genlist_item_fields_update(item_to_update->item, "*", ELM_GENLIST_ITEM_FIELD_CONTENT);
			}

		}

		SETTING_TRACE_END;
		return;
#if SUPPORT_NETWORK_RESTRICTION
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE)) {
		int status = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, &status);

		/* Update network restriction mode UI */
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_MobileApp], status);
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_WiFi], status);
			setting_force_update_gl_item_chk_status(ad->gl_data_item[i][GL_NetRestriction], status);
		}

		return;
#endif
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL)) {
		SETTING_TRACE_DEBUG("%s updated", vconf_name);
		int status = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &status);
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_force_update_gl_item_chk_status(ad->gl_data_item[i][GL_Landscape], status);
		}

		if (status) {
			int angle = elm_win_rotation_get(ad->win_main);
			elm_win_rotation_with_resize_set(ad->win_main, angle);
		} else {
			elm_win_rotation_with_resize_set(ad->win_main, 0);
		}
		return;
	} else if (!safeStrCmp(vconf_name, VCONFKEY_NFC_STATE)) {
		SETTING_TRACE_DEBUG("%s updated", vconf_name);
		if (ad->nfc_timer) {
			ecore_timer_del(ad->nfc_timer);
			ad->nfc_timer = NULL;
		}

		int status = 0;
		vconf_get_bool(VCONFKEY_NFC_STATE, &status);

		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_NFC];
			if (item_to_update) {
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1RADIO;
				item_to_update->chk_status = status;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_fields_update(item_to_update->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
			}
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DM_DRIVING_MODE)) {
		SETTING_TRACE_DEBUG("%s updated", vconf_name);
		/* cfg set_item_state has do the work, need to del it, or it will be a issue */

		int status = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_DM_DRIVING_MODE, &status);
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_DM];
			setting_force_update_gl_item_chk_status(item_to_update, status);
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DEVELOPER_OPTION_STATE)) {
#ifndef BINARY_RELEASE_TYPE_ENG
		/*for user binary: need to check develop_option_state vconf value*/
		if (ad->sub_view[SETTING_TAB_VIEW_GENERAL]) {
			SETTING_TRACE("redraw Gernel Tab");
			evas_object_del(ad->sub_view[SETTING_TAB_VIEW_GENERAL]);
			ad->sub_view[SETTING_TAB_VIEW_GENERAL] = NULL;
		}
		//setting_search_genlist_update(ad);
#endif

	}  else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_ENHANCED_TOUCH)) {
		int status = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_ENHANCED_TOUCH, &status);

		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			SETTING_TRACE("ad->gl_data_item[%d][GL_TouchSens]:%p", i, ad->gl_data_item[i][GL_TouchSens]);
			item_to_update = ad->gl_data_item[i][GL_TouchSens];
			setting_force_update_gl_item_chk_status(item_to_update, status);
		}
	}
}

/**
* the event process when int VCONFS changes
* @todo code clean - it has big if-else structure
*/
static void setting_int_vconf_change_cb(keynode_t *key, void *data)
{
	setting_main_appdata *ad = data;
	retm_if(!data, "Invalid argument: data is NULL");

	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("Enter %s(%s=%d)", __FUNCTION__, vconf_name, status);

	Setting_GenGroupItem_Data *item_to_update = NULL;
	int i = 0;
	if (!safeStrCmp(vconf_name, VCONFKEY_WIFI_STATE)) {
		/* do handling in case of VCONFKEY_WIFI_STATE */
		SETTING_TRACE_DEBUG("%s updated", vconf_name);
		SETTING_TRACE("status:%d", status);
		int on_off_state = EINA_FALSE;
		switch (status) {
			case VCONFKEY_WIFI_OFF:
				on_off_state = EINA_FALSE;
				break;

			case VCONFKEY_WIFI_UNCONNECTED: {
					/*item_to_update->chk_status = EINA_FALSE; */
					/*item_to_update->swallow_type = SWALLOW_Type_1ICON_1PROCESS; */
					/*elm_object_item_data_set(item_to_update->item, item_to_update); */
					/*elm_genlist_item_update(item_to_update->item); */
					/*return; */
					bool last_state = FALSE;
					int r = preference_get_boolean(WIFI_LAST_ON_OFF_STATE, &last_state);
					if (0 != r) {
						SETTING_TRACE("ERROR, r:%d", r);
					}
					char *ap_name = vconf_get_str(VCONFKEY_WIFI_CONNECTED_AP_NAME);
					SETTING_TRACE("Connect AP:%s", ap_name);
					if (last_state) {
						/*on -> off */
						SETTING_TRACE("on -> off");
						if (isEmptyStr(ap_name)) {
							if (OP_WIFI_TURNING_ON == ad->wifi_op
							    || OP_WIFI_TURNING_OFF == ad->wifi_op) {
								SETTING_TRACE("Operating in Setting(ad->wifi_op:%d)", ad->wifi_op);
								/*ad->wifi_op = OP_WIFI_TURNING_INVALID; */
								/*on_off_state = EINA_FALSE; */
								return;
							} else {
								SETTING_TRACE("Operating in Other App");
								return;
							}
						} else {
							on_off_state = EINA_FALSE;
						}
					} else {
						/*off -> on */
						SETTING_TRACE("off -> on");
						on_off_state = EINA_TRUE;
					}
					break;
				}
			case VCONFKEY_WIFI_CONNECTED:
			case VCONFKEY_WIFI_TRANSFER:

				on_off_state = EINA_TRUE;
				break;
			default:
				SETTING_TRACE_ERROR("Invalid wifi status!");
				return;
		}
		ad->wifi_op = OP_WIFI_TURNING_INVALID;
		if (ad->wifi_timer) {
			ecore_timer_del(ad->wifi_timer);
			ad->wifi_timer = NULL;
		}

		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_WiFi];
			if (item_to_update) {

				item_to_update->chk_status = on_off_state;
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1RADIO;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_update(item_to_update->item);
			}
		}
		preference_set_boolean(WIFI_LAST_ON_OFF_STATE, on_off_state);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_CALL_STATE)) {
		/* do handling in case of VCONFKEY_CALL_STATE */
		/*In call situation : Wi-fi / Tethering / Network menu dimmed */
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			status = (VCONFKEY_CALL_OFF != status);
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_WiFi], status);
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_MobileApp], status);
		}

	} else if (!safeStrCmp(vconf_name, VCONFKEY_BT_STATUS)) {
		/* do handling in case of VCONFKEY_BT_STATUS */
		SETTING_TRACE_DEBUG("%s updated", vconf_name);
		if (ad->bt_timer) {
			ecore_timer_del(ad->bt_timer);
			ad->bt_timer = NULL;
		}

		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_BT];
			if (item_to_update) {
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1RADIO;
				item_to_update->chk_status = status;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_fields_update(item_to_update->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
			}
		}

	} else if (!safeStrCmp(vconf_name, VCONFKEY_MOBILE_HOTSPOT_MODE)) {
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SYSMAN_BATTERY_STATUS_LOW)) {
		/* do handling in case of VCONFKEY_SYSMAN_BATTERY_STATUS_LOW */
		status  = (status <= VCONFKEY_SYSMAN_BAT_CRITICAL_LOW);
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_genlist_item_disabled_set(ad->gl_data_item[i][GL_Bright], status);
		}

	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT)) {
		/* do handling in case of VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT */
		/*update directly */
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_Bright];
			if (item_to_update) {
				G_FREE(item_to_update->sub_desc);
				item_to_update->sub_desc = (char *)g_strdup(get_brightness_mode_str());
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_update(item_to_update->item);
			}
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL)) {
		/*update directly */
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_Backlight];
			if (item_to_update) {
				char backlight_time_str[MAX_DISPLAY_NAME_LEN_ON_UI];
				if (status == 0) {
					snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
					         _("IDS_ST_BODY_ALWAYS_ON"));
				} else if (status == 60) {
					snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
					         _("IDS_ST_BODY_1MINUTE"));
				} else if (status > 60) {
					snprintf(backlight_time_str, sizeof(backlight_time_str),
					         "%d %s", status / 60, (char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
				} else {
					snprintf(backlight_time_str, sizeof(backlight_time_str),
					         "%d %s", status, (char *)(_("IDS_ST_BODY_SECONDS")));
				}
				SETTING_TRACE("backlight_time_str:%s", backlight_time_str);
				G_FREE(item_to_update->sub_desc);
				item_to_update->sub_desc = (char *)g_strdup(backlight_time_str);
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_update(item_to_update->item);
			}
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE)) {

		/* do nothing */

	} else if (!safeStrCmp(vconf_name, VCONFKEY_SYSMAN_BATTERY_CAPACITY)) {
	} else {
		SETTING_TRACE_ERROR("vconf_name is bad[%s]", vconf_name);
	}

	return;
}


/**
* the event process when string VCONFS changes
*/
static void setting_string_vconf_change_cb(keynode_t *key, void *data)
{
	setting_main_appdata *ad = data;
	retm_if(!data, "Invalid argument: data is NULL");

	char *value = vconf_keynode_get_str(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("Enter %s(%s=%s)", __FUNCTION__, vconf_name, value);

	if (!safeStrCmp(vconf_name, VCONFKEY_WIFI_CONNECTED_AP_NAME)) {
	} /* vconf_name == VCONFKEY_WIFI_CONNECTED_AP_NAME */
	else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_SCREENMODE_SELNAME)) {
		int i;
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			Setting_GenGroupItem_Data *item_to_update = ad->gl_data_item[i][GL_Screenmode];
			if (item_to_update) {
				char *tmp = item_to_update->sub_desc;
				item_to_update->sub_desc = get_pa_screen_mode_str();

				SETTING_TRACE("item_to_update->sub_desc:%s", item_to_update->sub_desc);
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_update(item_to_update->item);
				G_FREE(tmp);
			}
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_BGSET)
	           || !safeStrCmp(vconf_name, VCONFKEY_IDLE_LOCK_BGSET)) {
	}
}

/**
* The function is called to create Setting view widgets
*/
static bool setting_main_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	SETTING_TRACE_DEBUG("[TIME] 3. it taked %d msec from main to setting_main_app_create ", appcore_measure_time());
	appcore_measure_start();

	/*elm_config_preferred_engine_set("opengl_x11"); */
	//ug_create_cb(setting_ug_create_cb, NULL);

#ifdef TIZEN_BUILD_TARGET
	SETTING_TRACE_DEBUG(" ------------------------> TIZEN_BUILD_TARGET is ON");
#endif

#ifdef TIZEN_BUILD_EMULATOR
	SETTING_TRACE_DEBUG(" ------------------------> TIZEN_BUILD_EMULATOR is ON");
#endif

#if SUPPORT_DRI
	setenv("EVAS_DRI_SWAPBUF", "1", 1);
#endif

	setting_main_appdata *ad = data;
	/* regitering sigterm */
	if (signal(SIGTERM, termination_handler) == SIG_IGN) {
		signal(SIGTERM, SIG_IGN);
	}

	app_event_handler_h handlers[5] = {NULL, };
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, setting_main_low_battery_cb, ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, setting_main_lang_changed_cb, ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, setting_main_region_changed_cb, ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);	/* no callback */


#if 0
	app_control_h svc;
	app_control_create(&svc);
	/*app_control_create_request(b, &svc); */
	/*ad->is_searchmode = setting_main_sfinder_handler(svc, ad, NULL); */
	app_control_destroy(svc);
	svc = NULL;
#endif
	/*--------------------------------------------------------------------------------------------- */
	ad->data_roaming_popup = NULL;

	elm_app_base_scale_set(2.4);

	/* create window */
	ad->win_main = setting_main_create_win(SETTING_PACKAGE);
	setting_retvm_if(ad->win_main == NULL, SETTING_RETURN_FAIL, "window is null");
	UG_INIT_EFL(ad->win_main, UG_OPT_INDICATOR_ENABLE);
	ad->evas = evas_object_evas_get(ad->win_main);
	ad->current_rotation = elm_win_rotation_get(ad->win_main);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d", ad->current_rotation);
	if (elm_win_wm_rotation_supported_get(ad->win_main)) {
		int rots[4] = { 0, 90, 180, 270 };  /* rotation value that app may want */
		elm_win_wm_rotation_available_rotations_set(ad->win_main, rots, 4);
	}
	evas_object_smart_callback_add(ad->win_main, "wm,rotation,changed", _rot_changed_cb, ad);

	/* load config file */
	int cfg_operation_ret = setting_cfg_init();
	/*PLUGIN_INIT(ad); */
	/*if ( ! ad->is_searchmode) */
	/*{ */
	elm_theme_extension_add(NULL, SETTING_THEME_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_NEWUX_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_NEW_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, SETTING_SLIDER_EDJ_NAME);
	/*} */

	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win_main, ELM_WIN_INDICATOR_OPAQUE);

	evas_object_show(ad->win_main);

	g_main_ad = ad;
	setting_view_create(&setting_view_main, ad);

	/* error handling */
	if (Cfg_Error_Type_Sucess != cfg_operation_ret) {
		SETTING_TRACE_ERROR("cfg_operation_ret: %d", cfg_operation_ret);
		const char *notifyStr = NULL;
		switch (cfg_operation_ret) {

			case Cfg_Error_Type_CreateCfg_Failed: {
					notifyStr = _("failed to create config file, <br>re-install org.tizen.setting please");
					break;
				}
			case Cfg_Error_Type_Mkdir_Failed: {
					notifyStr = _("file system missed<br>/opt/usr/data/setting, <br>re-install org.tizen.setting please");
					break;
				}
			case Cfg_Error_Type_RemoveCfg_Failed: {
					notifyStr = _("config file size 0 byte<br>and failed to re-create it.<br>try to startup the app again");
					break;
				}
			case Cfg_Error_Type_ReadCfg_Failed: {
					notifyStr = _("failed to read config file, <br>try to startup the app again");
					break;
				}
			case Cfg_Error_Type_DirPermissionDenied: {
					notifyStr =
					    _("the dir of config file permission denied");
					break;
				}
			case Cfg_Error_Type_FilePermissionDenied: {
					notifyStr = _("config file permission denied");
					break;
				}
			default: {
					notifyStr = _("Invalid Result, try to startup the app again");
					break;
				}
		}
		setting_create_popup_without_btn(ad, ad->win_main, NULL, (char *)notifyStr, NULL, 10, FALSE, FALSE);
		return SETTING_RETURN_FAIL;
	}


	SETTING_TRACE_DEBUG("[TIME] 4. setting_main_app_create taked %d msec ", appcore_measure_time());
	appcore_measure_start();
	LAUNCH_SETTING_OUT();
	return true;
}

/**
* The function is called when Setting is terminated
*/
static void setting_main_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = data;
	if (ad->account_subscribe_handle) {
		(void)account_unsubscribe_notification(ad->account_subscribe_handle);
	}
	vconf_set_bool(VCONFKEY_SETAPPL_ROTATE_HOLD_BOOL, FALSE);
	evas_object_smart_callback_del(ad->win_main, "wm,rotation,changed", _rot_changed_cb);

	/*PLUGIN_FINI; */
	setting_cfg_exit();
	clear_system_service_data();

	ug_destroy_all();
	ad->ug = NULL;

	//ug_create_cb(NULL , NULL);

	SETTING_TRACE_DEBUG("%s*** SETTING APPLICATION CLOSED ***%s", SETTING_FONT_BGREEN, SETTING_FONT_BLACK);
	DEREGISTER_VCONFS(ad->listened_list);


#if SUPPORT_LCD_TIMEOUT_KEEPING
	/* once Setting is not running, LCD_TIMEOUT must be backuped */
	int back_value = 0;
	vconf_get_int(VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, &back_value);
	vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, back_value);
#endif
	setting_view_destroy(&setting_view_main, ad);

	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->win_main) {
		evas_object_del(ad->win_main);
		ad->win_main = NULL;
	}

	if (ad->b) {
		bundle_free(ad->b);
		ad->b = NULL;
	}

	SETTING_TRACE_END;
	return;
}

/**
* The function is called when Setting begins run in background from forground
*/
static void setting_main_app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = data;
	if (ad->ug) {
		ug_pause();
	}

#if SUPPORT_LCD_TIMEOUT_KEEPING
	/* once Setting is running in background, LCD_TIMEOUT must be backuped */
	int back_value = 0;
	vconf_get_int(VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, &back_value);
	vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, back_value);
#endif

}

/**
* The function is called when Setting begins run in forground from background
*/
static void setting_main_app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = data;

#if SUPPORT_LCD_TIMEOUT_KEEPING
	/* once Setting is running in foreground, LCD_TIMEOUT must be reset to 600 */
	vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, 600);
#endif
	_rot_changed_cb(ad, ad->win_main, NULL);/*to fix P131121-02103 */

	if (ad->is_searchmode % 10 == Cfg_Item_AppLauncher_Node) {
		/* app-launching exit */
		elm_exit();
	}

	if (!(ad->isInUGMode && ad->ug)) {	/* top-level view is not on UG */
		SETTING_TRACE("update main genlist in resuming app without UG");
		Eina_Bool is_freezed = evas_object_freeze_events_get(ad->navibar_main);
		SETTING_TRACE_DEBUG("is_freezed : %d", is_freezed);
		if (is_freezed) {
			evas_object_freeze_events_set(ad->navibar_main, EINA_FALSE);
		}

		/* update personal mode toggle */
		Setting_GenGroupItem_Data *item_to_update;
		int status = 0;
		vconf_get_bool(VCONFKEY_SETAPPL_PERSONAL_MODE_STATUS_BOOL, &status);
		int i;
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_PersonalPage];
			setting_force_update_gl_item_chk_status(item_to_update, status);
		}
	} else if (ad->ug) {
		ug_resume();
	}
}

/**
 * The function is called by app-fwk after app_create. It always do the process which cost much time.
 */
static void setting_main_app_reset(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = data;

	if (is_searchmode_app(ad->is_searchmode)) {
		evas_object_hide(ad->view_layout);
	} else {
		evas_object_show(ad->view_layout);
	}
	vconf_callback_fn cb = NULL;

	cb = setting_int_vconf_change_cb;
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_BT_STATUS, cb, data);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_WIFI_STATE, cb, ad);

	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, cb, data);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, cb, data);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, cb, data);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SYSMAN_BATTERY_CAPACITY, cb, data);

	cb = setting_string_vconf_change_cb;
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_WIFI_CONNECTED_AP_NAME, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_SCREENMODE_SELNAME, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_BGSET, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_IDLE_LOCK_BGSET, cb, ad);

	cb = setting_other_vconf_change_cb;
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_USB_MODE_INT, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_TELEPHONY_FLIGHT_MODE, cb, ad);
#if SUPPORT_NETWORK_RESTRICTION
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, cb, ad);
#endif
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, cb, ad);
#ifndef BINARY_RELEASE_TYPE_ENG
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_DEVELOPER_OPTION_STATE, cb, ad);
#endif
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_ENHANCED_TOUCH, cb, ad);
	/*---------------------------------------------------------------------------------- */

	/*---------------------------------------------------------------------------------- */

	if (ad->win_main) {
		elm_win_activate(ad->win_main);
	}
	int flight_mode = 0;
	vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight_mode);
	if (flight_mode) {
		if (ad->data_network) setting_disable_genlist_item(ad->data_network->item);
		if (ad->data_powersaving) setting_disable_genlist_item(ad->data_powersaving->item);
	}
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	int r = 0;
	setting_main_appdata ad;

	SETTING_TRACE_DEBUG("[TIME] 1. aul_launch -> main :: Setting main : %d msec ", appcore_measure_time_from("APP_START_TIME"));
	appcore_measure_start();


	ui_app_lifecycle_callback_s ops = {
		.create = setting_main_app_create,
		.terminate = setting_main_app_terminate,
		.pause = setting_main_app_pause,
		.resume = setting_main_app_resume,
		.app_control = setting_main_app_reset,
	};
	memset(&ad, 0x00, sizeof(setting_main_appdata));

	bundle *b = NULL;
	b = bundle_import_from_argv(argc, argv);
	ad.b = b;

	SETTING_TRACE_DEBUG("[TIME] 2. main : %d msec ", appcore_measure_time());
	appcore_measure_start();
	r = ui_app_main(argc, argv, &ops, &ad);
	retv_if(r == -1, -1);

	return 0;
}
