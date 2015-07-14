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
#include <setting-memory-common.h>
#include<sys/wait.h>

int setting_memory_update_popup_with_progress_bar(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	if (ad->progress_bar != NULL) {
		SETTING_TRACE("ad->progress_bar_rate = %f",
			      (float)ad->progress_bar_rate);
		elm_progressbar_value_set(ad->progress_bar,
					  ad->progress_bar_rate);
	}

	if (ad->progress_bar_timer != NULL) {
		ecore_timer_del(ad->progress_bar_timer);
		ad->progress_bar_timer = NULL;
	}

	return 0;
}

void setting_memory_hide_popup_with_progress_bar(SettingMemoryUG *ad)
{
	SETTING_TRACE_BEGIN;
	if (ad->progress_bar) {
		elm_progressbar_value_set(ad->progress_bar, 1.0);

		evas_object_del(ad->progress_bar);
		ad->progress_bar = NULL;
	}
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (ad->popup_timer) {
		ecore_timer_del(ad->popup_timer);
		ad->popup_timer = NULL;
	}
}

/** not used now */
int setting_system_command(const char *command)
{
	SETTING_TRACE_BEGIN;
	int pid = 0, status = 0;
	char *const environ[] = { NULL };

	if (command == 0) {
		return 1;
	}

	pid = fork();

	if (pid == -1) {
		return SETTING_RETURN_FAIL;
	}

	if (pid == 0) {
		char *argv[SETTING_MEMORY_SYS_COMMAND_ARGV_LEN];

		argv[0] = "sh";
		argv[1] = "-c";
		argv[2] = (char *)command;
		argv[3] = 0;

		execve("/bin/sh", argv, environ);
		/* exit(127); */
		abort();
	}

	do {
		if (waitpid(pid, &status, 0) == -1) {
			if (errno != EINTR) {
				return SETTING_RETURN_FAIL;
			}
		} else {
			return status;
		}

	} while (1);
}
