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

#include "setting-storage-utils.h"
#include "setting-storage-default.h"

enum {
    STORAGEUG_DEFAULT_STORAGE_PHONE,
    STORAGEUG_DEFAULT_STORAGE_SD
};

enum {
    SETTING_DEF_MEMORY_PHONE = 0,
    SETTING_DEF_MEMORY_MMC,
    SETTING_DEF_MEMORY_MAX
};

static setting_view setting_view_storage_default;


Evas_Object *storageUg_default_notice_get_icon(void *data, Evas_Object *obj,
                                               const char *part)
{
	Evas_Object *layout;
	Evas_Coord height, width;
	SettingStorageUG *ad = data;
	int offset = SETTING_STORAGE_PIE_RECT_OFFSET * 2;

	retv_if(NULL == obj, NULL);
	retv_if(NULL == data, NULL);
	retvm_if(safeStrCmp(part, "elm.icon"), NULL, "part(%s) Invaild", part);

	layout = elm_layout_add(obj);
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME, "default_storage");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0.0);
	elm_object_part_text_set(layout, "application_desp.text",
	                         _(STORAGEUG_STR_APP_LOC_NOTICE));

	/* in landscape mode, the size should be resize */
	evas_object_geometry_get(obj, NULL, NULL, &width, NULL);

	app_device_orientation_e m = elm_win_rotation_get(ad->win);
	if (APP_DEVICE_ORIENTATION_90 == m || APP_DEVICE_ORIENTATION_270 == m)
		height = SETTING_STORAGE_DEFAULT_STORAGE_APP_DESP_SIZE * 2 / 3;
	else
		height = SETTING_STORAGE_DEFAULT_STORAGE_APP_DESP_SIZE;

	setting_resize_object(layout, width - offset, height);
	evas_object_show(layout);
	return layout;
}

static Eina_Bool storageUg_default_back_cb(void *data, Elm_Object_Item *it)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, EINA_FALSE);

	if (ad->main_view->is_create)
		setting_view_change(ad->default_view, ad->main_view, ad);
	else
		ug_destroy_me(ad->ug);

	return EINA_TRUE;
}

static void _storageUg_default_set_value(Elm_Object_Item *parentItem,
                                         Setting_GenGroupItem_Data *d_subItem, Setting_GenGroupItem_Data *d_parentItem)
{
	int err;

	setting_set_int_slp_key(d_parentItem->int_slp_setting_binded, d_subItem->chk_status,
	                        &err);
	retm_if(0 != err, "setting_set_int_slp_key(%d) Fail(%d)",
	        d_parentItem->int_slp_setting_binded, err);

	G_FREE(d_parentItem->sub_desc);
	d_parentItem->sub_desc = (char *)g_strdup(d_subItem->keyStr);
	elm_object_item_data_set(d_parentItem->item, d_parentItem);
	elm_genlist_item_update(d_parentItem->item);

	SettingStorageUG *ad = d_parentItem->userdata;
	if (ad && ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

static void storageUg_default_expand_item_sel(void *data, Evas_Object *obj,
                                              void *event_info)
{
	Elm_Object_Item *parentItem;
	Elm_Object_Item *subitem = event_info;
	Setting_GenGroupItem_Data *d_subItem;
	Setting_GenGroupItem_Data *d_parentItem;

	ret_if(event_info == NULL || data == NULL);

	elm_genlist_item_selected_set(subitem, 0);

	/*parentItem = elm_genlist_item_parent_get(subitem); */


	d_subItem = elm_object_item_data_get(subitem);
	ret_if(NULL == d_subItem);

	d_parentItem = (Setting_GenGroupItem_Data *)data;	/* parent data */
	ret_if(NULL == d_parentItem);

	parentItem = d_parentItem->item;

	elm_radio_value_set(d_subItem->rgd, d_subItem->chk_status);

	_storageUg_default_set_value(parentItem, d_subItem, d_parentItem);


	/*elm_genlist_item_expanded_set(d_parentItem->item, EINA_FALSE); */
}


static void storageUg_default_radio_change(void *data, Evas_Object *obj,
                                           void *event_info)
{
	Elm_Object_Item *subItem, *parentItem;
	Setting_GenGroupItem_Data *list_item = data;
	Setting_GenGroupItem_Data *d_subItem;
	Setting_GenGroupItem_Data *d_parentItem;

	ret_if(data == NULL);

	list_item->chk_status = elm_radio_value_get(obj);	/*  for update */

	subItem = list_item->item;

	d_subItem = elm_object_item_data_get(subItem);
	ret_if(NULL == d_subItem);

	if (d_subItem->disableflag)
		return;

	d_parentItem = (Setting_GenGroupItem_Data *)data;
	ret_if(NULL == d_parentItem);

	parentItem = d_parentItem->item;

	_storageUg_default_set_value(parentItem, d_subItem, d_parentItem);
}

static void storageUg_default_reset_vconf(Setting_GenGroupItem_Data *data_item)
{
	int err = 0;
	int value = 0;

	ret_if(NULL == data_item);

	if (!safeStrCmp(data_item->sub_desc, STORAGEUG_STR_SD_CARD)) {

		setting_get_int_slp_key(data_item->int_slp_setting_binded, &value, &err);
		retm_if(0 != err, "setting_get_int_slp_key(%d) Fail(%d)",
		        data_item->int_slp_setting_binded, err);
		if (value != STORAGEUG_DEFAULT_STORAGE_PHONE) {
			setting_set_int_slp_key(data_item->int_slp_setting_binded,
			                        STORAGEUG_DEFAULT_STORAGE_PHONE, &err);
			retm_if(0 != err, "setting_set_int_slp_key(%d) Fail(%d)",
			        data_item->int_slp_setting_binded, err);
		}

		G_FREE(data_item->sub_desc);
		data_item->sub_desc = (char *)g_strdup(STORAGEUG_STR_INTERNAL_STORAGE);
		elm_object_item_data_set(data_item->item, data_item);
		elm_genlist_item_update(data_item->item);
	}
}

static void storageUg_default_mmc_changed_cb(keynode_t *node, void *data)
{
	int status;
	SettingStorageUG *ad = data;

	ret_if(NULL == node);
	ret_if(NULL == data);

	status = vconf_keynode_get_int(node);

	SETTING_TRACE("status:%d", status);
	if (VCONFKEY_SYSMAN_MMC_MOUNTED != status) {
		storageUg_default_reset_vconf(ad->data_bt);
		storageUg_default_reset_vconf(ad->data_wifidirect);
		storageUg_default_reset_vconf(ad->data_nfc);
		storageUg_default_reset_vconf(ad->data_installapp);
	}
}

static const char *storageUg_default_cur_storage_get(int mmc_status,
                                                     setting_int_slp_list slp_key)
{
	int err, value = 0;
	const char *phone = STORAGEUG_STR_INTERNAL_STORAGE;
	const char *sd = STORAGEUG_STR_SD_CARD;

	setting_get_int_slp_key(slp_key, &value, &err);
	warn_if(0 != err, "setting_get_int_slp_key(%d) Fail(%d)", slp_key, err);

	if (mmc_status != VCONFKEY_SYSMAN_MMC_MOUNTED && STORAGEUG_DEFAULT_STORAGE_SD == value) {
		setting_set_int_slp_key(slp_key, STORAGEUG_DEFAULT_STORAGE_PHONE, &err);
		warn_if(0 != err, "setting_set_int_slp_key(%d) Fail(%d)", slp_key, err);

		value = STORAGEUG_DEFAULT_STORAGE_PHONE;
	}

	if (STORAGEUG_DEFAULT_STORAGE_SD == value)
		return sd;
	else
		return phone;
}
void __storageUg_default_popup_del(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	evas_object_del(ad->popup);
	ad->popup = NULL;
}

static void storageUg_default_main_item_sel(void *data, Evas_Object *obj,
                                            void *event_info)
{

	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingStorageUG *ad = (SettingStorageUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */
	elm_genlist_item_selected_set(parentItem, 0);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */

	/* create popup */
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	Evas_Object *scroller = NULL;
	ad->popup = setting_create_popup_with_list(&scroller, ad, ad->win,
	                                           data_parentItem->keyStr, NULL, 0, false, false, 0);
	_P(ad->popup);


	Evas_Object *rgd;

	Setting_GenGroupItem_Data *memory_card;
	Setting_GenGroupItem_Data *phone;


	if (data_parentItem == ad->data_bt || data_parentItem == ad->data_nfc
	    || data_parentItem == ad->data_wifidirect
	    || data_parentItem == ad->data_installapp) {
		SETTING_TRACE("POPUP ITEMS IS CREATED");
		rgd = elm_radio_add(scroller);
		elm_object_style_set(rgd, "list");
		evas_object_propagate_events_set(rgd, EINA_TRUE);
		elm_radio_value_set(rgd, -1);
		/*phone = setting_create_Gendial_exp_sub_field(scroller, &itc_1icon_1text_sub,
				storageUg_default_expand_item_sel, ad, parentItem, SWALLOW_Type_1RADIO_RIGHT,
				rgd, SETTING_DEF_MEMORY_PHONE, STORAGEUG_STR_INTERNAL_STORAGE, NULL);*/
		phone = setting_create_Gendial_field_1radio(scroller,
		                                            &itc_multiline_1text_1icon,
		                                            storageUg_default_expand_item_sel, data_parentItem,
		                                            SWALLOW_Type_1RADIO_RIGHT, rgd,
		                                            SETTING_DEF_MEMORY_PHONE,
		                                            STORAGEUG_STR_INTERNAL_STORAGE,
		                                            NULL);
		setting_retm_if(NULL == phone, "phone is NULL");
		phone->disableflag = EINA_FALSE;

		/*memory_card = setting_create_Gendial_exp_sub_field(scroller,
				&itc_1icon_1text_sub, storageUg_default_expand_item_sel, ad,
				parentItem, SWALLOW_Type_1RADIO_RIGHT, rgd, SETTING_DEF_MEMORY_MMC,
				STORAGEUG_STR_SD_CARD, storageUg_default_radio_change);*/
		memory_card = setting_create_Gendial_field_1radio(scroller,
		                                                  &itc_multiline_1text_1icon,
		                                                  storageUg_default_expand_item_sel, data_parentItem,
		                                                  SWALLOW_Type_1RADIO_RIGHT, rgd,
		                                                  SETTING_DEF_MEMORY_MMC,
		                                                  STORAGEUG_STR_SD_CARD,
		                                                  storageUg_default_radio_change);
		retm_if(NULL == memory_card, "setting_create_Gendial_exp_sub_field() Fail");

		memory_card->disableflag = EINA_FALSE;

		if (ad->mmc_status != VCONFKEY_SYSMAN_MMC_MOUNTED) {
			setting_disable_genlist_item(memory_card->item);
			memory_card->disableflag = EINA_TRUE;
		}

		setting_update_chk_status(rgd, data_parentItem->int_slp_setting_binded);
	}
}

static int storageUg_default_create(void *data)
{
	int ret;
	SettingStorageUG *ad = data;
	Evas_Object *genlist = NULL;
	const char *grp_title, *item_title;
	const char *cur_storage;

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	if (!ad->main_view->is_create) {
		ret = vconf_get_int(storageUg_MMC_stat, &ad->mmc_status);
		retvm_if(ret, SETTING_RETURN_FAIL, "vconf_get_int(%s) Fail", storageUg_MMC_stat);
	}

	setting_push_layout_navi_bar_genlist(ad->lo_parent, ad->win,
	                                     STORAGEUG_STR_DEF_STORAGE, STORAGEUG_STR_BACK, NULL,
	                                     (setting_call_back_func)storageUg_default_back_cb,
	                                     NULL, ad, &genlist, ad->navi);

	/*Shared contents */
	setting_create_Gendial_field_titleItem(genlist, &itc_group_item,
	                                       STORAGEUG_STR_SHARED_CONTENTS, NULL);

	/* bluetooth */
	cur_storage = storageUg_default_cur_storage_get(ad->mmc_status,
	                                                INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH);
	ad->data_bt = setting_create_Gendial_field_def(genlist, &(itc_2text), storageUg_default_main_item_sel,
	                                               ad, SWALLOW_Type_INVALID, NULL,
	                                               NULL, 0, STORAGEUG_STR_BT, (char *)cur_storage, NULL);
	if (ad->data_bt) {
		ad->data_bt->int_slp_setting_binded = INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH;
		ad->data_bt->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_bt is NULL");
	}

	/* Wifi Direct */
	cur_storage = storageUg_default_cur_storage_get(ad->mmc_status,
	                                                INT_SLP_SETTING_DEFAULT_MEM_WIFIDIRECT);
	ad->data_wifidirect = setting_create_Gendial_field_def(genlist, &(itc_2text), storageUg_default_main_item_sel,
	                                                       ad, SWALLOW_Type_INVALID, NULL,
	                                                       NULL, 0, STORAGEUG_STR_WIFI, (char *)cur_storage, NULL);
	if (ad->data_wifidirect) {
		ad->data_wifidirect->int_slp_setting_binded = INT_SLP_SETTING_DEFAULT_MEM_WIFIDIRECT;
		ad->data_wifidirect->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_wifidirect is NULL");
	}

	/*Install application */
	grp_title = STORAGEUG_STR_INSTALLED_APP_H;
	item_title = STORAGEUG_STR_INSTALLED_APP_B;

	setting_create_Gendial_field_titleItem(genlist, &itc_group_item, grp_title,
	                                       NULL);

	cur_storage = storageUg_default_cur_storage_get(ad->mmc_status,
	                                                INT_SLP_SETTING_DEFAULT_MEM_INSTALLAPPLICATIONS);
	ad->data_installapp = setting_create_Gendial_field_def(genlist, &(itc_2text), storageUg_default_main_item_sel,
	                                                       ad, SWALLOW_Type_INVALID, NULL,
	                                                       NULL, 0, item_title, (char *)cur_storage, NULL);
	if (ad->data_installapp) {
		ad->data_installapp->int_slp_setting_binded =
		    INT_SLP_SETTING_DEFAULT_MEM_INSTALLAPPLICATIONS;
		ad->data_installapp->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_installapp is NULL");
	}

	setting_add_gl_help(genlist, STORAGEUG_STR_APP_LOC_NOTICE);

	ret = vconf_notify_key_changed(storageUg_MMC_stat, storageUg_default_mmc_changed_cb,
	                               ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("vconf_notify_key_changed(%s) Fail(%d)", storageUg_MMC_stat,
		                    ret);
	}

	setting_view_storage_default.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_default_destroy(void *data)
{
	int ret;

	SettingStorageUG *ad = (SettingStorageUG *) data;

	ret = vconf_ignore_key_changed(storageUg_MMC_stat, storageUg_default_mmc_changed_cb);
	warn_if(ret, "vconf_ignore_key_changed(%s) Fail(%d)", storageUg_MMC_stat, ret);

	setting_view_storage_default.is_create = 0;
	elm_naviframe_item_pop(ad->navi);

	return SETTING_RETURN_SUCCESS;
}
static int storageUg_default_update(void *data)
{
	return SETTING_RETURN_SUCCESS;
}
static int storageUg_default_cleanup(void *data)
{
	return storageUg_default_destroy(data);
}


void storageUg_default_init(SettingStorageUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->default_view);

	setting_view_storage_default.create = storageUg_default_create;
	setting_view_storage_default.destroy = storageUg_default_destroy;
	setting_view_storage_default.update = storageUg_default_update;
	setting_view_storage_default.cleanup = storageUg_default_cleanup;

	ad->default_view = &setting_view_storage_default;
}

