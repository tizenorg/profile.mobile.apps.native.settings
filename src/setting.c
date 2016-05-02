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
#include "setting.h"
#include "setting-helper.h"
#include "setting-main.h"
#include "setting-cfg.h"
#include "setting-common-init.h"
#include "setting-common-search.h"

#include <app.h>
#include <sound_manager.h>
#include <appcore-common.h>
#include <sensor.h>
#include <aul.h>
#include <app_preference.h>
#include <elm_object.h>
#include <appsvc.h>
#include <signal.h>
#include <system_settings.h>
#include <bundle_internal.h>

/*#define SUPPORT_UG_MESSAGE */

#define SETTING_SOUND_VOL_MAX 15
#define SETTING_DEFAULT_RINGTONE_VOL_INT	11
#define SETTING_DEFAULT_NOTI_VOL_INT		11
#define SETTING_DEFAULT_MEDIA_VOL_INT		9

int g_geometry_x, g_geometry_y, g_geometry_w, g_geometry_h;

setting_main_appdata *g_main_ad;

void termination_handler(int signum)
{
	SETTING_TRACE_BEGIN;
	/* do something for signal handling */
	/*SETTING_TRACE_DEBUG(">>>>>>>>>>>>>>>>> SIGTERM >>>>>>>>>>>>>>> SETTING "); */

	elm_exit();
}


#if LOW_BATTERY_DO_NOTHING

/*	in case of low battery, don't terminate itself.*/

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
	if (ad == NULL || ad->md.win_main == NULL) {
		return;
	}
	int change_ang = elm_win_rotation_get(ad->md.win_main);
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
	SETTING_TRACE_DEBUG("diff:%d", elm_win_rotation_get(ad->md.win_main) - ad->current_rotation);

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

	elm_object_item_part_text_set(ad->md.navibar_main_it, "elm.text.title", _("IDS_ST_OPT_SETTINGS"));
	/*setting_navi_items_update(ad->md.navibar_main); */

	char *localeLanguage = NULL;
	system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &localeLanguage);
	elm_language_set(localeLanguage);
	free(localeLanguage);

	if (ad->isInUGMode && ad->ug) {
		/* app launching */
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
* the event process when int VCONFS changes
* @todo code clean - it has big if-else structure
*/
static void setting_int_vconf_change_cb(keynode_t *key, void *data)
{
	/* setting_main_appdata *ad = data; */
	retm_if(!data, "Invalid argument: data is NULL");

	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("Enter %s(%s=%d)", __FUNCTION__, vconf_name, status);
	return;
}


/**
* the event process when string VCONFS changes
*/
static void setting_string_vconf_change_cb(keynode_t *key, void *data)
{
	/* setting_main_appdata *ad = data; */
	retm_if(!data, "Invalid argument: data is NULL");

	char *value = vconf_keynode_get_str(key);
	char *vconf_name = vconf_keynode_get_name(key);
	SETTING_TRACE("Enter %s(%s=%s)", __FUNCTION__, vconf_name, value);

}

/**
* The function is called to create Setting view widgets
*/
static bool setting_main_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	LAUNCH_SETTING_IN();
	SETTING_TRACE_DEBUG("[TIME] 3. it took %d msec from main to setting_main_app_create ", appcore_measure_time());
	appcore_measure_start();

	/*elm_config_accel_preference_set("3d"); */

#if SUPPORT_DRI
	setenv("EVAS_DRI_SWAPBUF", "1", 1);
#endif

	setting_main_appdata *ad = data;
	/* regitering sigterm */
	if (signal(SIGTERM, termination_handler) == SIG_IGN) {
		signal(SIGTERM, SIG_IGN);
	}

	app_event_handler_h handlers[5] = {NULL, };
#if LOW_BATTERY_DO_NOTHING
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, setting_main_low_battery_cb, ad);
#endif
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

	if (app_init(&ad->md, SETTING_PACKAGE) != SETTING_RETURN_SUCCESS)
		return false;

	UG_INIT_EFL(ad->md.win_main, UG_OPT_INDICATOR_ENABLE);
	ad->current_rotation = elm_win_rotation_get(ad->md.win_main);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d", ad->current_rotation);
	if (elm_win_wm_rotation_supported_get(ad->md.win_main)) {
		int rots[4] = { 0, 90, 180, 270 };	/* rotation value that app may want */
		elm_win_wm_rotation_available_rotations_set(ad->md.win_main, rots, 4);
	}
	evas_object_smart_callback_add(ad->md.win_main, "wm,rotation,changed", _rot_changed_cb, ad);

	/* load config file */
	int cfg_operation_ret = setting_cfg_init();

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
				notifyStr = _("file system missed<br>"_TZ_SYS_DATA"/setting, <br>re-install org.tizen.setting please");
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
		case Cfg_Error_Type_OutOfMemory: {
				notifyStr = _("out of memory");
				break;
			}
		default: {
				notifyStr = _("Invalid Result, try to startup the app again");
				break;
			}
		}
		setting_create_popup(ad, ad->md.win_main, NULL, (char *)notifyStr, NULL, 10, FALSE, FALSE, 0);
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
	vconf_set_bool(VCONFKEY_SETAPPL_ROTATE_HOLD_BOOL, FALSE);
	evas_object_smart_callback_del(ad->md.win_main, "wm,rotation,changed", _rot_changed_cb);

#if 1
	/*PLUGIN_FINI; */
	setting_cfg_exit();
	clear_system_service_data();

	ug_destroy_all();
	ad->ug = NULL;

#endif
	SETTING_TRACE("*** SETTING APPLICATION CLOSED ***");
	DEREGISTER_VCONFS(ad->listened_list);
	setting_view_destroy(&setting_view_main, ad);

	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
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
}

/**
* The function is called when Setting begins run in forground from background
*/
static void setting_main_app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = data;

	_rot_changed_cb(ad, ad->md.win_main, NULL);/*to fix P131121-02103 */

	if (ad->is_searchmode % 10 == Cfg_Item_AppLauncher_Node) {
		/* app-launching exit */
		elm_exit();
	}

	if (!(ad->isInUGMode && ad->ug)) {	/* top-level view is not on UG */
		SETTING_TRACE("update main genlist in resuming app without UG");
		Eina_Bool is_freezed = evas_object_freeze_events_get(ad->md.navibar_main);
		SETTING_TRACE_DEBUG("is_freezed : %d", is_freezed);
		if (is_freezed) {
			evas_object_freeze_events_set(ad->md.navibar_main, EINA_FALSE);
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
		evas_object_hide(ad->md.view_layout);
	} else {
		evas_object_show(ad->md.view_layout);
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

	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_USB_MODE_INT, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_TELEPHONY_FLIGHT_MODE, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, cb, ad);
	REGISTER_VCONF_NODE(ad->listened_list, VCONFKEY_SETAPPL_VIBRATION_STATUS_BOOL, cb, ad);

	/*---------------------------------------------------------------------------------- */

	/*---------------------------------------------------------------------------------- */

	if (ad->md.win_main) {
		elm_win_activate(ad->md.win_main);
	}

	/* Disable data_network if flight mode is ON */
	int flight_mode = 0;
	vconf_get_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, &flight_mode);
	if (flight_mode) {
		if (ad->data_network) setting_disable_genlist_item(ad->data_network->item);
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

	SETTING_TRACE_DEBUG("[TIME] 2. main : %d msec ", appcore_measure_time());
	appcore_measure_start();
	r = ui_app_main(argc, argv, &ops, &ad);
	retv_if(r == -1, -1);

	return 0;
}
