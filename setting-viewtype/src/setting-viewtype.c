/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <setting-viewtype.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

setting_view *__get_viewtype_view_to_load(service_h service)
{
	SETTING_TRACE_BEGIN;

	const char *viewtype = NULL;
	service_get_extra_data(service, "viewtype", &viewtype);
	if(!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "taskswitcher")) {
		return &setting_view_task_switcher_viewtype;
	} else {
		return NULL;	/* &setting_view_viewtype_main; */
	}

}

Evas_Object *__get_viewtype_layout_to_return(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingViewTypeUG *viewtypeUG = priv;
	const char *viewtype = NULL;
	service_get_extra_data(service, "viewtype", &viewtype);
	if(!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "taskswitcher")) {
		return viewtypeUG->ly_task;
	} else {
		return NULL;	/* &setting_view_viewtype_main; */
	}

}

static void setting_viewtype_ug_cb_resize(void *data, Evas *e,
					  Evas_Object *obj, void *event_info)
{
	SettingViewTypeUG *ad = (SettingViewTypeUG *) data;
	/* setting_view_update(&setting_view_about_main, ad); */
	setting_view_update(ad->view_to_load, ad);
}

static void *setting_viewtype_ug_on_create(ui_gadget_h ug,
					   enum ug_mode mode, service_h service,
					   void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingViewTypeUG *viewtypeUG = priv;
	viewtypeUG->ug = ug;

	viewtypeUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	viewtypeUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(viewtypeUG->win_main_layout);
	viewtypeUG->evas = evas_object_evas_get(viewtypeUG->win_main_layout);

	setting_retvm_if(viewtypeUG->win_main_layout == NULL, NULL,
			 "cannot get main window ");


	setting_create_Gendial_itc("dialogue/1text.1icon.2",
				   &(viewtypeUG->itc_1text_1icon_2));

	viewtypeUG->itc_seperator.item_style = "dialogue/separator";
	viewtypeUG->itc_seperator.func.text_get = NULL;
	viewtypeUG->itc_seperator.func.content_get = NULL;
	viewtypeUG->itc_seperator.func.state_get = NULL;
	viewtypeUG->itc_seperator.func.del = NULL;

	/*  creating a view. */
	/* setting_view_create(&setting_view_viewtype_main, (void *)viewtypeUG); */
	viewtypeUG->view_to_load = __get_viewtype_view_to_load(data);
	setting_retvm_if(NULL == viewtypeUG->view_to_load, NULL,
			 "NULL == viewtypeUG->view_to_load");
	setting_view_create(viewtypeUG->view_to_load, (void *)viewtypeUG);
	evas_object_event_callback_add(viewtypeUG->win_main_layout,
				       EVAS_CALLBACK_RESIZE,
				       setting_viewtype_ug_cb_resize,
				       viewtypeUG);
	return __get_viewtype_layout_to_return(data, viewtypeUG);
}

static void setting_viewtype_ug_on_start(ui_gadget_h ug, service_h service,
					 void *priv)
{
}

static void setting_viewtype_ug_on_pause(ui_gadget_h ug, service_h service,
					 void *priv)
{
}

static void setting_viewtype_ug_on_resume(ui_gadget_h ug, service_h service,
					  void *priv)
{
}

static void setting_viewtype_ug_on_destroy(ui_gadget_h ug, service_h service,
					   void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingViewTypeUG *viewtypeUG = priv;

	evas_object_event_callback_del(viewtypeUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_viewtype_ug_cb_resize);	/* fix flash issue for gallery */
	viewtypeUG->ug = ug;

	/*  called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_task_switcher_viewtype == viewtypeUG->view_to_load) {
		setting_view_destroy(&setting_view_task_switcher_viewtype,
				     viewtypeUG);
	}

	if (NULL != ug_get_layout(viewtypeUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(viewtypeUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(viewtypeUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_viewtype_ug_on_message(ui_gadget_h ug, service_h msg,
					   service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_viewtype_ug_on_event(ui_gadget_h ug,
					 enum ug_event event, service_h service,
					 void *priv)
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

static void setting_viewtype_ug_on_key_event(ui_gadget_h ug,
					     enum ug_key_event event,
					     service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

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
	SettingViewTypeUG *viewtypeUG = calloc(1, sizeof(SettingViewTypeUG));
	setting_retvm_if(!viewtypeUG, -1,
			 "Create SettingViewTypeUG obj failed");

	ops->create = setting_viewtype_ug_on_create;
	ops->start = setting_viewtype_ug_on_start;
	ops->pause = setting_viewtype_ug_on_pause;
	ops->resume = setting_viewtype_ug_on_resume;
	ops->destroy = setting_viewtype_ug_on_destroy;
	ops->message = setting_viewtype_ug_on_message;
	ops->event = setting_viewtype_ug_on_event;
	ops->key_event = setting_viewtype_ug_on_key_event;
	ops->priv = viewtypeUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingViewTypeUG *viewtypeUG;
	setting_retm_if(!ops, "ops == NULL");

	viewtypeUG = ops->priv;
	if (viewtypeUG)
		FREE(viewtypeUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/**
* Reset function to 'reset' the settings of the UG, it will be invoked by 'Reset' UG
*/
UG_MODULE_API int setting_plugin_reset(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	#if SUPPORT_RUN_SYSTEM_COMMAND
	ret += excuteCmd(SETTING_POSTINST_FILE, 1, "viewtype");
	#else
#ifdef UNUSED_VCONFKEY
	ret += vconf_set_int(VCONFKEY_TASKSWITCHER_VIEWTYPE, VCONFKEY_TASKSWITCHER_VIEWTYPE_LIST);
#endif
	#endif

	return ret;
}


