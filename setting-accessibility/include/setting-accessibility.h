/*
 * accessibility
 *
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
 *
 * Contact: Tomasz Olszak <t.olszak@samsung.com>
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
#ifndef __SETTING_ACCESSIBILITY_H__
#define __SETTING_ACCESSIBILITY_H__

#include <Elementary.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include "setting-common-init.h"

#define DEVOPTION_STR_ACCESSIBILITY_VISION	"IDS_ST_HEADER_VISION"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_SPEECH \
	"IDS_ST_BODY_SPEECH"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_ON	"IDS_ST_BODY_ON"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_OFF	"IDS_ST_BODY_OFF"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS "IDS_ST_OPT_SETTINGS"

#define DEVOPTION_STR_ACCESSIBILITY_MAGNIFIER	"IDS_ST_BODY_MAGNIFIER"
#define DEVOPTION_STR_ACCESSIBILITY_SCREENREADER \
	"IDS_ST_MBODY_SCREEN_READER_HTTS"
/*english only translation */
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_DESCRIPTION \
	"While Screen reader is enabled, your device provides spoken feedback."\
	" For example, Screen reader tells you what you can tap, select, or "\
	"enable. Basic interactions include tapping an item to select it and "\
	"double tapping the item to enable its function."
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_OTHER_FEEDBACK \
	"IDS_ACCS_OPT_OTHER_FEEDBACK_TTS"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_STATUS_BAR_INFORMATION \
	"IDS_ACCS_TMBODY_STATUS_BAR_INFORMATION"
#define DEVOPTION_STR_ACCESSIBILITY_SCREEN_READER_SETTINGS_STATUS_BAR_INFORMATION_DESCRIPTION \
	"IDS_ACCS_SBODY_SELECT_WHICH_INFORMATION_FROM_INDICATORS_SHOWN_ON_THE_"\
	"STATUS_BAR_WILL_BE_READ_OUT_WHEN_YOU_TRIPLE_TAP_THE_SCREEN_WITH_TWO_"\
	"FINGERS_MSG"

#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_TIME	"IDS_ST_HEADER_TIME"
#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_BATTERY	"IDS_ST_BODY_BATTERY"
#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_BLUETOOTH	"IDS_ST_BODY_BLUETOOTH"
#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_DATE	"IDS_ST_BODY_DATE"
#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_SIGNAL_STRENGTH \
	"IDS_ST_BODY_SIGNAL_STRENGTH"
#define DEVOPTION_STR_ACCESSIBILITY_INDICATOR_UNREAD_NOTIFICATIONS \
	"IDS_SCR_BODY_PD_UNREAD_NOTIFICATIONS"


typedef struct _SettingAccessibilityUG SettingAccessibilityUG;

struct _SettingAccessibilityUG {
	MainData md;

	Elm_Genlist_Item_Class itc_1text;

	bool empty_flag;
};

extern setting_view setting_view_accessibility_main;

#endif /*__SETTING_ACCESSIBILITY_H__ */
