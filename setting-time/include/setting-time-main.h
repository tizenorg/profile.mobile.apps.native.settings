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
#ifndef __SETTING_TIME_MAIN_H__
#define __SETTING_TIME_MAIN_H__

#include <setting-time.h>
#include <setting-time-engine.h>

#define MAX_ARGS			15
#define CITY_BUF_SIZE		128
#define GMT_BUF_SIZE		10
#define __MAX_PATH_SIZE		1024

#define SETTING_TIME_ZONEINFO_PATH		"/usr/share/zoneinfo/"
#define SETTING_TIME_SHARE_LOCAL_PATH	"/usr/share/locale"
#define SETTING_TZONE_SYMLINK_PATH		_TZ_SYS_ETC"/localtime"

void setting_time_main_launch_worldclock_sg(void *data);
void setting_time_main_close_worldclock_sg(gpointer cb_data, gint argc,
		gchar **argv);

void setting_time_main_click_softkey_left_cb(void *data, Evas_Object *obj,
		void *event_info);
void setting_time_main_click_softkey_caller_exist_left_cb(void *data,
		Evas_Object *obj, void *event_info);
void setting_time_main_click_softkey_caller_exist_right_cb(void *data,
		Evas_Object *obj, void *event_info);
static void setting_time_main_chk_btn_cb(void *data, Evas_Object *obj,
		void *event_info);
void setting_time_main_item_mouse_up_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);
static void setting_time_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info);
void setting_time_main_btn_update_ok_cb(void *cb);
void setting_time_main_btn_update_cancel_cb(void *cb);
void setting_time_update_time_date_format_string(SettingTimeUG *ad);
Eina_Bool setting_update_datefield_cb(void *cb);
int setting_time_check_automatic_time_update_state();
static int __setting_set_city_tzone(const char *pTZPath);

#endif				/* __SETTING_TIME_MAIN_H__ */
