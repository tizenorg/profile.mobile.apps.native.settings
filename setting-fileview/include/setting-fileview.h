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
#ifndef __SETTING_FILEVIEW_H__
#define __SETTING_FILEVIEW_H__

#include <stdio.h>
#include <Elementary.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#if 1
#include <EWebKit.h>
#include <ewk_chromium.h>
#else
#include <ewk_auth_challenge.h>
#include <ewk_certificate.h>
#include <ewk_console_message.h>
#include <ewk_context.h>
#include <ewk_context_menu.h>
#include <ewk_cookie_manager.h>
#include <ewk_custom_handlers.h>
#include <ewk_enums.h>
#include <ewk_error.h>
#include <ewk_export.h>
#include <ewk_geolocation.h>
#include <ewk_log.h>
#include <ewk_main.h>
#include <ewk_notification.h>
#include <ewk_policy_decision.h>
#include <ewk_settings.h>
#include <ewk_touch.h>
#include <ewk_user_media.h>
#include <ewk_view.h>
#include <ewk_hit_test.h>
#include <ewk_text_style.h>
#include <ewk_web_application_icon_data.h>
#endif

typedef struct _SettingFileviewUG SettingFileviewUG;

/**
 * Setting About UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingFileviewUG and the functions access app context.
 */
struct _SettingFileviewUG {
	ui_gadget_h ug;
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *navi_bar;
	Evas_Object *ly_main;
	char *input_file;
	char *input_title;

	Evas_Object *webkit;
	Evas_Object *scroller;
};
extern setting_view setting_view_fileview_main;
#endif				/* __SETTING_FILEVIEW_H__ */
