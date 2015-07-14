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

/**
 *@defgroup setting-memory
 *UG creation code for setting-memory
 */

#include <setting-memory.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_memory_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
					void *event_info)
{
	SettingMemoryUG *ad = (SettingMemoryUG *) data;
	setting_view_update(&setting_view_memory_default_memory, ad);
}

static void *setting_memory_ug_on_create(ui_gadget_h ug,
					 enum ug_mode mode, service_h service,
					 void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingMemoryUG *memoryUG = priv;
	memoryUG->ug = ug;

	memoryUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	memoryUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(memoryUG->win_main_layout);
	memoryUG->evas = evas_object_evas_get(memoryUG->win_main_layout);

	setting_retvm_if(memoryUG->win_main_layout == NULL, NULL,
			 "cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();

	setting_view_node_table_register(&setting_view_memory_default_memory, NULL);

	setting_create_Gendial_itc("dialogue/1text.1icon.2",
				   &(memoryUG->itc_1text_1icon_2));
	setting_create_Gendial_itc("dialogue/title",
				   &(memoryUG->itc_group_item));
	setting_create_Gendial_itc("dialogue/1text", &(memoryUG->itc_1text));
	setting_create_Gendial_itc("1text.1icon.2",
				   &(memoryUG->itc_1text_1icon));
	setting_create_Gendial_itc("select_all", &(memoryUG->itc_sel_all));

	setting_create_Gendial_itc("dialogue/2text.3/expandable",
				   &(memoryUG->itc_2text_3_parent));
	setting_create_Gendial_itc("dialogue/1text.1icon/expandable2",
				   &(memoryUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc("dialogue/2text.2",
				   &(memoryUG->itc_2text_2));

	memoryUG->itc_seperator.item_style = "dialogue/separator";
	memoryUG->itc_seperator.func.text_get = NULL;
	memoryUG->itc_seperator.func.content_get = NULL;
	memoryUG->itc_seperator.func.state_get = NULL;
	memoryUG->itc_seperator.func.del = NULL;

	SETTING_TRACE_DEBUG("[DBG] set current view");

	/*  creating a view. */
	setting_view_node_set_cur_view(&setting_view_memory_default_memory);
	setting_view_create(&setting_view_memory_default_memory, (void *)memoryUG);
	evas_object_event_callback_add(memoryUG->win_main_layout,
				       EVAS_CALLBACK_RESIZE,
				       setting_memory_ug_cb_resize, memoryUG);
	return memoryUG->ly_main;
}

static void setting_memory_ug_on_start(ui_gadget_h ug, service_h service,
				       void *priv)
{
}

static void setting_memory_ug_on_pause(ui_gadget_h ug, service_h service,
				       void *priv)
{
}

static void setting_memory_ug_on_resume(ui_gadget_h ug, service_h service,
					void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!ug || !priv), "!ug || !priv");
	//SettingMemoryUG *ad = priv;

	// Need to update the mem. status view in resuming the view

	SETTING_TRACE_END;
}

static void setting_memory_ug_on_destroy(ui_gadget_h ug, service_h service,
					 void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingMemoryUG *memoryUG = priv;

	evas_object_event_callback_del(memoryUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_memory_ug_cb_resize);	/* fix flash issue for gallery */
	memoryUG->ug = ug;

	/*  delete the allocated objects. */

	if (TRUE == setting_view_memory_default_memory.is_create) {
		setting_view_destroy(&setting_view_memory_default_memory, memoryUG);
	}

	if (NULL != ug_get_layout(memoryUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(memoryUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(memoryUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_memory_ug_on_message(ui_gadget_h ug, service_h msg,
					 service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_memory_ug_on_event(ui_gadget_h ug,
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

static void setting_memory_ug_on_key_event(ui_gadget_h ug,
					   enum ug_key_event event,
					   service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	if (!ug)
		return;
	SettingMemoryUG *ad = (SettingMemoryUG *) priv;

	switch (event) {
	case UG_KEY_EVENT_END:
		{
			if (ad->wait_timer) {
				ecore_timer_del(ad->wait_timer);
				ad->wait_timer = NULL;
			}

			if (ad->progress_bar_timer) {
				ecore_timer_del(ad->progress_bar_timer);
				ad->progress_bar_timer = NULL;
			}
			if (ad->popup_timer) {
				ecore_timer_del(ad->popup_timer);
				ad->popup_timer = NULL;
			}

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
	SettingMemoryUG *memoryUG = calloc(1, sizeof(SettingMemoryUG));
	setting_retvm_if(!memoryUG, -1, "Create SettingMemoryUG obj failed");

	ops->create = setting_memory_ug_on_create;
	ops->start = setting_memory_ug_on_start;
	ops->pause = setting_memory_ug_on_pause;
	ops->resume = setting_memory_ug_on_resume;
	ops->destroy = setting_memory_ug_on_destroy;
	ops->message = setting_memory_ug_on_message;
	ops->event = setting_memory_ug_on_event;
	ops->key_event = setting_memory_ug_on_key_event;
	ops->priv = memoryUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingMemoryUG *memoryUG;
	setting_retm_if(!ops, "ops == NULL");

	memoryUG = ops->priv;
	if (memoryUG)
		FREE(memoryUG);
}

/**
* Reset function to 'reset' the settings of the UG, it will be invoked by 'Reset' UG
*
* @param[in] data
* @param[in] priv
*/
UG_MODULE_API int setting_plugin_reset(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
#if SUPPORT_RUN_SYSTEM_COMMAND
	ret += excuteCmd(SETTING_POSTINST_FILE, 1, "memory");
#else
	int value = SETTING_DEF_MEMORY_PHONE;
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_WAP_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_BLUETOOTH_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_CAMERA_INT, value);

	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_VOICE_RECORDER_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_FM_RADIO_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_ALL_SHARE_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_ADOBE_AIR_INT, value);
	ret += vconf_set_int(VCONFKEY_SETAPPL_DEFAULT_MEM_DVB_H_INT, value);
#endif
	return ret;
}


