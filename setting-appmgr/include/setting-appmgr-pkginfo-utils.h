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
#ifndef __SETTING_APPMGR_PKGINFO_UTILS_H__
#define __SETTING_APPMGR_PKGINFO_UTILS_H__

#include "setting-appmgr.h"

int appmgrUg_pkg_get_info(SettingAppMgrUG *ad);
void appmgrUg_pkg_free_pkginfo(appmgr_pkginfo *info);
void appmgrUg_pkg_webapp_ug(void *data, Evas_Object *obj, void *event_info);
void appmgrUg_pkg_clear_default(void *data, Evas_Object *obj, void *event_info);
void appmgrUg_pkg_clear_cache(void *data, Evas_Object *obj, void *event_info);
int appmgrUg_pkg_get_privileges_grp_iter(const char *grp, void *user_data);
void *appmgrUg_pkg_moveto_worker(void *data);
void appmgrUg_pkg_moveto_worker_finish(SettingAppMgrUG *ad);
char *appmgrUg_pkg_size_gl_label_get(void *data, Evas_Object *obj,
		const char *part);
void appmgrUg_pkg_update_cache_size(void *data);

#endif /*__SETTING_APPMGR_PKGINFO_UTILS_H__ */
