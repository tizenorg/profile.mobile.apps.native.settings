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
#ifndef __SETTING_NETWORK_CONNECTION_CREATE_H__
#define __SETTING_NETWORK_CONNECTION_CREATE_H__

#include <setting-network.h>
static void setting_network_connection_check_entry_empty(SettingNetworkUG *ad);
static void setting_network_connection_display_auth_type(SettingNetworkUG *
                                                         ad,
                                                         int auth_type);

static void setting_network_connection_entry_changed_cb(void *data,
                                                        Evas_Object *obj,
                                                        void *event_info);
static void setting_network_connection_click_softkey_save_cb(void *data,
                                                             Evas_Object *
                                                             obj, void
                                                             *event_info);
static void setting_network_connection_click_softkey_cancel_cb(void *data,
                                                               Evas_Object
                                                               *obj, void
                                                               *event_info);
static Eina_Bool setting_network_connection_click_softkey_back_cb(void *data, Elm_Object_Item *it);
/*
static void setting_network_connection_click_softkey_done_cb(void *data,
						      Evas_Object *obj,
						      void *event_info); */
static void setting_network_connection_hide_input_pannel(void *data);
static void setting_network_connection_entry_next_activated_cb(void *data, Evas_Object *obj, void *event_info);


#endif				/* __SETTING_NETWORK_CONNECTION_CREATE_H__ */
