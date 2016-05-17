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
#ifndef _SETTING_PASSWORD_MAIN_H_
#define _SETTING_PASSWORD_MAIN_H_

#include <setting-password.h>

Eina_Bool __rotate_cb(void *data);

int setting_password_main_draw_1line_password(void *data, void *cb);
int setting_password_main_draw_2line_password(void *data, void *cb);
int setting_password_main_clear_1line_password(void *data);
int setting_password_main_clear_2line_password(void *data);
int setting_password_main_check_1line_password(void *data);
int setting_password_main_check_2line_password(void *data);

void setting_password_main_click_softkey_done_cb(void *data, Evas_Object *obj,
		void *event_info);
void setting_password_main_click_softkey_continue_cb(void *data,
		Evas_Object *obj, void *event_info);

void setting_password_main_warning_entry_added_byte_popup(SettingPasswordUG *ad,
		int min, int max);

#endif
