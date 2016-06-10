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
#include <setting-common-data-type.h>
#include <setting-common-data-slp-setting.h>
#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#include <setting-locktype-main.h>
#include <pkgmgr-info.h>
#include <efl_extension.h>
#if SUPPORT_ENCRYPTION
#include <ode.h>
#endif

#include <dpm/restriction.h>
#include <dpm/password.h>

#include <auth-passwd.h>

#define TBD 0

static int setting_locktype_main_create(void *cb);
static int setting_locktype_main_destroy(void *cb);
static int setting_locktype_main_update(void *cb);
static int setting_locktype_main_cleanup(void *cb);

#if 0
static void __record_btn_click_cb(void *data, Evas_Object *obj,
		void *event_info);
#endif

setting_view setting_view_locktype_main = {
		.create = setting_locktype_main_create,
		.destroy = setting_locktype_main_destroy,
		.update = setting_locktype_main_update,
		.cleanup = setting_locktype_main_cleanup,
};

openlock_appdata *lockapp_data = NULL;

/* ***************************************************
 **
 **basic func
 **
 ****************************************************/

/*
static void dpm_dpmusb_policy_changed_cb(const char* policy, const char* state, void* data)
{
	SETTING_TRACE_BEGIN;

	SETTING_TRACE("External storag policy changed: %s : %s", policy, state);

	SETTING_TRACE_END;
}
*/

static int dpm_get_password_policy(dpm_password_quality_e *quality, int *minimum_length)
{
	SETTING_TRACE_BEGIN;
	dpm_context_h context;
	dpm_restriction_policy_h policy;


	context = dpm_context_create();
	if (context == NULL) {
		SETTING_TRACE_ERROR("Failed to create client context");
		return 0;
	}

	policy = dpm_context_acquire_restriction_policy(context);
	if (policy == NULL) {
		SETTING_TRACE_ERROR("Failed to get storage policy interface");
		dpm_context_destroy(context);
		return 0;
	}

	int ret;
	if ((ret=dpm_password_get_quality(policy, quality)) != DPM_ERROR_NONE){
		SETTING_TRACE_ERROR("Failed to dpm_password_get_quality : %d", ret);
		dpm_context_release_password_policy(context, policy);
		dpm_context_destroy(context);
		return 0;
	}
	SETTING_TRACE("quality : %d", *quality);

	if ((ret=dpm_password_get_minimum_length(policy, minimum_length)) != DPM_ERROR_NONE){
		SETTING_TRACE_ERROR("Failed to dpm_password_get_minimum_length : %d", ret);
		dpm_context_release_password_policy(context, policy);
		dpm_context_destroy(context);
		return 0;
	}
	SETTING_TRACE("minimum_length : %d", *minimum_length);

	dpm_context_release_restriction_policy(context, policy);
	dpm_context_destroy(context);

	return 1;
}


static int __get_lockapp_index_from_appname(char *app_name)
{
	SETTING_TRACE_BEGIN;
	if (app_name == NULL)
		return SETTING_RETURN_FAIL;

	openlock_appdata *item = lockapp_data;
	for (; item != NULL; item = item->next) {
		if (safeStrCmp(item->app_name, app_name) == 0)
			return item->index;
	}
	return SETTING_RETURN_FAIL;
}
static char *__get_lockapp_pkgname_from_appname(char *app_name)
{
	SETTING_TRACE_BEGIN;
	if (app_name == NULL)
		return NULL;

	openlock_appdata *item = lockapp_data;
	for (; item != NULL; item = item->next) {
		if (safeStrCmp(item->app_name, app_name) == 0)
			return item->pkg_name;
	}
	return NULL;
}

int __get_appinfo_cb(pkgmgrinfo_appinfo_h appinfo, void *user_data)
{
	SETTING_TRACE_BEGIN;
	if (!appinfo) {
		SETTING_TRACE_DEBUG("handle is NULL");
		return -1;
	}

	openlock_appdata *item = lockapp_data;
	openlock_appdata *new_item = NULL;
	char *app_name = NULL;
	char *app_id = NULL;
	pkgmgrinfo_appinfo_h handle;
	int ret = 0;

	ret = pkgmgrinfo_appinfo_get_appid(appinfo, &app_id);
	if (ret != 0) {
		SETTING_TRACE_DEBUG(
				"pkgmgrinfo_appinfo_get_appid() returns error");
		return -1;
	}

	if (!app_id) {
		SETTING_TRACE_DEBUG("app_id is NULL");
		return -1;
	}

	SETTING_TRACE_DEBUG("app_id %s", app_id);

	ret = pkgmgrinfo_appinfo_get_appinfo(app_id, &handle);
	if (ret != PMINFO_R_OK) {
		SETTING_TRACE_DEBUG(
				"pkgmgrinfo_appinfo_get_appinfo() returns error");
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return -1;
	}

	ret = pkgmgrinfo_appinfo_get_label(handle, &app_name);
	if (ret != PMINFO_R_OK) {
		SETTING_TRACE_DEBUG(
				"pkgmgrinfo_appinfo_get_label() returns error");
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return -1;
	}

	SETTING_TRACE_DEBUG("app_name %s", app_name);

	while (item->next != NULL)
		item = item->next;

	if (item->index == 0 && item->pkg_name == NULL) {
		SETTING_TRACE_DEBUG("first app");
		/* first app */
		item->pkg_name = (char *) strdup(app_id);
		item->app_name = (char *) strdup(app_name);

		if (app_id && app_name)
			SETTING_TRACE_DEBUG("app info %d %s %s", item->index,
					item->pkg_name, item->app_name);
	} else {
		SETTING_TRACE_DEBUG("create new");
		/* create new */
		new_item = (openlock_appdata *) malloc(
				sizeof(openlock_appdata));
		if (new_item != NULL) {
			memset(new_item, 0x00, sizeof(openlock_appdata));
			new_item->pkg_name = (char *) strdup(app_id);
			new_item->app_name = (char *) strdup(app_name);
			new_item->index = item->index + 1;
			new_item->prev = item;
			item->next = new_item;

			if (app_id && app_name)
				SETTING_TRACE_DEBUG("app info %d %s %s",
						new_item->index,
						new_item->pkg_name,
						new_item->app_name);
		} else {
			SETTING_TRACE_DEBUG("malloc() failed");
		}
	}
	/*FREE(app_id); */
	/*FREE(app_name); */
	pkgmgrinfo_appinfo_destroy_appinfo(handle);
	SETTING_TRACE_END;
	return 0;
}

#if 0
static int __add_3rd_party_lock(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, -1);

	int ret_index = -1;
	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;

	/* Get info from AIL */
	pkgmgrinfo_appinfo_filter_h filter = NULL;
	int ret = 0;
	int count = -1;

	ret = pkgmgrinfo_appinfo_filter_create(&filter);
	if (ret > 0) {
		SETTING_TRACE_DEBUG("pkgmgrinfo_appinfo_filter_create() "
				"failed");
		return ret_index;
	}
	ret = pkgmgrinfo_appinfo_filter_add_string(filter,
			PMINFO_APPINFO_PROP_APP_CATEGORY,
			"http://tizen.org/category/lockapp");
	ret = pkgmgrinfo_appinfo_filter_count(filter, &count);
	SETTING_TRACE_DEBUG("There is/are %d 3rd lock app(s)", count);
	if (count <= 0) {
		SETTING_TRACE_DEBUG("No 3rd lock app");
		pkgmgrinfo_appinfo_filter_destroy(filter);
		filter = NULL;
		return ret_index;
	}

	/* delete if already exist. */
	if (lockapp_data) {
		openlock_appdata *list = lockapp_data;
		while (list != NULL) {
			openlock_appdata *cur = list;
			FREE(list->app_name);
			FREE(list->pkg_name);
			list = list->next;
			FREE(cur);
		}
		lockapp_data = NULL;
	}

	lockapp_data = (openlock_appdata *)malloc(sizeof(openlock_appdata));
	if (lockapp_data == NULL) {
		SETTING_TRACE_DEBUG("malloc() failed");
		FREE(filter);
		return ret_index;
	}
	memset(lockapp_data, 0x00, sizeof(openlock_appdata));
	lockapp_data->prev = NULL;
	lockapp_data->next = NULL;
	lockapp_data->pkg_name = NULL;
	lockapp_data->app_name = NULL;
	lockapp_data->index = 0;

	ret = pkgmgrinfo_appinfo_filter_foreach_appinfo(filter,
			__get_appinfo_cb, NULL);
	SETTING_TRACE("ret:%d", ret);
	pkgmgrinfo_appinfo_filter_destroy(filter);
	filter = NULL;

	/* Add to genlist */
	openlock_appdata *list = lockapp_data;
	int index = 0;
	for (index = 0; index < count; index++) {
		ad->data_locktype_3rd[index] = setting_create_Gendial_field_1radio(
				ad->genlist,
				&(itc_1text_1icon_3),
				/*add to sel_cb */
				setting_locktype_main_mouse_up_Gendial_list_cb,
				ad, /* sel data */
				SWALLOW_Type_1RADIO,
				ad->lock_type_rd,
				SETTING_SCREEN_LOCK_TYPE_OTHER + index,
				list->app_name,
				NULL);
		if (ad->data_locktype_3rd[index]) {
			ad->data_locktype_3rd[index]->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}

		char *open_lock_name = NULL;
		open_lock_name = vconf_get_str(
				VCONFKEY_SETAPPL_3RD_LOCK_PKG_NAME_STR);
		if (safeStrCmp(open_lock_name, list->app_name) == 0)
		ret_index = index;
		FREE(open_lock_name);

		list = list->next;
	}
	SETTING_TRACE_END;
	FREE(filter);
	return ret_index;
}
#endif

static void __change_simple_password_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	/*	for update */

	SettingLocktypeUG *ad = (SettingLocktypeUG *) list_item->userdata;
	if (ad == NULL)
		return;

	ad->pw_type = SETTING_LOCKTYPE_PW_CHANGE_SIMPLE_PASSWD;
	setting_locktype_create_password_sg(ad);
}

static void __change_password_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	/*	for update */

	SettingLocktypeUG *ad = (SettingLocktypeUG *) list_item->userdata;
	if (ad == NULL)
		return;

	ad->pw_type = SETTING_LOCKTYPE_PW_CHANGE_PASSWD;
	setting_locktype_create_password_sg(ad);
}

void __add_locktype_items(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;

	Evas_Object *radio;

	int locktype = 0;
	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &locktype);
	SETTING_TRACE("locktype is %d", locktype);

	radio = elm_radio_add(ad->genlist);
	if (radio) {
		elm_radio_state_value_set(radio, -1);
		ad->lock_type_rd = radio;
	}

	evas_object_smart_callback_add(ad->genlist, "realized",
			__gl_realized_cb, NULL);


	/* to do : radio menu */
	/* 0) None */
	ad->data_locktype_none = setting_create_Gendial_field_def(
			ad->genlist, &(itc_1text_1icon_3),
			/*add to sel_cb */
			setting_locktype_main_mouse_up_Gendial_list_cb,
			ad, /* sel data */
			SWALLOW_Type_INVALID,
			NULL, NULL, SETTING_SCREEN_LOCK_TYPE_NONE,
			"IDS_ST_BODY_NONE",
			NULL, NULL);
	if (ad->data_locktype_none) {
		ad->data_locktype_none->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 1) swipe */
	ad->data_locktype_swipe = setting_create_Gendial_field_def(ad->genlist,
			&(itc_1text_1icon_3),
			/*add to sel_cb */
			setting_locktype_main_mouse_up_Gendial_list_cb,
			ad, /* sel data */
			SWALLOW_Type_INVALID,
			NULL, NULL, SETTING_SCREEN_LOCK_TYPE_SWIPE,
			Keystr_Swipe,
			NULL, NULL);
	if (ad->data_locktype_swipe) {
		ad->data_locktype_swipe->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 4) simple password */
	ad->data_locktype_simple = setting_create_Gendial_field_def(
			ad->genlist,
			&(itc_1text_1icon_3),
			/*add to sel_cb */
			setting_locktype_main_mouse_up_Gendial_list_cb,
			ad, /* sel data */
			SWALLOW_Type_INVALID,
			NULL,
			NULL,
			SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD,
			"IDS_ST_BODY_SIMPLE_PASSWORD",
			NULL,
			NULL);
	if (ad->data_locktype_simple) {
		ad->data_locktype_simple->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	/* 5) password */
	ad->data_locktype_password = setting_create_Gendial_field_def(
			ad->genlist,
			&(itc_1text_1icon_3),
			/*add to sel_cb */
			setting_locktype_main_mouse_up_Gendial_list_cb,
			ad, /* sel data */
			SWALLOW_Type_INVALID,
			NULL,
			NULL,
			SETTING_SCREEN_LOCK_TYPE_PASSWORD,
			"IDS_ST_BODY_PASSWORD",
			NULL,
			NULL);
	if (ad->data_locktype_password) {
		ad->data_locktype_password->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}



	dpm_password_quality_e quality;
	int minimum_length;
	dpm_get_password_policy(&quality, &minimum_length);

#if 0
typedef enum {
    DPM_PASSWORD_QUALITY_UNSPECIFIED     = 0x00,    /**< No requirements for password. */
    DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD = 0x01,    /**< EAS(Exchange ActiveSync) requirement for simple password */
    DPM_PASSWORD_QUALITY_SOMETHING       = 0x10,    /**< Some kind password is required, but doesn't care what it is */
    DPM_PASSWORD_QUALITY_NUMERIC         = 0x20,    /**< Containing at least numeric characters */
    DPM_PASSWORD_QUALITY_ALPHABETIC      = 0x40,    /**< Containing at least alphabetic (or other symbol) characters */
    DPM_PASSWORD_QUALITY_ALPHANUMERIC    = 0x80,    /**< Containing at least numeric and alphabetic characters */
} dpm_password_quality_e;
#endif

	if(quality == DPM_PASSWORD_QUALITY_UNSPECIFIED){
		/* no dim*/
	}
	else if(quality == DPM_PASSWORD_QUALITY_SIMPLE_PASSWORD || quality == DPM_PASSWORD_QUALITY_SOMETHING || quality == DPM_PASSWORD_QUALITY_NUMERIC){
		// dim swipe, none
		if(ad->data_locktype_none) setting_disable_genlist_item(ad->data_locktype_none->item);
		if(ad->data_locktype_swipe) setting_disable_genlist_item(ad->data_locktype_swipe->item);
		if(minimum_length>=5){
			/* dim simple password */
			if(ad->data_locktype_simple) setting_disable_genlist_item(ad->data_locktype_simple->item);
		}
	}
	else if(quality == DPM_PASSWORD_QUALITY_ALPHABETIC || quality == DPM_PASSWORD_QUALITY_ALPHANUMERIC){
		// dim swipe, none, simple password
		if(ad->data_locktype_none) setting_disable_genlist_item(ad->data_locktype_none->item);
		if(ad->data_locktype_swipe) setting_disable_genlist_item(ad->data_locktype_swipe->item);
		if(ad->data_locktype_simple) setting_disable_genlist_item(ad->data_locktype_simple->item);
	}


#if 0
	int index = -1;
	/* Add 3rd party lock apps in list. */
	index = __add_3rd_party_lock(ad);

	if (index < 0)
	elm_radio_value_set(radio, locktype);
	else
	elm_radio_value_set(radio, locktype + index);
#endif
	/* End */
	elm_radio_value_set(radio, locktype);

	SETTING_TRACE_END;
}

static void __lock_type_key_changed_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);

	SettingLocktypeUG *ad = data;

	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT)) {
		/*status = vconf_keynode_get_int(key); */
		if (ad->ug_passwd) {
			ad->ug_is_destroying = TRUE;
			setting_ug_destroy(ad->ug_passwd);
		}

		if (ad->genlist) {
			elm_genlist_clear(ad->genlist);
			__add_locktype_items(ad);
		}
	}
	/*ug_destroy_me(ad->ug); */
}

static int setting_locktype_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingLocktypeUG *ad = (SettingLocktypeUG *) cb;

	/* add basic layout */

	Evas_Object *scroller = NULL;
	ad->ly_main = setting_create_layout_navi_bar_genlist(
			ad->win_main_layout, ad->win_get,
			"IDS_ST_BODY_SCREEN_LOCK_TYPE", _("IDS_ST_BUTTON_BACK"),
			NULL,
			(setting_call_back_func) setting_locktype_main_click_softkey_back_cb,
			NULL, ad, &scroller, &(ad->navi_bar));
	ad->screen_lock_main_item = elm_naviframe_top_item_get(ad->navi_bar);

	ad->genlist = scroller;

	__add_locktype_items(ad);

	vconf_notify_key_changed(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
			__lock_type_key_changed_cb, ad);

	setting_view_locktype_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_locktype_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingLocktypeUG *ad = (SettingLocktypeUG *) cb;

	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}

	(void) vconf_ignore_key_changed(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
			__lock_type_key_changed_cb);

	evas_object_del(ad->ly_main);
	ad->screen_lock_main_item = NULL;

	setting_view_locktype_main.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_locktype_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*__update_locktype_items_status(cb); */

	return SETTING_RETURN_SUCCESS;
}

static int setting_locktype_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/

const char *motion_img[] = {
	SETTING_ICON_PATH_CFG "motions/unlock_illust_01.png",
	SETTING_ICON_PATH_CFG "motions/unlock_illust_02.png",
	NULL
};

#if 0

static void
__motion_set_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("[HYEJIN] __motion_guild_ly_cb() start. ");
	/* error check */
	retm_if(data == NULL, "[Setting > Security] Data parameter is NULL");
	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;

	elm_naviframe_item_pop(ad->navi_bar);
	ug_destroy_me(ad->ug);
}

static void
__motion_cancel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("[HYEJIN] __motion_guild_ly_cb() start. ");
	/* error check */
	retm_if(data == NULL, "[Setting > Security] Data parameter is NULL");
	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;

	elm_naviframe_item_pop(ad->navi_bar);
	ug_destroy_me(ad->ug);
}
#endif

static void setting_locktype_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)
			elm_object_item_data_get(item);
	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;
	setting_retm_if(NULL == list_item, "list_item is NULL");

	int lock_type = list_item->chk_status;
	SETTING_TRACE("clicking item[%s - %d] ", _(list_item->keyStr),
			lock_type);

	int old_type;
	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &old_type);

	ad->selected_lock_type = list_item->keyStr;

#if 0
	/* If lock_type is same with old_lock_type, return. */
	if (lock_type == old_type) {
		SETTING_TRACE_DEBUG("[Screen Lock Type] Selected same type");
		return;
	}
#endif
	int index = -1;
	char *pkg_name = NULL;

	switch (lock_type) {
	case SETTING_SCREEN_LOCK_TYPE_NONE:
	case SETTING_SCREEN_LOCK_TYPE_SWIPE:
		/* To do : Call security-server API. pw : 0000 */
		if (old_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD
				|| old_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
#if SECURITY_SERVER
			//int result = security_server_set_pwd(ad->input_pwd,
			//"0000", 0, 0);
			int result = auth_passwd_set_passwd(AUTH_PWD_NORMAL,
					ad->input_pwd, "0000");
			SETTING_TRACE_DEBUG("set_pwd result : %d", result);
#endif

#if 0
			uid_t user = 5000;
			int ckmc_ret = CKMC_ERROR_NONE;
			ckmc_ret = ckmc_change_user_password(user,
					ad->input_pwd, NULL);
			SETTING_TRACE("ckmc_change_user_password() returns %d",
					ckmc_ret);
#endif
		}
		if (vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
				lock_type) == 0) {
			/* set radio */
			elm_radio_value_set(list_item->eo_check, lock_type);
		}
		/*SETTING_TRACE_DEBUG("PASS #1"); */
		ug_destroy_me(ad->ug);
		break;
#if 0
		case SETTING_SCREEN_LOCK_TYPE_MOTION:
		ad->old_type = old_type;
		setting_create_guild_layout(ad->navi_bar,
				_(About_Motion_Unlock_Str),
				_("IDS_ST_BUTTON_CANCEL_ABB"),
				_(SET_AS_LOCK_STR), NULL,
				__motion_cancel_cb, __motion_set_cb, NULL,
				_("IDS_LCKSCN_BODY_TAP_AND_HOLD_THE_SCREEN_THEN_TILT_THE_DEVICE_TOWARDS_YOU_TO_UNLOCK_IT_MSG"),
				(char **)motion_img,
				NULL,
				NULL,
				NULL, ad);
		break;
#endif
	case SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD:
		if (SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD == old_type) {
			__change_simple_password_cb(list_item, NULL, NULL);
		} else {
			ad->pw_type = SETTING_LOCKTYPE_PW_SIMPLE_PASSWD;
			if (old_type != SETTING_SCREEN_LOCK_TYPE_PASSWORD) {
				FREE(ad->input_pwd);
				ad->input_pwd = (char *) strdup("0000");
			}
			setting_locktype_create_password_sg(ad);
		}
		break;
	case SETTING_SCREEN_LOCK_TYPE_PASSWORD:
		if (SETTING_SCREEN_LOCK_TYPE_PASSWORD == old_type) {
			__change_password_cb(list_item, NULL, NULL);
		} else {
			ad->pw_type = SETTING_LOCKTYPE_PW_PASSWORD;
			if (old_type != SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
				FREE(ad->input_pwd);
				ad->input_pwd = (char *) strdup("0000");
			}
			setting_locktype_create_password_sg(ad);
		}
		break;
	case SETTING_SCREEN_LOCK_TYPE_OTHER:
		if (old_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD
				|| old_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
#if SECURITY_SERVER
			//int result = security_server_set_pwd(ad->input_pwd,
			//"0000", 0, 0);
			int result = auth_passwd_set_passwd(AUTH_PWD_NORMAL,
					ad->input_pwd, "0000");
			SETTING_TRACE_DEBUG("set_pwd result : %d", result);
#endif
#if 0
			uid_t user = 5000;
			int ckmc_ret = CKMC_ERROR_NONE;
			ckmc_ret = ckmc_change_user_password(user,
					ad->input_pwd, NULL);
			SETTING_TRACE("ckmc_change_user_password() returns %d",
					ckmc_ret);
#endif
		}
		index = __get_lockapp_index_from_appname(list_item->keyStr);
		pkg_name = __get_lockapp_pkgname_from_appname(
				list_item->keyStr);
		SETTING_TRACE_DEBUG("3rd lock selected. index[%d] pkg_name[%s]",
				index, pkg_name);
		vconf_set_str(VCONFKEY_SETAPPL_3RD_LOCK_PKG_NAME_STR, pkg_name);
		vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type);
		/* set radio */
		elm_radio_value_set(list_item->eo_check, lock_type + index);
		ug_destroy_me(ad->ug);
		break;
	default:
		break;
	}
}

#if 0
static void
setting_locktype_main_click_radio_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	(Setting_GenGroupItem_Data *) data;
	SettingLocktypeUG *ad = list_item->userdata;

	int lock_type = elm_radio_value_get(obj);
	int old_type = 0;

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &old_type);

	if (old_type == lock_type)
	return;

	switch (lock_type) {
		case SETTING_SCREEN_LOCK_TYPE_NONE:
		case SETTING_SCREEN_LOCK_TYPE_SWIPE:
		/* To do : Call security-server API. pw : 0000 */
		if (old_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD
				|| old_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
#if SECURITY_SERVER
//			int result = security_server_set_pwd(
//			ad->input_pwd, "0000", 0, 0);
			int result = auth_passwd_set_passwd(
					AUTH_PWD_NORMAL,ad->input_pwd, "0000");
			SETTING_TRACE_DEBUG("set_pwd result : %d", result);
#endif
#if 0
			uid_t user = 5000;
			int ckmc_ret = CKMC_ERROR_NONE;
			ckmc_ret = ckmc_change_user_password(user,
					ad->input_pwd, NULL);
			SETTING_TRACE("ckmc_change_user_password() returns %d",
					ckmc_ret);
#endif
		}
		vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, lock_type);
		ug_destroy_me(ad->ug);
		break;
#if 0
		case SETTING_SCREEN_LOCK_TYPE_MOTION:
		ad->old_type = old_type;
		elm_radio_value_set(obj, old_type);
		setting_create_guild_layout(ad->navi_bar,
				_(About_Motion_Unlock_Str),
				_("IDS_ST_BUTTON_CANCEL_ABB"),
				_(SET_AS_LOCK_STR), NULL,
				__motion_cancel_cb, __motion_set_cb, NULL,
				_("IDS_LCKSCN_BODY_TAP_AND_HOLD_THE_SCREEN_THEN_TILT_THE_DEVICE_TOWARDS_YOU_TO_UNLOCK_IT_MSG"),
				(char **)motion_img,
				NULL,
				NULL,
				NULL, ad);
		break;
#endif
		case SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD:
		elm_radio_value_set(obj, old_type);
		ad->pw_type = SETTING_SEC_PW_SIMPLE_PASSWD;
		if (old_type != SETTING_SCREEN_LOCK_TYPE_PASSWORD) {
			FREE(ad->input_pwd);
			ad->input_pwd = (char *)strdup("0000");
		}
		setting_locktype_create_password_sg(ad);
		break;
		case SETTING_SCREEN_LOCK_TYPE_PASSWORD:
		elm_radio_value_set(obj, old_type);
		ad->pw_type = SETTING_SEC_PW_PASSWORD;
		if (old_type != SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
			FREE(ad->input_pwd);
			ad->input_pwd = (char *)strdup("0000");
		}
		setting_locktype_create_password_sg(ad);
		break;
		case SETTING_SCREEN_LOCK_TYPE_OTHER: {
			int index = -1;
			char *pkg_name = NULL;
			if (old_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD
					|| old_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD) {
#if SECURITY_SERVER
//				int result = security_server_set_pwd(
//				ad->input_pwd, "0000", 0, 0);
				int result = auth_passwd_set_passwd(
						AUTH_PWD_NORMAL,ad->input_pwd,
						"0000");
				SETTING_TRACE_DEBUG("set_pwd result : %d",
						result);
#endif
#if 0
				uid_t user = 5000;
				int ckmc_ret = CKMC_ERROR_NONE;
				ckmc_ret = ckmc_change_user_password(user,
						ad->input_pwd, NULL);
				SETTING_TRACE("ckmc_change_user_password() returns %d",
						ckmc_ret);
#endif
			}
			index = __get_lockapp_index_from_appname(
					list_item->keyStr);
			pkg_name = __get_lockapp_pkgname_from_appname(
					list_item->keyStr);
			SETTING_TRACE_DEBUG(
					"3rd lock selected. index[%d] pkg_name[%s]",
					index, pkg_name);
			vconf_set_str(VCONFKEY_SETAPPL_3RD_LOCK_PKG_NAME_STR,
					pkg_name);
			vconf_set_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT,
					lock_type);
			/* set radio */
			elm_radio_value_set(list_item->eo_check,
					lock_type + index);
			ug_destroy_me(ad->ug);
		}
		break;
		default:
		break;
	}
}
#endif

Eina_Bool setting_locktype_main_click_softkey_back_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retvm_if(data == NULL, EINA_FALSE,
			"[Setting > Security] Data parameter is NULL");

	SettingLocktypeUG *ad = (SettingLocktypeUG *) data;
	if (ad->ug_passwd) {
		return EINA_FALSE;
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_TRUE;
}
