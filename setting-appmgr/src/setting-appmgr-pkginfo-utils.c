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
#include <privilege_info.h>
#include <app2ext_interface.h>

#include <pkgmgr-info.h>
#include <package_manager.h>

#include "setting-appmgr-utils.h"
#include "setting-appmgr-pkginfo-utils.h"

static int appmgrUg_pkg_get_privileges_help(const char *privilege,
                                            void *user_data)
{
	int ret;
	gchar *desc;
	appmgr_pkginfo *info;
	SettingAppMgrUG *ad = user_data;
	char *name = NULL, *help = NULL;

	retv_if(NULL == user_data, -1);
	retv_if(NULL == ad->pkginfo, -1);
	retv_if(NULL == privilege, 0);

	info = ad->pkginfo;

	ret = privilege_info_get_privilege_display_name(privilege, &name);
	if (PRVMGR_ERR_NONE != ret) {
		SETTING_TRACE_ERROR("privilege_info_get_privilege_display_name() Fail(%d)", ret);
		return 0;
	}

	ret = privilege_info_get_privilege_description(privilege, &help);
	if (PRVMGR_ERR_NONE != ret) {
		SETTING_TRACE_ERROR("privilege_info_get_privilege_description() Fail(%d)", ret);
		free(name);
		return 0;
	}

	desc = g_strconcat("<font_size=28><B>", name, "</B></font_size><br>",
	                   help, "<br>", NULL);
	free(help);
	free(name);

	appmgrUg_append_separator(info->gl_prv, ad);

	setting_create_Gendial_field_titleItem(info->gl_prv, &ad->itc_multiline, desc, NULL);

	g_free(desc);
	return 0;
}

static void appmgrUg_pkg_privilege_info_view(void *data, Evas_Object *obj,
                                             void *event_info)
{
	char *grp = data;
	SettingAppMgrUG *ad;
	appmgr_pkginfo *info;
	char *grp_name = NULL;
	Setting_GenGroupItem_Data *d_item;
	Elm_Object_Item *item = event_info;

	ret_if(data == NULL);
	ret_if(event_info == NULL);

	elm_genlist_item_selected_set(item, EINA_FALSE);

	d_item = elm_object_item_data_get(item);
	ret_if(NULL == d_item);

	ad = d_item->userdata;
	info = ad->pkginfo;

	privilege_info_get_privilege_group_display_name(grp, &grp_name);

	setting_push_layout_navi_bar_genlist(ad->lo_parent, ad->win, grp_name,
	                                     NULL, NULL, NULL, NULL, NULL, &info->gl_prv, ad->navi);
	elm_genlist_mode_set(info->gl_prv, ELM_LIST_COMPRESS);

	privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group(info->pkgid, grp,
	                                                                   appmgrUg_pkg_get_privileges_help, ad);
}

static int appmgrUg_pkg_get_privileges_iter(const char *privilege, void *user_data)
{
	int ret;
	gchar **str = user_data;
	char *old_str, *name = NULL;

	retv_if(NULL == user_data, -1);
	retv_if(NULL == privilege, 0);

	ret = privilege_info_get_privilege_display_name(privilege, &name);
	if (PRVMGR_ERR_NONE != ret) {
		SETTING_TRACE_ERROR("privilege_info_get_privilege_display_name() Fail(%d)", ret);
		return 0;
	}

	old_str = *str;
	*str = g_strconcat(*str, name, "<br>", NULL);
	g_free(old_str);
	free(name);

	return 0;
}

int appmgrUg_pkg_get_privileges_grp_iter(const char *grp, void *user_data)
{
	int ret;
	appmgr_pkginfo *info;
	char *grp_name = NULL;
	gchar *prv_desc;
	SettingAppMgrUG *ad = user_data;

	retv_if(NULL == user_data, -1);
	retv_if(NULL == ad->pkginfo, -1);

	info = ad->pkginfo;

	ret = privilege_info_get_privilege_group_display_name(grp, &grp_name);
	if (PRVMGR_ERR_NONE != ret) {
		SETTING_TRACE_ERROR(
		    "privilege_info_get_privilege_group_display_name() Fail(%d)", ret);
		return 0;
	}

	prv_desc = g_strdup("");
	ret = privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group(info->pkgid,
	                                                                         grp, appmgrUg_pkg_get_privileges_iter, &prv_desc);
	if (PRVMGR_ERR_NONE != ret || '\0' == *prv_desc) {
		SETTING_TRACE_ERROR(
		    "privilege_info_foreach_privilege_list_by_pkgid_and_privilege_group()"
		    " Fail(%d), prv_desc(%p)", ret, prv_desc);
		G_FREE(prv_desc);
		FREE(grp_name);
		return 0;
	}


	/* remove the last <br> */
	char *p = strrchr(prv_desc, '<');
	if (p)
		*p = '\0';

	info->last_prv = setting_create_Gendial_field_def(ad->gl_pkg, &ad->itc_multiline_2txt,
	                                                  appmgrUg_pkg_privilege_info_view, (void *)grp, SWALLOW_Type_INVALID, NULL,
	                                                  NULL, 0, grp_name, prv_desc, NULL);
	if (info->last_prv) {
		info->last_prv->userdata = ad;

		if (NULL == info->first_prv) {
			info->first_prv = info->last_prv;
			info->first_prv->group_style = SETTING_GROUP_STYLE_TOP;
		} else {
			info->last_prv->group_style = SETTING_GROUP_STYLE_CENTER;
		}
	}

	FREE(grp_name);
	G_FREE(prv_desc);

	return 0;
}

void appmgrUg_pkg_free_pkginfo(appmgr_pkginfo *info)
{
	ret_if(NULL == info);

	free(info->pkgid);
	free(info->pkg_label);
	free(info->icon_path);
	free(info->pkg_type);
	free(info->pkg_version);
	free(info->main_appid);
	g_list_free_full(info->appids, free);

	if (info->pc_size) {
		pkgmgr_client_free(info->pc_size);
		info->pc_size = NULL;
	}
	if (info->pc_request) {
		pkgmgr_client_free(info->pc_request);
		info->pc_request = NULL;
	}

	free(info);
}

static int appmgrUg_pkg_get_appinfo_iter(pkgmgrinfo_appinfo_h handle, void *data)
{
	int ret;
	char *appid, *label;
	appmgr_pkginfo *pkginfo;
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, -1);

	pkginfo = ad->pkginfo;

	ret = pkgmgrinfo_appinfo_get_appid(handle, &appid);
	if (PMINFO_R_OK != ret || NULL == appid) {
		SETTING_TRACE_ERROR("pkgmgrinfo_appinfo_get_appid() Fail(%d)", ret);
		return 0;
	}

	ret = pkgmgrinfo_appinfo_get_label(handle, &label);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_get_label() Fail(%d)", ret);

	if (NULL == pkginfo->icon_path && safeStrCmp(appid, pkginfo->main_appid))
		pkginfo->icon_path = appmgrUg_get_defualt_icon(handle);

	pkginfo->appids = g_list_append(pkginfo->appids, strdup(appid));

	return 0;
}

static inline void appmgrUg_pkg_update_size(appmgr_pkginfo *info)
{
	SETTING_TRACE_BEGIN;
	char desc[APPMGRUG_MAX_STR_LEN] = {0};

	appmgrUg_size_to_str(info->sz_total, desc, sizeof(desc));
	g_free(info->total_size->sub_desc);
	info->total_size->sub_desc = (char *)g_strdup(desc);
	elm_genlist_item_fields_update(info->total_size->item, "elm.text.sub.left.bottom",
	                               ELM_GENLIST_ITEM_FIELD_TEXT);

	appmgrUg_size_to_str(info->sz_data, desc, sizeof(desc));
	g_free(info->data_size->sub_desc);
	info->data_size->sub_desc = (char *)g_strdup(desc);
	elm_genlist_item_fields_update(info->data_size->item, "elm.text.sub.left.bottom",
	                               ELM_GENLIST_ITEM_FIELD_TEXT);

	appmgrUg_size_to_str(info->sz_total - info->sz_data, desc, sizeof(desc));
	g_free(info->app_size->sub_desc);
	info->app_size->sub_desc = (char *)g_strdup(desc);
	elm_genlist_item_fields_update(info->app_size->item, "elm.text.sub.left.bottom",
	                               ELM_GENLIST_ITEM_FIELD_TEXT);
}

void appmgrUg_pkg_update_cache_size(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingAppMgrUG *ad = data;
	appmgr_pkginfo *info = ad->pkginfo;
	retm_if(info == NULL, "info is NULL");

	char desc[APPMGRUG_MAX_STR_LEN] = {0};

	appmgrUg_size_to_str(info->sz_cache, desc, sizeof(desc));

	if (ad->cache_size && ad->cache_size->item) {
		ad->cache_size->sub_desc = (char *)g_strdup(desc);
		elm_genlist_item_fields_update(ad->cache_size->item, "elm.text.sub.left.bottom",
		                               ELM_GENLIST_ITEM_FIELD_TEXT);
	}

	if (ad->clear_cache && ad->clear_cache->item) {
		elm_genlist_item_update(ad->clear_cache->item);
	}
}

static int _appmgrUg_pkg_get_pkg_size(int req_id, const char *pkg_type,
                                      const char *pkgid, const char *key, const char *val, const void *pmsg, void *data)
{
	appmgr_pkginfo *info = data;

	retv_if(NULL == data, 0);
	retv_if(NULL == key, 0);
	retv_if(NULL == val, 0);

	info->sz_total = atoi(val);
	info->sz_data = atoi(key);

	appmgrUg_pkg_update_size(info);

	return 0;
}

static inline void appmgrUg_pkg_get_pkg_size(appmgr_pkginfo *info)
{
	int ret;

	ret_if(NULL == info);

	if (info->pc_size)
		pkgmgr_client_free(info->pc_size);

	info->pc_size = pkgmgr_client_new(PC_REQUEST);
	if (NULL == info->pc_size) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Fail");
		return;
	}

	ret = pkgmgr_client_get_size(info->pc_size, info->pkgid, PM_GET_TOTAL_SIZE,
	                             _appmgrUg_pkg_get_pkg_size, info);
	warn_if(ret, "pkgmgr_client_get_size(%s) Fail(%d)", info->pkgid, ret);
}

static void _get_cache_cb(const char *package_id, const pkg_size_info_t *size_info, void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "data == NULL");
	SettingAppMgrUG *ad = data;
	appmgr_pkginfo *info = ad->pkginfo;
	retm_if(info == NULL, "info == NULL");

	info->sz_cache = (int)size_info->cache_size;
	SETTING_TRACE("cache size: %lld", size_info->cache_size);
	appmgrUg_pkg_update_cache_size(ad);
}

static Eina_Bool __get_pkg_size_on_time(void *data)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingAppMgrUG *ad = data;
	appmgr_pkginfo *info = ad->pkginfo;
	retv_if(info == NULL, 0);

	/*get cache size */
	if (info->is_disable) {
		/*if pkg is disabled, size = 0 */
		info->sz_cache = 0;
		appmgrUg_pkg_update_cache_size(ad);
	} else {
		int ret = package_manager_get_package_size_info(info->pkgid,
		                                                _get_cache_cb, ad);
		if (ret != 0) {
			SETTING_TRACE_ERROR("failed to invoke ret = %d", ret);
		}
	}

	/*get pkg total & data size */
	if (ad->sel_total < 0 || ad->sel_data_size < 0) {
		appmgrUg_pkg_get_pkg_size(info);
	} else {
		info->sz_total = ad->sel_total;
		info->sz_data = ad->sel_data_size;
		appmgrUg_pkg_update_size(info);
	}

	ad->pkginfo_getsize_timer = NULL;
	return EINA_FALSE;
}

static int _appmgrUg_pkg_get_info(pkgmgrinfo_pkginfo_h handle, void *data)
{
	retv_if(data == NULL, 0);
	SettingAppMgrUG *ad = data;
	appmgr_pkginfo *info = ad->pkginfo;
	retv_if(info == NULL, 0);

	int ret = 0;
	char *value = NULL;

	/*get pkgsize info in timer to avoid block UI */
	info->sz_cache = -1;
	info->sz_total = -1;
	info->sz_data = -1;
	if (ad->pkginfo_getsize_timer) {
		ecore_timer_del(ad->pkginfo_getsize_timer);
		ad->pkginfo_getsize_timer = NULL;
	}
	ad->pkginfo_getsize_timer = ecore_timer_add(0.6,
	                                            (Ecore_Task_Cb) __get_pkg_size_on_time, ad);

	/*get other info */
	if (ad->sel_icon) {
		info->icon_path = strdup(ad->sel_icon);
	} else {
		value = NULL;
		ret = pkgmgrinfo_pkginfo_get_icon(handle, &value);
		warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_icon() Fail(%d)", ret);
		info->icon_path = appmgrUg_check_icon(value);
		ad->sel_icon = SAFE_STRDUP(info->icon_path);
	}

	SETTING_TRACE("icon_path : %s", info->icon_path);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_mainappid(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_mainappid() Fail(%d)", ret);
	info->main_appid = SAFE_STRDUP(value);

	ret = pkgmgrinfo_appinfo_get_list(handle, PMINFO_ALL_APP,
	                                  appmgrUg_pkg_get_appinfo_iter, ad);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_get_list() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_is_movable(handle, &info->is_movable);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_is_movable() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_is_preload(handle, &info->is_preload);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_is_preload() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_is_update(handle, &info->is_updated);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_is_update() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_get_installed_storage(handle, &info->storage);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_get_installed_storage() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_is_removable(handle, &info->removable);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_is_removable() Fail(%d)", ret);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_type(handle, &value);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_get_type() Fail(%d)", ret);
	info->pkg_type = SAFE_STRDUP(value);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_version(handle, &value);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_get_version() Fail(%d)", ret);
	info->pkg_version = SAFE_STRDUP(value);

	if (ad->sel_label) {
		info->pkg_label = strdup(ad->sel_label);
	} else {
		value = NULL;
		ret = pkgmgrinfo_pkginfo_get_label(handle, &value);
		warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_label() Fail(%d)", ret);
		info->pkg_label = SAFE_STRDUP(value);
		ad->sel_label = SAFE_STRDUP(info->pkg_label);
	}

	ret = pkgmgrinfo_pkginfo_is_support_disable(handle, &info->is_support_disable);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_is_support_disable() Fail(%d)", ret);

	return 0;
}

int appmgrUg_pkg_get_info(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;
	int ret;
	appmgr_pkginfo *info;

	retv_if(NULL == ad, -1);
	retv_if(NULL == ad->sel_pkgid, -1);

	info = calloc(1, sizeof(appmgr_pkginfo));
	if (NULL == info) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return -1;
	}
	ad->pkginfo = info;
	info->is_disable = ad->sel_is_disabled;
	info->pkgid = strdup(ad->sel_pkgid);

	if (info->is_disable) {
		SETTING_TRACE("Get disabled app[%s] info", info->pkgid);
		pkgmgrinfo_pkginfo_filter_h filter_handle;
		int ret = pkgmgrinfo_pkginfo_filter_create(&filter_handle);

		warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_filter_create() Fail(%d)", ret);

		ret = pkgmgrinfo_pkginfo_filter_add_bool(filter_handle,
		                                         PMINFO_PKGINFO_PROP_PACKAGE_DISABLE, EINA_TRUE);

		warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_filter_add_bool() Fail(%d)", ret);

		ret = pkgmgrinfo_pkginfo_filter_add_string(filter_handle,
		                                           PMINFO_PKGINFO_PROP_PACKAGE_ID, info->pkgid);

		warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_filter_add_string() Fail(%d)", ret);

		ret = pkgmgrinfo_pkginfo_filter_foreach_pkginfo(filter_handle,
		                                                _appmgrUg_pkg_get_info, ad);
		warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_pkginfo_filter_foreach_pkginfo() Fail(%d)", ret);

		pkgmgrinfo_pkginfo_filter_destroy(filter_handle);

	} else {
		pkgmgrinfo_pkginfo_h handle;
		ret = pkgmgrinfo_pkginfo_get_pkginfo(info->pkgid, &handle);
		if (PMINFO_R_OK != ret) {
			SETTING_TRACE_ERROR("pkgmgrinfo_pkginfo_get_pkginfo() Fail(%d)", ret);
			appmgrUg_pkg_free_pkginfo(info);
			ad->pkginfo = NULL;
			return -1;
		}
		_appmgrUg_pkg_get_info(handle, ad);
		pkgmgrinfo_pkginfo_destroy_pkginfo(handle);
	}

	return 0;
}

void appmgrUg_pkg_moveto_worker_finish(SettingAppMgrUG *ad)
{
	int ret;
	void *ret_val;

	ret = pthread_join(ad->pkginfo->tid, &ret_val);
	if (ret) {
		SETTING_TRACE_ERROR("pthread_join() Fail(%d)", ret);
		ret = -1;
	} else {
		ret = (int)ret_val;
	}

	switch (ret) {
		case PKGMGR_R_OK:
			if (ad->popup)
				evas_object_del(ad->popup);

			ad->popup = setting_create_popup_with_btn(ad, ad->win, NULL,
			                                          MGRAPP_STR_MOVE_COMPLETED, appmgrUg_popup_del, 0, 1, MGRAPP_STR_OK);
			setting_view_update(ad->pkginfo_view, ad);
			break;
		case APP2EXT_ERROR_MMC_STATUS:
		case APP2EXT_ERROR_MMC_INSUFFICIENT_MEMORY:
			SETTING_TRACE_ERROR("moveto() SD_FULL Fail(%d)", ret);
			appmgrUg_fail_popup(MGRAPP_STR_SD_FULL, ad);
			break;
		case APP2EXT_ERROR_INVALID_CASE:
			if (PMINFO_INTERNAL_STORAGE == ad->pkginfo->storage) {
				SETTING_TRACE_ERROR("moveto() NO_MEM Fail(%d)", ret);
				appmgrUg_fail_popup(MGRAPP_STR_NO_MEM, ad);
				break;
			}
		default:
			SETTING_TRACE_ERROR("moveto() Fail(%d)", ret);
			appmgrUg_fail_popup(MGRAPP_STR_MOVE_FAIL, ad);
			break;
	}

	ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
}

void *appmgrUg_pkg_moveto_worker(void *data)
{
	int ret;
	pkgmgr_client *pc;
	appmgr_pkginfo *info;
	SettingAppMgrUG *ad = data;
	pkgmgr_move_type move_type;

	retv_if(NULL == data, NULL);
	retv_if(NULL == ad->pkginfo, NULL);

	info = ad->pkginfo;

	if (PMINFO_INTERNAL_STORAGE == info->storage)
		move_type = PM_MOVE_TO_SDCARD;
	else
		move_type = PM_MOVE_TO_INTERNAL;

	pc = pkgmgr_client_new(PC_REQUEST);
	ret = pkgmgr_client_request_service(PM_REQUEST_MOVE, move_type, pc, info->pkg_type,
	                                    info->pkgid, NULL, NULL, NULL);

	pkgmgr_client_free(pc);

	pthread_exit((void *)ret);
}

static void appmgrUg_pkg_ug_layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	Evas_Object *base = NULL;

	ret_if(priv == NULL);

	base = ug_get_layout(ug);
	if (base == NULL) {
		SETTING_TRACE_ERROR("ug_get_layout() Fail");
		return;
	}

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_show(base);
			break;
		default:
			/* do nothing */
			break;
	}
}

static void appmgrUg_pkg_ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	SettingAppMgrUG *ad = priv;

	ret_if(priv == NULL);

	if (ug)
		setting_ug_destroy(ug);

	elm_object_tree_focus_allow_set(ad->lo_main, EINA_TRUE);
}

void appmgrUg_pkg_webapp_ug(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	app_control_h svc;
	ui_gadget_h ug;
	struct ug_cbs cbs;
	SettingAppMgrUG *ad = data;
	Elm_Object_Item *item = event_info;

	ret_if(NULL == data);
	ret_if(NULL == event_info);

	elm_genlist_item_selected_set(item, EINA_FALSE);

	if (app_control_create(&svc))
		return;

	app_control_add_extra_data(svc, "pkg_name", ad->pkginfo->pkgid);

	memset(&cbs, 0, sizeof(struct ug_cbs));
	cbs.layout_cb = appmgrUg_pkg_ug_layout_cb;
	cbs.result_cb = NULL;
	cbs.destroy_cb = appmgrUg_pkg_ug_destroy_cb;
	cbs.priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->lo_main, EINA_FALSE);
	ug = setting_ug_create(ad->ug, "webapp-detail-efl", UG_MODE_FULLVIEW, svc, &cbs);
	warn_if(NULL == ug, "setting_ug_create() Fail");

	app_control_destroy(svc);
	SETTING_TRACE_END;
}

void appmgrUg_pkg_clear_default(void *data, Evas_Object *obj, void *event_info)
{
#if 0
	GList *cur;
	appmgr_pkginfo *info;
	SettingAppMgrUG *ad = data;
	Elm_Object_Item *next;
	Elm_Object_Item *item = event_info;

	ret_if(NULL == data);
	ret_if(NULL == ad->pkginfo);
	ret_if(NULL == event_info);

	info = ad->pkginfo;

	elm_genlist_item_selected_set(item, EINA_FALSE);

	cur = info->appids;
	while (cur) {
		int ret;
		char *appid = cur->data;

		cur = cur->next;

		if (NULL == appid)
			continue;

		ret = appsvc_unset_defapp(appid);
		warn_if(APPSVC_RET_OK != ret, "appsvc_unset_defapp() Fail(%d)", ret);
	}

	/* clear default separator */
	next = elm_genlist_item_next_get(info->def_sep);
	elm_object_item_del(info->def_sep);
	info->def_sep = NULL;

	/* clear default title */
	item = next;
	next = elm_genlist_item_next_get(next);
	elm_object_item_del(item);

	/* clear default button */
	item = next;
	next = elm_genlist_item_next_get(next);
	elm_object_item_del(item);

	/* clear default notice */
	elm_object_item_del(next);

	setting_view_update(ad->main_view, ad);
#endif
}

char *appmgrUg_pkg_size_gl_label_get(void *data, Evas_Object *obj,
                                     const char *part)
{
	char *label = NULL;
	Evas_Object *ao = NULL;
	char desc[APPMGRUG_MAX_STR_LEN] = {0};
	Setting_GenGroupItem_Data *item_data = data;

	retv_if(data == NULL, NULL);

	if (0 == strcmp(part, "elm.text.sub.left.bottom"))
		label = SAFE_STRDUP(_(item_data->sub_desc));
	else if (0 == strcmp(part, "elm.text.main.left.top"))
		label = SAFE_STRDUP(_(item_data->keyStr));

	if (item_data->keyStr && item_data->sub_desc)
		snprintf(desc, sizeof(desc), "%s,%s", _(item_data->keyStr), _(item_data->sub_desc));

	ao = elm_object_item_access_object_get(item_data->item);
	setting_set_tts_info(ao, desc, "", "", "");

	return label;
}
