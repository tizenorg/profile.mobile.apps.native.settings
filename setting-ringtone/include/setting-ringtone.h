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
#ifndef __SETTING_RINGTONE_H__
#define __SETTING_RINGTONE_H__

#include <stdio.h>
#include <Elementary.h>
#include <glib-object.h>
#include <player.h>
#include <sound_manager.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include "setting-common-init.h"

typedef struct _SettingRingtoneUG SettingRingtoneUG;

/**
 * Setting About UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingRingtoneUG and the functions access app context.
 */
struct _SettingRingtoneUG {
	app_control_h source_svc;

	MainData md;

	Evas_Object *ring_rgd;

	Elm_Genlist_Item_Class itc_ring;
	Eina_List *filelist;

	player_h *mp_ringtone;

	/*extra data */
	char *dir_path;
	char *file_path;
	int is_show_silent;
	int is_show_def;

	/*result data */
	char *sel_file_path;
	char *rlt_file_path;

	/*selected item data */
	Setting_GenGroupItem_Data *sel_item_data;
};
extern setting_view setting_view_ringtone_main;
#endif				/* __SETTING_RINGTONE_H__ */
