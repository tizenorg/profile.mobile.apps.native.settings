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
 *@defgroup common-data-type
 *common data type
 */

#ifndef __SETTING_COMMON_DATA_TYPE_H__
#define __SETTING_COMMON_DATA_TYPE_H__

/* including Elementary lib */
#include <Elementary.h>
/* including debuging type */
#include <setting-debug.h>
/* including one class of data type */
#include <setting-common-resource.h>
/* including another class of data type */
#include <setting-common-data-error.h>

#include <setting-cfg.h>

#ifndef PREFIX
#define PREFIX "/opt"
#endif

#ifndef bool
#define bool Eina_Bool
#endif
#define PWD_FILE	_TZ_SYS_DATA"/setting/set_info"

#define SETTING_GENLIST_1ICON_STYLE			"full"
#define SETTING_GENLIST_GROUP_INDEX_STYLE		"group_index"
#define SETTING_GENLIST_ICON_1LINE_STYLE		"type1"
#define SETTING_GENLIST_2LINE_STYLE			"type1"
#define SETTING_GENLIST_MULTILINE_STYLE			"multiline"
#define SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE	"full"
#define SETTING_GENLIST_ENTRY_STYLE			"entry.main"

#define SETTING_POSTINST_FILE "/var/lib/dpkg/info/org.tizen.setting.postinst"
#define SETTING_PACKAGE		"setting"
#define SYSTEM_PACKAGE		"sys_string"
#define SETTING_LOCALEDIR	_TZ_SYS_RO_APP\
	"/org.tizen.setting/res/locale"
#define DISPLAY_LOCALEDIR		_TZ_SYS_RO_APP\
	"/org.tizen.setting-display/res/locale"

#define MODE_SYSPOPUP_PACKAGE		"mode-syspopup"
#define MODE_SYSPOPUP_LOCALEDIR	_TZ_SYS_RO_APP\
	"/org.tizen.mode-syspopup/res/locale"

#define SYSTEM_ICON_PATH	_TZ_SYS_RO_APP"/org.tizen.setting/res/icons"
#define SETTING_ICON_PATH	_TZ_SYS_RO_APP"/org.tizen.setting/res/icons"
#define SETTING_UG_ICON_PATH	_TZ_SYS_RO_UG"/res/icon/org.tizen.setting"

#define SETTING_APP_MGR_ICON_SIZE	117

#define SETTING_UG_PATH		_TZ_SYS_RO_UG"/lib"
#define SETTING_UG_PATH_USR	_TZ_SYS_RO_UG"/lib"

#define SETTING_PATH_MEMORY_OPT		"/opt/usr"
#define SETTING_PATH_SDCARD_OPT		_TZ_SYS_MEDIA"/sdcard"
#define SETTING_PATH_PARENT_SDCARD_OPT	_TZ_SYS_MEDIA"/sdcard/.."


#define SETTING_KILOBYTE_VALUE		1024
#define SETTING_MEGABYTE_VALUE		1048576
#define SETTING_GIGABYTE_VALUE		1073741824

#define SETTING_MAX_STR_LEN		30


#define SETTING_EDJ_PATH		EDJDIR
#define SETTINGS_EDJ			SETTING_EDJ_PATH"/settings.edj"
#define SETTING_THEME_EDJ_NAME		SETTING_EDJ_PATH"/setting-theme.edj"
#define SETTING_5STEP_SLIDER_EDJ_NAME	SETTING_EDJ_PATH"/font-slider.edj"
#define SETTING_GENLIST_EDJ_NAME	SETTING_EDJ_PATH"/setting-genlist.edj"
#define SETTING_NEW_GENLIST_EDJ_NAME	SETTING_EDJ_PATH"/setting-new-genlist.edj"
#define SETTING_PROC_EDJ_NAME			SETTING_EDJ_PATH"/setting-proc.edj"
#define SETTING_SLIDER_EDJ_NAME	SETTING_EDJ_PATH"/setting-slider.edj"
#define SETTING_EFF_EDJ_NAME			SETTING_EDJ_PATH"/setting-eff.edj"

#define SETTING_THEME_COLOR_THEME_EDJ_NAME		SETTING_EDJ_PATH"/setting-color-theme.edj"
/* DISPLAY PART */
#if !defined(DISPLAYEDJ)
#define DISPLAYEDJ  _TZ_SYS_RO_APP"/org.tizen.setting-display/res/edje"
#endif

#define DISPLAY_THEME_EDJ_NAME		DISPLAYEDJ"/setting-theme.edj"
#define DISPLAY_NEWUX_EDJ_NAME		DISPLAYEDJ"/setting-newux.edj"
#define DISPLAY_GENLIST_EDJ_NAME	DISPLAYEDJ"/setting-genlist.edj"
#define DISPLAY_NEW_GENLIST_EDJ_NAME	DISPLAYEDJ"/setting-new-genlist.edj"
#define DISPLAY_SLIDER_EDJ_NAME		DISPLAYEDJ"/setting-slider.edj"

#define RCS_CHECHING_FILE		_TZ_SYS_SHARE"/ims/.RCS_ENABLED"

/* inoti (broadcasting without data */
#define SETTING_TIME_CHANGED	"setting_time_changed"

/* anci c color type */
#define SETTING_FONT_RED "\033[31m"
#define SETTING_FONT_BLACK "\033[0m"
#define SETTING_FONT_BGREEN "\033[36m"

#define THEME_STYLE "beat"

#define IMG_EC_ARROW_OPEN  SETTING_ICON_PATH"org.tizen.setting.png"
#define IMG_EC_ARROW_CLOSE SETTING_ICON_PATH"org.tizen.setting.png"

#define SETTING_IDLE_IMAGE_PATH "/etc/idle_image/"

/*"naviframe_control/default" */
#define NAVI_DEF_BUTTON_STYLE  "naviframe/toolbar/default"
#define NAVI_BACK_BUTTON_STYLE "naviframe/title_left"
#define NAVI_BACK_ARROW_BUTTON_STYLE "naviframe/end_btn/default"
/*"naviframe_control/default" */
#define NAVI_MORE_BUTTON_STYLE "naviframe/more/default"


#define NAVI_CONTROLBAR_PART "controlbar"

#define BTN_STYLE_DEF "naviframe/title/default"
#define NAVI_BTN_STYLE_LEFT "title_left_btn"
#define NAVI_BTN_STYLE_RIGHT "title_right_btn"
#define NAVI_BTN_STYLE_MORE "title_more_btn"
#define NAVI_BTN_STYLE_PREV "prev_btn"

#define PROGRESSBAR_STYLE "process_medium"
#define SETTING_FONT_CONF_FILE	_TZ_SYS_ETC"/fonts/conf.avail/99-slp.conf"
#define SETTING_DEFAULT_FONT_CONF_FILE	_TZ_SYS_DATA\
	"/etc/fonts/conf.avail/99-slp.conf"



/*#define GENLIST_LIST_SUB_TEXT_SETTINGS_COLOR_STYLE	  "#3B73B6FF" */
#define BLUE_COLOR		"#3B73B6" /*3B73B6FF <== 59 115 182 255 */
#define GRAY_COLOR "#7B7B7B"
#define RED_COLOR "#FF0000"
#define WHITE_COLOR "#FFFFFF"

/**
* @brief slider, button, and etc.
*
* @param data application context
* @param obj Evas Object
* @param event_info even type
*
* @return vo return
*/

typedef void (*setting_call_back_func)(
		void *data, Evas_Object *obj, void *event_info);
typedef Eina_Bool(*setting_naviframe_cb)(void *data, Elm_Object_Item *it);

#define WIDGET_SCALE_FACTOR		(get_widgets_factor())
#define SETTING_PADDING_WIDTH		(8 * WIDGET_SCALE_FACTOR)
#define SETTING_PADDING_HEIGHT		(8 * WIDGET_SCALE_FACTOR)
#define SETTING_HALF_SCREEN_WIDTH \
	(SETTING_PADDING_WIDTH * 22 * WIDGET_SCALE_FACTOR)

#define SYS_NOTI_NAME_LEN		128
#define MAX_DISPLAY_NAME_LEN_ON_UI	128
#define MAX_COMMON_BUFFER_LEN		1024
#define BUF_SIZE			512

#define MAX_SEARCH_STR_LEN 40

#define HELP_MSG_BUFF_SIZE		1000
#define MAX_SPECIALIZITION_LEN		1000

#define PHONE_LOCK_ATTEMPS_MAX		5
#define SIM_LOCK_ATTEMPS_MAX		5
#define PHONE_LOCK_BLOCK_SECONDS	30
#define SIM_LOCK_BLOCK_SECONDS		30
#define LOCK_TIMESTAMP_LEN		64
#define MAX_DATETIME_FORMAT_LEN		64
#define MAX_DATE_FORMAT_LEN		10

#define SMALL_FONT_DPI			(-80)
#define MIDDLE_FONT_DPI			(-100)
#define LARGE_FONT_DPI			(-150)
#define HUGE_FONT_DPI			(-190)
#define GIANT_FONT_DPI			(-250)

#define LABEL_FONT_SIZE_SMALL		32
#define LABEL_FONT_SIZE_NORMAL	40
#define LABEL_FONT_SIZE_LARGE		60
#define LABEL_FONT_SIZE_HUGE		76
#define LABEL_FONT_SIZE_GIANT		100

#define STR_LABEL_FONT_SIZE_SMALL		"32"
#define STR_LABEL_FONT_SIZE_NORMAL	"40"
#define STR_LABEL_FONT_SIZE_LARGE		"60"
#define STR_LABEL_FONT_SIZE_HUGE		"76"
#define STR_LABEL_FONT_SIZE_GIANT		"100"


#define __STR_EMUL			"emul"
#define __LEN_STR_EMUL			4

#define MAX_SYSTEM_SERVICE_NUM		30
#define SYSTEM_SERVICE_LIST_PATH \
	"/opt/osp/system/configuration/system-services.list"
#define MAX_PKG_NAME_LEN			128

#define MIN_MOVE_DISTANCE 30

typedef enum _button_state {
	SETTING_ON_OFF_BTN_OFF = 0,
	SETTING_ON_OFF_BTN_ON,
	SETTING_ON_OFF_BTN_MAX
} ButtonState ;


typedef struct {
	char *locale;				/* ex) "en_US.UTF" */
	char *title;				/* "United Kingdom" */
	char *mcc;
	/*int number;				// no -	 UI */
} setting_lang_entry;

typedef enum _Theme_Type {
	Theme_Invalid = -1,
	Theme_Tizen = 0,
	Theme_Tizen_Black = 1,
	Theme_Tizen_Blue,
	Theme_ERROR
} Theme_Type;

typedef enum _GENDIAL_Type {
	GENDIAL_Type_1text_1icon_2 = 0,
	GENDIAL_Type_1icon_2text = 1,
	GENDIAL_Type_expandable_proc = 2,
	GENDIAL_Type_MAX
} GENDIAL_Type;

typedef enum _SWALLOW_Type {
	SWALLOW_Type_INVALID = -1,
	SWALLOW_Type_1ICON_1RADIO = 2,
	SWALLOW_Type_1CHECK = 3,
	SWALLOW_Type_1CHECK_RIGHT = 4,
	SWALLOW_Type_1CHECKICON = 5,
	SWALLOW_Type_1RADIO = 6,
	SWALLOW_Type_1TOGGLE =	7,
	SWALLOW_Type_1CHECK_RIGHT_INVISIBLE,
	SWALLOW_Type_1ICON_1DOTTOOGLE,
	SWALLOW_Type_1ICON_SMALL_ICON,

	SWALLOW_Type_1ICON_1IMAGE,
	SWALLOW_Type_1GIF,
	SWALLOW_Type_1BUTTON,

	SWALLOW_TYPE_1RADIO_RIGHT_PROPAGATE_EVENTS_SET,
	SWALLOW_Type_1RADIO_RIGHT,
	SWALLOW_Type_1RADIO_1BTN,
	SWALLOW_Type_1RADIO_1TEXT,

	SWALLOW_Type_1LINE_LAYOUT,
	SWALLOW_Type_LAYOUT_EDITFIELD,
	SWALLOW_Type_LAYOUT_SIMPLE_PASSWORD, /* for simple password */
	SWALLOW_Type_LAYOUT_DATEFIELD,
	SWALLOW_Type_LAYOUT_SLIDER,
	SWALLOW_Type_LAYOUT_5STEP_SLIDER,
	SWALLOW_Type_LAYOUT_SPECIALIZTION,
	SWALLOW_Type_LAYOUT_SPECIALIZTION_X,/*extension */

	SWALLOW_Type_MAX
} SWALLOW_Type;

typedef enum {
	UNDEFINED_LIB_SYMBOL = -2,/*  undefined symbol : -2 */
	LOAD_LIB_FAILED = -1,/*	 load failed : -1 */
	OPERATE_LIB_SUCESS = 0,
} OPERATE_LIB_ERROR;

typedef enum _POPUP_BTN_RESPONSE_TYPE {
	POPUP_RESPONSE_INVALID = -1,
	POPUP_RESPONSE_OK = 0,
	POPUP_RESPONSE_CANCEL = 1,
} POPUP_BTN_RESPONSE_TYPE;

/**
 * an item node in genlist
 * @todo begings_to is requried?
 * @todo isItemDisableFlag is required?
 * @todo isSinglelineFlag is requred?
 */
typedef struct _Setting_GenGroupItem_Data {

	void **pBack;
	Evas_Object *win_main;
	Evas *evas;

	Elm_Object_Item *item;

	SWALLOW_Type swallow_type;
	const char *l_swallow_path;	 /**< left image */
	const char *r_swallow_path;	 /**< right image */

	/* output */
	/**< slider, button, check, editfield, datefield */
	Evas_Object *eo_check;
	/**< represent check value, slider value, datefield value etc.. */
	int chk_status;
	setting_call_back_func chk_change_cb;

	/* radio button */
	/**< radio button */
	Evas_Object *rgd;

	char *keyStr;		/**< title */
	char *keyStr2;		/**< title for font name expanlist item*/
	char *sub_desc;		/**< sub text */
	char *guide_text;	/**< guide text for editfield */

	/*	for slider control */
	bool isIndicatorVisible;/**< indicator option (displaying number) */
	double slider_min;	/**< to swallow a slider */
	double slider_max;	/**< to swallow a slider */

	setting_call_back_func start_change_cb;
	setting_call_back_func stop_change_cb;
	setting_call_back_func maxlength_reached_cb;
	setting_call_back_func focus_cb;		   /**< for entry */
	setting_call_back_func activated_cb;		/**< for entry */
	setting_call_back_func mouse_up_cb;

	void *x_callback_cb;

	/* sound/vibration */
	/**< 1: slider belongs to sound group.
	 * 0: slider belongs to vibration group */
	int belongs_to;

	/*	for input panel	 */
	/**< input panel type */
	Elm_Input_Panel_Layout input_type;
	/** the return key type in input method panel*/
	Elm_Input_Panel_Return_Key_Type return_key_type;
	bool input_panel_disable_flag;
	/** do not autocapital the first char in edit field*/
	int disable_auto_cap;

	void *userdata;	/**< just a reference, do not need release. */

	bool isPasswordFlag;
	bool isFocusFlag;
	bool isItemDisableFlag;		/**< date/time UG only */
	bool isSinglelineFlag;
	bool maxLengthReachFlag;
	/**< for about UG, enter or space key are not allowed, if they are
	 * pressed, it is true */
	bool enterKeyPressFlag;
	/**<for about UG,if space key is pressed, need to popup a
	 * notification>*/
	bool spaceKeyPressFlag;

	bool isItemHideFlag;	/**< setting > storage only */

	/**< its relative vconf value must increase from 0, and step is 1.
	 * eg, 0, 1, 2, 3.. */
	int int_slp_setting_binded;
	/*for enty, if this is true, then entry will be focus state when the
	 * entry is created*/
	bool entry_auto_focus;

	/* popup */
	Evas_Object *notify; /**< act as any popup to show user any message */

	Elm_Entry_Filter_Accept_Set *digits_filter_data;
	Elm_Entry_Filter_Limit_Size *limit_filter_data;

	bool focus_unallowed;

	/*for incoming call*/
	Eina_Bool isPlaying;

	/* for disable item*/
	Eina_Bool disableflag;

	/*record itc style for all genlist item with check/radio/toggle */
	char *itc;

	/* storage item style */
	/* 2: app, 3: pic,video, 4: audio, 5: Misc files, 6: Avail space */
	int color;

	/* Sound, ringtone ug, fullpath */
	char *filepath;

	/* elm_datetime new */
	Evas_Object *btn_left;
	Evas_Object *btn_right;

	Evas_Object *nf;

} Setting_GenGroupItem_Data;

typedef enum {
	SETTING_VCONF_INT_TYPE = 0,
	SETTING_VCONF_BOOL_TYPE,
	SETTING_VCONF_STR_TYPE
} setting_vconf_type;

#define MAX_RESET_MENU_NUM 100

typedef struct _item_table {
	char *item_name;	/* pointer to the key string */
	char *ug_args;
	Setting_GenGroupItem_Data *data_GenItem;
} Item_Table;

typedef struct _AllowListItem AllowListItem;

struct _AllowListItem {
	char *person_id;
	char *name;
	char *thumb_path;
};
/**
 * @todo is 50 sufficient value?
 */
typedef struct _Setting_Done_List_Data {
	/* external input arguements */
	Evas_Object *win_main;
	void *UG_data;
	Evas_Object *ly_parent;
	Evas_Object *controllbar;
	Elm_Object_Item *navi_it;
	const char navi_title[MAX_DISPLAY_NAME_LEN_ON_UI];

	/* internal output%input arguements */
	Item_Table chk_items[MAX_RESET_MENU_NUM];
	int cur_item_selnum;
	int cur_item_num;
	Evas_Object *selInfoPop;
	Evas_Object *selInfoPop_layout;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1text_2icon;
	/*Elm_Genlist_Item_Class itc_seperator;*/
	Evas_Object *select_all_checkbox;
	Eina_Bool select_all_checked;
	Eina_List *list;

	AllowListItem allow_items[MAX_RESET_MENU_NUM];
} Setting_Done_List_Data;


#define MAX_BUTTON_TEXT_LEN 20
#define MAX_BUTTON_NUM 20
typedef struct _setting_btn_data {
	Evas_Object *parent;
	const char	*btn_text[MAX_BUTTON_NUM];
	const char	*btn_icon[MAX_BUTTON_NUM];
	setting_call_back_func btn_click_cb;
	void *btn_cb_data;
	Evas_Object *ctxpopup;
	Elm_Object_Item *item[MAX_BUTTON_NUM];
} Setting_Btn_Data;

#define USEC_PER_SEC  1000000UL
typedef struct {
	const char *key_font_name;
	int font_size;
} settings_font_size_table;

#endif		/* __SETTING_COMMON_DATA_TYPE_H__ */
