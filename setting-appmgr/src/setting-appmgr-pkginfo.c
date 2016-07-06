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
#include <appsvc.h>
#include <aul_svc.h>
#include <package_manager.h>
#include <privilege_info.h>

#include "setting-appmgr-utils.h"
#include "setting-appmgr-pkginfo-utils.h"
#include "setting-appmgr-pkginfo.h"

#define APPMGRUG_PROGRESSBAR_TIMEOUT 10

setting_view setting_view_appmgr_pkginfo;

static Eina_Bool appmgrUg_pkg_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, EINA_FALSE);

	if (ad->support_taskmanager) {
		ug_destroy_me(ad->ug);
		return EINA_FALSE;
	} else {
		setting_view_destroy(ad->pkginfo_view, ad);
		return EINA_TRUE;
	}
}

static inline void appmgrUg_pkg_append_storage(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	appmgr_pkginfo *info;
	char desc[APPMGRUG_MAX_STR_LEN] = { 0 };

	ret_if(NULL == ad);

	info = ad->pkginfo;

	/*appmgrUg_append_separator(ad->gl_pkg, ad); */

	setting_create_Gendial_field_titleItem(ad->gl_pkg, &(ad->itc_grp_title),
	MGRAPP_STR_STORAGE, NULL);

	/* Total size */
	if (0 <= ad->sel_total)
		appmgrUg_size_to_str(ad->sel_total, desc, sizeof(desc));
	else
		snprintf(desc, sizeof(desc), "%s", MGRAPP_STR_COMPUTING);
	info->total_size = setting_create_Gendial_field_def(ad->gl_pkg,
			&ad->itc_2txt_2,
			NULL, NULL, SWALLOW_Type_INVALID, NULL, NULL, 0,
			_(MGRAPP_STR_TOTAL), desc,
			NULL);

	if (info->total_size) {
		elm_genlist_item_select_mode_set(info->total_size->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	/* App size */
	if (0 <= info->sz_total && 0 <= info->sz_data)
		appmgrUg_size_to_str(info->sz_total - info->sz_data, desc,
				sizeof(desc));
	else
		snprintf(desc, sizeof(desc), "%s", MGRAPP_STR_COMPUTING);
	info->app_size = setting_create_Gendial_field_def(ad->gl_pkg,
			&ad->itc_2txt_2,
			NULL, NULL, SWALLOW_Type_INVALID, NULL, NULL, 0,
			MGRAPP_STR_APPLICATION, desc, NULL);
	if (info->app_size) {
		elm_genlist_item_select_mode_set(info->app_size->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	/* Data size */
	if (0 <= info->sz_data)
		appmgrUg_size_to_str(info->sz_data, desc, sizeof(desc));
	else
		snprintf(desc, sizeof(desc), "%s", MGRAPP_STR_COMPUTING);
	info->data_size = setting_create_Gendial_field_def(ad->gl_pkg,
			&ad->itc_2txt_2,
			NULL, NULL, SWALLOW_Type_INVALID, NULL, NULL, 0,
			MGRAPP_STR_DATA, desc, NULL);
	if (info->data_size) {
		elm_genlist_item_select_mode_set(info->data_size->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

}

static inline void appmgrUg_pkg_append_cache(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	appmgr_pkginfo *info;
	Setting_GenGroupItem_Data *item_data = NULL;
	char desc[APPMGRUG_MAX_STR_LEN] = { 0 };

	ret_if(NULL == ad);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	/*appmgrUg_append_separator(ad->gl_pkg, ad); */

	/* title */
	setting_create_Gendial_field_titleItem(ad->gl_pkg, &(ad->itc_grp_title),
	MGRAPP_STR_CACHE, NULL);

	/* cache size */
	if (0 <= info->sz_cache)
		appmgrUg_size_to_str(info->sz_cache, desc, sizeof(desc));
	else
		snprintf(desc, sizeof(desc), "%s", MGRAPP_STR_COMPUTING);

	SETTING_TRACE(
			" *********************************** ----> DESC : %s",
			desc);

	ad->cache_size = setting_create_Gendial_field_def(ad->gl_pkg,
			&ad->itc_2txt_2,
			NULL, NULL, SWALLOW_Type_INVALID, NULL, NULL, 0,
			MGRAPP_STR_CACHE, desc, NULL);
	if (ad->cache_size) {
		__BACK_POINTER_SET(ad->cache_size);
		elm_genlist_item_select_mode_set(ad->cache_size->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}

	/* clear cache */
	item_data = (Setting_GenGroupItem_Data *)calloc(1,
			sizeof(Setting_GenGroupItem_Data));
	if (item_data) {
		ad->clear_cache = item_data;
		__BACK_POINTER_SET(ad->clear_cache);
		item_data->userdata = ad;
		item_data->item = elm_genlist_item_append(ad->gl_pkg,
				&(ad->itc_1button), item_data, NULL,
				ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}
}

static inline void appmgrUg_pkg_append_default(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	GList *cur;
	appmgr_pkginfo *info;

	ret_if(NULL == ad);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	cur = info->appids;

#if 0
	while (cur) {
		char *appid = cur->data;

		/*TODO */
		/* appsvc has bad api */
		/*		if (appid && 1 == appsvc_is_defapp(appid)) */
		/*			break; */

		cur = cur->next;
	}
#endif

	if (NULL == cur) {
		SETTING_TRACE_ERROR("No defualt Application");
		return;
	}

	/*info->def_sep = appmgrUg_append_separator(ad->gl_pkg, ad); */
	char *appid = cur->data;
	int ret = aul_svc_is_defapp(appid);

	setting_create_Gendial_field_titleItem(ad->gl_pkg, &(ad->itc_grp_title),
			MGRAPP_STR_LAUNCH_BY_DEFAULT, NULL);
	Setting_GenGroupItem_Data *data = setting_create_Gendial_field_def(
		ad->gl_pkg, &ad->itc_1txt,
		appmgrUg_pkg_clear_default, ad, SWALLOW_Type_INVALID,
		NULL, NULL, 0,
		MGRAPP_STR_CLEAR_DEFAULTS, NULL, NULL);
	ret_if(NULL == data);

	setting_create_Gendial_field_titleItem(ad->gl_pkg, &ad->itc_multiline,
			MGRAPP_STR_DEFAULT_DESC, NULL);
	if (1 != ret)
		elm_object_item_disabled_set(data->item, EINA_TRUE);
}

static inline void appmgrUg_pkg_append_website(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	appmgr_pkginfo *info;

	ret_if(NULL == ad);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	if (safeStrCmp(info->pkg_type, "wgt")) {
		SETTING_TRACE_ERROR("No Web Application");
		return;
	}

	/*appmgrUg_append_separator(ad->gl_pkg, ad); */

	setting_create_Gendial_field_def(ad->gl_pkg, &ad->itc_1txt,
			appmgrUg_pkg_webapp_ug, ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0,
			MAGAPP_STR_WEB_SETTING, NULL, NULL);
}

static inline void appmgrUg_pkg_append_privilege(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	appmgr_pkginfo *info = NULL;
	Setting_GenGroupItem_Data *title_itdata = NULL;

	ret_if(NULL == ad);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	title_itdata = setting_create_Gendial_field_titleItem(ad->gl_pkg,
			&(ad->itc_grp_title),
			MGRAPP_STR_PRIVILEGE, NULL);

	info->first_prv = NULL;

	/* Deprecated API */
	ret = privilege_info_foreach_privilege_group_list_by_pkgid(info->pkgid,
			appmgrUg_pkg_get_privileges_grp_iter, ad);
	warn_if(PRVMGR_ERR_NONE != ret,
			"privilege_info_foreach_privilege_group_list_by_pkgid() Fail(%d)",
			ret);

	if (NULL == info->first_prv) {
		SETTING_TRACE_ERROR("No privilege");
		/*elm_object_item_del(sep_it); */
		if (title_itdata)
			elm_object_item_del(title_itdata->item);
		return;
	}
}

static void appmgrUg_pkg_uninstall(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	int ret;
	char *btn_str;
	appmgr_pkginfo *info;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	ret = safeStrCmp(btn_str, _(MGRAPP_STR_UNINSTALL));
	free(btn_str);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (0 != ret)
		return;

	ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
			PROGRESSBAR_STYLE, NULL,
			MGRAPP_STR_UNINSTALLING, appmgrUg_popup_del, 0, TRUE,
			TRUE, 0);

	if (info->pc_request)
		pkgmgr_client_free(info->pc_request);
	info->pc_request = pkgmgr_client_new(PC_REQUEST);
	if (NULL == info->pc_request) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Fail");
		return;
	}

	ad->pkg_request = APPMGRUG_PKG_REQUEST_UNINSTALL;
	ret = pkgmgr_client_uninstall(info->pc_request, info->pkg_type,
			info->pkgid, PM_QUIET,
			NULL, NULL);
	if (ret < PKGMGR_R_OK) {
		SETTING_TRACE_ERROR("pkgmgr_client_uninstall() Fail(%d)", ret);
		ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
		appmgrUg_fail_popup(MGRAPP_STR_UNINSTALL_FAILED, ad);
	}
}

void appmgrUg_pkg_uninstall_click(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);

	if (ad->popup)
		evas_object_del(ad->popup);
	ad->popup = setting_create_popup(ad, ad->win, MGRAPP_STR_UNINSTALL,
			MGRAPP_STR_UNINSTALL_MSG, appmgrUg_pkg_uninstall, 0,
			FALSE, FALSE, 2, MGRAPP_STR_UNINSTALL,
			MGRAPP_STR_CANCEL);
}

void appmgrUg_pkg_clear_cache_click(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAppMgrUG *ad = data;
	ret_if(NULL == ad->pkginfo);

	appmgr_pkginfo *info = ad->pkginfo;
	int ret = package_manager_clear_cache_dir(info->pkgid);
	if (ret != 0) {
		SETTING_TRACE_ERROR("failed to invoke, ret = %d", ret);
	} else {
		info->sz_cache = 0;
		appmgrUg_pkg_update_cache_size(ad);
	}
}

void appmgrUg_pkg_moveto_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	int ret;
	appmgr_pkginfo *info;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;
	if (ad->popup)
		evas_object_del(ad->popup);

	ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
			PROGRESSBAR_STYLE,
			NULL, MGRAPP_STR_MOVEING, appmgrUg_popup_del, 0, TRUE,
			TRUE, 0);

	ad->pkg_request = APPMGRUG_PKG_REQUEST_MOVE;

	ret = pthread_create(&info->tid, NULL, appmgrUg_pkg_moveto_worker, ad);
	if (ret) {
		SETTING_TRACE_ERROR("phread_create() Fail(%d)", ret);
		appmgrUg_fail_popup(MGRAPP_STR_MOVE_FAIL, ad);
	}
}

static void appmgrUg_pkg_append_toolbar(Evas_Object *toolbar,
		SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	int ret, mmc;
	char *btn_str;
	appmgr_pkginfo *info;
	Elm_Object_Item *tool_it;
	const char *mmc_key = VCONFKEY_SYSMAN_MMC_STATUS;

	ret_if(NULL == ad->pkginfo);

	info = ad->pkginfo;

	if (NULL == toolbar) {
		elm_object_item_del(
				elm_toolbar_first_item_get(ad->pkg_toolbar));
		elm_object_item_del(elm_toolbar_last_item_get(ad->pkg_toolbar));
		toolbar = ad->pkg_toolbar;
	}

	/* Move to */
	mmc = VCONFKEY_SYSMAN_MMC_REMOVED;
	ret = vconf_get_int(mmc_key, &mmc);
	warn_if(ret, "vconf_get_int(%s) Fail", mmc_key);

	if (PMINFO_INTERNAL_STORAGE == info->storage)
		btn_str = MGRAPP_STR_MOVE_TO_SD;
	else
		btn_str = MGRAPP_STR_MOVE_TO_PHONE;
	tool_it = elm_toolbar_item_append(toolbar, NULL, btn_str,
			appmgrUg_pkg_moveto_cb, ad);
	elm_object_item_domain_text_translatable_set(tool_it, SETTING_PACKAGE,
			EINA_TRUE);
	if (VCONFKEY_SYSMAN_MMC_MOUNTED != mmc || !info->is_movable
			|| info->is_preload)
		elm_object_item_disabled_set(tool_it, EINA_TRUE);

	/* Uninstall */
	if (info->is_preload && info->is_updated)
		btn_str = MGRAPP_STR_UNINSTALL_UPDATES;
	else
		btn_str = MGRAPP_STR_UNINSTALL;
	tool_it = elm_toolbar_item_append(toolbar, NULL, btn_str,
			appmgrUg_pkg_uninstall_click, ad);
	elm_object_item_domain_text_translatable_set(tool_it, SETTING_PACKAGE,
			EINA_TRUE);

	if (!info->removable)
		elm_object_item_disabled_set(tool_it, EINA_TRUE);
}

static inline Evas_Object *appmgrUg_pkg_create_toolbar(Evas_Object *parent,
		SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *toolbar;

	retv_if(NULL == ad, NULL);

	toolbar = elm_toolbar_add(parent);

	elm_object_style_set(toolbar, "default");
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_NONE);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);

	appmgrUg_pkg_append_toolbar(toolbar, ad);

	evas_object_show(toolbar);

	return toolbar;
}

static void appmgrUg_pkg_append_genlist(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item;

	/*appmgrUg_append_separator(ad->gl_pkg, ad); */

	/* Title (pkg name, version S) */
	item = elm_genlist_item_append(ad->gl_pkg, &ad->itc_info_title, ad,
			NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	item = elm_genlist_item_append(ad->gl_pkg, &ad->itc_2button1, ad, NULL,
			ELM_GENLIST_ITEM_NONE, NULL, NULL);
	/*elm_genlist_item_select_mode_set(item,
	 * ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY); */

	appmgrUg_pkg_append_storage(ad);

	item = elm_genlist_item_append(ad->gl_pkg, &ad->itc_2button2, ad, NULL,
			ELM_GENLIST_ITEM_NONE, NULL, NULL);
	/*elm_genlist_item_select_mode_set(item,
	 * ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY); */

	appmgrUg_pkg_append_cache(ad);

	appmgrUg_pkg_append_default(ad);
#if 0
	appmgrUg_pkg_append_website(ad);
#endif

	appmgrUg_pkg_append_privilege(ad);

	/*appmgrUg_append_separator(ad->gl_pkg, ad); */
}

/**
 * [UI] Naviframe : "Application info"
 */
static int appmgrUg_pkg_create(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret;
	Elm_Object_Item *navi_it;
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	ret = appmgrUg_pkg_get_info(ad);
	if (ret) {
		SETTING_TRACE_ERROR("appmgrUg_pkg_get_info() Fail", ret);
		appmgrUg_fail_popup(MGRAPP_STR_ERROR, ad);
		return -1;
	}

	navi_it = setting_push_layout_navi_bar_genlist(ad->lo_parent, ad->win,
			MGRAPP_STR_APPLICATION_INFO, NULL, NULL,
			(setting_call_back_func)appmgrUg_pkg_back_cb,
			NULL, ad, &ad->gl_pkg, ad->navi);
	elm_genlist_mode_set(ad->gl_pkg, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(ad->gl_pkg, EINA_FALSE);
	elm_naviframe_item_pop_cb_set(navi_it, appmgrUg_pkg_back_cb, ad);

	appmgrUg_pkg_append_genlist(ad);

	if (ad->support_taskmanager && ad->pc_main == NULL) {
		SETTING_TRACE("Load pkginfo view");
		appmgrUg_pkgmgr_subscribe(ad);
	}

	setting_view_appmgr_pkginfo.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_pkg_destroy(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (ad->pkginfo_getsize_timer) {
		ecore_timer_del(ad->pkginfo_getsize_timer);
		ad->pkginfo_getsize_timer = NULL;
	}

	if (ad->support_taskmanager && ad->pc_main != NULL) {

		SETTING_TRACE("Load pkginfo view");
		pkgmgr_client_free(ad->pc_main);
		ad->pc_main = NULL;
	}

	appmgrUg_pkg_free_pkginfo(ad->pkginfo);
	ad->pkginfo = NULL;
	ad->info_run = NULL;
	ad->runinfos = NULL;

	FREE(ad->sel_pkgid);
	FREE(ad->sel_label);
	FREE(ad->sel_icon);

	setting_view_appmgr_pkginfo.is_create = 0;
	elm_naviframe_item_pop(ad->navi);

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_pkg_update(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret;
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*record old size info, need not re-get size info when update */
	if (ad->pkginfo) {
		if (ad->pkginfo->sz_total >= 0)
			ad->sel_total = ad->pkginfo->sz_total;
		if (ad->pkginfo->sz_data >= 0)
			ad->sel_data_size = ad->pkginfo->sz_data;
	}

	elm_genlist_clear(ad->gl_pkg);

	appmgr_pkginfo *old_pkginfo = ad->pkginfo;
	ad->pkginfo = NULL;

	ret = appmgrUg_pkg_get_info(ad);
	if (ret) {
		SETTING_TRACE_ERROR("appmgrUg_pkg_get_info() Fail", ret);
		elm_naviframe_item_pop(ad->navi);
		return -1;
	}

	/*appmgrUg_pkg_append_toolbar(NULL, ad); */
	appmgrUg_pkg_append_genlist(ad);

	appmgrUg_pkg_free_pkginfo(old_pkginfo);

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_pkg_cleanup(void *data)
{
	return appmgrUg_pkg_destroy(data);
}

void appmgrUg_pkginfo_init(SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->pkginfo_view);

	setting_view_appmgr_pkginfo.create = appmgrUg_pkg_create;
	setting_view_appmgr_pkginfo.destroy = appmgrUg_pkg_destroy;
	setting_view_appmgr_pkginfo.update = appmgrUg_pkg_update;
	setting_view_appmgr_pkginfo.cleanup = appmgrUg_pkg_cleanup;

	ad->pkginfo_view = &setting_view_appmgr_pkginfo;
}
