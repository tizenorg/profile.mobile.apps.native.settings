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

#include <setting-common-view.h>
#include <setting-common-search.h>

#define EXPORT_PUBLIC __attribute__ ((visibility ("default")))

/*  TAPI*/
#include <tapi_common.h>
#include <ITapiModem.h>

#include <sensor.h>
#include <notification.h>
#include <account.h>
#include <app_manager.h>
#include <efl_extension.h>
#include <dd-display.h>
#include <call-manager.h>


#define NO_UG_FOUND_MSG 	"IDS_MSGF_POP_UNSUPPORTED"

#define Keystr_Connection	"IDS_ST_BODY_CONNECTION"

#define MAX_GROUP_NUM_IN_VIEW 10


typedef enum _FLIGHT_MODE_OPEARTION {
	FM_INVALID = 0, /*< There is no request waiting for processing */
	FM_ENTER = 1,	/*< Some "flightmode enter" request is waiting for processing */
	FM_LEAVE,		/*< Some "flightmode leave" request is waiting for processing */
} FLIGHT_MODE_OPEARTION;


#define MAX_MORE_MENU_NUM	6
#define WIFI_LAST_ON_OFF_STATE "PREFKEY_WIFI_ON_OFF"

enum {
	OP_WIFI_TURNING_INVALID = 0x00,
	OP_WIFI_TURNING_OFF,
	OP_WIFI_TURNING_ON,
};

enum {
	SC_All_List,
	SC_Max
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

	ui_gadget_h ug; 					/**< ug */

	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *navibar_main;
	Elm_Object_Item *navibar_main_it;

	Evas_Object *ly_main;			/**< seting view main */
	Evas_Object *sc_gl[SC_Max];		/**< genlist array */

	/* listen / unlisten code */
	Eina_List *listened_list;
	Evas_Object *conform;
	Evas_Object *view_layout;       /* view layout */

	Elm_Genlist_Item_Class itc_table[GENDIAL_Type_MAX];

	TapiHandle *handle;
	Evas_Object *popup;

	Setting_GenGroupItem_Data *data_network; /** special layout */
	bool isInUGMode;				/**< set TRUE when UG is running */

	int current_rotation;
	/* ---------------------------------------------------------- */
	Ecore_Timer *event_freeze_timer;

	/*bundle *b; */

	Cfg_Item_Type is_searchmode;
	Eina_Hash *main_list_hash;

} setting_main_appdata;
extern setting_main_appdata *g_main_ad;

/* launch view */
extern setting_view setting_view_main;

extern void __gl_realized(void *data, Evas_Object *obj, void *event_info);
extern void setting_main_refresh_topview_list(void *data);

extern Evas_Object *__get_top_list(void *data);
extern void setting_main_click_list_ex_ug_cb(void *data, Evas_Object *obj, void *event_info);
extern Eina_Bool __save_timer(void *data);

typedef Setting_GenGroupItem_Data *(*UI_HANDLER)(void *data, char *title, char *iconpath, char *ugname, Evas_Object *genlist, Elm_Object_Item *parent);


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
