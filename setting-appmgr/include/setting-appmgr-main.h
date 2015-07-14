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
#ifndef __SETTING_APPMGR_MAIN_H__
#define __SETTING_APPMGR_MAIN_H__

#include "setting-appmgr.h"

void appmgrUg_main_init(SettingAppMgrUG *ad);
void appmgrUg_main_genlist_append_items(SettingAppMgrUG *ad);
char *appmgrUg_main_gl_label_get(void *data, Evas_Object *obj, const char *part);
Evas_Object *appmgrUg_main_gl_icon_get(void *data, Evas_Object *obj,
                                       const char *part);

#endif /*__SETTING_APPMGR_MAIN_H__ */
