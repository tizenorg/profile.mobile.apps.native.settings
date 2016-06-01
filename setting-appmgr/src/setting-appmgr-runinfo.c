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
#include <app_manager.h>

#include "setting-appmgr-utils.h"
#include "setting-appmgr-runinfo.h"

#include <app_manager_extension.h>

static setting_view setting_view_appmgr_runinfo;

static Eina_Bool appmgrUg_run_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, EINA_FALSE);

	setting_view_destroy(ad->runinfo_view, ad);

	return EINA_TRUE;
}

void appmgrUg_free_runinfo(gpointer data)
{
	appmgr_runinfo *info = data;

	ret_if(NULL == data);

	free(info->appid);
	free(info->label);
	free(info);
}

static void appmgrUg_del_running_list(gpointer data, gpointer user_data)
{
	appmgr_listinfo *pkg_info = data;

	ret_if(NULL == data);

	g_list_free_full(pkg_info->runinfos, appmgrUg_free_runinfo);
	pkg_info->runinfos = NULL;
}

static inline int appmgrUg_run_get_app_info(const char *appid,
		appmgr_runinfo *out)
{
	int ret;
	char *value;
	pkgmgrinfo_appinfo_h handle;

	ret = pkgmgrinfo_appinfo_get_appinfo(appid, &handle);
	if (PMINFO_R_OK != ret) {
		SETTING_TRACE_ERROR("pkgmgrinfo_appinfo_get_appinfo() Fail(%d)",
				ret);
		return -1;
	}

	ret = pkgmgrinfo_appinfo_get_pkgid(handle, &value);
	if (PMINFO_R_OK != ret) {
		SETTING_TRACE_ERROR("pkgmgrinfo_appinfo_get_pkgid() Fail(%d)",
				ret);
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return -1;
	}
	out->appid = SAFE_STRDUP(value);

	ret = pkgmgrinfo_appinfo_get_label(handle, &value);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_get_label() Fail(%d)",
			ret);
	out->label = SAFE_STRDUP(value);

	ret = pkgmgrinfo_appinfo_is_taskmanage(handle, &out->can_kill);
	warn_if(PMINFO_R_OK != ret,
			"pkgmgrinfo_appinfo_is_taskmanage() Fail(%d)", ret);

	pkgmgrinfo_appinfo_destroy_appinfo(handle);
	return 0;
}

static int appmgrUg_get_running_list_iter(const aul_app_info *info, void *data)
{
	int ret;
	GList *cur;
	char *label = NULL;
	char *pkgid = NULL;
	SettingAppMgrUG *ad = data;
	appmgr_runinfo tmp_info = { NULL, NULL, false };

	retv_if(NULL == data, 0);
	retv_if(NULL == info, 0);
	retv_if(NULL == info->appid, 0);

	ret = appmgrUg_run_get_app_info(info->appid, &tmp_info);
	if (ret) {
		SETTING_TRACE_ERROR("appmgrUg_run_get_app_info(%s) Fail",
				info->appid);
		return 0;
	}
	pkgid = tmp_info.appid; /*temporary using */

	cur = g_list_find_custom(ad->pkg_list, pkgid, appmgrUg_compare_pkgname);
	if (cur) {
		appmgr_listinfo *listinfo = cur->data;
		appmgr_runinfo *runinfo = calloc(1, sizeof(appmgr_runinfo));

		if (runinfo) {
			runinfo->appid = strdup(info->appid);
			runinfo->label = strdup(tmp_info.label);
			runinfo->can_kill = tmp_info.can_kill;
			listinfo->runinfos = g_list_append(listinfo->runinfos,
					runinfo);
		} else {
			SETTING_TRACE_ERROR("calloc() Fail");
			free(label);
		}
	}

	free(pkgid);
	free(tmp_info.label);
	return 0;
}

static void appmgrUg_run_stop_iter(gpointer data, gpointer user_data)
{
	int ret;
	appmgr_runinfo *info = data;
	app_context_h app_context;

	ret_if(NULL == data);

	ret = app_manager_get_app_context(info->appid, &app_context);
	if (APP_MANAGER_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("app_manager_get_app_context(%s) Fail(%d)",
				info->appid, ret);
		return;
	}

#if 1
	SETTING_TRACE_ERROR(
			"app_manager_terminate_app is not External interface. "\
			"need check in more detail.");
#else
	ret = app_manager_terminate_app(app_context);
	if (APP_MANAGER_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("app_manager_terminate_app() Fail(%d)",
				ret);
		return;
	}
#endif

	app_context_destroy(app_context);
}

void appmgrUg_run_stop_click(void *data, Evas_Object *obj, void *event_info)
{
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);

	g_list_foreach(ad->runinfos, appmgrUg_run_stop_iter, NULL);
	/*remove runinfos in running listinfo */
	if (ad->info_run && ad->info_run->runinfos) {
		g_list_free_full(ad->info_run->runinfos, appmgrUg_free_runinfo);
		ad->info_run->runinfos = NULL;
		ad->info_run = NULL;
	}
	if (APPMGRUG_TAB_RUNNING == ad->tabtype) {
		elm_naviframe_item_pop(ad->navi);
	} else if (APPMGRUG_TAB_DISABLED == ad->tabtype) {
		elm_object_disabled_set(obj, EINA_TRUE);
	}

	setting_view_update(ad->main_view, ad);
}

static void appmgrUg_run_append_run_apps(SettingAppMgrUG *ad)
{
	int cnt = 0;
	GList *cur;
	Setting_GenGroupItem_Data *d_item = NULL;

	ret_if(NULL == ad);

	cur = ad->runinfos;
	while (cur) {
		appmgr_runinfo *info;

		info = cur->data;
		cur = cur->next;

		if (NULL == info) {
			SETTING_TRACE_ERROR("info is a null");
			continue;
		}

		d_item = setting_create_Gendial_field_def(ad->gl_run,
				&ad->itc_1txt_1ic_2, NULL,
				NULL, SWALLOW_Type_1ICON_SMALL_ICON,
				(char *)ad->sel_icon, NULL, 0, info->label,
				NULL, NULL);
		if (NULL == d_item) {
			SETTING_TRACE_ERROR(
					"setting_create_Gendial_field_def() Fail");
			continue;
		}
		elm_genlist_item_select_mode_set(d_item->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

		cnt++;
	}
	retm_if(NULL == d_item, "No item");
}

static int appmgrUg_run_create(void *data)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item;
	SettingAppMgrUG *ad = data;
	char msg[1024];

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	Elm_Object_Item *navi_item = setting_push_layout_navi_bar_genlist(
			ad->lo_parent, ad->win,
			MGRAPP_STR_ACTIVE_APP, NULL, NULL,
			(setting_call_back_func)appmgrUg_run_back_cb,
			NULL, ad, &ad->gl_run, ad->navi);
	elm_naviframe_item_pop_cb_set(navi_item, appmgrUg_run_back_cb, ad);

	elm_genlist_mode_set(ad->gl_run, ELM_LIST_COMPRESS);

	/*appmgrUg_append_separator(ad->gl_run, ad); */

	/* Title (pkg name, process N) */
	item = elm_genlist_item_append(ad->gl_run, &ad->itc_info_title, ad,
			NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/*appmgrUg_append_separator(ad->gl_run, ad); */

	/* notice */
	snprintf(msg, sizeof(msg), _(MGRAPP_STR_APP_STOP_MSG), ad->sel_label,
			ad->sel_label);
	setting_create_Gendial_field_titleItem(ad->gl_run, &ad->itc_multiline,
			msg, NULL);

	/* Stop button */
	elm_genlist_item_append(ad->gl_run, &ad->itc_1ic, ad, NULL,
			ELM_GENLIST_ITEM_NONE,
			NULL, NULL);

	/*appmgrUg_append_separator(ad->gl_run, ad); */

	/* group title(applications) */
	setting_create_Gendial_field_titleItem(ad->gl_run, &ad->itc_grp_title,
	MGRAPP_STR_APPS, NULL);

	appmgrUg_run_append_run_apps(ad);

	/*appmgrUg_append_separator(ad->gl_run, ad); */

	setting_view_appmgr_runinfo.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_run_destroy(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	setting_view_appmgr_runinfo.is_create = 0;
	elm_naviframe_item_pop(ad->navi);

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	ad->runinfos = NULL;

	FREE(ad->sel_pkgid);
	FREE(ad->sel_label);
	FREE(ad->sel_icon);

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_run_update(void *data)
{
	SETTING_TRACE_BEGIN;
	/*
	 SettingAppMgrUG *ad = data;

	 retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	 elm_naviframe_item_pop(ad->navi);
	 */

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_run_cleanup(void *data)
{
	return appmgrUg_run_destroy(data);
}

void appmgrUg_appinfo_init(SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->runinfo_view);

	setting_view_appmgr_runinfo.create = appmgrUg_run_create;
	setting_view_appmgr_runinfo.destroy = appmgrUg_run_destroy;
	setting_view_appmgr_runinfo.update = appmgrUg_run_update;
	setting_view_appmgr_runinfo.cleanup = appmgrUg_run_cleanup;

	ad->runinfo_view = &setting_view_appmgr_runinfo;
}

Evas_Object *appmgrUg_run_gl_stop_btn(void *data, Evas_Object *obj,
		const char *part)
{
	GList *cur;
	Evas_Object *btn;
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, NULL);

	if (0 != safeStrCmp(part, "elm.swallow.content"))
		return NULL;
	btn = setting_create_button(obj, MGRAPP_STR_STOP, NULL,
			appmgrUg_run_stop_click, ad);
	evas_object_size_hint_expand_set(btn, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	cur = ad->runinfos;
	while (cur) {
		appmgr_runinfo *info = cur->data;

		if (info && info->can_kill)
			break;
		cur = cur->next;
	}

	if (NULL == cur) {
		elm_object_disabled_set(btn, EINA_TRUE);
		elm_object_disabled_set(obj, EINA_TRUE);
	}

	return btn;
}

int appmgrUg_get_running_list(SettingAppMgrUG *ad)
{
	int ret;

	retv_if(NULL == ad, SETTING_RETURN_FAIL);

	g_list_foreach(ad->pkg_list, appmgrUg_del_running_list, NULL);

	ret = aul_app_get_running_app_info(appmgrUg_get_running_list_iter, ad);
	warn_if(AUL_R_OK != ret, "aul_app_get_running_app_info() Fail", ret);
	ad->is_runlist_ready = EINA_TRUE;
	return SETTING_RETURN_SUCCESS;
}
