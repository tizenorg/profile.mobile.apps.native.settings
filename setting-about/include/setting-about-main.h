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
#ifndef __SETTING_ABOUT_MAIN_H__
#define __SETTING_ABOUT_MAIN_H__

#include <setting-about.h>

#define SUPPORT_FOTA

#define SETTING_ABOUT_POWER_SUPPLY_PATH "/sys/class/power_supply"
#define SETTING_ABOUT_BD_ADDR_PATH	_TZ_SYS_ETC"/.bd_addr"
#define SETTING_ABOUT_STAT_PATH "/proc/stat"

#define SETTING_ABOUT_NEVER_TURN_WIFI_ON_STR "Never Turn WI-FI ON"
#define SETTING_ABOUT_DEVICE_INFO_STR "IDS_ST_BODY_DEVICE_INFO"
#define SETTING_ABOUT_DEVICE_NAME_STR "IDS_ST_BODY_NAME"
#define SETTING_ABOUT_MY_NUMBER_STR "IDS_ST_BODY_MY_NUMBER"
#define SETTING_ABOUT_IMEI_STR "IDS_ST_BODY_IMEI"
#define SETTING_ABOUT_BLUETOOTH_STR KeyStr_Bluetooth
#define SETTING_ABOUT_WIFI_STR "IDS_ST_HEADER_WI_FI"
#define SETTING_ABOUT_SN_STR "IDS_ST_BODY_SERIAL_NUMBER"
#define SETTING_ABOUT_BATTERY_STR "IDS_ST_BODY_BATTERY"
#define SETTING_ABOUT_CPU_USAGE_STR "IDS_ST_BODY_CPU_USAGE"
#define SETTING_ABOUT_STATUS_STR "IDS_ST_MBODY_DEVICE_STATUS"
#define SETTING_ABOUT_STATUS_UNROOT_STR "IDS_ST_BODY_OFFICIAL_M_DEVICE_STATUS"
#define SETTING_ABOUT_STATUS_ROOT_STR "IDS_ST_BODY_CUSTOM_M_DEVICE_STATUS"

#define SETTING_ABOUT_SLP_STR			"SLP"
#define SETTING_ABOUT_SLP_STR_LEN		3


#endif				/* __SETTING_ABOUT_MAIN_H__ */
