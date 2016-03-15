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

#include "setting-ringtone-util.h"

inline char *strlower(char *str)
{
	if (!str) return NULL;
	char *orig = str;
	for (; *str != '\0 '; str++)
		*str = tolower(*str);
	return orig;
};

/*remove ext name */
char *get_filename_from_fullname(const char *fullname)
{
	retvm_if(fullname == NULL, NULL, "fullname == NULL");

	char tmp[512];
	snprintf(tmp, sizeof(tmp), "%s", fullname);

	char *name = strrchr(tmp, '.');
	if (name != NULL) {
		*name = '\0';
	}
	return g_strdup(tmp);
}

char *get_media_basename(const char *dir_path, const char *name)
{
	if (isEmptyStr(dir_path) || isEmptyStr(name)) {
		return NULL;
	}

	char path[512] = {0, };
	snprintf(path, sizeof(path), "%s/%s", dir_path, name);

	metadata_extractor_h metadata = NULL;
	char *title = NULL;
	int ret = metadata_extractor_create(&metadata);
	if (ret == METADATA_EXTRACTOR_ERROR_NONE && metadata) {
		ret = metadata_extractor_set_path(metadata, path);
		if (ret == METADATA_EXTRACTOR_ERROR_NONE) {
			ret = metadata_extractor_get_metadata(metadata, METADATA_TITLE, &title);
			metadata_extractor_destroy(metadata);
			if (title)
				/*return (char *)g_strdup(title);*/
				return (char *)title;
			else
				return g_strdup(name);
		} else {
			metadata_extractor_destroy(metadata);
			return g_strdup(name);
		}
	} else {
		return g_strdup(name);
	}
}

int get_filelist_from_dir_path(char *path, Eina_List **file_list)
{
	SETTING_TRACE_BEGIN;
	DIR *pDir = NULL;
	struct dirent *ent;

	retvm_if(path == NULL, -1, "dir path is null");
	retvm_if(file_list == NULL, -1, "file_list is null");

	pDir = opendir(path);

	if (pDir == NULL) {
		return -2;
	}

	while ((ent = readdir(pDir)) != NULL) {
		fileNodeInfo *pNode = NULL;

		if (strncmp(ent->d_name, ".", 1) == 0 || strcmp(ent->d_name, "..") == 0) {
			continue;
		}

		if ((ent->d_type & DT_REG) == 0) {
			continue;
		}

		pNode = (fileNodeInfo *) malloc(sizeof(fileNodeInfo));
		if (pNode == NULL) {
			continue;
		}
		memset(pNode, 0, sizeof(fileNodeInfo));

		pNode->path = g_strdup(path);
		pNode->name = g_strdup(ent->d_name);
		pNode->media_name = get_media_basename(pNode->path, pNode->name);

		*file_list = eina_list_append(*file_list, pNode);
	}
	closedir(pDir);
	SETTING_TRACE_END;

	return 0;
}

void ringtone_play_sound(const char *sound_file, player_h **mp_handle)
{
	SETTING_TRACE_BEGIN;
	player_h *player = calloc(1, sizeof(player_h));
	if (!player) {
		SETTING_TRACE_ERROR("failed to calloc player_h");
		return;
	}

	sound_manager_set_session_type(SOUND_SESSION_TYPE_MEDIA);
	sound_manager_set_media_session_option(SOUND_SESSION_OPTION_PAUSE_OTHERS_WHEN_START, SOUND_SESSION_OPTION_INTERRUPTIBLE_DURING_PLAY);

	int err = player_create(player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("creating the player handle failed[%d]",
		                    err);
		FREE(player);
		return;
	}

	err = player_set_uri(*player, sound_file);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("error to set attribute---profile_uri[%d]",
		                    err);
		player_destroy(*player);
		FREE(player);
		return;
	}

	err = player_prepare(*player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("realizing the player handle failed[%d]",
		                    err);
		player_destroy(*player);
		FREE(player);
		return;
	}
	/* sleep(0.001); */
	player_state_e state = -1;
	player_get_state(*player, &state);
	SETTING_TRACE("state:%d", state);

	if (state != PLAYER_STATE_READY) {
		SETTING_TRACE_ERROR("state of player is invalid %d", err);
		player_unprepare(*player);
		player_destroy(*player);
		FREE(player);
		return;
	}

	/*player_set_completed_cb(*player, cb, data); */

	err = player_start(*player);
	if (err != PLAYER_ERROR_NONE) {	/* if directly return error.. */
		SETTING_TRACE_ERROR("player_start [%d]", err);
		player_unset_completed_cb(*player);
		player_unprepare(*player);
		player_destroy(*player);
		FREE(player);
		return;
	}

	*mp_handle = player;
}

/*
 *@brief the function for sound balance stop the sound.
 *
 *@param data
 */
void ringtone_stop_sound(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingRingtoneUG *ad = (SettingRingtoneUG *) data;
	setting_retm_if(ad->mp_ringtone == NULL, "ad->mp_ringtone is NULL");
	player_state_e state = -1;
	int ret = 0;
	player_h player = *(ad->mp_ringtone);
	player_get_state(*ad->mp_ringtone, &state);
	if (state == PLAYER_STATE_PLAYING) {
		if (player_stop(player) != PLAYER_ERROR_NONE) {
			SETTING_TRACE("mm player stop failed");
			ret = SETTING_MMPLAYER_STOP_ERR;
		}
	}

	/*player_unset_completed_cb(player); */

	if ((ret == 0) && (player_unprepare(player) != PLAYER_ERROR_NONE)) {
		SETTING_TRACE("mm player unrealize failed");
		ret = SETTING_MMPLAYER_UNREALIZE_ERR;
	}

	if ((ret == 0) && (player_destroy(player) != PLAYER_ERROR_NONE)) {
		SETTING_TRACE("mm player destroy failed");
		ret = SETTING_MMPLAYER_DESTROY_ERR;
	}

	sound_manager_set_session_type(SOUND_SESSION_TYPE_MEDIA);

	FREE(ad->mp_ringtone);
	ad->mp_ringtone = NULL;
}
