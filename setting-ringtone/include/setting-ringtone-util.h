
#ifndef __SETTING_RINGTONE_UTIL_H__
#define __SETTING_RINGTONE_UTIL_H__

#include "setting-ringtone.h"
#include <sys/types.h>
#include <dirent.h>
#include <metadata_extractor.h>

typedef struct _ugFsNodeInfo fileNodeInfo;
struct _ugFsNodeInfo {
	char *path;
	char *name;
	char *media_name;
};

int get_filelist_from_dir_path(char *path, Eina_List **file_list);
char *get_filename_from_fullname(const char *fullname);
inline char *strlower(char *str);
void ringtone_play_sound(const char *sound_file, player_h **mp_handle);
void ringtone_stop_sound(void *data);

#endif
