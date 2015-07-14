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
#ifndef __SETTING_MEMORY_STATUS_H__
#define __SETTING_MEMORY_STATUS_H__

#include <setting-memory-common.h>
#include <setting-debug.h>

#ifndef _A_PROJECT_
#define _A_PROJECT_
#endif

#include <sys/statvfs.h>
#include <sys/stat.h>
#include <sys/mount.h>

#define SETTING_MEMORY_EMAIL_MAX "5000"
#define SETTING_MEMORY_MSG_SMS_MAX "2600"
#define SETTING_MEMORY_MSG_MMS_MAX "800"
#define SETTING_MEMORY_CONTACT_MAX "100"
#define SETTING_MEMORY_CAL_APPOINT_MAX "500"
#define SETTING_MEMORY_CAL_ANNIVERS_MAX "100"
#define SETTING_MEMORY_MEMO_MAX "400"

#define SETTING_MEMORY_STATUS_EX_SD_CARD_MOUNT_ALREADY_DESC "Already mount"
#define SETTING_MEMORY_STATUS_EX_SD_CARD_DESC "IDS_ST_BODY_SD_CARD"
#define SETTING_MEMORY_STATUS_TOTAL_SPACE_DESC _("IDS_ST_BODY_TOTAL_SPACE")
#define SETTING_MEMORY_STATUS_AVAILABLE_SPACE_DESC _("IDS_ST_BODY_AVAILABLE_SPACE")
#define SETTING_MEMORY_STATUS_MOUNT_SD_CARD_DESC "IDS_ST_BODY_MOUNT_SD_CARD"
#define SETTING_MEMORY_STATUS_UNMOUNT_SD_CARD_DESC "IDS_ST_BODY_UNMOUNT_SD_CARD"
#define SETTING_MEMORY_STATUS_FORMAT_SD_CARD_DESC "IDS_ST_BODY_FORMAT_SD_CARD"
#define SETTING_MEMORY_STATUS_INTERNAL_SD_CARD_DESC "Internal SD card"
#define SETTING_MEMORY_STATUS_MAIN_STORAGE_DESC "Main storage"

#define SETTING_MEMORY_STATUS_SYSTEM_SPACE_DESC "IDS_COM_BODY_SYSTEM_STORAGE"

#define SETTING_MEMORY_STATUS_FORMAT_ASK_MSG "This action will erase the<br>external storage in the phone.<br>You will lost ALL data on the card!<br>Format?"

#define SETTING_MEMORY_STATUS_PROGRESSBAR_TIMEOUT 10

enum {
	SETTING_MEMORY_STATUS_SYSMAN_REQUEST_MOUNT_ALREADY = -2,
	SETTING_MEMORY_STATUS_SYSMAN_REQUEST_SUCCESS = 0
};

static int __fs_stat(double* pdTotal, double* pdAvail, const char* szPath);

static void setting_memory_status_click_softkey_back(void *data,
						     Evas_Object *obj,
						     void *event_info);
static void setting_memory_status_ex_sd_card_info(char *ex_total_space,
						  char *ex_available_space,
						  char *ex_mount_desc,
						  int *ex_mmc_status);
static int setting_memory_status_ex_sd_card_detail(char *ex_total_space,
						   char *ex_available_space);
static void setting_memory_status_mouse_up_Gendial_list_cb(void *data,
							   Evas_Object *obj,
							   void *event_info);
static void setting_memory_status_ex_sd_card_format_promt_popup_create(void
								       *data);
static void setting_memory_status_ex_sd_card_ask_format_resp_cb(void *data,
								Evas_Object *
								obj,
								void
								*event_info);
static void setting_memory_status_ex_sd_card_format_promt_popup_left_btn_cb(void
									    *data,
									    Evas_Object
									    *
									    obj,
									    void
									    *event_info);
static void
setting_memory_status_ex_sd_card_format_promt_popup_right_btn_cb(void *data,
								 Evas_Object *
								 obj,
								 void
								 *event_info);
static void setting_memory_status_ex_sd_card_mount_unmount(void *data);
static void setting_memory_status_ex_sd_card_refresh_card_details_cb(void *data,
								     Evas_Object
								 *obj,
								     void
								     *event_info);
static void setting_memory_status_ex_sd_card_operate_complete(void *data,
							      char *desc,
							      bool popup_flag);
static void setting_memory_status_list_item_enable_set_by_mmc_status(void *data,
								     int
								     ex_mmc_status);
static void setting_memory_status_list_item_data_set(Setting_GenGroupItem_Data *
						     item, char *data);
static void setting_memory_status_get_phone_storage_available_size(void *data,
								   char
								   *phone_storage_available);
static int setting_memory_status_size_transition(double size,
						 char *size_desc);
static void setting_memory_status_ex_sd_card_format_cb(int val, void *data);
static void setting_memory_status_ex_sd_card_mount_cb(int val, void *data);
static void setting_memory_status_ex_sd_card_unmount_cb(int val, void *data);

#endif				/* __SETTING_MEMORY_STATUS_H__ */
