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

#include <setting-applications-defaultapp.h>
#include <dbus/dbus.h>
#include <dbus/dbus-glib-lowlevel.h>


static int setting_applications_defaultapp_create(void *cb);
static int setting_applications_defaultapp_destroy(void *cb);
static int setting_applications_defaultapp_update(void *cb);
static int setting_applications_defaultapp_cleanup(void *cb);

static void setting_applications_defaultapp_click_softkey_cancel_cb(void *data, Evas_Object *obj, void *event_info);
static void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info);

static gboolean setting_applications_create_homescreen_setting_ug(void *data);

setting_view setting_view_applications_defaultapp = {
	.create = setting_applications_defaultapp_create,
	.destroy = setting_applications_defaultapp_destroy,
	.update = setting_applications_defaultapp_update,
	.cleanup = setting_applications_defaultapp_cleanup,
};
/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void _indicator_free(char *str)
{
	free(str);
}

void construct_defaultapp(void *data, Evas_Object *genlist)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	ad->data_home =
	    setting_create_Gendial_field_def(genlist, &(ad->itc_2text_2),
	                                     setting_applications_defaultapp_mouse_up_Gendial_list_cb,
	                                     ad, SWALLOW_Type_INVALID, NULL,
	                                     NULL, 0, "Home",
	                                     "Tizen home", NULL);
	if (ad->data_home) {
		ad->data_home->userdata = ad;
		ad->data_home->group_style = SETTING_GROUP_STYLE_TOP;
		__BACK_POINTER_SET(ad->data_home);
	} else {
		SETTING_TRACE_ERROR("ad->data_home is NULL");
	}


	ad->data_message =
	    setting_create_Gendial_field_def(genlist, &(ad->itc_2text_2),
	                                     setting_applications_defaultapp_mouse_up_Gendial_list_cb,
	                                     ad, SWALLOW_Type_INVALID, NULL,
	                                     NULL, 0, "Messages",
	                                     "Messages", NULL);
	if (ad->data_message) {
		ad->data_message->userdata = ad;
		ad->data_message->group_style = SETTING_GROUP_STYLE_TOP;
		__BACK_POINTER_SET(ad->data_message);
	} else {
		SETTING_TRACE_ERROR("ad->data_message is NULL");
	}


}

void destruct_defaultapp(void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
#if 0
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
#endif
}

static int setting_applications_defaultapp_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;
	Evas_Object *genlist = elm_genlist_add(ad->win_main_layout);
	retvm_if(genlist == NULL, SETTING_RETURN_FAIL, "Cannot set scroller object as contento of layout");
	/*elm_object_style_set(genlist, "dialogue"); */
	//elm_genlist_realization_mode_set(genlist, EINA_TRUE);
	elm_genlist_clear(genlist);	/* first to clear list */

	setting_push_layout_navi_bar("Default Applications",
	                             _("IDS_ST_BUTTON_BACK"), NULL, NULL,
	                             setting_applications_defaultapp_click_softkey_cancel_cb,
	                             NULL,
	                             NULL, ad, genlist, ad->navi_bar, NULL);

	/*ad->genlist = genlist; */
	evas_object_smart_callback_add(genlist, "realized", __gl_realized_cb, ad);

	construct_defaultapp(ad, genlist);/*------- */


	setting_view_applications_defaultapp.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_defaultapp_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	retv_if(!(setting_view_applications_defaultapp.is_create), SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	destruct_defaultapp(cb);/*------- */

	SettingApplicationsUG *ad = (SettingApplicationsUG *) cb;

	setting_view_applications_defaultapp.is_create = 0;
	elm_naviframe_item_pop(ad->navi_bar);

	return SETTING_RETURN_SUCCESS;
}

static int setting_applications_defaultapp_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;

}

static int setting_applications_defaultapp_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_applications_defaultapp_destroy(cb);

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
setting_applications_defaultapp_click_softkey_cancel_cb(void *data,
                                                        Evas_Object *obj,
                                                        void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

	setting_view_change(&setting_view_applications_defaultapp,
	                    &setting_view_applications_main, ad);
}

void setting_applications_defaultapp_mouse_up_Gendial_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	/*SETTING_TRACE("chk_status[%d]", list_item->chk_status); */
	if (!strcmp(_(list_item->keyStr), "Home")) {
		SETTING_TRACE("click Home and run UG");
		setting_applications_create_homescreen_setting_ug(ad);
	} else if (!strcmp(_(list_item->keyStr), "Messages")) {
		SETTING_TRACE("click Home and run Message logic - do nothing now.");
	}
}

static void setting_applications_destroy_homescreen_setting_ug_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(priv == NULL);
	SettingApplicationsUG *ad = (SettingApplicationsUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_homescreen = NULL;
	}
	elm_genlist_realized_items_update(ad->genlist);
}

static gboolean setting_applications_create_homescreen_setting_ug(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, FALSE);

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;	/* ad is point to data */

	if (ad->ug_homescreen) {
		SETTING_TRACE("Font UG is already loaded.");
		return FALSE;
	}
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));

	if (!cbs) {
		return FALSE;
	}
	cbs->layout_cb = setting_applications_layout_ug_cb;
	cbs->result_cb = NULL;
	cbs->destroy_cb = setting_applications_destroy_homescreen_setting_ug_cb;
	cbs->priv = (void *)ad;

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);

	ad->ug_homescreen = setting_ug_create(ad->ug, "setting-homescreen-efl", UG_MODE_FULLVIEW, NULL, cbs);
	if (NULL == ad->ug_homescreen) {	/* error handling */
		SETTING_TRACE_ERROR("NULL == ad->ug_homescreen");
		evas_object_show(ad->ly_main);
	}
	FREE(cbs);

	return TRUE;
}


