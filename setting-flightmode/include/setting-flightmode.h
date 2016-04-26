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

#include "setting-common-init.h"
#include "setting-common-data-type.h"
#include "setting-common-view.h"
#include "setting-common-draw-widget.h"

#include <Elementary.h>

#define FLIGHTMODE_LOCALEDIR	_TZ_SYS_RO_APP\
	"/org.tizen.setting-flightmode/res/locale"

typedef enum _MODE_OPEARTION {
	MODE_INVALID = 0,	/* There is no request waiting for
				processing */
	MODE_ENTER = 1,		/* Some "flightmode enter" request is waiting
				for processing */
	MODE_LEAVE,		/* Some "flightmode leave" request is waiting
				for processing */
} MODE_OPEARTION;

typedef Setting_GenGroupItem_Data *(*UI_HANDLER)(void *data, char *title,
		char *iconpath, char *ugname, Evas_Object *genlist,
		Elm_Object_Item *parent);

typedef enum {
	ERROR_STATE = -1,
	DEFAULT_UI = 0,
	UI_PROC,
	UG_HANDLE,
} mainlist_entry_action_type;

typedef struct _mainlist_entry {
	char *title;
	UI_HANDLER ui_handler;
	mainlist_entry_action_type type;
	/*-------------------------------------------------- */
	void *item_data;
} mainlist_entry;

typedef struct _SettingFlightMode {
	MainData md;
	Evas_Object *bg;
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
	bool b_fm_requesting; /* whether some request is processing;
				1:yes, 0:no */
	MODE_OPEARTION fm_waiting_op;
} SettingFlightMode;

extern setting_view setting_view_flightmode_main;
#endif
