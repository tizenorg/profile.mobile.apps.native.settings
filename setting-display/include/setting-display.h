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

#include <Elementary.h>


#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <device.h>
#include <dd-display.h>
#include <dd-deviced.h>
#include <dbus/dbus.h>
/*#include <devman.h> */

#define Keystr_Dynamic		"IDS_ST_BODY_DYNAMIC_T_DISPLAY_EFFECT"
#define Keystr_Standard		"IDS_ST_BODY_STANDARD_T_DISPLAY_EFFECT"
#define Keystr_Natural		"IDS_ST_BODY_NATURAL_T_DISPLAY_EFFECT"
#define Keystr_Movie		"IDS_ST_BODY_MOVIE_T_DISPLAY_EFFECT"

#define LAUNCH_EFFECT_STR "IDS_ST_BODY_APP_OPENING_EFFECT"
#define LAUNCH_IMAGE_STR  "IDS_ST_BODY_APP_OPENING_IMAGE"

#define LAUNCH_BATTERY_DESC  "IDS_ST_BODY_SHOWS_BATTERY_PERCENTAGE_ON_INDICATOR" /*"Increase the screen's sensitivity so you can touch it while wearing gloves" */

#define SETTING_DISPLAY_TIME_15_SEC_STR "IDS_ST_BODY_15SEC"
#define SETTING_DISPLAY_TIME_30_SEC_STR "IDS_ST_BODY_30SEC"
#define SETTING_DISPLAY_TIME_1_MIN_STR "IDS_ST_BODY_1MINUTE"
#define SETTING_DISPLAY_TIME_2_MIN_STR "IDS_ST_BODY_2_MINUTES"
#define SETTING_DISPLAY_TIME_5_MIN_STR "IDS_ST_BODY_5_MINUTES"
#define SETTING_DISPLAY_TIME_10_MIN_STR "IDS_ST_BODY_10_MINUTES"


#define URI_SMART_STAY "tizen-help://org.tizen.setting/smartstay"
#define URI_SMART_ROTATION "tizen-help://org.tizen.setting/smartrotation"

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
} DisplayExpandState;


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
} display_viewtype;

typedef struct _SettingDisplayUG SettingDisplayUG;

#define BRIGHTNESS_DISPLAY 0

/**
 * Setting Display UG context
 * all UG function has void* as an agument. this is casted back to SettingDisplayUG
 * and the functions access app context.
 */
struct _SettingDisplayUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	ui_gadget_h ug_loading;
	ui_gadget_h ug_font;
	ui_gadget_h ug_passwd;
	ui_gadget_h ug_ledindicator;

	Evas_Object *ly_main;	/**< link to src/setting-display-main.c */

	Evas_Object *navi_bar;	/*  for navigation effect */
	Elm_Object_Item *nf_it;
	Ecore_Timer *event_freeze_timer;

	Evas_Object *screen_timeout_popup;
	Evas_Object *screen_timeout_rdg;

	Setting_GenGroupItem_Data *data_screen_mode;
	Setting_GenGroupItem_Data *data_br;
	Setting_GenGroupItem_Data *data_font;
	Setting_GenGroupItem_Data *data_back;
	//Setting_GenGroupItem_Data *data_touchkey_light_duration;
	Setting_GenGroupItem_Data *data_br_auto;
	Setting_GenGroupItem_Data *data_br_sli;
	Setting_GenGroupItem_Data *data_adjust;
	Setting_GenGroupItem_Data *data_overheating;

	Setting_GenGroupItem_Data *data_br_adjustment;

	Setting_GenGroupItem_Data *data_auto_rotate;

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
	Setting_GenGroupItem_Data *smart_screen_help;
	Setting_GenGroupItem_Data *try_tilting_head;
	Setting_GenGroupItem_Data *try_tilting_device;
	Setting_GenGroupItem_Data *data_smart_screen_image;
	Setting_GenGroupItem_Data *data_smart_screen_rotation_image;

	/*int bright_value; */
	int low_bat;

	setting_view *view_to_load;

	Evas_Object *popup_chk;
	Evas_Object *popup;

	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_screen_mode;
	Elm_Genlist_Item_Class itc_2text_3;
	Elm_Genlist_Item_Class itc_1icon;


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
	Ecore_Event_Handler *help_event_handler;
	Evas_Object *help_popup;
	char *uri;

	/*for brightness*/
	Eina_Bool is_event_registered;
	int last_requested_level;
	display_viewtype display_viewtype;

	/*for smart screen try it*/
	Evas_Object *smart_stay_image;
	Evas_Object *smart_rotation_image;
	Evas_Object *smart_stay_sublayout;
	Evas_Object *smart_rotation_sublayout;
	Ecore_Timer *timer_show_guide;

	DisplayExpandState display_expand_state;

};

extern setting_view setting_view_display_main;
extern setting_view setting_view_display_brightness;
extern setting_view setting_view_display_backlight;

void setting_display_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
                                  void *priv);

extern int setting_display_set_auto_adjust_tone(int value);
Eina_Bool __show_smartstay_guide_popup(void *data);
Eina_Bool __show_smartrotation_guide_popup(void *data);

extern void destruct_brightness(void *data);
extern void construct_brightness(void *data, Evas_Object *genlist);


#endif				/* __SETTING_DISPLAY_H__ */
