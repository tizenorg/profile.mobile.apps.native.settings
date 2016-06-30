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
#include <storage.h>
#include <media_content.h>
#include <tzplatform_config.h>

#include "setting-storage-async-worker.h"
#include "setting-storage-utils.h"

void storageUg_popup_del(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;

	ret_if(data == NULL);

	evas_object_del(ad->popup);
	ad->popup = NULL;
}

void storageUg_get_internal_storage_status(double *total, double *avail)
{
	int ret;
	double tmp_total;
	struct statvfs s;
	const unsigned int GB = (1024 * 1024) * 1024;
	const double sz_32G = 32. * GB;
	const double sz_16G = 16. * GB;
	const double sz_8G = 8. * GB;

	ret_if(NULL == total);
	ret_if(NULL == avail);

	ret = storage_get_internal_memory_size(&s);
	if (0 == ret) {
		SETTING_TRACE("Total = %lu, Available = %lu",
				(s.f_frsize * s.f_blocks),
				(s.f_bsize * s.f_bavail));
		tmp_total = (double) s.f_frsize * s.f_blocks;
#if 0
		*avail = (double)s.f_bfree * s.f_frsize;
#else
		*avail = (double) s.f_bsize * s.f_bavail;
#endif
		if (sz_16G < tmp_total)
			*total = sz_32G;
		else if (sz_8G < tmp_total)
			*total = sz_16G;
		else
			*total = sz_8G;
	}
}

void storageUg_get_external_storage_status(const char *path, double *total,
		double *avail)
{
	struct statvfs s;

	ret_if(NULL == path);
	ret_if(NULL == total);
	ret_if(NULL == avail);

	if (!storage_get_external_memory_size(&s)) {
		SETTING_TRACE("f_frsize = %ld f_blocks = %ld f_bsize = %ld "
				"f_bavail = %ld ",
				s.f_frsize, s.f_blocks, s.f_bsize, s.f_bavail);
		*total = (double) s.f_frsize * s.f_blocks;
#if 0
		*avail = (double)s.f_bsize * s.f_bavail;
#else
		*avail = (double) s.f_bfree * s.f_frsize;
		SETTING_TRACE("NEW STYLE, %ld", *avail);
#endif
	}
}

void storageUg_size_to_str(double size, char *desc, int desc_size)
{
	double tmp_size = 0.0;
	const int KILOBYTE_VALUE = 1024;
	const int MEGABYTE_VALUE = KILOBYTE_VALUE * 1024;
	const int GIGABYTE_VALUE = MEGABYTE_VALUE * 1024;

	if (size < MEGABYTE_VALUE) { /* size < 1MB: show x.xKB */
		tmp_size = size / KILOBYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(STORAGEUG_STR_KB));
	} else if (size < GIGABYTE_VALUE) { /* size < 1GB: show x.xMB */
		tmp_size = size / MEGABYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(STORAGEUG_STR_MB));
	} else { /* 1G <= size: show x.xGB */
		tmp_size = size / GIGABYTE_VALUE;
		snprintf(desc, desc_size, "%4.2lf%s", tmp_size,
				_(STORAGEUG_STR_GB));
	}
}

void storageUg_ug_layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	Evas_Object *base = NULL;

	ret_if(priv == NULL);

	base = ug_get_layout(ug);
	if (base == NULL) {
		SETTING_TRACE_ERROR("ug_get_layout() Fail");
		return;
	}

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		evas_object_show(base);
		break;
	default:
		/* do nothing */
		break;
	}
}

void storageUg_ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	SettingStorageUG *ad = priv;

	ret_if(priv == NULL);

	if (ug)
		setting_ug_destroy(ug);

	elm_object_tree_focus_allow_set(ad->lo_main, EINA_TRUE);
	setting_view_update(ad->main_view, ad);
}

void storageUg_fail_popup(SettingStorageUG *ad)
{
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	ad->popup = setting_create_popup(ad, ad->win, NULL,
	STORAGEUG_STR_FAIL, storageUg_popup_del,
	SETTING_STORAGE_POPUP_TIMER, FALSE, FALSE, 0);
}




static Eina_Bool storageUg_manage_apps_freeze_event_timer_cb(
		void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingStorageUG *ad = (SettingStorageUG *)cb;

	evas_object_freeze_events_set(ad->navi, EINA_FALSE);

	ad->event_freeze_timer = NULL;
	SETTING_TRACE_END;
	return EINA_FALSE;
}


void storageUg_manage_apps_ug(SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;

	ret_if(NULL == ad);

	if (app_launcher(
			"setting-manage-applications-efl|viewtype:manage-applications", NULL, NULL)
			== 0) {
		ad->event_freeze_timer = ecore_timer_add(1,
				storageUg_manage_apps_freeze_event_timer_cb,
				ad);
		evas_object_freeze_events_set(ad->navi, EINA_TRUE);
	}
}



struct _calculated_sizes {
	double video_total;
	double audio_total;
	double misces_total;
};

static bool storageUg_get_misces_item(media_info_h media, void *data)
{
	unsigned long long size = 0;
	struct _calculated_sizes *sizes = data;

	retv_if(NULL == media, true);
	retv_if(NULL == data, false);
	char *file_path = NULL;
	media_info_get_file_path(media, &file_path);
	if (!ecore_file_exists(file_path)) {
		SETTING_TRACE_DEBUG("!ecore_file_exists(file_path)");
		FREE(file_path);
		return true;
	}
	media_info_get_size(media, &size);
	sizes->misces_total += size;
	FREE(file_path);

	return true;
}

static bool storageUg_get_media_item(media_info_h media, void *data)
{
	media_content_type_e type;
	unsigned long long size = 0;
	struct _calculated_sizes *sizes = data;

	retv_if(NULL == media, true);
	retv_if(NULL == data, false);

	media_info_get_size(media, &size);
	media_info_get_media_type(media, &type);
	switch (type) {
	case MEDIA_CONTENT_TYPE_IMAGE:
	case MEDIA_CONTENT_TYPE_VIDEO:
		sizes->video_total += size;
		break;
	case MEDIA_CONTENT_TYPE_SOUND:
	case MEDIA_CONTENT_TYPE_MUSIC:
		sizes->audio_total += size;
		break;
	default:
		SETTING_TRACE_ERROR("Invalid Type(%d)", type);
		break;
	}

	return true;
}

enum {
	STORAGEUG_TYPE_APP,
	STORAGEUG_TYPE_PIC_VIDEO,
	STORAGEUG_TYPE_AUDIO,
	STORAGEUG_TYPE_MISCES,
};

void storageug_genlist_text_update(Setting_GenGroupItem_Data *item_data,
		double size)
{
	char desc[STORAGEUG_MAX_STR_LEN] = { 0 };

	ret_if(NULL == item_data);
	ret_if(NULL == item_data->item);

	storageUg_size_to_str(size, desc, sizeof(desc));

	G_FREE(item_data->sub_desc);
	item_data->sub_desc = (char *) g_strdup(desc);

	elm_genlist_item_fields_update(item_data->item, "elm.text.sub",
			ELM_GENLIST_ITEM_FIELD_TEXT);

}

void storageUg_get_internal_detail_cb(int fn_result, SettingStorageUG *ad)
{
	ret_if(NULL == ad);

	ad->size_worker = NULL;

	if (SETTING_RETURN_SUCCESS != fn_result) {
		SETTING_TRACE_ERROR("storageUg_get_internal_detail() Fail(%d)",
				fn_result);
		return;
	}

	ad->sz_sys = ad->sz_inter_total - ad->sz_apps - ad->sz_pics_videos
			- ad->sz_audio - ad->sz_misces - ad->sz_inter_avail;

	storageug_genlist_text_update(ad->sys_mem, ad->sz_sys);
	storageug_genlist_text_update(ad->pics_videos, ad->sz_pics_videos);
	storageug_genlist_text_update(ad->audio, ad->sz_audio);
	storageug_genlist_text_update(ad->misces, ad->sz_misces);

	/* update storage pie */
	elm_genlist_item_update(ad->pie_it);
}

static int storageUG_get_media_info(const char *cond, media_info_cb cb,
		struct _calculated_sizes *sizes)
{
	int ret;
	filter_h filter = NULL;

	/*Set Filter*/
	ret = media_filter_create(&filter);
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("media_filter_create() Fail(%d)", ret);
		return ret;
	}

	ret = media_filter_set_condition(filter, cond,
			MEDIA_CONTENT_COLLATE_DEFAULT);
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		media_filter_destroy(filter);
		SETTING_TRACE_ERROR("media_filter_set_condition() Fail(%d)",
				ret);
		return ret;
	}

	ret = media_info_foreach_media_from_db(filter, cb, sizes);
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		media_filter_destroy(filter);
		SETTING_TRACE_ERROR(
				"media_info_foreach_media_from_db() Fail(%d)",
				ret);
		return ret;
	}

	ret = media_filter_destroy(filter);
	if (MEDIA_CONTENT_ERROR_NONE != ret) {
		SETTING_TRACE_ERROR("media_filter_destroy() Fail(%d)", ret);
		return ret;
	}

	return ret;
}

int storageUg_get_internal_detail(SettingStorageUG *ad)
{
	int ret;
	const char *cond;
	const char *cond_misc;
	struct _calculated_sizes sizes = { 0.0, 0.0, 0.0 };

	retv_if(NULL == ad, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	storageUG_STOP_POINT;

	/*0-image, 1-video, 2-sound, 3-music, 4-other*/
	cond = "((MEDIA_TYPE < 4) AND (MEDIA_STORAGE_TYPE=0))";
	ret = storageUG_get_media_info(cond, storageUg_get_media_item, &sizes);
	warn_if(MEDIA_CONTENT_ERROR_NONE != ret,
			"storageUG_get_media_info() Fail(%d)", ret);

	storageUG_STOP_POINT;

	cond_misc = "((MEDIA_TYPE=4) AND (MEDIA_STORAGE_TYPE=0))";
	ret = storageUG_get_media_info(cond_misc, storageUg_get_misces_item,
			&sizes);
	warn_if(MEDIA_CONTENT_ERROR_NONE != ret,
			"storageUG_get_media_info() Fail(%d)", ret);

	storageUG_STOP_POINT;

	ad->sz_pics_videos = sizes.video_total;
	ad->sz_audio = sizes.audio_total;
	ad->sz_misces = sizes.misces_total;

	return SETTING_RETURN_SUCCESS;
}

static void storageUG_get_app_cache_size_cb(pkgmgr_client *pc,
		const pkg_size_info_t *size_info, void *user_data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == user_data, "user_data is NULL");
	setting_retm_if(NULL == size_info, "size_info is NULL");


	/*char * path = app_get_cache_path(); */
	/*SETTING_TRACE_DEBUG("cache path:%s",path); */

	SettingStorageUG *ad = user_data;

	ad->sz_apps = (double) (size_info->app_size	+ size_info->ext_app_size);
	ad->sz_caches = (double) (size_info->cache_size + size_info->ext_cache_size);

	SETTING_TRACE_DEBUG("size_info->app_size : %ld", ad->sz_apps);
	SETTING_TRACE_DEBUG("ad->sz_caches:%lf", ad->sz_caches);

	storageug_genlist_text_update(ad->apps, ad->sz_apps);
	storageug_genlist_text_update(ad->caches, ad->sz_caches);

	setting_retm_if(!ad->pie_it, "!ad->pie_it");
	elm_genlist_item_update(ad->pie_it);

	pkgmgr_client_free(ad->pc_total_size);
	ad->pc_total_size = NULL;
	SETTING_TRACE_END;
}

void storageUG_update_apps_cache_info(SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;
	int ret;

	ret_if(NULL == ad);

	if (ad->pc_total_size)
		pkgmgr_client_free(ad->pc_total_size);

	ad->pc_total_size = pkgmgr_client_new(PC_REQUEST);
	if (NULL == ad->pc_total_size) {
		SETTING_TRACE_ERROR("pkgmgr_client_new() Fail");
		return;
	}

	ret = pkgmgr_client_get_total_package_size_info(ad->pc_total_size,
			storageUG_get_app_cache_size_cb, ad);

	setting_retm_if(ret, "pkgmgr_client_get_total_package_size_info() Fail(%d)", ret);

}

