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
#ifndef __SETTING_FONT_H__
#define __SETTING_FONT_H__

#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <system_settings.h>

typedef struct _SettingFontUG SettingFontUG;

typedef enum {
	FONT_SIZE_AND_FONT_SEL_UG,
	FONT_SEL_VIEW_APPCONTROL,
	FONT_SIZE_VIEW_APPCONTROL,
} FontUGViewMode;

typedef enum {
	SELECTED_FONT_CHANGE_NONE,
	SELECTED_FONT_CHANGE_IN_PROCESS,
	SELECTED_FONT_CHANGE_DONE
} FontChangStatus;

typedef enum {
	FONT_EXPAND_STATE_NONE,
	FONT_EXPAND_STATE_FONT_TYPE,
	FONT_EXPAND_STATE_FONT_SIZE,
} FontExpandState;


/**
 * Setting Font UG context
 * all UG function has void* as an agument. this is casted back to SettingFontUG
 * and the functions access app context.
 */
struct _SettingFontUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *ly_main;

	Evas_Object *genlist;
	Evas_Object *navibar;
	Elm_Object_Item *navi_it_font;
	Elm_Object_Item *navi_it_font_size;

	Evas_Object *btn_done;
	Evas_Object *btn_cancel;

	Elm_Theme *theme;

	/* font setting */
	Evas_Object *main_popup;
	Evas_Object *size_popup;

	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_1text_1icon_2_font_size;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_bg_1icon;
	/*Elm_Genlist_Item_Class itc_seperator;*/
	Elm_Genlist_Item_Class itc_preview;

	Setting_GenGroupItem_Data *subitem;
	Setting_GenGroupItem_Data *font_type;
	Setting_GenGroupItem_Data *font_size;
	Setting_GenGroupItem_Data *font_example;

	/* font type */
	int prev_font;
	char *font_name;

	char *font_size_str;
	char *font_type_str;
	int init_font_size;
	int init_font_type;
	bool size_change_flag;
	bool type_change_flag;

	int ret_font_size;

	FontUGViewMode viewmode;

	/* Font Size & Sel */
	Evas_Object *size_rdg;
	setting_view *view_to_load;

	Elm_Object_Item *cur_focus_item;
	Eina_List *font_type_list;

	Ecore_Timer *timer;
	Ecore_Timer *font_size_idler;
	Ecore_Timer *font_type_timer;

	FontChangStatus font_change_status;
	FontExpandState font_expand_state;

	int rotate_angle;
};

extern setting_view setting_view_font_main;
extern setting_view setting_view_font_font_size;


#endif				/* __SETTING_FONT_H__ */
