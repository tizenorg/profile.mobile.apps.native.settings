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
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>


static int setting_applications_defaultapp_create(void *cb);
static int setting_applications_defaultapp_destroy(void *cb);
static int setting_applications_defaultapp_update(void *cb);
static int setting_applications_defaultapp_cleanup(void *cb);

static void setting_applications_defaultapp_click_softkey_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info);

static gboolean setting_applications_create_homescreen_setting_ug(void *data);
static char *setting_application_get_defaultapp_name(const char *appid);

setting_view setting_view_applications_defaultapp = {
	.create = setting_applications_defaultapp_create,
	.destroy = setting_applications_defaultapp_destroy,
	.update = setting_applications_defaultapp_update,
	.cleanup = setting_applications_defaultapp_cleanup,
};
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
		SETTING_TRACE_ERROR("Failed[%d] ail_get_appinfo(%s)", ret, appid);
		return NULL;
	}
	ret = pkgmgrinfo_appinfo_get_label(handle, &name);
	if (ret != PMINFO_R_OK) {
		SETTING_TRACE_ERROR("Failed ail_appinfo_get_str(%s) : %d", appid, ret);
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

void construct_defaultapp(void *data, Evas_Object *genlist)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	char *appid = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
	char *sub_desc = setting_application_get_defaultapp_name(appid);
	ad->data_home =
		setting_create_Gendial_field_def(genlist, &(ad->itc_2text_2),
										 setting_applications_defaultapp_mouse_up_Gendial_list_cb,
										 ad, SWALLOW_Type_INVALID, NULL,
										 NULL, 0, KeyStr_Home,
										 sub_desc, NULL);
	if (ad->data_home) {
		ad->data_home->userdata = ad;
		__BACK_POINTER_SET(ad->data_home);
	} else {
		SETTING_TRACE_ERROR("ad->data_home is NULL");
	}
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

static void setting_applications_defaultapp_vconf_change_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);

	SettingApplicationsUG *ad = data;

	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME)) {
		if (ad->data_home) {
			G_FREE(ad->data_home->sub_desc);
			/*ad->data_home->sub_desc = get_pa_backlight_time_value_str(); */
			char *pkgname = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);

			/* pkgname --> label */
			char *label = setting_application_get_defaultapp_name(pkgname);
			ad->data_home->sub_desc = label;
			elm_object_item_data_set(ad->data_home->item, ad->data_home);
			elm_genlist_item_update(ad->data_home->item);
		}
	}
}



static int setting_applications_defaultapp_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;
	Evas_Object *genlist = elm_genlist_add(ad->win_main_layout);
	retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "Cannot set scroller object as contento of layout");
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_clear(genlist);	/* first to clear list */

	setting_push_layout_navi_bar(KeyStr_DefaultApplications,
								 _("IDS_ST_BUTTON_BACK"), NULL, NULL,
								 setting_applications_defaultapp_click_softkey_cancel_cb,
								 NULL,
								 NULL, ad, genlist, ad->navi_bar, NULL);

	/*ad->genlist = genlist; */
	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, ad);

	construct_defaultapp(ad, genlist);/*------- */
	/* ad->data_home */
	int ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME,
									   setting_applications_defaultapp_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	/* org.tizen.homescreen-efl : homescreen-efl */
	/* org.tizen.homescreen     : Homescreen */
	/* org.tizen.menu-screen : Simple Menu-screen */

	setting_view_applications_defaultapp.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_defaultapp_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_applications_defaultapp.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	destruct_defaultapp(cb);/*------- */

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;


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

static void
setting_applications_defaultapp_click_softkey_cancel_cb(void *data,
														Evas_Object *obj,
														void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	setting_view_change(&setting_view_applications_defaultapp,
						&setting_view_applications_main, ad);
}

void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
		(Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

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

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;	/* ad is point to data */

	if (ad->ug_homescreen) {
		SETTING_TRACE("Font UG is already loaded.");
		return FALSE;
	}


#if 0
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs) {
		return FALSE;
	}
	cbs->layout_cb = setting_applications_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_applications_destroy_homescreen_setting_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);

	ad->ug_homescreen = setting_ug_create(ad->ug, "setting-homescreen-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_homescreen) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_homescreen");
		evas_object_show(ad->ly_main);
	}
	FREE(cbs);
#else
	app_launcher("setting-homescreen-efl");
#endif
	return TRUE;
}


