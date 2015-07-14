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
#ifndef __SETTING_FONT_FONT_SIZE_H__
#define __SETTING_FONT_FONT_SIZE_H__

#include <setting-font.h>
#include <system_settings.h>

static const settings_font_size_table font_size_table[] = {
	{"IDS_ST_BODY_SMALL_M_TEXTSIZE", SYSTEM_SETTINGS_FONT_SIZE_SMALL},
	{"IDS_ST_BODY_MEDIUM", SYSTEM_SETTINGS_FONT_SIZE_NORMAL},
	{"IDS_ST_BODY_TEXTSTYLE_LARGE", SYSTEM_SETTINGS_FONT_SIZE_LARGE},
	{"IDS_EMAIL_OPT_HUGE_M_TEXTSIZE", SYSTEM_SETTINGS_FONT_SIZE_HUGE},
	{"IDS_EMAIL_POP_GIANT_M_TEXTSIZE", SYSTEM_SETTINGS_FONT_SIZE_GIANT},
	{NULL, 0}
};

void setting_font_font_size_list_mouse_up_cb(void *data, Evas_Object *obj, void *event_info);
char *_item_text_font_size_keystr2_get(void *data, Evas_Object *obj, const char *part);

#endif				/* __SETTING_FONT_FONT_SIZE_H__ */
