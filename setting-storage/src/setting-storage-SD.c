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
#include <glib.h>
#include <sys/mount.h>
#if SUPPORT_ENCRYPTION
#include <ode.h>
#endif
#include "setting-storage-utils.h"
#include "setting-storage-SD.h"

typedef void (*storageUg_mmc_cb)(int result, void *data);

enum {
    STORAGEUG_SD_REQ_NONE,
    STORAGEUG_SD_REQ_MOUNT,
    STORAGEUG_SD_REQ_UNMOUNT,
    STORAGEUG_SD_REQ_FORMAT
};

static void storageUg_SD_finish_progress(int result, void *data)
{
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (result < 0) {
		SETTING_TRACE_ERROR("result(%d) Invalid", result);
		storageUg_fail_popup(ad);
	}
	ad->sd_request = STORAGEUG_SD_REQ_NONE;
}

static void storageUg_SD_unmount(SettingStorageUG *ad, storageUg_mmc_cb cb)
{
	int ret;

	ret_if(ad == NULL);

	ad->popup = setting_create_popup_with_progressbar(ad, ad->win, PROGRESSBAR_STYLE,
	                                                  NULL, STORAGEUG_STR_UNMOUNTING_SD, storageUg_popup_del, 0, TRUE, TRUE);

	ad->mmc_content.mmc_cb = cb;
	ad->mmc_content.user_data = ad;
	ret = deviced_request_unmount_mmc(&ad->mmc_content, MNT_FORCE);
	if (ret == -1) {
		SETTING_TRACE_ERROR("deviced_request_unmount_mmc() Fail");
		storageUg_fail_popup(ad);
	}
	ad->sd_request = STORAGEUG_SD_REQ_UNMOUNT;
}

static void storageUg_SD_unmount_resp(void *data, Evas_Object *obj,
                                      void *event_info)
{
	int response_type;
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK == response_type)
		storageUg_SD_unmount(ad, storageUg_SD_finish_progress);
}

static void storageUg_SD_handle_mount_unmount(void *data)
{
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	SETTING_TRACE("mmc_status = %d", ad->mmc_status);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (VCONFKEY_SYSMAN_MMC_MOUNTED == ad->mmc_status) {
		ad->popup = setting_create_popup_with_btn(ad, ad->win, NULL,
		                                          STORAGEUG_STR_SD_UNMOUNT_POPUP_MSG, storageUg_SD_unmount_resp, 0,
		                                          2, STORAGEUG_STR_OK, STORAGEUG_STR_CANCEL);
	} else {
		int ret;

		ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
		                                                  PROGRESSBAR_STYLE, NULL, NULL, storageUg_popup_del, 0, TRUE, TRUE);

		ad->mmc_content.mmc_cb = storageUg_SD_finish_progress;
		ad->mmc_content.user_data = ad;
		ret = deviced_request_mount_mmc(&ad->mmc_content);
		/*  if fail, popup failed info */
		if (ret == -1) {
			SETTING_TRACE_ERROR("deviced_request_mount_mmc() Fail");
			storageUg_fail_popup(ad);
			return;
		}
		ad->sd_request = STORAGEUG_SD_REQ_MOUNT;
	}
}

static void storageUg_SD_finish_format(int result, void *data)
{
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	ad->sd_request = STORAGEUG_SD_REQ_NONE;

	if (result < 0) {
		SETTING_TRACE_ERROR("result(%d) Invalid", result);
		storageUg_fail_popup(ad);
		return;
	}
}

static void storageUg_SD_format(int result, void *data)
{
	int ret;
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (result < 0) {
		SETTING_TRACE_ERROR("result(%d) Invalid", result);
		storageUg_fail_popup(ad);
		ad->sd_request = STORAGEUG_SD_REQ_NONE;
		return;
	}

	ad->popup = setting_create_popup_with_progressbar(ad, ad->win, PROGRESSBAR_STYLE,
	                                                  NULL, STORAGEUG_STR_FORMATTING_SD, NULL, 0, TRUE, TRUE);

	ad->mmc_content.mmc_cb = storageUg_SD_finish_format;
	ad->mmc_content.user_data = ad;
	ret = deviced_request_format_mmc(&ad->mmc_content);
	if (ret == -1) {
		SETTING_TRACE_ERROR("fail to call deviced_request_format_mmc");
		storageUg_fail_popup(ad);
	}
	ad->sd_request = STORAGEUG_SD_REQ_FORMAT;
}


static void storageUg_SD_format_se_confirm_resp(void *data, Evas_Object *obj,
                                                void *event_info)
{
	int response_type;
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK == response_type)
		storageUg_SD_unmount(ad, storageUg_SD_format);
}


static void storageUg_SD_format_se_confirm(SettingStorageUG *ad)
{
	if (ad->popup)
		evas_object_del(ad->popup);

	ad->popup = setting_create_popup_with_btn(ad, ad->win, NULL,
	                                          STORAGEUG_STR_FORMAT_SECOND_Q, storageUg_SD_format_se_confirm_resp, 0,
	                                          2, STORAGEUG_STR_OK, STORAGEUG_STR_CANCEL);

}

static void storageUg_passwd_ug_result_cb(ui_gadget_h ug, app_control_h service,
                                          void *priv)
{
	char *result = NULL;
	SettingStorageUG *ad = priv;

	ret_if(NULL == priv);

	app_control_get_extra_data(service, "result", &result);
	if (safeStrCmp(result, "SETTING_PW_TYPE_ENTER_LOCK_TYPE") == 0)
		storageUg_SD_format_se_confirm(ad);
	else
		storageUg_fail_popup(ad);

	FREE(result);

	elm_object_tree_focus_allow_set(ad->lo_main, EINA_TRUE);
	setting_ug_destroy(ug);
}

static inline void storageUg_passwd_ug(SettingStorageUG *ad)
{
	app_control_h svc;
	ui_gadget_h ug;
	struct ug_cbs cbs;

	ret_if(NULL == ad);

	if (app_control_create(&svc))
		return;

	app_control_add_extra_data(svc, "viewtype", "SETTING_PW_TYPE_ENTER_LOCK_TYPE");

	memset(&cbs, 0, sizeof(struct ug_cbs));
	cbs.layout_cb = storageUg_ug_layout_cb;
	cbs.result_cb = storageUg_passwd_ug_result_cb;
	cbs.destroy_cb = storageUg_ug_destroy_cb;
	cbs.priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->lo_main, EINA_FALSE);
	ug = setting_ug_create(ad->ug, "setting-password-efl", UG_MODE_FULLVIEW, svc, &cbs);
	warn_if(NULL == ug, "setting_ug_create() Fail");

	app_control_destroy(svc);
}

static inline int storageUg_checkencrypt()
{
	int sde_status = 1;
#if SUPPORT_ENCRYPTION
	if (ode_init() == 0) {
		sde_status = sde_checkencrypt();
		ode_deinit();
	}
#endif
	return !sde_status;
}

static inline void storageUg_SD_prepare_format(SettingStorageUG *ad)
{
	int is_encrypt;

	is_encrypt = storageUg_checkencrypt();
	if (is_encrypt)
		storageUg_passwd_ug(ad);
	else
		storageUg_SD_format_se_confirm(ad);
}

static void storageUg_SD_format_first_confirm_resp(void *data,
                                                   Evas_Object *obj, void *event_info)
{
	int response_type;
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	response_type = btn_type(obj);
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (POPUP_RESPONSE_OK == response_type) {
		storageUg_SD_prepare_format(ad);
	}

}


static inline void storageUg_SD_handle_format(SettingStorageUG *ad)
{
	char *popup_msg;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	popup_msg = STORAGEUG_STR_FORMAT_USE_MSG;

	ad->popup = setting_create_popup_with_btn(ad, ad->win, NULL, popup_msg,
	                                          storageUg_SD_format_first_confirm_resp, 0,
	                                          2, STORAGEUG_STR_OK, STORAGEUG_STR_CANCEL);
}

static void storageUg_SD_sel(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;
	Elm_Object_Item *item = event_info;

	ret_if(NULL == data);
	ret_if(NULL == event_info);

	elm_genlist_item_selected_set(item, 0);

	if (item == ad->sd_mount->item)
		storageUg_SD_handle_mount_unmount(ad);
	else if (item == ad->sd_format->item)
		storageUg_SD_handle_format(ad);
	else
		SETTING_TRACE_ERROR("item(%p) Invalid", item);
}

static Setting_GenGroupItem_Data *storageUg_SD_gl_insert_after(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    Elm_Object_Item *before,
    setting_call_back_func gl_sel,
    void *sel_data,
    const char *keyStr,
    char *sub_desc,
    setting_group_style group_style,
    SettingStorageUG *ad)
{
	Setting_GenGroupItem_Data *it_data = calloc(1, sizeof(Setting_GenGroupItem_Data));
	retvm_if(NULL == it_data, NULL, "calloc failed");

	it_data->keyStr = (char *)g_strdup(keyStr);
	it_data->sub_desc = (char *)g_strdup(sub_desc);
	it_data->swallow_type = SWALLOW_Type_INVALID;
	it_data->group_style = group_style;
	it_data->userdata = ad;

	it_data->item = elm_genlist_item_insert_after(genlist, itc, it_data, NULL, before,
	                                              ELM_GENLIST_ITEM_NONE, gl_sel, sel_data);

	return it_data;
}

static inline void storageUg_SD_info_removed(SettingStorageUG *ad)
{

	ad->sd_mount = storageUg_SD_gl_insert_after(ad->gl_main, &itc_2text_2,
	                                            ad->sd_card->item, NULL, NULL, STORAGEUG_STR_MOUNT_SD, STORAGEUG_STR_INSERT,
	                                            SETTING_GROUP_STYLE_NONE, NULL);
	if (ad->sd_mount) {
		elm_object_item_disabled_set(ad->sd_mount->item, EINA_TRUE);
	} else {
		SETTING_TRACE_ERROR("ad->sd_mount is NULL");
	}
}

static inline void storageUg_SD_info_inserted_not_mounted(SettingStorageUG *ad)
{
	ad->sd_mount = storageUg_SD_gl_insert_after(ad->gl_main, &itc_1text,
	                                            ad->sd_card->item, storageUg_SD_sel, ad, STORAGEUG_STR_MOUNT_SD, NULL,
	                                            SETTING_GROUP_STYLE_TOP, ad);
	warn_if(NULL == ad->sd_mount, "ad->sd_mount is NULL");

	ad->sd_format = storageUg_SD_gl_insert_after(ad->gl_main, &itc_1text,
	                                             ad->sd_mount->item, storageUg_SD_sel, ad, STORAGEUG_STR_FORMAT_SD, NULL,
	                                             SETTING_GROUP_STYLE_BOTTOM, ad);
	if (ad->sd_format) {
		int status = -1;

		vconf_get_int(VCONFKEY_SYSMAN_MMC_MOUNT, &status);
		if (VCONFKEY_SYSMAN_MMC_MOUNT_COMPLETED == status
		    || VCONFKEY_SYSMAN_MMC_MOUNT_ALREADY == status) {
			setting_disable_genlist_item(ad->sd_format->item);
		}
	} else {
		SETTING_TRACE_ERROR("ad->sd_format is NULL");
	}
}

static inline void storageUg_SD_info_mounted(SettingStorageUG *ad)
{
	double total = 0.0;
	double avail = 0.0;
	char total_str[STORAGEUG_MAX_STR_LEN] = {0};
	char avail_str[STORAGEUG_MAX_STR_LEN] = {0};
	const char *MMC_path = "/opt/storage/sdcard";

	storageUg_get_external_storage_status(MMC_path, &total, &avail);
	storageUg_size_to_str(total, total_str, sizeof(total_str));
	storageUg_size_to_str(avail, avail_str, sizeof(avail_str));

	ad->sd_total = storageUg_SD_gl_insert_after(ad->gl_main, &itc_2text_2,
	                                            ad->sd_card->item, NULL, ad, STORAGEUG_STR_TOTAL, total_str,
	                                            SETTING_GROUP_STYLE_TOP, ad);
	if (ad->sd_total) {
		elm_genlist_item_select_mode_set(ad->sd_total->item,
		                                 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		ad->sd_avail = storageUg_SD_gl_insert_after(ad->gl_main, &itc_2text_2,
		                                            ad->sd_total->item, NULL, ad, STORAGEUG_STR_AVAIL_SPACE, avail_str,
		                                            SETTING_GROUP_STYLE_CENTER, ad);
		if (ad->sd_avail) {
			elm_genlist_item_select_mode_set(ad->sd_avail->item,
			                                 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
			ad->sd_mount = storageUg_SD_gl_insert_after(ad->gl_main, &itc_1text,
			                                            ad->sd_avail->item, storageUg_SD_sel, ad, STORAGEUG_STR_UNMOUNT_SD, NULL,
			                                            SETTING_GROUP_STYLE_CENTER, ad);
			warn_if(NULL == ad->sd_mount, "ad->sd_mount is NULL");
		} else {
			SETTING_TRACE_ERROR("ad->sd_avail is NULL");
		}
		ad->sd_format = storageUg_SD_gl_insert_after(ad->gl_main, &itc_1text,
		                                             ad->sd_mount->item, storageUg_SD_sel, ad, STORAGEUG_STR_FORMAT_SD, NULL,
		                                             SETTING_GROUP_STYLE_BOTTOM, ad);
		warn_if(NULL == ad->sd_format, "ad->sd_format is NULL");

	} else {
		SETTING_TRACE_ERROR("ad->sd_total is NULL");
	}
}

static inline void storageUg_SD_remove_info(SettingStorageUG *ad)
{
	if (ad->sd_mount) {
		elm_object_item_del(ad->sd_mount->item);
		ad->sd_mount = NULL;
	}
	if (ad->sd_total) {
		elm_object_item_del(ad->sd_total->item);
		ad->sd_total = NULL;
	}
	if (ad->sd_avail) {
		elm_object_item_del(ad->sd_avail->item);
		ad->sd_avail = NULL;
	}
	if (ad->sd_format) {
		elm_object_item_del(ad->sd_format->item);
		ad->sd_format = NULL;
	}
}

void storageUg_main_append_SD_info(SettingStorageUG *ad)
{
	char *str;
	int mmc_mode;

	ret_if(ad == NULL);

	mmc_mode = ad->mmc_status;

	SETTING_TRACE_DEBUG("mmc_mode: %d", mmc_mode);

	storageUg_SD_remove_info(ad);

	if (-1 == mmc_mode)
		mmc_mode = VCONFKEY_SYSMAN_MMC_REMOVED;

	if (VCONFKEY_SYSMAN_MMC_REMOVED == mmc_mode)
		storageUg_SD_info_removed(ad);
	else if (VCONFKEY_SYSMAN_MMC_INSERTED_NOT_MOUNTED == mmc_mode)
		storageUg_SD_info_inserted_not_mounted(ad);
	else if (VCONFKEY_SYSMAN_MMC_MOUNTED == mmc_mode)
		storageUg_SD_info_mounted(ad);
	else
		SETTING_TRACE_ERROR("mmc_mode(%d) Invalid", mmc_mode);

#if 0
	str = vconf_get_str(storageUg_ENCRYPT_stat);
	warn_if(NULL == str, "vconf_get_str(%s) Fail", storageUg_ENCRYPT_stat);
	if (!safeStrCmp(str, "encryption_start") || !safeStrCmp(str, "decryption_start")) {
		if (ad->sd_mount) setting_disable_genlist_item(ad->sd_mount->item);
		if (ad->sd_format) setting_disable_genlist_item(ad->sd_format->item);
	}
#endif
}

void storageUg_SD_change_cb(keynode_t *node, void *user_data)
{
	SettingStorageUG *ad = user_data;

	ret_if(NULL == user_data);

	ad->mmc_status = vconf_keynode_get_int(node);

	storageUg_main_append_SD_info(ad);
}
