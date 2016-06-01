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
#ifndef __SETTING_PHONE_REGION_FORMAT_H__
#define __SETTING_PHONE_REGION_FORMAT_H__

#include <setting-phone-language-region.h>
typedef struct _Setting_Region_Table {
	char *key;
	char *desc;
} Setting_Region_Table;

#define DATA_FORMAT_SKELETON "yMd"
#define DATA_FORMAT_CATEGORY_NUM 4
#define DATA_FORMAT_DEFAULT 1

#define SETTING_PHONE_REGION_FORMAT_ANIMATOR_START_INDEX 30

static int setting_phone_region_format_get_region_fmt(char *list[],
		int item_idx[], char *region_keyStr[], int *region_num);
static Eina_Bool setting_phone_region_format_click_softkey_cancel_cb(void *data,
		Elm_Object_Item *it);
static void setting_phone_region_format_mouse_up_Gendial_list_radio_cb(
		void *data, Evas_Object *obj, void *event_info);
static int setting_phone_region_format_compare_cb(const void *d1,
		const void *d2);
int setting_phone_region_format_set_dateformat(const char *region, void *data);
#endif				/* __SETTING_PHONE_REGION_FORMAT_H__ */
