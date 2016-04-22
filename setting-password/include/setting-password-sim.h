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
#ifndef _SETTING_PASSWORD_SIM_H_
#define _SETTING_PASSWORD_SIM_H_

#include <setting-password.h>

int setting_password_sim_create_ui(void *data);

static Eina_Bool __setting_password_sim_rotate_cb(void *data);

int setting_password_sim_draw_1line_entry(void *data, void *cb);
int setting_password_sim_draw_2line_entry(void *data, void *cb);
int setting_password_sim_draw_3line_entry(void *data, void *cb);
int setting_password_sim_clear_1line_entry(void *data);
int setting_password_sim_clear_2line_entry(void *data);
int setting_password_sim_clear_3line_entry(void *data);
int setting_password_sim_check_1line_entry(void *data);
int setting_password_sim_check_2line_entry(void *data);
int setting_password_sim_check_3line_entry(void *data);

void setting_password_sim_click_softkey_done_cb(void *data,
												Evas_Object *obj,
												void *event_info);
void setting_password_sim_warning_entry_added_byte_popup(SettingPasswordUG *ad,
														 int min, int max);


void setting_get_pin_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_get_sim_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data);

void setting_sim_change_pins_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_sim_verify_puks_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_sim_facility_enable_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_sim_facility_disable_cb(TapiHandle *handle, int result, void *data, void *user_data);

static void setting_password_ug_display_result_popup(tapi_receive_info *result, SettingPasswordUG *ad);

#endif
