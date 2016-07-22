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
#include <setting-debug.h>
#include <app.h>
#include <ui-gadget.h>
#include <Elementary.h>
#include <Evas.h>
#include <appcore-efl.h>
#include <appcore-common.h>
#include <vconf.h>
#include <feedback.h>
#include <device.h>
#include <efl_extension.h>
#include <setting-common-general-func.h>

typedef struct _support_help_appdata {

	ui_gadget_h ug;

	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *navibar_main;
	Evas_Object *ly_main; /**< seting view main */
	app_control_h service;
	Ecore_Event_Handler *event_handler;
} support_help_appdata;

/**
 * The function is called when Setting is terminated
 */
static void support_help_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	support_help_appdata *ad = data;

	SETTING_TRACE("support_help_terminate!");

	if (ad->event_handler) {
		ecore_event_handler_del(ad->event_handler);
		ad->event_handler = NULL;
	}

	if (ad->ly_main) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}

	if (ad->win_main) {
		evas_object_del(ad->win_main);
		ad->win_main = NULL;
	}
	elm_exit();

	SETTING_TRACE_END;
}

/**
 * The event process when win object is destroyed
 */
static void support_help_del_win(void *data, Evas_Object *obj, void *event)
{
	/*ui_app_exit(); */
	elm_exit();
}

/**
 * To create a win object, the win is shared between the App and all its UGs
 */
static Evas_Object *support_help_create_win(const char *name)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *eo;
	int w, h;

	eo = elm_win_add(NULL, name, ELM_WIN_BASIC);

	if (eo) {
		elm_win_title_set(eo, name);
		elm_win_borderless_set(eo, EINA_TRUE);
		evas_object_smart_callback_add(eo, "delete,request",
				support_help_del_win, NULL);
#ifdef ECORE_X
		ecore_x_window_size_get(ecore_x_window_root_first_get(),
				&w, &h);
#else
		elm_win_screen_size_get(eo, NULL, NULL, &w, &h);
#endif
		evas_object_resize(eo, w, h);
	}

	return eo;
}

static Eina_Bool __key_press_cb(void *data, int type, void *event)
{
	SETTING_TRACE_BEGIN;
	Evas_Event_Key_Down *ev = event;
	if (!ev || !data)
		return ECORE_CALLBACK_RENEW;

	support_help_appdata *ad = data;

	if (strcmp(ev->keyname, "XF86Home") == 0)
		setting_ug_destroy(ad->ug);

	return ECORE_CALLBACK_RENEW;
}

/**
 * The function is called to create Setting view widgets
 */
static bool support_help_app_create(void *data)
{
	SETTING_TRACE_BEGIN;

	support_help_appdata *ad = data;

	SETTING_TRACE("[TIME] 3. it taked %d msec from main to setting_help_app_create ",
			appcore_measure_time());
	appcore_measure_start();

	/* create window */
	ad->win_main = support_help_create_win(
			"org.tizen.setting.helpringtone");
	if (ad->win_main == NULL) {
		SETTING_TRACE("Can't create window");
		return 0;
	}

	if (elm_win_wm_rotation_supported_get(ad->win_main)) {
		int rots[4] = { 0, 90, 180, 270 };
		elm_win_wm_rotation_available_rotations_set(ad->win_main, rots,
				4);
	}

	UG_INIT_EFL(ad->win_main, UG_OPT_INDICATOR_ENABLE);
	ad->evas = evas_object_evas_get(ad->win_main);

	elm_win_indicator_mode_set(ad->win_main, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(ad->win_main, ELM_WIN_INDICATOR_OPAQUE);

	/* call ug */
	Evas_Object *conform = elm_conformant_add(ad->win_main);
	ad->ly_main = elm_layout_add(ad->win_main);
	evas_object_size_hint_weight_set(ad->ly_main, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_layout_theme_set(ad->ly_main, "layout", "application", "default");

	/* add bg */
	Evas_Object *bg = elm_bg_add(ad->ly_main);
	elm_object_style_set(bg, "group_list");
	evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	elm_object_part_content_set(ad->ly_main, "elm.swallow.bg", bg);
	evas_object_show(bg);

	/* Indicator bg */
	Evas_Object *indicator_bg = elm_bg_add(conform);
	elm_object_style_set(indicator_bg, "indicator/headerbg");
	elm_object_part_content_set(conform, "elm.swallow.indicator_bg",
			indicator_bg);
	evas_object_show(indicator_bg);

	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(conform, EVAS_HINT_FILL,
			EVAS_HINT_FILL);
	elm_win_resize_object_add(ad->win_main, conform);
	elm_object_content_set(conform, ad->ly_main);
	evas_object_show(conform);
	elm_win_conformant_set(ad->win_main, EINA_TRUE);

	SETTING_TRACE("[TIME] 4. setting_main_app_create taked %d msec ",
			appcore_measure_time());
	appcore_measure_start();

	/* add event handler */
	ad->event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN,
			__key_press_cb, ad);

	return TRUE;
}

/**
 * The function is called when Setting begins run in background from forground
 */
static void support_help_app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	support_help_appdata *ad = data;

	if (ad->ug) {
		ug_pause();
		/*ad->ug = NULL; */
	}
}

/**
 * The function is called when Setting begins run in forground from background
 */
static void support_help_app_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	support_help_appdata *ad = data;

	if (ad->ug)
		ug_resume();
}

void support_help_app_destroy_ug_cb(ui_gadget_h ug, void *priv)
{
	support_help_appdata *ad = (support_help_appdata *)priv;
	if (!ad)
		return; /*	do nothing if ad is NULL */

	elm_win_lower(ad->win_main);

	if (ug) {
		setting_ug_destroy(ug);
		ad->ug = NULL;
	}
}

void support_help_app_end_ug_cb(ui_gadget_h ug, void *priv)
{
	if (!ug)
		return;
	support_help_appdata *ad = (support_help_appdata *)priv;
	if (!ad)
		return; /*	do nothing if ad is NULL */
	support_help_app_terminate(ad);
}

void support_help_app_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
		void *priv)
{
	Evas_Object *base;

	if (!priv)
		return;

	base = (Evas_Object *)ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		ug_disable_effect(ug);
		evas_object_show(base);
		break;
	default:
		break;
	}
}

static int _set;
EXPORT_PUBLIC void update_lang(void)
{
	char *lang;
	char *r;

	lang = vconf_get_str(VCONFKEY_LANGSET);
	if (lang) {
		setenv("LANG", lang, 1);
		setenv("LC_MESSAGES", lang, 1);
		r = setlocale(LC_ALL, "");
		if (r == NULL)
			setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));

		free(lang);
	}
}

EXPORT_PUBLIC void update_region(void)
{
	char *region;

	region = vconf_get_str(VCONFKEY_REGIONFORMAT);
	if (region) {
		setenv("LC_CTYPE", region, 1);
		setenv("LC_NUMERIC", region, 1);
		setenv("LC_TIME", region, 1);
		setenv("LC_COLLATE", region, 1);
		setenv("LC_MONETARY", region, 1);
		setenv("LC_PAPER", region, 1);
		setenv("LC_NAME", region, 1);
		setenv("LC_ADDRESS", region, 1);
		setenv("LC_TELEPHONE", region, 1);
		setenv("LC_MEASUREMENT", region, 1);
		setenv("LC_IDENTIFICATION", region, 1);
		free(region);
	}
}

static int __set_i18n(const char *domain, const char *dir)
{
	char *r;
	if (domain == NULL) {
		errno = EINVAL;
		return -1;
	}

	r = setlocale(LC_ALL, "");
	/* if locale is not set properly, try again to set as language base */
	if (r == NULL)
		r = setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));

	bindtextdomain(domain, dir);
	textdomain(domain);
	return 0;
}

int set_i18n(char *pkgname, char *localedir)
/*int set_i18n(const char *domainname, const char *dirname) */
{
	if (_set)
		return 0;
	update_lang();
	update_region();

	return __set_i18n(pkgname, localedir);
}

/**
 * The function is called by app-fwk after app_create. It always do the process
 * which cost much time.
 */
static void support_help_app_reset(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
	support_help_appdata *ad = data;

	app_control_h svc;

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs) {
		support_help_app_terminate(ad);
		return;
	}

	cbs->layout_cb = support_help_app_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = support_help_app_destroy_ug_cb;
	cbs->end_cb = support_help_app_end_ug_cb;
	cbs->priv = (void *)ad;

	if (app_control_create(&svc)) {
		FREE(cbs);
		support_help_app_terminate(ad);
		return;
	}

	app_control_set_uri(svc, "tizen-help://org.tizen.setting/ringtone");

	char *cur_ringtone_path = vconf_get_str(
			VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
	if (cur_ringtone_path == NULL)
	cur_ringtone_path = (char *)strdup(
			_TZ_SYS_SHARE"/settings/Ringtones/Over_the_horizon.ogg");
	app_control_add_extra_data(svc, "title", "IDS_ST_HEADER_RINGTONES");
	app_control_add_extra_data(svc, "domain", "setting");
	app_control_add_extra_data(svc, "path",
			_TZ_SYS_SHARE"/settings/Ringtones");
	app_control_add_extra_data(svc, "select_type", "SINGLE_FILE");
	app_control_add_extra_data(svc, "file_type", "SOUND");
	app_control_add_extra_data(svc, "drm_type", "DRM_ALL");
	app_control_add_extra_data(svc, "silent", "silent_show");
	if (safeStrCmp(cur_ringtone_path, "silent") == 0)
		app_control_add_extra_data(svc, "marked_mode", "silent");
	else
		app_control_add_extra_data(svc, "marked_mode",
				cur_ringtone_path);
	ad->ug = setting_ug_create(NULL, "myfile-efl", UG_MODE_FULLVIEW, svc,
			cbs);

	app_control_destroy(svc);

	if (!ad->ug) {
		evas_object_show(ad->ly_main);
		if (cbs) {
			FREE(cbs);
			FREE(cur_ringtone_path);
			support_help_app_terminate(ad);
			return;
		}
	}

	if (ad->win_main) {
		evas_object_show(ad->win_main);
		elm_win_activate(ad->win_main);
	}
	if (cbs)
		FREE(cbs);

	FREE(cur_ringtone_path);
}

static void support_help_app_lang_changed(app_event_info_h event_info,
		void *data)
{
	SETTING_TRACE_BEGIN;
	support_help_appdata *ad = data;
	char *lang = NULL;

	lang = vconf_get_str(VCONFKEY_LANGSET);
	if (lang) {
		elm_language_set((const char *)lang);
		FREE(lang);
	}

	if (ad->ug)
		ug_send_event(UG_EVENT_LANG_CHANGE);
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SETTING_TRACE_BEGIN;
	int r = 0;
	support_help_appdata ad;

	ui_app_lifecycle_callback_s ops = {
			.create = support_help_app_create,
			.terminate = support_help_app_terminate,
			.pause = support_help_app_pause,
			.resume = support_help_app_resume,
			.app_control = support_help_app_reset, };

	app_event_handler_h handlers[5] = { NULL, };
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED,
			support_help_app_lang_changed, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&ad, 0x00, sizeof(support_help_appdata));

	r = ui_app_main(argc, argv, &ops, &ad);
	SETTING_TRACE_DEBUG("r = %d", r);

	if (r == -1) {
		SETTING_TRACE_ERROR("ui_app_main() returns -1");
		return -1;
	}

	return 0;
}
