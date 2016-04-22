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
#ifndef __SETTING_FLIGHTMODE_H__
#define __SETTING_FLIGHTMODE_H__

#include <Elementary.h>
#include <notification.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-common-resource.h>
#include <setting-common-data-slp-setting.h>

#include <ITapiModem.h>
#include <TapiUtility.h>
#include <tapi_common.h>

typedef struct _SettingFlightModeUG SettingFlightModeUG;

typedef enum _MODE_OPEARTION {
	MODE_INVALID = 0, /*< There is no request waiting for processing */
	MODE_ENTER = 1,	/*< Some "flightmode enter" request is waiting for processing */
	MODE_LEAVE,		/*< Some "flightmode leave" request is waiting for processing */
} MODE_OPEARTION;


struct _SettingFlightModeUG {
	ui_gadget_h ug;
	ui_gadget_h ug_loading;
	/* add more variables here (move your appdata to here) */
	app_control_h bundle_data; /**< when loading this UG, bundle is stored */

	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *bg;
	Evas_Object *ly_main;
	Evas_Object *navi_bar;	/* for navigation effect */
	Evas_Object *scroller;
	Evas_Object *popup_flight_mode;

	Eina_List *app_list;
	Eina_List *app_check_list;
	Elm_Genlist_Item_Class itc_1text_2icon;
	Setting_GenGroupItem_Data *data_flightmode;

	/* UI elements - view main */
	setting_view *view_to_load;

	/*notification hander */
	int noti_id;

	/**
	 * vars for flightmode
	 */
	TapiHandle *handle;
	bool b_fm_requesting; /**< whether some request is processing. 1:yes, 0:no */
	MODE_OPEARTION fm_waiting_op;
};

extern setting_view setting_view_flightmode_main;
#endif
