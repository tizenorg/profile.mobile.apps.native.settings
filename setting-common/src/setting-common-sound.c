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
#include <setting-common-sound.h>
#include <setting-common-data-slp-setting.h>
#include <feedback.h>

/**
 * @brief Initialize function for tap sound playing
 *
 * @return #0 on success, else on failed
 */
EXPORT_PUBLIC
int effect_playsound_init()
{
	if (feedback_initialize() != 0)
		return -1;

	return 0;
}

/**
 * @brief Check whether system is in mute mode
 *
 * @return #1 on non-mute, 0 on mute
 */
EXPORT_PUBLIC
int effect_playsound_is_activated()
{
	int state = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_SOUND_STATUS_BOOL, &state);
	return state;
}

/**
 * @brief Deinitialize function for tap sound playing
 *
 */
EXPORT_PUBLIC
void effect_playsound_close()
{
	feedback_deinitialize();
}

/**
 * @brief Play tap sound
 *
 * This function is used to play system tap sound.
 * You may customized some edc for special layouts,
 * sometimes, you can insert tap sound effect code in the edc,
 * but it was complex and you should supply tap sound resource which is
 * wholely same as the system tap sound,this has potential issues,
 * that is when system sound is changed, you should change your edc
 * resource too,it's not smart.
 *
 * The another known problem is if you add sound effect code in your edc,
 * the the life of playing is limited by the life of layout.
 * eg, you have a popup which has 2 customized icons in its content, the icon
 * should be clickable. The following requirements was demaned too:
 *	 1.view will be changed to another if you click any icon of the popup
 *	 2.the popup will be disappeared after you click any icon of the popup
 * if you add the sound code in edc, the normally case is the tap sound is not
 * played in time before the popup was destroyed, and once popup was destroyed,
 * the edc sound code won't work because the layout was destoryed in that time
 *
 * Use effect_playsound_play() will block the cpu until tap Sound was played,
 * Only after that the cpu can do popup destoryed
 *
 */
EXPORT_PUBLIC
void effect_playsound_play()
{
	/* sound type -> feedback_pattern */
	feedback_play(FEEDBACK_PATTERN_TAP);
}

