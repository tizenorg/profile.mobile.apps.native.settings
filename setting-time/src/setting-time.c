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

#include <setting-time.h>
#include <setting-cfg.h>
#include <unicode/uloc.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

/**
* @callback handler for EVAS_CALLBACK_RESIZE
*
* @param data user data (SettingTimeUG *)
*/
static void setting_time_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) data;
	setting_view_update(&setting_view_time_main, ad);
}

/**
* @callback function called first while loading setting-time UG.
*
* @param ug ug object itself.
* @param mode
* @param data bundle (key, value pair) data as arguments into UG.
* @param priv SettingTimeUG context
*
* @return
*/
static void *setting_time_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingTimeUG *timeUG = priv;
	timeUG->ug = ug;
	timeUG->bundle_data = service;
	timeUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	timeUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(timeUG->win_main_layout);
	timeUG->evas = evas_object_evas_get(timeUG->win_main_layout);

	timeUG->pop_progress = NULL;
	/*	Initialize Caller value */
	timeUG->caller = NULL;

	setting_retvm_if(timeUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	/* set launguage */
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	elm_theme_extension_add(NULL, SETTING_GENLIST_EDJ_NAME);
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_time_main, NULL);

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(timeUG->itc_1text_1icon));
	setting_create_Gendial_itc("entry.main", &(timeUG->itc_layout));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(timeUG->itc_1icon_1text_sub));

	timeUG->is_expand_time_format_list = 0;

	char *viewtype = NULL;
	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!safeStrCmp(viewtype, "format_expand"))
		timeUG->is_expand_time_format_list = 1;

	/*	creating a view. */
	setting_view_node_set_cur_view(&setting_view_time_main);
	setting_view_create(&setting_view_time_main, (void *)timeUG);
	evas_object_event_callback_add(timeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE,
			setting_time_ug_cb_resize, timeUG);
	return timeUG->ly_main;
}

static void setting_time_ug_on_start(ui_gadget_h ug, app_control_h service,
			void *priv)
{
	SETTING_TRACE_BEGIN;
	/*	do nothing */
}

static void setting_time_ug_on_pause(ui_gadget_h ug, app_control_h service,
			void *priv)
{
	SETTING_TRACE_BEGIN;
	/*	do nothing */
}

static void setting_time_ug_on_resume(ui_gadget_h ug, app_control_h service,
			void *priv)
{
	/*	do nothing */
}

static void setting_time_ug_on_destroy(ui_gadget_h ug, app_control_h service,
			void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingTimeUG *timeUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(timeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_time_ug_cb_resize);
	timeUG->ug = ug;

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_time_main, timeUG);

	if (NULL != ug_get_layout(timeUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(timeUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(timeUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_time_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_time_ug_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) priv;

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE: {
			const char *pa_lang = vconf_get_str(VCONFKEY_LANGSET);
			if (pa_lang) {
				char *q = strchr(pa_lang, '.');
				if (q)
					*q = '\0';
			}
			int err = -1;

			uloc_setDefault(pa_lang, &err);
			FREE(pa_lang);
			setting_view_update(&setting_view_time_main, ad);
		}
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

/**
* @brief key event handler for UG
*/
static void setting_time_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
#if 0
			if (0 == safeStrCmp(ad->caller, "pwlock")) {
				/*	Create Bundle and send message */

				app_control_h svc;
				if (app_control_create(&svc))
					return;

				app_control_add_extra_data(svc, "result",
						"lbutton_click");
				ug_send_result(ad->ug, svc);

				app_control_destroy(svc);
			}
#endif

			if (elm_naviframe_top_item_get(ad->navi_bar) ==
				elm_naviframe_bottom_item_get(ad->navi_bar)) {
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

/**
* @brief UG entry point - create the SettingTimeUG context
* @return 0
*/
UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *timeUG = calloc(1, sizeof(SettingTimeUG));
	setting_retvm_if(!timeUG, -1, "Create SettingTimeUG obj failed");

	ops->create = setting_time_ug_on_create;
	ops->start = setting_time_ug_on_start;
	ops->pause = setting_time_ug_on_pause;
	ops->resume = setting_time_ug_on_resume;
	ops->destroy = setting_time_ug_on_destroy;
	ops->message = setting_time_ug_on_message;
	ops->event = setting_time_ug_on_event;
	ops->key_event = setting_time_ug_on_key_event;
	ops->priv = timeUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

/**
* @brief UG finalizer - destroy the SettingTimeUG context
*/
UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingTimeUG *timeUG;
	setting_retm_if(!ops, "ops == NULL");

	timeUG = ops->priv;
	if (timeUG)
		FREE(timeUG);
}

