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

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

bool __parse_ug_argument(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingRingtoneUG *ad = priv;

	ad->file_path = NULL;
	app_control_get_extra_data(service, "marked_mode", &(ad->file_path));
	setting_retvm_if(!ad->file_path, FALSE,
			"no arguement to specialize file");

	ad->dir_path = NULL;
	app_control_get_extra_data(service, "path", &(ad->dir_path));
	setting_retvm_if(!ad->dir_path, FALSE,
			"no arguement to specialize file");

	char *silent = NULL;
	app_control_get_extra_data(service, "silent", &silent);
	if (!safeStrCmp(silent, "silent show"))
		ad->is_show_silent = 1;

	char *default_str = NULL;
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
static void *setting_ringtone_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);

		setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingRingtoneUG *ringtoneUG = priv;
	ringtoneUG->ug = ug;

	ringtoneUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	ringtoneUG->win_get = (Evas_Object *)ug_get_window();
	evas_object_show(ringtoneUG->win_main_layout);
	ringtoneUG->evas = evas_object_evas_get(ringtoneUG->win_main_layout);

	setting_retvm_if(ringtoneUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	/*	creating a view. */
	if (!__parse_ug_argument(service, priv)) {
		SETTING_TRACE_ERROR("Invalid arguement");
		return NULL;
	}

	if (0 != app_control_clone(&(ringtoneUG->source_svc), service))
		SETTING_TRACE_ERROR("failed to clone svc");

	setting_view_create(&setting_view_ringtone_main, (void *)ringtoneUG);
	evas_object_event_callback_add(ringtoneUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_ringtone_ug_cb_resize,
			ringtoneUG);
	return ringtoneUG->ly_main;
}

static void setting_ringtone_ug_on_start(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_ringtone_ug_on_pause(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_ringtone_ug_on_resume(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
}

/**
 * on_destroy function of the UG
 *
 * @param ug
 * @param data
 * @param priv
 */
static void setting_ringtone_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingRingtoneUG *ringtoneUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ringtoneUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_ringtone_ug_cb_resize);
	ringtoneUG->ug = ug;
	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_ringtone_main, ringtoneUG);

	if (ringtoneUG->source_svc) {
		app_control_destroy(ringtoneUG->source_svc);
		ringtoneUG->source_svc = NULL;
	}

	if (NULL != ug_get_layout(ringtoneUG->ug)) {
		evas_object_hide((Evas_Object *)ug_get_layout(ringtoneUG->ug));
		evas_object_del((Evas_Object *)ug_get_layout(ringtoneUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_ringtone_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_ringtone_ug_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_ringtone_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingRingtoneUG *ad = (SettingRingtoneUG *)priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
		if (elm_naviframe_top_item_get(ad->navi_bar)
				== elm_naviframe_bottom_item_get(ad->navi_bar)) {
			ug_destroy_me(ug);
		} else {
			/* elm_naviframe_item_pop(ad->navi_bar); */
			setting_view_cb_at_endKey(ad);
		}
	}
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingRingtoneUG *ringtoneUG = calloc(1, sizeof(SettingRingtoneUG));
	setting_retvm_if(!ringtoneUG, -1,
			"Create SettingRingtoneUG obj failed");

	ops->create = setting_ringtone_ug_on_create;
	ops->start = setting_ringtone_ug_on_start;
	ops->pause = setting_ringtone_ug_on_pause;
	ops->resume = setting_ringtone_ug_on_resume;
	ops->destroy = setting_ringtone_ug_on_destroy;
	ops->message = setting_ringtone_ug_on_message;
	ops->event = setting_ringtone_ug_on_event;
	ops->key_event = setting_ringtone_ug_on_key_event;
	ops->priv = ringtoneUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingRingtoneUG *ringtoneUG;
	setting_retm_if(!ops, "ops == NULL");

	ringtoneUG = ops->priv;
	if (ringtoneUG)
		FREE(ringtoneUG);
}

/* ***************************************************
 **
 **general func
 **
 ****************************************************/

