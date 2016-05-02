/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SETTING_COMMON_INIT_H__
#define __SETTING_COMMON_INIT_H__

#include "setting-common-data-type.h"

#include <Elementary.h>

enum {
	SC_All_List,
	SC_Max
};

typedef struct _MainData {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *ly_main;
	Evas_Object *conform;
	Evas_Object *navibar_main;	/* for navigation effect */
	Elm_Object_Item *navibar_main_it;
	Evas_Object *view_layout;
	Evas_Object *genlist;
} MainData;

int app_init(MainData *ad, char *name);
int view_init(MainData *ad, char *name);

#endif				/*__SETTING_COMMON_INIT_H__ */
