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
#ifndef __SETTING_APPLICATION_H__
#define __SETTING_APPLICATION_H__

#include <stdio.h>
#include <string.h>

#include <glib.h>
#include <pkgmgr-info.h>
#include <package-manager.h>

#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <device.h>
#include <dd-display.h>
#include <dd-deviced.h>
#include <dbus/dbus.h>
/*#include <devman.h> */

#include <pkgmgr-info.h>

#define SAFE_STRDUP(src) (src) ? strdup(src) : NULL

#define Keystr_Clear		"Clear"

#define SMART_SCREEN_TRY_IT_IMAGE_L \
	SETTING_ICON_PATH_CFG"air_jump_try_full_h.png"
#define SMART_SCREEN_TRY_IT_IMAGE_P \
	SETTING_ICON_PATH_CFG"air_jump_try_full.png"

#define KeyStr_ScreenCapture "IDS_MF_BODY_SCREEN_CAPTURE_M_NOUN"
#define KeyStr_ScreenCapture_Destination "IDS_LBS_BODY_DESTINATION"
#define KeyStr_ScreenCapture_Ask "IDS_SKM_BODY_ASK_M_IDEA_SKETCH"
#define KeyStr_ScreenCapture_Gallery "IDS_ST_BODY_GALLERY"
#define KeyStr_ScreenCapture_KeepIt \
	"IDS_KEEPIT_HEADER_KEEPIT_M_APPLICATION_NAME"
#define KeyStr_ScreenCapture_Edit "IDS_ST_MBODY_EDIT_AFTER_SCREEN_CAPTURE"
#define KeyStr_Dynamic_Status_Bar "IDS_ST_BODY_DYNAMIC_STATUS_BAR"

typedef struct _SettingApplicationsUG SettingApplicationsUG;

/**
 * Setting Applications UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingApplicationsUG and the functions access app context.
 */
struct _SettingApplicationsUG {
	MainData md;

	/* add more variables here (move your appdata to here) */
	ui_gadget_h ug_loading;
	ui_gadget_h ug_appmgr;
	ui_gadget_h ug_homescreen;
	ui_gadget_h ug_passwd;
	ui_gadget_h ug_ledindicator;

	Ecore_Timer *event_freeze_timer;

	Setting_GenGroupItem_Data *data_home;
	Setting_GenGroupItem_Data *data_back;
	Setting_GenGroupItem_Data *data_auto_rotate;
	Setting_GenGroupItem_Data *data_tts;

	setting_view *view_to_load;
	Evas_Object *popup_chk;
	Evas_Object *popup;

	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_screen_mode;
	Elm_Genlist_Item_Class itc_2text_3;
	Elm_Genlist_Item_Class itc_1text_1icon_divider;
	Elm_Genlist_Item_Class itc_1icon;
	Elm_Genlist_Item_Class itc_grp_title;
	Elm_Genlist_Item_Class itc_1icon_1button;
	Elm_Genlist_Item_Class itc_variable_height;
	Elm_Genlist_Item_Class itc_2text_3_parent_backlight;
	Elm_Genlist_Item_Class itc_2text_3_parent_touch_duration;
	Elm_Genlist_Item_Class itc_1icon_1text_sub_back;
	Elm_Genlist_Item_Class itc_1icon_1text_sub_touch;
	Elm_Genlist_Item_Class itc_multiline_text;
	Elm_Genlist_Item_Class itc_multiline_overheating;

	/*Improve response speed */
	Ecore_Idler *set_idler;
	Ecore_Timer *set_timer;

	DBusConnection *bus;
	char *uri;
	GList *pkg_list;

	/*for smart screen try it*/
	Evas_Object *smart_stay_image;
	Evas_Object *smart_rotation_image;
	Evas_Object *smart_stay_sublayout;
	Evas_Object *smart_rotation_sublayout;
	Ecore_Timer *timer_show_guide;
};

extern setting_view setting_view_applications_main;
extern setting_view setting_view_applications_defaultapp;

typedef struct _default_app {
	Elm_Object_Item *item;

	int defapp;
	char *pkgid;
	char *icon_path;
	char *pkg_label;

} default_app;

void setting_applications_layout_ug_cb(
		ui_gadget_h ug, enum ug_mode mode, void *priv);

#endif				/* __SETTING_APPLICATION_H__ */
