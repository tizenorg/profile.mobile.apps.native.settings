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

#include <Elementary.h>		/* including Elementary lib */
#include <setting-debug.h>	/*  including debuging type */
#include <setting-common-resource.h>	/* including one class of data type */
#include <setting-common-data-error.h>	/* including another class of data type */
#ifndef PREFIX
#define PREFIX "/opt"
#endif

#ifndef bool
#define bool Eina_Bool
#endif
#define PWD_FILE "/opt/usr/data/setting/set_info"
/*#define PLUGIN_CFG_DIR "/opt/usr/data/setting/plugin-cfg" */
#define PLUGIN_CFG_DIR "/opt/usr/data/setting/plugin-xml"

#define SETTING_POSTINST_FILE "/var/lib/dpkg/info/org.tizen.setting.postinst"
#define SETTING_PACKAGE			"setting"
#define SYSTEM_PACKAGE			"sys_string"
#define SETTING_LOCALEDIR		"/usr/apps/org.tizen.setting/res/locale"

#define MODE_SYSPOPUP_PACKAGE                  "mode-syspopup"
#define MODE_SYSPOPUP_LOCALEDIR                "/usr/apps/org.tizen.mode-syspopup/res/locale"

#define SYSTEM_ICON_PATH		"/usr/apps/org.tizen.setting/res/icons"
#define SETTING_ICON_PATH		"/usr/apps/org.tizen.setting/res/icons"
#define SETTING_UG_ICON_PATH	"/usr/ug/res/icon/org.tizen.setting"

#define SETTING_UG_PATH			"/usr/ug/lib"
#define SETTING_UG_PATH_USR		"/usr/ug/lib"

/*#define SETTING_UG_PATH_OPT			"/opt/ug/lib" */
/*#define SETTING_UG_PATH_USR_OPT		"/opt/ug/lib" */

#define SETTING_PATH_MEMORY_OPT 		"/opt/usr"
#define SETTING_PATH_SDCARD_OPT			"/opt/storage/sdcard"
#define SETTING_PATH_PARENT_SDCARD_OPT 	"/opt/storage/sdcard/.."


#define SETTING_KILOBYTE_VALUE		1024
#define SETTING_MEGABYTE_VALUE		1048576
#define SETTING_GIGABYTE_VALUE		1073741824

#define SETTING_MAX_STR_LEN			30


#define SETTING_EDJ_PATH		EDJDIR
#define SETTING_MEMORY_EDJ_NAME		SETTING_EDJ_PATH"/setting-memory.edj"
#define SETTING_THEME_EDJ_NAME		SETTING_EDJ_PATH"/setting-theme.edj"
#define SETTING_5STEP_SLIDER_EDJ_NAME		SETTING_EDJ_PATH"/font-slider.edj"
#define SETTING_NEWUX_EDJ_NAME		SETTING_EDJ_PATH"/setting-newux.edj"
#define SETTING_SOUND_EDJ_NAME		SETTING_EDJ_PATH"/setting-sound.edj"
#define SETTING_GENLIST_EDJ_NAME	SETTING_EDJ_PATH"/setting-genlist.edj"
#define SETTING_NEW_GENLIST_EDJ_NAME	SETTING_EDJ_PATH"/setting-new-genlist.edj"
#define SETTING_PROC_EDJ_NAME			SETTING_EDJ_PATH"/setting-proc.edj"
#define SETTING_SPINNER_EDJ_NAME	SETTING_EDJ_PATH"/setting-spinner.edj"
#define SETTING_SLIDER_EDJ_NAME	SETTING_EDJ_PATH"/setting-slider.edj"
#define SETTING_EFF_EDJ_NAME			SETTING_EDJ_PATH"/setting-eff.edj"
#define SETTING_POWER_EDJ_NAME			SETTING_EDJ_PATH"/setting-power.edj"
#define SETTING_BATT_EDJ_NAME			SETTING_EDJ_PATH"/setting-batt.edj"

#define SETTING_THEME_COLOR_THEME_EDJ_NAME		SETTING_EDJ_PATH"/setting-color-theme.edj"


#define RCS_CHECHING_FILE "/opt/share/ims/.RCS_ENABLED"

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

#define NAVI_DEF_BUTTON_STYLE  "naviframe/toolbar/default"/*"naviframe_control/default" */
#define NAVI_BACK_BUTTON_STYLE "naviframe/back_btn/default"
#define NAVI_MORE_BUTTON_STYLE "naviframe/more/default"/*"naviframe_control/default" */


#define NAVI_CONTROLBAR_PART "controlbar"

#define BTN_STYLE_DEF "naviframe/title/default"
#define NAVI_BTN_STYLE_LEFT "title_left_btn"
#define NAVI_BTN_STYLE_RIGHT "title_right_btn"
#define NAVI_BTN_STYLE_MORE "title_more_btn"
#define NAVI_BTN_STYLE_PREV "prev_btn"

/*#define PROGRESSBAR_STYLE "Process_Large" */
#define PROGRESSBAR_STYLE "process_large"
#define SETTING_FONT_CONF_FILE "/opt/etc/fonts/conf.avail/99-slp.conf"
#define SETTING_DEFAULT_FONT_CONF_FILE "/usr/opt/etc/fonts/conf.avail/99-slp.conf"



/*#define GENLIST_LIST_SUB_TEXT_SETTINGS_COLOR_STYLE          "#3B73B6FF" */
#define BLUE_COLOR          "#3B73B6"  /*3B73B6FF <== 59 115 182 255 */
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
typedef void (*setting_call_back_func)(void *data, Evas_Object *obj, void *event_info);

#define WIDGET_SCALE_FACTOR				(get_widgets_factor())
#define SETTING_PADDING_WIDTH			(8 * WIDGET_SCALE_FACTOR)
#define SETTING_PADDING_HEIGHT			(8 * WIDGET_SCALE_FACTOR)
#define SETTING_HALF_SCREEN_WIDTH \
	(SETTING_PADDING_WIDTH * 22 * WIDGET_SCALE_FACTOR)

#define SYS_NOTI_NAME_LEN					128
#define MAX_DISPLAY_NAME_LEN_ON_UI			128
#define MAX_COMMON_BUFFER_LEN				1024
#define BUF_SIZE					512

#define MAX_SEARCH_STR_LEN 40

#define HELP_MSG_BUFF_SIZE   				1000
#define MAX_SPECIALIZITION_LEN				1000
#define SPECIALIZITION_FONT					25
#define HELP_TITLE_SIZE						30
#define HELP_CONTENT_SIZE					28

#define PHONE_LOCK_ATTEMPS_MAX 				5
#define SIM_LOCK_ATTEMPS_MAX 				5
#define PHONE_LOCK_BLOCK_SECONDS 			30
#define SIM_LOCK_BLOCK_SECONDS 				30
#define LOCK_TIMESTAMP_LEN 					64
#define MAX_DATETIME_FORMAT_LEN 			64
#define MAX_DATE_FORMAT_LEN 				10

#define SMALL_FONT_DPI 						(-80)
#define MIDDLE_FONT_DPI 					(-100)
#define LARGE_FONT_DPI						(-144)
#define HUGE_FONT_DPI						(-184)
#define GIANT_FONT_DPI						(-240)

#define LABEL_FONT_SIZE_SMALL					24
#define LABEL_FONT_SIZE_NORMAL 					35
#define LABEL_FONT_SIZE_LARGE					55
#define LABEL_FONT_SIZE_HUGE					75
#define LABEL_FONT_SIZE_GIANT					92

#define __STR_EMUL							"emul"
#define __LEN_STR_EMUL						4

#define MAX_SYSTEM_SERVICE_NUM				30
#define SYSTEM_SERVICE_LIST_PATH "/opt/osp/system/configuration/system-services.list"
#define MAX_PKG_NAME_LEN			128

#define MIN_MOVE_DISTANCE 30

enum {
    SETTING_ON_OFF_BTN_OFF = 0,
    SETTING_ON_OFF_BTN_ON,
    SETTING_ON_OFF_BTN_MAX
};


typedef struct {
	char *locale;				/* ex) "en_US.UTF" */
	char *title;				/* "United Kingdom" */
	char *mcc;
	/*int number;				// no -  UI */
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
    GENDIAL_Type_1text_2icon = 1,
    GENDIAL_Type_1icon_2text = 2,
    GENDIAL_Type_2text_2icon_4 = 3,
    GENDIAL_Type_1text_2icon_2 = 4,
    GENDIAL_Type_2text_2icon_3 = 5,
    GENDIAL_Type_sel_all,

    GENDIAL_Type_1text_2icon_divider,
    GENDIAL_Type_group_item,
    GENDIAL_Type_mygroup_item,
    GENDIAL_Type_2text_3,
    GENDIAL_Type_1gengrid,
    GENDIAL_Type_1gengrid_proc,

    GENDIAL_Type_expandable_proc,
    GENDIAL_Type_MAX
} GENDIAL_Type;

typedef enum _SWALLOW_Type {
    SWALLOW_Type_INVALID = -1,
    SWALLOW_Type_2ICON = 0,
    SWALLOW_Type_1ICON_1CHECK = 1,
    SWALLOW_Type_1ICON_1RADIO = 2,
    SWALLOW_Type_1CHECK = 3,
    SWALLOW_Type_1CHECK_RIGHT = 4,
    SWALLOW_Type_1CHECKICON = 5,
    SWALLOW_Type_1RADIO = 6,
    SWALLOW_Type_1TOGGLE =  7,
    SWALLOW_Type_1ICON_1PROCESS,
    SWALLOW_Type_MULTILINE_1TOGGLE,
    SWALLOW_Type_1ICON_1DOTTOOGLE,

    SWALLOW_Type_1ICON_1IMAGE,
    SWALLOW_Type_1GIF,
    SWALLOW_Type_1IMAGE,
    SWALLOW_Type_1TEXT_1IMAGE,
    SWALLOW_Type_1BUTTON,
    SWALLOW_Type_1PROCESS,
    SWALLOW_Type_1ICON_WITHOUT_CIRCLE_BG,

    SWALLOW_Type_1RADIO_SUB,
    SWALLOW_Type_1RADIO_RIGHT,
    SWALLOW_Type_1RADIO_1PROCESS,
    SWALLOW_Type_1RADIO_1SEARCH,
    SWALLOW_Type_1RADIO_1LABLE,
    SWALLOW_Type_1RADIO_1BTN,
    SWALLOW_Type_1RADIO_1TEXT,
    SWALLOW_Type_1SEARCH,

    SWALLOW_Type_1LINE_LAYOUT,
    SWALLOW_Type_1LINE_LAYOUT_1RADIO,
    SWALLOW_Type_1LINE_LAYOUT_1TOGGLE,
    SWALLOW_Type_1LINE_LAYOUT_1CHECK,
    SWALLOW_Type_LOAD_LAYOUT, /*load an app or UG */
    SWALLOW_Type_LAYOUT_ENTRY,
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
    LOAD_LIB_FAILED = -1,/*  load failed : -1 */
    OPERATE_LIB_SUCESS = 0,
} OPERATE_LIB_ERROR;

typedef enum _POPUP_BTN_RESPONSE_TYPE {
    POPUP_RESPONSE_INVALID = -1,
    POPUP_RESPONSE_OK = 0,
    POPUP_RESPONSE_CANCEL = 1,
} POPUP_BTN_RESPONSE_TYPE;

typedef enum {
    SETTING_GROUP_STYLE_NONE = 0,
    SETTING_GROUP_STYLE_TOP,
    SETTING_GROUP_STYLE_CENTER,
    SETTING_GROUP_STYLE_BOTTOM
} setting_group_style;


/**
 * an item node in genlist
 * @todo begings_to is requried?
 * @todo isItemDisableFlag is required?
 * @todo isSinglelineFlag is requred?
 */
typedef struct _Setting_GenGroupItem_Data {

	void **pBack;

	Evas_Object *win_main;

	Elm_Object_Item *item;

	SWALLOW_Type swallow_type;

	const char *l_swallow_path;
	const char *r_swallow_path;

	/* for shortcut item */
	const char *exec_path;

	Evas *evas;

	/* radio button */
	Evas_Object *rgd;						/**< radio button */

	/* output */
	Evas_Object *eo_check;					/**< slider, button, check, editfield, datefield */
	setting_call_back_func chk_change_cb;

	int chk_status;			 				/**< represent check value, slider value, datefield value etc.. */
	char *keyStr;							/**< title */
	char *keyStr2;                          /**< title for font name expanlist item*/
	char *sub_desc;							/**< sub text */
	char *guide_text;						/**< guide text for editfield */

	/* to swallow a whole layout (genlist item is 1line-layout or 2line-layout) */
	char *item_style;						/**< itc style name */

	/*  for slider control */
	bool isIndicatorVisible;				/**< indicator option (displaying number) */
	double slider_min;						/**< to swallow a slider */
	double slider_max;						/**< to swallow a slider */
	setting_call_back_func start_change_cb;
	setting_call_back_func stop_change_cb;
	setting_call_back_func maxlength_reached_cb;
	setting_call_back_func focus_cb;           /**< for entry */
	setting_call_back_func activated_cb;		/**< for entry */

	setting_call_back_func mouse_up_cb;

	void *x_callback_cb;

	/* sound/vibration */
	int belongs_to; 						/**< 1: slider belongs to sound group. 0: slider belongs to vibration group */

	/*  input panel  */
	Elm_Input_Panel_Layout input_type;		/**< input panel type */
	bool input_panel_disable_flag;      	/** do not show the input panel automatically when the widget is clicked or has focus*/
	Elm_Input_Panel_Return_Key_Type return_key_type;	/** the return key type in input method panel*/
	int disable_auto_cap; /** do not autocapital the first char in edit field*/

	void *userdata;							/**< just a reference, do not need release. */

	bool isPasswordFlag;
	bool isFocusFlag;
	bool isItemDisableFlag;					/**< date/time UG only */
	bool isSinglelineFlag;
	bool maxLengthReachFlag;
	bool enterKeyPressFlag;	                /**< for about UG, enter or space key are not allowed, if they are pressed, it is true */
	bool spaceKeyPressFlag;                 /**<for about UG,if space key is pressed, need to popup a notification>*/

	int int_slp_setting_binded; 			/**< its relative vconf value must increase from 0, and step is 1. eg, 0, 1, 2, 3.. */
	bool entry_auto_focus;                  /*for enty, if this is true, then entry will be focus state when the entry is created*/

	/* popup */
	Evas_Object *notify; 					/**< act as any popup to show user any message */

	Elm_Entry_Filter_Accept_Set *digits_filter_data;
	Elm_Entry_Filter_Limit_Size *limit_filter_data;

	Eina_List *childs;		/**< child nodes if this has child nodes for handling radio buttons */

	setting_group_style group_style;
	setting_group_style exp_style;
	setting_group_style con_style;
	bool isGroupStyleTOP; /*fix aboutUG device name display style issue. */
	bool focus_unallowed;
	/*for data usage*/
	char *app_id;

	/* Edit control : min length filter */
	int minlength;		/* min length -- characters */

	/*for incoming call*/
	Eina_Bool isPlaying;

	/* for disable item*/
	Eina_Bool disableflag;
	Eina_Bool toggleDisableFlag;

	bool isDisableClick;

	/*record itc style for all genlist item with check/radio/toggle */
	char *itc;

	/* storage item style */
	int color; /* 2: app, 3: pic,video, 4: audio, 5: Misc files, 6: Avail space */

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
	Elm_Genlist_Item_Class itc_seperator;
	Elm_Genlist_Item_Class itc_sel_all;
	Evas_Object *select_all_checkbox;
	Eina_Bool select_all_checked;
	Eina_List *list;

	AllowListItem allow_items[MAX_RESET_MENU_NUM];
} Setting_Done_List_Data;


#define MAX_BUTTON_TEXT_LEN 20
#define MAX_BUTTON_NUM 20
typedef struct _setting_btn_data {
	Evas_Object *parent;
	const char  *btn_text[MAX_BUTTON_NUM];
	const char  *btn_icon[MAX_BUTTON_NUM];
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

#endif				/* __SETTING_COMMON_DATA_TYPE_H__ */
