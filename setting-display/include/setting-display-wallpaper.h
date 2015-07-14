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
#ifndef __SETTING_DISPLAY_WALLPAPER_H__
#define __SETTING_DISPLAY_WALLPAPER_H__

#include <setting-display.h>

#define SETTING_WALLPAPER_HOME_AND_LOCK_SCREEN "IDS_ST_BODY_HOME_AND_LOCK_SCREEN_WALLPAPER"
#define SETTING_WALLPAPER_HOME_SCREEN_CHANGED "Home screen changed"
#define SETTING_WALLPAPER_LOCK_SCREEN_CHANGED "Lock screen changed"
#define SETTING_WALLPAPER_HOME_AND_LOCK_SCREEN_CHANGED "Home screen and Lock screen changed"

static void setting_display_wallpaper_click_softkey_cancel_cb(void *data,
							      Evas_Object *obj,
							      void *event_info);
#endif				/* __SETTING_DISPLAY_WALLPAPER_H__ */
