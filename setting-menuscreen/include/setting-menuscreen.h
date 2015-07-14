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
#ifndef __SETTING_MENUSCREEN_H__
#define __SETTING_MENUSCREEN_H__

#include <stdio.h>
#include <string.h>

#include <Elementary.h>

#include <glib-object.h>

#include <Eina.h>
#include <ail.h>
#include <aul.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#define SETTING_PW_STR "16777216"
#define SETTING_THEME_LIVE_MAGAZINE 	"com.samsung.live-magazine"
#define SETTING_THEME_CLUSTER_HOME 		"com.samsung.cluster-home"

#define SETTING_MENUSCREEN_FOLDER "/opt/share/install-info/"
#define SETTING_MENUSCREEN_FILE_EXTENSION "desktop"
#define SETTING_MENUSCREEN_INDEX_NAME "name"

#define SETTING_MENUSCREEN_SELECTED_PACKAGE_VCONF_KEY VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME

#define SETTING_MENUSCREEN_TABLE_NAME  "menu"
#define SETTING_MENUSCREEN_CLAUSE  "categories = '?'"
#define SETTING_MENUSCREEN_VALUE_CATEGORY "home-screen"
#define SETTING_MENUSCREEN_COLUMN_NAME  "name"
#define SETTING_MENUSCREEN_COLUMN_PKGNAME  "package"
#define SETTING_MENUSCREEN_COLUMN_CATEGORY "category"


typedef struct tag_ug_menuscreen_info {
    char* pkgname;
    char* appname;
}ug_menuscreen_info ;

typedef struct _SettingMenuscreenUG SettingMenuscreenUG;

/**
 * Setting Menuscreen UG context
 * all UG function has void* as an agument. this is casted back to SettingMenuscreenUG
 * and the functions access app context.
 */
struct _SettingMenuscreenUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *scroller;

	ui_gadget_h ug_loading;

	Evas_Object *ly_main;
	Evas_Object *ly_wall;

	Evas_Object *navi_bar;	/*  for navigation effect */
	Elm_Object_Item *navi_it;

	/* entries in Genlist */
	setting_view *view_to_load;

	// page 1 : main list
	Evas_Object* chk_radio;
	int pkglist_cnt;
	int chk_menuscreen;			// selcted item
	char* selected_theme;
	Eina_List * menuscreen_list;

	// page 2 : password
	Setting_GenGroupItem_Data* data_pwd;

	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_layout;
	Elm_Genlist_Item_Class itc_seperator;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1text;
};

extern setting_view setting_view_menuscreen_main;
extern setting_view setting_view_menuscreen_password;

void setting_menuscreen_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv);

#endif				/* __SETTING_MENUSCREEN_H__ */
