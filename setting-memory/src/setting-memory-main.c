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
#include <setting-memory-main.h>
#define VCONFKEY_MEMORY_STATUS "memory/msg/ready"

static int setting_memory_main_create(void *cb);
static int setting_memory_main_destroy(void *cb);
static int setting_memory_main_update(void *cb);
static int setting_memory_main_cleanup(void *cb);

setting_view setting_view_memory_main = {
	.create = setting_memory_main_create,
	.destroy = setting_memory_main_destroy,
	.update = setting_memory_main_update,
	.cleanup = setting_memory_main_cleanup,
};

static void setting_memory_main_mouse_up_Gendial_list_cb(void *data,
							 Evas_Object *obj,
							 void *event_info);
static void setting_memory_main_click_softkey_back(void *data,
						   Evas_Object *obj,
						   void *event_info);
static void setting_memory_main_memory_status_change_cb(keynode_t *key,
							void *data);

/*******************************************
 *
 *	basic function
 *
 ******************************************/

static int setting_memory_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = (SettingMemoryUG *) cb;

	Evas_Object *scroller;
	/* int mmc_mode; */
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _("IDS_ST_BODY_MEMORY"),
						   S_("IDS_COM_BODY_BACK"),
						   NULL,
						   setting_memory_main_click_softkey_back,
						   NULL, ad, &scroller,
						   &(ad->navi_bar));
	retvm_if(ad->ly_main == NULL, -1, "Can't create main layout");

	Elm_Object_Item *item = NULL;;
	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	// [UI] Default Memory
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					 setting_memory_main_mouse_up_Gendial_list_cb,
					 ad, SWALLOW_Type_INVALID, NULL, NULL,
					 0,
					 "IDS_ST_BODY_MEMORYSETTINGS_DEFAULT_MEMORY",
					 NULL, NULL);

	// [UI] Memory Status
	setting_create_Gendial_field_def(scroller, &(ad->itc_1text),
					 setting_memory_main_mouse_up_Gendial_list_cb,
					 ad, SWALLOW_Type_INVALID, NULL, NULL,
					 0, "IDS_ST_BODY_MEMORY_STATUS", NULL,
					 NULL);
	setting_view_memory_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_memory_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	setting_view_memory_main.is_create = 0;

	SettingMemoryUG *ad = (SettingMemoryUG *) cb;
	vconf_ignore_key_changed(VCONFKEY_MEMORY_STATUS,
				 setting_memory_main_memory_status_change_cb);
	evas_object_del(ad->ly_main);

	return 0;
}

static int setting_memory_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = (SettingMemoryUG *) cb;
	evas_object_show(ad->ly_main);

	return 0;
}

static int setting_memory_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* SettingMemoryUG *ad = (SettingMemoryUG *)cb; */
	/* evas_object_hide(ad->ly_main); */

	return 0;
}

/*********************************************
 *
 *	callback function
 *
 *********************************************/
static void
setting_memory_main_memory_status_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingMemoryUG *ad = data;
	int status = vconf_keynode_get_bool(key);
	/* char *vconf_name = vconf_keynode_get_name(key); */

	if (1 == status) {
		if (ad->msg_intial_popup != NULL) {
			evas_object_del(ad->msg_intial_popup);
			ad->msg_intial_popup = NULL;
		}
		vconf_ignore_key_changed(VCONFKEY_MEMORY_STATUS,
					 setting_memory_main_memory_status_change_cb);
		setting_view_change(&setting_view_memory_main,
				    &setting_view_memory_status, ad);
	}
}

static void
setting_memory_main_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj,
					     void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp
		("IDS_ST_BODY_MEMORYSETTINGS_DEFAULT_MEMORY",
		 list_item->keyStr)) {
		setting_view_change(&setting_view_memory_main,
				    &setting_view_memory_default_memory, ad);
	} else if (!safeStrCmp("IDS_ST_BODY_MEMORY_STATUS", list_item->keyStr)) {
		/* check if msg server is ready */
		int val = 0;	/* intial value is 0(not ready) */
		vconf_get_bool(VCONFKEY_MEMORY_STATUS, &val);

		if (1 != val) {	/* not ready */
			int ret;
			ret =
			    vconf_notify_key_changed(VCONFKEY_MEMORY_STATUS,
						     setting_memory_main_memory_status_change_cb,
						     ad);
			setting_retm_if(ret < 0, "%s notifications Failed(%d)",
					VCONFKEY_MEMORY_STATUS, ret);

			ad->msg_intial_popup =
			    setting_create_popup_without_btn(ad, ad->win_get,
						 NULL, _(SETTING_MEMORY_MSG_SERVER_POPUP_INFO_MSG),
						 NULL, 0, TRUE, TRUE);
		} else {	/* ready */

			setting_view_change(&setting_view_memory_main,
					    &setting_view_memory_status, ad);
		}
	} else
		SETTING_TRACE("Invalid selection");

}

static void
setting_memory_main_click_softkey_back(void *data, Evas_Object *obj,
				       void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "data == NULL");

	SettingMemoryUG *ad = (SettingMemoryUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);

}
