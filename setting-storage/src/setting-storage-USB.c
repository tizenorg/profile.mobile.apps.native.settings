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
#include <dd-usbhost.h>

#include "setting-storage-utils.h"
#include "setting-storage-USB.h"

enum {
	STORAGEUG_USB_REQ_NONE,
	STORAGEUG_USB_REQ_MOUNT,
	STORAGEUG_USB_REQ_UNMOUNT,
	STORAGEUG_USB_REQ_UNMOUNT_BEFORE_FORMAT,
	STORAGEUG_USB_REQ_FORMAT
};

static inline void storageUg_USB_unmount(SettingStorageUG *ad)
{
	int ret;
	ret = unmount_usb_storage(ad->usb_otg_path);
	if (ret < 0) {
		SETTING_TRACE_ERROR("unmount_usb_storage(%s) Fail(%d)",
				ad->usb_otg_path, ret);
		storageUg_fail_popup(ad);
		ad->usb_request = STORAGEUG_USB_REQ_NONE;
	} else {
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
				PROGRESSBAR_STYLE,
				NULL, STORAGEUG_STR_UNMOUNTING,
				storageUg_popup_del, 0, TRUE, TRUE, 0);
	}
}

static inline void storageUg_USB_mount(SettingStorageUG *ad)
{
	int ret;

	ret = mount_usb_storage(ad->usb_otg_path);
	if (ret < 0) {
		SETTING_TRACE_ERROR("mount_usb_storage(%s) Fail(%d)",
				ad->usb_otg_path, ret);
		storageUg_fail_popup(ad);
		ad->usb_request = STORAGEUG_USB_REQ_NONE;
	} else {
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
				PROGRESSBAR_STYLE,
				NULL, NULL, storageUg_popup_del, 2, TRUE, TRUE,
				0);
		ad->usb_request = STORAGEUG_USB_REQ_MOUNT;
	}
}

static inline int storageUg_USB_format(SettingStorageUG *ad)
{
	int ret;

	ret = format_usb_storage(ad->usb_otg_path);
	if (ret < 0) {
		SETTING_TRACE_ERROR("format_usb_storage(%s) Fail(%d)",
				ad->usb_otg_path, ret);
		storageUg_fail_popup(ad);
		return SETTING_RETURN_FAIL;
	} else {
		ad->popup = setting_create_popup_with_progressbar(ad, ad->win,
				PROGRESSBAR_STYLE,
				NULL, STORAGEUG_STR_FORMATTING,
				storageUg_popup_del, 0, TRUE, TRUE, 0);
	}
	return SETTING_RETURN_SUCCESS;
}

static void storageUg_USB_unmount_resp(void *data, Evas_Object *obj,
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

	if (POPUP_RESPONSE_OK == response_type) {
		ad->usb_request = STORAGEUG_USB_REQ_UNMOUNT;
		storageUg_USB_unmount(ad);
	}
}

static void storageUg_USB_format_se_confirm(void *data, Evas_Object *obj,
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

	if (POPUP_RESPONSE_OK == response_type) {
		ad->usb_request = STORAGEUG_USB_REQ_UNMOUNT_BEFORE_FORMAT;
		storageUg_USB_unmount(ad);
	}
}

static void storageUg_USB_format_first_confirm(void *data, Evas_Object *obj,
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

	if (POPUP_RESPONSE_OK == response_type) {
		if (ad->popup)
			evas_object_del(ad->popup);

		ad->popup = setting_create_popup(ad, ad->win, NULL,
		STORAGEUG_STR_USB_FORMAT_SECOND_Q,
				storageUg_USB_format_se_confirm, 0, FALSE,
				FALSE, 2, STORAGEUG_STR_OK,
				STORAGEUG_STR_CANCEL);
	}

}

static void storageUg_main_USB_sel(void *data, Evas_Object *obj,
		void *event_info)
{
	SettingStorageUG *ad = data;
	Elm_Object_Item *item = event_info;
	Setting_GenGroupItem_Data *list_item;

	ret_if(NULL == data);
	ret_if(NULL == event_info);

	elm_genlist_item_selected_set(item, 0);
	list_item = elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SETTING_TRACE("clicking item[%s]", list_item->keyStr);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (list_item == ad->usb_mount) {
		storageUg_USB_mount(ad);
	} else if (list_item == ad->usb_unmount) {
		ad->popup = setting_create_popup(ad, ad->win, NULL,
		STORAGEUG_STR_USB_UNMOUNT_POPUP_MSG, storageUg_USB_unmount_resp,
				0, FALSE, FALSE, 2, STORAGEUG_STR_OK,
				STORAGEUG_STR_CANCEL);

	} else if (list_item == ad->usb_format) {
		ad->popup = setting_create_popup(ad, ad->win, NULL,
		STORAGEUG_STR_USB_FORMAT_Q, storageUg_USB_format_first_confirm,
				0, FALSE, FALSE, 2, STORAGEUG_STR_OK,
				STORAGEUG_STR_CANCEL);
	}
}

static inline void storageUg_USB_remove_info(SettingStorageUG *ad)
{
	if (ad->usb_card) {
		elm_object_item_del(ad->usb_card->item);
		ad->usb_card = NULL;
	}
	if (ad->usb_mount) {
		elm_object_item_del(ad->usb_mount->item);
		ad->usb_mount = NULL;
	}
	if (ad->usb_total) {
		elm_object_item_del(ad->usb_total->item);
		ad->usb_total = NULL;
	}
	if (ad->usb_available) {
		elm_object_item_del(ad->usb_available->item);
		ad->usb_available = NULL;
	}
	if (ad->usb_unmount) {
		elm_object_item_del(ad->usb_unmount->item);
		ad->usb_unmount = NULL;
	}
	if (ad->usb_format) {
		elm_object_item_del(ad->usb_format->item);
		ad->usb_format = NULL;
	}
	if (ad->usb_sep_item) {
		elm_object_item_del(ad->usb_sep_item);
		ad->usb_sep_item = NULL;
	}
}

static inline void storageUg_USB_append_mounted_info(SettingStorageUG *ad)
{
	double total = 0.0;
	double avail = 0.0;
	char total_str[STORAGEUG_MAX_STR_LEN] = { 0 };
	char avail_str[STORAGEUG_MAX_STR_LEN] = { 0 };

	storageUg_get_external_storage_status(ad->usb_otg_path, &total, &avail);
	storageUg_size_to_str(total, total_str, sizeof(total_str));
	storageUg_size_to_str(avail, avail_str, sizeof(avail_str));

	/* Total space */
	ad->usb_total = setting_create_Gendial_field_def(ad->gl_main,
			&(ad->itc_2text_2),
			NULL, ad, SWALLOW_Type_INVALID, NULL, NULL, 0,
			STORAGEUG_STR_TOTAL, total_str, NULL);
	if (ad->usb_total) {
		ad->usb_total->userdata = ad;
		elm_genlist_item_select_mode_set(ad->usb_total->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->usb_total is NULL");
	}

	/* Avaliable */
	ad->usb_available = setting_create_Gendial_field_def(ad->gl_main,
			&(ad->itc_2text_2),
			NULL, ad, SWALLOW_Type_INVALID, NULL, NULL, 0,
			STORAGEUG_STR_AVAIL_SPACE, avail_str, NULL);
	if (ad->usb_available) {
		ad->usb_available->userdata = ad;
		elm_genlist_item_select_mode_set(ad->usb_available->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->usb_available is NULL");
	}

	/* Usb Unmount*/
	ad->usb_unmount = setting_create_Gendial_field_def(ad->gl_main,
			&(ad->itc_1text), storageUg_main_USB_sel, ad,
			SWALLOW_Type_INVALID, NULL, NULL, 0,
			STORAGEUG_STR_UNMOUNT_USB, NULL, NULL);
	if (ad->usb_unmount) {
		ad->usb_unmount->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->usb_unmount is NULL");
	}

	/* Usb Format*/
	ad->usb_format = setting_create_Gendial_field_def(ad->gl_main,
			&(ad->itc_1text), storageUg_main_USB_sel, ad,
			SWALLOW_Type_INVALID, NULL, NULL, 0,
			STORAGEUG_STR_FORMAT_USB, NULL, NULL);
	if (ad->usb_format) {
		ad->usb_format->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->usb_format is NULL");
	}
}

static inline void storageUg_main_append_USB_info(SettingStorageUG *ad)
{
	ret_if(NULL == ad);

	if (ad->usb_card)
		storageUg_USB_remove_info(ad);

	if (ad->usb_otg_status == SETTING_STORAGE_USB_OTG_REMOVE)
		return;

	/* USB OTG storage */
	ad->usb_card = setting_create_Gendial_field_titleItem(ad->gl_main,
			&(ad->itc_group_item), STORAGEUG_STR_USB, NULL);

	if (SETTING_STORAGE_USB_OTG_MOUNT == ad->usb_otg_status) {
		storageUg_USB_append_mounted_info(ad);
	} else {
		/* Usb Mount*/
		ad->usb_mount = setting_create_Gendial_field_def(ad->gl_main,
				&(ad->itc_1text), storageUg_main_USB_sel, ad,
				SWALLOW_Type_INVALID, NULL, NULL, 0,
				STORAGEUG_STR_MOUNT_USB, NULL, NULL);
		if (ad->usb_mount) {
			ad->usb_mount->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("ad->usb_mount is NULL");
		}
	}
}

static void storageUg_USB_cb(char *type, char *path, int mount, void *data)
{
	int ret;
	SettingStorageUG *ad = data;

	ret_if(NULL == type);
	ret_if(NULL == path);
	ret_if(NULL == data);

	SETTING_TRACE_DEBUG("path:%s type:%s", path, type);

	FREE(ad->usb_otg_path);
	ad->usb_otg_path = SAFE_STRDUP(path);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (!strcmp(type, "storage_remove")) {
		ad->usb_otg_status = SETTING_STORAGE_USB_OTG_REMOVE;
	} else if (!strcmp(type, "storage_mount")) {
		ad->usb_otg_status = SETTING_STORAGE_USB_OTG_MOUNT;
		if (STORAGEUG_USB_REQ_UNMOUNT == ad->usb_request) {
			SETTING_TRACE_ERROR("unmount_usb_storage() Fail");
			storageUg_fail_popup(ad);
		}
	} else if (!strcmp(type, "storage_unmount")) {
		ad->usb_otg_status = SETTING_STORAGE_USB_OTG_UNMOUNT;
		if (STORAGEUG_USB_REQ_UNMOUNT_BEFORE_FORMAT == ad->usb_request) {
			ret = storageUg_USB_format(ad);
			if (SETTING_RETURN_SUCCESS == ret) {
				ad->usb_request = STORAGEUG_USB_REQ_FORMAT;
			} else {
				ad->usb_request = STORAGEUG_USB_REQ_NONE;
				storageUg_USB_mount(ad);
			}
			return;
		} else if (STORAGEUG_USB_REQ_FORMAT == ad->usb_request) {
			SETTING_TRACE_ERROR("format_usb_storage() Fail");
			storageUg_fail_popup(ad);
		} else if (STORAGEUG_USB_REQ_MOUNT == ad->usb_request) {
			SETTING_TRACE_ERROR("mount_usb_storage() Fail");
			storageUg_fail_popup(ad);
		}
	} else if (!strcmp(type, "storage_updated")) {
		if (mount == 0)
			ad->usb_otg_status = SETTING_STORAGE_USB_OTG_UNMOUNT;
		else
			ad->usb_otg_status = SETTING_STORAGE_USB_OTG_MOUNT;
	}
	ad->usb_request = STORAGEUG_USB_REQ_NONE;
	storageUg_main_append_USB_info(ad);
}

void storageUg_init_USB(SettingStorageUG *ad)
{
	int ret;

	ret = init_usbhost_signal();
	retm_if(ret < 0, "init_usbhost_signal() Fail(%d)", ret);

	ret = register_usb_storage_change_handler(storageUg_USB_cb, ad);
	if (ret < 0) {
		SETTING_TRACE_ERROR(
				"register_usb_storage_change_handler() Fail(%d)",
				ret);
		deinit_usbhost_signal();
		return;
	}

	ret = request_usb_storage_info();
	warn_if(ret < 0, "request_usb_storage_info() Fail(%d)", ret);
}

void storageUg_deinit_USB()
{
	deinit_usbhost_signal();
}
