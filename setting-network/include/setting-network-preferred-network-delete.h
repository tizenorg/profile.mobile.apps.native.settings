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
#ifndef __SETTING_NETWORK_PREFERRED_NETWORK_DELETE_H__
#define __SETTING_NETWORK_PREFERRED_NETWORK_DELETE_H__

#include <setting-network.h>
#include <setting-network-preferred-network.h>

void setting_network_preferred_network_delete_refresh_view(SettingNetworkUG *
                                                           ad);
void setting_network_preferred_network_delete_draw_network_list(SettingNetworkUG
                                                                *ad);

void setting_network_preferred_network_delete_ok(void *data);
void setting_network_preferred_network_delete_failed(void *data);

#endif				/* __SETTING_NETWORK_PREFERRED_NETWORK_DELETE_H__ */
