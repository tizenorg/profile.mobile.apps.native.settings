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
#ifndef __SETTING_APPLICATION_H__
#define __SETTING_APPLICATION_H__

#include <glib.h>
#include <Elementary.h>

#include "setting-common-draw-widget.h"
#include "setting-common-view.h"
#include "setting-common-init.h"

#define SAFE_STRDUP(src) (src) ? strdup(src) : NULL

#define Keystr_Clear		"Clear"

typedef struct _SettingApplications SettingApplications;

/**
 * Setting Applications context
 */
struct _SettingApplications {
	MainData md;
	setting_view *view_to_load;

	Ecore_Timer *event_freeze_timer;
	Setting_GenGroupItem_Data *data_home;

	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_2text_3;
	Elm_Genlist_Item_Class itc_1text_1icon_divider;
	Elm_Genlist_Item_Class itc_grp_title;
	Elm_Genlist_Item_Class itc_1icon_1button;

	GList *pkg_list;
};

extern setting_view setting_view_applications_main;
extern setting_view setting_view_applications_defaultapp;

typedef struct _default_app {
	Elm_Object_Item *item;
	int defapp;
	char *pkgid;
	char *icon_path;
	char *pkg_label;
} default_app;

#endif				/* __SETTING_APPLICATION_H__ */
