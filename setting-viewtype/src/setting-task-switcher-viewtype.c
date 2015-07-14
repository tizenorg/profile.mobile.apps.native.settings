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
#include <setting-task-switcher-viewtype.h>

static int setting_task_switcher_viewtype_create(void *cb);
static int setting_task_switcher_viewtype_destroy(void *cb);
static int setting_task_switcher_viewtype_update(void *cb);
static int setting_task_switcher_viewtype_cleanup(void *cb);

setting_view setting_view_task_switcher_viewtype = {
	.create = setting_task_switcher_viewtype_create,
	.destroy = setting_task_switcher_viewtype_destroy,
	.update = setting_task_switcher_viewtype_update,
	.cleanup = setting_task_switcher_viewtype_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_task_switcher_viewtype_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingViewTypeUG *ad = (SettingViewTypeUG *) cb;

	Evas_Object *navi_bar = NULL;
	Evas_Object *scroller;

	ad->ly_task =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _
						   ("IDS_ST_BODY_TASK_SWITCHER"),
						   dgettext("sys_string", "IDS_COM_SK_CANCEL"),
						   dgettext("sys_string", "IDS_COM_SK_SET"),
						   setting_task_switcher_viewtype_click_softkey_cancel_cb,
						   setting_task_switcher_viewtype_click_softkey_set_cb,
						   ad, &scroller, &navi_bar);

	Elm_Object_Item *item = NULL;;

	item =
	    elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* scroller is a genlist */
	SETTING_TRACE("scroller:%p", scroller);
	ad->chk_task = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->chk_task, -1);

	setting_create_Gendial_field_1radio(scroller, &(ad->itc_1text_1icon_2),
					    setting_mouse_up_Gendial_list_radio_cb,
					    ad->chk_task, SWALLOW_Type_1RADIO,
					    ad->chk_task,
					    VCONFKEY_TASKSWITCHER_VIEWTYPE_LIST,
					    "IDS_ST_BODY_LIST_VIEW", NULL);

	setting_create_Gendial_field_1radio(scroller, &(ad->itc_1text_1icon_2),
					    setting_mouse_up_Gendial_list_radio_cb,
					    ad->chk_task, SWALLOW_Type_1RADIO,
					    ad->chk_task,
					    VCONFKEY_TASKSWITCHER_VIEWTYPE_CARD,
					    "Card view", NULL);
#ifdef UNUSED_VCONFKEY
	setting_update_chk_status(ad->chk_task,
				  INT_SLP_SETTING_TASKSWITCHER_VIEWTYPE);
#endif
	setting_view_task_switcher_viewtype.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_task_switcher_viewtype_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingViewTypeUG *ad = (SettingViewTypeUG *) cb;

	if (ad->ly_task != NULL) {
		evas_object_del(ad->ly_task);
		setting_view_task_switcher_viewtype.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_task_switcher_viewtype_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingViewTypeUG *ad = (SettingViewTypeUG *) cb;

	if (ad->ly_task != NULL) {
		evas_object_show(ad->ly_task);
#ifdef UNUSED_VCONFKEY
		setting_update_chk_status(ad->chk_task,
					  INT_SLP_SETTING_TASKSWITCHER_VIEWTYPE);
#endif
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_task_switcher_viewtype_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingViewTypeUG *ad = (SettingViewTypeUG *) cb;

	if (ad->ly_task != NULL) {
		evas_object_hide(ad->ly_task);
	}

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
setting_task_switcher_viewtype_click_softkey_cancel_cb(void *data,
						       Evas_Object *obj,
						       void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingViewTypeUG *ad = (SettingViewTypeUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

static void
setting_task_switcher_viewtype_click_softkey_set_cb(void *data,
						    Evas_Object *obj,
						    void *event_info)
{
	SETTING_TRACE_BEGIN;
	int value = 0;
	int err = 0;
	int ret = 0;

	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingViewTypeUG *ad = (SettingViewTypeUG *) data;

	value = elm_radio_value_get(ad->chk_task);
	SETTING_TRACE("value:%d", value);
#ifdef UNUSED_VCONFKEY
	ret =
	    setting_set_int_slp_key(INT_SLP_SETTING_TASKSWITCHER_VIEWTYPE,
				    value, &err);
#endif
	/* shared_gadget_send_command(SHARED_GADGET(ad), CLOSE_SETTING_DISPLAY_SG_CMD, NULL); */
	/* Send message to ug */
	/*bundle *b = bundle_create();
	   if (!b) return;
	   bundle_add(b, "updateType", "ON_OFF_TEXT");
	   bundle_add(b, "ON_OFF_TEXT", get_task_switcher_viewtype_str());
	   ug_send_result(ad->ug, b);
	   bundle_free(b); */

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}
