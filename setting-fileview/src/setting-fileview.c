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
 *@defgroup setting-fileview
 *UG creation code for setting-fileview
 * html file viewer
 */

#include <setting-fileview.h>
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

bool __parse_ug_argument(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingFileviewUG *ad = priv;
	ad->input_file = NULL;

	app_control_get_extra_data(service, "file", &(ad->input_file));
	setting_retvm_if(!ad->input_file, FALSE,
			"no arguement to specialize file");

	ad->input_title = NULL;

	app_control_get_extra_data(service, "title", &(ad->input_title));
	setting_retvm_if(!ad->input_title, FALSE, "no arguement to title");
	SETTING_TRACE("ad->input_file:%s", ad->input_file);
	SETTING_TRACE("ad->input_title:%s", ad->input_title);
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
static void setting_fileview_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingFileviewUG *ad = (SettingFileviewUG *)data;
	setting_view_update(&setting_view_fileview_main, ad);
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
static void *setting_fileview_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingFileviewUG *fileviewUG = priv;
	fileviewUG->ug = ug;

	fileviewUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	fileviewUG->win_get = (Evas_Object *)ug_get_window();
	evas_object_show(fileviewUG->win_main_layout);
	fileviewUG->evas = evas_object_evas_get(fileviewUG->win_main_layout);

	setting_retvm_if(fileviewUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	/*	creating a view. */
	if (!__parse_ug_argument(service, priv)) {
		SETTING_TRACE_ERROR("Invalid arguement");
		return NULL;
	}
	setting_view_create(&setting_view_fileview_main, (void *)fileviewUG);
	evas_object_event_callback_add(fileviewUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_fileview_ug_cb_resize,
			fileviewUG);
	return fileviewUG->ly_main;
}

static void setting_fileview_ug_on_start(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_fileview_ug_on_pause(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_fileview_ug_on_resume(ui_gadget_h ug, app_control_h service,
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
static void setting_fileview_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingFileviewUG *fileviewUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(fileviewUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_fileview_ug_cb_resize);
	fileviewUG->ug = ug;
	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_fileview_main, fileviewUG);

	if (fileviewUG->input_file)
		FREE(fileviewUG->input_file);
	if (fileviewUG->input_title)
		FREE(fileviewUG->input_title);

	if (NULL != ug_get_layout(fileviewUG->ug)) {
		evas_object_hide((Evas_Object *)ug_get_layout(fileviewUG->ug));
		evas_object_del((Evas_Object *)ug_get_layout(fileviewUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_fileview_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_fileview_ug_on_event(ui_gadget_h ug, enum ug_event event,
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

static void setting_fileview_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingFileviewUG *ad = (SettingFileviewUG *)priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
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

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingFileviewUG *fileviewUG = calloc(1, sizeof(SettingFileviewUG));
	setting_retvm_if(!fileviewUG, -1,
			"Create SettingFileviewUG obj failed");

	ops->create = setting_fileview_ug_on_create;
	ops->start = setting_fileview_ug_on_start;
	ops->pause = setting_fileview_ug_on_pause;
	ops->resume = setting_fileview_ug_on_resume;
	ops->destroy = setting_fileview_ug_on_destroy;
	ops->message = setting_fileview_ug_on_message;
	ops->event = setting_fileview_ug_on_event;
	ops->key_event = setting_fileview_ug_on_key_event;
	ops->priv = fileviewUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingFileviewUG *fileviewUG;
	setting_retm_if(!ops, "ops == NULL");

	fileviewUG = ops->priv;
	if (fileviewUG)
		FREE(fileviewUG);
}

/* ***************************************************
 **
 **general func
 **
 ****************************************************/

