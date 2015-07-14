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
#ifndef __SETTING_CONNECTIVITY_USB_H__
#define __SETTING_CONNECTIVITY_USB_H__

#include <setting-connectivity.h>

#define NUM_PROGRESSBAR 4

#define SETTING_USB_DEBUGGING_DESC "IDS_ST_BODY_DEBUGGING_MODE_LAUNCHES_WHEN_USB_IS_CONNECTED"

enum progressbar_status {
    STATE_NONE = 0x00,
    STATE_PROGRESSBAR
};

typedef struct _button_status button_status;

struct _button_status {
	Evas_Object *progressbar;	/*  progressbar */
	Elm_Object_Item *item;	/*  button */
	int pstate;		/*  progressbar status to show or hide */
};

static void setting_connectivity_usb_click_softkey_cancel_cb(void *data,
                                                             Evas_Object *obj,
                                                             void *event_info);
#ifdef HELP_UG_EXIST
static void setting_connectivity_usb_click_softkey_set_cb(void *data,
                                                          Evas_Object *obj,
                                                          void *event_info);
#endif

#endif				/* __SETTING_CONNECTIVITY_USB_H__ */
