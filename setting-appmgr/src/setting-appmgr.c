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

	/*
	 ad->itc_sep.item_style = "dialogue/separator.transparent.2";
	 ad->itc_sep.func.text_get = NULL;
	 ad->itc_sep.func.content_get = NULL;
	 ad->itc_sep.func.state_get = NULL;
	 ad->itc_sep.func.del = NULL;
	 */
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


#if SUPPORT_APP_ROATION
static void _rot_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = (SettingAppMgrUG *)data;
	ret_if(ad == NULL || ad->win == NULL);

	int change_ang = elm_win_rotation_get(ad->win);
	SETTING_TRACE_DEBUG("....change_ang:%d", change_ang);
	SETTING_TRACE_DEBUG("current_rotation:%d", ad->current_rotation);
	/*Send the rotation event to UGs.. */
	enum ug_event event = UG_EVENT_ROTATE_PORTRAIT;
	switch (change_ang) {
	case APP_DEVICE_ORIENTATION_0:
		event = UG_EVENT_ROTATE_PORTRAIT;
		break;
	case APP_DEVICE_ORIENTATION_180:
		event = UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN;
		break;
	case APP_DEVICE_ORIENTATION_270:
		event = UG_EVENT_ROTATE_LANDSCAPE;
		break;
	case APP_DEVICE_ORIENTATION_90:
		event = UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN;
		break;
	default:
		return;
	}
	SETTING_TRACE_DEBUG("diff:%d",
			elm_win_rotation_get(ad->win) - ad->current_rotation);

	if (change_ang != ad->current_rotation) {
		int diff = change_ang - ad->current_rotation;
		if (diff < 0)
			diff = -diff;

		/**
		 * @todo if app didn't launch UG, is the call required to
		 * invoke?
		 */
		ug_send_event(event);
		/* if (diff == 180) {
			do nothing
		} */
		ad->current_rotation = change_ang;
	}
}
#endif



static int appmgrUg_get_tabtype(char *keyword)
{
	retv_if(NULL == keyword, APPMGRUG_TAB_DOWNLOAD);

	if (0 == safeStrCmp(keyword, MGRAPP_STR_DOWNLOADS))
		return APPMGRUG_TAB_DOWNLOAD;
	else if (0 == safeStrCmp(keyword, MGRAPP_STR_RUNNING))
		return APPMGRUG_TAB_RUNNING;
	else if (0 == safeStrCmp(keyword, MGRAPP_STR_ALL))
		return APPMGRUG_TAB_ALL;
	else
		return APPMGRUG_TAB_DOWNLOAD;
}

static void *appmgrUg_on_create(ui_gadget_h ug, enum ug_mode mode,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

	char *viewtype = NULL;
	SettingAppMgrUG *ad = priv;
	char *search_keyword = NULL;

	retvm_if(NULL == ug || NULL == ad, NULL, "ug=%p, priv=%p is Invalid",
			ug, ad);

	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);
	textdomain(SETTING_PACKAGE);

	ad->ug = ug;
	ad->win = ug_get_window();
	ad->lo_parent = ug_get_parent_layout(ug);
	if (NULL == ad->lo_parent) {
		SETTING_TRACE_ERROR("ug_get_parent_layout(ug) Fail");
		return NULL;
	}


	ad->current_rotation = elm_win_rotation_get(ad->win);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d", ad->current_rotation);
	if (elm_win_wm_rotation_supported_get(ad->win)) {
		int rots[4] = { 0, 90, 180, 270 }; /* rotation value that app
		may want */
		elm_win_wm_rotation_available_rotations_set(ad->win, rots, 4);
	}
	evas_object_smart_callback_add(ad->win, "wm,rotation,changed",
			_rot_changed_cb, ad);


	appmgrUg_main_init(ad);
	appmgrUg_pkginfo_init(ad);
	appmgrUg_appinfo_init(ad);

	appmgrUg_init_itcs(ad);

	ad->lo_main = setting_create_win_layout(ad->win);
	ad->navi = appmgrUg_create_navi(ad->lo_main, ad);
	if (NULL == ad->navi) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return NULL;
	}
	elm_object_part_content_set(ad->lo_main, "elm.swallow.content",
			ad->navi);

	app_control_get_extra_data(service, "viewtype", &viewtype);
	app_control_get_extra_data(service, "keyword", &search_keyword);

	if (0 == safeStrCmp(viewtype, "application-info")) {
		/*register view table */
		setting_view_node_table_register(&setting_view_appmgr_pkginfo,
		NULL);

		SETTING_TRACE("viewtype : %s", viewtype);
		char *pkgid = NULL;
		app_control_get_extra_data(service, "pkgname", &pkgid);
		SETTING_TRACE("pkgname : %s", pkgid);
		ad->sel_pkgid = pkgid;
		ad->sel_total = -1;
		ad->sel_data_size = -1;
		ad->support_taskmanager = TRUE;
		ad->sel_is_disabled = FALSE;
		ad->sel_icon = NULL;
		ad->sel_label = NULL;
		setting_view_create(ad->pkginfo_view, ad);
	} else {
		/*register view table */
		setting_view_node_table_register(&setting_view_appmgr_main,
		NULL);
		setting_view_node_table_register(&setting_view_appmgr_pkginfo,
				&setting_view_appmgr_main);

		ad->tabtype = appmgrUg_get_tabtype(search_keyword);
		setting_view_create(ad->main_view, ad);
	}
	free(viewtype);

	evas_object_event_callback_add(ad->lo_parent, EVAS_CALLBACK_RESIZE,
			appmgrUg_resize, ad);
	return ad->lo_main;
}

static void appmgrUg_on_destroy(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SettingAppMgrUG *ad = priv;

	retm_if(NULL == ug || NULL == ad, "ug=%p, priv=%p is Invalid", ug, ad);

	evas_object_event_callback_del(ad->lo_parent, EVAS_CALLBACK_RESIZE,
			appmgrUg_resize);

	if (ad->runinfo_view->is_create)
		setting_view_destroy(ad->runinfo_view, ad);
	if (ad->pkginfo_view->is_create)
		setting_view_destroy(ad->pkginfo_view, ad);
	if (ad->main_view->is_create)
		setting_view_destroy(ad->main_view, ad);

	if (ad->lo_main) {
		evas_object_del(ad->lo_main);
		ad->lo_main = NULL;
	}
}

static void appmgrUg_on_pause(ui_gadget_h ug, app_control_h service, void *priv)
{
	SettingAppMgrUG *ad = priv;

	retm_if(NULL == ug || NULL == ad, "ug=%p, priv=%p is Invalid", ug, ad);

	if (ad->runinfo_view->is_create)
		setting_view_update(ad->runinfo_view, ad);
}

static void appmgrUg_on_resume(ui_gadget_h ug, app_control_h service,
		void *priv)
{
	SettingAppMgrUG *ad = priv;

	retm_if(NULL == ug || NULL == ad, "ug=%p, priv=%p is Invalid", ug, ad);

	if (ad->pkginfo_view->is_create)
		setting_view_update(ad->pkginfo_view, ad);
	if (ad->main_view->is_create)
		setting_view_update(ad->main_view, ad);

	evas_object_show(ad->lo_main);
}

static void appmgrUg_on_event(ui_gadget_h ug, enum ug_event event,
		app_control_h service, void *priv)
{
	SettingAppMgrUG *ad = priv;

	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == ad, "ad is NULL");

	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		setting_navi_items_update(ad->navi);
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void appmgrUg_on_key_event(ui_gadget_h ug, enum ug_key_event event,
		app_control_h service, void *priv)
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

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SettingAppMgrUG *ad;

	retv_if(NULL == ops, -1);

	ad = calloc(1, sizeof(SettingAppMgrUG));
	if (NULL == ad) {
		SETTING_TRACE_ERROR("calloc() Fail");
		return -1;
	}

	ops->create = appmgrUg_on_create;
	ops->start = NULL;
	ops->pause = appmgrUg_on_pause;
	ops->resume = appmgrUg_on_resume;
	ops->destroy = appmgrUg_on_destroy;
	ops->message = NULL;
	ops->event = appmgrUg_on_event;
	ops->key_event = appmgrUg_on_key_event;
	ops->priv = ad;
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
	int i, size;
	Setting_Cfg_Node_T *node;
	Eina_List **pplist = priv;
	char ug_args[APPMGRUG_MAX_STR_LEN];
	const Setting_Cfg_Node_T search_configs[] = { {
	MGRAPP_STR_DOWNLOADS,
	NULL,
	NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
	NULL,
	NULL,
	NULL,
	NULL }, {
	MGRAPP_STR_RUNNING,
	NULL,
	NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
	NULL,
	NULL,
	NULL,
	NULL }, {
	MGRAPP_STR_ALL,
	NULL,
	NULL, 0, Cfg_Item_unResetable, 0, Cfg_Item_View_Node,
	NULL,
	NULL,
	NULL,
	NULL }, };

	retv_if(NULL == priv, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	*applocale = strdup("setting:"_TZ_SYS_RO_APP\
			"/org.tizen.setting/res/locale");

	size = sizeof(search_configs) / sizeof(Setting_Cfg_Node_T);

	for (i = 0; i < size; i++) {
		snprintf(ug_args, APPMGRUG_MAX_STR_LEN, "keyword:%s",
				search_configs[i].key_name);
		node = setting_plugin_search_item_subindex_add(
				search_configs[i].key_name,
				ug_args,
				IMG_Applications,
				search_configs[i].item_type,
				search_configs[i].data, "Application Manager");

		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}
