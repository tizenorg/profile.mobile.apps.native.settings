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

#ifndef __SETTING_COMMON_DATA_EDJ_DEFINE_H__
#define __SETTING_COMMON_DATA_EDJ_DEFINE_H__
/*Cnnot be fixed when select multiple resolution */
#define ITEM_X_DEN (480.0)
#define ITEM_Y_DEN (112.0)
#define ITEM_Y_DEN2 (60.0)
#define MAIN_Y_DEN (800.0)

#define GROUP_ITEM_HEIGHT (61.0)

/*"personal_vib_layout" */
#define TOP_AREA_H			171
#define BOTTOM_BAR_AREA_H	98
#define HD_HEIGHT			1280
#define PERSONAL_VIBRATION_LAYOUT_H \
	((HD_HEIGHT)-(TOP_AREA_H)-(BOTTOM_BAR_AREA_H))

#define SETTING_LAYOUT_FOCUS_PART(bg) \
	part { name: "focus_bg_left_top_padding";\
	type: RECT;\
		scale : 1;\
	description { state: "default" 0.0;\
			min: 2 2;\
			align: 0.0 0.0;\
			visible: 0;\
			fixed: 1 1;\
	rel1.to: bg;\
			rel2 {\
				relative: 0.0 0.0;\
	to: bg;\
			}\
		}\
	}\
	part { name: "focus_bg_right_bottom_padding";\
	type: RECT;\
		scale: 1;\
	description { state: "default" 0.0;\
			min: 2 2;\
			align: 1.0 1.0;\
			visible: 0;\
			fixed: 1 1;\
			rel1 {\
				relative: 1.0 1.0;\
	to: bg;\
			}\
	rel2.to: bg;\
		}\
	}\
	part { name: "focus_bg";\
	type: IMAGE;\
	description { state: "default" 0.0;\
			visible: 0;\
			rel1 {\
				relative: 1.0 1.0;\
	to: "focus_bg_left_top_padding";\
			}\
			rel2 {\
				relative: 0.0 0.0;\
	to: "focus_bg_right_bottom_padding";\
			}\
	image.normal: "org.tizen.setting.png";\
			image.border: 13 13 13 13;\
		}\
	description { state: "enabled" 0.0;\
	inherit: "default" 0.0;\
			visible: 1;\
		}\
	}

#define SETTING_LAYOUT_FOCUS_PROGRAME \
	program { name: "focus_in";\
	signal: "elm,action,focus_highlight,show";\
	source: "elm";\
	action: STATE_SET "enabled" 0.0;\
	target: "focus_bg";\
	}\
	program { name: "focus_out";\
	signal: "elm,action,focus_highlight,hide";\
	source: "elm";\
	action: STATE_SET "default" 0.0;\
	target: "focus_bg";\
	}\
	 

#define ADD_PART_CLICKED_SOUND_HEADER \
	sounds {\
		sample {\
	name: "touch_sound" AS_IS;\
	source: "button-pressed.wav";\
		}\
	}

#define ADD_PART_CLICKED_SOUND(part)\
	program { name: "touch_snd"part;\
	signal: "mouse,clicked,1";\
	source: part;\
	action: PLAY_SAMPLE "touch_sound" 1.0;\
	}

#endif				/* __SETTING_COMMON_DATA_EDJ_DEFINE_H__ */
