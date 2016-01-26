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

/*///////////////////////////////////////////////////////////////////////////////////////// */
/* Player related codes - End */
/*///////////////////////////////////////////////////////////////////////////////////////// */
static void vp_del_win(void *data, Evas_Object *obj, void *event)
{
	/*ui_app_exit(); */
	elm_exit();
}

static Evas_Object *vp_create_win(const char *name, bool transparent)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *eo;
	int w = 0;
	int h = 0;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);

	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_conformant_set(eo, EINA_TRUE);

		if (transparent) {
			elm_win_alpha_set(eo, EINA_TRUE);

#ifdef ECORE_X
			unsigned int opaqueVal = 1;
			Ecore_X_Atom opaqueAtom = ecore_x_atom_get("_E_ILLUME_WINDOW_REGION_OPAQUE");
			Ecore_X_Window xwin = elm_win_xwindow_get(eo);
			ecore_x_window_prop_card32_set(xwin, opaqueAtom, &opaqueVal, 1);
#endif
		}

		evas_object_smart_callback_add(eo, "delete,request", vp_del_win, NULL);
#ifdef ECORE_X
		ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
#else
		elm_win_screen_size_get(eo, NULL, NULL, &w, &h);
#endif
		evas_object_resize(eo, w, h);

		evas_object_show(eo);
		elm_win_activate(eo);
	}

	return eo;
}

static void _vp_rot_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
}

/**
* The function is called to create Setting view widgets
*/
static bool vp_app_create(void *data)
{
	SETTING_TRACE_BEGIN;

	SettingProfileUG *ad = data;

	/* create window */
	ad->win_get = vp_create_win("org.tizen.setting.volume", TRUE);
	if (ad->win_get == NULL) {
		SETTING_TRACE("Can't create window");
		return FALSE;
	}

	UG_INIT_EFL(ad->win_get, UG_OPT_INDICATOR_DISABLE);
	ad->evas = evas_object_evas_get(ad->win_get);
	ad->viewtype = strdup(VOLUME_APP_NAME);
	setting_sound_init(ad);

	SETTING_TRACE_END;
	return TRUE;
}

/**
* The function is called when Setting is terminated
*/
static void vp_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingProfileUG *ad = data;

	SETTING_TRACE("volume_popup_terminate!");

	evas_object_smart_callback_del(ad->win_get, "wm,rotation,changed", _vp_rot_changed_cb);
	setting_sound_deinit(ad);
	if (ad->win_get) {
		evas_object_del(ad->win_get);
		ad->win_get = NULL;
	}
	SETTING_TRACE_END;
}

/**
* The function is called when Setting begins run in background from forground
*/
static void vp_app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	elm_exit();
}

/**
* The function is called when Setting begins run in forground from background
*/
static void vp_app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

/**
* The function is called by app-fwk after app_create. It always do the process which cost much time.
*/
static void vp_app_reset(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
	/*ret_if(!data); */
	/*SettingProfileUG *ad = data; */
}

static void vp_app_lang_changed(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	/*fingerprint_manager_appdata *ad = data; */
	char *lang = NULL;

	lang = vconf_get_str(VCONFKEY_LANGSET);
	if (lang) {
		elm_language_set((const char *)lang);
		FREE(lang);
	}
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SETTING_TRACE_BEGIN;
	int r = 0;
	SettingProfileUG ad;

	ui_app_lifecycle_callback_s ops = {
		.create = vp_app_create,
		.terminate = vp_app_terminate,
		.pause = vp_app_pause,

		.resume = vp_app_resume,
		.app_control = vp_app_reset,

	};

	app_event_handler_h handlers[5] = {NULL, };
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, vp_app_lang_changed, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&ad, 0x00, sizeof(SettingProfileUG));

	r = ui_app_main(argc, argv, &ops, &ad);
	SETTING_TRACE("r = %d", r);

	if (r == -1) {
		SETTING_TRACE_ERROR("ui_app_main() returns -1");
		return -1;
	}

	return 0;
}
