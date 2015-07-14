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
#ifndef __SETTING_MEMORY_COMMON_H__
#define __SETTING_MEMORY_COMMON_H__

#include <setting-memory.h>
#include <sys/vfs.h>

#define VCONFKEY_SETAPPL_MMC_FORMAT "memory/mmc/format"
typedef enum {
	SETTING_MMC_FORMAT_INVALID = -1,
	SETTING_MMC_FORMAT_FORMATTED = 0,	/* sucess */
	SETTING_MMC_FORMAT_FORMATTING = 1,	/* processing */
	SETTING_MMC_FORMAT_FORMATTED_FAILED = 2	/* failed */
} SETTING_MMC_FORMAT_STATUS;

#define STR_SIZE									30
#define SETTING_MEMORY_MMS_DEFAULT_FOLDER_SIZE		4
#define SETTING_MEMORY_EMAIL_DEFAULT_FOLDER_SIZE	8
#define SETTING_MEMORY_MYFILES_DEFAULT_FOLDER_SIZE	0
#define SETTING_MEMORY_BUFFER_SIZE					255
#define SETTING_MEMORY_MMC_MAX_NAME_LEN				360
#define SETTING_MEMORY_MMC_DEV_PATH_LEN				30
#define SETTING_MEMORY_MMC_FILEPATH_LEN_MAX			255
#define SETTING_MEMORY_MMC_MAX_MEM_STR_LEN			30
#define SETTING_MEMORY_MMC_MOUNT_PATH_LEN			30
#define SETTING_MEMORY_MMC_MAX_CARD_NAME_LEN		11
#define SETTING_MEMORY_MMC_MLABEL_MAX_COMMAND_LEN	50
#define SETTING_MEMORY_MYFILES_RESERVED_SPACE		2000
#define SETTING_MEMORY_POPUP_TIMER					1
#define SETTING_MEMORY_CLEAR_PHONE_CMD_LEN_MAX		80
#define SETTING_MEMORY_SYS_COMMAND_ARGV_LEN			4

#define SETTING_MEMORY_KILOBYTE_VALUE				1024
#define SETTING_MEMORY_MEGABYTE_VALUE				1048576
#define SETTING_MEMORY_GIGABYTE_VALUE				1073741824


/* These definition will be removed */
#define SETTING_MEMORY_TEMP_FILE_PATH	"/tmp/mmc_tmp_file"
#define SETTING_MEMORY_FILE_SYSTEM_PATH	"/opt/usr/media/"
#define SETTING_MEMORY_MY_FILES_IMAGES_PATH	"/opt/usr/media/Images"
#define SETTING_MEMORY_MY_FILES_VIDEOS_PATH	"/opt/usr/media/Videos"
#define SETTING_MEMORY_MY_FILES_SOUNDS_PATH	"/opt/usr/media/Sounds"
#define SETTING_MEMORY_MY_FILES_GAMES_PATH	"/opt/usr/media/Games"
#define SETTING_MEMORY_MY_FILES_OTHER_FILES_PATH	"/opt/usr/media/Other files"
#define SETTING_MEMORY_MM_MSG_PATH	"/opt/usr/media/.message/"
#define SETTING_MEMORY_CLEAR_PHONE_CALENDAR_FILE_PATH \
					"/opt/dbspace/.calendar.db"
#define SETTING_MEMORY_CLEAR_PHONE_MEMO_FILE_PATH	"/opt/dbspace/.memo.db"
#define SETTING_MEMORY_CLEAR_PHONE_MEMO_CONTACT_PATH \
					"/opt/dbspace/.contact.db"

/* New directory list */
/*use df command to check all the file system, where,
* mmc storage space: /opt/storage/sdcard
* myfile storage space: /opt/media
* email, mms, contact, calendar...etc sotrage space: /opt
* your linux maped storage spces: /mnt/nfs
*/
#define SETTING_MEMORY_USTORG_UMS_PATH			"/opt/usr/media"
#define SETTING_MEMORY_USTORG_MOVIFS_PATH		"/mnt/movifs"
#define SETTING_MEMORY_USTORG_OPT_PATH			"/opt"
#define SETTING_MEMORY_USTORG_MCC_PATH			"/opt/storage/sdcard"
#define SETTING_MEMORY_STATUS_MMC_PARENT_PATH 	"/opt/storage/sdcard/.."

#define SETTING_MEMORY_STATUS_APP_PATH 			"/opt"
#define SETTING_MEMORY_STATUS_MEDIA_PATH		"/opt/usr/media"
#define SETTING_MEMORY_STATUS_MMC_PATH 			"/opt/storage/sdcard"

#define SETTING_MEMORY_ICON_MMS_PATH \
			SETTING_UG_ICON_PATH"/68_icon_multi_message.png"
#define SETTING_MEMORY_ICON_EMAIL_PATH \
			SETTING_UG_ICON_PATH"/68_icon_e-mail.png"
#define SETTING_MEMORY_ICON_MYFILES_PATH \
			SETTING_UG_ICON_PATH"/68_icon_myfiles.png"
#define SETTING_MEMORY_ICON_OTHERS_PATH \
			SETTING_UG_ICON_PATH"/68_icon_others.png"
#define SETTING_MEMORY_ICON_DUMMY_PATH \
			SETTING_UG_ICON_PATH"/68_memory_status_bg.png"

typedef struct _setting_LABEL_info {
	char *LABEL_EDJ_NAME;
	char *LABEL_GRP_NAME;
	char *LABEL_ITEM_TITLE_STR;

} setting_LABEL_info;

int setting_system_command(const char *command);

int setting_memory_update_popup_with_progress_bar(void *data);
void setting_memory_hide_popup_with_progress_bar(SettingMemoryUG *ad);

#endif				/* __SETTING_MEMORY_COMMON_H__ */
