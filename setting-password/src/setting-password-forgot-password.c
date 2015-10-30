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

#include <sys/socket.h>
#include <sys/un.h>
#include <ode.h>

#include <setting-password.h>
#include <setting-password-forgot-password.h>

static int setting_password_forgot_password_create(void *cb);
static int setting_password_forgot_password_destroy(void *cb);
static int setting_password_forgot_password_update(void *cb);
static int setting_password_forgot_password_cleanup(void *cb);

setting_view setting_view_password_forgot_password = {
	.create = setting_password_forgot_password_create,
	.destroy = setting_password_forgot_password_destroy,
	.update = setting_password_forgot_password_update,
	.cleanup = setting_password_forgot_password_cleanup,
};

void __reset_personal_popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);

	SettingPasswordUG *ad = (SettingPasswordUG *)data;

	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	if (!safeStrCmp(btn_str, _(PW_ST_RESET))) {
		int ret = 0;
		/* call ode API */
		if (ode_init() == 0) {
			ret = pse_restore();
			SETTING_TRACE("pse_restore() returns %d", ret);

			/* initialize vconf */
			vconf_set_int(VCONFKEY_SETAPPL_PERSONAL_MODE_UNLOCK_METHOD_INT, -1);
			vconf_set_bool(VCONFKEY_SETAPPL_PERSONAL_MODE_STATUS_BOOL, 0);

			ode_deinit();
			/* send resunt to personal mode ug */
			app_control_h svc;
			if (!app_control_create(&svc)) {
				app_control_add_extra_data(svc, "result", "Restore");
				ug_send_result(ad->ug, svc);
				app_control_destroy(svc);
			}
			ug_destroy_me(ad->ug);
		}
	}

	if (ad->reset_personal_popup) {
		evas_object_del(ad->reset_personal_popup);
		ad->reset_personal_popup = NULL;
	}
}

static void __launch_lockscreen(void)
{
	SETTING_TRACE_BEGIN;
	struct sockaddr_un saddr;
	int fd;
	int retry = 1;

	fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (fd < 0) {
		fd = socket(AF_UNIX, SOCK_STREAM, 0);
		if (fd < 0) {
			SETTING_TRACE("socket() failed");
			return;
		}
	}
	bzero(&saddr, sizeof(saddr));
	saddr.sun_family = AF_UNIX;
	strncpy(saddr.sun_path, "/tmp/phlock", strlen("/tmp/phlock"));
	saddr.sun_path[strlen("/tmp/phlock")] = 0;

	SETTING_TRACE("saddr.sun_path = %s", saddr.sun_path);
retry_con:
	if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
		if (retry > 0) {
			usleep(100 * 1000);
			retry--;
			goto retry_con;
		}
		close(fd);
		SETTING_TRACE("connect() failed");
		return;
	}
	char *cmd = "launch_phone_lock";
	int len = strlen(cmd);
	send(fd, cmd, len, 0);
}

static void setting_password_forgot_password_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPasswordUG *ad = (SettingPasswordUG *)data;

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SETTING_TRACE("%s clicked", list_item->keyStr);
	if (!safeStrCmp(list_item->keyStr, PW_ST_RESET_PERSONAL_MODE)) {
		/* if lock type is simple password or password, call lockscreen. */
		int screen_lock_type = 0;
		vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &screen_lock_type);
		if (screen_lock_type == SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD
		    || screen_lock_type == SETTING_SCREEN_LOCK_TYPE_PASSWORD) {
			/* call lockscreen app */
			__launch_lockscreen();
			/*int ret = 0; */
			/*ret = aul_launch_app("org.tizen.lockscreen", NULL); */
			/*SETTING_TRACE("aul_launch_app() returns %d", ret); */
		}

		/* draw popup */
		ad->reset_personal_popup = setting_create_popup(ad, ad->win_get,
														PW_ST_RESET_PERSONAL_MODE,
														PW_ST_RESET_PERSONAL_MODE_POPUP_Q,
														__reset_personal_popup_resp_cb, 0, FALSE, FALSE,
														2, PW_ST_RESET, PW_ST_CANCEL);
	}
}

static Eina_Bool __forgot_password_pop_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;

	setting_view_change(&setting_view_password_forgot_password, &setting_view_password_main, data);

	return EINA_TRUE;
}

static int setting_password_forgot_password_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	Evas_Object *genlist = NULL;
	Elm_Object_Item *navi_it = NULL;

	navi_it = setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
	                                               PW_ST_FORGOT_PASSWORD,
	                                               NULL, NULL, NULL, NULL, NULL,
	                                               &genlist, ad->navi_bar);
	/* add pop cb */
	elm_naviframe_item_pop_cb_set(navi_it, (Elm_Naviframe_Item_Pop_Cb)__forgot_password_pop_cb, ad);

	if (genlist) {
		Elm_Object_Item *item = elm_genlist_item_append(genlist,
		                                                &itc_seperator, NULL, NULL,
		                                                ELM_GENLIST_ITEM_NONE, NULL, NULL);
		if (item)
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

		/* menu 1. */
		setting_create_Gendial_field_def(genlist, &(itc_1text),
		                                 setting_password_forgot_password_mouse_up_Gendial_list_cb, ad,
		                                 SWALLOW_Type_INVALID,
		                                 NULL, NULL,
		                                 0, PW_ST_RESET_PERSONAL_MODE, NULL, NULL);

		/* menu 2. */
		ADD_GL_HELP(genlist, PW_ST_RESET_PERSONAL_MODE_HELP);
	}


	setting_view_password_forgot_password.is_create = 1;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_forgot_password_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_view_password_forgot_password.is_create = 0;

	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_password_forgot_password_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPasswordUG *ad = (SettingPasswordUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_password_forgot_password_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return setting_password_forgot_password_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

