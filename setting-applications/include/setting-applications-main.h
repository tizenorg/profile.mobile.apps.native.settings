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
#ifndef __SETTING_APPLICATIONS_MAIN_H__
#define __SETTING_APPLICATIONS_MAIN_H__

#include <setting-applications.h>

static void setting_applications_main_auto_adjust_chk_btn_cb(void *data, Evas_Object *obj, void *event_info);
static void setting_applications_main_auto_rotate_chk_btn_cb(void *data, Evas_Object *obj, void *event_info);
static void setting_applications_main_tts_chk_btn_cb(void *data, Evas_Object *obj, void *event_info);

static void setting_applications_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info);
static Eina_Bool setting_applications_main_click_softkey_back_cb(void *data, Evas_Object *obj, void *event_info);
#ifdef UNUSED_VCONFKEY
static void setting_applications_main_launch_image_chk_btn_cb(void *data, Evas_Object *obj, void *event_info);
#endif

#endif				/* __SETTING_APPLICATIONS_MAIN_H__ */
