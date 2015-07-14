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
#ifndef __SETTING_MEMORY_H__
#define __SETTING_MEMORY_H__

#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-debug.h>
#include <sysman.h>

typedef void (*setting_memory_mmc_cb) (int val, void *data);

typedef struct _SettingMemoryUG SettingMemoryUG;

typedef struct setting_memory_umem_size {
	unsigned long total_mem_size;
	unsigned long free_mem_size;
	unsigned long used_mem_size;
	unsigned long block_size;
	unsigned long mms_size;
	unsigned long myfiles_size;
	unsigned long etc_size;
} setting_memory_umem_size_t;

typedef struct __setting_mem_status {
	double	dTotal;
	double	dAvail;
} setting_mem_status;

/**
 * Setting Memory UG context
 * all UG function has void* as an agument. this is casted back to SettingMemoryUG
 * and the functions access app context.
 */
struct _SettingMemoryUG {
	ui_gadget_h ug;
	ui_gadget_h ug_loading;

	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *popup;
	setting_call_back_func curr_popup_cb;

	Evas_Object *progress_bar;
	Ecore_Timer *wait_timer;
	Ecore_Timer *popup_timer;
	Ecore_Timer *progress_bar_timer;
	float progress_bar_rate;
	Evas_Object *navi_bar;

	Evas_Object *msg_intial_popup;
	Evas_Object *ly_main;

	Setting_GenGroupItem_Data *data_wap;
	Setting_GenGroupItem_Data *data_bt;
	Setting_GenGroupItem_Data *data_cmr;
	Setting_GenGroupItem_Data *data_vrec;
	Setting_GenGroupItem_Data *data_rdo;
	Setting_GenGroupItem_Data *data_shr;
	Setting_GenGroupItem_Data *data_adobeAir;
	Setting_GenGroupItem_Data *data_dvb_h;

	setting_memory_umem_size_t umem_size;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_sel_all;
	Elm_Genlist_Item_Class itc_seperator;
	Elm_Genlist_Item_Class itc_variable_height;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_2text_2;
	Setting_GenGroupItem_Data *data_ex_total;
	Setting_GenGroupItem_Data *data_ex_available;
	Setting_GenGroupItem_Data *data_ex_mount;
	Setting_GenGroupItem_Data *data_ex_format;
	Setting_GenGroupItem_Data *data_in_total;
	Setting_GenGroupItem_Data *data_in_available;
	Setting_GenGroupItem_Data *data_in_format;
	Setting_GenGroupItem_Data *data_phone_storage_available;
	Setting_GenGroupItem_Data *data_main_storage_available;
	Setting_GenGroupItem_Data *data_system_storage_available;
	struct mmc_contents *mmc_content;
};

extern setting_view setting_view_memory_main;
extern setting_view setting_view_memory_default_memory;
extern setting_view setting_view_memory_status;


#endif				/* __SETTING_MEMORY_H__ */
