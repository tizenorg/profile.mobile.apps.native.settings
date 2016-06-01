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
#ifndef __SETTING_PHONE_LICENSE_MAIN_H__
#define __SETTING_PHONE_LICENSE_MAIN_H__

#include <setting-phone.h>

#define SETTING_DIVX_MODE_REG 0
#define SETTING_DIVX_MODE_DEREG 1
#define SETTING_LICESE_SAMSUNG_LEGAL "IDS_ST_BODY_SAMSUNG_LEGAL"

void setting_phone_license_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info);
void setting_phone_license_main_click_softkey_cancel(void *data,
		Evas_Object *obj, void *event_info);
void setting_phone_license_main_popup_resp_cb(void *data, Evas_Object *obj,
		void *event_info);

#endif				/* __SETTING_PHONE_LICENSE_MAIN_H__ */
