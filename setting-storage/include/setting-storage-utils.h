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
#ifndef __SETTING_STORAGE_UTILS_H__
#define __SETTING_STORAGE_UTILS_H__

#include "setting-storage.h"

void storageUg_popup_del(void *data, Evas_Object *obj, void *event_info);
void storageUg_get_internal_storage_status();
void storageUg_get_external_storage_status(const char *path, double *total,
		double *avail);

void storageUG_update_apps_cache_info(SettingStorageUG *ad);

void storageUg_size_to_str(double size, char *desc, int desc_size);
void storageUg_ug_layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv);
void storageUg_ug_destroy_cb(ui_gadget_h ug, void *priv);
void storageUg_fail_popup(SettingStorageUG *ad);
void storageUg_manage_app_ug(SettingStorageUG *ad);

int storageUg_get_internal_detail(SettingStorageUG *ad);
void storageUg_get_internal_detail_cb(int fn_result, SettingStorageUG *ad);

void storageug_genlist_text_update(Setting_GenGroupItem_Data *item_data,
		double size);

#endif /*__SETTING_STORAGE_UTILS_H__ */

