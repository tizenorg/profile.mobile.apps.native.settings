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
#include <setting-cfg.h>

#include "setting-appmgr-pkginfo.h"
#include "setting-appmgr-pkginfo-utils.h"
#include "setting-appmgr-runinfo.h"
#include "setting-appmgr-utils.h"
#include "setting-appmgr-main.h"
#include "setting-appmgr.h"

#include <app.h>
#include <appcore-common.h>

#define SETTING_APPMGR_PACKAGE_NAME "org.tizen.setting-appmgr"

/******************************APP CONTROL***********************************/
static bool _setting_appmgr_app_create(void *data);
static void _setting_appmgr_app_control_cb(app_control_h app_control, void *data);
static void _setting_appmgr_app_on_pause(void *data);
static void _setting_appmgr_app_on_resume(void *data);
static void _setting_appmgr_app_terminate(void *data);
/********************************OTHER***************************************/
static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info);


static void _main_win_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	elm_exit();
}

static void appmgrUg_resize(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);

	if (ad->runinfo_view->is_create)
		setting_view_update(ad->runinfo_view, ad);
	if (ad->pkginfo_view->is_create)
		setting_view_update(ad->pkginfo_view, ad);
	if (ad->main_view->is_create)
		setting_view_update(ad->main_view, ad);
}
#if 0
static void _app_mgr_item_del(void *data, Evas_Object *obj)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data =
	(Setting_GenGroupItem_Data *) data;
	if (item_data)
		FREE(item_data);
}
#endif

static void _main_gl_del_cb(void *data, Evas_Object *obj EINA_UNUSED)
{
	ret_if(NULL == data);
	/*appmgr_listinfo *info = data; */
	/*info->item = NULL; */
}

static inline void appmgrUg_init_itcs(SettingAppMgrUG *ad)
{
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_grp_title));
	setting_create_Gendial_itc(SETTING_GENLIST_MULTILINE_STYLE,
			&(ad->itc_multiline));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1txt_1ic_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1txt));
	setting_create_Gendial_itc("multiline_sub.main",
			&(ad->itc_multiline_2txt));

	ad->itc_main.item_style = SETTING_GENLIST_2LINE_STYLE;
	ad->itc_main.func.text_get = appmgrUg_main_gl_label_new_get;
	ad->itc_main.func.content_get = appmgrUg_main_gl_icon_new_get;
	ad->itc_main.func.state_get = NULL;
	ad->itc_main.func.del = _main_gl_del_cb;

	ad->itc_1ic.item_style = SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE;
	ad->itc_1ic.func.text_get = NULL;
	ad->itc_1ic.func.content_get = appmgrUg_run_gl_stop_btn;
	ad->itc_1ic.func.state_get = NULL;
	ad->itc_1ic.func.del = NULL;

	ad->itc_info_title.item_style = SETTING_GENLIST_2LINE_STYLE;
	ad->itc_info_title.func.text_get = appmgrUg_info_title_gl_label_get;
	ad->itc_info_title.func.content_get = appmgrUg_info_title_gl_icon_get;
	ad->itc_info_title.func.state_get = NULL;
	ad->itc_info_title.func.del = NULL;

	ad->itc_2button1.item_style = SETTING_GENLIST_1ICON_STYLE;
	ad->itc_2button1.func.text_get = NULL;
	ad->itc_2button1.func.content_get = appmgrUg_info_2button1_gl_icon_get;
	ad->itc_2button1.func.state_get = NULL;
	ad->itc_2button1.func.del = NULL;

	ad->itc_2button2.item_style = SETTING_GENLIST_1ICON_STYLE;
	ad->itc_2button2.func.text_get = NULL;
	ad->itc_2button2.func.content_get = appmgrUg_info_2button2_gl_icon_get;
	ad->itc_2button2.func.state_get = NULL;
	ad->itc_2button2.func.del = NULL;

	setting_create_Gendial_itc(SETTING_GENLIST_1ICON_STYLE,
			&(ad->itc_1button));
	ad->itc_1button.func.text_get = NULL;
	ad->itc_1button.func.content_get = appmgrUg_info_1button_gl_icon_get;
	ad->itc_1button.func.del = NULL;

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2txt_2));
	ad->itc_2txt_2.func.text_get = appmgrUg_pkg_size_gl_label_get;
}

static void appmgrUg_navi_back(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;

	ret_if(data == NULL);
	retm_if(ad->pkg_request, "pkg_request(%d)", ad->pkg_request);

	elm_naviframe_item_pop(obj);
}

static inline Evas_Object *appmgrUg_create_navi(Evas_Object *parent,
		SettingAppMgrUG *ad)
{
	Evas_Object *navi;

	retv_if(NULL == parent, NULL);

	navi = elm_naviframe_add(parent);
	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_FALSE);

	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK,
			appmgrUg_navi_back, ad);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE,
			eext_naviframe_more_cb, ad);

	evas_object_show(navi);
	return navi;
}

static void _setting_appmgr_app_control_cb(app_control_h app_control, void *data)
{
	SETTING_TRACE_BEGIN;
}

static bool _setting_appmgr_app_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, false, "!data");

	SettingAppMgrUG *appmgr_ad = (SettingAppMgrUG *)data;

	setting_set_i18n("setting", APPMGR_LOCALEDIR);
	SETTING_TRACE_BEGIN;
	SETTING_TRACE_BEGIN;
	if (app_init(&appmgr_ad->md, SETTING_APPMGR_PACKAGE_NAME)
				!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}
	evas_object_smart_callback_add(appmgr_ad->md.win_main, "delete,request",
                                   _main_win_del_cb, appmgr_ad);

	appmgrUg_init_itcs(appmgr_ad);
	appmgrUg_main_init(appmgr_ad);
	appmgrUg_pkginfo_init(appmgr_ad);
	appmgrUg_appinfo_init(appmgr_ad);

	evas_object_event_callback_add(appmgr_ad->md.win_main,
									EVAS_CALLBACK_RESIZE,
									appmgrUg_resize, appmgr_ad);

	setting_view_node_table_register(&setting_view_appmgr_main,
		NULL);
	setting_view_node_table_register(&setting_view_appmgr_pkginfo,
		&setting_view_appmgr_main);

	setting_view_create(appmgr_ad->main_view, appmgr_ad);

	appmgr_ad->current_rotation = elm_win_rotation_get(appmgr_ad->md.win_main);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d", appmgr_ad->current_rotation);
	if (elm_win_wm_rotation_supported_get(appmgr_ad->md.win_main)) {
		int rots[4] = { 0, 90, 180, 270 };
		/* rotation value that app may want */
		elm_win_wm_rotation_available_rotations_set(appmgr_ad->md.win_main, rots, 4);
	}

	return true;
}

static void _setting_appmgr_app_terminate(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = (SettingAppMgrUG *)data;
	retm_if(NULL == ad, "data=%p is Invalid", data);

	if (ad->runinfo_view->is_create)
		setting_view_destroy(ad->runinfo_view, ad);
	if (ad->pkginfo_view->is_create)
		setting_view_destroy(ad->pkginfo_view, ad);
	if (ad->main_view->is_create)
		setting_view_destroy(ad->main_view, ad);

	SETTING_TRACE_DEBUG("!!! After setting_view_destroy");
	if (ad->md.win_main) {
		evas_object_del(ad->md.win_main);
		ad->md.win_main = NULL;
	}
}

static void _setting_appmgr_app_on_pause(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;
	retm_if(NULL == data, "data=%p is Invalid", data);

	if (ad->runinfo_view->is_create)
		setting_view_update(ad->runinfo_view, ad);
}

static void _setting_appmgr_app_on_resume(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;
	retm_if(NULL == data, "data=%p is Invalid", data);

	if (ad->pkginfo_view->is_create)
		setting_view_update(ad->pkginfo_view, ad);
	if (ad->main_view->is_create)
		setting_view_update(ad->main_view, ad);

	evas_object_show(ad->md.ly_main);
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	SettingAppMgrUG appmgr_ad;

	ui_app_lifecycle_callback_s ops = {
        .create = _setting_appmgr_app_create,
        .pause = _setting_appmgr_app_on_pause,
        .resume = _setting_appmgr_app_on_resume,
        .terminate = _setting_appmgr_app_terminate,
        .app_control = _setting_appmgr_app_control_cb,
    };

    memset(&appmgr_ad, 0x00, sizeof(SettingAppMgrUG));
	int r = 0;
    r = ui_app_main(argc, argv, &ops, &appmgr_ad);
    retv_if(r == -1, -1);

    return 0;
}
