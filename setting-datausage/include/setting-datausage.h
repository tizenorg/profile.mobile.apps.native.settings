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
#ifndef __SETTING_DATAUSAGE_H__
#define __SETTING_DATAUSAGE_H__

#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <cairo.h>
#include <data_usage.h>
#include <resman.h>

#define SETTING_PKG_NAME "com.samsung.setting"

#define SETTING_DATAUSAGE_MOBILE_STR "Mobile"

#define SETTING_DATAUSAGE_DATA_ROAMING_STR "IDS_ST_BODY_DATA_ROAMING"
#define SETTING_DATAUSAGE_RESTRICT_BACKGROUND_STR "Restrict background data"
#define SETTING_DATAUSAGE_SHOW_WIFI_STR "Show Wi-Fi usage"

#define SETTING_DATAUSAGE_ON_THIS_DATE_EACH_MONTH_STR "On this date each month"

#define SETTING_DATAUSAGE_MOBILE_DATA_STR "IDS_ST_MBODY_MOBILE_DATA"
#define SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_STR "Set data usage limit"
#define SETTING_DATAUSAGE_DATA_LIMIT_STR "IDS_ST_BODY_DATA_LIMIT"
#define SETTING_DATAUSAGE_DATA_USAGE_CYCLE_STR "Data usage cycle"
#define SETTING_DATAUSAGE_STARTING_DATE_STR "IDS_ST_BODY_START_DATE"
#define SETTING_DATAUSAGE_CHANGE_CYCLE_STR "IDS_ST_BODY_CHANGE_CYCLE"
#define SETTING_DATAUSAGE_MORE_SETTING_STR "More data usage setting"
#define SETTING_DATAUSAGE_GRAPH_DESC_STR "%s : About %s used"
#define SETTING_DATAUSAGE_DESC_STR "IDS_ST_BODY_DATA_USAGE_IS_MEASURED_BY_YOUR_PHONE_AND_YOUR_SERVICE_PROVIDER_MAY_ACCOUNT_FOR_USAGE_DIFFERENTLY"


#define SETTING_DATAUSAGE_NO_APPS_USE_DATA_STR "No apps used data during this period"

#define SETTING_DATAUSAGE_MONTH_STR "Month"
#define SETTING_DATAUSAGE_WEEK_STR "Week"
#define SETTING_DATAUSAGE_DAY_STR "Day"

#define SETTING_DATAUSAGE_DAYS_IN_MONTH 31
#define SETTING_DATAUSAGE_DAYS_IN_WEEK 7
#define SETTING_DATAUSAGE_HOURS_IN_DAY 24

typedef struct _SettingDatausageUG SettingDatausageUG;

/**
 * Setting Datausage UG context
 * all UG function has void* as an agument. this is casted back to SettingDatausageUG
 * and the functions access app context.
 */
struct _SettingDatausageUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *ly_main;

	Evas_Object *genlist;
	Evas_Object *layout;
	Evas_Object *navibar;

	Evas_Object *tabbar;
	Elm_Object_Item *tabbar_item[2];

	setting_view *view_to_load;

	/* datausage setting */
	Evas_Object *popup;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_bg_1icon;
	Elm_Genlist_Item_Class itc_seperator;
	Elm_Genlist_Item_Class itc_variable_height;
	Elm_Genlist_Item_Class itc_1text_1icon_date;
	Elm_Genlist_Item_Class itc_layout;
	Elm_Genlist_Item_Class itc_2text_1icon_4;

	Setting_GenGroupItem_Data *use_packet_data;
	Setting_GenGroupItem_Data *set_data_usage_limit;
	Setting_GenGroupItem_Data *data_limit;
	Setting_GenGroupItem_Data *data_usage_cycle;
	Setting_GenGroupItem_Data *starting_date;
	Setting_GenGroupItem_Data *change_cycle;
	Setting_GenGroupItem_Data *more_setting;
	Setting_GenGroupItem_Data *limit_graph;
	Setting_GenGroupItem_Data *graph_image;
	Setting_GenGroupItem_Data *graph_desc;

	Evas_Object *data_limit_conformant;
	Evas_Object *data_limit_entry;
	Evas_Object *data_limit_popup;
	Evas_Object *ctxpopup;

	Eina_List *app_list;
	int app_list_index;
	unsigned long long apps_total_size;
	Ecore_Animator *animator;
	rsml_iface_type iftype;

	int data_arr_len;
	unsigned long long data_arr[SETTING_DATAUSAGE_DAYS_IN_MONTH];
	int maximum;
	unsigned long long total_size;
	unsigned long long left_size;

	cairo_surface_t *cairo_surface;
	cairo_t 	*cr;

	cairo_surface_t *limit_cairo_surface;
	cairo_t 	*limit_cr;

};

extern setting_view setting_view_datausage_main;

#endif				/* __SETTING_DATAUSAGE_H__ */
