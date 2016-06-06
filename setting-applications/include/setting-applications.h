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
#ifndef __SETTING_DISPLAY_H__
#define __SETTING_DISPLAY_H__

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

#define SAFE_STRDUP(src) (src)?strdup(src):NULL

#define Keystr_Dynamic		"IDS_ST_BODY_DYNAMIC_T_DISPLAY_EFFECT"
#define Keystr_Standard		"IDS_ST_BODY_STANDARD_T_DISPLAY_EFFECT"
#define Keystr_Natural		"IDS_ST_BODY_NATURAL_T_DISPLAY_EFFECT"
#define Keystr_Movie		"IDS_ST_BODY_MOVIE_T_DISPLAY_EFFECT"

#define LAUNCH_EFFECT_STR "IDS_ST_BODY_APP_OPENING_EFFECT"
#define LAUNCH_IMAGE_STR  "IDS_ST_BODY_APP_OPENING_IMAGE"

#define KeyStr_AdjustScreenTone_Dsc "IDS_ST_BODY_SAVE_POWER_BY_ADJUSTING_"\
	"SCREEN_TONE_ACCORDING_TO_ANALYSIS_OF_IMAGES"
#define KeyStr_HighTouchSensitivity_Dsc "IDS_ST_SBODY_INCREASE_THE_"\
	"SENSITIVITY_OF_THE_TOUCH_SCREEN_SBODY_MSG"

/* Increase the screen's sensitivity so you can touch it while wearing gloves */
#define LAUNCH_BATTERY_DESC "IDS_ST_BODY_SHOWS_BATTERY_PERCENTAGE_ON_INDICATOR"

#define SETTING_DISPLAY_TIME_15_SEC_STR "IDS_ST_BODY_15SEC"
#define SETTING_DISPLAY_TIME_30_SEC_STR "IDS_ST_BODY_30SEC"
#define SETTING_DISPLAY_TIME_1_MIN_STR "IDS_ST_BODY_1_MINUTE"

#define SETTING_DISPLAY_TIME_2_MIN_STR "IDS_ST_BODY_2_MINUTES"
#define SETTING_DISPLAY_TIME_5_MIN_STR "IDS_ST_BODY_5_MINUTES"
#define SETTING_DISPLAY_TIME_10_MIN_STR "IDS_ST_BODY_10_MINUTES"
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

#define POPUP_TURN_ON_STR "IDS_ST_BUTTON_ENABLE"

#define SAMRT_SCREEN_TRY_IT_IAMGE_PORTRAIT_H 2636
#define SAMRT_SCREEN_TRY_IT_IAMGE_PORTRAIT_W 720
#define SAMRT_SCREEN_TRY_IT_IAMGE_LANDSCAPE_H 2055
#define SAMRT_SCREEN_TRY_IT_IAMGE_LANDSCAPE_W 1280
#define SETTING_DISPLAY_ICON_PATH 12

typedef enum {
	SCREENMODE_DYNAMIC = 0,
	SCREENMODE_STANDARD,
	SCREENMODE_NATURAL,
	SCREENMODE_MOVIE,
	SCREENMODE_MAX
} screenmode_t;

typedef enum {
	DISPLAY_EXPAND_NONE = 0,
	DISPLAY_EXPAND_BACKLIGHT,
	DISPLAY_EXPAND_TOUCH_KEY_LIGHT_DURATION
} ApplicationsExpandState;


typedef struct _Ctxpopup_Data {
	Evas_Object *ctx;
	Evas_Object *btn;
} Ctxpopup_Data;

typedef enum {
	SETTING_DISPLAY_VIEWTYPE_MAIN,
	SETTING_DISPLAY_VIEWTYPE_BRIGHTNESS,
	SETTING_DISPLAY_VIEWTYPE_BACKLIGHT,
	SETTING_DISPLAY_VIEWTYPE_SMARTSCREEN,
	SETTING_DISPLAY_VIEWTYPE_MAX,
} applications_viewtype;

typedef struct _SettingApplicationsUG SettingApplicationsUG;

#define BRIGHTNESS_DISPLAY 0

/**
 * Setting Applications UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingApplicationsUG and the functions access app context.
 */
struct _SettingApplicationsUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	ui_gadget_h ug_loading;
	ui_gadget_h ug_appmgr;
	ui_gadget_h ug_homescreen;
	ui_gadget_h ug_passwd;
	ui_gadget_h ug_ledindicator;

	Evas_Object *ly_main;

	Evas_Object *navi_bar;	/*	for navigation effect */
	Elm_Object_Item *nf_it;
	Ecore_Timer *event_freeze_timer;


	Setting_GenGroupItem_Data *data_home;

	Setting_GenGroupItem_Data *data_screen_mode;
	Setting_GenGroupItem_Data *data_br;
	Setting_GenGroupItem_Data *data_back;
	Setting_GenGroupItem_Data *data_br_auto;
	Setting_GenGroupItem_Data *data_br_sli;
	Setting_GenGroupItem_Data *data_adjust;
	Setting_GenGroupItem_Data *data_overheating;

	Setting_GenGroupItem_Data *data_br_adjustment;

	Setting_GenGroupItem_Data *data_auto_rotate;
	Setting_GenGroupItem_Data *data_tts;

	/* smart screen */
	Setting_GenGroupItem_Data *data_smart_stay;
	Setting_GenGroupItem_Data *data_smart_rotation;
	Setting_GenGroupItem_Data *data_auto_adjust_scrn_tone;
	Setting_GenGroupItem_Data *data_dynamic;


	Evas_Object *more_popup;
	Ctxpopup_Data ctx_data;

	Setting_GenGroupItem_Data *tilting_head;
	Setting_GenGroupItem_Data *tilting_device;
	Setting_GenGroupItem_Data *smart_scroll_speed;
	Setting_GenGroupItem_Data *visual_feedback_display;
	Setting_GenGroupItem_Data *scroll_speed_slider;
	Setting_GenGroupItem_Data *try_tilting_head;
	Setting_GenGroupItem_Data *try_tilting_device;
	Setting_GenGroupItem_Data *data_smart_screen_image;
	Setting_GenGroupItem_Data *data_smart_screen_rotation_image;

	/*int bright_value; */
	int low_bat;

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


	ui_gadget_h ug_gallery;
	ui_gadget_h ug_imageviewer;
	Eina_Bool isLoading;
	Setting_GenGroupItem_Data *data_backlight_always_on;
	Setting_GenGroupItem_Data *data_backlight_15sec;
	Setting_GenGroupItem_Data *data_backlight_30sec;
	Setting_GenGroupItem_Data *data_backlight_1min;
	Setting_GenGroupItem_Data *data_backlight_2min;
	Setting_GenGroupItem_Data *data_backlight_5min;
	Setting_GenGroupItem_Data *data_backlight_10min;
	/*Improve response speed */
	Ecore_Idler *set_idler;
	Ecore_Timer *set_timer;

	/* genlist */
	Evas_Object *genlist;

	DBusConnection *bus;
	char *uri;

	GList *pkg_list;

	/*for brightness*/
	Eina_Bool is_event_registered;
	int last_requested_level;
	applications_viewtype applications_viewtype;

	/*for smart screen try it*/
	Evas_Object *smart_stay_image;
	Evas_Object *smart_rotation_image;
	Evas_Object *smart_stay_sublayout;
	Evas_Object *smart_rotation_sublayout;
	Ecore_Timer *timer_show_guide;

	ApplicationsExpandState applications_expand_state;

};

extern setting_view setting_view_applications_main;
extern setting_view setting_view_applications_defaultapp;

typedef struct _default_app{
	Elm_Object_Item *item;

	int defapp;
	char *pkgid;
	char *icon_path;
	char *pkg_label;

} default_app;

void setting_applications_layout_ug_cb(
		ui_gadget_h ug, enum ug_mode mode, void *priv);

Eina_Bool __show_smartstay_guide_popup(void *data);
Eina_Bool __show_smartrotation_guide_popup(void *data);

extern void destruct_brightness(void *data);
extern void construct_brightness(void *data, Evas_Object *genlist);

#endif				/* __SETTING_DISPLAY_H__ */
