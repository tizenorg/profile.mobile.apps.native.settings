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

#include "setting-display.h"
#include <setting-cfg.h>

#include <app.h>
#include <appcore-common.h>
#include <Eina.h>

#define SETTING_DISPLAY_PACKAGE_NAME "org.tizen.setting-display"

/******************************APP CONTROL***********************************/
static bool _setting_display_app_create(void *data);
static void _setting_display_app_control_cb(app_control_h app_control, void *data);
static void _setting_display_app_on_pause(void *data);
static void _setting_display_app_on_resume(void *data);
static void _setting_display_app_terminate(void *data);
/********************************OTHER***************************************/
static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info);
static void _low_battery(app_event_info_h event_info, void *data);
static void _lang_changed(app_event_info_h event_info, void *data);

static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	elm_exit();
}

/**
 * view selection by service variable
 *
 * viewtype == 'brightness' --> setting_view_display_brightness
 * viewtype != NULL && viewtype --> setting_view_display_main
 */
setting_view *__get_display_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;
	SettingDisplay *ad = (SettingDisplay *)data;

	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "!data");
	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;
	ret = app_control_get_uri(service, &uri);
	if (!ret && uri) {
		SETTING_TRACE("uri:%s", uri);
		/* DO NOTHING NOW */
	} else {

		SETTING_TRACE_DEBUG("viewtype:%s", viewtype);

		if (!safeStrCmp(viewtype, "brightness")) {
			setting_view_node_table_register(
					&setting_view_display_brightness, NULL);
			FREE(viewtype);
			return &setting_view_display_brightness;
		} else if (!safeStrCmp(viewtype, "backlight")) {
			setting_view_node_table_register(
					&setting_view_display_backlight, NULL);
			FREE(viewtype);
			return &setting_view_display_backlight;
		} else {
			if (!safeStrCmp(viewtype, "frontpage_backlight")) {
				ad->display_expand_state =
						DISPLAY_EXPAND_BACKLIGHT;
			} else if (!safeStrCmp(viewtype,
					"frontpage_duration")) {
				ad->display_expand_state =
						DISPLAY_EXPAND_TOUCH_KEY_LIGHT_DURATION;
			} else {
				ad->display_expand_state =
						DISPLAY_EXPAND_NONE;
			}
			setting_view_node_table_register(
					&setting_view_display_main, NULL);
			setting_view_node_table_register(
					&setting_view_display_brightness,
					&setting_view_display_main);
			FREE(viewtype);
			return &setting_view_display_main;
		}
	}
	return NULL;
}

/**
 * return view layout
 *
 * viewtype != 'wallpaper' --> ly_main
 */
Evas_Object *__get_display_layout_to_return(app_control_h service, void *priv)
{
	SettingDisplay *ad = priv;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;

	SETTING_TRACE_BEGIN;

	ret = app_control_get_uri(service, &uri);
	if (!ret && uri)
		return ad->md.view_layout;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);
	FREE(viewtype);

	return ad->md.win_main;
}

static void setting_display_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingDisplay *ad = (SettingDisplay *)data;
	setting_view_update(ad->view_to_load, ad);
}

/*help smart screen popup*/
Eina_Bool __show_smartstay_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Data parameter is NULL");

	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retvm_if(ad->smart_stay_sublayout == NULL,
			ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Dad->smart_stay_sublayout parameter is NULL");

	/*hide top popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout, "elm,state,top,hide",
			"elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout,
			"elm,state,center,show", "elm");

	return ECORE_CALLBACK_CANCEL;
}
Eina_Bool __show_smartrotation_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Data parameter is NULL");

	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retvm_if(ad->smart_rotation_sublayout == NULL,
			ECORE_CALLBACK_CANCEL,
			"[Setting > Display] ad->smart_rotation_sublayout parameter is NULL");
	/*hide top popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout,
			"elm,state,top,hide", "elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout,
			"elm,state,center,show", "elm");

	return ECORE_CALLBACK_CANCEL;
}

//static void setting_display_ug_on_destroy(ui_gadget_h ug, app_control_h service,
//		void *priv)
//{
//	SettingDisplay *ad = priv;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if((!priv), "!priv");
//
//	/* fix flash issue for gallery */
//	evas_object_event_callback_del(ad->win_main_layout,
//			EVAS_CALLBACK_RESIZE, setting_display_ug_cb_resize);
//	ad->ug = ug;
//
//	FREE(ad->uri);
//	if (ad->help_popup) {
//		evas_object_del(ad->help_popup);
//		ad->help_popup = NULL;
//	}
//
//	/*	called when this shared gadget is terminated. similar with
//	 * app_exit */
//	if (&setting_view_display_main == ad->view_to_load) {
//		setting_view_destroy(&setting_view_display_brightness,
//				ad);
//		setting_view_destroy(&setting_view_display_main, ad);
//	} else if (&setting_view_display_brightness ==
//			ad->view_to_load) {
//		setting_view_destroy(&setting_view_display_brightness,
//				ad);
//	} else if (&setting_view_display_backlight == ad->view_to_load) {
//		setting_view_destroy(&setting_view_display_backlight,
//				ad);
//	}
//
//	if (NULL != ug_get_layout(ad->ug)) {
//		evas_object_hide((Evas_Object *)ug_get_layout(ad->ug));
//		evas_object_del((Evas_Object *)ug_get_layout(ad->ug));
//	}
//
//	SETTING_TRACE_END;
//}
//
//static void setting_display_ug_on_message(ui_gadget_h ug, app_control_h msg,
//		app_control_h service, void *priv)
//{
//}
//
//static void setting_display_ug_on_key_event(ui_gadget_h ug,
//		enum ug_key_event event, app_control_h service, void *priv)
//{
//	SettingDisplay *ad = (SettingDisplay *)priv;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if(NULL == ad, "ad is NULL");
//
//	switch (event) {
//	case UG_KEY_EVENT_END: {
//		if (elm_naviframe_top_item_get(ad->navi_bar) ==
//				elm_naviframe_bottom_item_get(
//						ad->navi_bar)) {
//			ug_destroy_me(ug);
//		} else {
//			if (&setting_view_display_brightness ==
//					setting_view_node_get_cur_view()) {
//				/* ????? */
//			}
//			setting_view_cb_at_endKey(ad);
//		}
//	}
//		break;
//	default:
//		break;
//	}
//}
//
//UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
//{
//	SettingDisplay *ad = calloc(1, sizeof(SettingDisplay));
//
//	SETTING_TRACE_BEGIN;
//	setting_retvm_if(!ad, -1, "Create SettingDisplay obj failed");
//
//	memset(ad, 0x00, sizeof(SettingDisplay));
//
//	ops->create = setting_display_ug_on_create;
//	ops->start = setting_display_ug_on_start;
//	ops->pause = setting_display_ug_on_pause;
//	ops->resume = setting_display_ug_on_resume;
//	ops->destroy = setting_display_ug_on_destroy;
//	ops->message = setting_display_ug_on_message;
//	ops->event = setting_display_ug_on_event;
//	ops->key_event = setting_display_ug_on_key_event;
//	ops->priv = ad;
//	ops->opt = UG_OPT_INDICATOR_ENABLE;
//
//	return 0;
//}

//UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
//{
//	struct SettingDisplay *ad;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if(!ops, "ops == NULL");
//
//	ad = ops->priv;
//	if (ad)
//		FREE(ad);
//}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_display_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	Evas_Object *base;

	SETTING_TRACE_BEGIN;

	base = (Evas_Object *)ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		evas_object_show(base);
		break;
	default:
		break;
	}

	SETTING_TRACE_END;
}

#define SUBINDEX_NAME "Display"

EXPORT_PUBLIC
int get_display_ug_state(Cfg_Item_State *stat, void *data)
{
	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL,
			&value);

	SETTING_TRACE_BEGIN;

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return ret;
}

/**************************************************************
 toggle state get/set function for "auto rotate screen"
 ************************************************************/
EXPORT_PUBLIC
int set_display_ug_state(Cfg_Item_State stat, void *item, void *data)
{
	int value = -1;

	SETTING_TRACE_BEGIN;

	if (stat == Cfg_Item_On)
		value = 1;
	else
		value = 0;

	return vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, value);
}

EXPORT_PUBLIC
int set_display_ug_update_ui(Cfg_Item_State stat, void *data)
{
	return 0;
}

/**************************************************************
 toggle state get/set function for "auto adjust screen tone"
 ************************************************************/
EXPORT_PUBLIC
int get_display_ug_auto_adjust_state(Cfg_Item_State *stat, void *data)
{
	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT,
			&value);

	SETTING_TRACE_BEGIN;

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return ret;
}

EXPORT_PUBLIC
int set_display_ug_auto_adjust_state(Cfg_Item_State stat, void *item,
		void *data)
{
	int value = -1;

	SETTING_TRACE_BEGIN;

	if (stat == Cfg_Item_On)
		value = 1;
	else
		value = 0;

	return vconf_set_bool(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT,
			value);
}

EXPORT_PUBLIC
int set_display_ug_auto_adjust_update_ui(Cfg_Item_State stat, void *data)
{
	return 0;
}

/**************************************************************
 toggle state get/set function for "dynamic status bar"
 ************************************************************/
EXPORT_PUBLIC
int get_display_ug_dynamic_status_state(Cfg_Item_State *stat, void *data)
{
	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, &value);

	SETTING_TRACE_BEGIN;

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return ret;
}

EXPORT_PUBLIC
int set_display_ug_dynamic_status_state(Cfg_Item_State stat, void *item,
		void *data)
{
	int value = -1;

	SETTING_TRACE_BEGIN;

	if (stat == Cfg_Item_On)
		value = 1;
	else
		value = 0;

	return vconf_set_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, value);
}

EXPORT_PUBLIC
int set_display_ug_dynamic_status_update_ui(Cfg_Item_State stat, void *data)
{
	return 0;
}

/**************************************************************
 toggle state get/set function for "edit after screen capture"
 ************************************************************/
EXPORT_PUBLIC
int get_display_ug_edit_screen_state(Cfg_Item_State *stat, void *data)
{
	int value = -1;
	int ret = vconf_get_bool(
			VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE,
			&value);

	SETTING_TRACE_BEGIN;

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return ret;
}

EXPORT_PUBLIC
int set_display_ug_edit_screen_state(Cfg_Item_State stat, void *item,
		void *data)
{
	int value = -1;

	SETTING_TRACE_BEGIN;

	if (stat == Cfg_Item_On)
		value = 1;
	else
		value = 0;

	return vconf_set_bool(
			VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE,
			value);
}

EXPORT_PUBLIC
int set_display_ug_edit_screen_update_ui(Cfg_Item_State stat, void *data)
{
	return 0;
}

EXPORT_PUBLIC
cfg_func_table opt_tab_rotation = {
	.get_item_state = get_display_ug_state,
	.set_item_state = set_display_ug_state,
	.set_item_update_ui = set_display_ug_update_ui, };

EXPORT_PUBLIC
cfg_func_table opt_tab_auto_adjust = {
	.get_item_state = get_display_ug_auto_adjust_state,
	.set_item_state = set_display_ug_auto_adjust_state,
	.set_item_update_ui = set_display_ug_auto_adjust_update_ui, };

EXPORT_PUBLIC
cfg_func_table opt_tab_dynamic_status = {
	.get_item_state = get_display_ug_dynamic_status_state,
	.set_item_state = set_display_ug_dynamic_status_state,
	.set_item_update_ui = set_display_ug_dynamic_status_update_ui, };
EXPORT_PUBLIC
cfg_func_table opt_tab_edit_screen = {
	.get_item_state = get_display_ug_edit_screen_state,
	.set_item_state = set_display_ug_edit_screen_state,
	.set_item_update_ui = set_display_ug_edit_screen_update_ui, };

/* cfg_func_table* tfunc; */
//UG_MODULE_API int setting_plugin_search_query_ops(char *str_id,
//		void **tfunc_obj)
//{
//	SETTING_TRACE_BEGIN;
//	SETTING_TRACE(">> get tfunc operation via plugin-model 1");
//	if (str_id && !safeStrCmp(str_id,
//			_("IDS_ST_HEADER_AUTO_ROTATE_SCREEN_ABB"))) {
//		*tfunc_obj = (void *)&opt_tab_rotation;
//	} else if (str_id && !safeStrCmp(str_id,
//			_("IDS_ST_MBODY_AUTO_ADJUST_SCREEN_TONE"))) {
//		*tfunc_obj = (void *)&opt_tab_auto_adjust;
//	} else if (str_id && !safeStrCmp(str_id,
//			_("IDS_ST_BODY_DYNAMIC_STATUS_BAR"))) {
//		*tfunc_obj = (void *)&opt_tab_dynamic_status;
//	}
//	if (str_id && !safeStrCmp(str_id,
//			_("IDS_ST_MBODY_EDIT_AFTER_SCREEN_CAPTURE"))) {
//		*tfunc_obj = (void *)&opt_tab_edit_screen;
//	}
//	SETTING_TRACE(">> get tfunc operation via plugin-model 2");
//	return 0;
//}
static void _low_battery(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = data;
	setting_retm_if(NULL == data, "data is NULL");

	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
		if(ad->md.navibar_main)
			setting_navi_items_update(ad->md.navibar_main);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

static void _setting_display_app_control_cb(app_control_h app_control, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = data;
	setting_retm_if(NULL == data, "data is NULL");

	ad->view_to_load = __get_display_view_to_load(ad, app_control);

	setting_retm_if(NULL == ad->view_to_load, "NULL == ad->view_to_load");
	setting_view_create(ad->view_to_load, (void *)ad);
}

static bool _setting_display_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, false, "!data");

	SettingDisplay *display_ad = (SettingDisplay *)data;

	setting_set_i18n("setting", DISPLAY_LOCALEDIR);

//    __theme = elm_theme_new();
//    elm_theme_ref_set(__theme, NULL);
//
//    elm_theme_extension_add(__theme, DISPLAY_THEME_EDJ_NAME);
//    elm_theme_extension_add(__theme, DISPLAY_GENLIST_EDJ_NAME);
//    elm_theme_extension_add(__theme, DISPLAY_NEWUX_EDJ_NAME);
//    elm_theme_extension_add(__theme, DISPLAY_NEW_GENLIST_EDJ_NAME);
//    elm_theme_extension_add(__theme, DISPLAY_SLIDER_EDJ_NAME);

if (app_init(&display_ad->md, SETTING_DISPLAY_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
	SETTING_TRACE_ERROR("Cannot initialize application");
	return false;
}

	evas_object_smart_callback_add(display_ad->md.win_main, "delete,request",
                                   _main_win_del_cb, display_ad);

//	Create base Layout
	setting_create_Gendial_itc("1line.top", &(display_ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(display_ad->itc_2text_3));


//	setting_view_create(&setting_view_display_main, display_ad);
	SETTING_TRACE_BEGIN;

	evas_object_event_callback_add(display_ad->md.win_main,
									EVAS_CALLBACK_RESIZE,
									setting_display_ug_cb_resize, display_ad);
	SETTING_TRACE_BEGIN;
	return true;
}

static void _setting_display_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingDisplay *ad = (SettingDisplay *)data;
	setting_retm_if((!data), "!data");


	setting_view_destroy(&setting_view_display_main, ad);
	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.ly_main,
			EVAS_CALLBACK_RESIZE, setting_display_ug_cb_resize);

	FREE(ad->uri);
	if (ad->help_popup) {
		evas_object_del(ad->help_popup);
		ad->help_popup = NULL;
	}

	/* called when this shared gadget is terminated. similar with
	 * app_exit */

//	 if (storageUG->default_view->is_create)
//			 setting_view_destroy(storageUG->default_view, storageUG);

	if (&setting_view_display_main == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, ad);
		setting_view_destroy(&setting_view_display_main, ad);
	} else if (&setting_view_display_brightness == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, ad);
	} else if (&setting_view_display_backlight == ad->view_to_load) {
		setting_view_destroy(&setting_view_display_backlight, ad);
	}

	if (NULL != ad->md.ly_main) {
		evas_object_hide((Evas_Object *)ad->md.ly_main);
		evas_object_del(ad->md.ly_main);
	}

	SETTING_TRACE_END;

}

static void _setting_display_app_on_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void _setting_display_app_on_resume(void *data)
{
	SETTING_TRACE_BEGIN;
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SettingDisplay display_ad ;
	app_event_handler_h handlers[5] = {NULL, };

	ui_app_lifecycle_callback_s ops = {
		.create = _setting_display_app_create,
		.pause = _setting_display_app_on_pause,
		.resume = _setting_display_app_on_resume,
		.terminate = _setting_display_app_terminate,
		.app_control = _setting_display_app_control_cb,
	};


	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, _low_battery, &display_ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &display_ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);

	memset(&display_ad, 0x00, sizeof(SettingDisplay));
	int r = 0;
	r = ui_app_main(argc, argv, &ops, &display_ad);
	retv_if(r == -1, -1);

	return 0;
}
