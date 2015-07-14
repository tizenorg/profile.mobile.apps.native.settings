/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <setting-memory-status.h>
#define MAX_STR_LEN_ON_MEMORY_STATUS	32

#define MAIN_STORAGE 0

static int setting_memory_status_create(void *cb);
static int setting_memory_status_destroy(void *cb);
static int setting_memory_status_update(void *cb);
static int setting_memory_status_cleanup(void *cb);

setting_view setting_view_memory_status = {
	.create = setting_memory_status_create,
	.destroy = setting_memory_status_destroy,
	.update = setting_memory_status_update,
	.cleanup = setting_memory_status_cleanup,
};

static void __setting_memory_status_vconf_change_cb( keynode_t *key, void *data);
static int __setting_memory_status_check_mmc_status();
static int __setting_memory_status_mmc_content_init(void *data, setting_memory_mmc_cb cb);

static void __setting_memory_status_system_storage_available_size_get(char *szAvail)
{
	SETTING_TRACE_BEGIN;

	double dTotal = 0.0;
	double dAvail = 0.0;
	int ret = SETTING_RETURN_SUCCESS;
	setting_mem_status internal_mem_stat;

	ret = __fs_stat(&dTotal, &dAvail, SETTING_MEMORY_STATUS_APP_PATH);
	setting_retm_if(ret == SETTING_RETURN_FAIL, "get app_mem stat failed");
	internal_mem_stat.dTotal = dTotal;
	internal_mem_stat.dAvail = dAvail;

	ret = setting_memory_status_size_transition(internal_mem_stat.dAvail, szAvail);
	setting_retm_if(ret == SETTING_RETURN_FAIL, "setting_memory_status_size_transition fail");
}

static int setting_memory_status_create(void *cb)
{
	SETTING_TRACE_BEGIN;

	SettingMemoryUG *ad = (SettingMemoryUG *) cb;
	Evas_Object *scroller;

	/* Evas_Object *user_mem_fr, *phone_mem_fr, *fr_bx1, *fr_bx2; */
	/* Evas_Object *sp; */
	/* float tmp_free_size, tmp_total_size; */
	/* char free_str[MAX_STR_LEN_ON_MEMORY_STATUS] = {0, }; */
	/* char total_str[MAX_STR_LEN_ON_MEMORY_STATUS] = {0, }; */
	/* char size_str[MAX_STR_LEN_ON_MEMORY_STATUS*2] = {0, }; */
	setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
					     _("IDS_ST_BODY_MEMORY_STATUS"),
					     S_("IDS_COM_BODY_BACK"), NULL,
					     setting_memory_status_click_softkey_back,
					     NULL, ad, &scroller, ad->navi_bar);

	/*  External SD card */
	setting_create_Gendial_field_titleItem(scroller, &(ad->itc_group_item),
					       SETTING_MEMORY_STATUS_EX_SD_CARD_DESC,
					       NULL);

	char ex_total_space[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = { 0, };
	char ex_available_space[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = { 0, };
	char ex_mount_status[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = { 0, };
	int ex_mmc_status = VCONFKEY_SYSMAN_MMC_REMOVED;
	setting_memory_status_ex_sd_card_info(ex_total_space,
					      ex_available_space,
					      ex_mount_status, &ex_mmc_status);

	ad->data_ex_total =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2), NULL,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, ex_total_space,
					     SETTING_MEMORY_STATUS_TOTAL_SPACE_DESC,
					     NULL);
	if (ad->data_ex_total) {
		ad->data_ex_total->userdata = ad;
		elm_genlist_item_select_mode_set(ad->data_ex_total->item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->data_ex_total is NULL");
	}

	ad->data_ex_available =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2), NULL,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, ex_available_space,
					     SETTING_MEMORY_STATUS_AVAILABLE_SPACE_DESC,
					     NULL);
	if (ad->data_ex_available) {
		ad->data_ex_available->userdata = ad;
		elm_genlist_item_select_mode_set(ad->data_ex_available->item,
					  ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->data_ex_available is NULL");
	}

	if(!isEmulBin())
	{
		ad->data_ex_mount =
					setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					     setting_memory_status_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, ex_mount_status, NULL,
					     NULL);
		if (ad->data_ex_mount) {
			ad->data_ex_mount->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("ad->data_ex_mount is NULL");
		}

		ad->data_ex_format =
					setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					     setting_memory_status_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0,
					     SETTING_MEMORY_STATUS_FORMAT_SD_CARD_DESC,
					     NULL, NULL);
		if (ad->data_ex_format) {
			ad->data_ex_format->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("ad->data_ex_format is NULL");
		}

		setting_memory_status_list_item_enable_set_by_mmc_status(ad, ex_mmc_status);
	}

	/* System storage */
	setting_create_Gendial_field_titleItem(scroller, &(ad->itc_group_item),
					       SETTING_MEMORY_STATUS_SYSTEM_SPACE_DESC,
					       NULL);
	char system_storage_available[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0,};
	__setting_memory_status_system_storage_available_size_get(system_storage_available);
	ad->data_system_storage_available =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2), NULL,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, system_storage_available,
					     SETTING_MEMORY_STATUS_AVAILABLE_SPACE_DESC,
					     NULL);
	if (ad->data_system_storage_available) {
		ad->data_system_storage_available->userdata = ad;
		elm_genlist_item_select_mode_set(ad->data_system_storage_available->
					  item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->data_system_storage_available is NULL");
	}

#if MAIN_STORAGE
	/*  Internal phone storage */
	setting_create_Gendial_field_titleItem(scroller, &(ad->itc_group_item),
					       SETTING_MEMORY_STATUS_MAIN_STORAGE_DESC,
					       NULL);

	char phone_storage_available[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0,};
	setting_memory_status_get_phone_storage_available_size(ad,
							       phone_storage_available);
	ad->data_phone_storage_available =
	    setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2), NULL,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, phone_storage_available,
					     SETTING_MEMORY_STATUS_AVAILABLE_SPACE_DESC,
					     NULL);
	if (ad->data_phone_storage_available) {
		ad->data_phone_storage_available->userdata = ad;
		elm_genlist_item_select_mode_set(ad->data_phone_storage_available->
					  item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->data_phone_storage_available is NULL");
	}
#endif

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			__setting_memory_status_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	setting_view_memory_status.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_memory_status_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = (SettingMemoryUG *) cb;

	if (ad->progress_bar) {
		evas_object_del(ad->progress_bar);
		ad->progress_bar = NULL;
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	int ret = vconf_ignore_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			__setting_memory_status_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	if (ad->mmc_content) {
		FREE(ad->mmc_content);
	}

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_memory_status.is_create = 0;

	return 0;
}

static int setting_memory_status_update(void *cb)
{
	SETTING_TRACE_BEGIN;

	SettingMemoryUG *ad = (SettingMemoryUG *) cb;
	if (!ad) {
		SETTING_TRACE_ERROR("ad is null.");
	}
	// update sd card statistic
	if (ad)
		setting_memory_status_ex_sd_card_refresh_card_details_cb(ad, NULL, NULL);

	// update internal phone storage statistic
	char phone_storage_available[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0,};
	setting_memory_status_get_phone_storage_available_size(ad, phone_storage_available);

#if MAIN_STORAGE
	if (ad->data_phone_storage_available)
		setting_memory_status_list_item_data_set(ad->data_phone_storage_available, phone_storage_available);
#endif
	SETTING_TRACE_END;
	return 0;
}

static int setting_memory_status_cleanup(void *cb)
{
	return setting_memory_status_destroy(cb);
}

/*******************************************
 *
 *general function
 *
 *******************************************/

static int __fs_stat(double* pdTotal, double* pdAvail, const char* szPath)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == pdTotal || NULL == pdAvail,
		SETTING_RETURN_FAIL, "Null output parameters");

	struct statvfs s;

	if (!statvfs(szPath, &s)) {
		SETTING_TRACE("f_bsize = %ld f_blocks = %ld f_bavail = %ld f_frsize = %ld",
		     s.f_bsize, s.f_blocks, s.f_bavail, s.f_frsize);
		*pdTotal = (double)s.f_frsize * s.f_blocks;
		*pdAvail = (double)s.f_bsize * s.f_bavail;
	} else {
		return SETTING_RETURN_FAIL;
	}

	return SETTING_RETURN_SUCCESS;
}

/*******************************************
 *
 *callback function
 *
 *******************************************/

static void setting_memory_status_click_softkey_back(void *data,
						     Evas_Object *obj,
						     void *event_info)
{
	SettingMemoryUG *ad = (SettingMemoryUG *) data;
	setting_view_change(&setting_view_memory_status,
			    &setting_view_memory_main, ad);
}

static void
setting_memory_status_ex_sd_card_info(char* szTotal, char* szAvail,
				      char* ex_mount_desc, int *ex_mmc_status)
{
	int mmc_mode = VCONFKEY_SYSMAN_MMC_REMOVED;
	int ret = vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &mmc_mode);
	setting_retm_if(ret != 0, "Get VCONFKEY_SYSMAN_MMC_STATUS failed");
	*ex_mmc_status = mmc_mode;
	SETTING_TRACE("mmc_mode = %d", mmc_mode);

	int mmc_status = __setting_memory_status_check_mmc_status();
	setting_retm_if(mmc_status == -1, "check mmc status failed");

	/* Does not mounted, same st_dev */
	if (mmc_status == 0) {
		snprintf(szTotal, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN,
			 "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		snprintf(szAvail, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN,
			 "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		snprintf(ex_mount_desc, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN,
			 "%s", SETTING_MEMORY_STATUS_MOUNT_SD_CARD_DESC);
	}
	/*  MMC mounted, diff st_dev */
	else {
		snprintf(ex_mount_desc, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN,
			 "%s", SETTING_MEMORY_STATUS_UNMOUNT_SD_CARD_DESC);
		ret =
		    setting_memory_status_ex_sd_card_detail(szTotal, szAvail);
		setting_retm_if(ret == SETTING_RETURN_FAIL,
				"get sd card detail failed");
	}
}

static int
setting_memory_status_ex_sd_card_detail(char* szTotal, char* szAvail)
{
	SETTING_TRACE_BEGIN;
	double dTotal = 0.0;
	double dAvail = 0.0;

	int ret = SETTING_RETURN_SUCCESS;
	ret = __fs_stat(&dTotal, &dAvail, SETTING_MEMORY_STATUS_MMC_PATH);
	setting_retvm_if(ret == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL,
			 "get sd card size detail failed");

	ret = setting_memory_status_size_transition(dTotal, szTotal);
	setting_retvm_if(ret == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL,
			 "setting_memory_status_size_transition failed");

	ret = setting_memory_status_size_transition(dAvail, szAvail);
	setting_retvm_if(ret == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL,
			 "setting_memory_status_size_transition failed");

	SETTING_TRACE("Total[%s], Avail[%s]", szTotal, szAvail);
	return SETTING_RETURN_SUCCESS;
}

static void
setting_memory_status_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
					       void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL,
			"Invalid argument: event info is NULL");

	SettingMemoryUG *ad = (SettingMemoryUG *) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (list_item == ad->data_ex_mount) {
		setting_memory_status_ex_sd_card_mount_unmount(ad);
	} else if (list_item == ad->data_ex_format) {
		setting_memory_status_ex_sd_card_format_promt_popup_create(ad);
	}
}

static void
setting_memory_status_ex_sd_card_format_promt_popup_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	ad->curr_popup_cb = setting_memory_status_ex_sd_card_ask_format_resp_cb;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	ad->popup = setting_create_popup_with_btn(ad, ad->win_get, NULL,
					 _(SETTING_MEMORY_STATUS_FORMAT_ASK_MSG),
					 ad->curr_popup_cb, 0,
					 2, _("IDS_COM_SK_YES"),_("IDS_COM_SK_NO"));
}

static void
setting_memory_status_ex_sd_card_ask_format_resp_cb(void *data,
						    Evas_Object *obj,
						    void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	int response_type = btn_type(obj);
	if (POPUP_RESPONSE_OK == response_type) {
		setting_memory_status_ex_sd_card_format_promt_popup_left_btn_cb
		    (data, NULL, NULL);
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		setting_memory_status_ex_sd_card_format_promt_popup_right_btn_cb
		    (data, NULL, NULL);
	}
}

static void
setting_memory_status_ex_sd_card_format_promt_popup_left_btn_cb(void *data,
								Evas_Object *
								obj,
								void
								*event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	/* Show Formatting..... popup */
	ad->popup = setting_create_popup_with_progressbar(ad, ad->win_get,
						 "list_process",
						 _("IDS_ST_POP_MSG_FORMATTING"),
						 NULL, NULL,
						 SETTING_MEMORY_STATUS_PROGRESSBAR_TIMEOUT, TRUE, TRUE);

	int ret = __setting_memory_status_mmc_content_init(ad, setting_memory_status_ex_sd_card_format_cb);
	if (ret == SETTING_RETURN_FAIL) {
		SETTING_TRACE_ERROR("__setting_memory_status_mmc_content_init fail");
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _("IDS_COM_POP_FAILED"),
						 NULL,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
		return;
	}

	ret = sysman_request_format_mmc(ad->mmc_content);
	/*  if fail, destory popup */
	if (ret == -1) {
		SETTING_TRACE_ERROR("call sysman_request_format_mmc fail");
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _("IDS_COM_POP_FAILED"),
						 NULL,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
	}
}

static void
setting_memory_status_ex_sd_card_format_promt_popup_right_btn_cb(void *data,
								 Evas_Object *
								 obj,
								 void
								 *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

static void setting_memory_status_ex_sd_card_mount_unmount(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;

	int mmc_status = __setting_memory_status_check_mmc_status();
	setting_retm_if(mmc_status == -1, "check mmc status failed");

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	/* Show progressbar popup */
	ad->popup = setting_create_popup_with_progressbar(ad, ad->win_get,
						 "pending_list",
						 NULL, NULL, NULL,
						 SETTING_MEMORY_STATUS_PROGRESSBAR_TIMEOUT, TRUE, TRUE);

	int ret = -1;
	/* Does not mounted, same st_dev */
	if (mmc_status == 0) {
		ret = __setting_memory_status_mmc_content_init(ad, setting_memory_status_ex_sd_card_mount_cb);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("__setting_memory_status_mmc_content_init fail");
			if (ad->popup) {
				evas_object_del(ad->popup);
				ad->popup = NULL;
			}
			ad->popup = setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _("IDS_COM_POP_FAILED"),
						 NULL,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
			return;
		}
		ret = sysman_request_mount_mmc(ad->mmc_content);
		if (ret == -1) {
			SETTING_TRACE_ERROR("call sysman_request_mount_mmc fail");
		}
	}
	/*  MMC mounted, diff st_dev */
	else {
		ret = __setting_memory_status_mmc_content_init(ad, setting_memory_status_ex_sd_card_unmount_cb);
		if (ret == SETTING_RETURN_FAIL) {
			SETTING_TRACE_ERROR("__setting_memory_status_mmc_content_init fail");
			if (ad->popup) {
				evas_object_del(ad->popup);
				ad->popup = NULL;
			}
			ad->popup = setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _("IDS_COM_POP_FAILED"),
						 NULL,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
			return;
		}

		ret = sysman_request_unmount_mmc(ad->mmc_content,
						 MNT_FORCE);	/*  1 = MNT_FORCE */
		if (ret == -1) {
			SETTING_TRACE_ERROR("call sysman_request_unmount_mmc fail");
		}
	}

	/*  if fail, popup failed info */
	if (ret == -1) {
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _("IDS_COM_POP_FAILED"),
						 NULL,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
	}
}

static void
setting_memory_status_ex_sd_card_refresh_card_details_cb(void *data,
							 Evas_Object *obj,
							 void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	char ex_total_space[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0, };
	char ex_available_space[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0, };
	char ex_mount_status[SETTING_MEMORY_MMC_MAX_MEM_STR_LEN] = {0, };
	int ex_mmc_status = VCONFKEY_SYSMAN_MMC_REMOVED;
	setting_memory_status_ex_sd_card_info(ex_total_space, ex_available_space,
					      ex_mount_status, &ex_mmc_status);

	setting_memory_status_list_item_data_set(ad->data_ex_total, ex_total_space);
	setting_memory_status_list_item_data_set(ad->data_ex_available, ex_available_space);
	if(!isEmulBin())
	{
		setting_memory_status_list_item_data_set(ad->data_ex_mount, ex_mount_status);
	}
	setting_memory_status_list_item_enable_set_by_mmc_status(ad, ex_mmc_status);
}

static void
setting_memory_status_ex_sd_card_operate_complete(void *data, char *desc,
						  bool popup_flag)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *) data;
	setting_memory_hide_popup_with_progress_bar(ad);
	if (popup_flag) {
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		ad->popup = setting_create_popup_without_btn(ad, ad->win_get, NULL, desc,
						 setting_memory_status_ex_sd_card_refresh_card_details_cb,
						 SETTING_MEMORY_POPUP_TIMER, FALSE, FALSE);
	}
}

static void
setting_memory_status_list_item_enable_set_by_mmc_status(void *data,
							 int ex_mmc_status)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	if (ex_mmc_status == VCONFKEY_SYSMAN_MMC_REMOVED) {
		setting_disable_genlist_item(ad->data_ex_total->item);
		setting_disable_genlist_item(ad->data_ex_available->item);
		if(!isEmulBin())
		{
			setting_disable_genlist_item(ad->data_ex_mount->item);
			setting_disable_genlist_item(ad->data_ex_format->item);
		}
	} else if (ex_mmc_status == VCONFKEY_SYSMAN_MMC_INSERTED_NOT_MOUNTED) {
		setting_disable_genlist_item(ad->data_ex_total->item);
		setting_disable_genlist_item(ad->data_ex_available->item);
		if(!isEmulBin())
		{
			setting_enable_genlist_item(ad->data_ex_mount->item);
			setting_disable_genlist_item(ad->data_ex_format->item);
		}
	} else if (ex_mmc_status == VCONFKEY_SYSMAN_MMC_MOUNTED){
	} else {
		SETTING_TRACE_DEBUG("abnormal status of MMC");
	}
}

static void setting_memory_status_list_item_data_set(Setting_GenGroupItem_Data *
						     item_data, char *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(item_data == NULL, "Data parameter is NULL");

	G_FREE(item_data->keyStr);
	item_data->keyStr = (char *)g_strdup(data);
	elm_object_item_data_set(item_data->item, item_data);
	elm_genlist_item_update(item_data->item);
}

static void
setting_memory_status_get_phone_storage_available_size(void *data,
						       char *szAvail)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	//SettingMemoryUG *ad = (SettingMemoryUG *) data;

	double dTotal = 0.0;
	double dAvail = 0.0;
	int ret = SETTING_RETURN_SUCCESS;
	setting_mem_status internal_mem_stat;

	ret = __fs_stat(&dTotal, &dAvail, SETTING_MEMORY_STATUS_APP_PATH);
	setting_retm_if(ret == SETTING_RETURN_FAIL, "get app_mem stat failed");
	internal_mem_stat.dTotal = dTotal;
	internal_mem_stat.dAvail = dAvail;

	dTotal = 0.0;
	dAvail = 0.0;
	ret = __fs_stat(&dTotal, &dAvail, SETTING_MEMORY_STATUS_MEDIA_PATH);
	setting_retm_if(ret == SETTING_RETURN_FAIL, "get media_mem stat failed");
	internal_mem_stat.dTotal += dTotal;
	internal_mem_stat.dAvail += dAvail;

	ret = setting_memory_status_size_transition(internal_mem_stat.dAvail,
						  szAvail);
	setting_retm_if(ret == SETTING_RETURN_FAIL, "setting_memory_status_size_transition fail");
}

static int
setting_memory_status_size_transition(double size, char *size_desc)
{
	SETTING_TRACE_BEGIN;

	double tmp_size = 0.0;
	int ret = 0;
	if (size < SETTING_MEMORY_MEGABYTE_VALUE) {	// size < 1MB: show x.xKB
		tmp_size = size / SETTING_MEMORY_KILOBYTE_VALUE;
		ret = snprintf(size_desc, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN, "%4.2lf%s", tmp_size, "KB");
		retv_if(ret < 0, SETTING_RETURN_FAIL);
	} else if (size < SETTING_MEMORY_GIGABYTE_VALUE) {	// size < 1GB: show x.xMB
		tmp_size = size / SETTING_MEMORY_MEGABYTE_VALUE;
		ret = snprintf(size_desc, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN, "%4.2lf%s", tmp_size, "MB");
		retv_if(ret < 0, SETTING_RETURN_FAIL);
	} else { // 1G <= size: show x.xGB
		tmp_size = size / SETTING_MEMORY_GIGABYTE_VALUE;
		ret = snprintf(size_desc, SETTING_MEMORY_MMC_MAX_MEM_STR_LEN, "%4.2lf%s", tmp_size, "GB");
		retv_if(ret < 0, SETTING_RETURN_FAIL);
	}

	return SETTING_RETURN_SUCCESS;
}

static void setting_memory_status_ex_sd_card_format_cb(int val, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	if (val == SETTING_MEMORY_STATUS_SYSMAN_REQUEST_SUCCESS) {
		setting_memory_status_ex_sd_card_operate_complete(data,
								  _("IDS_COM_POP_SUCCESS"),
								  TRUE);

	} else {
		setting_memory_status_ex_sd_card_operate_complete(data,
							  	  _("IDS_COM_POP_FAILED"),
							  	  TRUE);
	}
}

static void setting_memory_status_ex_sd_card_mount_cb(int val, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	if (val == SETTING_MEMORY_STATUS_SYSMAN_REQUEST_SUCCESS) {
		setting_memory_status_ex_sd_card_operate_complete(data,
							  	  _("IDS_COM_POP_SUCCESS"),
							  	  FALSE);
	} else if (val == SETTING_MEMORY_STATUS_SYSMAN_REQUEST_MOUNT_ALREADY) {
		setting_memory_status_ex_sd_card_operate_complete(data,
							 	  _(SETTING_MEMORY_STATUS_EX_SD_CARD_MOUNT_ALREADY_DESC),
							  	  TRUE);
	} else {
		setting_memory_status_ex_sd_card_operate_complete(data,
							  	  _("IDS_COM_POP_FAILED"),
							  	  TRUE);
	}
}

static void setting_memory_status_ex_sd_card_unmount_cb(int val, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	if (val == SETTING_MEMORY_STATUS_SYSMAN_REQUEST_SUCCESS) {
		setting_memory_status_ex_sd_card_operate_complete(data,
							  	  _("IDS_COM_POP_SUCCESS"),
							  	  FALSE);
	} else {
		setting_memory_status_ex_sd_card_operate_complete(data,
							  	  _("IDS_COM_POP_FAILED"),
							  	  TRUE);
	}
}

static void __setting_memory_status_vconf_change_cb( keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;

	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SYSMAN_MMC_STATUS)) {
		SETTING_TRACE("status:%d", status);
		setting_memory_status_ex_sd_card_refresh_card_details_cb(ad,
						 	NULL,
						 	NULL);
	} else {
		SETTING_TRACE_ERROR("vconf_name is NULL");
	}
}

static int __setting_memory_status_check_mmc_status()
{
	int ret = 0;
	struct stat parent_stat, mount_stat;

	ret = stat(SETTING_MEMORY_STATUS_MMC_PATH, &mount_stat);
	setting_retvm_if(ret != 0, -1, "get stat error : /opt/storage/sdcard");

	ret = stat(SETTING_MEMORY_STATUS_MMC_PARENT_PATH, &parent_stat);
	setting_retvm_if(ret != 0, -1, "get stat error : /opt/storage/sdcard/..");

	/* Does not mounted, same st_dev */
	if(mount_stat.st_dev == parent_stat.st_dev)
	{
		return 0;
	}
	/* MMC mounted, diff st_dev */
	else
	{
		return 1;
	}
}

static int __setting_memory_status_mmc_content_init(void *data, setting_memory_mmc_cb cb)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, SETTING_RETURN_FAIL, "data is NULL");
	setting_retvm_if(NULL == cb, SETTING_RETURN_FAIL, "cb is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;
	if (ad->mmc_content == NULL) {
		ad->mmc_content = (struct mmc_contents *)calloc(1, sizeof(struct mmc_contents));
		if (ad->mmc_content == NULL ) {
			SETTING_TRACE_ERROR("calloc failed");
			return SETTING_RETURN_FAIL;
		}
	}
	ad->mmc_content->mmc_cb = cb;
	ad->mmc_content->user_data = ad;
	return SETTING_RETURN_SUCCESS;
}
