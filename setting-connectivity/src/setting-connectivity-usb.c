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

#include <app_manager.h>

#include "setting-connectivity-usb.h"

#define GROUP_MAX	30
#define USB_BTN_NO	0
#define SETTING_CPU_USAGE_PKG_NAME "org.tizen.dispcpuinfo"

#define DIAGNOSTICS_USAGE_STR		"IDS_ST_BODY_CRASH_VIEWER"
#define DEBUG_MODE_POPUP_TITLE	"IDS_ST_HEADER_ALLOW_USB_DEBUGGING_Q_ABB"
#define DEBUG_MODE_POPUP_TEXT \
	"IDS_ST_BODY_USB_DEBUGGING_IS_INTENDED_FOR_DEVELOPMENT_PURPOSES_ONLY_"\
	"MSG"
#define DEFAULT_RENDERING_ENGINE_STR	"IDS_ST_BODY_DEFAULT_RENDERING_ENGINE"
#if 0
#define SOFTWARE_STR			"IDS_ST_BODY_SOFTWARE"
#define HARDWARE_STR			"IDS_ST_BODY_HARDWARE"
#else
#define NONE_STR			"NONE"	/* -1 */
#define SOFTWARE_STR			"OFF"	/*	0 */
#define HARDWARE_STR			"ON"	/*	1 */
#endif
#define CONNECTTIVITY_SELECT_INFO_POPUP_STR \
	"IDS_HS_HEADER_USB_DEBUGGING_CONNECTED"

static char *bgprocess_list[] = {
	"IDS_ST_HEADER_STANDARD_LIMIT_ABB",
	"IDS_ST_HEADER_NO_BACKGROUND_PROCESSES_ABB",
	"IDS_ST_HEADER_1_PROCESS_AT_MOST_ABB",
	"IDS_ST_HEADER_2_PROCESSES_AT_MOST_ABB",
	"IDS_ST_HEADER_3_PROCESSES_AT_MOST_ABB",
	"IDS_ST_HEADER_4_PROCESSES_AT_MOST_ABB", };

static int setting_connectivity_usb_create(void *cb);
static int setting_connectivity_usb_destroy(void *cb);
static int setting_connectivity_usb_update(void *cb);
static int setting_connectivity_usb_cleanup(void *cb);

static void debug_mode_change_vconf_cb(keynode_t *key, void *data);
static void in_mode_change_vconf_cb(keynode_t *key, void *data);
static void unload_popup(void *data);
void load_usb_connection_popup(void *data);

/* This is pointer to indicate progressbars*/
/*static button_status button[NUM_PROGRESSBAR]; */

/* These variables are for the popup */

setting_view setting_view_connectivity_usb = {
	.create = setting_connectivity_usb_create,
	.destroy = setting_connectivity_usb_destroy,
	.update = setting_connectivity_usb_update,
	.cleanup = setting_connectivity_usb_cleanup, };

static void __setting_processes_sub_list_sel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *)event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);
	int err;
	setting_set_int_slp_key(data_parentItem->int_slp_setting_binded,
			data_subItem->chk_status, &err);
	setting_retm_if(0 != err, "Set vconf error[%d]",
			data_parentItem->int_slp_setting_binded);

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
	elm_object_item_signal_emit(data_parentItem->item, "elm,state,top", "");
	elm_genlist_item_expanded_set(parentItem, EINA_FALSE);
}

#if 1
static void __setting_devoptions_main_sub_list_sel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(event_info == NULL,
			"Invalid argument: event info is NULL");

	Elm_Object_Item *subitem = (Elm_Object_Item *)event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);
	SETTING_TRACE("data_subItem->chk_status = %d",
			data_subItem->chk_status);

	if (data_subItem->chk_status == 0) { /* none */
		vconf_set_int(VCONFKEY_SETAPPL_APP_HW_ACCELERATION,
				SETTING_HW_ACCELERATION_NONE);
	} else if (data_subItem->chk_status == 1) { /* software */
		elm_config_engine_set("software_x11"); /*deprecated API*/
		vconf_set_int(VCONFKEY_SETAPPL_APP_HW_ACCELERATION,
				SETTING_RENDERING_ENGINE_SW);
		elm_config_save();
	} else if (data_subItem->chk_status == 2) { /* hardware */
		elm_config_engine_set("opengl_x11"); /*deprecated API*/
		vconf_set_int(VCONFKEY_SETAPPL_APP_HW_ACCELERATION,
				SETTING_RENDERING_ENGINE_HW);
		elm_config_save();
	}
	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);
	elm_object_item_signal_emit(data_parentItem->item, "elm,state,top", "");
	elm_genlist_item_expanded_set(parentItem, EINA_FALSE);
}
#endif

static char *_get_graphic_engine()
{
	/* Get GPU Rendering state */
	const char *engine_str = NULL;
	engine_str = elm_config_engine_get(); /*deprecated API*/
	char *render_engine = NULL;
	if (engine_str) {
		SETTING_TRACE("engine_str is %s", engine_str);
		if (0 == safeStrCmp(engine_str, "opengl_x11")) {
			render_engine = HARDWARE_STR;
		} else if (0 == safeStrCmp(engine_str, "software_x11")) {
			render_engine = SOFTWARE_STR;
		} else {
			SETTING_TRACE_ERROR("engine_str is wrong");
			render_engine = NONE_STR;
		}
		return strdup(render_engine);
	} else {
		SETTING_TRACE_ERROR("engine_str is NULL");
		return NULL;
	}
}

static void __setting_devoptions_main_exp_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	Elm_Object_Item *parentItem = event_info; /*parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(
			parentItem); /*parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;

	if (data_parentItem) {
		if (data_parentItem == ad->bg_processes) {
			/*elm_object_item_signal_emit(parentItem,
			 * "elm,state,top", ""); */
			rgd = elm_radio_add(scroller);
			elm_radio_value_set(rgd, -1);

			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 0,
					_(bgprocess_list[0]),
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 1,
					_(bgprocess_list[1]),
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 2,
					_(bgprocess_list[2]),
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 3,
					_(bgprocess_list[3]),
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 4,
					_(bgprocess_list[4]),
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_processes_sub_list_sel_cb, ad,
					parentItem, SWALLOW_Type_1RADIO, rgd, 5,
					_(bgprocess_list[5]),
					NULL);

			setting_update_chk_status(rgd,
					data_parentItem->int_slp_setting_binded);
		} else if (data_parentItem == ad->gpu_render) {
			/*elm_object_item_signal_emit(parentItem,
			 * "elm,state,top", ""); */
			rgd = elm_radio_add(scroller);
			elm_radio_value_set(rgd, -1);

			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_devoptions_main_sub_list_sel_cb,
					ad, parentItem, SWALLOW_Type_1RADIO,
					rgd,
					/*NONE*/0, "NONE",
					NULL);

			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_devoptions_main_sub_list_sel_cb,
					ad, parentItem, SWALLOW_Type_1RADIO,
					rgd,
					/*SOFTWARE*/1, "OFF",
					NULL);
			setting_create_Gendial_exp_sub_field(scroller,
					&itc_1text_1icon,
					__setting_devoptions_main_sub_list_sel_cb,
					ad, parentItem, SWALLOW_Type_1RADIO,
					rgd,
					/*HARDWARE*/2, "ON",
					NULL);
			/* on, off, none */
			const char *render_engine = _get_graphic_engine();
			if (!safeStrCmp("NONE", render_engine)) {
				vconf_set_int(
						ad->bg_processes->int_slp_setting_binded,
						SETTING_HW_ACCELERATION_NONE);
				elm_radio_value_set(rgd, 0);
			} else if (!safeStrCmp("OFF", render_engine)) {
				vconf_set_int(
						ad->bg_processes->int_slp_setting_binded,
						SETTING_HW_ACCELERATION_OFF);
				elm_radio_value_set(rgd, 1);
			} else if (!safeStrCmp("ON", render_engine)) {
				vconf_set_int(
						ad->bg_processes->int_slp_setting_binded,
						SETTING_HW_ACCELERATION_ON);
				elm_radio_value_set(rgd, 2);
			} else {
				SETTING_TRACE_ERROR(
						"ERROR here for selection of r"\
						"ender_engine");
			}
			FREE(render_engine);
		}
	}
}

static int setting_connectivity_change_debug_mode_toggle(
		SettingConnectivityUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (!ad)
		return -1;

	int ret = -1;
	int debugMode = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL, &debugMode);
	if (ret != 0) {
		SETTING_TRACE("FAIL: vconf_get_bool(VCONFKEY_SETAPPL_USB_DEBUG"\
				"_MODE_BOOL)");
		return -1;
	}
	SETTING_TRACE("debugMode: %d", debugMode);

	setting_update_gl_item_chk_status(ad->debug_mode, EINA_FALSE);
	if (debugMode == 0) {
		if (USB_CONNECTED == check_usb_jack_status()) {
			ret = vconf_set_bool(
					VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL,
					1);
			if (ret != 0) {
				SETTING_TRACE("FAIL: vconf_set_bool(VCONFKEY_S"\
						"ETAPPL_USB_DEBUG_MODE_BOOL)");
				return -1;
			}
			setting_create_popup(ad, ad->md.view_layout, NULL,
					CONNECTTIVITY_SELECT_INFO_POPUP_STR,
					NULL, 3, false, false, 0);
		} else {
			load_usb_connection_popup(ad);
		}
	} else {
		ret = vconf_set_bool(VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL, 0);
		if (ret != 0) {
			SETTING_TRACE("FAIL: vconf_set_bool(VCONFKEY_SETAPPL"\
					"_USB_DEBUG_MODE_BOOL)");
			return -1;
		}

		if (ad->selectioninfo_popup) {
			evas_object_del(ad->selectioninfo_popup);
			ad->selectioninfo_popup = NULL;
		}
	}

	SETTING_TRACE_END;
	return 0;
}

static void setting_connectivity_main_chk_usb_debug_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)data;
	SettingConnectivityUG *ad = list_item->userdata;

	if (0 > setting_connectivity_change_debug_mode_toggle(ad)) {
		SETTING_TRACE("FAIL: setting_connectivity_change_debug_mode_to"\
				"ggle()");
	}

	SETTING_TRACE_END;
}

static void setting_connectivity_usb_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	retm_if(event_info == NULL, "Invalid argument: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	elm_genlist_item_selected_set(item, 0);

	if (0 > setting_connectivity_change_debug_mode_toggle(ad)) {
		SETTING_TRACE("FAIL: setting_connectivity_change_debug_mode_to"\
				"ggle()");
	}

	SETTING_TRACE_END;
}

static void setting_connectivity_main_chk_btn_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/*error check */
	retm_if(data == NULL, "Data parameter is NULL");

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;

	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	int opposite = (list_item->chk_status == 1) ? SETTING_ON_OFF_BTN_ON :
			SETTING_ON_OFF_BTN_OFF;

	SETTING_TRACE("list_item->keyStr = %s", list_item->keyStr);
	if (!safeStrCmp("IDS_ST_MBODY_SHOW_CPU_USAGE", list_item->keyStr)) {
		if (opposite) {
			app_launcher(SETTING_CPU_USAGE_PKG_NAME, NULL, NULL);
		} else {
#if 0
			bool running_flag = FALSE;
			int ret = app_manager_is_running(
					SETTING_CPU_USAGE_PKG_NAME,
					&running_flag);
			setting_retm_if(ret != 0,
					"fail to call app_manager_is_running");
			if (running_flag) {
				app_context_h context = NULL;
				ret = app_manager_get_app_context(
						SETTING_CPU_USAGE_PKG_NAME,
						&context);
				setting_retm_if(ret != 0,
						"fail to call app_manager_get_"\
						"app_context");
				int ret = app_manager_terminate_app(context);
				if (ret == 0) {
					SETTING_TRACE("===== app_manager_termi"\
							"nate_app successed.");
				} else {
					SETTING_TRACE_ERROR("===== app_manager"\
							"_terminate_app fail.");
				}
			}
#endif
		}
	} else {
		/* do nothing */
	}
	/*app_launcher("org.tizen.dispcpuinfo"); */
}

static void setting_connectivity_usb_mouse_up_Gendial_CPU_usage_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);

	setting_retm_if(data == NULL, "Data parameter is NULL");
	/*SettingConnectivityUG *ad = (SettingConnectivityUG *) data; */

	if (list_item) {
		setting_update_gl_item_chk_status(list_item,
				!(list_item->chk_status));

		if (list_item->chk_status) {
			vconf_set_bool("db/setting/show_cpu_info", 1);
			app_launcher(SETTING_CPU_USAGE_PKG_NAME, NULL, NULL);
		} else {
			vconf_set_bool("db/setting/show_cpu_info", 0);
		}
	}
	/*app_launcher("org.tizen.dispcpuinfo"); */
	SETTING_TRACE_END;
}

static void __setting_connectivity_usb_dia_usage_mouse_up_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);

	/* SettingAboutUG *ad = (SettingAboutUG *) data; */

	if (list_item)
		SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	app_launcher("org.tizen.crash-viewer", NULL, NULL);
}

char *get_bg_processes()
{
	int value;
	int err;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DEVOPTION_BGPROCESS,
			&value, &err);
	if (ret != 0)
		SETTING_TRACE_ERROR("get vconf failed");

	if (value >= 0 && value <
			sizeof(bgprocess_list) / sizeof(bgprocess_list[0]))
		return _(bgprocess_list[value]);
	else {
		return _(bgprocess_list[0]); /* error proc */
	}
}

/**
 * To create the usb main view
 *
 *@param cb
 *
 *@return
 */
static int setting_connectivity_usb_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("Start USB utilities\n");
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingConnectivityUG *ad = (SettingConnectivityUG *)cb;
	Evas_Object *scroller = NULL;

	int vconf_ret = -1;
	int ret = -1;

	ret = view_init(&ad->md, KeyStr_DeveloperOption);
	if (ret != SETTING_RETURN_SUCCESS)
		return ret;

	elm_layout_file_set(ad->md.view_layout, SETTING_THEME_EDJ_NAME,
			"selinfo_bottom");
	evas_object_size_hint_weight_set(ad->md.view_layout, EVAS_HINT_EXPAND,
			0.0);

	/* win_main of the popup */
#ifdef HELP_UG_EXIST
	ad->ly_usb =
	setting_create_layout_navi_bar_genlist(ad->md.ly_main,
			ad->md.win_main,
			KeyStr_DeveloperOption,
			dgettext("sys_string", "IDS_ST_BUTTON_BACK"),
			_("IDS_ST_HEADER_HELP"),
			setting_connectivity_usb_click_softkey_cancel_cb,
			setting_connectivity_usb_click_softkey_set_cb,
			ad, &scroller,
			&ad->md.navibar_main);
#else
	ad->ly_usb = setting_create_layout_navi_bar(ad->md.ly_main,
			ad->md.win_main,
			KeyStr_DeveloperOption, _("IDS_ST_BUTTON_BACK"),
			setting_connectivity_usb_click_softkey_cancel_cb, ad,
			ad->md.view_layout, &ad->md.navibar_main, NULL);
#endif

	SETTING_TRACE("before init\n");

	scroller = elm_genlist_add(ad->md.ly_main);
	retvm_if(scroller == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
			"Cannot set scroller object  as content of layout");
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller); /* first to clear list */
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb,
			NULL);

	elm_object_part_content_set(ad->md.view_layout, "elm.swallow.contents",
			scroller);

	/*button[SETTING_USB_DEBUG_MODE].item = NULL; */
	/*button[SETTING_USB_DEBUG_MODE].pstate = STATE_NONE; */
	setting_enable_expandable_genlist(scroller, ad,
			__setting_devoptions_main_exp_cb, NULL);

	Elm_Object_Item *item = NULL;;
#if 1
	/* [UI] separator */
	/*item = elm_genlist_item_append(scroller, &itc_seperator, NULL, NULL,
	 ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/* [UI] USB debugging */
	int vconf_val;
	int toggle_dbg = 0;

	ret = vconf_get_bool(VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL, &toggle_dbg);
	if (ret != 0) {
		SETTING_TRACE_ERROR("FAIL: vconf_get_bool(VCONFKEY_SETAPPL_USB"\
				"_DEBUG_MODE_BOOL)");
		/* set debug mode to true to find the problem*/
		toggle_dbg = 1;
	}
	SETTING_TRACE("toggle_dbg: %d", toggle_dbg);

	ad->debug_mode = setting_create_Gendial_field_def(scroller,
			&itc_1text_1icon,
			setting_connectivity_usb_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_1ICON_1RADIO, NULL,
			NULL, toggle_dbg, "IDS_ST_BODY_USB_DEBUGGING",
			NULL, setting_connectivity_main_chk_usb_debug_cb);

	if (ad->debug_mode) {
		ad->debug_mode->userdata = ad;
		__BACK_POINTER_SET(ad->debug_mode);
		/*button[SETTING_USB_DEBUG_MODE].item = ad->debug_mode->item; */
	} else {
		SETTING_TRACE_ERROR("ad->debug_mode is NULL");
		return SETTING_RETURN_FAIL;
	}

	ADD_GL_HELP(scroller, SETTING_USB_DEBUGGING_DESC);
#endif

	SETTING_TRACE("Second item of Second group is set\n");

	if (!isEmulBin()) {
		/* [UI] Show CPU Usage */
		bool running_flag = FALSE;
		int ret = app_manager_is_running(SETTING_CPU_USAGE_PKG_NAME,
				&running_flag);
		setting_retvm_if(ret != 0, SETTING_RETURN_FAIL,
				"fail to call app_manager_is_running");
		if (running_flag)
			vconf_val = 1;
		else
			vconf_val = 0;

		ad->cpu_usage = setting_create_Gendial_field_def(scroller,
				&itc_1text_1icon,
				setting_connectivity_usb_mouse_up_Gendial_CPU_usage_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, vconf_val, "IDS_ST_MBODY_SHOW_CPU_USAGE",
				NULL, setting_connectivity_main_chk_btn_cb);
		if (ad->cpu_usage) {
			ad->cpu_usage->userdata = ad;
			__BACK_POINTER_SET(ad->cpu_usage);
			/*setting_disable_genlist_item(ad->cpu_usage->item); */
		} else {
			SETTING_TRACE_ERROR("ad->cpu_usage is NULL");
			return SETTING_RETURN_FAIL;
		}

		/* [UI] help text */
		ADD_GL_HELP(scroller,
				"IDS_ST_BODY_SCREEN_OVERLAY_SHOWING_CURRENT_CP"\
				"U_USAGE_ABB");

#if 1
		/* Get GPU Rendering state */

		const char *render_engine = _get_graphic_engine();

		/* [UI] Force GPU rendering */
		ad->gpu_render = setting_create_Gendial_exp_parent_field(
				scroller, &(ad->itc_2text_3_parent),
				NULL, NULL, SWALLOW_Type_INVALID,
				DEFAULT_RENDERING_ENGINE_STR, _(render_engine));
		if (ad->gpu_render) {
			__BACK_POINTER_SET(ad->gpu_render);
			ad->gpu_render->userdata = ad;
			ad->gpu_render->int_slp_setting_binded =
					INT_SLP_SETTING_DEVOPTION_BGPROCESS;
			/*setting_disable_genlist_item(ad->gpu_render->item); */
		} else {
			SETTING_TRACE_ERROR("ad->gpu_render is NULL");
			return SETTING_RETURN_FAIL;
		}
		FREE(render_engine);
		/* [UI] help text */
		ADD_GL_HELP(scroller, "IDS_ST_POP_USE_2D_HARDWARE_OR_SOFTWARE_"\
				"ACCELERATION_IN_APPLICATIONS");
#endif
		/* NEWCODE: */

		/* [UI] expandble list - Automatic answering */
		char *bgproc = get_bg_processes();
		ad->bg_processes = setting_create_Gendial_exp_parent_field(
				scroller, &(ad->itc_2text_3_parent),
				NULL, NULL, SWALLOW_Type_INVALID,
				"IDS_ST_BODY_LIMIT_BACKGROUND_PROCESSES",
				bgproc);

		/* [UI] Limit background processes */
		if (ad->bg_processes) {
			__BACK_POINTER_SET(ad->bg_processes);
			ad->bg_processes->userdata = ad;
			ad->bg_processes->int_slp_setting_binded =
					VCONFKEY_SETAPPL_APP_HW_ACCELERATION;
		} else {
			SETTING_TRACE_ERROR("ad->bg_processes is NULL");
			return SETTING_RETURN_FAIL;
		}

		/*item = elm_genlist_item_append(scroller, &itc_seperator, NULL,
		 * NULL,
		 ELM_GENLIST_ITEM_NONE, NULL, NULL);
		 elm_genlist_item_select_mode_set(item,
		 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

		/* [UI] Diagnostics and Usage */
		setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
				__setting_connectivity_usb_dia_usage_mouse_up_cb,
				ad, SWALLOW_Type_INVALID, NULL, NULL, 0,
				DIAGNOSTICS_USAGE_STR,
				NULL, NULL);
	}

	/*
	 item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL,
	 NULL,
	 ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	 */

	/*------------------------------------------------------------------ */
	/* Registering callback function for VCONFKEY_SETAPPL_USB_MODE_INT */
	vconf_ret = vconf_notify_key_changed(
			VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL,
			debug_mode_change_vconf_cb, ad);
	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_notify_key_changed(VCONFKEY_SETAPPL"\
				"_USB_DEBUG_MODE_BOOL)");
		return SETTING_RETURN_FAIL;
	}

	/* Registering callback function for
	 * VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE */
	vconf_ret = vconf_notify_key_changed(
			VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE,
			in_mode_change_vconf_cb, ad);
	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_notify_key_changed(VCONFKEY_SETAPPL"\
				"_USB_IN_MODE_CHANGE)\n");
		return SETTING_RETURN_FAIL;
	}

	int val;
	vconf_ret = vconf_get_int(VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE, &val);
	if (vconf_ret == 0 && val == IN_MODE_CHANGE) {
		SETTING_TRACE("ad->blockUI: EINA_TRUE\n");
		ad->blockUI = EINA_TRUE;
	} else { /* val == CHANGE_COMPLETE */
		SETTING_TRACE("ad->blockUI: EINA_FALSE\n");
		ad->blockUI = EINA_FALSE;
	}

	setting_view_connectivity_usb.is_create = 1;
	setting_view_update(&setting_view_connectivity_usb, ad);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

/**
 * This callback function is called when USB-setting changes
 * VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE
 *
 *@param data
 */
static void in_mode_change_vconf_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "ERROR:The parameter(data) is NULL\n");
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;
	SETTING_TRACE("ad->blockUI: %d\n", ad->blockUI);

	int in_mode_change;
	int ret = -1;

	ret = vconf_get_int(VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE,
			&in_mode_change);
	setting_retm_if(ret != 0,
			"vconf_get_int(VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE\n");

	if (CHANGE_COMPLETE == in_mode_change) {
		SETTING_TRACE("ad->blockUI: EINA_FALSE\n");
		ad->blockUI = EINA_FALSE;
	} else if (IN_MODE_CHANGE == in_mode_change) {
		SETTING_TRACE("ad->blockUI: EINA_TRUE\n");
		ad->blockUI = EINA_TRUE;
	} else {
		SETTING_TRACE("Getting in_mode_change failed\n");
	}

	SETTING_TRACE_END;
}

/**
 * When VCONFKEY_SETAPPL_USB_MODE_INT is changed, all buttons are enabled
 *
 *@param key
 *@param data
 */
static void debug_mode_change_vconf_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "ERROR:The parameter(data) is NULL\n");
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	int ret = -1;
	int debugMode = 0;

	ret = vconf_get_bool(VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL, &debugMode);
	setting_retm_if(ret != 0, "ERROR: vconf_get_int(VCONFKEY_SETAPPL_USB_D"\
			"EBUG_MODE_BOOL)");
	SETTING_TRACE("debugMode: %d", debugMode);

	if (debugMode == 1)
		setting_update_gl_item_chk_status(ad->debug_mode, EINA_TRUE);
	else if (debugMode == 0)
		setting_update_gl_item_chk_status(ad->debug_mode, EINA_FALSE);
	else
		SETTING_TRACE("ERROR: debugMode value is improper");

	/* Unblock setting UI */
	SETTING_TRACE("ad->blockUI: EINA_FALSE\n");
	ad->blockUI = EINA_FALSE;

	SETTING_TRACE_END;
}

/**
 * To destory the view of usb
 *
 *@param cb
 *
 *@return
 */
static int setting_connectivity_usb_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingConnectivityUG *ad = (SettingConnectivityUG *)cb;
	if (!setting_view_connectivity_usb.is_create)
		return SETTING_RETURN_SUCCESS;

	setting_view_connectivity_usb.is_create = 0;
	int vconf_ret = -1;
	int num_err = 0;

	/* Removing the popup if the popup is loaded */
	unload_popup(ad);

	/*if (ad->selectioninfo_popup) {
	 evas_object_del(ad->selectioninfo_popup);
	 ad->selectioninfo_popup = NULL;
	 }*/

	/* Closing vconf trace */
	vconf_ret = vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL,
			debug_mode_change_vconf_cb);

	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_ignore_key_changed(VCONFKEY_SETAPPL"\
				"_USB_DEBUG_MODE_BOOL)");
		num_err++;
	}

	vconf_ret = vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_USB_IN_MODE_CHANGE,
			in_mode_change_vconf_cb);

	if (vconf_ret != 0) {
		SETTING_TRACE("FAIL: vconf_ignore_key_changed(VCONFKEY_SETAPPL"\
				"_USB_MODE_INT)\n");
		num_err++;
	}

	if (ad->ly_usb != NULL)
		evas_object_del(ad->ly_usb);

	SETTING_TRACE_END;
	if (num_err > 0) {
		SETTING_TRACE("ERROR: the number of errors to destroy is %d\n",
				num_err);
		return SETTING_RETURN_FAIL;
	} else {
		return SETTING_RETURN_SUCCESS;
	}

}

/**
 * To update the view of usb
 *
 *@param cb
 *
 *@return
 */
static int setting_connectivity_usb_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingConnectivityUG *ad = (SettingConnectivityUG *)cb;

	if (ad->ly_usb != NULL)
		evas_object_show(ad->ly_usb);

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

/**
 * To cleanup the view of usb
 *
 *@param cb
 *
 *@return
 */
static int setting_connectivity_usb_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

/**
 * Do process when clicking '<-' button
 *
 *@param data
 *@param obj
 *@param event_info
 */
static void setting_connectivity_usb_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	if (ad->md.win_main)
		elm_win_lower(ad->md.win_main);

	SETTING_TRACE_END;
}

/**
 * Do process when clicking 'Help' button
 *
 *@param data
 *@param obj
 *@param event_info
 */
#ifdef HELP_UG_EXIST
static void
setting_connectivity_usb_click_softkey_set_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingConnectivityUG *ad = (SettingConnectivityUG *) data;

	/* Send destroy request */
	setting_view_change(&setting_view_connectivity_usb,
			&setting_view_connectivity_usb_help, ad);
	SETTING_TRACE_END;
}
#endif

/**
 * Here is for loading popup
 */

/**
 * unload popup
 */
static void unload_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	/*ad->pop */
	if (ad->pop) {
		evas_object_del(ad->pop);
		ad->pop = NULL;
	} else {
		SETTING_TRACE("ad->pop == NULL\n");
	}

	SETTING_TRACE_END;
}

/**
 * Callback function to respond pushing cancel button of the usb connection
 * popup
 */
static void debug_mode_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	int resp_type = btn_type(obj);
	if (resp_type == POPUP_RESPONSE_OK) {
		/* do nothing */
		int ret = vconf_set_bool(VCONFKEY_SETAPPL_USB_DEBUG_MODE_BOOL,
				1);
		if (ret != 0) {
			SETTING_TRACE("FAIL: vconf_set_bool(VCONFKEY_SETAPPL_U"\
					"SB_DEBUG_MODE_BOOL)");
			return;
		}
		setting_create_popup(ad, ad->md.view_layout, NULL,
		CONNECTTIVITY_SELECT_INFO_POPUP_STR, NULL, 3, false, false, 0);
	} else if (resp_type == POPUP_RESPONSE_CANCEL) {
		/* rollback */
		/*setting_update_gl_item_chk_status(ad->debug_mode, 0); */
		if (ad->debug_mode) {
			ad->debug_mode->chk_status = 0;
			elm_genlist_item_fields_update(ad->debug_mode->item,
					"*", ELM_GENLIST_ITEM_FIELD_CONTENT);
		}
	}

	unload_popup(ad);
	SETTING_TRACE_END;
}

/**
 * When a button on USB utilities is pushed,
 * this function makes a popup if USB cable is not connected
 */
void load_usb_connection_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;

	unload_popup(ad);
	ad->pop = setting_create_popup(ad, ad->md.win_main, NULL,
			_(DEBUG_MODE_POPUP_TEXT), debug_mode_resp_cb, 0, false,
			false, 2, _("IDS_ST_BUTTON_CANCEL_ABB"),
			_("IDS_ST_BUTTON_ENABLE"));

	if (!(ad->pop))
		SETTING_TRACE("FAIL: setting_create_popup()");

	SETTING_TRACE_END;
}
