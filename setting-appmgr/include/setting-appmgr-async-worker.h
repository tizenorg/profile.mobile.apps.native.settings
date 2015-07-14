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
#ifndef __SETTING_APPMGR_ASYNC_WORKER_H__
#define __SETTING_APPMGR_ASYNC_WORKER_H__

#include "setting-appmgr.h"

#define APPMGRUG_STOP_POINT appmgrUg_thread_testcancel()

typedef int (*async_fn)(SettingAppMgrUG *ad);
typedef void (*callback_fn)(int fn_result, SettingAppMgrUG *ad);

void *appmgrUg_start_async_worker(async_fn fn, callback_fn cb,
                                  SettingAppMgrUG *ad);
void appmgrUg_stop_async_worker(void *worker_id);
void appmgrUg_stop_async_worker_all(void);
void appmgrUg_thread_testcancel();

#endif /*__SETTING_APPMGR_ASYNC_WORKER_H__ */

