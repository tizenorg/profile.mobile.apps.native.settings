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

#include <setting-about.h>
#include <setting-about-main.h>
#include <setting-cfg.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

void setting_about_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	SETTING_TRACE_BEGIN;
	/*SettingAboutUG *ad = (SettingAboutUG *) priv; */
	Evas_Object *base;

	ret_if(!priv);
	base = (Evas_Object *)ug_get_layout(ug);
	ret_if(!base);

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		/* elm_win_resize_object_add(ad->win_get, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}

	SETTING_TRACE_END;
}

/**
 * @brief Event process when the sizeof UG view changes
 *
 * @param data
 * @param e
 * @param obj
 * @param event_info
 */
static void setting_about_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingAboutUG *ad = (SettingAboutUG *)data;
	setting_view_update(&setting_view_about_main, ad);
}

/**
 * @brief on_create function of the UG
 *
 * @param ug
 * @param mode
 * @param data
 * @param priv
 *
 * @return
 */
static void *setting_about_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((NULL == priv), NULL, "NULL == priv");
	SettingAboutUG *aboutUG = priv;
	aboutUG->ug = ug;

	aboutUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(ug);
	aboutUG->win_get = (Evas_Object *)ug_get_window();
	evas_object_show(aboutUG->win_main_layout);
	aboutUG->evas = evas_object_evas_get(aboutUG->win_main_layout);

	setting_retvm_if(aboutUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_about_main, NULL);

	/*	creating a view. */
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(aboutUG->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(aboutUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(aboutUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(aboutUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(aboutUG->itc_group_item));
	setting_create_Gendial_itc("multiline/1text",
			&(aboutUG->itc_help_style));

	setting_view_node_set_cur_view(&setting_view_about_main);
	setting_view_create(&setting_view_about_main, (void *)aboutUG);
	evas_object_event_callback_add(aboutUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_about_ug_cb_resize,
			aboutUG);

	aboutUG->popup_showed_flag = FALSE;
	return aboutUG->ly_main;
}

static void setting_about_ug_on_start(ui_gadget_h ug, app_control_h service,
		void *priv)
{
}

static void setting_about_ug_on_pause(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *SettingAboutUG = priv;
	SettingAboutUG->pause_flag = TRUE;
}

static void setting_about_ug_on_resume(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingAboutUG *aboutUG = priv;
	aboutUG->pause_flag = FALSE;

	char str[MAX_DISPLAY_STR_LEN_ON_PHONE_INFO] = { 0, };

	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_wifi) {
		setting_about_main_get_wifi_mac_address_string(str,
				MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		aboutUG->item_data_wifi->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(aboutUG->item_data_wifi->item,
				aboutUG->item_data_wifi);
		elm_genlist_item_update(aboutUG->item_data_wifi->item);
	}
	memset(str, 0x00, MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
	if (aboutUG->item_data_bt) {
		setting_about_main_get_bluetooth_address_string(str,
				MAX_DISPLAY_STR_LEN_ON_PHONE_INFO);
		aboutUG->item_data_bt->sub_desc = (char *)g_strdup(str);
		elm_object_item_data_set(aboutUG->item_data_bt->item,
				aboutUG->item_data_bt);
		elm_genlist_item_update(aboutUG->item_data_bt->item);
	}

	if (aboutUG->item_dev_name) {
		if (elm_object_focus_get(aboutUG->item_dev_name->eo_check)) {
			elm_object_focus_set(aboutUG->item_dev_name->eo_check,
					EINA_TRUE);
			Ecore_IMF_Context *imf_context =
					(Ecore_IMF_Context *)elm_entry_imf_context_get(
					aboutUG->item_dev_name->eo_check);
			if (imf_context)
				ecore_imf_context_input_panel_show(imf_context);
		}
	}
}

/**
 * on_destroy function of the UG
 *
 * @param ug
 * @param data
 * @param priv
 */
static void setting_about_ug_on_destroy(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingAboutUG *aboutUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(aboutUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_about_ug_cb_resize);
	aboutUG->ug = ug;

	/*	delete the allocated objects. */
	setting_view_destroy(&setting_view_about_main, aboutUG);
	if (NULL != ug_get_layout(aboutUG->ug)) {
		evas_object_hide((Evas_Object *)ug_get_layout(aboutUG->ug));
		evas_object_del((Evas_Object *)ug_get_layout(aboutUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_about_ug_on_message(ui_gadget_h ug, app_control_h msg,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

/**
 * @brief ug_event processing function
 *
 * @param ug
 * @param event
 * @param service
 * @param priv
 *
 * @return
 */
static void setting_about_ug_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)priv;
	setting_retm_if(NULL == ad, "ad is NULL");
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		setting_navi_items_update(ad->navi_bar);
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		/*break; */
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		/*break; */
	case UG_EVENT_ROTATE_LANDSCAPE:
		/*break; */
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN: {
		/*restore the focus status */
		SettingAboutUG *ad = (SettingAboutUG *)priv;
		ret_if(!ad || !ad->is_dev_name_focus || !ad->item_dev_name);
		SETTING_TRACE("ad->is_dev_name_focus:%d",
				ad->is_dev_name_focus);

		Ecore_IMF_Context *imf_context =
				(Ecore_IMF_Context *)elm_entry_imf_context_get(
						ad->item_dev_name->eo_check);
		if (imf_context)
			ecore_imf_context_input_panel_show(imf_context);

		elm_object_focus_set(ad->item_dev_name->eo_check,
				ad->is_dev_name_focus);
		elm_entry_cursor_pos_set(ad->item_dev_name->eo_check,
				ad->cursor_pos);
		ad->is_dev_name_focus = FALSE;
	}
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

/**
 * @brief ug_key_event processing function
 *
 * @param ug
 * @param event
 * @param service
 * @param priv
 *
 * @return
 */
static void setting_about_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *ad = (SettingAboutUG *)priv;
	ret_if(!ug);

	switch (event) {
	case UG_KEY_EVENT_END: {
		if (elm_naviframe_top_item_get(ad->navi_bar)
				== elm_naviframe_bottom_item_get(
						ad->navi_bar)) {
			ug_destroy_me(ug);
		} else {
			/* elm_naviframe_item_pop(ad->navi_bar); */
			setting_view_cb_at_endKey(ad);
		}
		break;
	}
	default:
		break;
	}
}

/**
 * @brief aboutUG init
 *
 * @param ops
 */
UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *aboutUG = calloc(1, sizeof(SettingAboutUG));
	setting_retvm_if(!aboutUG, -1, "Create SettingAboutUG obj failed");

	ops->create = setting_about_ug_on_create;
	ops->start = setting_about_ug_on_start;
	ops->pause = setting_about_ug_on_pause;
	ops->resume = setting_about_ug_on_resume;
	ops->destroy = setting_about_ug_on_destroy;
	ops->message = setting_about_ug_on_message;
	ops->event = setting_about_ug_on_event;
	ops->key_event = setting_about_ug_on_key_event;
	ops->priv = aboutUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

/**
 * @brief aboutUG exit
 *
 * @param ops
 */
UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingAboutUG *aboutUG;
	setting_retm_if(!ops, "ops == NULL");

	aboutUG = ops->priv;
	if (aboutUG) {
		if (aboutUG->handle
				&& tel_deinit(aboutUG->handle)
						== TAPI_API_SUCCESS) {
			SETTING_TRACE("tel_deinit sucessed");
		}
		FREE(aboutUG);
	}
}

