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
 *@defgroup setting-datausage
 *UG creation code for setting-datausage
 */

#include <setting-datausage.h>
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

setting_view *__get_datausage_view_to_load(service_h service)
{
	SETTING_TRACE_BEGIN;

	setting_view_node_table_register(&setting_view_datausage_main, NULL);

	return &setting_view_datausage_main;
}

static void setting_datausage_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
				      void *event_info)
{
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	setting_view_update(ad->view_to_load, ad);
}

static void *setting_datausage_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
				       service_h service, void *priv)
{
	setting_retvm_if((!ug || !priv), NULL, "!ug || !priv");

	SettingDatausageUG *datausageUG = priv;
	datausageUG->ug = ug;
	datausageUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	datausageUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(datausageUG->win_main_layout);
	datausageUG->evas = evas_object_evas_get(datausageUG->win_main_layout);

	setting_retvm_if(datausageUG->win_main_layout == NULL, NULL,
			 "cannot get main window ");

	datausageUG->itc_seperator.item_style = "dialogue/separator";
	datausageUG->itc_seperator.func.text_get = NULL;
	datausageUG->itc_seperator.func.content_get = NULL;
	datausageUG->itc_seperator.func.state_get = NULL;
	datausageUG->itc_seperator.func.del = NULL;

	setting_create_Gendial_itc("dialogue/2text.3/expandable", &(datausageUG->itc_2text_3_parent));
	setting_create_Gendial_itc("dialogue/1text.1icon/expandable2", &(datausageUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc("dialogue/2text.3", &(datausageUG->itc_2text_2));
	setting_create_Gendial_itc("dialogue/1text.1icon.2", &(datausageUG->itc_1text_1icon_2));
	setting_create_Gendial_itc("dialogue/1text", &(datausageUG->itc_1text));
	setting_create_Gendial_itc("dialogue/1text.1icon", &(datausageUG->itc_1text_1icon));
	setting_create_Gendial_itc("dialogue/title", &(datausageUG->itc_group_item));
	setting_create_Gendial_itc("multiline/1text", &(datausageUG->itc_bg_1icon));

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/*  creating a view. */
	datausageUG->view_to_load = __get_datausage_view_to_load(service);
	setting_retvm_if(NULL == datausageUG->view_to_load, NULL,
			 "NULL == datausageUG->view_to_load");
	setting_view_node_set_cur_view(datausageUG->view_to_load);
	setting_view_create(datausageUG->view_to_load, (void *)datausageUG);
	evas_object_event_callback_add(datausageUG->win_main_layout,
				       EVAS_CALLBACK_RESIZE,
				       setting_datausage_ug_cb_resize, datausageUG);
	return datausageUG->ly_main;
}

static void setting_datausage_ug_on_start(ui_gadget_h ug, service_h service,
				     void *priv)
{
}

static void setting_datausage_ug_on_pause(ui_gadget_h ug, service_h service,
				     void *priv)
{
}

static void setting_datausage_ug_on_resume(ui_gadget_h ug, service_h service,
				      void *priv)
{
}

static void setting_datausage_ug_on_destroy(ui_gadget_h ug, service_h service,
				       void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!ug || !priv), "!ug || !priv");
	SettingDatausageUG *datausageUG = priv;

	evas_object_event_callback_del(datausageUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_datausage_ug_cb_resize);	/* fix flash issue for gallery */
	datausageUG->ug = ug;

	/*  delete the allocated objects. */
	setting_view_destroy(&setting_view_datausage_main, datausageUG);
	if (NULL != ug_get_layout(datausageUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(datausageUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(datausageUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_datausage_ug_on_message(ui_gadget_h ug, service_h msg,
				       service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_datausage_ug_on_event(ui_gadget_h ug, enum ug_event event,
				     service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!ug || !priv), "!ug || !priv");
	SettingDatausageUG *datausageUG = priv;

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		{
			if (datausageUG->graph_desc) {
				elm_object_item_data_set(datausageUG->graph_desc->item, datausageUG->graph_desc);
				elm_genlist_item_update(datausageUG->graph_desc->item);
			}
		}
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		{
			if (datausageUG->graph_desc) {
				elm_object_item_data_set(datausageUG->graph_desc->item, datausageUG->graph_desc);
				elm_genlist_item_update(datausageUG->graph_desc->item);
			}
		}
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_datausage_ug_on_key_event(ui_gadget_h ug,
					 enum ug_key_event event, service_h service,
					 void *priv)
{
	SETTING_TRACE_BEGIN;
	if (!ug)
		return;

	switch (event) {
	case UG_KEY_EVENT_END:
		ug_destroy_me(ug);
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingDatausageUG *datausageUG = calloc(1, sizeof(SettingDatausageUG));
	setting_retvm_if(!datausageUG, -1, "Create SettingDatausageUG obj failed");

	ops->create = setting_datausage_ug_on_create;
	ops->start = setting_datausage_ug_on_start;
	ops->pause = setting_datausage_ug_on_pause;
	ops->resume = setting_datausage_ug_on_resume;
	ops->destroy = setting_datausage_ug_on_destroy;
	ops->message = setting_datausage_ug_on_message;
	ops->event = setting_datausage_ug_on_event;
	ops->key_event = setting_datausage_ug_on_key_event;
	ops->priv = datausageUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingDatausageUG *datausageUG = NULL;
	setting_retm_if(!ops, "ops == NULL");

	datausageUG = ops->priv;
	FREE(datausageUG);
}
