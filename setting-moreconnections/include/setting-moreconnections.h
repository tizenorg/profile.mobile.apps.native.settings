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
#ifndef __SETTING_MORECONNECTIONS_H__
#define __SETTING_MORECONNECTIONS_H__

#include <stdio.h>
#include <string.h>
#include <Elementary.h>
#include <glib-object.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-common-data-type.h>
#include <setting-cfg.h>
//#include <locations.h>

#define KeyStr_On				"IDS_ST_BODY_ON"
#define KeyStr_Off				"IDS_ST_BODY_ALERTTYPE_OFF"
#define KeyStr_LocationService	"IDS_LBS_BODY_LOCATION_SERVICE"
#define KeyStr_VPN				"IDS_ST_BODY_VPN"

typedef struct _SettingMoreConnectionsUG SettingMoreConnectionsUG;

/**
 * Setting MoreConnections UG context
 * all UG function has void* as an agument. this is casted back to SettingMoreConnectionsUG
 * and the functions access app context.
 */
struct _SettingMoreConnectionsUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *ly_main;

	/*  for navigation effect */
	Evas_Object *navi_bar;
	Elm_Object_Item *navi_item;

	/* genlist */
	Evas_Object *genlist;
	Setting_GenGroupItem_Data *location_service;
	Setting_GenGroupItem_Data *VPN;

	/* item class */
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text;

	bool empty_flag;

};

extern setting_view setting_view_moreconnections_main;
char *setting_location_is_enable(void *data);
void setting_moreconnections_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv);

#endif				/* __SETTING_MORECONNECTIONS_H__ */
