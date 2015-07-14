/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SETTING_POWERSAVING_H__
#define __SETTING_POWERSAVING_H__

#include <stdio.h>
#include <string.h>

#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#define SUPPORT_UI0_3		0
#define SUPPORT_UI0_4		1

/* string resources..*/
//in main view
#define KeyStr_Powersaving_Mode "IDS_ST_BODY_POWER_SAVING_MODE"
#define KeyStr_Powersaving_Mode_DSC "Use default settings for power saving. Settings will be enabled immediately."

/* To do : strings of custom power saving will be removed. */
#define KeyStr_Powersaving_Customed_Mode "Custom power saving mode"
#define KeyStr_Powersaving_Customed "Custom power saving settings"
#define KeyStr_Powersaving_Level_DSC "Use settings for power saving. Settings will be enabled when battery level is below the value you set."

#define KeyStr_Powersaving_Tips "IDS_ST_MBODY_POWER_SAVING_TIPS"
#define KeyStr_Powersaving_About "IDS_ST_BODY_LEARN_ABOUT_POWER_SAVING"

//in "Custom settings" view
#define __TURN_Off "Turn off "
#define KeyStr_WIFI_Off __TURN_Off"Wi-Fi"
#define KeyStr_BT_Off __TURN_Off"Bluetooth"
#define KeyStr_GPS_Off __TURN_Off"GPS"
#define KeyStr_SYNC_Off __TURN_Off"Data sync"
#define KeyStr_HOTSPOT_Off __TURN_Off"Portable Wi-Fi hotspot"
#define KeyStr_Adjust_Bright "Adjust brightness"

#define KeyStr_Powersaving_AT "Enable power saving at"
#define KeyStr_Screen_Timeout "IDS_ST_BACKLIGHT_TIME"

#define SETTING_POWERSAVING_AT_10_STR "10% battery power"
#define SETTING_POWERSAVING_AT_20_STR "20% battery power"
#define SETTING_POWERSAVING_AT_30_STR "30% battery power"
#define SETTING_POWERSAVING_AT_40_STR "40% battery power"
#define SETTING_POWERSAVING_AT_50_STR "50% battery power"
/* To do : end */


//v0.4
#define KeyStr_PS_SETTINGS	"IDS_ST_BODY_POWER_SAVING_MODE_SETTINGS"
#define KeyStr_CPU_PS		"IDS_ST_BODY_CPU_POWER_SAVING"
#define KeyStr_CPU_PS_DSC	"IDS_ST_BODY_LIMIT_THE_MAXIMUM_PERFORMANCE_OF_THE_CPU"
#define KeyStr_DISPLAY_PS	"IDS_ST_BODY_SCREEN_POWER_SAVING"
#define KeyStr_DISPLAY_PS_DSC	"IDS_ST_BODY_USE_LOW_POWER_LEVEL_FOR_SCREEN"
#define KeyStr_BG_COLOR		"IDS_ST_DISPLAYSETTINGS_BODY_MAINGREETINGEDITSTYLEBGCOLOUR"
#define KeyStr_BG_COLOR_DSC	"IDS_ST_BODY_CHANGE_BACKGROUND_COLOUR_TO_SAVE_POWER_IN_EMAIL_AND_INTERNET"
#define KeyStr_SCREEN_VIB	"IDS_ST_BODY_TURN_OFF_HAPTIC_FEEDBACK"
#define KeyStr_SCREEN_VIB_DSC	"IDS_ST_BODY_TURNING_OFF_HAPTIC_FEEDBACK_CAN_EXTEND_BATTERY_LIFE"

#define SETTING_POWERSAVING_ALL_SUB_ITEMS_OFF "To turn on the Power saving mode. At least one item should be selected"

typedef struct _SettingPowersavingUG SettingPowersavingUG;

/**
 *Setting Powersaving UG context
 *all UG function has void * as an agument. this is casted back to SettingPowersavingUG
 *and the functions access app context.
 */
struct _SettingPowersavingUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *popup;
	Evas_Object *popup_chk;

	Evas_Object *ly_main;
	Evas_Object *navi_bar;

	Setting_GenGroupItem_Data *data_powersaving_active;
	Setting_GenGroupItem_Data *data_customed_active;
	Setting_GenGroupItem_Data *data_customed_setting;
	Setting_GenGroupItem_Data *data_powersaving_about;
	//v0.4
	Setting_GenGroupItem_Data *data_cpu_ps;
	Setting_GenGroupItem_Data *data_display_ps;
	Setting_GenGroupItem_Data *data_bg_color;
	Setting_GenGroupItem_Data *data_screen_vib;

	Setting_GenGroupItem_Data *data_saving_at;
	Setting_GenGroupItem_Data *data_screen_timeout;
	Setting_GenGroupItem_Data *data_wifi_off;
	Setting_GenGroupItem_Data *data_bt_off;
	Setting_GenGroupItem_Data *data_gps_off;
	Setting_GenGroupItem_Data *data_sync_off;
	Setting_GenGroupItem_Data *data_hotspot_off;
	Setting_GenGroupItem_Data *data_adjust_bright;
	Setting_GenGroupItem_Data *data_brightness;

	Setting_GenGroupItem_Data *data_br_auto;
	Setting_GenGroupItem_Data *data_br_sli;

};

extern setting_view setting_view_powersaving_main;
extern setting_view setting_view_powersaving_customed;
extern setting_view setting_view_powersaving_about;
extern setting_view setting_view_powersaving_brightness;
extern void setting_powersaving_ug_exp_cb(void *data, Evas_Object *obj, void *event_info);
extern char *setting_powersaving_ug_get_brightness_str();

#endif				/* __SETTING_POWERSAVING_H__ */
