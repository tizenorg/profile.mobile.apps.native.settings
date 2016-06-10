/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
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

#include <setting-applications-defaultapp.h>
#include <aul_svc.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>

static int setting_applications_defaultapp_create(void *cb);
static int setting_applications_defaultapp_destroy(void *cb);
static int setting_applications_defaultapp_update(void *cb);
static int setting_applications_defaultapp_cleanup(void *cb);

static void setting_applications_defaultapp_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info);
static void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info);

static gboolean setting_applications_create_homescreen_setting_ug(void *data);
static char *setting_application_get_defaultapp_name(const char *appid);

setting_view setting_view_applications_defaultapp = {
		.create = setting_applications_defaultapp_create,
		.destroy = setting_applications_defaultapp_destroy,
		.update = setting_applications_defaultapp_update,
		.cleanup = setting_applications_defaultapp_cleanup, };
/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static char *setting_application_get_defaultapp_name(const char *appid)
{
	SETTING_TRACE_BEGIN;
	int ret = PMINFO_R_ERROR;
	char *name = NULL;
	char *ret_name = NULL;
	pkgmgrinfo_appinfo_h handle;
	if (!appid) {
		SETTING_TRACE_ERROR("invalid appid");
		return NULL;
	}
	ret = pkgmgrinfo_appinfo_get_appinfo(appid, &handle);
	if (ret != PMINFO_R_OK) {
		SETTING_TRACE_ERROR(
				"Failed[%d] pkgmgrinfo_appinfo_get_appinfo(%s)",
				ret, appid);
		return NULL;
	}
	ret = pkgmgrinfo_appinfo_get_label(handle, &name);
	if (ret != PMINFO_R_OK) {
		SETTING_TRACE_ERROR("pkgmgrinfo_appinfo_get_label(%s) : %d",
				appid, ret);
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return NULL;
	}
	if (!name) {
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return NULL;
	}
	ret_name = strdup(name);
	pkgmgrinfo_appinfo_destroy_appinfo(handle);
	return ret_name;
}

void _unset_sel(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Elm_Object_Item *item = event_info;
	elm_genlist_item_selected_set(item, EINA_FALSE);
}

static void app_default_get_listinfo(pkgmgrinfo_pkginfo_h handle,
		default_app *info)
{
	int ret = 0;
	char *value;

	ret_if(NULL == handle);
	ret_if(NULL == info);


	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_pkgid(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_pkgid() Fail(%d)", ret);
	info->pkgid = SAFE_STRDUP(value);

	//first check if this app is default
	ret = aul_svc_is_defapp(info->pkgid);
	if (info->pkgid && 1 == ret)
	{
		info->defapp = 1;
	} else
		return;

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_icon(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_icon() Fail(%d)", ret);
	if (EINA_TRUE == ecore_file_exists(value))
		info->icon_path = strdup(value);
	else
		info->icon_path = SETTING_ICON_PATH"/mainmenu.png";
	SETTING_TRACE("##:[%s]", info->icon_path);

	value = NULL;
	ret = pkgmgrinfo_pkginfo_get_label(handle, &value);
	warn_if(ret < 0, "pkgmgrinfo_pkginfo_get_label() Fail(%d)", ret);
	info->pkg_label = SAFE_STRDUP(value);

}

static int app_default_get_pkg_list_iter(pkgmgrinfo_pkginfo_h handle, void *data)
{
	GList **pkg_list = data;
	default_app *info = NULL;

	retv_if(NULL == handle, 0);
	retv_if(NULL == data, -1);

	info = calloc(1, sizeof(default_app));
	if (NULL == info) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return 0;
	}

	app_default_get_listinfo(handle, info);

	if(info->defapp == 1)
		*pkg_list = g_list_append(*pkg_list, info);

	return 0;
}

static gint _glist_sort_atoz(gconstpointer a, gconstpointer b)
{
	int len;
	const default_app *info1 = a;
	const default_app *info2 = b;

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

void default_app_list_init(void *data, Evas_Object *genlist)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;
	GList *pkg_list = NULL;
	default_app *info = NULL;
	int ret = -1;

	/* Get list of default app by package manager */
	/* init app list */
	ret = pkgmgrinfo_pkginfo_get_list(app_default_get_pkg_list_iter,
			&pkg_list);
	if (ret < 0) {
		SETTING_TRACE_ERROR("pkgmgrinfo_pkginfo_get_list() Fail(%d)", ret);
		return;
	}
	if (NULL != pkg_list) {
		if(g_list_length(pkg_list) > 0){
			pkg_list = g_list_sort(pkg_list, _glist_sort_atoz);

			while(pkg_list){
				info = pkg_list->data;
				pkg_list = pkg_list->next;

				if (NULL == info) {
					SETTING_TRACE_ERROR("info is a null");
					continue;
				}

				info->item = elm_genlist_item_append(genlist,
						&(ad->itc_1icon_1button), info, NULL,
						ELM_GENLIST_ITEM_NONE,
						_unset_sel, ad);
			}
		}
	} else {
		Setting_GenGroupItem_Data *item = setting_create_Gendial_field_def(
				genlist, &(ad->itc_1text),
				NULL, NULL,
				SWALLOW_Type_INVALID, NULL,
				NULL, 0,
				MGRAPP_STR_DEFAULT_NO_APPS, NULL, NULL);
		elm_object_item_disabled_set(item->item, EINA_TRUE);
	}
	ad->pkg_list = pkg_list;
}

void construct_defaultapp(void *data, Evas_Object *genlist)
{
	ret_if(data == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;

	/* "Set defaults" */
	setting_create_Gendial_field_titleItem(genlist, &(ad->itc_grp_title),
			MGRAPP_STR_LAUNCH_BY_DEFAULT, NULL);
	char *appid = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
	char *sub_desc = setting_application_get_defaultapp_name(appid);
	ad->data_home = setting_create_Gendial_field_def(genlist,
			&(ad->itc_2text_2),
			setting_applications_defaultapp_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0, KeyStr_Home, sub_desc, NULL);
	if (ad->data_home) {
		ad->data_home->userdata = ad;
		__BACK_POINTER_SET(ad->data_home);
	} else {
		SETTING_TRACE_ERROR("ad->data_home is NULL");
	}

	/* Create default app list */
	setting_create_Gendial_field_titleItem(genlist, &(ad->itc_grp_title),
			MGRAPP_STR_CLEAR_DEFAULT, NULL);
	default_app_list_init(ad, genlist);
}

void destruct_defaultapp(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
#if 0
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
#endif
}

static void setting_applications_defaultapp_vconf_change_cb(keynode_t *key,
		void *data)
{
	ret_if(data == NULL);

	SettingApplicationsUG *ad = data;

	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME)) {
		if (ad->data_home) {
			G_FREE(ad->data_home->sub_desc);
			/*ad->data_home->sub_desc =
			 * get_pa_backlight_time_value_str(); */
			char *pkgname = vconf_get_str(
					VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);

			/* pkgname --> label */
			char *label = setting_application_get_defaultapp_name(
					pkgname);
			ad->data_home->sub_desc = label;
			elm_object_item_data_set(ad->data_home->item,
					ad->data_home);
			elm_genlist_item_update(ad->data_home->item);
		}
	}
}

static int setting_applications_defaultapp_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *)cb;
	Evas_Object *genlist = elm_genlist_add(ad->win_main_layout);
	retvm_if(genlist == NULL, SETTING_RETURN_FAIL,
			"Cannot set scroller object as contento of layout");
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_clear(genlist); /* first to clear list */

	setting_push_layout_navi_bar(KeyStr_DefaultApplications,
			_("IDS_ST_BUTTON_BACK"), NULL, NULL,
			setting_applications_defaultapp_click_softkey_cancel_cb,
			NULL,
			NULL, ad, genlist, ad->navi_bar, NULL);

	/*ad->genlist = genlist; */
	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb,
			ad);

	construct_defaultapp(ad, genlist);/*------- */
	/* ad->data_home */
	int ret = vconf_notify_key_changed(
			VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME,
			setting_applications_defaultapp_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	/* org.tizen.homescreen-efl : homescreen-efl */
	/* org.tizen.homescreen		: Homescreen */
	/* org.tizen.menu-screen : Simple Menu-screen */

	setting_view_applications_defaultapp.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_defaultapp_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_applications_defaultapp.is_create),
			SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	destruct_defaultapp(cb);/*------- */

	SettingApplicationsUG *ad = (SettingApplicationsUG *)cb;

	vconf_ignore_key_changed(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME,
			setting_applications_defaultapp_vconf_change_cb);

	setting_view_applications_defaultapp.is_create = 0;
	elm_naviframe_item_pop(ad->navi_bar);

	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_defaultapp_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;

}

static int setting_applications_defaultapp_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_applications_defaultapp_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void setting_applications_defaultapp_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;

	setting_view_change(&setting_view_applications_defaultapp,
			&setting_view_applications_main, ad);
}

void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	/*SETTING_TRACE("chk_status[%d]", list_item->chk_status); */
	if (!safeStrCmp(KeyStr_Home, list_item->keyStr)) {
		SETTING_TRACE("click Home and run UG");
		setting_applications_create_homescreen_setting_ug(ad);
	}
}

static gboolean setting_applications_create_homescreen_setting_ug(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	/* ad is point to data */
	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;

	if (ad->ug_homescreen) {
		SETTING_TRACE("Font UG is already loaded.");
		return FALSE;
	}

	app_launcher("org.tizen.setting-homescreen");

	return TRUE;
}

