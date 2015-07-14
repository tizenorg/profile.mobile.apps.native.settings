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

#ifndef __SETTING_POWERSAVING_MAIN_H__
#define __SETTING_POWERSAVING_MAIN_H__

#include <setting-powersaving.h>

#define NEVER_SHOW_AGAIN "IDS_COM_BODY_NEVER_SHOW_AGAIN"
#define POWERSAVING_REMINDER "IDS_ST_BODY_BATTERY_LIFE_EXTENDED_BUT_PERFORMANCE_RESTRICTED_BRIGHTNESS_LOW_MSG"

static void setting_powersaving_main_mouse_up_Gendial_list_cb(void *data,
							      Evas_Object *obj,
							      void *event_info);
static void setting_powersaving_main_click_softkey_back_cb(void *data,
							   Evas_Object *obj,
							   void *event_info);
static void setting_powersaving_main_motion_active_chk_btn_cb(void *data,
							      Evas_Object *obj,
							      void *event_info);
#endif				/* __SETTING_POWERSAVING_MAIN_H__ */
