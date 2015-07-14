/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SETTING_MENUSCREEN_PASSWORD_H__
#define __SETTING_MENUSCREEN_PASSOWRD_H__

#include <setting-menuscreen.h>

/** @todo need to i18n string registration */

static void setting_menuscreen_password_click_softkey_back_cb(void *data, Evas_Object *obj, void *event_info);

static void setting_menuscreen_password_entry_change_cb (void *data, Evas_Object *obj, void *event_info);
static void setting_menuscreen_password_click_softkey_done_cb(void *data, Evas_Object *obj, void *event_info);

#endif				/* __SETTING_MENUSCREEN_PASSWORD_H__ */
