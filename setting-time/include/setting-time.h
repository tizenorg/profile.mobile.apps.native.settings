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
#ifndef __SETTING_TIME_H__
#define __SETTING_TIME_H__

#include <stdio.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <fcntl.h>
#include <unistd.h>
#include <Elementary.h>
#include <alarm.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#define ERR_BIGGER_THAN_2037 "Cannot Set the year bigger than 2037"
#define AUTO_TIME_UPDATE_ON	 "Automatic time update is ON!"
#define SETTING_TIME_AUTO_UPDATE_DESC  "IDS_ST_BODY_USE_TIME_PROVIDED_BY_NETWORK"
#define NOT_SUPPORT_AUTO_UPDATE_DESC "IDS_ST_POP_AUTO_TIME_UPDATE_UNAVAILABLE_CHOOSE_ALTERNATIVE_TIME_SET_MODE_JPN_SBM"


#define DATE_TIME_STR_AUTOMATIC_UPDATE "IDS_ST_MBODY_AUTO_UPDATE"
#define DATE_TIME_STR_TIME_ZONE		"IDS_ST_BODY_TIME_ZONE"
#define DATE_TIME_STR_DATE_AND_TIME "IDS_ST_BODY_DATE_AND_TIME"
#define DATE_TIME_STR_TIME_FORMAT	"IDS_ST_BODY_MSG_TIME_FORMAT"


typedef struct _SettingTimeUG SettingTimeUG;

/**
 * Setting Time UG context
 * all UG function has void* as an agument. this is casted back to SettingTimeUG
 * and the functions access app context.
 */
struct _SettingTimeUG {

	ui_gadget_h ug;	/**< ug struct itself */

	/* add more variables here (move your appdata to here) */
	int noti_fd;

	app_control_h bundle_data; /**< when loading this UG, bundle is stored */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *ly_main;

	ui_gadget_h ug_loading;

	Evas_Object *navi_bar;
	Evas_Object *scrl_main;
	char *caller;

#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
	Ecore_Timer *update_timer;
#endif
	Evas_Object *pop_progress;
	Evas_Object *pop;
	Eina_Bool is_datefield_selected;

	/*	UI	 */
	Setting_GenGroupItem_Data *data_auto;
	Setting_GenGroupItem_Data *data_tz;
	Setting_GenGroupItem_Data *data_time;
	Setting_GenGroupItem_Data *data_time_fmt;
	Setting_GenGroupItem_Data *data_date_fmt;

	/*Evas_Object *chk_dst; */
	/*Evas_Object *chk_auto; */

	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_2text_arrow;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_layout;
	Elm_Genlist_Item_Class itc_layout_time;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;

	Ecore_Idler *update_timezone_idler;
	Ecore_Idler *refresh_time_idler;

	int is_expand_time_format_list;
};

extern setting_view setting_view_time_main;

#endif				/* __SETTING_TIME_H__ */
