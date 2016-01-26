/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
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
#include <setting-ringtone-main.h>

#define RINGTONE_FILE_PATH	"/opt/usr/share/settings/Ringtones"
#define ALERT_FILE_PATH		"/opt/usr/share/settings/Alerts"
#define RINGTONE_FILE_PATH_DEF	"/opt/usr/share/settings/Ringtones/Over_the_horizon.ogg"
#define ALERT_FILE_PATH_DEF	"/opt/usr/share/settings/Alerts/Beep_Once.ogg"

#define RINGTONE_FILE_PATH_1	"/opt/usr/share/settings/Ringtones/"
#define ALERT_FILE_PATH_1	"/opt/usr/share/settings/Alerts/"

static int setting_ringtone_create(void *cb);
static int setting_ringtone_destroy(void *cb);
static int setting_ringtone_update(void *cb);
static int setting_ringtone_cleanup(void *cb);

setting_view setting_view_ringtone_main = {
	.create = setting_ringtone_create,
	.destroy = setting_ringtone_destroy,
	.update = setting_ringtone_update,
	.cleanup = setting_ringtone_cleanup,
};

static void setting_ringtone_cancel_click_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingRingtoneUG *ad = (SettingRingtoneUG *) data;

	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
}

static void setting_ringtone_done_click_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingRingtoneUG *ad = (SettingRingtoneUG *) data;

	if (ad->sel_file_path) {

		int is_def_seleted = 0;
		if (ad->sel_item_data && !safeStrCmp(ad->sel_item_data->keyStr, STR_DEFAULT_RINGTONE)) {
			is_def_seleted = 1;
		}
		SETTING_TRACE("is_def_seleted: %d", is_def_seleted);

		if (is_def_seleted) {
			/*if default item is seleted, return "default" */
			ad->rlt_file_path = strdup("default");
		} else {
			ad->rlt_file_path = strdup(ad->sel_file_path);
		}

		app_control_h reply;
		if (ad->rlt_file_path && ad->source_svc && !app_control_create(&reply)) {
			SETTING_TRACE("reply rlt: %s", ad->rlt_file_path);
			app_control_add_extra_data(reply, "result", ad->rlt_file_path);
			app_control_reply_to_launch_request(reply, ad->source_svc, APP_CONTROL_RESULT_SUCCEEDED);
			ug_send_result(ad->ug, reply);
			app_control_destroy(reply);
		}
	}

	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
}

char *_ringtone_gl_label_get(void *data, Evas_Object *obj, const char *part)
{
	retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (safeStrCmp(part, "elm.text.main.left"))
		return NULL;

	return (char *)g_strdup(_(item_data->keyStr));
}

Evas_Object *_ringtone_gl_icon_get(void *data, Evas_Object *obj,
                                   const char *part)
{
	retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	/*SettingRingtoneUG *ad = (SettingRingtoneUG *) (item_data->userdata); */

	if (safeStrCmp(part, "elm.icon.right"))
		return NULL;

	Evas_Object *lay = elm_layout_add(obj);

	Evas_Object *radio = elm_radio_add(lay);
	evas_object_propagate_events_set(radio, EINA_FALSE);
	evas_object_repeat_events_set(radio, EINA_TRUE);
	elm_radio_state_value_set(radio, item_data->chk_status);
	elm_radio_group_add(radio, item_data->rgd);
	evas_object_show(radio);
	item_data->eo_check = radio;

	elm_layout_theme_set(lay, "layout", "list/C/type.2", "default");
	elm_layout_content_set(lay, "elm.swallow.content", radio);

	return lay;
}

static void _ringtone_gl_del(void *data, Evas_Object *obj)
{
	Setting_GenGroupItem_Data *item_data =
	    (Setting_GenGroupItem_Data *) data;

	if (item_data) {
		G_FREE(item_data->keyStr);
		G_FREE(item_data->filepath);
		FREE(item_data);
	}
}

static inline void appmgrUg_init_itcs(SettingRingtoneUG *ad)
{
	SETTING_TRACE_BEGIN;
	ad->itc_ring.item_style = "1line";
	ad->itc_ring.func.text_get = _ringtone_gl_label_get;
	ad->itc_ring.func.content_get = _ringtone_gl_icon_get;
	ad->itc_ring.func.state_get = NULL;
	ad->itc_ring.func.del = _ringtone_gl_del;
}

static void ringtone_item_sel(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingRingtoneUG *ad = (SettingRingtoneUG *)data;

	Evas_Object *radio = list_item->eo_check;
	elm_radio_value_set(radio, list_item->chk_status);

	G_FREE(ad->sel_file_path);
	ad->sel_file_path = g_strdup(list_item->filepath);
	ad->sel_item_data = list_item;

	SETTING_TRACE("sel file: %s", ad->sel_file_path);
	if (ad->mp_ringtone) {
		ringtone_stop_sound(ad);
	}
	if (safeStrCmp(list_item->keyStr, "IDS_ST_BODY_PHONEPROFILES_SILENT")) {
		ringtone_play_sound(ad->sel_file_path, &ad->mp_ringtone);
	}
}

static int _compare_cb(const void *d1, const void *d2)
{
	const fileNodeInfo *pNode1 = (const fileNodeInfo *)d1;
	const fileNodeInfo *pNode2 = (const fileNodeInfo *)d2;

	return safeStrCmp(pNode1->media_name, pNode2->media_name);
}

char *get_default_filepath(SettingRingtoneUG *ad)
{
	retv_if(ad == NULL, NULL);

	char *default_ringtone = NULL;

	if (!safeStrCmp(ad->dir_path, RINGTONE_FILE_PATH) ||
	    !safeStrCmp(ad->dir_path, RINGTONE_FILE_PATH_1)) {

		default_ringtone = vconf_get_str(VCONFKEY_SETAPPL_CALL_RINGTONE_PATH_STR);
		if (default_ringtone && ecore_file_exists(default_ringtone)) {
			return default_ringtone;
		} else {
			FREE(default_ringtone);
			default_ringtone = g_strdup(RINGTONE_FILE_PATH_DEF);
		}

	} else {

		default_ringtone = vconf_get_str(VCONFKEY_SETAPPL_NOTI_RINGTONE_PATH_STR);
		if (default_ringtone && ecore_file_exists(default_ringtone)) {
			return default_ringtone;
		} else {
			FREE(default_ringtone);
			default_ringtone = g_strdup(ALERT_FILE_PATH_DEF);
		}
	}

	return default_ringtone;
}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_ringtone_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingRingtoneUG *ad = (SettingRingtoneUG *) cb;

	ad->ly_main = setting_create_layout_navi_bar_genlist(ad->win_main_layout,
	                                                     ad->win_get,
	                                                     STR_RINGTONE_SELECT,
	                                                     "IDS_ST_BUTTON_BACK",
	                                                     NULL,
	                                                     NULL,
	                                                     NULL, ad, &ad->scroller,
	                                                     &(ad->navi_bar));
	Elm_Object_Item *navi_it;
	navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	elm_naviframe_item_pop_cb_set(navi_it, setting_ringtone_click_back_cb, ad);

	Evas_Object *btn_cancel = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn_cancel, "naviframe/title_cancel");
	evas_object_smart_callback_add(btn_cancel, "clicked", setting_ringtone_cancel_click_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_left_btn", btn_cancel);

	Evas_Object *btn_done = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn_done, "naviframe/title_done");
	evas_object_smart_callback_add(btn_done, "clicked", setting_ringtone_done_click_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_right_btn", btn_done);

	appmgrUg_init_itcs(ad);

	/*Get file list */
	Eina_List *filelist = NULL;
	int ret = get_filelist_from_dir_path(ad->dir_path, &filelist);
	if (ret != 0) {
		SETTING_TRACE_ERROR("Failed to get filelist, ret = %d", ret);
	}
	filelist = eina_list_sort(filelist,
	                          eina_list_count(filelist), _compare_cb);
	ad->filelist = filelist;

	Eina_List *l;
	fileNodeInfo *pNode;
	Elm_Object_Item *cur_item = NULL;
	Setting_GenGroupItem_Data *item_data = NULL;
	int cnt = 0;
	int cur_pos = -1; /*current selected item */
	char fullPath[512] = {0, };

	ad->ring_rgd = elm_radio_add(ad->scroller);
	elm_radio_state_value_set(ad->ring_rgd, -1);

	/*create default item */
	if (ad->is_show_def) {
		item_data = (Setting_GenGroupItem_Data *)calloc(1, sizeof(Setting_GenGroupItem_Data));
		if (item_data) {
			item_data->userdata = ad;
			item_data->keyStr = g_strdup(STR_DEFAULT_RINGTONE);
			item_data->rgd = ad->ring_rgd;
			item_data->chk_status = cnt;
			item_data->filepath = get_default_filepath(ad);
			item_data->item = elm_genlist_item_append(ad->scroller, &(ad->itc_ring), item_data, NULL,
			                                          ELM_GENLIST_ITEM_NONE, ringtone_item_sel, ad);

			if (!safeStrCmp("default", ad->file_path)) {
				cur_pos = cnt;
				cur_item = item_data->item;
			}
			cnt++;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}

	/*create silent item */
	if (ad->is_show_silent) {
		item_data = (Setting_GenGroupItem_Data *)calloc(1, sizeof(Setting_GenGroupItem_Data));
		if (item_data) {
			item_data->userdata = ad;
			item_data->keyStr = g_strdup("IDS_ST_BODY_PHONEPROFILES_SILENT");
			item_data->rgd = ad->ring_rgd;
			item_data->chk_status = cnt;
			item_data->filepath = g_strdup("silent");
			item_data->item = elm_genlist_item_append(ad->scroller, &(ad->itc_ring), item_data, NULL,
			                                          ELM_GENLIST_ITEM_NONE, ringtone_item_sel, ad);

			if (!safeStrCmp("silent", ad->file_path)) {
				cur_pos = cnt;
				cur_item = item_data->item;
			}
			cnt++;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}

	/*create items in specified dir */
	EINA_LIST_FOREACH(filelist, l, pNode) {
		if (pNode) {
			if (pNode->path && pNode->name) {
				memset(fullPath, 0x00, sizeof(fullPath));
				snprintf(fullPath, sizeof(fullPath), "%s/%s", pNode->path, pNode->name);
			}
		} else {
			continue;
		}
		item_data = (Setting_GenGroupItem_Data *)calloc(1, sizeof(Setting_GenGroupItem_Data));
		if (item_data) {
			item_data->userdata = ad;
			item_data->keyStr = g_strdup(pNode->media_name);
			item_data->rgd = ad->ring_rgd;
			item_data->chk_status = cnt;
			item_data->filepath = g_strdup(fullPath);
			item_data->item = elm_genlist_item_append(ad->scroller, &(ad->itc_ring), item_data, NULL,
			                                          ELM_GENLIST_ITEM_NONE, ringtone_item_sel, ad);

			if (!safeStrCmp(fullPath, ad->file_path)) {
				cur_pos = cnt;
				cur_item = item_data->item;
			}
			cnt++;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
	}
	elm_radio_value_set(ad->ring_rgd, cur_pos);
	if (cur_item) {
		elm_genlist_item_bring_in(cur_item, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);
	}

	setting_view_ringtone_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_ringtone_destroy(void *cb)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingRingtoneUG *ad = (SettingRingtoneUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		setting_view_ringtone_main.is_create = 0;
	}

	if (ad->mp_ringtone) {
		ringtone_stop_sound(ad);
	}

	FREE(ad->dir_path);
	FREE(ad->file_path);
	G_FREE(ad->sel_file_path);

	/*free eina list */
	if (ad->filelist) {
		Eina_List *l = NULL;
		fileNodeInfo *node = NULL;
		EINA_LIST_FOREACH(ad->filelist, l, node) {
			FREE(node->path);
			FREE(node->name);
			G_FREE(node->media_name);
			FREE(node);
		}
		eina_list_free(ad->filelist);
		ad->filelist = NULL;
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_ringtone_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingRingtoneUG *ad = (SettingRingtoneUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
	}
	return SETTING_RETURN_SUCCESS;

}

static int setting_ringtone_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingRingtoneUG *ad = (SettingRingtoneUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_hide(ad->ly_main);
	}
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static Eina_Bool
setting_ringtone_click_back_cb(void *data, Elm_Object_Item *it)
{
	setting_retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");

	SettingRingtoneUG *ad = (SettingRingtoneUG *) data;

	ug_destroy_me(ad->ug);
	return EINA_FALSE;
}
