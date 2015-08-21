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
#include <efl_assist.h>
#include <dd-display.h>
/*#include <tethering.h> */

#define NETCONFIG_SERVICE			"net.netconfig"
#define NETCONFIG_WIFI_INTERFACE		NETCONFIG_SERVICE ".wifi"
#define NETCONFIG_WIFI_PATH			"/net/netconfig/wifi"

#define FEATURE_UNAVALIABLE_WHILE_TTS_ON "IDS_SCR_POP_SCREEN_READER_IS_NOT_SUPPORTED_BY_PS_PRESS_THE_BACK_KEY_TO_RETURN_TO_THE_PREVIOUS_SCREEN"

#define NO_UG_FOUND_MSG 			"IDS_MSGF_POP_UNSUPPORTED"
#define USB_NEED_OFF "Please turn USB tethering off first in Mobile hotspot"

#define Kies_Caring_Str "IDS_KW_POP_BE_CAREFUL_WHEN_TRANSMITTING_INFORMATION_ON_AN_UNSECURED_WI_FI_NETWORK_MSG"
#define Kies_Warrning_Str "IDS_USB_POP_UNABLE_TO_CONNECT_TO_KIES_VIA_WI_FI_WHEN_CONNECTED_TO_KIES_VIA_USB"

#define _2_TAB_STR 		Keystr_Connection
#define _3_TAB_STR 		KeyStr_Device
#define _4_TAB_STR 		KeyStr_MotionControl /**/
#define _5_TAB_STR 		"IDS_ST_BODY_GENERAL"
#define _6_TAB_STR 		KeyStr_DownloadedAPPs

#define LIST_VIEW "IDS_CLD_BUTTON_LIST_VIEW"
#define GRID_VIEW "IDS_ST_BODY_GRID_VIEW"
#define TAB_VIEW "IDS_ST_OPT_TAB_VIEW"
#define EDIT_VIEW "IDS_ST_OPT_EDIT_QUICK_SETTINGS"


#define Keystr_Apps			"IDS_ST_BODY_APPLICATION_SETTINGS"
#define Keystr_All			"IDS_ST_OPT_ALL"
#define Keystr_MostUsed		"IDS_ST_TZTAB4_MOST_USED_M_APP"
#define Keystr_Connection	"IDS_ST_BODY_CONNECTION"
#define Keystr_Device		"IDS_ST_BODY_DEVICE"
#define Keystr_General		"IDS_ST_BODY_GENERAL"


#define USER_DEFINED_SETTING_STR "IDS_ST_HEADER_USER_SETTINGS_ABB"
#define FREQUENTLY_SETTING_STR "IDS_ST_BODY_FREQUENTLY_USED"
#define EMPTY_STR "IDS_ST_BODY_EMPTY"

#define ENABLE_FLIGHT_MODE_MSG "IDS_ST_BODY_FLIGHT_MODE_MSG"

/**
 * @brief
 */
typedef enum _Setting_Tab_View_Type {
    SETTING_TAB_VIEW_INVALID = -1,
    SETTING_TAB_VIEW_CONNECTION = 0,
    SETTING_TAB_VIEW_DEVICE = 1,
    SETTING_TAB_VIEW_CONTROL = 2,
    SETTING_TAB_VIEW_GENERAL = 3,
    SETTING_TAB_VIEW_APPS = 4,
    SETTING_TAB_VIEW_MAX,
} Setting_Tab_View_Type;

typedef enum _setting_view_type {
    SETTING_INVALID_VIEW = -1,
    SETTING_LIST_VIEW = 0,
    SETTING_MAX,
} setting_view_type;


typedef void (*menu_list_load_fn)(void *data);
typedef void (*menu_list_load_fn2)(void *, Cfg_Item_Position, Evas_Object *);

#define MAX_GROUP_NUM_IN_VIEW 10

/**
 * enum for genlist item to be updated
 * @see setting_update_gl_item
 */
typedef enum _Update_GL_Item_Type {
    GL_ITEM_WIFI = 0,
    GL_ITEM_BT,
    GL_ITEM_NFC,
    GL_ITEM_MAP,
    GL_ITEM_USB,

    GL_ITEM_ROTATION,
    GL_ITEM_FLIGHT,
    GL_ITEM_MAX
} Update_GL_Item_Type;

typedef enum _FLIGHT_MODE_OPEARTION {
    FM_INVALID = 0, /*< There is no request waiting for processing */
    FM_ENTER = 1,	/*< Some "flightmode enter" request is waiting for processing */
    FM_LEAVE,		/*< Some "flightmode leave" request is waiting for processing */
} FLIGHT_MODE_OPEARTION;

struct _GridItem {
	const char *name;				/**< name below the icon image 	*/
	const char *icon_path;		/**< icon path (full path) 		*/
	const char *ug_name;			/**< UG name 					*/
	void *data;				/* app context pointer 			*/
	Elm_Object_Item *item;	/**< gengrid object item */
};
typedef struct _GridItem GridItem;

#define MAX_MORE_MENU_NUM	6
#define WIFI_LAST_ON_OFF_STATE "PREFKEY_WIFI_ON_OFF"

enum {
    OP_WIFI_TURNING_INVALID = 0x00,
    OP_WIFI_TURNING_OFF,
    OP_WIFI_TURNING_ON,
};

/*driving mode */
#define PREFKEY_SETTING_MAIN_DRIVING_MODE_ON_POPUP_REMINDER 		"setting/main/driving_mode/on/popup_reminder"

enum {
    SC_All_List,
    SC_All_Grid,
    SC_Edit_List,
    SC_Edit_Grid,

    /*to delete */
    SC_All,
    SC_Top,
    SC_Max
}; /*genlist or gengrid */

enum {
    VIEW_All_List,
    VIEW_Max
};


enum {
    GL_WiFi = 0,

    GL_BT,
    GL_FlightMode,
    GL_NetRestriction,
    GL_MobileApp,
    GL_Network,

    GL_NearByDev,
    GL_NFC,
    GL_SBeam,
    GL_Motion,
    GL_GuestMode,

    GL_Bright,
    GL_Backlight,
    GL_Battery,
    GL_MultiWin,
    GL_BM,

    GL_DM,
    GL_PM,
    GL_PersonalPage,
    GL_TouchSens,
    GL_TouchkeyLight,

    GL_Landscape,
    GL_BlockingMode,
    GL_Screentone,
    GL_Screenmode,

    GL_LanguageInput,
    GL_Storage,
    GL_DateTime,
    GL_About,

    GL_Group_Connection,
    GL_Group_Device,
    GL_Group_Personal,
    /*GL_Group_UserBackup, */
    GL_Group_System,
    GL_Group_Apps,
    GL_Max
};

/**
 * @brief main view context
 */
typedef struct _setting_main_appdata {

	ui_gadget_h ug; 					/**< ug */

	TapiHandle *handle;
	Evas_Object *popup;

	Evas_Object *ly_editview;			/**< Edit mode */

	Evas_Object *popup_kies_via_wifi;
	Evas_Object *popup_wifi_mode;
	Evas_Object *popup_flight_mode;
	Evas_Object *popup_blocking_mode;
	Evas_Object *popup_net_restriction_mode;
	Evas_Object *popup_palmmotion_tts;
	Evas_Object *popup_palmmotion_enable;

	Evas *evas;
	Evas_Object *win_main;

	Evas_Object *navibar_main;
	Elm_Object_Item *main_navi_it;

	Evas_Object *ly_main;			/**< seting view main */
	Evas_Object *sc_gl[SC_Max];		/**< genlist array */
	Evas_Object *sc_gg[SC_Max];		/**< gengrid array */

	Setting_GenGroupItem_Data *gl_data_item[VIEW_Max][GL_Max];

	bool isInUGMode;				/**< set TRUE when UG is running */

	/**
	 * vars for flightmode
	 */
	bool b_fm_requesting; /**< whether some request is processing. 1:yes, 0:no */
	FLIGHT_MODE_OPEARTION fm_waiting_op;

	/* special layout */
	Setting_GenGroupItem_Data *data_flight;
	/*Setting_GenGroupItem_Data *data_multi_window; */
	Setting_GenGroupItem_Data *data_netrestrictionmode;
	Setting_GenGroupItem_Data *data_wifi;
	Setting_GenGroupItem_Data *data_bt;

	Setting_GenGroupItem_Data *data_mobileApp;

	Setting_GenGroupItem_Data *data_bright;
	Setting_GenGroupItem_Data *data_backlight;
	Setting_GenGroupItem_Data *data_battery;
	Setting_GenGroupItem_Data *data_landscape;
	Setting_GenGroupItem_Data *data_screentone;
	Setting_GenGroupItem_Data *data_screenmode;
	Setting_GenGroupItem_Data *data_touchkey_light_duration;
	Setting_GenGroupItem_Data *data_powersaving;
	Setting_GenGroupItem_Data *data_driving;
	Setting_GenGroupItem_Data *data_blocking;
	Setting_GenGroupItem_Data *data_personalpage;
	Setting_GenGroupItem_Data *data_guest;
	Setting_GenGroupItem_Data *data_high_touch_sensitivity;

	Setting_GenGroupItem_Data *data_moreSystem;
	Setting_GenGroupItem_Data *data_moreConnect;
	Setting_GenGroupItem_Data *data_nfc;
	Setting_GenGroupItem_Data *data_motion;
	Setting_GenGroupItem_Data *data_nearby_devices;

#if 1
	Setting_GenGroupItem_Data *data_language_input;
	Setting_GenGroupItem_Data *data_storage;
	Setting_GenGroupItem_Data *data_datetime;
	Setting_GenGroupItem_Data *data_about;
#endif

	/*following are not for update.. */
	Setting_GenGroupItem_Data *data_network;

	/*--------------------------------------------------------------- */
	Setting_Tab_View_Type view_type;
	Evas_Object *select_sub_view[SETTING_TAB_VIEW_MAX];
	GridItem *select_edit_mode_item;
	Evas_Object *edit_genlist;
	Eina_List *edit_sel_list;
	Elm_Object_Item *edit_navi_it;
	Elm_Object_Item *select_ctrl_item[3];

	Evas_Object *main_genlist;				/**< 'Setting main' in main page */

	Elm_Genlist_Item_Class itc[GENDIAL_Type_MAX];
	Elm_Genlist_Item_Class itc_no_reult;

	Setting_GenGroupItem_Data *check_all_item; /**< genlist node for "Select all" */

	Evas_Object *data_roaming_popup;

	/*driving mode */
	Evas_Object *driving_mode_popup;
	Evas_Object *driving_mode_popup_chk;

	/* Personal page */
	char *personal_data;

	/* listen / unlisten code */
	Eina_List *listened_list;
	sensor_h sf_handle;

	const char *more_connective_menus[MAX_MORE_MENU_NUM];
	const char *more_system_menus[MAX_MORE_MENU_NUM];

	Evas_Object *webkit;
	Evas_Object *conform;

	/* NEW UX --------------------------------------------------- */
	Elm_Object_Item *ctrl_item[SETTING_TAB_VIEW_MAX];  /* 0:Grid, 1:General 2:Applications */
	Evas_Object *sub_view[SETTING_TAB_VIEW_MAX];       /* 0:Grid, 1:General 2:Applications */
	Evas_Object *view_layout;       /* view layout */
	Evas_Object *ctrl_bar;

	Eina_Bool click_flag;
	Elm_Gengrid_Item_Class *gic;
	Elm_Gengrid_Item_Class groupic;
	Elm_Gengrid_Item_Class editGic;
	Ecore_Timer *save_idler;

	Ecore_Timer *update_idler;

	Ecore_Timer *bt_timer;
	Ecore_Timer *nfc_timer;
	Ecore_Timer *wifi_timer;
	Ecore_Idler *update_font_idler;
	Ecore_Timer *dashboard_timer;
	int current_rotation;
	int noti_id;
	int driving_noti_id;
	int flight_noti_id;
	/* ---------------------------------------------------------- */

	Ecore_Timer *event_freeze_timer;

	/* FEATURE : setting search */
	/* ly_topview */
	/* search app */
	Evas_Object *ly_topview;
	Evas_Object *search_genlist;
	int search_count;
	int cur_count;
	ListItemObject *search_selected_item_data;

	account_subscribe_h account_subscribe_handle;
	Evas_Object *search_bar;
	char search_text[512 + 1];
	char current_search_text[512 + 1];
	Ecore_Idler *search_idler;
	Eina_Bool nfc_try_activate_running;

	Elm_Object_Item *grid_item[6];

	bundle *b;
	int wifi_op;

	Cfg_Item_Type is_searchmode;

	Setting_GenGroupItem_Data *data_temp;

	/*cis-panel */
	Evas_Object *cis_left_panel;
	Evas_Object *cis_left_panel_list;

	bool	cis_style_gengrid;

	int scrl_type;

	Eina_Hash *main_list_hash;

} setting_main_appdata;
extern setting_main_appdata *g_main_ad;

/* launch view */
extern setting_view setting_view_main;
extern setting_view setting_view_edit_mode;

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
