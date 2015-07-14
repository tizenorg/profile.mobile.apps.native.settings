/*
 * accessibility
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Contact: Tomasz Olszak <t.olszak@samsung.com>
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

#include "setting-accessibility.h"
#include <setting-cfg.h>
#include <setting-debug.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void *setting_accessibility_ug_on_create(ui_gadget_h ug,
                                                enum ug_mode mode, app_control_h service,
                                                void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingAccessibilityUG *accessibilityUG = priv;
	accessibilityUG->ug = ug;
	accessibilityUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	accessibilityUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(accessibilityUG->win_main_layout);
	accessibilityUG->evas = evas_object_evas_get(accessibilityUG->win_main_layout);

	setting_retvm_if(accessibilityUG->win_main_layout == NULL, NULL,
	                 "cannot get main window ");
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_accessibility_main, NULL);


	setting_view_node_set_cur_view(&setting_view_accessibility_main);
	setting_view_create(&setting_view_accessibility_main, (void *)accessibilityUG);

	return accessibilityUG->ly_main;
}

void setting_accessibility_ug_on_start(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

void setting_accessibility_ug_on_pause(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

void setting_accessibility_ug_on_resume(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

void setting_accessibility_ug_on_destroy(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

void setting_accessibility_ug_on_message(ui_gadget_h ug, app_control_h msg, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}
void setting_accessibility_ug_on_event(ui_gadget_h ug, enum ug_event event, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

void setting_accessibility_ug_on_key_event(ui_gadget_h ug, enum ug_key_event event,
                                           app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;

	SettingAccessibilityUG *accessibilityUG =
	    calloc(1, sizeof(SettingAccessibilityUG));
	setting_retvm_if(!accessibilityUG, -1,
	                 "Create SettingAccessibilityUG obj failed");

	ops->create = setting_accessibility_ug_on_create;
	ops->start = setting_accessibility_ug_on_start;
	ops->pause = setting_accessibility_ug_on_pause;
	ops->resume = setting_accessibility_ug_on_resume;
	ops->destroy = setting_accessibility_ug_on_destroy;
	ops->message = setting_accessibility_ug_on_message;
	ops->event = setting_accessibility_ug_on_event;
	ops->key_event = setting_accessibility_ug_on_key_event;
	ops->priv = accessibilityUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	SETTING_TRACE_END;
	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;

	struct SettingAccessibilityUG *accessibilityUG;
	setting_retm_if(!ops, "ops == NULL");

	accessibilityUG = ops->priv;
	if (accessibilityUG) {
		FREE(accessibilityUG);
	}

	SETTING_TRACE_END;
}

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv,
                                             char **applocale)
{
	SETTING_TRACE_BEGIN;
	int i, size;
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = priv;
	const Setting_Cfg_Node_T search_configs[] = {
		{DEVOPTION_STR_ACCESSIBILITY_MAGNIFIER, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
		{DEVOPTION_STR_ACCESSIBILITY_SCREENREADER, NULL, NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	};

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:/usr/apps/org.tizen.setting/res/locale");

	size = sizeof(search_configs) / sizeof(Setting_Cfg_Node_T);

	for (i = 0; i < size; i++) {
		node = setting_plugin_search_item_subindex_add(
		           search_configs[i].key_name,
		           "viewtype:accessibility",
		           IMG_USBconnection,
		           search_configs[i].item_type,
		           search_configs[i].data,
		           "Accessibility");

		*pplist = eina_list_append(*pplist, node);
	}

	SETTING_TRACE_END;
	return 0;
}

