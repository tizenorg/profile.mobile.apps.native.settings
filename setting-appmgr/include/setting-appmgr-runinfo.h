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
#ifndef __SETTING_APPMGR_RUNINFO_H__
#define __SETTING_APPMGR_RUNINFO_H__

#include "setting-appmgr.h"

void appmgrUg_appinfo_init(SettingAppMgrUG *ad);
Evas_Object *appmgrUg_run_gl_stop_btn(void *data, Evas_Object *obj,
		const char *part);
int appmgrUg_get_running_list(SettingAppMgrUG *ad);
void appmgrUg_free_runinfo(gpointer data);


#endif /*__SETTING_APPMGR_RUNINFO_H__ */
