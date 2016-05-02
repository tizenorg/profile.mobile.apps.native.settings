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

#include <Elementary.h>
#include <app_control.h>

typedef struct _MainData {
	Evas *evas;
	Evas_Object *win_main;
	Evas_Object *ly_main;
	Evas_Object *conform;
	Evas_Object *navibar_main;	/* for navigation effect */
	Elm_Object_Item *navibar_main_it;
	Evas_Object *view_layout;
	Evas_Object *genlist;

	app_control_h app_caller_svc;
	app_control_h app_reply_svc;
} MainData;

int app_init(MainData *md, char *name);
int view_init(MainData *md, char *name);
int app_control_init(MainData *md, app_control_h service);
int app_control_finish(MainData *md);
int add_app_reply(MainData *md, char *key, char *val);

#endif				/*__SETTING_COMMON_INIT_H__ */
