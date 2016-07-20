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

#include <media_content.h>
#include <package_manager.h>
#include <Elementary.h>
#include <efl_extension.h>

#include "setting-common-general-func.h"
#include "setting-storage-SD.h"
#include "setting-storage-USB.h"
#include "setting-storage-utils.h"
#include "setting-storage-async-worker.h"
#include "setting-storage-main.h"

#include <dpm/restriction.h>

#if 0
#define SETTING_STORAGE_PIE_RECT_WIDTH (432 * WIDGET_SCALE_FACTOR)
#define SETTING_STORAGE_PIE_RECT_HEIGHT (414 * WIDGET_SCALE_FACTOR)
#define SETTING_STORAGE_PIE_CYCLE_SIZE (294 * WIDGET_SCALE_FACTOR)
#else
#define SETTING_STORAGE_PIE_RECT_WIDTH (ELM_SCALE_SIZE(432))
#define SETTING_STORAGE_PIE_RECT_HEIGHT (ELM_SCALE_SIZE(414))
#define SETTING_STORAGE_PIE_CYCLE_SIZE (ELM_SCALE_SIZE(294))
#endif



#define SETTING_STORAGE_APPS_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_applications.png"
#define SETTING_STORAGE_PICS_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_pictures.png"
#define SETTING_STORAGE_AUDIO_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_Audio.png"
#define SETTING_STORAGE_MISCES_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_miscellaneous_files.png"
#define SETTING_STORAGE_AVAIL_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_available.png"
#define SETTING_STORAGE_SYS_ICON_PATH SETTING_ICON_PATH\
	"/A01-1_icon_color_system.png"

const char *storageUg_MMC_stat = VCONFKEY_SYSMAN_MMC_STATUS;

static setting_view setting_view_storage_main;

Eina_Bool _back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	ui_app_exit();
	return EINA_TRUE;
}

static Eina_Bool setting_storage_main_click_softkey_back_cb(
		void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* Send destroy request */
	ui_app_exit();

	SETTING_TRACE_END;

	return EINA_FALSE;

}

static int dpm_usb_mass_storage(int* enable)
{
	SETTING_TRACE_BEGIN;
	dpm_context_h context;
	dpm_restriction_policy_h policy;

	int ret = 1;
	context = dpm_context_create();
	if (context == NULL) {
		SETTING_TRACE_ERROR("Failed to create client context\n");
		return 0;
	}

	policy = dpm_context_acquire_restriction_policy(context);
	if (policy == NULL) {
		SETTING_TRACE_ERROR("Failed to get storage policy interface\n");
		dpm_context_destroy(context);
		return 0;
	}

	int rval = 0;
	ret = dpm_restriction_get_external_storage_state(policy, &rval);
	*enable = rval;
	SETTING_TRACE("dpm_restriction_get_external_storage_state : %d", *enable);

	dpm_context_release_restriction_policy(context, policy);
	dpm_context_destroy(context);
	return ret;
}



static inline void storageUg_main_pie_graph_cairo(Evas_Object *pie_image,
		SettingStorageUG *ad)
{
	SETTING_TRACE_BEGIN;
	cairo_t *cr;
	double total_size;
	int cairo_w, cairo_h;
	double account_size = 0.0;
	cairo_surface_t *cairo_surface;
	double apps_sz = 0, pics_sz = 0, audio_size = 0, misces_sz = 0,
			internal_sz = 0, system_sz = 0, cache_sz = 0;

	ret_if(NULL == ad);

	cairo_h = cairo_w = SETTING_STORAGE_PIE_CYCLE_SIZE;
	total_size = ad->sz_inter_total;

	if (total_size == 0) {
		SETTING_TRACE_BEGIN;
		apps_sz = 0;
		pics_sz = 0;
		audio_size = 0;
		misces_sz = 0;
		internal_sz = 0;
		cache_sz = 0;
	} else {
		SETTING_TRACE_BEGIN;
		/* system memory */
		system_sz = ad->sz_sys / total_size;
		if (system_sz > 1 || system_sz < 0)
			system_sz = 1;

		/* apps */
		apps_sz = ad->sz_apps / total_size;
		if (apps_sz > 1 || apps_sz < 0)
			apps_sz = 1;
		/* pics */
		pics_sz = ad->sz_pics_videos / total_size;
		if (pics_sz > 1 || pics_sz < 0)
			pics_sz = 1;

		/* audio */
		audio_size = ad->sz_audio / total_size;
		if (audio_size > 1 || audio_size < 0)
			audio_size = 1;

		/* Miscellaneous Files */
		misces_sz = ad->sz_misces / total_size;
		if (misces_sz > 1 || misces_sz < 0)
			misces_sz = 1;

		/* Cache Files */
		cache_sz = (double)ad->sz_caches / total_size;
		if (cache_sz > 1 || cache_sz < 0)
			cache_sz = 1;

		/* internal */
		internal_sz = ad->sz_inter_avail / total_size;
		if (internal_sz > 1 || internal_sz < 0)
			internal_sz = 1;
	}

	if (ad->cr) {
		cairo_destroy(ad->cr);
		ad->cr = NULL;
	}

	cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
			cairo_w, cairo_h);
	if (NULL == cairo_surface) {
		SETTING_TRACE_ERROR("cairo_image_surface_create() Fail");
		return;
	}

	cr = cairo_create(cairo_surface);
	cairo_surface_destroy(cairo_surface);

	/*all, use avaliable color */
	cairo_set_line_width(cr, 4);
	SETTING_TRACE("cairo_w: [%d], cairo_h[%d]", cairo_w, cairo_h);
	cairo_arc(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2, 0, 2 * M_PI);
	cairo_set_source_rgba(cr, 230 / 255., 226 / 255., 218 / 255., 1);
	cairo_fill(cr);

	/*system memory */
	account_size = 0.75;
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	SETTING_TRACE("SYSTEM cairo_w: [%d], cairo_h[%d]", cairo_w, cairo_h);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - system_sz) * 2 * M_PI);
	cairo_set_source_rgba(cr, 153 / 255., 66 / 255., 194 / 255., 1);
	cairo_fill(cr);
	SETTING_TRACE("SYSTEM system_sz: [%f]", system_sz);
	account_size -= system_sz;

	/*app */
	SETTING_TRACE("APP cairo_w: [%d], cairo_h[%d]", cairo_w, cairo_h);
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - apps_sz) * 2 * M_PI);
	cairo_set_source_rgba(cr, 255 / 255., 189 / 255., 8 / 255., 1);
	cairo_fill(cr);
	account_size -= apps_sz;
	SETTING_TRACE("PIC cairo_w: [%d], cairo_h[%d]", cairo_w, cairo_h);
	/*pic */
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - pics_sz) * 2 * M_PI);
	cairo_set_source_rgba(cr, 119 / 255., 222 / 255., 60 / 255., 1);
	cairo_fill(cr);
	account_size -= pics_sz;

	SETTING_TRACE("AUDIO cairo_w: [%d], cairo_h[%d]", cairo_w, cairo_h);
	/*audio */
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - audio_size) * 2 * M_PI);
	cairo_set_source_rgba(cr, 28 / 255., 140 / 255., 252 / 255., 1);
	cairo_fill(cr);
	account_size -= audio_size;

	/*miscellaneous files */
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - misces_sz) * 2 * M_PI);
	cairo_set_source_rgba(cr, 52 / 255., 220 / 255., 250 / 255., 1);
	cairo_fill(cr);
	account_size -= misces_sz;

	/*cache files */
	cairo_line_to(cr, cairo_w / 2, cairo_h / 2);
	cairo_arc_negative(cr, cairo_w / 2, cairo_h / 2, cairo_w / 2,
			account_size * 2 * M_PI,
			(account_size - cache_sz) * 2 * M_PI);
	cairo_set_source_rgba(cr, 26 / 255., 49 / 255., 125 / 255., 1);
	cairo_fill(cr);
	account_size -= cache_sz;

	/*evas_object_image_alpha_set(pie_image, EINA_FALSE); */
	evas_object_image_alpha_set(pie_image, EINA_TRUE);
	evas_object_image_size_set(pie_image, cairo_w, cairo_h);
	evas_object_image_data_set(pie_image,
			cairo_image_surface_get_data(cairo_surface));
	evas_object_image_fill_set(pie_image, 0, 0, cairo_w, cairo_h);
	evas_object_resize(pie_image, cairo_w, cairo_h);
	ad->cr = cr;
	SETTING_TRACE_BEGIN;
}

static inline void storageUg_main_part_pie_graph_set(Evas_Object *parent,
		const char *part, void *data)
{
	SETTING_TRACE_BEGIN;
	Evas *evas;
	int cairo_w, cairo_h;
	Evas_Object *clip;
	Evas_Object *pie_image;
	SettingStorageUG *ad = data;

	ret_if(parent == NULL);
	ret_if(data == NULL);

	cairo_h = cairo_w = SETTING_STORAGE_PIE_CYCLE_SIZE;

	evas = evas_object_evas_get(parent);
	if (NULL == evas) {
		SETTING_TRACE_ERROR("evas_object_evas_get() Fail");
		return;
	}

	clip = evas_object_rectangle_add(evas);
	evas_object_color_set(clip, 255, 255, 255, 255);
	setting_resize_object(clip, cairo_w, cairo_h);

	pie_image = evas_object_image_add(evas);
	evas_object_color_set(pie_image, 255, 255, 255, 250);
	evas_object_clip_set(pie_image, clip);
	evas_object_show(pie_image);
	elm_object_part_content_set(parent, part, pie_image);

	storageUg_main_pie_graph_cairo(pie_image, ad);
}

Evas_Object *storageUg_main_pie_item_get_icon(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;
	double used_size;
	Evas_Object *layout;
	SettingStorageUG *ad = data;
	char size_str[STORAGEUG_MAX_STR_LEN] = {0};
	char final_str[STORAGEUG_MAX_STR_LEN] = {0};

	retv_if(NULL == obj, NULL);
	retv_if(NULL == data, NULL);

	SETTING_TRACE_ERROR("###PART: [%s]", part);
	retvm_if(safeStrCmp(part, "elm.swallow.content"), NULL,
			"part(%s) Invaild", part);
	SETTING_TRACE_BEGIN;
	layout = elm_layout_add(obj);

	elm_layout_file_set(layout, STORAGE_THEME_EDJ_NAME,
			"storage_pie");

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
			EVAS_HINT_EXPAND);

	/*title */
	elm_object_part_text_set(layout, "storage_title.text",
			_(STORAGEUG_STR_INTERNAL_STORAGE));

	storageUg_size_to_str(ad->sz_inter_avail, size_str, sizeof(size_str));
	snprintf(final_str, sizeof(final_str), "%s %s", size_str,
			 _(STORAGEUG_STR_AVAIL));
	elm_object_part_text_set(layout, "storage_total.text", final_str);

	used_size = ad->sz_inter_total - ad->sz_inter_avail;
	storageUg_size_to_str(used_size, size_str, sizeof(size_str));
	snprintf(final_str, sizeof(final_str), "%s %s", size_str,
			 _(STORAGEUG_STR_USED));
	elm_object_part_text_set(layout, "storage_used.text", final_str);
	SETTING_TRACE_BEGIN;
	/*pie */
	storageUg_main_part_pie_graph_set(layout, "pie_rect", ad);
	SETTING_TRACE_BEGIN;
	evas_object_show(layout);

	return layout;
}

Evas_Object *storageUg_color_item_content_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	int color = list_item->color;

//	SETTING_TRACE_ERROR("part : %s", part);

	if (!strcmp(part, "elm.swallow.icon")) {
		Evas_Object *layout = elm_layout_add(obj);

		Eina_Bool result = elm_layout_file_set(layout,
				STORAGE_THEME_EDJ_NAME, "storage_edge");
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(layout, EVAS_HINT_FILL,
				EVAS_HINT_FILL);
		if (!result) {
			SETTING_TRACE_ERROR(" ERROR -- elm_layout_file_set("
					"layout, SETTING_THEME_EDJ_NAME, "
					"storage_edge)");
			return NULL;
		}

		switch (color) {
		case 1:
			elm_object_signal_emit(layout, "elm,state,show,system",
					"elm");
			break;
		case 2:
			elm_object_signal_emit(layout, "elm,state,show,app",
					"elm");
			break;
		case 3:
			elm_object_signal_emit(layout, "elm,state,show,pic",
					"elm");
			break;
		case 4:
			elm_object_signal_emit(layout, "elm,state,show,audio",
					"elm");
			break;
		case 5:
			elm_object_signal_emit(layout, "elm,state,show,misc",
					"elm");
			break;
		case 6:
			elm_object_signal_emit(layout, "elm,state,show,cache",
					"elm");
			break;
		case 7:
			elm_object_signal_emit(layout, "elm,state,show,avail",
					"elm");
			break;
		default:
			break;
		}
		evas_object_show(layout);
		return layout;
	}

	return NULL;
}
static void __storage_cache_file_clean_popup_no_cache_rsp_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingStorageUG *ad = (SettingStorageUG *)data;
	switch (btn_type(obj)) {
	case POPUP_RESPONSE_OK: {
			break;
		}
	case POPUP_RESPONSE_CANCEL: {
			break;
		}
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	SETTING_TRACE_END;
}

static void __storage_cache_file_clean_popup_rsp_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingStorageUG *ad = (SettingStorageUG *)data;
	switch (btn_type(obj)) {
	case POPUP_RESPONSE_OK: {
			int ret = package_manager_clear_all_cache_dir();
			if (PACKAGE_MANAGER_ERROR_NONE == ret) {
				ad->sz_caches = 0;
				storageug_genlist_text_update(ad->caches,
						ad->sz_caches);
			} else {
				SETTING_TRACE_ERROR("Failed to call package_"
						"manager_clear_all_cache_dir() "
						"with error code-%d", ret);
			}

			break;
		}
	case POPUP_RESPONSE_CANCEL: {
			break;
		}
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	SETTING_TRACE_END;
}

static void storageUg_main_sel(void *data, Evas_Object *obj, void *event_info)
{
	SettingStorageUG *ad = data;
	Elm_Object_Item *item = event_info;

	ret_if(data == NULL);

	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SETTING_TRACE("clicking item[%s]", list_item->keyStr);

	if (!safeStrCmp(STORAGEUG_STR_APPS, list_item->keyStr)) {
		storageUg_manage_apps_ug(ad);
	} else if (!safeStrCmp(STORAGEUG_STR_DEF_STORAGE, list_item->keyStr)) {
		setting_view_change(ad->main_view, ad->default_view, ad);
	} else if (!safeStrCmp(STORAGEUG_STR_PICS, list_item->keyStr)) {
		app_launcher("org.tizen.gallery", NULL, NULL);
	} else if (!safeStrCmp(STORAGEUG_STR_AUDIO, list_item->keyStr)) {
		app_launcher("org.tizen.music-player", NULL, NULL);
	} else if (!safeStrCmp(STORAGEUG_STR_MISCES, list_item->keyStr)) {
		setting_view_change(ad->main_view, ad->misces_view, ad);
	} else if (!safeStrCmp(STORAGEUG_STR_CACHE, list_item->keyStr)) {
		if (ad->sz_caches <= 0) {
			ad->popup = setting_create_popup(ad, ad->md.win_main,
					"IDS_ST_HEADER_CLEAR_CACHE_ABB",
					"IDS_ST_POP_THERE_IS_NO_CACHE_DATA_TO_CLEAR",
					__storage_cache_file_clean_popup_no_cache_rsp_cb,
					0, FALSE, FALSE,
					1, "IDS_ST_BUTTON_OK_ABB");
		} else {
			ad->popup = setting_create_popup(ad, ad->md.win_main,
					"IDS_ST_HEADER_CLEAR_CACHE_ABB",
					"IDS_ST_POP_ALL_THE_CACHE_DATA_WILL_BE_CLEARED",
					__storage_cache_file_clean_popup_rsp_cb,
					0, FALSE, FALSE,
					2, "IDS_ST_BUTTON_CLEAR_CACHE_ABB",
					"IDS_ST_BUTTON_CANCEL_ABB");
		}
	}
}

static Setting_GenGroupItem_Data *storageUg_main_append_group_mid_item(
	SettingStorageUG *ad, int color, const char *key_str,
	const char *sub_str)
{
	Setting_GenGroupItem_Data *item_data = NULL;
	item_data = (Setting_GenGroupItem_Data *) calloc(
			1, sizeof(Setting_GenGroupItem_Data));
	if (item_data) {
		item_data->userdata = ad;
		item_data->keyStr = (char *)g_strdup(key_str);
		item_data->sub_desc = (char *)g_strdup(sub_str);
		item_data->color = color;
		item_data->item = elm_genlist_item_append(ad->md.genlist,
				&(ad->itc_color_item), item_data, NULL,
				ELM_GENLIST_ITEM_NONE, storageUg_main_sel, ad);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	return item_data;
}

static inline void storageUg_main_dev_memory_detail(SettingStorageUG *ad)
{
	char avail_desc[STORAGEUG_MAX_STR_LEN] = {0};
	char total_desc[STORAGEUG_MAX_STR_LEN] = {0};

	/* memory total */
	storageUg_size_to_str(ad->sz_inter_total, total_desc,
			sizeof(total_desc));
	ad->total_space = setting_create_Gendial_field_def(ad->md.genlist,
			&itc_2text_2, NULL, ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0, STORAGEUG_STR_TOTAL, total_desc, NULL);
	ret_if(NULL == ad->total_space);
	ad->total_space->userdata = ad;
	elm_genlist_item_select_mode_set(ad->total_space->item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* system memory */
	ad->sys_mem = storageUg_main_append_group_mid_item(ad,
			1, STORAGEUG_STR_SYS_MEM, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->sys_mem);
	elm_genlist_item_select_mode_set(ad->sys_mem->item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* Applications */
	ad->apps = storageUg_main_append_group_mid_item(ad,
			2, STORAGEUG_STR_APPS, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->apps);

	/* Pictures, Videos */
	ad->pics_videos = storageUg_main_append_group_mid_item(ad,
			3, STORAGEUG_STR_PICS, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->pics_videos);

	/* Audio */
	ad->audio = storageUg_main_append_group_mid_item(ad,
			4, STORAGEUG_STR_AUDIO, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->audio);

	/* Miscellaneous files */
	ad->misces = storageUg_main_append_group_mid_item(ad,
			5, STORAGEUG_STR_MISCES, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->misces);

	/* cache files */
	ad->caches = storageUg_main_append_group_mid_item(ad,
			6, STORAGEUG_STR_CACHE, STORAGEUG_STR_WAIT);
	ret_if(NULL == ad->caches);


	/* Available */
	storageUg_size_to_str(ad->sz_inter_avail, avail_desc,
			sizeof(avail_desc));
	ad->avail = storageUg_main_append_group_mid_item(ad,
			7, STORAGEUG_STR_AVAIL, avail_desc);
	ret_if(NULL == ad->avail);
	elm_genlist_item_select_mode_set(ad->avail->item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
}

static void storageUg_main_dev_memory_update(SettingStorageUG *ad)
{
	char desc[STORAGEUG_MAX_STR_LEN] = {0};

	ret_if(NULL == ad);

	storageUg_get_internal_storage_status(&ad->sz_inter_total,
			&ad->sz_inter_avail);

	/* update internal total size */
	storageUg_size_to_str(ad->sz_inter_total, desc, sizeof(desc));
	G_FREE(ad->total_space->sub_desc);
	ad->total_space->sub_desc = (char *)g_strdup(desc);
	elm_genlist_item_fields_update(ad->total_space->item, "elm.text.sub",
			ELM_GENLIST_ITEM_FIELD_TEXT);

	/* update internal available size */
	storageUg_size_to_str(ad->sz_inter_avail, desc, sizeof(desc));
	G_FREE(ad->avail->sub_desc);
	ad->avail->sub_desc = (char *)g_strdup(desc);
	elm_genlist_item_fields_update(ad->avail->item, "elm.text.sub",
			ELM_GENLIST_ITEM_FIELD_TEXT);

}

static Eina_Bool storageUg_update_timer(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *ad = data;

	retv_if(NULL == data, ECORE_CALLBACK_CANCEL);

	storageUg_main_dev_memory_update(ad);
	SETTING_TRACE_DEBUG("ad->misces_view->is_create:%d",
			ad->misces_view->is_create);
	/* if (ad->misces_view->is_create) {
		setting_view_update(ad->misces_view, ad);
	} */
	if (ad->size_worker)
		storageUg_stop_async_worker(ad->size_worker);
	ad->size_worker = storageUg_start_async_worker(
			storageUg_get_internal_detail,
			storageUg_get_internal_detail_cb, ad);


	ad->update_timer = NULL;
	SETTING_TRACE_END;
	return ECORE_CALLBACK_CANCEL;
}

static void storageUg_main_update_media_data(SettingStorageUG *ad)
{
	ret_if(NULL == ad);

	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}

	ad->update_timer = ecore_timer_add(0.5, storageUg_update_timer, ad);
}

static void storageUg_media_filesys_changed_cb(
	media_content_error_e error,
	int pid,
	media_content_db_update_item_type_e update_item,
	media_content_db_update_type_e update_type,
	media_content_type_e media_type,
	char *uuid,
	char *path,
	char *mime_type,
	void *user_data)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *ad = user_data;

	ret_if(user_data == NULL);

	storageUg_main_update_media_data(ad);

	SETTING_TRACE_END;
}

static Eina_Bool storageUg_main_back_cb(void *data, Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	SettingStorageUG *ad = data;

	retv_if(data == NULL, EINA_TRUE);

	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}

	if (ad->size_worker) {
		if (storageUg_worker_is_running(ad->size_worker)) {
			SETTING_TRACE("DO NOT Close Storage UI!!!!! "
					"<<------------");
			return EINA_FALSE;
		}
	} else {
		SETTING_TRACE("ad->size_worker is NULL <<--------");
	}

//	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;
	return EINA_FALSE;
}

static inline Evas_Object *storageUg_main_genlist(Evas_Object *parent)
{
	Evas_Object *genlist;

	genlist = elm_genlist_add(parent);
	if (NULL == genlist) {
		SETTING_TRACE_ERROR("elm_genlist_add() Fail");
		return NULL;
	}
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(genlist, EINA_TRUE);
	elm_genlist_block_count_set(genlist, 2);
	evas_object_show(genlist);

	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb,
			NULL);

	return genlist;
}

static int storageUg_main_create(void *data)
{
	SETTING_TRACE_BEGIN;

	int ret;
	Elm_Object_Item *pie_item;
	SettingStorageUG *ad = data;

//	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);


	ad->update_timer = NULL;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SETTING_TRACE_BEGIN;

	Evas_Object *layout = NULL;
	Evas_Object *conform = NULL;
	Evas_Object *bg = NULL;

	// Conformant
	conform = elm_conformant_add(ad->md.win_main);
	if (conform == NULL) {
		SETTING_TRACE(" *** elm_conformant_add returns NULL *** ");
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;
	}
	evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->md.win_main, conform);
	evas_object_show(conform);
//
//	SETTING_TRACE_BEGIN;
//	/*  Base Layout */
//	layout = elm_layout_add(conform);
//	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");
//
//	elm_layout_theme_set(layout, "layout", "application", "default");
//	evas_object_size_hint_align_set(conform, EVAS_HINT_FILL,
//			EVAS_HINT_FILL);
//	elm_win_resize_object_add(ad->md.win_main, conform);
//	elm_object_content_set(conform, layout);
//
//	SETTING_TRACE_BEGIN;
//	bg = setting_create_bg(layout, ad->md.win_main, "group_list");
//	elm_object_part_content_set(layout, "elm.swallow.bg", bg);
//	SETTING_TRACE_BEGIN;
//	evas_object_show(layout);
//	ad->md.ly_main = layout;
//
//	SETTING_TRACE_BEGIN;
////	elm_win_conformant_set(ad->md.win_main, EINA_TRUE);
//	evas_object_show(conform);
//
//	SETTING_TRACE_BEGIN;
//	ad->md.conform = conform;
//
//	evas_object_data_set(ad->md.win_main, "conformant", conform);
//	SETTING_TRACE_BEGIN;
	/* navi frame */
	Evas_Object *navi = NULL;
	SETTING_TRACE_BEGIN;
	/* navi frame */
	navi = elm_naviframe_add(conform);
	SETTING_TRACE_BEGIN;
	if (navi == NULL) {
		SETTING_TRACE(" *** elm_naviframe_add returns NULL *** ");
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;
	}
	SETTING_TRACE_BEGIN;
	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_TRUE);
	elm_object_part_content_set(conform, "elm.swallow.content", navi);
	ad->md.navibar_main = navi;
	evas_object_show(navi);
	SETTING_TRACE_BEGIN;
	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	/* genlist */
	Evas_Object *genlist = NULL;
	genlist = elm_genlist_add(navi);
	retvm_if(genlist == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "Cannot set scroller object as contento of layout");
	elm_genlist_mode_set(genlist, ELM_LIST_COMPRESS);
	elm_genlist_clear(genlist);	/* first to clear list */

	ad->md.genlist = genlist;

	evas_object_size_hint_weight_set(ad->md.genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(ad->md.navibar_main, ad->md.genlist);
	evas_object_show(genlist);

	Evas_Object *button = elm_button_add(ad->md.navibar_main);
	elm_object_style_set(button, NAVI_BACK_ARROW_BUTTON_STYLE);
	evas_object_smart_callback_add(button, "clicked",
			(setting_call_back_func)setting_storage_main_click_softkey_back_cb,
			ad);
	evas_object_show(button);

	Elm_Object_Item *nf_it = NULL;
	nf_it = elm_naviframe_item_push(navi, _("IDS_ST_BODY_STORAGE"), button, NULL, genlist, NULL);
	elm_naviframe_item_pop_cb_set(nf_it, _back_cb, ad);

//	ret = view_init(&ad->md, _("IDS_ST_BODY_STORAGE"));
//	if (ret != SETTING_RETURN_SUCCESS)
//		return ret;
//	SETTING_TRACE_BEGIN;

//	ad->gl_main = storageUg_main_genlist(ad->navi);
//	setting_create_navi_bar_buttons(STORAGEUG_STR_STORAGE,
//			NULL, /* arrow style */
//			(setting_call_back_func)storageUg_main_back_cb,
//			ad,
//			ad->md.genlist, ad->md.navibar_main, NULL);
//

	SETTING_TRACE_BEGIN;
	storageUg_get_internal_storage_status(&ad->sz_inter_total,
			&ad->sz_inter_avail);

	SETTING_TRACE_BEGIN;
	/* storage pie view */
	pie_item = elm_genlist_item_append(ad->md.genlist, &(ad->itc_pie),
			ad, NULL, ELM_GENLIST_ITEM_NONE, NULL, NULL);

	elm_genlist_item_select_mode_set(pie_item,
			ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	ad->pie_it = pie_item;

	SETTING_TRACE_BEGIN;

	storageUg_main_dev_memory_detail(ad);

	if (ad->size_worker)
		storageUg_stop_async_worker(ad->size_worker);
	ad->size_worker = storageUg_start_async_worker(
			storageUg_get_internal_detail,
			storageUg_get_internal_detail_cb,
			ad);

	storageUG_update_apps_cache_info(ad);

	if (!isEmulBin()) {
		ret = vconf_get_int(storageUg_MMC_stat, &ad->mmc_status);
		retvm_if(ret, SETTING_RETURN_FAIL, "vconf_get_int(%s) Fail",
				storageUg_MMC_stat);

		/* Default storage */
		setting_create_Gendial_field_def(
				ad->md.genlist,
				&itc_1text,
				storageUg_main_sel,
				ad,
				SWALLOW_Type_INVALID,
				NULL,
				NULL,
				0,
				STORAGEUG_STR_DEF_STORAGE,
				NULL, NULL);

		/* SD */
		ad->sd_card = setting_create_Gendial_field_titleItem(
				ad->md.genlist,
				&itc_group_item,
				STORAGEUG_STR_SD_CARD,
				NULL);
		if (ad->sd_card)
			storageUg_main_append_SD_info(ad);

		ret = vconf_notify_key_changed(storageUg_MMC_stat,
				storageUg_SD_change_cb, ad);
		warn_if(ret != 0, "vconf_notify_key_changed(%s) Fail",
				storageUg_MMC_stat);
	}

	/* usb otg storage*/
	ad->usb_otg_status = SETTING_STORAGE_USB_OTG_REMOVE;

	storageUg_init_USB(ad);

	setting_view_storage_main.is_create = 1;
	media_content_set_db_updated_cb(storageUg_media_filesys_changed_cb, ad);
	SETTING_TRACE("-----------------------------------------------");
	SETTING_TRACE(" WIDGET_SCALE_FACTOR : %f", WIDGET_SCALE_FACTOR);
	SETTING_TRACE("-----------------------------------------------");

	int enable = 0;
	dpm_usb_mass_storage(&enable);
	SETTING_TRACE("------------------------------------------------------------------------------");
	SETTING_TRACE(" DPM_USB_MASS_STORAGE : %d", enable);
	SETTING_TRACE("------------------------------------------------------------------------------");

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_main_destroy(void *data)
{
	int ret;
	SettingStorageUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_view_destroy(ad->misces_view, ad);

	media_content_unset_db_updated_cb();

	storageUg_deinit_USB();

	ret = vconf_ignore_key_changed(storageUg_MMC_stat,
			storageUg_SD_change_cb);
	warn_if(ret, "vconf_ignore_key_changed(%s) Fail(%d)",
			storageUg_MMC_stat, ret);

	storageUg_stop_async_worker_all();

	if (ad->update_idler) {
		ecore_idler_del(ad->update_idler);
		ad->update_idler = NULL;
	}

	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	if (ad->cr) {
		cairo_destroy(ad->cr);
		ad->cr = NULL;
	}
	if (ad->pc) {
		pkgmgr_client_free(ad->pc);
		ad->pc = NULL;
	}
	if (ad->pc_total_size) {
		pkgmgr_client_free(ad->pc_total_size);
		ad->pc_total_size = NULL;
	}

	setting_view_storage_main.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_main_update(void *data)
{
	SettingStorageUG *ad = data;

	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*storageUg_main_update_media_data(ad); */
	storageUG_update_apps_cache_info(ad);

	if (!ad->misces_list)
		evas_object_show(ad->md.ly_main);
	/* else
		setting_view_update(ad->misces_view, ad); */

	return SETTING_RETURN_SUCCESS;
}

static int storageUg_main_cleanup(void *data)
{
	return SETTING_RETURN_SUCCESS;
}

void storageUg_main_init(SettingStorageUG *ad)
{
	ret_if(NULL == ad);
	ret_if(ad->main_view);

	setting_view_storage_main.create = storageUg_main_create;
	setting_view_storage_main.destroy = storageUg_main_destroy;
	setting_view_storage_main.update = storageUg_main_update;
	setting_view_storage_main.cleanup = storageUg_main_cleanup;

	ad->main_view = &setting_view_storage_main;
}
