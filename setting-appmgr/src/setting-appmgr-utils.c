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
/*#include <resourced.h> */
/*#include <data_usage.h> */
#include <app_manager.h>
#include <app_manager_extension.h>
#include <pkgmgr-info.h>
#include <package_manager.h>

#include "setting-appmgr-main.h"
#include "setting-appmgr-runinfo.h"
#include "setting-appmgr-pkginfo-utils.h"
#include "setting-appmgr-async-worker.h"
#include "setting-appmgr-utils.h"

extern void appmgrUg_pkg_uninstall_click(void *data, Evas_Object *obj,
		void *event_info);
extern void appmgrUg_pkg_clear_cache_click(void *data, Evas_Object *obj,
		void *event_info);
extern void appmgrUg_pkg_moveto_cb(void *data, Evas_Object *obj,
		void *event_info);
extern Evas_Object *appmgrUg_run_gl_stop_btn(void *data, Evas_Object *obj,
		const char *part);
extern void appmgrUg_run_stop_click(void *data, Evas_Object *obj,
		void *event_info);

static void appmgrUg_free_listinfo(gpointer data);

void _get_size_cb(const char *package_id, const package_size_info_h size_info,
		void *data)
{
	retm_if(data == NULL, "data == NULL");
	appmgr_listinfo *info = data;

	long long size = 0;
	package_size_info_get_data_size(size_info, &size);
	info->total_size = (int)size;

	package_size_info_get_app_size(size_info, &size);
	info->total_size += (int)size;

	info->valid_size = true;
	if (info->size_idler) {
		ecore_timer_del(info->size_idler);
		info->size_idler = NULL;
	}
	elm_genlist_item_fields_update(info->item, "elm.text.sub",
		ELM_GENLIST_ITEM_FIELD_TEXT);
}

Eina_Bool appmgrUg_get_app_size(void *data)
{
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	appmgr_listinfo *info = data;
	retv_if(info == NULL, 0);

	elm_genlist_item_fields_update(info->item, "elm.text.sub",
		ELM_GENLIST_ITEM_FIELD_TEXT);

	int ret = package_manager_get_package_size_info(info->pkgid,
		_get_size_cb, info);
	if (ret != 0) {
		SETTING_TRACE_ERROR("failed to invoke ret = %d", ret);
	}

	return EINA_FALSE;
}

void _free_GSList(GSList *list)
{
	g_slist_foreach(list, (GFunc) g_free, NULL);
	g_slist_free(list);
}

/*
 resourced_cb_ret _reset_all_restrict_apps_cb(const resourced_restriction_info
 *info, void *user_data) {
 GSList** appid_list = (GSList**)user_data;
 *appid_list = g_slist_prepend(*appid_list, g_strdup(info->app_id));
 return 1;
 }

 void _reset_restrict_cb(char *app_id){
 resourced_ret_c ret = remove_restriction_by_iftype(app_id,
 RESOURCED_IFACE_DATACALL);
 if(RESOURCED_ERROR_OK != ret){
 SETTING_TRACE_ERROR("reset restrict app: %s failed with %d", app_id, ret);
 }
 } */

void appmgrUg_size_to_str(int size, char *desc, int desc_size)
{
	double tmp_size = 0.0;
	const int KILOBYTE_VALUE = 1024;
	const int MEGABYTE_VALUE = 1048576;
	const int GIGABYTE_VALUE = 1073741824;

	if (size < MEGABYTE_VALUE) { /* size < 1MB: show x.xKB */
		tmp_size = (double) size / KILOBYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(MGRAPP_STR_KB));
	} else if (size < GIGABYTE_VALUE) { /* size < 1GB: show x.xMB */
		tmp_size = (double) size / MEGABYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(MGRAPP_STR_MB));
	} else { /* 1G <= size: show x.xGB */
		tmp_size = (double) size / GIGABYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(MGRAPP_STR_GB));
	}
}

void appmgrUg_popup_del(void *data, Evas_Object *obj, void *event_info)
{
	SettingAppMgrUG *ad = data;

	ret_if(data == NULL);

	evas_object_del(ad->popup);
	ad->popup = NULL;
}

void appmgrUg_fail_popup(char *str, SettingAppMgrUG *ad)
{
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	ad->popup = setting_create_popup(ad, ad->win, NULL, str,
			appmgrUg_popup_del, 0, FALSE, FALSE, 1, MGRAPP_STR_OK);
}

static void appmgrUg_update_listinfos(SettingAppMgrUG *ad)
{
	SETTING_TRACE_BEGIN;

	if (ad->list_worker)
		appmgrUg_stop_async_worker(ad->list_worker);
	ad->list_worker = appmgrUg_start_async_worker(appmgrUg_get_listinfos,
			appmgrUg_get_listinfos_cb, ad);
}

static int appmgrUg_pkgmgr_changed_cb(int req_id, const char *pkg_type,
		const char *pkgid, const char *key, const char *val,
		const void *pmsg, void *data)
{
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, -1);

	SETTING_TRACE_DEBUG("key:%s val:%s", key, val);
	SETTING_TRACE_DEBUG("pkg_type:%s, pkgid:%s, sel_pkgid:%s", pkg_type,
			pkgid, ad->sel_pkgid);

	/*
	 if (!safeStrCmp(key, "start") && safeStrCmp(val, "uninstall"))
	 if (!safeStrCmp(key, "start") && !safeStrCmp(val, "install"))
	 if (!safeStrCmp(key, "start") && !safeStrCmp(val, "move"))
	 if (!safeStrCmp(key, "start") && !safeStrCmp(val, "update"))
	 */

	if (!safeStrCmp(key, "error")) {
		SETTING_TRACE_ERROR("uninstall() Fail(key:%s val:%s)",
				key, val);
	} else if (!safeStrCmp(key, "end") && !safeStrCmp(val, "fail")) {
		if (0 == safeStrCmp(ad->sel_pkgid, pkgid)) {
			if (APPMGRUG_PKG_REQUEST_UNINSTALL == ad->pkg_request) {
				SETTING_TRACE_ERROR("uninstall() Fail");
				appmgrUg_fail_popup(MGRAPP_STR_UNINSTALL_FAILED,
						ad);
				ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
			} else if (APPMGRUG_PKG_REQUEST_MOVE
					== ad->pkg_request) {
				appmgrUg_pkg_moveto_worker_finish(ad);
			}
		}
	} else if (!safeStrCmp(key, "end") && !safeStrCmp(val, "ok")) {
		if (0 == safeStrCmp(ad->sel_pkgid, pkgid)) {
			if (APPMGRUG_PKG_REQUEST_UNINSTALL == ad->pkg_request) {
				if (APPMGRUG_TAB_NONE == ad->tabtype)
					ug_destroy_me(ad->ug);
				else
					elm_naviframe_item_pop(ad->navi);

				if (ad->popup)
					evas_object_del(ad->popup);
				ad->popup = setting_create_popup(ad, ad->win,
						NULL,
						MGRAPP_STR_UNINSTALL_COMPLETE,
						appmgrUg_popup_del, 0, FALSE,
						FALSE, 1, MGRAPP_STR_OK);

				ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
			} else if (APPMGRUG_PKG_REQUEST_MOVE
					== ad->pkg_request) {
				appmgrUg_pkg_moveto_worker_finish(ad);
			} else if (APPMGRUG_PKG_REQUEST_DISABLE
					== ad->pkg_request) {
				ad->sel_is_disabled = EINA_TRUE;
				ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
				if (ad->pkginfo_view->is_create)
					setting_view_update(ad->pkginfo_view,
							ad);
			} else if (APPMGRUG_PKG_REQUEST_ENABLE
					== ad->pkg_request) {
				ad->sel_is_disabled = EINA_FALSE;
				ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;
				if (ad->pkginfo_view->is_create)
					setting_view_update(ad->pkginfo_view,
							ad);
			} else {
				if (ad->pkginfo_view->is_create)
					setting_view_update(ad->pkginfo_view,
							ad);
			}
		}

		if (ad->main_view->is_create)
			appmgrUg_update_listinfos(ad);
	}

	return 0;
}

Evas_Object *appmgrUg_no_item(Evas_Object *parent, const char *text,
		const char *help)
{
	Evas_Object *layout, *icon;

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "nocontents", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	icon = elm_image_add(layout);
	elm_image_file_set(icon,
			SETTING_ICON_PATH_CFG "/00_nocontents_text_new.png",
			NULL);
	elm_object_part_content_set(layout, "nocontents.image", icon);

	elm_object_part_text_set(layout, "elm.text", _(text));
	elm_layout_signal_emit(layout, "text,disabled", "");

	if (help) {
		elm_object_part_text_set(layout, "elm.help.text", _(help));
		elm_layout_signal_emit(layout, "align.center", "elm");
	}

	return layout;
}

void progressbar_pulse_add(char *style_name, char *container_name,
		Evas_Object *layout)
{
	Evas_Object *progressbar = elm_progressbar_add(layout);

	elm_object_style_set(progressbar, style_name);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_part_content_set(layout, container_name, progressbar);

	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	evas_object_show(progressbar);
}

Evas_Object *appmgrUg_loading_item(Evas_Object *parent, const char *text,
		const char *help)
{
	Evas_Object *layout;

	layout = elm_layout_add(parent);
	elm_layout_theme_set(layout, "layout", "nocontents_loading", "default");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

	progressbar_pulse_add("process_Xlarge", "nocontents.image", layout);

	elm_object_part_text_set(layout, "elm.text", _(text));
	elm_layout_signal_emit(layout, "text,disabled", "");

	if (help) {
		elm_object_part_text_set(layout, "elm.help.text", _(help));
		elm_layout_signal_emit(layout, "align.center", "elm");
	}

	return layout;
}

/*
 Elm_Object_Item *appmgrUg_append_separator(Evas_Object *genlist,
 SettingAppMgrUG *ad)
 {
 Elm_Object_Item *item = NULL;

 item = elm_genlist_item_append(genlist, &ad->itc_sep, NULL, NULL,
 ELM_GENLIST_ITEM_NONE, NULL, NULL);
 elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
 return item;
 }
 */

char *appmgrUg_get_defualt_icon(pkgmgrinfo_appinfo_h handle)
{
	int ret;
	char *type;
	const char *icon;
	const char *svc_icon = SETTING_ICON_PATH
	"/default_icon_service.png";
	const char *app_icon = SETTING_ICON_PATH
	"/mainmenu.png";

	ret = pkgmgrinfo_appinfo_get_component_type(handle, &type);
	if (PMINFO_R_OK == ret) {
		if (0 == safeStrCmp(type, "svcapp"))
			icon = svc_icon;
		else
			icon = app_icon;
	} else {
		SETTING_TRACE_ERROR(
				"pkgmgrinfo_appinfo_get_component_type() Fail(%d)",
				ret);
		icon = app_icon;
	}

	return strdup(icon);
}

static void appmgrUg_get_listinfo(pkgmgrinfo_pkginfo_h handle,
		appmgr_listinfo *info)
{
	int ret = 0;
	char *value;

	ret_if(NULL == handle);
	ret_if(NULL == info);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_pkgid(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_pkgid() Fail(%d)", ret);
	info->pkgid = SAFE_STRDUP(value);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_label(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_label() Fail(%d)", ret);
	info->pkg_label = SAFE_STRDUP(value);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_mainappid(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_mainappid() Fail(%d)", ret);
	info->mainappid = SAFE_STRDUP(value);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_icon(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_icon() Fail(%d)", ret);
	info->icon_path = appmgrUg_check_icon(value);

	ret = pkgmgrinfo_pkginfo_is_preload(handle, &info->is_preload);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_is_preload() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_is_update(handle, &info->is_update);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_is_update() Fail(%d)", ret);

}

static int appmgrUg_get_pkg_list_iter(pkgmgrinfo_pkginfo_h handle, void *data)
{
	GList **pkg_list = data;
	appmgr_listinfo *info = NULL;

	retv_if(NULL == handle, 0);
	retv_if(NULL == data, -1);

	info = calloc(1, sizeof(appmgr_listinfo));
	if (NULL == info) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return 0;
	}

	appmgrUg_get_listinfo(handle, info);

	*pkg_list = g_list_append(*pkg_list, info);
	return 0;
}

static int appmgrUg_get_disable_pkg_list_iter(pkgmgrinfo_pkginfo_h handle,
		void *data)
{
	GList **pkg_list = data;
	appmgr_listinfo *info = NULL;

	retv_if(NULL == handle, 0);
	retv_if(NULL == data, -1);

	info = calloc(1, sizeof(appmgr_listinfo));
	if (NULL == info) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return 0;
	}

	info->is_disabled = EINA_TRUE;
	info->valid_size = EINA_TRUE;
	info->total_size = 0;

	appmgrUg_get_listinfo(handle, info);

	*pkg_list = g_list_append(*pkg_list, info);
	return 0;
}

static gint appmgrUg_sort_atoz(gconstpointer a, gconstpointer b)
{
	int len;
	const appmgr_listinfo *info1 = a;
	const appmgr_listinfo *info2 = b;

	retv_if(NULL == a, 0);
	retv_if(NULL == b, 0);
	retv_if(NULL == info1->pkg_label, 1);
	retv_if(NULL == info2->pkg_label, -1);

	if (strlen(info1->pkg_label) < strlen(info2->pkg_label))
		len = strlen(info1->pkg_label);
	else
		len = strlen(info2->pkg_label);

	return strncasecmp(info1->pkg_label, info2->pkg_label, len);
}

static gint appmgrUg_sort_ztoa(gconstpointer a, gconstpointer b)
{
	int len;
	const appmgr_listinfo *info1 = a;
	const appmgr_listinfo *info2 = b;

	retv_if(NULL == a, 0);
	retv_if(NULL == b, 0);
	retv_if(NULL == info1->pkg_label, -1);
	retv_if(NULL == info2->pkg_label, 1);

	if (strlen(info1->pkg_label) < strlen(info2->pkg_label))
		len = strlen(info1->pkg_label);
	else
		len = strlen(info2->pkg_label);

	return strncasecmp(info2->pkg_label, info1->pkg_label, len);
}

static gint appmgrUg_sort_size(gconstpointer a, gconstpointer b)
{
	const appmgr_listinfo *info1 = a;
	const appmgr_listinfo *info2 = b;

	retv_if(NULL == a, 0);
	retv_if(NULL == b, 0);

	return info2->total_size - info1->total_size;
}

gint appmgrUg_compare_pkgname(gconstpointer item, gconstpointer pkg_name)
{
	const appmgr_listinfo *appinfo = item;

	return safeStrCmp(appinfo->pkgid, pkg_name);
}

static Eina_Bool appmgrUg_get_all_pkg_sizesort(void *data)
{
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, ECORE_CALLBACK_CANCEL);
	retv_if(APPMGRUG_SORT_SIZE != ad->sorttype, ECORE_CALLBACK_CANCEL);

	ad->pkg_list = g_list_sort(ad->pkg_list, appmgrUg_sort_size);
	appmgrUg_main_genlist_append_items(ad);

	ad->size_idler = NULL;
	return ECORE_CALLBACK_CANCEL;
}

static int appmgrUg_get_all_pkg_size(int req_id, const char *pkg_type,
		const char *pkg_name, const char *key, const char *val,
		const void *pmsg, void *data)
{
	/*If get all size finished. return directly */
	GList *list;
	SettingAppMgrUG *ad = data;
	retvm_if(!ad || ad->can_sizesort, 0, "return");

	retv_if(NULL == data, 0);
	retv_if(NULL == val, 0);

	if (safeStrCmp(val, "end") == 0) {
		SETTING_TRACE_DEBUG("Finish size info update");
		ad->can_sizesort = TRUE;
		return 0;
	}

	list = g_list_find_custom(ad->pkg_list, pkg_name,
			appmgrUg_compare_pkgname);
	if (list) {
		appmgr_listinfo *info = list->data;

		info->valid_size = true;
		info->total_size = atoi(val);

		if (APPMGRUG_SORT_SIZE == ad->sorttype) {
			if (ad->size_idler)
				ecore_idler_del(ad->size_idler);
			ad->size_idler = ecore_idler_add(
					appmgrUg_get_all_pkg_sizesort, ad);
		} else {
			if (info->item)
				elm_genlist_item_fields_update(info->item,
						"elm.text.sub",
						ELM_GENLIST_ITEM_FIELD_TEXT);
		}
	}

	return 0;
}

GList *appmgrUg_sort_pkg_list(int sorttype, GList *pkg_list)
{
	if (APPMGRUG_SORT_SIZE == sorttype)
		return g_list_sort(pkg_list, appmgrUg_sort_size);
	else if (APPMGRUG_SORT_ZTOA == sorttype)
		return g_list_sort(pkg_list, appmgrUg_sort_ztoa);
	else
		return g_list_sort(pkg_list, appmgrUg_sort_atoz);
}

int appmgrUg_get_listinfos(SettingAppMgrUG *ad)
{
	int ret;
	GList *pkg_list = NULL;

	retv_if(ad == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*init app list */
	ret = pkgmgrinfo_pkginfo_get_list(appmgrUg_get_pkg_list_iter,
			&pkg_list);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgrinfo_pkginfo_get_list() Fail(%d)",
				ret);
		return SETTING_RETURN_FAIL;
	}

	APPMGRUG_STOP_POINT;

	/*Get disabled pkg list */
	pkgmgrinfo_pkginfo_filter_h filter_handle;
	ret = pkgmgrinfo_pkginfo_filter_create(&filter_handle);

	warn_if(PMINFO_R_OK != ret,
			"pkgmgrinfo_appinfo_filter_create() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_filter_add_bool(filter_handle,
	PMINFO_PKGINFO_PROP_PACKAGE_SUPPORT_DISABLE, EINA_TRUE);

	warn_if(PMINFO_R_OK != ret,
			"pkgmgrinfo_pkginfo_filter_add_bool() Fail(%d)", ret);

	ret = pkgmgrinfo_pkginfo_filter_foreach_pkginfo(filter_handle,
			appmgrUg_get_disable_pkg_list_iter, &pkg_list);
	warn_if(PMINFO_R_OK != ret,
			"pkgmgrinfo_pkginfo_filter_foreach_pkginfo() Fail(%d)",
			ret);

	pkgmgrinfo_pkginfo_filter_destroy(filter_handle);

	APPMGRUG_STOP_POINT;

	ad->old_pkg_list = ad->pkg_list;
	ad->pkg_list = appmgrUg_sort_pkg_list(ad->sorttype, pkg_list);

	APPMGRUG_STOP_POINT;

	if (ad->pc_all_size)
		pkgmgr_client_free(ad->pc_all_size);

	APPMGRUG_STOP_POINT;

	/*request total size */
	ad->can_sizesort = FALSE;

	ad->pc_all_size = pkgmgr_client_new(PC_LISTENING);
	if (NULL == ad->pc_all_size) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Faill");
		return SETTING_RETURN_FAIL;
	}

	APPMGRUG_STOP_POINT;

	ret = pkgmgr_client_set_status_type(ad->pc_all_size,
			PKGMGR_CLIENT_STATUS_GET_SIZE);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgr_client_set_status_type() Fail(%d)",
				ret);
		pkgmgr_client_free(ad->pc_all_size);
		ad->pc_all_size = NULL;
		return SETTING_RETURN_FAIL;
	}

	APPMGRUG_STOP_POINT;

	ret = pkgmgr_client_listen_status(ad->pc_all_size,
			(pkgmgr_handler) appmgrUg_get_all_pkg_size, ad);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgr_client_listen_status() Fail(%d)",
				ret);
		pkgmgr_client_free(ad->pc_all_size);
		ad->pc_all_size = NULL;
		return SETTING_RETURN_FAIL;
	}

	APPMGRUG_STOP_POINT;

	/* TODO */
	/* Find new api for Tizen 3.0 */
	/*ret = pkgmgr_client_request_size_info(); */
	/*warn_if(ret < 0, "pkgmgr_client_request_size_info() Fail", ret); */

	return SETTING_RETURN_SUCCESS;
}

void appmgrUg_get_listinfos_cb(int fn_result, SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);

	if (SETTING_RETURN_SUCCESS != fn_result) {
		SETTING_TRACE_ERROR("appmgrUg_get_listinfos() Fail(%d)",
				fn_result);
	} else {
		setting_view_update(ad->main_view, ad);

		/*after listinfos & genlist updated, free old listinfos */
		if (ad->old_pkg_list) {
			g_list_free_full(ad->old_pkg_list,
					appmgrUg_free_listinfo);
			ad->old_pkg_list = NULL;
		}
	}
	ad->list_worker = NULL;
}

void appmgrUg_get_runlistinfos_cb(int fn_result, SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);

	if (SETTING_RETURN_SUCCESS != fn_result) {
		SETTING_TRACE_ERROR("appmgrUg_get_listinfos() Fail(%d)",
				fn_result);
	} else {
		setting_view_update(ad->main_view, ad);
	}
	ad->running_list_worker = NULL;
}

void appmgrUg_pkgmgr_subscribe(SettingAppMgrUG *ad)
{
	int ret;
	ret_if(NULL == ad);

	ad->pc_main = pkgmgr_client_new(PC_LISTENING);
	if (NULL == ad->pc_main) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Fail");
		return;
	}

	ret = pkgmgr_client_set_status_type(ad->pc_all_size,
			PKGMGR_CLIENT_STATUS_GET_SIZE);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgr_client_set_status_type() Fail(%d)",
				ret);
		pkgmgr_client_free(ad->pc_all_size);
		ad->pc_main = NULL;
	}

	ret = pkgmgr_client_listen_status(ad->pc_main,
			(pkgmgr_handler) appmgrUg_pkgmgr_changed_cb, ad);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgr_client_listen_status() Fail(%d)",
				ret);
		pkgmgr_client_free(ad->pc_main);
		ad->pc_main = NULL;
		return;
	}
}

void appmgrUg_pkgmgr_deinit(SettingAppMgrUG *ad)
{
	if (ad->pc_main) {
		pkgmgr_client_free(ad->pc_main);
		ad->pc_main = NULL;
	}
	if (ad->pc_all_size) {
		pkgmgr_client_free(ad->pc_all_size);
		ad->pc_all_size = NULL;
	}
	if (ad->size_idler) {
		ecore_idler_del(ad->size_idler);
		ad->size_idler = NULL;
	}
}

static void appmgrUg_free_listinfo(gpointer data)
{
	appmgr_listinfo *info = data;

	ret_if(NULL == data);

	free(info->pkgid);
	free(info->icon_path);
	free(info->pkg_label);

	g_list_free_full(info->runinfos, appmgrUg_free_runinfo);
	g_list_free_full(info->def_appids, free);
	free(info);
}

void appmgrUg_free_listinfos(SettingAppMgrUG *ad)
{
	g_list_free_full(ad->pkg_list, appmgrUg_free_listinfo);
	ad->pkg_list = NULL;
}

Evas_Object *appmgrUg_info_title_gl_icon_get(void *data, Evas_Object *obj,
		const char *part)
{
	Evas_Object *icon = NULL;
	/*Evas_Object *lay = NULL;*/
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, NULL);
	if (safeStrCmp(part, "elm.icon.1"))
		return NULL;

	/* icon for 'elm.icon' part */
	icon = elm_icon_add(obj);

	elm_image_file_set(icon, ad->sel_icon, NULL);
	evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

#if 1
	evas_object_show(icon);
	return icon;

#else
	lay = elm_layout_add(obj);
	elm_layout_theme_set(lay, "layout", "list/B/type.2", "default");
	elm_layout_content_set(lay, "elm.swallow.content", icon);

	evas_object_show(icon);

	return lay;
#endif
}

void appmgrUg_pkg_disable_enable_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAppMgrUG *ad = data;
	ret_if(NULL == ad->pkginfo);
	appmgr_pkginfo *info = ad->pkginfo;

	if (info->pc_request)
		pkgmgr_client_free(info->pc_request);
	info->pc_request = pkgmgr_client_new(PC_REQUEST);
	if (NULL == info->pc_request) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Fail");
		return;
	}

	if (info->is_disable) {
		pkgmgr_client_activate(info->pc_request, info->pkg_type,
				info->pkgid);
		ad->pkg_request = APPMGRUG_PKG_REQUEST_ENABLE;
	} else {
		pkgmgr_client_deactivate(info->pc_request, info->pkg_type,
				info->pkgid);
		ad->pkg_request = APPMGRUG_PKG_REQUEST_DISABLE;
	}
}

void appmgrUg_pkg_stop_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

	SettingAppMgrUG *ad = data;
	setting_retm_if(ad == NULL, "Data parameter is NULL");
	appmgr_pkginfo *info = ad->pkginfo;
	ret_if(info == NULL);

	GList *cur = info->appids;
	while (cur) {
		int ret;
		bool running_flag = 0;
		char *appid = cur->data;
		cur = cur->next;
		if (NULL == appid)
			continue;
		ret = app_manager_is_running(appid, &running_flag);
		warn_if(APP_MANAGER_ERROR_NONE != ret,
				"app_manager_is_running Fail(%d)", ret);
		if (0 == running_flag)
			continue;

		app_context_h app_context;

		ret = app_manager_get_app_context(appid, &app_context);
		if (APP_MANAGER_ERROR_NONE != ret) {
			SETTING_TRACE_ERROR(
					"app_manager_get_app_context Fail(%d)",
					ret);
			return;
		}

		ret = app_manager_terminate_app(app_context);
		if (APP_MANAGER_ERROR_NONE != ret) {
			SETTING_TRACE_ERROR(
					"app_manager_terminate_app() Fail(%d)",
					ret);
			app_context_destroy(app_context);
			return;
		}

		app_context_destroy(app_context);
	}
	elm_object_disabled_set(obj, EINA_TRUE);
}

Evas_Object *appmgrUg_info_2button1_gl_icon_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;

	SettingAppMgrUG *ad = data;
	appmgr_pkginfo *info;
	bool running_flag;
	GList *cur = NULL;
	int ret;
	char *appid = NULL;
	Evas_Object *box = NULL;
	Evas_Object *button1 = NULL;
	Evas_Object *button2 = NULL;

	setting_retvm_if(ad == NULL, NULL, "Data parameter is NULL");
	if (0 != safeStrCmp(part, "elm.swallow.content")) {
		return NULL;
	}
	setting_retvm_if(ad->pkginfo == NULL, NULL, "ad->pkginfo is NULL");
	info = ad->pkginfo;

	box = elm_box_add(obj);
	elm_box_horizontal_set(box, 1);
	elm_box_align_set(box, 0.0, 0.5);
	elm_box_padding_set(box, 10, 0);

	/* Force stop button */
	running_flag = false;	/* check if have running app */
	cur = info->appids;
	while (cur) {
		appid = cur->data;
		cur = cur->next;
		if (NULL == appid)
			continue;

		ret = app_manager_is_running(appid, &running_flag);
		warn_if(APP_MANAGER_ERROR_NONE != ret,
				"app_manager_is_running Fail(%d)", ret);
		if (running_flag) {
			break;
		}
	}

	button1 = setting_create_button(box, MGRAPP_STR_FORCE_STOP,
			NULL, appmgrUg_pkg_stop_cb, ad);
	setting_retvm_if(!button1, NULL, "button1 is NULL");

	if (!running_flag)
		elm_object_disabled_set(button1, EINA_TRUE);

	/* Disable/Enable button */
	button2 = setting_create_button(box, MGRAPP_STR_DISABLE,
			NULL, appmgrUg_pkg_disable_enable_cb, ad);
	setting_retvm_if(!button2, NULL, "button2 is NULL");

	if (!info->is_support_disable) {
		elm_object_disabled_set(button2, EINA_TRUE);
	} else if (info->is_disable) {
		elm_object_disabled_set(button2, EINA_FALSE);
		elm_object_text_set(button2, _(MGRAPP_STR_ENABLE));
	}

	elm_box_pack_end(box, button1);
	elm_box_pack_end(box, button2);

	/* TODO: remove when box size bug will be fixed */
	int w, h;
	evas_object_size_hint_min_get(button1, &w, &h);
	evas_object_size_hint_min_set(box, 0, h);
	/* */

	evas_object_show(button1);
	evas_object_show(button2);
	evas_object_show(box);
	return box;
}

Evas_Object *appmgrUg_info_2button2_gl_icon_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;

	SettingAppMgrUG *ad = data;
	int ret;
	int mmc;
	char *btn_str = NULL;
	appmgr_pkginfo *info = NULL;
	const char *mmc_key = VCONFKEY_SYSMAN_MMC_STATUS;
	Evas_Object *box = NULL;
	Evas_Object *button1 = NULL;
	Evas_Object *button2 = NULL;

	setting_retvm_if(ad == NULL, NULL, "Data parameter is NULL");
	info = ad->pkginfo;
	setting_retvm_if(info == NULL, NULL, "ad->pkginfo is NULL");
	if (safeStrCmp(part, "elm.swallow.content") != 0)
		return NULL;

	box = elm_box_add(obj);
	elm_box_horizontal_set(box, 1);
	elm_box_align_set(box, 0.0, 0.5);
	elm_box_padding_set(box, 8, 0);


	/* Move to */
	mmc = VCONFKEY_SYSMAN_MMC_REMOVED;
	ret = vconf_get_int(mmc_key, &mmc);
	warn_if(ret, "vconf_get_int(%s) Fail", mmc_key);

	if (PMINFO_INTERNAL_STORAGE == info->storage)
		btn_str = MGRAPP_STR_MOVE_TO_SD;
	else
		btn_str = MGRAPP_STR_MOVE_TO_PHONE;

	button1 = setting_create_button(box, btn_str, NULL,
			appmgrUg_pkg_moveto_cb, ad);

	if (VCONFKEY_SYSMAN_MMC_MOUNTED != mmc || !info->is_movable
			|| info->is_preload)
		elm_object_disabled_set(button1, EINA_TRUE);

	/* Uninstall */
	if (info->is_preload && info->is_updated)
		btn_str = MGRAPP_STR_UNINSTALL_UPDATES;
	else
		btn_str = MGRAPP_STR_UNINSTALL;

	button2 = setting_create_button(box, btn_str, NULL,
			appmgrUg_pkg_uninstall_click, ad);

	if (!info->removable)
		elm_object_disabled_set(button2, EINA_TRUE);

	elm_box_pack_end(box, button1);
	elm_box_pack_end(box, button2);

	/* TODO: remove when box size bug will be fixed */
	int w, h;
	evas_object_size_hint_min_get(button1, &w, &h);
	evas_object_size_hint_min_set(box, 0, h);
	/* */

	evas_object_show(button1);
	evas_object_show(button2);
	evas_object_show(box);
	return box;
}

Evas_Object *appmgrUg_info_1button_gl_icon_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;

	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	if (0 != safeStrCmp(part, "elm.swallow.content"))
		return NULL;

	Setting_GenGroupItem_Data *item_data = data;
	SettingAppMgrUG *ad = item_data->userdata;
	setting_retvm_if(ad == NULL, NULL, "ad is NULL");

	Evas_Object *box = elm_box_add(obj);
	elm_box_horizontal_set(box, 1);
	elm_box_align_set(box, 0.0, 0.5);
	elm_box_padding_set(box, 8, 0);

	appmgr_pkginfo *info;

	retv_if(NULL == ad->pkginfo, NULL);

	info = ad->pkginfo;

	/* Clear cache */
	Evas_Object *button1 = setting_create_button(box,
			MGRAPP_STR_CLEAR_CACHE, NULL,
			appmgrUg_pkg_clear_cache_click, ad);

	if (info->sz_cache <= 0) {
		elm_object_disabled_set(button1, EINA_TRUE);
	}

	/* Fake button to reserve space after button1 */
	Evas_Object *button2 = setting_create_button(box, "BLANK", NULL, NULL,
			NULL);
	evas_object_hide(button2);

	elm_box_pack_end(box, button1);
	elm_box_pack_end(box, button2);

	/* TODO: remove when box size bug will be fixed */
	int w, h;
	evas_object_size_hint_min_get(button1, &w, &h);
	evas_object_size_hint_min_set(box, 0, h);
	/* */

	/*evas_object_show(button1); */
	evas_object_show(button1);
	evas_object_show(box);

	return box;
}

char *appmgrUg_info_title_gl_label_get(void *data, Evas_Object *obj,
		const char *part)
{
	char *label = NULL;
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, NULL);

	if (0 == strcmp(part, "elm.text")) {
		label = SAFE_STRDUP(ad->sel_label);
	} else if (0 == strcmp(part, "elm.text.sub")) {
		char desc[APPMGRUG_MAX_STR_LEN] = { 0 };

		if (APPMGRUG_TAB_RUNNING == ad->tabtype) {
			snprintf(desc, sizeof(desc),
					_(MGRAPP_STR_ACTIVE_APPS_N),
					g_list_length(ad->runinfos));
		} else {
			snprintf(desc, sizeof(desc), _(MGRAPP_STR_VERSION_STR),
					ad->pkginfo->pkg_version);
		}

		label = strdup(desc);
	}

	return label;
}
/*
 void reset_all_restrict_apps() {
 GSList *appid_list = NULL;
 resourced_ret_c ret = restrictions_foreach(_reset_all_restrict_apps_cb,
 &appid_list);
 if(RESOURCED_ERROR_OK != ret){
 SETTING_TRACE_ERROR("unset all restrict apps fail with %d",ret);
 }
 g_slist_foreach(appid_list, (GFunc)_reset_restrict_cb, NULL);
 _free_GSList(appid_list);
 } */

static void _reset_app_settings_cb(int fn_result, SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);

	ad->pkg_request = APPMGRUG_PKG_REQUEST_NONE;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (fn_result) {
		SETTING_TRACE_ERROR("_reset_app_setting_fn() Fail(%d)",
				fn_result);
		appmgrUg_fail_popup(MGRAPP_STR_ERROR, ad);
	}
	ad->reset_worker = NULL;
}

static int _reset_app_setting_fn(SettingAppMgrUG *ad)
{
	int ret;

	retv_if(NULL == ad, -1);

	ret = appsvc_unset_all_defapps();
	warn_if(APPSVC_RET_OK != ret, "appsvc_unset_all_defapps() Fail(%d)",
			ret);

	APPMGRUG_STOP_POINT;

	/*reset_all_restrict_apps(); */

	return 0;
}

int appmgrUg_reset_app_settings(SettingAppMgrUG *ad)
{
	if (ad->popup)
		evas_object_del(ad->popup);
	/* do not show popup
	 ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
	 PROGRESSBAR_STYLE,
	 NULL, NULL, appmgrUg_popup_del, 0, TRUE, TRUE, 0);
	 */
	ad->pkg_request = APPMGRUG_PKG_REQUEST_RESET;

	ad->reset_worker = appmgrUg_start_async_worker(_reset_app_setting_fn,
			_reset_app_settings_cb, ad);

	return 0;
}
