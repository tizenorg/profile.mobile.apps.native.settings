/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
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
#ifndef __SETTING_STORAGE_H__
#define __SETTING_STORAGE_H__

#include <string.h>
#include <pthread.h>
#include <cairo.h>

#include <Elementary.h>
#include <dd-deviced.h>
#include <package-manager.h>

#include "setting-common-view.h"
#include "setting-common-draw-widget.h"
#include "setting-storage-string.h"

extern const char *storageUg_MMC_stat;
extern const char *storageUg_ENCRYPT_stat;


#define SETTING_STORAGE_PIE_RECT_OFFSET (36 * WIDGET_SCALE_FACTOR)
#define SETTING_STORAGE_DEFAULT_STORAGE_APP_DESP_SIZE (240 * WIDGET_SCALE_FACTOR)

#define SETTING_STORAGE_POPUP_TIMER	2
#define SETTING_STORAGE_PROGRESSBAR_TIMEOUT 10


#define SAFE_STRDUP(src) (src)?strdup(src):NULL

typedef struct _Setting_Storage_Status {
	double	dTotal;
	double	dAvail;
} Setting_Storage_Status;


typedef struct _SettingStorageUG SettingStorageUG;

typedef struct {
	pthread_t tid;

	double video_total;
	double audio_total;
	double misces_total;

	SettingStorageUG *ad;
} storageUg_worker_data;

struct _SettingStorageUG {
	ui_gadget_h ug;

	setting_view *main_view;
	setting_view *default_view;
	setting_view *misces_view;

	Evas_Object *win;
	Evas_Object *lo_parent;
	Evas_Object *navi;

	Evas_Object *lo_main;
	Evas_Object *gl_main;

	/* storage setting */
	Evas_Object *popup;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_2text_1icon_3;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_pie;
	Elm_Genlist_Item_Class itc_notice;
	Elm_Genlist_Item_Class itc_color_item;
	Elm_Genlist_Item_Class itc_msic_file_item;

	double sz_inter_total;
	double sz_inter_avail;
	double sz_sys;
	double sz_apps;
	double sz_pics_videos;
	double sz_audio;
	double sz_misces;
	double sz_caches;

	Setting_GenGroupItem_Data *total_space;
	Setting_GenGroupItem_Data *sys_mem;
	Setting_GenGroupItem_Data *apps;
	Setting_GenGroupItem_Data *pics_videos;
	Setting_GenGroupItem_Data *audio;
	Setting_GenGroupItem_Data *misces;
	Setting_GenGroupItem_Data *caches;
	Setting_GenGroupItem_Data *avail;
	pkgmgr_client *pc;
	pkgmgr_client *pc_total_size;

	/* pie sub_layout*/
	Elm_Object_Item *pie_it;
	cairo_t *cr;

	Setting_GenGroupItem_Data *sd_card;
	Setting_GenGroupItem_Data *sd_mount;
	Setting_GenGroupItem_Data *sd_total;
	Setting_GenGroupItem_Data *sd_avail;
	Setting_GenGroupItem_Data *sd_format;
	struct mmc_contents mmc_content;
	int sd_request;

	/* usb otg storage */
	Setting_GenGroupItem_Data *usb_card;
	Setting_GenGroupItem_Data *usb_mount;
	Setting_GenGroupItem_Data *usb_total;
	Setting_GenGroupItem_Data *usb_available;
	Setting_GenGroupItem_Data *usb_unmount;
	Setting_GenGroupItem_Data *usb_format;
	Elm_Object_Item *usb_sep_item;

	int usb_otg_status;
	int usb_request;
	char *usb_otg_path;

	void *size_worker;

	Ecore_Idler *update_idler;
	Ecore_Timer *update_timer;

	/* default storage */
	Setting_GenGroupItem_Data *data_bt;
	Setting_GenGroupItem_Data *data_nfc;
	Setting_GenGroupItem_Data *data_wifidirect;
	Setting_GenGroupItem_Data *data_installapp;

	/* miscellaneous files list */
	int misc_view_mode;				/** 0 : default 1 : delete layout */

	Elm_Object_Item *main_misces_navi_it;
	Elm_Object_Item *misces_navi_it;

	Evas_Object *gl_misces;			/** genlist for misc list */
	Evas_Object *misces_lo_noitem;

	Eina_Bool misces_is_select_all;
	Eina_List *misces_list;
	Eina_List *misces_sel_list;
	int misces_sz_all;
	int misces_checked;
	int misces_deleted;
	bool misces_ctx_popup_selected;
	void *del_worker;

	Evas_Object *ly_select_all;		/** misc > 'select all' layout */
	Evas_Object *ly_default;

	Evas_Object *title_left_btn;
	Evas_Object *title_right_btn;

	int mmc_status;
};

#endif /*__SETTING_STORAGE_H__ */
