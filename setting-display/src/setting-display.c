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
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;
	SettingDisplayUG *displayUG = (SettingDisplayUG *)data;

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
		app_control_get_extra_data(service, "viewtype", &viewtype);
		if (!viewtype)
			return NULL;

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
				displayUG->display_expand_state =
						DISPLAY_EXPAND_BACKLIGHT;
			} else if (!safeStrCmp(viewtype,
					"frontpage_duration")) {
				displayUG->display_expand_state =
						DISPLAY_EXPAND_TOUCH_KEY_LIGHT_DURATION;
			} else {
				displayUG->display_expand_state =
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
	SettingDisplayUG *displayUG = priv;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;

	SETTING_TRACE_BEGIN;

	ret = app_control_get_uri(service, &uri);
	if (!ret && uri)
		return displayUG->md.view_layout;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype)
		return NULL;

	SETTING_TRACE("viewtype:%s", viewtype);
	FREE(viewtype);

	return displayUG->md.win_main;
}

static void setting_display_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingDisplayUG *ad = (SettingDisplayUG *)data;
	setting_view_update(ad->view_to_load, ad);
}

/*help smart screen popup*/
Eina_Bool __show_smartstay_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
			"[Setting > Display] Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *)data;
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

	SettingDisplayUG *ad = (SettingDisplayUG *)data;
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
//	SettingDisplayUG *displayUG = priv;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if((!priv), "!priv");
//
//	/* fix flash issue for gallery */
//	evas_object_event_callback_del(displayUG->win_main_layout,
//			EVAS_CALLBACK_RESIZE, setting_display_ug_cb_resize);
//	displayUG->ug = ug;
//
//	FREE(displayUG->uri);
//	if (displayUG->help_popup) {
//		evas_object_del(displayUG->help_popup);
//		displayUG->help_popup = NULL;
//	}
//
//	/*	called when this shared gadget is terminated. similar with
//	 * app_exit */
//	if (&setting_view_display_main == displayUG->view_to_load) {
//		setting_view_destroy(&setting_view_display_brightness,
//				displayUG);
//		setting_view_destroy(&setting_view_display_main, displayUG);
//	} else if (&setting_view_display_brightness ==
//			displayUG->view_to_load) {
//		setting_view_destroy(&setting_view_display_brightness,
//				displayUG);
//	} else if (&setting_view_display_backlight == displayUG->view_to_load) {
//		setting_view_destroy(&setting_view_display_backlight,
//				displayUG);
//	}
//
//	if (NULL != ug_get_layout(displayUG->ug)) {
//		evas_object_hide((Evas_Object *)ug_get_layout(displayUG->ug));
//		evas_object_del((Evas_Object *)ug_get_layout(displayUG->ug));
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
//static void setting_display_ug_on_event(ui_gadget_h ug, enum ug_event event,
//		app_control_h service, void *priv)
//{
//	SettingDisplayUG *ad = priv;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if(NULL == ad, "ad is NULL");
//
//	switch (event) {
//	case UG_EVENT_LOW_MEMORY:
//		break;
//	case UG_EVENT_LOW_BATTERY:
//		break;
//	case UG_EVENT_LANG_CHANGE:
//		setting_navi_items_update(ad->navi_bar);
//		break;
//	case UG_EVENT_ROTATE_PORTRAIT:
//	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
//		break;
//	case UG_EVENT_ROTATE_LANDSCAPE:
//	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
//		break;
//	case UG_EVENT_REGION_CHANGE:
//		break;
//	default:
//		break;
//	}
//}
//
//static void setting_display_ug_on_key_event(ui_gadget_h ug,
//		enum ug_key_event event, app_control_h service, void *priv)
//{
//	SettingDisplayUG *ad = (SettingDisplayUG *)priv;
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
//	SettingDisplayUG *displayUG = calloc(1, sizeof(SettingDisplayUG));
//
//	SETTING_TRACE_BEGIN;
//	setting_retvm_if(!displayUG, -1, "Create SettingDisplayUG obj failed");
//
//	memset(displayUG, 0x00, sizeof(SettingDisplayUG));
//
//	ops->create = setting_display_ug_on_create;
//	ops->start = setting_display_ug_on_start;
//	ops->pause = setting_display_ug_on_pause;
//	ops->resume = setting_display_ug_on_resume;
//	ops->destroy = setting_display_ug_on_destroy;
//	ops->message = setting_display_ug_on_message;
//	ops->event = setting_display_ug_on_event;
//	ops->key_event = setting_display_ug_on_key_event;
//	ops->priv = displayUG;
//	ops->opt = UG_OPT_INDICATOR_ENABLE;
//
//	return 0;
//}

//UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
//{
//	struct SettingDisplayUG *displayUG;
//
//	SETTING_TRACE_BEGIN;
//	setting_retm_if(!ops, "ops == NULL");
//
//	displayUG = ops->priv;
//	if (displayUG)
//		FREE(displayUG);
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


static void _setting_display_app_control_cb(app_control_h app_control, void *data)
{
	SETTING_TRACE_BEGIN;
}

static bool _setting_display_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, false, "!data");

	SettingDisplayUG *display_ad = (SettingDisplayUG *)data;

//	Evas_Object *win;
//	int w, h;
//	const char *name = "setting-display";
	setting_set_i18n("setting", DISPLAY_LOCALEDIR);
	//	elm_app_base_scale_set(2.4);

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
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(display_ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(display_ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(display_ad->itc_2text_3));
	setting_view_create(&setting_view_display_main, display_ad);
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
	SettingDisplayUG *ad = (SettingDisplayUG *)data;
	setting_view_destroy(&setting_view_display_main, ad);
	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}
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
	SettingDisplayUG display_ad ;

	ui_app_lifecycle_callback_s ops = {
        .create = _setting_display_app_create,
        .pause = _setting_display_app_on_pause,
        .resume = _setting_display_app_on_resume,
        .terminate = _setting_display_app_terminate,
        .app_control = _setting_display_app_control_cb,
    };

//    app_event_handler_h hLowBatteryHandle;
//    app_event_handler_h hLanguageChangedHandle;

    memset(&display_ad, 0x00, sizeof(SettingDisplayUG));
	int r = 0;
    r = ui_app_main(argc, argv, &ops, &display_ad);
    retv_if(r == -1, -1);

    return 0;
}
