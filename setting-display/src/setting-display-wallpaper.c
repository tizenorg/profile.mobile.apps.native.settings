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
#include <setting-display-wallpaper.h>

static int setting_display_wallpaper_create(void *cb);
static int setting_display_wallpaper_destroy(void *cb);
static int setting_display_wallpaper_update(void *cb);
static int setting_display_wallpaper_cleanup(void *cb);

setting_view setting_view_display_wallpaper = {
	.create = setting_display_wallpaper_create,
	.destroy = setting_display_wallpaper_destroy,
	.update = setting_display_wallpaper_update,
	.cleanup = setting_display_wallpaper_cleanup,
};

#define SETTING_WALLPAPER_HOME_SCREEN "Wallpaper"      // ug-image-viewer parameter
#define SETTING_WALLPAPER_LOCK_SCREEN "Lockscreen"	// ug-image-viewer parameter
#define SETTING_WALLPAPER_HOME_AND_LOCK "Wallpaper & Lockscreen"	// ug-image-viewer parameter
#define SETTING_WALLPAPER_POPUP_TIMER 3
/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
#if DISABLED_CODE
static void __setting_display_wallpaper_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *)data;

	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_BGSET)) {
		if (ad->cur_sel == WALLPAPER_CUR_SEL_LOCK_SCREEN) {
			ad->cur_sel = WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN;
		} else if (ad->cur_sel == WALLPAPER_CUR_SEL_NONE) {
			ad->cur_sel = WALLPAPER_CUR_SEL_HOME_SCREEN;
		} else {
			/* do nothing */
		}
	} else if (!safeStrCmp(vconf_name, VCONFKEY_IDLE_LOCK_BGSET)) {
		if (ad->cur_sel == WALLPAPER_CUR_SEL_HOME_SCREEN) {
			ad->cur_sel = WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN;
		} else if (ad->cur_sel == WALLPAPER_CUR_SEL_NONE) {
			ad->cur_sel = WALLPAPER_CUR_SEL_LOCK_SCREEN;
		} else {
			/* do nothing */
		}
	}
}

static int __setting_display_wallpaper_listen_vconf_change(void *data)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	ret = vconf_notify_key_changed(VCONFKEY_BGSET,
			__setting_display_wallpaper_vconf_change_cb, data);
	setting_retvm_if(ret < 0, ret, "%s notifications Failed(%d)", (char *)VCONFKEY_BGSET, ret);

	ret = vconf_notify_key_changed(VCONFKEY_IDLE_LOCK_BGSET,
			__setting_display_wallpaper_vconf_change_cb, data);
	setting_retvm_if(ret < 0, ret, "%s notifications Failed(%d)", (char *)VCONFKEY_IDLE_LOCK_BGSET, ret);
	return ret;
}

static int __setting_display_wallpaper_unlisten_vconf_change()
{
	SETTING_TRACE_BEGIN;
	(void)vconf_ignore_key_changed(VCONFKEY_BGSET,
				       __setting_display_wallpaper_vconf_change_cb);
	(void)vconf_ignore_key_changed(VCONFKEY_IDLE_LOCK_BGSET,
				       __setting_display_wallpaper_vconf_change_cb);
	return 0;
}
#endif

static void __setting_display_wallpaper_imageviewer_ug_result_cb(ui_gadget_h ug, service_h result, void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) priv;

	if (result) {
		char *path = NULL;
		switch(ad->cur_sel) {
		case WALLPAPER_CUR_SEL_HOME_SCREEN:
		{
			service_get_extra_data(result, "homescreen_path", &path);
			if (path == NULL) {
				ad->wallpaper_changed = EINA_FALSE;
				return;
			}
			break;
		}
		case WALLPAPER_CUR_SEL_LOCK_SCREEN:
		{
			service_get_extra_data(result, "lockscreen_path", &path);
			if (path == NULL) {
				ad->wallpaper_changed = EINA_FALSE;
				return;
			}
			break;
		}
		case WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN:
		{
			service_get_extra_data(result, "homescreen_path", &path);
			if (path == NULL) {
				ad->wallpaper_changed = EINA_FALSE;
				return;
			}
			service_get_extra_data(result, "lockscreen_path", &path);
			if (path == NULL) {
				ad->wallpaper_changed = EINA_FALSE;
				return;
			}
			break;
		}
		default:
			ad->wallpaper_changed = EINA_FALSE;
			return;
		}

		ad->wallpaper_changed = EINA_TRUE;
	}
}

static void __setting_display_wallpaper_imageviewer_ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(priv == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;	/* ad is point to priv */

	if (ug) {
		ug_destroy(ug);
		ad->ug_imageviewer = NULL;
	}

	if (ad->wallpaper_changed) {
		if (ad->ug_gallery) {
			ug_destroy(ad->ug_gallery);
			ad->ug_gallery = NULL;
		}

		char text[MAX_DISPLAY_NAME_LEN_ON_UI +1] = {0, };
		SETTING_TRACE("cur_sel = %d", ad->cur_sel);

		switch(ad->cur_sel) {
		case WALLPAPER_CUR_SEL_HOME_SCREEN:
			snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, "%s", SETTING_WALLPAPER_HOME_SCREEN_CHANGED);
			break;
		case WALLPAPER_CUR_SEL_LOCK_SCREEN:
			snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, "%s", SETTING_WALLPAPER_LOCK_SCREEN_CHANGED);
			break;
		case WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN:
			snprintf(text, MAX_DISPLAY_NAME_LEN_ON_UI, "%s", SETTING_WALLPAPER_HOME_AND_LOCK_SCREEN_CHANGED);
			break;
		default:
			ad->cur_sel = WALLPAPER_CUR_SEL_NONE;
			return;
		}

		setting_create_sel_info_popup(ad->view_layout,
					      &(ad->selInfoPop_layout),
					      &(ad->selInfoPop));
		elm_notify_timeout_set(ad->selInfoPop, SETTING_WALLPAPER_POPUP_TIMER);

		edje_object_part_text_set(_EDJ(ad->selInfoPop_layout), "elm.text", text);
		ad->cur_sel = WALLPAPER_CUR_SEL_NONE;
		ad->wallpaper_changed = EINA_FALSE;
	} else {
		SETTING_TRACE("wallpaper not changed");
	}
}

static void __setting_display_wallpaper_gallery_ug_result_cb(ui_gadget_h ug, service_h result, void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) priv;

	if (result) {
		char *path = NULL;
		service_get_extra_data(result, "path", &path);
		setting_retm_if(path == NULL, "path is NULL");

		service_h service;
		if (service_create(&service)) {
			return;
		}

		service_add_extra_data(service, "Path", path);
		service_add_extra_data(service, "View Mode", "SETAS");

		SETTING_TRACE("Path = %s", path);

		switch(ad->cur_sel) {
		case WALLPAPER_CUR_SEL_HOME_SCREEN:
			service_add_extra_data(service, "Setas type", SETTING_WALLPAPER_HOME_SCREEN);
			break;
		case WALLPAPER_CUR_SEL_LOCK_SCREEN:
			service_add_extra_data(service, "Setas type", SETTING_WALLPAPER_LOCK_SCREEN);
			break;
		case WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN:
			service_add_extra_data(service, "Setas type", SETTING_WALLPAPER_HOME_AND_LOCK);
			break;
		default:
			service_destroy(service);
			SETTING_TRACE_ERROR("ad->cur_sel is error");
			return;
		}

		struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
		if (!cbs) {
			service_destroy(service);
			return;
		}

		cbs->layout_cb = setting_display_layout_ug_cb;
		cbs->result_cb = __setting_display_wallpaper_imageviewer_ug_result_cb;
		cbs->destroy_cb = __setting_display_wallpaper_imageviewer_ug_destroy_cb;
		cbs->priv = (void *)ad;

		ad->ug_imageviewer = ug_create(ad->ug, "image-viewer-efl", UG_MODE_FULLVIEW, service, cbs);

		if (NULL == ad->ug_imageviewer) {	/* error handling */
			SETTING_TRACE_ERROR("NULL == ad->ug_imageviewer");
	        	setting_create_simple_popup(ad, ad->win_get, NULL, _(UNSUPPORTED_FUNCTION));
		}

		service_destroy(service);
		FREE(cbs);
	}
}

static void __setting_display_wallpaper_gallery_ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(priv == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;	/* ad is point to priv */

	if (ad->ug_imageviewer == NULL) {
		ug_destroy(ug);
		ad->ug_gallery = NULL;
	} else {
		return;
	}

	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	retm_if(navi_it == NULL, "navi_it is NULL");
	Evas_Object *back_btn = elm_object_item_part_content_get(navi_it, "prev_btn");

	if (back_btn != NULL) {
		elm_object_style_set(back_btn, NAVI_BACK_BUTTON_STYLE);	/* take into effect */
	}
}

static void __setting_display_wallpaper_ug_launch(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	service_h svc;
	if (service_create(&svc)) {
		return;
	}

	service_add_extra_data(svc, "launch-type", "select-one");
	service_add_extra_data(svc, "file-type", "image");

	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	if (!cbs) {
		service_destroy(svc);
		return;
	}

	cbs->layout_cb = setting_display_layout_ug_cb;
	cbs->result_cb = __setting_display_wallpaper_gallery_ug_result_cb;
	cbs->destroy_cb = __setting_display_wallpaper_gallery_ug_destroy_cb;
	cbs->priv = (void *)ad;

	ad->ug_gallery = ug_create(ad->ug, "gallery-efl", UG_MODE_FULLVIEW, svc, cbs);
	SETTING_TRACE("ad->ug_gallery = %p", ad->ug_gallery);

	if (NULL == ad->ug_gallery) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_gallery");
        	setting_create_simple_popup(ad, ad->win_get, NULL, _(UNSUPPORTED_FUNCTION));
	}

	service_destroy(svc);
	FREE(cbs);
}

static void __setting_display_wallpaper_mouse_up_Gendial_list_cb(void *data, Evas_Object * obj, void *event_info)
{
	//error check
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	if (!safeStrCmp("IDS_ST_MBODY_HOME_SCREEN_WALLPAPER", list_item->keyStr)) {
		ad->cur_sel = WALLPAPER_CUR_SEL_HOME_SCREEN;
		__setting_display_wallpaper_ug_launch(ad);
	} else if (!safeStrCmp("IDS_ST_MBODY_LOCK_SCREEN_WALLPAPER_M_NOUN", list_item->keyStr)) {
		ad->cur_sel = WALLPAPER_CUR_SEL_LOCK_SCREEN;
		__setting_display_wallpaper_ug_launch(ad);
	} else if (!safeStrCmp(SETTING_WALLPAPER_HOME_AND_LOCK_SCREEN, list_item->keyStr)) {
		ad->cur_sel = WALLPAPER_CUR_SEL_HOME_AND_LOCK_SCREEN;
		__setting_display_wallpaper_ug_launch(ad);
	} else {
		/* do nothing */
	}

}

static int setting_display_wallpaper_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;	/* ad is point to cb */
	ad->cur_sel = WALLPAPER_CUR_SEL_NONE;

	Evas_Object *controllbar = NULL;
	Evas_Object *scroller = NULL;
	scroller = elm_genlist_add(ad->win_main_layout);
	elm_object_style_set(scroller, "dialogue");
	retvm_if(scroller == NULL, SETTING_RETURN_FAIL, "Cannot set scroller object as contento of layout");
	elm_genlist_clear(scroller);	/* first to clear list */

	ad->ly_wall = setting_create_layout_navi_bar(ad->win_main_layout, ad->win_get,
					   _(KeyStr_Wallpaper),
					   _("IDS_COM_BODY_BACK"), NULL, NULL,
					   setting_display_wallpaper_click_softkey_cancel_cb,
					   NULL, NULL, ad, scroller,
					   &ad->navi_bar, &controllbar);

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->data_home_screen =
		setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					     __setting_display_wallpaper_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, "IDS_ST_MBODY_HOME_SCREEN_WALLPAPER",
					     NULL, NULL);
	if (ad->data_home_screen) {
		ad->data_home_screen->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->screen_reader is NULL");
		return SETTING_RETURN_FAIL;
	}

	ad->data_lock_screen =
		setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					     __setting_display_wallpaper_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, "IDS_ST_MBODY_LOCK_SCREEN_WALLPAPER_M_NOUN",
					     NULL, NULL);
	if (ad->data_lock_screen) {
		ad->data_lock_screen->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->screen_reader is NULL");
		return SETTING_RETURN_FAIL;
	}

	ad->data_home_lock_screen =
		setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					     __setting_display_wallpaper_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, SETTING_WALLPAPER_HOME_AND_LOCK_SCREEN,
					     NULL, NULL);
	if (ad->data_home_lock_screen) {
		ad->data_home_lock_screen->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->screen_reader is NULL");
		return SETTING_RETURN_FAIL;
	}

	ad->wallpaper_changed = EINA_FALSE;
	setting_view_display_wallpaper.is_create = 1;
#if DISABLED_CODE
	__setting_display_wallpaper_listen_vconf_change(ad);
#endif
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_wallpaper_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDisplayUG *ad = (SettingDisplayUG *) cb;	/* ad is point to cb */
#if DISABLED_CODE
	__setting_display_wallpaper_unlisten_vconf_change();
#endif

	if (ad->selInfoPop) {
		evas_object_del(ad->selInfoPop);
		ad->selInfoPop = NULL;
	}

	if (ad->ly_wall != NULL) {
		evas_object_del(ad->ly_wall);
		ad->ly_wall = NULL;
	}
	setting_view_display_wallpaper.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_display_wallpaper_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_display_wallpaper_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
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
setting_display_wallpaper_click_softkey_cancel_cb(void *data, Evas_Object *obj,
						  void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingDisplayUG *ad = (SettingDisplayUG *) data;	/* ad is point to cb */

	/*  called when this shared gadget is terminated. similar with app_exit */
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}
