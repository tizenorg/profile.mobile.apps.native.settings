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
 * @mainpage Settings documentation
 *
 * @section overview Detailed Description
 *
 * @image html org.tizen.setting.screen1.png
 * @image html org.tizen.setting.screen2.png
 * @image html org.tizen.setting.screen3.png
 *
 * \n
 * <b>App ID:</b> org.tizen.setting\n
 *
 * Settings app main purpose is to give user possibility to configure main
 * system settings and maintain the telephone.\n
 *
 * On the application's main screen you can find list of possible settings -
 * icon and setting description. The list is divided in four sections:\n
 *
 * - <b>Connections</b>
 * \n- Wi-Fi - To manage Wi-Fi connections and options.
 * \n- Bluetooth - The Bluetooth service.
 * \n- Flight mode - To disable calling and messaging function and turn off
 * mobile data/Wi-Fi/Bluetooth.
 * \n- Tethering - The icon will be shown when GPS module is in use.
 * \n- Mobile networks - The icon will be shown when GPS module is in use.
 * \n- NFC - To manage NFC options.
 * \n- More connections - The location service settings.
 * - <b>Device</b>
 * \n- Sound - The settings to manage ringtones for calls and volume of
 * notification, media, system.
 * \n- Display - To choose screen brightness, font size or screen timeout.
 * \n- Applications - The Application settings where user can manage application
 * installed on the platform.
 * - <b>Personalization</b>
 * \n- Wallpapers - The settings to manage wallpapers.
 * \n- Lock Screen - The locksreen settings. to setup screen lock type etc.
 * \n- Accounts - The settings to manage user accounts in the system.
 * \n- Privacy and safety - The privacy service settings.
 * - <b>System</b>
 * \n- Language and input - The settings to choose displayed language and TTS
 * options.
 * \n- Storage - The Storage settings which show usage space in the device.
 * \n- Date and time - To setup time in the system.
 * \n- About device - Information about device, software version, serial number,
 * etc.
 *
 *
 * The <b>Connections</b> section contains <i>Wi-Fi</i>, <i>Bluetooth</i>,
 * <i>Airplane mode</i>, <i>NFC</i>, <i>Tethering</i>, <i>Mobile networks</i>
 * settings and the button named <i>More connection settings</i> with
 * <i>Location service</i> settings\n
 *
 * In the <b>Device</b> section you can find settings for <i>Sound</i>,
 * <i>Display</i> and <i>Applications</i>\n
 *
 * The <b>Personalization</b> section has <i>Wallpapers</i>, <i>Lock screen</i>,
 * <i>Accounts</i> and <i>Privacy and safety</i> settings nodes.\n
 *
 * And the <b>System</b> section allows to configure <i>Language and input</i>,
 * <i>Storage</i> and <i>Date and time</i>. You can also press
 * <i>About device</i> to see device information.\n
 * \n
 * Exact list of setting modules depends on hardware used (for example not all
 * devices has NFC setting etc.).\n
 * Every module is an application called from main settings application or from
 * other applications. If needed, it's called with parameters (messages), and
 * it may return result (reply message) to the application, which called it.\n
 **/

#ifndef __SETTING_H__
#define __SETTING_H__

#include <setting-common-view.h>
#include <setting-common-search.h>

/*	TAPI*/
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

	ui_gadget_h ug;						/**< ug */

	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *navibar_main;
	Elm_Object_Item *navibar_main_it;

	Evas_Object *ly_main;			/**< seting view main */
	Evas_Object *sc_gl[SC_Max];		/**< genlist array */

	/* listen / unlisten code */
	Eina_List *listened_list;
	Evas_Object *conform;
	Evas_Object *view_layout;		/* view layout */

	Elm_Genlist_Item_Class itc_table[GENDIAL_Type_MAX];

	TapiHandle *handle;
	Evas_Object *popup;

	Setting_GenGroupItem_Data *data_network; /** special layout */
	bool isInUGMode;		/**< set TRUE when UG is running */

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
