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

#include "setting-moreconnections.h"

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void *setting_moreconnections_ug_on_create(ui_gadget_h ug,
		enum ug_mode mode, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingMoreConnectionsUG *moreconnectionsUG = priv;
	moreconnectionsUG->ug = ug;
	moreconnectionsUG->win_main_layout =
			(Evas_Object *)ug_get_parent_layout(
					ug);
	moreconnectionsUG->win_get = (Evas_Object *)ug_get_window();

	moreconnectionsUG->evas = evas_object_evas_get(
			moreconnectionsUG->win_main_layout);

	setting_retvm_if(moreconnectionsUG->win_main_layout == NULL, NULL,
			"cannot get main window ");
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_moreconnections_main,
			NULL);

	/*	creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(moreconnectionsUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(moreconnectionsUG->itc_1text));

	setting_view_node_set_cur_view(&setting_view_moreconnections_main);
	setting_view_create(&setting_view_moreconnections_main,
			(void *)moreconnectionsUG);

	return moreconnectionsUG->ly_main;
}

static void setting_moreconnections_ug_on_start(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_pause(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_resume(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

	SettingMoreConnectionsUG *moreconnectionsUG = priv;
	setting_view_create(&setting_view_moreconnections_main,
			(void *)moreconnectionsUG);

	if (moreconnectionsUG->location_service) {
		char *sub_desc = setting_location_is_enable(priv);
		moreconnectionsUG->location_service->sub_desc = (char *)strdup(
				sub_desc);
		elm_object_item_data_set(
				moreconnectionsUG->location_service->item,
				moreconnectionsUG->location_service);
		elm_genlist_item_update(
				moreconnectionsUG->location_service->item);
	}

	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingMoreConnectionsUG *moreconnectionsUG = priv;

	/* called when this shared gadget is terminated. similar with
	 * app_exit */
	setting_view_destroy(&setting_view_moreconnections_main,
			moreconnectionsUG);

	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_message(ui_gadget_h ug,
		app_control_h msg, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_event(ui_gadget_h ug,
		enum ug_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

static void setting_moreconnections_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingMoreConnectionsUG *moreconnectionsUG = calloc(1,
			sizeof(SettingMoreConnectionsUG));
	setting_retvm_if(!moreconnectionsUG, -1,
			"Create SettingMoreConnectionsUG obj failed");

	memset(moreconnectionsUG, 0x00, sizeof(SettingMoreConnectionsUG));

	ops->create = setting_moreconnections_ug_on_create;
	ops->start = setting_moreconnections_ug_on_start;
	ops->pause = setting_moreconnections_ug_on_pause;
	ops->resume = setting_moreconnections_ug_on_resume;
	ops->destroy = setting_moreconnections_ug_on_destroy;
	ops->message = setting_moreconnections_ug_on_message;
	ops->event = setting_moreconnections_ug_on_event;
	ops->key_event = setting_moreconnections_ug_on_key_event;
	ops->priv = moreconnectionsUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingMoreConnectionsUG *moreconnectionsUG;
	setting_retm_if(!ops, "ops == NULL");

	moreconnectionsUG = ops->priv;
	if (moreconnectionsUG)
		FREE(moreconnectionsUG);
}
