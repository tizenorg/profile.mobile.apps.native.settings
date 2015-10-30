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
#include <appsvc.h>
#include <efl_extension.h>

#include "setting-appmgr-utils.h"
#include "setting-appmgr-runinfo.h"
#include "setting-appmgr-async-worker.h"
#include "setting-appmgr-main.h"

setting_view setting_view_appmgr_main;

static void appmgrUg_main_sort_sel(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingAppMgrUG *ad = data;
	Elm_Object_Item *item = event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	ret_if(data == NULL);
	ret_if(event_info == NULL);
	ret_if(list_item == NULL);

	const char *item_str = list_item->keyStr;
	SETTING_TRACE_ERROR("str: %s", item_str);
	if (!safeStrCmp(item_str, MGRAPP_STR_SIZE)) {
		ad->sort_str = (char *)strdup(MGRAPP_STR_SIZE);
		ad->sorttype = APPMGRUG_SORT_SIZE;
		elm_radio_value_set(ad->sort_rdg, 2);
	} else if (!safeStrCmp(item_str, MGRAPP_STR_Z_TO_A)) {
		ad->sort_str = (char *)strdup(MGRAPP_STR_Z_TO_A);
		ad->sorttype = APPMGRUG_SORT_ZTOA;
		elm_radio_value_set(ad->sort_rdg, 1);
	} else {
		ad->sort_str = (char *)strdup(MGRAPP_STR_A_TO_Z);
		ad->sorttype = APPMGRUG_SORT_ATOZ;
		elm_radio_value_set(ad->sort_rdg, 0);
	}

	ad->pkg_list = appmgrUg_sort_pkg_list(ad->sorttype, ad->pkg_list);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	appmgrUg_main_genlist_append_items(ad);
}

static void appmgrUg_main_sort_popup(void *data, Evas_Object *obj,
                                     void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");

	SettingAppMgrUG *ad = (SettingAppMgrUG *) data;

	if (ad->popup)
		evas_object_del(ad->popup);

	Evas_Object *menu_glist = NULL;
	ad->popup = setting_create_popup_with_list(&menu_glist, ad, ad->win,
												MGRAPP_STR_SORT_BY, NULL, 0, false, false, 0);

	Evas_Object *rdg = elm_radio_add(menu_glist);
	elm_object_style_set(rdg, "list");
	evas_object_propagate_events_set(rdg, EINA_TRUE);
	elm_radio_state_value_set(rdg, -1);
	evas_object_data_set(menu_glist, "radio", rdg);
	ad->sort_rdg = rdg;

	setting_create_Gendial_field_1radio(menu_glist, &(itc_1text_1icon),
										appmgrUg_main_sort_sel,
										ad,
										SWALLOW_Type_1RADIO_RIGHT,
										rdg, 0,
										MGRAPP_STR_A_TO_Z,
										NULL);

	setting_create_Gendial_field_1radio(menu_glist, &(itc_1text_1icon),
										appmgrUg_main_sort_sel,
										ad,
										SWALLOW_Type_1RADIO_RIGHT,
										rdg, 1,
										MGRAPP_STR_Z_TO_A,
										NULL);

	setting_create_Gendial_field_1radio(menu_glist, &(itc_1text_1icon),
										appmgrUg_main_sort_sel,
										ad,
										SWALLOW_Type_1RADIO_RIGHT,
										rdg, 2,
										MGRAPP_STR_SIZE,
										NULL);
	SETTING_TRACE("ad->sort_str:%s", ad->sort_str);
	if (!safeStrCmp(ad->sort_str, MGRAPP_STR_SIZE)) {
		elm_radio_value_set(rdg, 2);
	} else if (!safeStrCmp(ad->sort_str, MGRAPP_STR_Z_TO_A)) {
		elm_radio_value_set(rdg, 1);
	} else {
		elm_radio_value_set(rdg, 0);
	}
}

static void appmgrUg_main_more_popup_rotate(void *data, Evas_Object *obj,
                                            void *event_info)
{
	int pos;
	Evas_Coord w, h;
	SettingAppMgrUG *ad = data;

	ret_if(data == NULL);

	elm_win_screen_size_get(ad->win, NULL, NULL, &w, &h);

	pos = elm_win_rotation_get(ad->win);
	switch (pos) {
		case 90:
			evas_object_move(ad->popup, 0, w);
			break;
		case 270:
			evas_object_move(ad->popup, h, w);
			break;
		case 180:
		default:
			evas_object_move(ad->popup, 0, h);
			break;
	}
}

static void appmgrUg_main_more_popup_del_cb(void *data, Evas *e,
                                            Evas_Object *obj, void *event_info)
{
	SettingAppMgrUG *ad = data;

	ret_if(data == NULL);

	evas_object_smart_callback_del(elm_object_top_widget_get(ad->popup),
	                               "rotation,changed", appmgrUg_main_more_popup_rotate);
}

static char *_appmgrUg_access_info_prepend_cb(void *data, Evas_Object *obj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data, NULL);
	char str[256];

	sprintf(str, _("IDS_CAM_BODY_SHOWING_PD_ITEMS_T_TTS"), data);
	return strdup(_(str));
}

static void appmgrUg_main_create_more_popup(void *data, Evas_Object *obj,
                                            void *event_info)
{
	Elm_Object_Item *it;
	Evas_Object *ctxpopup;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);

	ctxpopup = elm_ctxpopup_add(ad->navi);
	elm_object_style_set(ctxpopup, "more/default");
	elm_ctxpopup_auto_hide_disabled_set(ctxpopup, EINA_TRUE);
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);
	eext_object_event_callback_add(ctxpopup, EEXT_CALLBACK_MORE, eext_ctxpopup_back_cb, NULL);

	evas_object_smart_callback_add(ctxpopup, "dismissed", appmgrUg_popup_del, ad);
	evas_object_smart_callback_add(elm_object_top_widget_get(ctxpopup),
	                               "rotation,changed", appmgrUg_main_more_popup_rotate, ad);
	evas_object_event_callback_add(ctxpopup, EVAS_CALLBACK_DEL,
	                               appmgrUg_main_more_popup_del_cb, ad);

	it = elm_ctxpopup_item_append(ctxpopup, MGRAPP_STR_SORT_BY, NULL,
	                              appmgrUg_main_sort_popup, ad);
	elm_object_item_domain_text_translatable_set(it, SETTING_PACKAGE, EINA_TRUE);

	if (ad->popup)
		evas_object_del(ad->popup);
	ad->popup = ctxpopup;

	appmgrUg_main_more_popup_rotate(ad, NULL, NULL);

	evas_object_show(ctxpopup);
}

static void appmgrUg_main_gl_realized(void *data, Evas_Object *obj,
                                      void *event_info)
{
	setting_retm_if(event_info == NULL, "invalid parameter: event_info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	appmgr_listinfo *list_item = (appmgr_listinfo *)elm_object_item_data_get(item);

	// setting_check_genlist_item_bottom_line(item);
}

static Evas_Object *appmgrUg_main_genlist(Evas_Object *parent)
{
	Evas_Object *genlist;

	genlist = elm_genlist_add(parent);
	if (NULL == genlist) {
		SETTING_TRACE_ERROR("elm_genlist_add() Fail");
		return NULL;
	}

	elm_genlist_block_count_set(genlist, 8);
	/* set true will consume many time to realize genlist at first time.
	elm_genlist_realization_mode_set(genlist, EINA_TRUE);
	*/
	evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_scroller_policy_set(genlist, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(genlist);

	evas_object_smart_callback_add(genlist, "realized", appmgrUg_main_gl_realized, NULL);

	return genlist;
}

static void appmgrUg_main_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	appmgr_listinfo *info;
	SettingAppMgrUG *ad = data;
	Elm_Object_Item *item = event_info;

	ret_if(data == NULL);
	ret_if(event_info == NULL);

	elm_genlist_item_selected_set(item, 0);

	info = elm_object_item_data_get(item);
	ret_if(NULL == info);

	ad->sel_pkgid = SAFE_STRDUP(info->pkgid);
	ad->sel_label = SAFE_STRDUP(info->pkg_label);
	ad->sel_icon = SAFE_STRDUP(info->icon_path);
	ad->sel_is_disabled = info->is_disabled;

	/* create app info view */
	if (APPMGRUG_TAB_RUNNING == ad->tabtype) {
		ad->info_run = info;
		ad->runinfos = info->runinfos;
		setting_view_change(ad->main_view, ad->runinfo_view, ad);
	} else {
		if (info->valid_size)
			ad->sel_total = info->total_size;
		else
			ad->sel_total = -1;
		ad->sel_data_size = -1;
		setting_view_change(ad->main_view, ad->pkginfo_view, ad);
	}
}

static inline Evas_Object *appmgrUg_main_no_item_handle(SettingAppMgrUG *ad)
{
	const char *text, *help_txt = NULL;
	Evas_Object *lo = NULL;

	switch (ad->tabtype) {
		case APPMGRUG_TAB_RUNNING:
			text = MGRAPP_STR_LOADING;
			break;
		case APPMGRUG_TAB_DISABLED:
			text = MGRAPP_STR_NO_APPS;
			break;
		case APPMGRUG_TAB_DOWNLOAD:
		case APPMGRUG_TAB_ALL:
		default:
			text = MGRAPP_STR_NO_APPS;
			help_txt = MGRAPP_STR_NO_DOWNLOAD_APPS_HELP;
			break;
	}

	if (NULL == ad->lo_noitem || ad->noitem_type != ad->tabtype) {
		if (ad->tabtype == APPMGRUG_TAB_RUNNING) {
			lo = appmgrUg_loading_item(ad->navi, text, help_txt);
		} else {
			lo = appmgrUg_no_item(ad->navi, text, help_txt);
		}
	}

	return lo;
}

void appmgrUg_main_genlist_append_items(SettingAppMgrUG *ad)
{
	GList *cursor;
	int item_cnt = 0;
	Evas_Object *lo_new = NULL;
	appmgr_listinfo *info = NULL;

	/*For first Empty view */
	if (NULL == ad->pkg_list) {
		lo_new = appmgrUg_no_item(ad->navi, MGRAPP_STR_CHECKING, "");
		elm_object_item_part_content_set(ad->navi_main_it, "elm.swallow.content", lo_new);
		ad->lo_noitem = lo_new;
		return;
	}

	if (NULL == ad->gl_main) {
		lo_new = ad->gl_main = appmgrUg_main_genlist(ad->navi);
	} else
		elm_genlist_clear(ad->gl_main);

	cursor = ad->pkg_list;
	while (cursor && (ad->tabtype != APPMGRUG_TAB_RUNNING || ad->is_runlist_ready)) {
		int invalid;

		info = cursor->data;
		cursor = cursor->next;

		if (NULL == info) {
			SETTING_TRACE_ERROR("info is a null");
			continue;
		}

		invalid = 0;
		switch (ad->tabtype) {
			case APPMGRUG_TAB_DOWNLOAD:
				if (info->is_preload && !info->is_update)
					invalid = 1;
				break;
			case APPMGRUG_TAB_RUNNING:
				if (NULL == info->runinfos)
					invalid = 1;
				break;
			case APPMGRUG_TAB_DISABLED:
				if (0 == info->is_disabled)
					invalid = 1;
				break;
		}
		if (invalid) {
			info->item = NULL;
			continue;
		}

		if (ad->gl_main) {
			info->item = elm_genlist_item_append(ad->gl_main, &ad->itc_main, info, NULL,
			                                     ELM_GENLIST_ITEM_NONE, appmgrUg_main_gl_sel, ad);
			item_cnt++;
		}
	}

	if (0 == item_cnt) {
		if (lo_new)
			evas_object_del(lo_new);

		lo_new = appmgrUg_main_no_item_handle(ad);

		if (lo_new)
			ad->lo_noitem = lo_new;
		ad->noitem_type = ad->tabtype;
		ad->gl_main = NULL;
	}

	if (lo_new) {
		elm_object_item_part_content_set(ad->navi_main_it, "elm.swallow.content", lo_new);
		if (ad->gl_main)
			ad->lo_noitem = NULL;
	}
}

char *appmgrUg_main_gl_label_new_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE(" ------------> label get [%s]",part);

	char *label = NULL;
	appmgr_listinfo *info = data;

	retv_if(data == NULL, NULL);

	if (0 == strcmp(part, "elm.text")) {
		label = SAFE_STRDUP(info->pkg_label);
	} else if (0 == strcmp(part, "elm.text.sub")) {
		char desc[APPMGRUG_MAX_STR_LEN] = {0};
		if (info->valid_size) {
			appmgrUg_size_to_str(info->total_size, desc, sizeof(desc));
			label = strdup(desc);
		} else {
			label = strdup(_(MGRAPP_STR_COMPUTING));
		}
	}

	return label;
}

char *appmgrUg_main_gl_label_get(void *data, Evas_Object *obj, const char *part)
{
	char *label = NULL;
	appmgr_listinfo *info = data;

	retv_if(data == NULL, NULL);

	if (0 == strcmp(part, "elm.text.main.left.top")) {
		label = SAFE_STRDUP(info->pkg_label);
	} else if (0 == strcmp(part, "elm.text.sub.left.bottom")) {
		char desc[APPMGRUG_MAX_STR_LEN] = {0};
		if (info->valid_size) {
			appmgrUg_size_to_str(info->total_size, desc, sizeof(desc));
			label = strdup(desc);
		} else {
			label = strdup(_(MGRAPP_STR_COMPUTING));
		}
	}

	return label;
}

static inline char *appmgrUg_get_listinfo_default_icon(const char *appid)
{
	int ret;
	char *icon;
	pkgmgrinfo_appinfo_h handle = NULL;

	ret = pkgmgrinfo_appinfo_get_appinfo(appid, &handle);
	warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_get_appinfo() Fail(%d)", ret);

	icon = appmgrUg_get_defualt_icon(handle);

	pkgmgrinfo_appinfo_destroy_appinfo(handle);

	return icon;
}


Evas_Object *appmgrUg_main_gl_icon_new_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE(" ------------> content get [%s]",part);

	Evas_Object *icon = NULL;
	Evas_Object *lay = NULL;
	appmgr_listinfo *info = data;

	retv_if(data == NULL, NULL);

	if (!safeStrCmp(part, "elm.swallow.icon")) {
		icon = elm_icon_add(obj);

		if (NULL == info->icon_path)
			info->icon_path = appmgrUg_get_listinfo_default_icon(info->mainappid);

		elm_image_file_set(icon, info->icon_path, NULL);
		evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

		lay = elm_layout_add(obj);
		retv_if(lay == NULL, NULL);
		elm_layout_theme_set(lay, "layout", "list/B/type.2", "default");
		elm_layout_content_set(lay, "elm.swallow.content", icon);

		return lay;
	} else {
		return NULL;
	}
}



Evas_Object *appmgrUg_main_gl_icon_get(void *data, Evas_Object *obj,
                                       const char *part)
{
	Evas_Object *icon = NULL;
	Evas_Object *lay = NULL;
	appmgr_listinfo *info = data;

	retv_if(data == NULL, NULL);

	if (!safeStrCmp(part, "elm.icon.1")) {
		icon = elm_icon_add(obj);

		if (NULL == info->icon_path)
			info->icon_path = appmgrUg_get_listinfo_default_icon(info->mainappid);

		elm_image_file_set(icon, info->icon_path, NULL);
		evas_object_size_hint_weight_set(icon, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);

		lay = elm_layout_add(obj);
		retv_if(lay == NULL, NULL);
		elm_layout_theme_set(lay, "layout", "list/B/type.2", "default");
		elm_layout_content_set(lay, "elm.swallow.content", icon);

		return lay;
	} else {
		return NULL;
	}
}


static Eina_Bool appmgrUg_main_back_cb(void *data, Elm_Object_Item *it)
{
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, EINA_TRUE);

	appmgrUg_stop_async_worker_all();
	if (ad->list_worker)
		ad->list_worker = NULL;

	if (ad->reset_worker) {
		SETTING_TRACE_ERROR("It can't be here(ad->reset_worker Must be NULL)");
		ad->reset_worker = NULL;
	}

	ug_destroy_me(ad->ug);

	return EINA_FALSE;
}

static void appmgrUg_main_clear_defapp(void *data, Evas_Object *obj,
                                       void *event_info)
{
	int ret;
	char *btn_str;
	SettingAppMgrUG *ad = data;

	ret_if(data == NULL);

	btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	ret = safeStrCmp(btn_str, _(MGRAPP_STR_OK));
	free(btn_str);

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	if (0 != ret)
		return;

	ret = appsvc_unset_all_defapps();
	if (APPSVC_RET_OK != ret) {
		SETTING_TRACE_ERROR("appsvc_unset_all_defapps() Fail(%d)", ret);
		appmgrUg_fail_popup(MGRAPP_STR_ERROR, ad);
	}

	setting_view_update(ad->main_view, ad);
}

static void appmgrUg_main_clear_defapp_click(void *data, Evas_Object *obj,
                                             void *event_info)
{
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);

	if (ad->popup)
		evas_object_del(ad->popup);

	ad->popup = setting_create_popup(ad, ad->win, NULL,
									 MGRAPP_STR_CLEAR_DEFAULT_APPS_Q, appmgrUg_main_clear_defapp, 0, FALSE, FALSE,
									 2, MGRAPP_STR_OK, MGRAPP_STR_CANCEL);
}

static inline Evas_Object *appmgrUg_main_clear_defapp_toolbar(
    Evas_Object *parent, SettingAppMgrUG *ad)
{
	Evas_Object *toolbar;
	Elm_Object_Item *tool_it;

	toolbar = elm_toolbar_add(parent);

	elm_object_style_set(toolbar, "default");
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_NONE);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);

	tool_it = elm_toolbar_item_append(toolbar, NULL, MGRAPP_STR_CLEAR_DEFAULT_APPS,
	                                  appmgrUg_main_clear_defapp_click, ad);
	elm_object_item_domain_text_translatable_set(tool_it, SETTING_PACKAGE, EINA_TRUE);

	evas_object_show(toolbar);

	return toolbar;
}

static void appmgrUg_download_create(void *data, Evas_Object *obj,
                                     void *event_info)
{
	Evas_Object *toolbar;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(APPMGRUG_TAB_DOWNLOAD == ad->tabtype);

	ad->tabtype = APPMGRUG_TAB_DOWNLOAD;
	/*toolbar = elm_object_item_part_content_unset(ad->navi_main_it, "toolbar"); */
	/*evas_object_del(toolbar); */

	appmgrUg_main_genlist_append_items(ad);
}

static void appmgrUg_disabled_create(void *data, Evas_Object *obj,
                                     void *event_info)
{
	Evas_Object *toolbar;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(APPMGRUG_TAB_DISABLED == ad->tabtype);

	ad->tabtype = APPMGRUG_TAB_DISABLED;
	/*toolbar = elm_object_item_part_content_unset(ad->navi_main_it, "toolbar"); */
	/*evas_object_del(toolbar); */

	appmgrUg_main_genlist_append_items(ad);
}

static void appmgrUg_running_create(void *data, Evas_Object *obj,
                                    void *event_info)
{
	Evas_Object *toolbar;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(APPMGRUG_TAB_RUNNING == ad->tabtype);

	ad->tabtype = APPMGRUG_TAB_RUNNING;
	/*toolbar = elm_object_item_part_content_unset(ad->navi_main_it, "toolbar"); */
	/*evas_object_del(toolbar); */

	ad->is_runlist_ready = EINA_FALSE;
	/*ad->running_list_worker = appmgrUg_start_async_worker(appmgrUg_get_running_list, appmgrUg_get_runlistinfos_cb, ad); */
	appmgrUg_get_running_list(ad);

	appmgrUg_main_genlist_append_items(ad);
}

static void appmgrUg_all_create(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *toolbar;
	SettingAppMgrUG *ad = data;

	ret_if(NULL == data);
	ret_if(APPMGRUG_TAB_ALL == ad->tabtype);

	ad->tabtype = APPMGRUG_TAB_ALL;
	/*toolbar = elm_object_item_part_content_unset(ad->navi_main_it, "toolbar"); */
	/*evas_object_del(toolbar); */

	appmgrUg_main_genlist_append_items(ad);
}

static inline Evas_Object *appmgrUg_main_create_top_tabbar(
    Evas_Object *parent, SettingAppMgrUG *ad)
{
	int tabtype;
	Evas_Object *toolbar;
	Elm_Object_Item *it[APPMGRUG_TAB_MAX];

	if (ad->tabtype <= APPMGRUG_TAB_NONE || APPMGRUG_TAB_MAX <= ad->tabtype)
		tabtype = APPMGRUG_TAB_DOWNLOAD;
	else
		tabtype = ad->tabtype;

	ad->tabtype = APPMGRUG_TAB_NONE;

	toolbar = elm_toolbar_add(parent);

	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_object_style_set(toolbar, "tabbar");
	elm_toolbar_reorder_mode_set(toolbar, EINA_FALSE);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);

	it[APPMGRUG_TAB_DOWNLOAD] = elm_toolbar_item_append(toolbar, NULL,
	                                                    MGRAPP_STR_DOWNLOADS, appmgrUg_download_create, ad);
	elm_object_item_domain_text_translatable_set(it[APPMGRUG_TAB_DOWNLOAD], SETTING_PACKAGE, EINA_TRUE);

	it[APPMGRUG_TAB_RUNNING] = elm_toolbar_item_append(toolbar, NULL,
	                                                   MGRAPP_STR_RUNNING, appmgrUg_running_create, ad);
	elm_object_item_domain_text_translatable_set(it[APPMGRUG_TAB_RUNNING], SETTING_PACKAGE, EINA_TRUE);

	it[APPMGRUG_TAB_ALL] = elm_toolbar_item_append(toolbar, NULL,
	                                               MGRAPP_STR_ALL, appmgrUg_all_create, ad);
	elm_object_item_domain_text_translatable_set(it[APPMGRUG_TAB_ALL], SETTING_PACKAGE, EINA_TRUE);

	it[APPMGRUG_TAB_DISABLED] = elm_toolbar_item_append(toolbar, NULL,
	                                                    MGRAPP_STR_DISABLED, appmgrUg_disabled_create, ad);
	elm_object_item_domain_text_translatable_set(it[APPMGRUG_TAB_DISABLED], SETTING_PACKAGE, EINA_TRUE);

	elm_toolbar_item_selected_set(it[tabtype], EINA_TRUE);

	evas_object_show(toolbar);

	return toolbar;
}

static inline Evas_Object *appmgrUg_main_creat_more_btn(Evas_Object *parent,
                                                        SettingAppMgrUG *ad)
{
	Evas_Object *btn;

	btn = elm_button_add(parent);
	elm_object_style_set(btn, "naviframe/more/default");

	evas_object_smart_callback_add(btn, "clicked", appmgrUg_main_create_more_popup, ad);

	return btn;
}

static Eina_Bool
setting_appmgr_main_click_softkey_back_cb(void *data, Evas_Object *obj,
                                          void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, EINA_FALSE,
	                 "[Setting > APPMGR] Data parameter is NULL");
	SettingAppMgrUG *ad = (SettingAppMgrUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;
}

static int appmgrUg_main_create(void *data)
{
	Elm_Object_Item *navi_it;
	Evas_Object *back_btn, *toolbar, *more_btn;
	SettingAppMgrUG *ad = data;

	retv_if(NULL == data, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(NULL == ad->navi, SETTING_GENERAL_ERR_WRONG_PARAMETER);

	ad->list_worker = appmgrUg_start_async_worker(appmgrUg_get_listinfos,
	                                              appmgrUg_get_listinfos_cb, ad);

	/* back button */
	back_btn = setting_create_button(ad->navi, MGRAPP_STR_APP_MANAGER,
                                     NAVI_BACK_ARROW_BUTTON_STYLE,
	                                 setting_appmgr_main_click_softkey_back_cb, ad);

	navi_it = elm_naviframe_item_push(ad->navi, MGRAPP_STR_APP_MANAGER,
	                                  back_btn, NULL, NULL, "tabbar");
	elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE, EINA_TRUE);
	elm_naviframe_item_pop_cb_set(navi_it, appmgrUg_main_back_cb, ad);
	ad->navi_main_it = navi_it;

	toolbar = appmgrUg_main_create_top_tabbar(ad->navi, ad);
	if (NULL == toolbar) {
		SETTING_TRACE_ERROR("appmgrUg_main_create_top_tabbar() Fail");
		return SETTING_RETURN_FAIL;
	}
	elm_object_item_part_content_set(navi_it, "tabbar", toolbar);
	setting_tabbar_enable_swip_effect(ad->lo_main, toolbar);

	more_btn = appmgrUg_main_creat_more_btn(ad->navi, ad);
	if (NULL == more_btn) {
		SETTING_TRACE_ERROR("appmgrUg_main_creat_more_btn() Fail");
		return SETTING_RETURN_FAIL;
	}
	elm_object_item_part_content_set(navi_it, "toolbar_more_btn", more_btn);

	appmgrUg_pkgmgr_subscribe(ad);

	setting_view_appmgr_main.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_main_destroy(void *data)
{
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	appmgrUg_stop_async_worker_all();
	if (ad->list_worker)
		ad->list_worker = NULL;

	/*if (ad->running_list_worker) */
	/*		ad->running_list_worker = NULL; */

	if (ad->reset_worker) {
		SETTING_TRACE_ERROR("It can't be here(ad->reset_worker Must be NULL)");
		ad->reset_worker = NULL;
	}

	appmgrUg_pkgmgr_deinit(ad);

#if 0
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
#endif

	appmgrUg_free_listinfos(ad);

	setting_view_appmgr_main.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_main_update(void *data)
{
	SettingAppMgrUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	appmgrUg_main_genlist_append_items(ad);

	return SETTING_RETURN_SUCCESS;
}

static int appmgrUg_main_cleanup(void *data)
{
	return SETTING_RETURN_SUCCESS;
}

void appmgrUg_main_init(SettingAppMgrUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->main_view);

	setting_view_appmgr_main.create = appmgrUg_main_create;
	setting_view_appmgr_main.destroy = appmgrUg_main_destroy;
	setting_view_appmgr_main.update = appmgrUg_main_update;
	setting_view_appmgr_main.cleanup = appmgrUg_main_cleanup;

	ad->main_view = &setting_view_appmgr_main;
}
