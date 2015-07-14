/*
 * accessibility
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Contact: Tomasz Olszak <t.olszak@samsung.com>
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
#ifndef __SETTING_ACCESSIBILITY_H__
#define __SETTING_ACCESSIBILITY_H__

#include <Elementary.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#define DEVOPTION_STR_ACCESSIBILITY_MAGNIFIER   "IDS_ST_BODY_MAGNIFIER"
#define DEVOPTION_STR_ACCESSIBILITY_SCREENREADER "IDS_ST_MBODY_SCREEN_READER_HTTS"

typedef struct _SettingAccessibilityUG SettingAccessibilityUG;

struct _SettingAccessibilityUG {
	ui_gadget_h ug;

	Elm_Genlist_Item_Class itc_1text;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *navi_bar;
	Evas_Object *ly_main;
	Elm_Object_Item *navi_item;

	Setting_GenGroupItem_Data *magnifier_checkbox;
	Setting_GenGroupItem_Data *screenreader_checkbox;

	Evas_Object *genlist;

	bool empty_flag;
};

extern setting_view setting_view_accessibility_main;

#endif /*__SETTING_ACCESSIBILITY_H__ */
