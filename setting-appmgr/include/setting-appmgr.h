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
#ifndef __SETTING_APPMGR_H__
#define __SETTING_APPMGR_H__

#include <pthread.h>
#include <glib.h>

#include <Elementary.h>
#include <pkgmgr-info.h>
#include <package-manager.h>

#include "setting-common-view.h"
#include "setting-common-draw-widget.h"
#include "setting-appmgr-string.h"

#define _S(s)	dgettext("sys_string", s)

#define SAFE_STRDUP(src) (src)?strdup(src):NULL

#define FREEandSTRDUP(dest, src) \
	do{ \
		if (dest) { free(dest); dest=NULL; }\
		if (src || *src) dest = strdup(src); \
	}while(false)

enum {
    APPMGRUG_TAB_NONE,
    APPMGRUG_TAB_DOWNLOAD,
    APPMGRUG_TAB_RUNNING,
    APPMGRUG_TAB_ALL,
    APPMGRUG_TAB_DISABLED,
    APPMGRUG_TAB_MAX
};

enum {
    APPMGRUG_PKG_REQUEST_NONE,
    APPMGRUG_PKG_REQUEST_UNINSTALL,
    APPMGRUG_PKG_REQUEST_MOVE,
    APPMGRUG_PKG_REQUEST_DISABLE,
    APPMGRUG_PKG_REQUEST_ENABLE,
    APPMGRUG_PKG_REQUEST_RESET
};

typedef struct _appmgr_pkginfo {
	char *pkgid;
	char *pkg_label;
	char *icon_path;
	char *pkg_type;
	char *pkg_version;
	char *main_appid;
	int sz_total;
	int sz_data;
	int sz_cache;
	bool nodisplay;
	bool removable;
	bool taskmanage;
	bool is_movable;
	bool is_preload;
	bool is_updated;
	bool is_support_disable;
	bool is_disable;

	GList *appids;
	pkgmgrinfo_installed_storage storage;

	pkgmgr_client *pc_size;
	pkgmgr_client *pc_request;
	Setting_GenGroupItem_Data *total_size;
	Setting_GenGroupItem_Data *data_size;
	Setting_GenGroupItem_Data *app_size;

	Setting_GenGroupItem_Data *first_prv;
	Setting_GenGroupItem_Data *last_prv;

	Elm_Object_Item *def_sep;
	Evas_Object *gl_prv;

	pthread_t tid;
} appmgr_pkginfo;

typedef struct _appmgr_listinfo {
	Elm_Object_Item *item;

	int group_style;

	char *pkgid;
	char *icon_path;
	char *pkg_label;
	char *mainappid;

	int total_size;
	bool valid_size;
	bool is_preload;
	bool is_update;
	bool is_disabled;
	GList *runinfos;
	GList *def_appids;
} appmgr_listinfo;

typedef struct _appmgr_runinfo {
	char *appid;
	char *label;
	bool can_kill;
} appmgr_runinfo;

typedef struct _SettingAppMgrUG {
	ui_gadget_h ug;
	int tabtype;
	int sorttype;
	int can_sizesort;
	int pkg_request;

	setting_view *main_view;
	setting_view *runinfo_view;
	setting_view *pkginfo_view;

	Evas_Object *win;
	Evas_Object *lo_parent;

	Evas_Object *lo_main;
	Evas_Object *navi;
	Evas_Object *popup;

	/*main view */
	Elm_Object_Item *navi_main_it;
	Evas_Object *gl_main;
	Evas_Object *lo_noitem;
	int noitem_type;
	Evas_Object *def_toolbar;
	pkgmgr_client *pc_main;
	pkgmgr_client *pc_all_size;
	GList *pkg_list;
	GList *old_pkg_list;
	Ecore_Idler *size_idler;
	void *reset_worker;
	void *list_worker;
	void *running_list_worker;

	char *sel_pkgid;
	char *sel_icon;
	char *sel_label;
	int sel_total;
	int sel_data_size;
	int sel_is_disabled;

	int is_runlist_ready;

	/*run info view */
	Evas_Object *gl_run;
	GList *runinfos;
	appmgr_listinfo *info_run;

	/*Pkg info view */
	bool support_taskmanager;
	Evas_Object *gl_pkg;
	Evas_Object *pkg_toolbar;
	char *pinfo_version;
	appmgr_pkginfo *pkginfo;

	Ecore_Timer *pkginfo_getsize_timer;

	Elm_Genlist_Item_Class itc_main;
	Elm_Genlist_Item_Class info_itc;
	Elm_Genlist_Item_Class itc_sep;
	Elm_Genlist_Item_Class itc_1txt_1ic_2;
	Elm_Genlist_Item_Class itc_1txt;
	Elm_Genlist_Item_Class itc_priv;
	Elm_Genlist_Item_Class itc_multiline;
	Elm_Genlist_Item_Class itc_grp_title;
	Elm_Genlist_Item_Class itc_info_title;
	Elm_Genlist_Item_Class itc_1ic;
	Elm_Genlist_Item_Class itc_2txt_2;
	Elm_Genlist_Item_Class itc_multiline_2txt;
	Elm_Genlist_Item_Class itc_2button1;
	Elm_Genlist_Item_Class itc_2button2;
	Elm_Genlist_Item_Class itc_1button;

	Setting_GenGroupItem_Data *cache_size;
	Setting_GenGroupItem_Data *clear_cache;
} SettingAppMgrUG;

#define APPMGRUG_TIME(fmt, arg...) \
	do { \
		struct timeval tv; \
		gettimeofday(&tv, NULL); \
		double now = tv.tv_sec * 1000 + (double)tv.tv_usec/1000; \
		SETTING_TRACE_DEBUG(fmt" time = %f ms", ##arg, now); \
	}while(0)


extern setting_view setting_view_appmgr_pkginfo;
extern setting_view setting_view_appmgr_main;

#endif /*__SETTING_APPMGR_H__ */
