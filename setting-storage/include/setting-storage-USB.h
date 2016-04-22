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
#ifndef __SETTING_STORAGE_USB_H__
#define __SETTING_STORAGE_USB_H__

#include "setting-storage.h"

enum {
	SETTING_STORAGE_USB_OTG_REMOVE = 0,
	SETTING_STORAGE_USB_OTG_UNMOUNT,
	SETTING_STORAGE_USB_OTG_MOUNT
};

void storageUg_init_USB(SettingStorageUG *ad);
void storageUg_deinit_USB();

#endif /*__SETTING_STORAGE_USB_H__ */

