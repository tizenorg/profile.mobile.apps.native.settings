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
#include <setting-memory-default-memory.h>
#define SUPPORT_DVB_B 1
#define DVB_H_STR "DVB-H"
static int setting_memory_default_memory_create(void *cb);
static int setting_memory_default_memory_destroy(void *cb);
static int setting_memory_default_memory_update(void *cb);
static int setting_memory_default_memory_cleanup(void *cb);
char *setting_default_memory_get_desc_str_by_vconf(setting_int_slp_list
						   slp_key);

setting_view setting_view_memory_default_memory = {
	.create = setting_memory_default_memory_create,
	.destroy = setting_memory_default_memory_destroy,
	.update = setting_memory_default_memory_update,
	.cleanup = setting_memory_default_memory_cleanup,
};

static void setting_memory_default_click_softkey_cancel_cb(void *data,
							   Evas_Object *obj,
							   void *event_info);
static void __setting_memory_default_memory_vconf_change_cb(
						keynode_t *key, void *data);
static void __setting_memory_default_memory_update_item_status(
					void *data, bool disable_mmc_flag);
static void __setting_memory_default_memory_update_vconf(void *data);

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static void __setting_memory_default_exp_cb(void *data, Evas_Object *obj,
					    void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = (SettingMemoryUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;
	if (data_parentItem == ad->data_wap
	    || data_parentItem == ad->data_bt
	    || data_parentItem == ad->data_cmr
	    || data_parentItem == ad->data_vrec
	    || data_parentItem == ad->data_rdo
	    || data_parentItem == ad->data_shr
	    || data_parentItem == ad->data_dvb_h) {

		SETTING_TRACE_DEBUG("start drawing exp menu.");

		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);
		setting_create_Gendial_exp_sub_field(scroller,
						     &(ad->itc_1icon_1text_sub),
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_DEF_MEMORY_PHONE,
						     "IDS_COM_BODY_PHONE",
						     NULL);

                Setting_GenGroupItem_Data *data_memory_card = NULL;
                data_memory_card =
                        setting_create_Gendial_exp_sub_field(scroller,
						     &(ad->itc_1icon_1text_sub),
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_DEF_MEMORY_MMC,
						     "IDS_COM_BODY_MEMORY_CARD",
						     NULL);
                setting_retm_if(!data_memory_card, "data_memory_card is NULL");

                int mmc_mode = VCONFKEY_SYSMAN_MMC_REMOVED;
                int ret = vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &mmc_mode);
                setting_retm_if(ret != 0, "Get vconf failed");

                /* if mmc card is not mounted, set item disable */
                if (mmc_mode == VCONFKEY_SYSMAN_MMC_REMOVED ||
                        mmc_mode == VCONFKEY_SYSMAN_MMC_INSERTED_NOT_MOUNTED) {
                        setting_disable_genlist_item(data_memory_card->item);
                }

		setting_update_chk_status(rgd,
					  data_parentItem->int_slp_setting_binded);

	}
}

static int setting_memory_default_memory_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMemoryUG *ad = (SettingMemoryUG *) cb;

	Evas_Object *scroller;

	Elm_Object_Item *item;
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
					     _("IDS_ST_BODY_MEMORYSETTINGS_DEFAULT_MEMORY"),
					     S_("IDS_COM_BODY_BACK"), NULL,
					     setting_memory_default_click_softkey_cancel_cb,
					     NULL, ad, &scroller, &(ad->navi_bar));

	retvm_if(ad->ly_main == NULL, -1, "Can't create main layout");

	setting_enable_expandable_genlist(scroller, ad,
					  __setting_memory_default_exp_cb,
					  NULL);
	/* scroller is a genlist */
	/* 1 wap */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_wap =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_ST_BODY_WAP_DOWNLOADS",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_WAP));
	if (ad->data_wap) {
		ad->data_wap->int_slp_setting_binded = INT_SLP_SETTING_DEFAULT_MEM_WAP;
	} else {
		SETTING_TRACE_ERROR("ad->data_wap is NULL");
	}

	/* 2 bt */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_bt =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_COM_BODY_BLUETOOTH",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH));
	if (ad->data_bt) {
		ad->data_bt->int_slp_setting_binded =
	    		INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH;
	} else {
		SETTING_TRACE_ERROR("ad->data_bt is NULL");
	}

	/* 3 cmr */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_cmr =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_COM_BODY_CAMERA",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_CAMERA));
	if (ad->data_cmr) {
		ad->data_cmr->int_slp_setting_binded =
	    		INT_SLP_SETTING_DEFAULT_MEM_CAMERA;
	} else {
		SETTING_TRACE_ERROR("ad->data_cmr is NULL");
	}
	/* 4 vrec */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_vrec =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_COM_BODY_VOICE_RECORDER",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_VOICE_RECORDER));
	if (ad->data_vrec) {
		ad->data_vrec->int_slp_setting_binded =
	    		INT_SLP_SETTING_DEFAULT_MEM_VOICE_RECORDER;
	} else {
		SETTING_TRACE_ERROR("ad->data_vrec is NULL");
	}

#if SUPPORT_FMradio
	/* 5 rdo */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_rdo =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_COM_BODY_FM_RADIO",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_FM_RADIO));
	if (ad->data_rdo) {
		ad->data_rdo->int_slp_setting_binded =
	    		INT_SLP_SETTING_DEFAULT_MEM_FM_RADIO;
	} else {
		SETTING_TRACE_ERROR("ad->data_rdo is NULL");
	}
#endif

#if SUPPORT_AllShare_Memory

	/* 6 shr */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_shr =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    "IDS_COM_BODY_ALLSHARE",
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_ALL_SHARE));
	if (ad->data_shr) {
		ad->data_shr->int_slp_setting_binded =
	    		INT_SLP_SETTING_DEFAULT_MEM_ALL_SHARE;
	} else {
		SETTING_TRACE_ERROR("ad->data_shr is NULL");
	}
#endif
#if SUPPORT_DVB_B
	/* 7 DVB-H */
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->data_dvb_h =
	    setting_create_Gendial_exp_parent_field(scroller,
						    &(ad->itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    DVB_H_STR,
						    setting_default_memory_get_desc_str_by_vconf
						    (INT_SLP_SETTING_DEFAULT_MEM_DVB_H));
	if (ad->data_dvb_h) {
		ad->data_dvb_h->int_slp_setting_binded =
			INT_SLP_SETTING_DEFAULT_MEM_DVB_H;
	} else {
		SETTING_TRACE_ERROR("ad->data_shr is NULL");
	}
#endif

	item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int ret = vconf_notify_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			__setting_memory_default_memory_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}
	setting_view_memory_default_memory.is_create = 1;

	SETTING_TRACE_END;

	return SETTING_RETURN_SUCCESS;
}

static int setting_memory_default_memory_destroy(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMemoryUG *ad = (SettingMemoryUG *) cb;

	int ret = vconf_ignore_key_changed(VCONFKEY_SYSMAN_MMC_STATUS,
			__setting_memory_default_memory_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	setting_view_memory_default_memory.is_create = 0;

	evas_object_del(ad->ly_main);

	return SETTING_RETURN_SUCCESS;
}

static int setting_memory_default_memory_update(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	return SETTING_RETURN_SUCCESS;
}

char *setting_default_memory_get_desc_str_by_vconf(setting_int_slp_list slp_key)
{
	/* SETTING_TRACE_BEGIN; */
	/* error check */

	char *setting_default_memory_type[SETTING_DEF_MEMORY_MAX] = {
		_("IDS_COM_BODY_PHONE"),
		_("IDS_COM_BODY_MEMORY_CARD")
	};
	int ret = SETTING_RETURN_FAIL;
	int value = 0;
	int err = -1;

        int mmc_mode = VCONFKEY_SYSMAN_MMC_REMOVED;
        ret = vconf_get_int(VCONFKEY_SYSMAN_MMC_STATUS, &mmc_mode);
        setting_retvm_if(ret != 0, NULL, "Get vconf failed");

        /* if mmc card is not mounted, set vconf by 0(phone) */
        if (mmc_mode == VCONFKEY_SYSMAN_MMC_REMOVED ||
                mmc_mode == VCONFKEY_SYSMAN_MMC_INSERTED_NOT_MOUNTED) {
                ret = setting_set_int_slp_key(slp_key, 0, &err);
                setting_retvm_if(ret != 0, NULL, "Set vconf failed");
        }

	ret = setting_get_int_slp_key(slp_key, &value, &err);

	switch (slp_key) {
	case INT_SLP_SETTING_DEFAULT_MEM_WAP:
	case INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH:
	case INT_SLP_SETTING_DEFAULT_MEM_CAMERA:
	case INT_SLP_SETTING_DEFAULT_MEM_VOICE_RECORDER:
	case INT_SLP_SETTING_DEFAULT_MEM_FM_RADIO:
	case INT_SLP_SETTING_DEFAULT_MEM_ALL_SHARE:
	case INT_SLP_SETTING_DEFAULT_MEM_ADOBE_AIR:
	case INT_SLP_SETTING_DEFAULT_MEM_DVB_H:
		{
			if (ret == SETTING_RETURN_FAIL
			    || value >= SETTING_DEF_MEMORY_MAX)
				value = SETTING_DEF_MEMORY_PHONE;
			return setting_default_memory_type[value];
		}
		break;

	default:
		ret = SETTING_GENERAL_ERR_NOT_FIND_VALUE_SWITCH;
		break;
	}

	return NULL;
}

static int setting_memory_default_memory_cleanup(void *cb)
{
	return 0;
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
setting_memory_default_click_softkey_cancel_cb(void *data, Evas_Object *obj,
					       void *event_info)
{
	ret_if(data == NULL);

	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	ug_destroy_me(ad->ug);
}

static void __setting_memory_default_memory_vconf_change_cb(
						keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;
	int status = vconf_keynode_get_int(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SYSMAN_MMC_STATUS)) {
		SETTING_TRACE("status:%d", status);
		if (VCONFKEY_SYSMAN_MMC_MOUNTED == status) {
			__setting_memory_default_memory_update_item_status(
								ad,FALSE);
		} else {
			__setting_memory_default_memory_update_item_status(
								ad, TRUE);
		}
	}
}

static void __setting_memory_default_memory_update_item_status(
					void *data, bool disable_mmc_flag)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	SettingMemoryUG *ad = (SettingMemoryUG *)data;

	setting_retm_if(NULL == ad->data_wap, "ad->data_wap is NULL");
	setting_retm_if(NULL == ad->data_wap->item,
			"ad->data_wap->item is NULL");

	Elm_Object_Item *list_item = ad->data_wap->item;
	Setting_GenGroupItem_Data *data_sub_item = NULL;
	while (list_item != NULL) {
		data_sub_item =
			(Setting_GenGroupItem_Data *)
				elm_object_item_data_get(list_item);
		/* data_sub_item is seperator*/
		if (data_sub_item == NULL) {
			list_item = elm_genlist_item_next_get(list_item);
			continue;
		}
		SETTING_TRACE("keystr = %s", data_sub_item->keyStr);
		/* disable_mmc_flag is true, means mmc is not mounted*/
		if (disable_mmc_flag) {
			if (!safeStrCmp(data_sub_item->keyStr,
					"IDS_COM_BODY_PHONE") &&
				! elm_genlist_item_selected_get(list_item)) {
				elm_genlist_item_selected_set(list_item, 1);
			} else if (!safeStrCmp(data_sub_item->keyStr,
					     "IDS_COM_BODY_MEMORY_CARD")) {
				elm_genlist_item_selected_set(list_item, 0);
				setting_disable_genlist_item(list_item);
			}
		} else {
			if (!safeStrCmp(data_sub_item->keyStr,
					     "IDS_COM_BODY_MEMORY_CARD") &&
				elm_object_item_disabled_get(list_item)) {
				setting_enable_genlist_item(list_item);
			}
		}
		list_item = elm_genlist_item_next_get(list_item);
	}

	/* if mmc is not mounted, update vconf*/
	setting_retm_if(NULL == ad->data_bt, "ad->data_bt is NULL");
	setting_retm_if(NULL == ad->data_cmr, "ad->data_cmr is NULL");
	setting_retm_if(NULL == ad->data_vrec, "ad->data_vrec is NULL");
	setting_retm_if(NULL == ad->data_rdo, "ad->data_rdo is NULL");
	setting_retm_if(NULL == ad->data_dvb_h, "ad->data_dvb_h is NULL");
	if (disable_mmc_flag) {
		__setting_memory_default_memory_update_vconf(ad->data_wap);
		__setting_memory_default_memory_update_vconf(ad->data_bt);
		__setting_memory_default_memory_update_vconf(ad->data_cmr);
		__setting_memory_default_memory_update_vconf(ad->data_vrec);
		__setting_memory_default_memory_update_vconf(ad->data_rdo);
		__setting_memory_default_memory_update_vconf(ad->data_dvb_h);
	}
}

static void __setting_memory_default_memory_update_vconf(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	Setting_GenGroupItem_Data *data_item = (Setting_GenGroupItem_Data *)data;
	int err = 0;
	int value = 0;
	int ret = 0;

	ret = setting_get_int_slp_key(
		data_item->int_slp_setting_binded, &value, &err);
	setting_retm_if(0 != ret, "get vconf failed");

	/* if mmc card is not mounted, set vconf by 0(phone) */
	if (value != 0) {
		ret = setting_set_int_slp_key(
			data_item->int_slp_setting_binded, 0, &err);
		setting_retm_if(ret != 0, "Set vconf failed");

		G_FREE(data_item->sub_desc);
		data_item->sub_desc = (char *)g_strdup(_("IDS_COM_BODY_PHONE"));
		elm_object_item_data_set(data_item->item, data_item);
		elm_genlist_item_update(data_item->item);
	}
}
