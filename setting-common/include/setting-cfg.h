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
 *@defgroup setting-cfg
 *configuration file management.
 *Setting app is based on JSON scripting for configuration management.
 */

#ifndef __SETTING_CFG_H__
#define __SETTING_CFG_H__

#include <glib.h>
#include <json-glib/json-glib.h>
#include <setting-common-resource.h>
#include <app.h>
#include <app_common.h>

#define EXPORT_PUBLIC __attribute__ ((visibility ("default")))

/*****/
typedef enum _Cfg_Item_Position_Type {
    Cfg_Item_Pos_Uninstall = -1,
    Cfg_Item_Pos_Level0 = 0,
    Cfg_Item_Pos_Level1 = 1,
    Cfg_Item_Pos_Level_All = 2,
} Cfg_Item_Position;

typedef enum _Cfg_Item_State {
    Cfg_Item_Error = -1,
    Cfg_Item_Off = 0,		/* same order - vconf 0 (boolean) */
    Cfg_Item_On = 1			/* same order - vconf 1 (boolean) */
} Cfg_Item_State;



typedef enum _Cfg_Item_Reset_Type {
    Cfg_Item_unResetable = 0,
    Cfg_Item_Resetable = 1,
} Cfg_Item_Reset_Type;

typedef enum _cfg_error_type {
    Cfg_Error_Type_Sucess = 0,
    Cfg_Error_Type_CreateCfg_Failed = -1,
    Cfg_Error_Type_Mkdir_Failed = -2,
    Cfg_Error_Type_RemoveCfg_Failed = -3,
    Cfg_Error_Type_ReadCfg_Failed = -4,
    Cfg_Error_Type_DirPermissionDenied = -5,
    Cfg_Error_Type_FilePermissionDenied = -6,
    Cfg_Error_Type_OutOfMemory = -7,
} Cfg_Error_Type;


#define SEARCH_TYPE_ERROR "0"
#define SEARCH_TYPE_UG_NODE "1"
#define SEARCH_TYPE_UI_NODE "2"
#define SEARCH_TYPE_TITLE_NODE "3"
#define SEARCH_TYPE_APP_NODE "4"
#define SEARCH_TYPE_VIEW_NODE "5"
#define SEARCH_TYPE_APPLAUNCHER_NODE "6"
#define SEARCH_TYPE_APPLAUNCHER_VIEW_NODE "7"

#define SEARCH_TYPE_UG_NODE_TOGGLE "1"SEARCH_TYPE_UG_NODE
#define SEARCH_TYPE_UI_NODE_TOGGLE "1"SEARCH_TYPE_UI_NODE
#define SEARCH_TYPE_VIEW_NODE_TOGGLE "1"SEARCH_TYPE_VIEW_NODE
#define SEARCH_TYPE_APPLAUNCHER_NODE_TOGGLE "1"SEARCH_TYPE_APPLAUNCHER_NODE


/**
 * 1, 4, 5, 6 - ug mode
 * 2, 3,
 */
typedef enum _Cfg_Item_Type {
    Cfg_Item_Node_Error = 0,		/** Error                                     - no action */
    Cfg_Item_Ug_Node = 1,			/** general UG                                - don't create Grid */
    Cfg_Item_Ui_Node = 2,			/** no UG, no app launching, just menu name   - create Grid */
    Cfg_Item_Title_Node = 3,		/** view name                                 - create Grid */
    Cfg_Item_App_Node = 4,			/** app type - by launcher - read doc '4' */
    Cfg_Item_View_Node = 5,			/** view name - 2depth search */
    Cfg_Item_AppLauncher_Node = 6,	/** app launcher (NOT Ug)                     - don't create Grid */
    Cfg_Item_AppLauncher_View_Node = 7,	/** view name - 2depth search by app-launcher */

    /*------------------------------------------------------------------------------- */
    /* (Cfg_Item_Ug_Node_Toggle/10 > 0 ) --> toggle style */
    Cfg_Item_Ug_Node_Toggle = Cfg_Item_Ug_Node + 10,			/** general UG                                - don't create Grid */
    Cfg_Item_Ui_Node_Toggle = Cfg_Item_Ui_Node + 10,			/** no UG, no app launching, just menu name   - create Grid */
    Cfg_Item_View_Node_Toggle =  Cfg_Item_View_Node + 10,		/** view name - 2depth search with toggle button */
    Cfg_Item_AppLauncher_Node_Toggle = Cfg_Item_AppLauncher_Node + 10,	/** app launcher (NOT Ug)                     - don't create Grid */

} Cfg_Item_Type;

typedef struct {
	int (*get_item_state)(Cfg_Item_State *stat, void *data);
	int (*set_item_state)(Cfg_Item_State stat, void *item, void *data);
	int (*set_item_update_ui)(Cfg_Item_State stat, void *data);
} cfg_func_table;


/**
 *  List item
 */
typedef struct {
	char *key_name;					/** key name */
	char *icon_path;				/** icon path */
	char *ug_args;					/** UG path or hyperlink */
	int pos;						/** position : 1st, 2st -- deprecated */
	Cfg_Item_Reset_Type reset_type;	/** if ug supports Reset function */
	int click_times;				/** UG menu need to check */
	Cfg_Item_Type item_type;		/** 0:item 	1:header title */

	/* app code */
	void *data;						/** Setting_GenGroupItem_Data type ptr */
	char *sub_index;				/** sub_index */
	cfg_func_table *tfunc;
	char *uuid;						/** primary key */
	int last_clicked;				/** the time of last clicked*/
	char *shortcut_appid;			/** appid to add shortcut */
} Setting_Cfg_Node_T;

Setting_Cfg_Node_T *get_cfg_node_first();
int get_cfg_array_size();



int setting_cfg_create(bool check_ug_exist);
int setting_cfg_migrate(void);
void setting_cfg_print(void);

char *setting_cfg_get_path();

int setting_cfg_init(void);

void setting_cfg_exit(void);

int setting_cfg_file_update(void);

int setting_cfg_get_category_length(void);

char *setting_cfg_get_category_name(int category_index);

char *setting_cfg_get_category_display_name(int category_index);

void setting_cfg_set_category_display_name(int category_index, char *newstring);

int setting_cfg_get_menu_length(int category_index);

char *setting_cfg_get_string_field_idx(int category_index, int menu_index, char *field_name);

int setting_cfg_get_int_field_idx(int category_index, int menu_index, char *field_name);

char *setting_cfg_get_icon_path_idx(int category_index, int menu_index);

char *setting_cfg_get_shortcut_appid_idx(int category_index, int menu_index);

char *setting_cfg_get_icon_path(char *keyname);

char *setting_cfg_get_ug_args_idx(int category_index, int menu_index);

char *setting_cfg_get_ug_args(char *keyname);

char *setting_cfg_get_keyname_idx(int category_index, int menu_index);

int setting_cfg_get_itemtype_idx(int category_index, int menu_index);

char *setting_cfg_get_ugpath_idx(int category_index, int menu_index);

char *setting_cfg_get_ugpath(char *keyname);

int setting_cfg_get_pos_idx(int category_index, int menu_index);

int setting_cfg_get_pos(char *keyname);

void setting_cfg_set_pos_idx(int category_index, int menu_index, int pos);

void setting_cfg_set_pos(char *keyname, int pos);

int setting_cfg_get_click_times_idx(int category_index, int menu_index);

int setting_cfg_get_click_times(char *keyname);

void setting_cfg_set_click_times_idx(int category_index, int menu_index, int click_times);

void setting_cfg_set_click_times(char *keyname, int click_times);

int setting_cfg_get_resetable_flag_idx(int category_index, int menu_index);

void setting_cfg_add_downloaded_app(char *keyname, char *icon_path, char *ug_args, int pos);

void setting_cfg_remove_downloaded_app(char *keyname);
int setting_cfg_get_last_clicked_idx(int category_index, int menu_index);
int setting_cfg_get_last_clicked(char *keyname);
void setting_cfg_set_last_clicked_idx(int category_index, int menu_index, int last_clicked);
void setting_cfg_set_last_clicked(char *keyname, int last_clicked);


extern char *get_ug_path_from_ug_args(void *data);
extern app_control_h get_bundle_from_ug_args(void *data);
extern bool is_ug_installed_by_ug_args(void *data);

extern int get_index_by_keystr(const char *keystr);

extern Setting_Cfg_Node_T *get_cfg_node_by_keystr(const char *keystr);

extern char *get_iconpath_by_keystr(const char *keystr);

extern void setting_cfg_dump_basic_info();

extern int set_node_pointer(char *name, void *ptr);
extern void *get_node_pointer(char *name);

extern int set_cfg_node_tfunc(char *name, void *ptr);
extern int set_cfg_node_tfunc_set_ptr(char *name, int (*set_item_state)(Cfg_Item_State stat, void *item, void *data));

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* search */
extern Setting_Cfg_Node_T *setting_plugin_search_item_add(char *key_name, char *url, char *icon_path, int item_type, void *data);
extern Setting_Cfg_Node_T *setting_plugin_search_item_subindex_add(char *key_name, char *url, char *icon_path, int item_type, void *data, char *sub_index);
extern Cfg_Item_Type setting_search_type(char *type);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif
