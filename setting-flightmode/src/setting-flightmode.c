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
#include <setting-flightmode.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_flightmode_ug_cb_resize(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	SettingFlightModeUG *ad = (SettingFlightModeUG *)data;
	setting_view_update(&setting_view_flightmode_main, ad);
}

setting_view *__get_flightmode_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "data is NULL");

	setting_view_node_table_intialize();

	/* default action */
	setting_view_node_table_register(&setting_view_flightmode_main, NULL);
	return &setting_view_flightmode_main;
}

static void *setting_flightmode_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");
	SettingFlightModeUG *flightmodeUG = priv;
	flightmodeUG->ug = ug;
	flightmodeUG->bundle_data = service;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	flightmodeUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	flightmodeUG->win_get = (Evas_Object *)ug_get_window();

	evas_object_show(flightmodeUG->win_main_layout);
	flightmodeUG->evas = evas_object_evas_get(
			flightmodeUG->win_main_layout);

	setting_retvm_if(flightmodeUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	/* register view node table */
	flightmodeUG->view_to_load = __get_flightmode_view_to_load(flightmodeUG,
			service);
	setting_retvm_if(NULL == flightmodeUG->view_to_load, NULL,
			"NULL == flightmodeUG->view_to_load");

	setting_create_Gendial_itc("dialogue/1text.2icon",
			&(flightmodeUG->itc_1text_2icon));

	/* creating a view. */
	setting_view_node_set_cur_view(flightmodeUG->view_to_load);
	setting_view_create(flightmodeUG->view_to_load, (void *)flightmodeUG);

	evas_object_event_callback_add(flightmodeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_flightmode_ug_cb_resize,
			flightmodeUG);

	/*---------------------------------------------------------------- */
	/* add genlist */
	/*---------------------------------------------------------------- */
	return flightmodeUG->ly_main;
}

static void setting_flightmode_ug_on_start(ui_gadget_h ug,
		app_control_h service, void *priv)
{
}

static void setting_flightmode_ug_on_pause(ui_gadget_h ug,
		app_control_h service, void *priv)
{
}

static void setting_flightmode_ug_on_resume(ui_gadget_h ug,
		app_control_h service, void *priv)
{
}

static void setting_flightmode_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingFlightModeUG *flightmodeUG = priv;
	/*fix flash issue for gallery */
	evas_object_event_callback_del(flightmodeUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_flightmode_ug_cb_resize);
	flightmodeUG->ug = ug;
	if (flightmodeUG->bg)
		evas_object_del(flightmodeUG->bg);

	flightmodeUG->bg = NULL;

	/* called when this shared gadget is terminated. similar with
	 * app_exit */
	setting_view_destroy(&setting_view_flightmode_main, flightmodeUG);

	if (NULL != ug_get_layout(flightmodeUG->ug)) {
		evas_object_hide(
				(Evas_Object *)ug_get_layout(flightmodeUG->ug));
		evas_object_del((Evas_Object *)ug_get_layout(flightmodeUG->ug));
	}
}

static void setting_flightmode_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_flightmode_ug_on_event(ui_gadget_h ug, enum ug_event event,
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

static void setting_flightmode_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingFlightModeUG *ad = (SettingFlightModeUG *)priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
		if (elm_naviframe_top_item_get(ad->navi_bar) ==
				elm_naviframe_bottom_item_get(ad->navi_bar)) {
			ug_destroy_me(ug);
		} else {

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
	SettingFlightModeUG *flightmodeUG = calloc(1,
			sizeof(SettingFlightModeUG));
	setting_retvm_if(!flightmodeUG, -1,
			"Create SettingFlightModeUG obj failed");

	ops->create = setting_flightmode_ug_on_create;
	ops->start = setting_flightmode_ug_on_start;
	ops->pause = setting_flightmode_ug_on_pause;
	ops->resume = setting_flightmode_ug_on_resume;
	ops->destroy = setting_flightmode_ug_on_destroy;
	ops->message = setting_flightmode_ug_on_message;
	ops->event = setting_flightmode_ug_on_event;
	ops->key_event = setting_flightmode_ug_on_key_event;
	ops->priv = flightmodeUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;
	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingFlightModeUG *flightmodeUG;
	setting_retm_if(!ops, "ops == NULL");

	flightmodeUG = ops->priv;
	if (flightmodeUG)
		FREE(flightmodeUG);
}

/* ***************************************************
 **
 ** general func
 **
 ** **************************************************/

/**
 * Reset function to 'reset' the settings of the UG, it will be invoked by
 * 'Reset' UG
 *
 * @param[in] data
 * @param[in] priv
 */

