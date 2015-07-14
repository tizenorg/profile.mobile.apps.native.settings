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
#ifndef __SETTING_APPMGR_UTILS_H__
#define __SETTING_APPMGR_UTILS_H__

#include "setting-appmgr.h"

enum {
    APPMGRUG_SORT_ATOZ,
    APPMGRUG_SORT_ZTOA,
    APPMGRUG_SORT_SIZE,
};

GList *appmgrUg_sort_pkg_list(int sorttype, GList *pkg_list);
int appmgrUg_get_listinfos(SettingAppMgrUG *ad);
void appmgrUg_get_listinfos_cb(int fn_result, SettingAppMgrUG *ad);
void appmgrUg_get_runlistinfos_cb(int fn_result, SettingAppMgrUG *ad);
void appmgrUg_pkgmgr_subscribe(SettingAppMgrUG *ad);
void appmgrUg_pkgmgr_deinit(SettingAppMgrUG *ad);
void appmgrUg_free_listinfos(SettingAppMgrUG *ad);
Elm_Object_Item *appmgrUg_append_separator(Evas_Object *genlist,
                                           SettingAppMgrUG *ad);
void appmgrUg_size_to_str(int size, char *desc, int desc_size);
char *appmgrUg_info_title_gl_label_get(void *data, Evas_Object *obj,
                                       const char *part);
Evas_Object *appmgrUg_info_title_gl_icon_get(void *data, Evas_Object *obj,
                                             const char *part);
Evas_Object *appmgrUg_info_2button1_gl_icon_get(void *data, Evas_Object *obj,
                                                const char *part);
Evas_Object *appmgrUg_info_2button2_gl_icon_get(void *data, Evas_Object *obj,
                                                const char *part);
Evas_Object *appmgrUg_info_1button_gl_icon_get(void *data, Evas_Object *obj,
                                               const char *part);
void appmgrUg_popup_del(void *data, Evas_Object *obj, void *event_info);
void appmgrUg_fail_popup(char *str, SettingAppMgrUG *ad);
int appmgrUg_reset_app_settings(SettingAppMgrUG *ad);
Evas_Object *appmgrUg_no_item(Evas_Object *parent, const char *text,
                              const char *help);
Evas_Object *appmgrUg_loading_item(Evas_Object *parent, const char *text,
                                   const char *help);
gint appmgrUg_compare_pkgname(gconstpointer item, gconstpointer pkg_name);
char *appmgrUg_get_defualt_icon(pkgmgrinfo_appinfo_h handle);

static inline char *appmgrUg_check_icon(const char *icon_path)
{
	if (EINA_TRUE == ecore_file_exists(icon_path))
		return strdup(icon_path);
	else
		return NULL;
}


#endif /*__SETTING_APPMGR_UTILS_H__ */
