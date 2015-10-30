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
#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#include <efl_extension.h>
#include <media_content.h>

#include "setting-cfg.h"
#include "setting-storage-main.h"
#include "setting-storage-default.h"
#include "setting-storage-miscellaneous.h"
#include "setting-storage.h"

typedef enum {
    STORAGEUG_KEYWORD_NONE = 0,
    STORAGEUG_KEYWORD_MAIN_SYS_MEM,
    STORAGEUG_KEYWORD_MAIN_APPS,
    STORAGEUG_KEYWORD_MAIN_PICS,
    STORAGEUG_KEYWORD_MAIN_AUDIO,
    STORAGEUG_KEYWORD_MAIN_MISCES,
    STORAGEUG_KEYWORD_MAIN_AVAIL,
    STORAGEUG_KEYWORD_MAIN_SD_CARD,

    STORAGEUG_KEYWORD_DEFAULT = 100,
    STORAGEUG_KEYWORD_DEFAULT_BT,
    STORAGEUG_KEYWORD_DEFAULT_WIFI,
    STORAGEUG_KEYWORD_DEFAULT_NFC,
    STORAGEUG_KEYWORD_DEFAULT_APP,
    STORAGEUG_KEYWORD_MAX
} STORAGEUG_KEYWORD;

typedef struct {
	char *key_name;
	int keynum;
} storageUg_search_data;

static void _storageUg_vconf_change_cb(keynode_t *key, void *data)
{
	char *vconf_name;
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	vconf_name = vconf_keynode_get_name(key);
	if (NULL == vconf_name)
		return;

	if (ad->pie_it)
		elm_genlist_item_update(ad->pie_it);
}

static inline void _storageUg_listen_vconf(SettingStorageUG *storageUG)
{
	int ret;
	const char *FONT_SIZE = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE;
	const char *FONT_NAME = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME;

	ret = vconf_notify_key_changed(FONT_SIZE, _storageUg_vconf_change_cb, storageUG);
	warn_if(ret < 0, "vconf_notify_key_changed(%s) Fail(%d)", FONT_SIZE, ret);

	ret = vconf_notify_key_changed(FONT_NAME, _storageUg_vconf_change_cb, storageUG);
	warn_if(ret < 0, "vconf_notify_key_changed(%s) Fail(%d)", FONT_NAME, ret);
}

static inline void _storageUg_ignore_vconf()
{
	int ret;
	const char *FONT_SIZE = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE;
	const char *FONT_NAME = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME;

	ret = vconf_ignore_key_changed(FONT_SIZE, _storageUg_vconf_change_cb);
	warn_if(ret < 0, "vconf_ignore_key_changed(%s) Fail(%d)", FONT_SIZE, ret);

	ret = vconf_ignore_key_changed(FONT_NAME, _storageUg_vconf_change_cb);
	warn_if(ret < 0, "vconf_ignore_key_changed(%s) Fail(%d)", FONT_NAME, ret);
}

static inline void storageUg_init_itcs(SettingStorageUG *ad)
{
	setting_create_Gendial_itc("dialogue/2text.3/expandable", &(ad->itc_2text_3_parent));
	setting_create_Gendial_itc("dialogue/1text.1icon/expandable2",
	                           &(ad->itc_1icon_1text_sub));
	setting_create_Gendial_itc("dialogue/2text.3", &(ad->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(ad->itc_group_item));
	setting_create_Gendial_itc("2line.top.4", &(ad->itc_2text_1icon_3));
	setting_create_Gendial_itc("dialogue/1text", &(ad->itc_1text));
	setting_create_Gendial_itc("1text.1icon.3", &(ad->itc_1text_1icon));
	setting_create_Gendial_itc("1icon", &(ad->itc_pie));
	ad->itc_pie.func.content_get = storageUg_main_pie_item_get_icon;
	ad->itc_pie.func.del = NULL;
	setting_create_Gendial_itc("1icon/no_padding_line", &(ad->itc_notice));
	ad->itc_notice.func.content_get = storageUg_default_notice_get_icon;
	ad->itc_notice.func.del = NULL;

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(ad->itc_color_item));
	ad->itc_color_item.func.content_get = storageUg_color_item_content_get;
	ad->itc_color_item.func.del = NULL;
}

static void storageUg_navi_back(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;

	ret_if(NULL == data);
	ret_if(NULL != ad->del_worker);
	retm_if(ad->sd_request || ad->usb_request, "sd_request(%d), usb_request(%d)",
	        ad->sd_request, ad->usb_request);

	elm_naviframe_item_pop(obj);
}

static inline Evas_Object *storageUg_create_navi(Evas_Object *parent,
                                                 SettingStorageUG *ad)
{
	Evas_Object *navi;

	retv_if(NULL == parent, NULL);

	navi = elm_naviframe_add(parent);
	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_FALSE);

	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK, storageUg_navi_back, ad);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, ad);

	evas_object_show(navi);
	return navi;
}

static inline void storageUg_move_view(STORAGEUG_KEYWORD keynum,
                                       SettingStorageUG *ad)
{
	Setting_GenGroupItem_Data *item_data;

	retm_if(keynum <= STORAGEUG_KEYWORD_NONE || STORAGEUG_KEYWORD_MAX <= keynum,
	        "keynum(%d) is Invalid", keynum);

	switch (keynum) {
		case STORAGEUG_KEYWORD_MAIN_SYS_MEM:
			item_data = ad->sys_mem;
			break;
		case STORAGEUG_KEYWORD_MAIN_APPS:
			item_data = ad->apps;
			break;
		case STORAGEUG_KEYWORD_MAIN_PICS:
			item_data = ad->pics_videos;
			break;
		case STORAGEUG_KEYWORD_MAIN_AUDIO:
			item_data = ad->audio;
			break;
		case STORAGEUG_KEYWORD_MAIN_MISCES:
			item_data = ad->misces;
			break;
		case STORAGEUG_KEYWORD_MAIN_AVAIL:
			item_data = ad->avail;
			break;
		case STORAGEUG_KEYWORD_MAIN_SD_CARD:
			item_data = ad->sd_card;
			break;
		case STORAGEUG_KEYWORD_DEFAULT_BT:
			item_data = ad->data_bt;
			break;
		case STORAGEUG_KEYWORD_DEFAULT_WIFI:
			item_data = ad->data_wifidirect;
			break;
		case STORAGEUG_KEYWORD_DEFAULT_NFC:
			item_data = ad->data_nfc;
			break;
		case STORAGEUG_KEYWORD_DEFAULT_APP:
			item_data = ad->data_installapp;
			break;
		default:
			item_data = NULL;
			break;
	}

	if (item_data && item_data->item) {
		if (STORAGEUG_KEYWORD_DEFAULT < keynum)
			elm_genlist_item_expanded_set(item_data->item, TRUE);
		else
			elm_genlist_item_bring_in(item_data->item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	}
}

static void *setting_storageUg_on_create(ui_gadget_h ug, enum ug_mode mode,
                                         app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	int ret;
	char *caller = NULL;
	SettingStorageUG *ad = priv;
	char *search_keyword = NULL;
	const char *GENLIST_edj = EDJDIR"/setting-genlist.edj";
	STORAGEUG_KEYWORD keyword_val = STORAGEUG_KEYWORD_NONE;

	SETTING_TRACE(" -----> [TIME-1] before ");
	appcore_measure_start();

	retvm_if(NULL == ug || NULL == priv, NULL, "ug=%p, priv=%p is Invalid", ug, priv);

	ad->ug = ug;
	ad->win = ug_get_window();
	ad->lo_parent = ug_get_parent_layout(ug);
	if (NULL == ad->lo_parent) {
		SETTING_TRACE_ERROR("ug_get_parent_layout(ug) Fail");
		return NULL;
	}

	ret = media_content_connect();
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("media_content_connect() Fail(%d)", ret);
		/*return NULL; */
	}

	app_control_get_extra_data(service, "caller", &caller);
	app_control_get_extra_data(service, "keyword", &search_keyword);
	if (NULL == caller)
		elm_theme_extension_add(NULL, GENLIST_edj);

	/* add listening */
	_storageUg_listen_vconf(ad);

	storageUg_main_init(ad);
	storageUg_default_init(ad);
	storageUg_misc_init(ad);

	setting_view_node_table_intialize();
	setting_view_node_table_register(ad->main_view, NULL);
	setting_view_node_table_register(ad->default_view, ad->main_view);
	setting_view_node_table_register(ad->misces_view, ad->main_view);

	storageUg_init_itcs(ad);

	ad->lo_main = setting_create_win_layout(ad->win);
	ad->navi = storageUg_create_navi(ad->lo_main, ad);
	if (NULL == ad->navi) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return NULL;
	}
	elm_object_part_content_set(ad->lo_main, "elm.swallow.content", ad->navi);

	if (search_keyword)
		keyword_val = atoi(search_keyword);

	if (STORAGEUG_KEYWORD_DEFAULT <= keyword_val && keyword_val < STORAGEUG_KEYWORD_MAX)
		setting_view_create(ad->default_view, ad);
	else
		setting_view_create(ad->main_view, ad);

	if (keyword_val)
		storageUg_move_view(keyword_val, ad);

	free(caller);
	free(search_keyword);

	SETTING_TRACE(" -----> [TIME-2] it took %d msec ", appcore_measure_time());
	appcore_measure_start();

	return ad->lo_main;
}

static void setting_storageUg_on_destroy(ui_gadget_h ug, app_control_h service,
                                         void *priv)
{
	SETTING_TRACE_BEGIN;
	int ret;
	SettingStorageUG *storageUG = priv;

	retm_if(NULL == ug || NULL == priv, "ug=%p, priv=%p is Invalid", ug, priv);

	FREE(storageUG->usb_otg_path);

	_storageUg_ignore_vconf();

	ret = media_content_disconnect();
	warn_if(ret != MEDIA_CONTENT_ERROR_NONE, "media_content_disconnect Fail");

	if (storageUG->main_view->is_create)
		setting_view_destroy(storageUG->main_view, storageUG);
	if (storageUG->default_view->is_create)
		setting_view_destroy(storageUG->default_view, storageUG);

	if (storageUG->lo_main) {
		evas_object_del(storageUG->lo_main);
		storageUG->lo_main = NULL;
	}
}

static void setting_storageUg_on_resume(ui_gadget_h ug, app_control_h service,
                                        void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *storageUG = priv;

	retm_if(NULL == ug || NULL == priv, "ug=%p, priv=%p is Invalid", ug, priv);

	if (storageUG->main_view->is_create)
		setting_view_update(storageUG->main_view, storageUG);


}


static void setting_storageUg_on_event(ui_gadget_h ug, enum ug_event event,
                                       app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *storageUG = priv;

	retm_if(NULL == ug || NULL == priv, "ug=%p, priv=%p is Invalid", ug, priv);

	switch (event) {
		case UG_EVENT_LOW_MEMORY:
			break;
		case UG_EVENT_LOW_BATTERY:
			break;
		case UG_EVENT_LANG_CHANGE:
			if (storageUG->navi)
				setting_navi_items_update(storageUG->navi);
			break;
		case UG_EVENT_ROTATE_PORTRAIT:
		case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		case UG_EVENT_ROTATE_LANDSCAPE:
		case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
			/*
			   if (storageUG->application_desp)
			   elm_genlist_item_update(storageUG->application_desp->item);
			   */
			break;
		case UG_EVENT_REGION_CHANGE:
			break;
		default:
			break;
	}
}

static void setting_storageUg_on_key_event(ui_gadget_h ug,
                                           enum ug_key_event event, app_control_h service, void *priv)
{
	if (!ug)
		return;

	switch (event) {
		case UG_KEY_EVENT_END:
			ug_destroy_me(ug);
			break;
		default:
			break;
	}
}

static void setting_storageUg_on_start(ui_gadget_h ug, app_control_h app_control, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *storageUG = priv;

	retm_if(NULL == ug || NULL == priv, "ug=%p, priv=%p is Invalid", ug, priv);

	SETTING_TRACE(" -----> [TIME-3] it took %d msec to finish on_Start ", appcore_measure_time());
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SettingStorageUG *storageUG;

	retv_if(NULL == ops, -1);

	storageUG = calloc(1, sizeof(SettingStorageUG));
	if (NULL == storageUG) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return -1;
	}

	ops->create = setting_storageUg_on_create;
	ops->start = setting_storageUg_on_start;
	ops->pause = NULL;
	ops->resume = setting_storageUg_on_resume;
	ops->destroy = setting_storageUg_on_destroy;
	ops->message = NULL;
	ops->event = setting_storageUg_on_event;
	ops->key_event = setting_storageUg_on_key_event;
	ops->priv = storageUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}


UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	ret_if(NULL == ops);

	FREE(ops->priv);
}

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv,
                                             char **applocale)
{
	int i;
	int size;
	char ug_args[STORAGEUG_MAX_STR_LEN];
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = (Eina_List **)priv;
	const storageUg_search_data search_configs[] = {
		/* key_name, view_type, keyword, action, data */
		{STORAGEUG_STR_SYS_MEM, STORAGEUG_KEYWORD_MAIN_SYS_MEM},
		{STORAGEUG_STR_APPS, STORAGEUG_KEYWORD_MAIN_APPS},
		{STORAGEUG_STR_PICS, STORAGEUG_KEYWORD_MAIN_PICS},
		{STORAGEUG_STR_AUDIO, STORAGEUG_KEYWORD_MAIN_AUDIO},
		{STORAGEUG_STR_MISCES, STORAGEUG_KEYWORD_MAIN_MISCES},
		{STORAGEUG_STR_AVAIL, STORAGEUG_KEYWORD_MAIN_AVAIL},
		{STORAGEUG_STR_SD_CARD, STORAGEUG_KEYWORD_MAIN_SD_CARD},
		{STORAGEUG_STR_DEF_STORAGE, STORAGEUG_KEYWORD_DEFAULT},
		{STORAGEUG_STR_BT, STORAGEUG_KEYWORD_DEFAULT_BT},
		{STORAGEUG_STR_WIFI, STORAGEUG_KEYWORD_DEFAULT_WIFI},
		{STORAGEUG_STR_NFC, STORAGEUG_KEYWORD_DEFAULT_NFC},
		{STORAGEUG_STR_INSTALLED_APP_B, STORAGEUG_KEYWORD_DEFAULT_APP}
	};

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:/usr/apps/org.tizen.setting/res/locale");

	size = sizeof(search_configs) / sizeof(storageUg_search_data);

	for (i = 0; i < size; i++) {
		sprintf(ug_args, "keyword:%d", search_configs[i].keynum);
		node = setting_plugin_search_item_subindex_add(
		           search_configs[i].key_name,
		           ug_args,
		           IMG_Storage,
		           Cfg_Item_View_Node,
		           NULL,
		           "Storage");

		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

