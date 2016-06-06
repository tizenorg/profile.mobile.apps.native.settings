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
#ifndef __SETTING_APPLICATIONS_DEFAULTAPP_H__
#define __SETTING_APPLICATIONS_DEFAULTAPP_H__

#include <setting-applications.h>
#define SUPPORT_CENTER_POINT 0

#define KeyStr_Brightness_Adjust "IDS_ST_BODY_ADJUST_BRIGHTNESS_LEVEL"
#define KeyStr_Brightness_Auto_Adjust "IDS_ST_BODY_ADJUST_AUTOMATIC_BRIGHTNESS"
#define KeyStr_Brightness_Overheating \
	"IDS_ST_BODY_TO_AVOID_OVERHEATING_MAXIMUM_BRIGHTNESS_HAS_BEEN_REDUCED"
#define MGRAPP_STR_LAUNCH_BY_DEFAULT "IDS_ST_BODY_LAUNCH_BY_DEFAULT"
#define MGRAPP_STR_CLEAR_DEFAULT "IDS_ST_BODY_CLEAR_DEFAULTS"

void __display_int_vconf_cb(keynode_t *key, void *data);

#endif				/* __SETTING_APPLICATIONS_DEFAULTAPP_H__ */
