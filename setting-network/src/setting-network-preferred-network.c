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
#include <setting-network-preferred-network.h>
#include <efl_extension.h>

static int setting_network_preferred_network_create(void *cb);
static int setting_network_preferred_network_destroy(void *cb);
static int setting_network_preferred_network_update(void *cb);
static int setting_network_preferred_network_cleanup(void *cb);

setting_view setting_view_network_preferred_network = {
	.create = setting_network_preferred_network_create,
	.destroy = setting_network_preferred_network_destroy,
	.update = setting_network_preferred_network_update,
	.cleanup = setting_network_preferred_network_cleanup,
};

static void setting_network_preferred_network_Gendial_list_item_mouse_up_cb(void
                                                                            *data,
                                                                            Evas_Object
                                                                            *obj,
                                                                            void
                                                                            *event_info);
static void setting_network_preferred_network_click_softkey_add_cb(void *data,
                                                                   Evas_Object *
                                                                   obj, void
                                                                   *event_info);
static void setting_network_preferred_network_click_softkey_delete_cb(void
                                                                      *data,
                                                                      Evas_Object
                                                                      *obj,
                                                                      void
                                                                      *event_info);
static void setting_network_preferred_network_click_softkey_back_cb(void *data,
                                                                    Evas_Object
                                                                    *obj, void
                                                                    *event_info);
static void setting_network_preferred_network_click_ctxpopup_menu_1_cb(void
                                                                       *data,
                                                                       Evas_Object
                                                                       *obj,
                                                                       void
                                                                       *event_info);
static void setting_network_preferred_network_click_ctxpopup_menu_2_cb(void
                                                                       *data,
                                                                       Evas_Object
                                                                       *obj,
                                                                       void
                                                                       *event_info);

static int setting_network_preferred_network_draw_network_list(void *data);

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_network_preferred_network_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	int tapi_ret = TAPI_API_SUCCESS;

	/*bool keep = TRUE; */
	tapi_ret = tel_get_network_preferred_plmn(ad->handle, setting_tapi_get_preferred_plmn_cb, ad);
	if (tapi_ret != TAPI_API_SUCCESS) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] tel_get_network_preferred_plmn. err=%d ***%s",
		 SETTING_FONT_RED, tapi_ret, SETTING_FONT_BLACK);
		/*keep = FALSE; */
	}

	ad->scl_pref = elm_genlist_add(ad->win_main_layout);
	retvm_if(ad->scl_pref == NULL, SETTING_RETURN_FAIL,
	         "Cannot set scroller object as contento of layout");
//	elm_genlist_realization_mode_set(ad->scl_pref, EINA_TRUE);
	elm_object_style_set(ad->scl_pref , "dialogue");
	elm_genlist_clear(ad->scl_pref);	/* first to clear list */
	evas_object_smart_callback_add(ad->scl_pref, "realized", __gl_realized_cb, NULL);

	setting_push_layout_navi_bar(_("IDS_ST_BODY_PREFERRED_NETWORKS"),
	                             _("IDS_ST_BUTTON_BACK"), _("IDS_COM_ADD"),
	                             _("IDS_ST_BODY_DELETE"),
	                             setting_network_preferred_network_click_softkey_back_cb,
	                             setting_network_preferred_network_click_softkey_add_cb,
	                             setting_network_preferred_network_click_softkey_delete_cb,
	                             ad, ad->scl_pref, ad->navi_bar, NULL);

	/* create preffered network list */
	setting_network_preferred_network_draw_network_list(ad);
	/* ad->pop_pref_sch = setting_network_preferred_network_create_search_popup(ad); */
	ad->network_ug_pop =
	    setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_ST_POP_NETWORKSERVICES_SEARCHING"),
	                                     NULL, 0.0, TRUE, TRUE);

	setting_view_network_preferred_network.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_preferred_network.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_update(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	setting_network_preferred_network_refresh_view(ad);

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_preferred_network_cleanup(void *cb)
{
	return setting_network_preferred_network_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

int setting_network_preferred_network_refresh_view(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_network_preferred_network_draw_network_list(ad);

	return 0;
}

static int setting_network_preferred_network_draw_network_list(void *data)
{
	/* error check */
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	int i = 0;
	elm_genlist_clear(ad->scl_pref);	/* first to clear list */
	for (i = 0; i < ad->pref_list.NumOfPrefPlmns; i++) {
		setting_create_Gendial_field_def(ad->scl_pref, &itc_1text,
		                                 setting_network_preferred_network_Gendial_list_item_mouse_up_cb,
		                                 ad, SWALLOW_Type_INVALID, NULL,
		                                 NULL, 0,
		                                 ad->
		                                 pref_list.PrefPlmnRecord
		                                 [i].network_name, NULL, NULL);
	}

	return 0;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void
setting_network_preferred_network_Gendial_list_item_mouse_up_cb(void *data,
                                                                Evas_Object *
                                                                obj, void
                                                                *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	int i;

	for (i = 0; i < ad->pref_list.NumOfPrefPlmns; i++) {
		if (!safeStrCmp
		    (ad->pref_list.PrefPlmnRecord[i].network_name,
		     list_item->keyStr)) {
			break;
		}
	}

	SETTING_TRACE("i:%d", i);
	ad->sel_pref_idx = ad->pref_list.PrefPlmnRecord[i].Index;
	setting_view_change(&setting_view_network_preferred_network,
	                    &setting_view_network_preferred_network_edit, ad);
}

static void
setting_network_preferred_network_click_softkey_back_cb(void *data,
                                                        Evas_Object *obj,
                                                        void *event_info)
{
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter is NULL");
	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_preferred_network,
	                    &setting_view_network_main, ad);
}

static void
setting_network_preferred_network_click_ctxpopup_menu_1_cb(void *data,
                                                           Evas_Object *obj,
                                                           void *event_info)
{
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	evas_object_del(ad->popup);

	setting_view_change(&setting_view_network_preferred_network,
	                    &setting_view_network_preferred_network_list, ad);
}

static void
setting_network_preferred_network_click_ctxpopup_menu_2_cb(void *data,
                                                           Evas_Object *obj,
                                                           void *event_info)
{
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	evas_object_del(ad->popup);

	setting_view_change(&setting_view_network_preferred_network,
	                    &setting_view_network_preferred_network_new, ad);
}

static void
setting_network_preferred_network_click_softkey_add_cb(void *data,
                                                       Evas_Object *obj,
                                                       void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	ad->popup = elm_ctxpopup_add(ad->navi_bar);
	eext_object_event_callback_add(ad->popup, EEXT_CALLBACK_BACK, eext_ctxpopup_back_cb, NULL);

	/* evas_object_smart_callback_add(ad->popup, "hide", _ctxpopup_cb, ad->popup); */

	elm_ctxpopup_item_append(ad->popup, _("IDS_ST_BODY_NETWORK_LIST"), NULL,
	                         setting_network_preferred_network_click_ctxpopup_menu_1_cb,
	                         ad);
	/*currently,this code logic is not used,so remove the ID which is not in po file*/
	elm_ctxpopup_item_append(ad->popup, _(""),
	                         NULL,
	                         setting_network_preferred_network_click_ctxpopup_menu_2_cb,
	                         ad);

	evas_object_resize(ad->popup, 480 * WIDGET_SCALE_FACTOR,
	                   300 * WIDGET_SCALE_FACTOR);

	Evas_Coord x, y, w, h;
	evas_object_geometry_get(obj, &x, &y, &w, &h);
	evas_object_move(ad->popup, (x + w / 2) * WIDGET_SCALE_FACTOR,
	                 (y + h) * WIDGET_SCALE_FACTOR);
	evas_object_show(ad->popup);
}

static void
setting_network_preferred_network_click_softkey_delete_cb(void *data,
                                                          Evas_Object *obj,
                                                          void *event_info)
{
	/* error check */
	retm_if(data == NULL,
	        "[Setting > Network > Preferred Net] Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	if (ad->pref_list.NumOfPrefPlmns == 0) {
		setting_create_simple_popup(ad, ad->win_get,
		                            NULL, _("No Network to delete!"));
	} else {
		setting_view_change(&setting_view_network_preferred_network,
		                    &setting_view_network_preferred_network_delete,
		                    ad);
	}
}
