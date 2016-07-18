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

#include "setting-cfg.h"
#include "setting-common-data-type.h"
#include "setting-common-draw-widget.h"
#include "setting-common-init.h"
#include "setting-common-view.h"

#define KeyStr_On		"IDS_ST_BODY_ON"
#define KeyStr_Off		"IDS_ST_BODY_ALERTTYPE_OFF"
#define KeyStr_LocationService	"IDS_LBS_BODY_LOCATION_SERVICE"
#define KeyStr_VPN		"IDS_ST_BODY_VPN"

typedef struct _SettingMoreConnections SettingMoreConnections;

/**
 * Setting MoreConnections context
 */
struct _SettingMoreConnections {
	MainData md;

	Setting_GenGroupItem_Data *location_service;
	Setting_GenGroupItem_Data *VPN;

	/* item class */
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text;

	bool empty_flag;
};

extern setting_view setting_view_moreconnections_main;
char *setting_location_is_enabled(void *data);

#endif				/* __SETTING_MORECONNECTIONS_H__ */
