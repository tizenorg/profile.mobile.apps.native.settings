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
#ifndef __SETTING_CONNECTIVITY_H__
#define __SETTING_CONNECTIVITY_H__



#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <runtime_info.h>


#define DEVOPTION_STR_USB_DEBUGGING	"IDS_ST_BODY_USB_DEBUGGING"
#define DEVOPTION_STR_SHOW_CPU_USAGE	"IDS_ST_MBODY_SHOW_CPU_USAGE"
#define DEVOPTION_STR_RENDERTING_ENGINE	"IDS_ST_BODY_DEFAULT_RENDERING_ENGINE"
#define DEVOPTION_STR_LIMIT_BACKGROUND_PRECESS \
	"IDS_ST_BODY_LIMIT_BACKGROUND_PROCESSES"
#define DEVOPTION_STR_CRASH_VIEWER	"IDS_ST_BODY_CRASH_VIEWER"

enum mode_change {
	CHANGE_COMPLETE = 0,
	IN_MODE_CHANGE,
	IN_MODE_CHANGE_NONE
};

enum usb_connection_status {
	USB_DISCONNECTED = 0,
	USB_CONNECTED,
	GETTING_STATUS_FAIL
};

typedef struct _SettingConnectivityUG SettingConnectivityUG;

/**
 * Setting Connectivity UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingConnectivityUG and the functions access app context.
 */
struct _SettingConnectivityUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *navi_bar;

	Evas_Object *ly_usb;
	Evas_Object *chk_con;

	Evas_Object *pop;

	Evas_Object *view_layout;
	Evas_Object *selectioninfo_popup;

	Setting_GenGroupItem_Data *debug_mode;	/* debug mode */
	Setting_GenGroupItem_Data *cpu_usage;	/* Show CPU Usage */
	Setting_GenGroupItem_Data *gpu_render;	/* Force GPU Rendering */
	Setting_GenGroupItem_Data *bg_processes;/* Limit background processes */

	/*****/
	setting_view *view_to_load;

	Elm_Genlist_Item_Class itc_title;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1text_1progress;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1text_1icon_gen;
	Elm_Genlist_Item_Class itc_2text_3;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_help_style;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;

	int noti_fd;
	Eina_Bool blockUI;

};

extern setting_view setting_view_connectivity_usb;
extern setting_view setting_view_connectivity_usb_help;

int check_usb_jack_status();
void set_usb_mode_default();

#endif				/* __SETTING_CONNECTIVITY_H__ */
