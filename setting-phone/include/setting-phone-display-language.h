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
#ifndef __SETTING_PHONE_DISPLAY_LANGUAGE_H__
#define __SETTING_PHONE_DISPLAY_LANGUAGE_H__

#include <setting-phone-language-region.h>

#define SETTING_PHONE_DISPLAY_LANGUAGE_PATH "/usr/lib/locale/"

static Eina_Bool setting_phone_display_language_click_softkey_cancel_cb(void *data, Elm_Object_Item *it);
static void setting_phone_display_language_mouse_up_Gendial_list_radio_cb(void
                                                                          *data,
                                                                          Evas_Object
                                                                          *obj,
                                                                          void
                                                                          *event_info);
static void setting_phone_display_language_close_popup_ex(void *data);
extern int setting_phone_region_format_set_dateformat(const char *region,
                                                      void *data);
#endif				/* __SETTING_PHONE_DISPLAY_LANGUAGE_H__ */
