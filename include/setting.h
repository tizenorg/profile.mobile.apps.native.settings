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
/**
 *@defgroup setting-app
 *entry point of Setting application
 */
#ifndef __SETTING_H__
#define __SETTING_H__

#define EXPORT_PUBLIC __attribute__((visibility("default")))

#include "setting-common-view.h"
#include "setting-common-search.h"
#include "setting-common-init.h"

#include <tapi_common.h>
#include <ITapiModem.h>
#include <sensor.h>
#include <notification.h>
#include <account.h>
#include <app_manager.h>
#include <efl_extension.h>
#include <dd-display.h>
#include <call-manager.h>


#define NO_UG_FOUND_MSG	"IDS_MSGF_POP_UNSUPPORTED"

#define Keystr_Connection	"IDS_ST_BODY_CONNECTION"

#define MAX_GROUP_NUM_IN_VIEW 10


typedef enum _FLIGHT_MODE_OPEARTION {
	/*< There is no request waiting for processing */
	FM_INVALID = 0,
	/*< Some "flightmode enter" request is waiting for processing */
	FM_ENTER = 1,
	/*< Some "flightmode leave" request is waiting for processing */
	FM_LEAVE,
} FLIGHT_MODE_OPEARTION;


#define MAX_MORE_MENU_NUM	6
#define WIFI_LAST_ON_OFF_STATE "PREFKEY_WIFI_ON_OFF"

enum {
	OP_WIFI_TURNING_INVALID = 0x00,
	OP_WIFI_TURNING_OFF,
	OP_WIFI_TURNING_ON,
};


enum {
	GL_Group_Connection,
	GL_Group_Device,
	GL_Group_Personal,
	GL_Group_System,
	GL_Group_Apps,
	GL_Max
};

/**
 * @brief main view context
 */
typedef struct _setting_main_appdata {

	MainData md;
	ui_gadget_h ug;					/**< ug */

	/* listen / unlisten code */
	Eina_List *listened_list;

	Evas_Object *popup;

	Setting_GenGroupItem_Data *data_network; /** special layout */
	bool isInUGMode;		/**< set TRUE when UG is running */

	int current_rotation;
	/* ---------------------------------------------------------- */
	Ecore_Timer *event_freeze_timer;

	/*bundle *b; */

	Cfg_Item_Type is_searchmode;
} setting_main_appdata;
extern setting_main_appdata *g_main_ad;

/* launch view */
extern setting_view setting_view_main;

extern void __gl_realized(void *data, Evas_Object *obj, void *event_info);
extern void setting_main_refresh_topview_list(void *data);

extern Evas_Object *__get_top_list(void *data);
extern void setting_main_click_list_ex_ug_cb(void *data, Evas_Object *obj,
		void *event_info);
extern Eina_Bool __save_timer(void *data);

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

#endif				/* __SETTING_H__ */
