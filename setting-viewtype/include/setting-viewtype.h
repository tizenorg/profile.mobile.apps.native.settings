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
#ifndef __SETTING_VIEWTYPE_H__
#define __SETTING_VIEWTYPE_H__

#include <string.h>
#include <Elementary.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

typedef struct _SettingViewTypeUG SettingViewTypeUG;

/**
 * Setting ViewType UG context
 * all UG function has void* as an agument. this is casted back to SettingViewTypeUG
 * and the functions access app context.
 */
struct _SettingViewTypeUG {
	ui_gadget_h ug;
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;

	Evas_Object *ly_task;

	Evas_Object *chk_task;

	setting_view *view_to_load;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_seperator;
};

extern setting_view setting_view_task_switcher_viewtype;

#endif				/* __SETTING_VIEWTYPE_H__ */
