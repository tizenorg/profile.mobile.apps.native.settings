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
#ifndef __SETTING_STORAGE_ASYNC_WORKER_H__
#define __SETTING_STORAGE_ASYNC_WORKER_H__

#include "setting-storage.h"

#define storageUG_STOP_POINT storageUg_thread_testcancel()

typedef int (*async_fn)(SettingStorageUG *ad);
typedef void (*callback_fn)(int fn_result, SettingStorageUG *ad);

void *storageUg_start_async_worker(async_fn fn, callback_fn cb,
		SettingStorageUG *ad);
bool storageUg_worker_is_running(void *worker_id);
void storageUg_stop_async_worker(void *worker_id);
void storageUg_stop_async_worker_all(void);
void storageUg_thread_testcancel();

#endif /*__SETTING_STORAGE_ASYNC_WORKER_H__ */

