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

#include <app.h>
#include <appcore-common.h>
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


#define SETTING_STORAGE_PACKAGE_NAME "org.tizen.setting-storage"

/******************************APP CONTROL***********************************/
static bool _setting_storage_app_create(void *data);
static void _setting_storage_app_control_cb(app_control_h app_control, void *data);
static void _setting_storage_app_on_pause(void *data);
static void _setting_storage_app_on_resume(void *data);
static void _setting_storage_app_terminate(void *data);
/********************************OTHER***************************************/
static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
//	ui_app_exit();
	elm_exit();
}

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

	ret = vconf_notify_key_changed(FONT_SIZE, _storageUg_vconf_change_cb,
			storageUG);
	warn_if(ret < 0, "vconf_notify_key_changed(%s) Fail(%d)", FONT_SIZE,
			ret);

	ret = vconf_notify_key_changed(FONT_NAME, _storageUg_vconf_change_cb,
			storageUG);
	warn_if(ret < 0, "vconf_notify_key_changed(%s) Fail(%d)", FONT_NAME,
			ret);
}

static inline void _storageUg_ignore_vconf()
{
	int ret;
	const char *FONT_SIZE = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE;
	const char *FONT_NAME = VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME;

	ret = vconf_ignore_key_changed(FONT_SIZE, _storageUg_vconf_change_cb);
	warn_if(ret < 0, "vconf_ignore_key_changed(%s) Fail(%d)", FONT_SIZE,
			ret);

	ret = vconf_ignore_key_changed(FONT_NAME, _storageUg_vconf_change_cb);
	warn_if(ret < 0, "vconf_ignore_key_changed(%s) Fail(%d)", FONT_NAME,
			ret);
}

static inline void storageUg_init_itcs(SettingStorageUG *ad)
{
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_group_item));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE,
			&(ad->itc_pie));
	ad->itc_pie.func.content_get = storageUg_main_pie_item_get_icon;
	ad->itc_pie.func.del = NULL;

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_color_item));
	ad->itc_color_item.func.content_get = storageUg_color_item_content_get;
	ad->itc_color_item.func.del = NULL;
}

static void storageUg_navi_back(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;

	ret_if(NULL == data);
	ret_if(NULL != ad->del_worker);
	retm_if(ad->sd_request || ad->usb_request,
			"sd_request(%d), usb_request(%d)",
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

	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK,
			storageUg_navi_back, ad);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE,
			eext_naviframe_more_cb, ad);

	evas_object_show(navi);
	return navi;
}

static void _lang_changed(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *ad = data;
	setting_retm_if(NULL == data, "data is NULL");

	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
		setting_navi_items_update(ad->md.navibar_main);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

static void _setting_storage_app_control_cb(app_control_h app_control, void *data)
{
	SETTING_TRACE_BEGIN;
}

static bool _setting_storage_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	setting_retvm_if(NULL == data, false, "!data");
	SettingStorageUG *ad = (SettingStorageUG *)data;
	SETTING_TRACE(" -----> [TIME-1] before ");
	appcore_measure_start();

	setting_set_i18n("setting", STORAGE_LOCALEDIR);
	if (app_init(&ad->md, SETTING_STORAGE_PACKAGE_NAME)
							!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}
	evas_object_smart_callback_add(ad->md.win_main, "delete,request",
				_main_win_del_cb, ad);

	/* add listening */
	_storageUg_listen_vconf(ad);

	ret = media_content_connect();
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("media_content_connect() Fail(%d)", ret);
		/*return false; */
	}

	storageUg_init_itcs(ad);
	storageUg_main_init(ad);
	storageUg_default_init(ad);
	storageUg_misc_init(ad);

	setting_view_node_table_intialize();
	setting_view_node_table_register(ad->main_view, NULL);
	setting_view_node_table_register(ad->default_view, ad->main_view);
	setting_view_node_table_register(ad->misces_view, ad->main_view);

	setting_view_create(ad->main_view, ad);
	SETTING_TRACE(" -----> [TIME-2] it took %d msec ",
			appcore_measure_time());
	appcore_measure_start();

	return true;
}

static void _setting_storage_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	SettingStorageUG *storageUG = data;

	retm_if(NULL == data, "data=%p is Invalid", data);

	FREE(storageUG->usb_otg_path);

	_storageUg_ignore_vconf();

	ret = media_content_disconnect();
	warn_if(ret != MEDIA_CONTENT_ERROR_NONE,
			"media_content_disconnect Fail");

	if (storageUG->default_view->is_create)
		setting_view_destroy(storageUG->default_view, storageUG);
	if (storageUG->misces_view->is_create)
		setting_view_destroy(storageUG->misces_view, storageUG);
	if (storageUG->main_view->is_create)
		setting_view_destroy(storageUG->main_view, storageUG);

	if (storageUG->md.win_main) {
		evas_object_del(storageUG->md.win_main);
		storageUG->md.win_main = NULL;
	}
}

static void _setting_storage_app_on_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void _setting_storage_app_on_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *storageUG = data;

	retm_if(NULL == data, "data=%p is Invalid", data);

	if (storageUG->main_view->is_create)
		setting_view_update(storageUG->main_view, storageUG);
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	int r = 0;
	SettingStorageUG storage_ad ;
	app_event_handler_h handlers[5] = {NULL, };

	ui_app_lifecycle_callback_s ops = {
		.create = _setting_storage_app_create,
		.pause = _setting_storage_app_on_pause,
		.resume = _setting_storage_app_on_resume,
		.terminate = _setting_storage_app_terminate,
		.app_control = _setting_storage_app_control_cb,
	};

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &storage_ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);


	memset(&storage_ad, 0x00, sizeof(SettingStorageUG));
	r = ui_app_main(argc, argv, &ops, &storage_ad);
	retv_if(r == -1, -1);

	return 0;
}
