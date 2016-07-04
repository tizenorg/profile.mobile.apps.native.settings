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
#include <setting-network-select-provider.h>

static int setting_view_network_select_provider_create(void *cb);
static int setting_view_network_select_provider_destroy(void *cb);
static int setting_view_network_select_provider_update(void *cb);
static int setting_view_network_select_provider_cleanup(void *cb);

setting_view setting_view_network_select_provider = {
	.create = setting_view_network_select_provider_create,
	.destroy = setting_view_network_select_provider_destroy,
	.update = setting_view_network_select_provider_update,
	.cleanup = setting_view_network_select_provider_cleanup, };

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void __network_main_gl_mouse_up(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data || !event_info);
	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event_info;
	Elm_Object_Item *selected_item = elm_genlist_at_xy_item_get(
			ad->genlist_sel_provider, ev->output.x, ev->output.y,
			NULL);
	ret_if(!selected_item);
	Setting_GenGroupItem_Data *data_Item = elm_object_item_data_get(
			selected_item);
	ret_if(!data_Item);
	if (!safeStrCmp(data_Item->keyStr,
			"IDS_ST_BODY_SELECT_AUTOMATICALLY")) {
		SETTING_TRACE("ev->output.x:%d, ev->output.y:%d", ev->output.x,
				ev->output.y);
		Evas_Coord x, y, w, h;
		evas_object_geometry_get(ad->data_auto_network_item->eo_check,
				&x, &y, &w, &h);
		SETTING_TRACE("x:%d, y:%d, w:%d, h:%d", x, y, w, h);
		if (x <= ev->output.x
				&& ev->output.x <= (x + w)
				&& y <= ev->output.y
				&& ev->output.y <= (y + h)) {
			return;
		}
		if (ad->data_auto_network_item->chk_status) {
			char *sub_text = setting_customize_text(
					ad->data_auto_network_item->sub_desc, 0,
					BLUE_COLOR, NULL);
			FREE(ad->data_auto_network_item->sub_desc);
			ad->data_auto_network_item->sub_desc = (char *)g_strdup(
					sub_text);
			FREE(sub_text);
		} else {
			char *sub_text = setting_customize_text(
					ad->data_auto_network_item->sub_desc, 0,
					GRAY_COLOR, NULL);
			FREE(ad->data_auto_network_item->sub_desc);
			ad->data_auto_network_item->sub_desc = (char *)g_strdup(
					sub_text);
			FREE(sub_text);
		}

		elm_genlist_item_fields_update(ad->data_auto_network_item->item,
				"*", ELM_GENLIST_ITEM_FIELD_TEXT);
	}
}
static void __network_main_gl_mouse_down(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data || !event_info);
	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
	ad->point_down.x = ev->output.x;
	ad->point_down.y = ev->output.y;
	Elm_Object_Item *selected_item = elm_genlist_at_xy_item_get(
			ad->genlist_sel_provider, ev->output.x, ev->output.y,
			NULL);
	ret_if(!selected_item);
	Setting_GenGroupItem_Data *data_Item = elm_object_item_data_get(
			selected_item);
	ret_if(!data_Item);
	if (!safeStrCmp(data_Item->keyStr,
			"IDS_ST_BODY_SELECT_AUTOMATICALLY")) {
		SETTING_TRACE("ev->output.x:%d, ev->output.y:%d", ev->output.x,
				ev->output.y);
		Evas_Coord x, y, w, h;
		evas_object_geometry_get(ad->data_auto_network_item->eo_check,
				&x, &y, &w, &h);
		SETTING_TRACE("x:%d, y:%d, w:%d, h:%d", x, y, w, h);
		if (x <= ev->output.x
				&& ev->output.x <= (x + w)
				&& y <= ev->output.y
				&& ev->output.y <= (y + h)) {
			return;
		}

		char *sub_text = setting_customize_text(
				ad->data_auto_network_item->sub_desc, 0,
				WHITE_COLOR, NULL);
		FREE(ad->data_auto_network_item->sub_desc);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(
				sub_text);
		FREE(sub_text);
		elm_genlist_item_fields_update(ad->data_auto_network_item->item,
				"*", ELM_GENLIST_ITEM_FIELD_TEXT);
	}

}
static void __network_main_gl_mouse_move(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	ret_if(!data || !event_info);
	SettingNetworkUG *ad = (SettingNetworkUG *)data;
	Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;
	int x_offset = ev->cur.output.x - ad->point_down.x;
	int y_offset = ev->cur.output.y - ad->point_down.y;
	if (x_offset < 0)
		x_offset = -x_offset;
	if (y_offset < 0)
		y_offset = -y_offset;
	/*SETTING_TRACE("x_offset:%d, y_offset:%d", x_offset, y_offset); */
	if (x_offset <= MIN_MOVE_DISTANCE && y_offset <= MIN_MOVE_DISTANCE)
		return;

	/*SETTING_TRACE("xxxxxxxxxx"); */
	if (ad->data_auto_network_item->chk_status) {
		char *sub_text = setting_customize_text(
				ad->data_auto_network_item->sub_desc, 0,
				BLUE_COLOR, NULL);
		FREE(ad->data_auto_network_item->sub_desc);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(
				sub_text);
		FREE(sub_text);
	} else {
		char *sub_text = setting_customize_text(
				ad->data_auto_network_item->sub_desc, 0,
				GRAY_COLOR, NULL);
		FREE(ad->data_auto_network_item->sub_desc);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(
				sub_text);
		FREE(sub_text);
	}

	elm_genlist_item_fields_update(ad->data_auto_network_item->item, "*",
			ELM_GENLIST_ITEM_FIELD_TEXT);
}

static int setting_view_network_select_provider_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	Elm_Object_Item *navi_it = NULL;

	SettingNetworkUG *ad = (SettingNetworkUG *)cb;
	if (ad->view_to_load == &setting_view_network_select_provider) {
		ad->ly_main = setting_create_layout_navi_bar_genlist(
				ad->win_main_layout, ad->win_get,
				"IDS_COM_BODY_SEVICE_PROVIDERS",
				_("IDS_ST_BUTTON_BACK"), NULL,
				setting_network_select_network_click_softkey_cancel_cb,
				NULL,
				ad,
				&ad->genlist_sel_provider,
				&ad->navi_bar);
	} else {
		navi_it = setting_push_layout_navi_bar_genlist(
				ad->win_main_layout, ad->win_get,
				"IDS_COM_BODY_SEVICE_PROVIDERS",
				_("IDS_ST_BUTTON_BACK"), NULL,
				setting_network_select_network_click_softkey_cancel_cb,
				NULL,
				ad,
				&ad->genlist_sel_provider,
				ad->navi_bar);
	}

	elm_naviframe_item_style_set(navi_it, "tabbar");

	Evas_Object *toolbar = elm_toolbar_add(ad->navi_bar);
	elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_EXPAND);
	//elm_object_style_set(toolbar, "tabbar");
	elm_toolbar_reorder_mode_set(toolbar, EINA_FALSE);
	elm_toolbar_transverse_expanded_set(toolbar, EINA_TRUE);
	elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_object_item_part_content_set(navi_it, "tabbar", toolbar);

	elm_toolbar_item_append(toolbar, NULL, "SIM1", NULL, ad);
	elm_toolbar_item_append(toolbar, NULL, "SIM2", NULL, ad);

	evas_object_show(toolbar);


	/*m_Object_Item *item = elm_genlist_item_append(ad->genlist_sel_provider,
	 &(itc_seperator), NULL, NULL,
	 ELM_GENLIST_ITEM_NONE, NULL, NULL);
	 elm_genlist_item_select_mode_set(item,
	 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);*/

	/* scroller is a genlist */
	ad->chk_sel = elm_radio_add(ad->genlist_sel_provider);
	elm_radio_state_value_set(ad->chk_sel, -1);
	/*retrieve here */
	vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT, &ad->sel_net);
	SETTING_TRACE("ad->sel_net:%d", ad->sel_net);

	/*retrieve here */
	vconf_get_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT, &ad->sel_act);
	SETTING_TRACE("ad->sel_act:%d", ad->sel_act);

	FREE(ad->sel_network);
	/*ad->sel_network = vconf_get_str(VCONFKEY_TELEPHONY_NWNAME); */
	ad->sel_network = get_pa_select_network();
	SETTING_TRACE("pa_sel_net_name:%s", ad->sel_network);
	if (!ad->sel_network) {
		ad->sel_network = strndup(Keystr_UNKNOWN_NETWORK,
				strlen(Keystr_UNKNOWN_NETWORK) + 1);
	}

	/* resolve abnormal height issue */
	elm_genlist_mode_set(ad->genlist_sel_provider, ELM_LIST_COMPRESS);

	evas_object_event_callback_add(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_UP, __network_main_gl_mouse_up, ad);
	evas_object_event_callback_add(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_DOWN, __network_main_gl_mouse_down,
			ad);
	evas_object_event_callback_add(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_MOVE, __network_main_gl_mouse_move,
			ad);

	if (TAPI_NETWORK_SELECTIONMODE_AUTOMATIC == ad->sel_net) {
		ad->data_auto_network_item = setting_create_Gendial_field_1radio(
				ad->genlist_sel_provider, &(itc_1text_1icon),
				setting_network_Gendial_select_plmn_cb, ad,
				SWALLOW_Type_1ICON_1RADIO,
				NULL, TRUE, "IDS_ST_BODY_SELECT_AUTOMATICALLY",
				setting_network_select_network_chk_changed);
		setting_retvm_if(NULL == ad->data_auto_network_item,
				SETTING_RETURN_FAIL, "Failed to calloc memory");
		char *sub_text = setting_customize_text(_(ad->sel_network), 0,
				BLUE_COLOR, NULL);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(
				sub_text);
		FREE(sub_text);
		ad->data_auto_network_item->userdata = ad;

	} else if (TAPI_NETWORK_SELECTIONMODE_MANUAL == ad->sel_net) {

		ad->data_auto_network_item = setting_create_Gendial_field_1radio(
				ad->genlist_sel_provider, &(itc_1text_1icon),
				setting_network_Gendial_select_plmn_cb, ad,
				SWALLOW_Type_1ICON_1RADIO,
				NULL, FALSE, "IDS_ST_BODY_SELECT_AUTOMATICALLY",
				setting_network_select_network_chk_changed);

		setting_retvm_if(NULL == ad->data_auto_network_item,
				SETTING_RETURN_FAIL, "Failed to calloc memory");
		/*ad->data_auto_network_item->sub_desc = (char *)g_strdup(
		 * ad->sel_network);//just a simple display */
		char *sub_text = setting_customize_text(_(ad->sel_network), 0,
				GRAY_COLOR, NULL);
		ad->data_auto_network_item->sub_desc = (char *)g_strdup(
				sub_text);
		FREE(sub_text);
		ad->data_auto_network_item->userdata = ad;

		setting_view_network_select_provider.is_create = TRUE;

		setting_network_searching_network(ad->data_auto_network_item);

		/* Searching icon appeared.
		 ad->data_search_network_item = setting_create_Gendial_field_1radio(
		 ad->genlist_sel_provider,
		 &itc_1icon,
		 NULL, NULL,
		 SWALLOW_Type_1RADIO_1SEARCH,
		 ad->chk_sel, -1, NULL, NULL);
		 __BACK_POINTER_SET(ad->data_search_network_item);
		 elm_genlist_item_select_mode_set(
		 ad->data_search_network_item->item,
		 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		 */

		/*
		 if (0 == safeStrCmp("IDS_ST_BODY_NOSERVICE", ad->sel_network)
		 || 0 == safeStrCmp(_("IDS_ST_BODY_NOSERVICE"),
		 _(ad->sel_network))
		 || 0 == safeStrCmp("EMERGENCY", ad->sel_network)
		 || 0 == safeStrCmp(_("EMERGENCY"), ad->sel_network)
		 || 0 == safeStrCmp("No service", ad->sel_network)
		 || 0 == safeStrCmp("No service", _(ad->sel_network))
		 || 0 == safeStrCmp(_("IDS_CALL_POP_CALLING_EMERG_ONLY"),
		 _(ad->sel_network))
		 || 0 == safeStrCmp("Emergency calls only", _(ad->sel_network))
		 )
		 {
		 //no service case:
		 ad->data_search_network_item = setting_create_Gendial_field_1radio(
		 ad->genlist_sel_provider,
		 &itc_1icon,
		 NULL, NULL,
		 SWALLOW_Type_1RADIO_1SEARCH,
		 ad->chk_sel, -1, NULL, NULL);
		 __BACK_POINTER_SET(ad->data_search_network_item);
		 elm_genlist_item_select_mode_set(
		 ad->data_search_network_item->item,
		 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		 }
		 else
		 {
		 //normal case:
		 Setting_GenGroupItem_Data *item_Data = NULL;

		 char sel_network_desc[MAX_COMMON_BUFFER_LEN] = { 0, };

		 snprintf(sel_network_desc, MAX_COMMON_BUFFER_LEN, "%s",
		 _(ad->sel_network));
		 item_Data = setting_create_Gendial_field_1radio(
		 ad->genlist_sel_provider,
		 &itc_multiline_1text_1icon,
		 setting_network_Gendial_select_plmn_cb, ad,
		 SWALLOW_Type_1RADIO,
		 ad->chk_sel, TAPI_NETWORK_SELECTIONMODE_MANUAL,
		 sel_network_desc,
		 setting_network_select_network_chk_changed);
		 setting_retvm_if(NULL == item_Data, SETTING_RETURN_FAIL,
		 "Failed to calloc memory");
		 item_Data->userdata = ad;

		 ad->data_search_network_item = setting_create_Gendial_field_1radio(
		 ad->genlist_sel_provider,
		 &itc_1icon,
		 NULL, NULL,
		 SWALLOW_Type_1RADIO_1SEARCH,
		 ad->chk_sel, -1, NULL, NULL);
		 __BACK_POINTER_SET(ad->data_search_network_item);
		 elm_genlist_item_select_mode_set(
		 ad->data_search_network_item->item,
		 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		 elm_radio_value_set(ad->chk_sel,
		 TAPI_NETWORK_SELECTIONMODE_MANUAL);

		 }
		 */
	}

	setting_view_network_select_provider.is_create = TRUE;
	return SETTING_RETURN_SUCCESS;
	/*
	 err_handle:
	 setting_view_network_select_provider.is_create = TRUE;
	 return SETTING_RETURN_FAIL;
	 */
}

static int setting_view_network_select_provider_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *)cb;

	evas_object_event_callback_del(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_UP, __network_main_gl_mouse_up);
	evas_object_event_callback_del(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_DOWN, __network_main_gl_mouse_down);
	evas_object_event_callback_del(ad->genlist_sel_provider,
			EVAS_CALLBACK_MOUSE_MOVE, __network_main_gl_mouse_move);

	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}

	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}

	/*cancel the searching operation */
	if (ad->b_searching_network) {
		int tapi_ret = -1;
/*TODO select handle:*/
		tapi_ret = tel_cancel_network_manual_search(ad->handle[0],
				setting_tapi_cancel_manual_search_cb, ad);
		if (tapi_ret != TAPI_API_SUCCESS) {
			SETTING_TRACE_ERROR(
					"*** [ERR] tel_cancel_network_manual_search. " "tapi_ret=%d ***",
					tapi_ret);
		}
		ad->b_searching_network = FALSE;
	}
	/*roolback..*/
	if (0 != vconf_get_int(VCONFKEY_SETAPPL_SELECT_NETWORK_INT,
			&ad->sel_net)) {
		SETTING_TRACE_ERROR("vconf get failed");
	}

	if (0 != vconf_get_int(VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT,
			&ad->sel_act)) {
		SETTING_TRACE_ERROR("vconf get failed");
	}

	FREE(ad->sel_network);
	ad->sel_network = get_pa_select_network();
	if (!ad->sel_network) {
		ad->sel_network = strndup(Keystr_UNKNOWN_NETWORK,
				strlen(Keystr_UNKNOWN_NETWORK) + 1);
	}

	/*reset..*/
	ad->b_set_manul_network = FALSE;
	ad->b_set_auto_network = FALSE;
	setting_network_update_sel_network(ad);

	if (ad->view_to_load == &setting_view_network_select_provider) {
		if (ad->ly_main) {
			evas_object_del(ad->ly_main);
			ad->ly_main = NULL;
		}
	} else {
		elm_naviframe_item_pop(ad->navi_bar);
	}
	/* elm_naviframe_item_pop(ad->navi_bar); */

	/*Following handlers will be used by async listening callback. They
	 * must be reset after genlist is 'popuped'. */
	ad->data_auto_network_item = NULL;
	/*data_sel_net was created in main view, it won't be destoryed. */
	/*ad->data_sel_net = NULL; */
	setting_view_network_select_provider.is_create = FALSE;

	return SETTING_RETURN_SUCCESS;
}

static int setting_view_network_select_provider_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*SettingNetworkUG *ad = (SettingNetworkUG *) cb; */
	/*elm_radio_value_set(ad->chk_sel, ad->sel_net); */

	return SETTING_RETURN_SUCCESS;
}

static int setting_view_network_select_provider_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return setting_view_network_select_provider_destroy(cb);
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

static void setting_network_select_network_click_softkey_cancel_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *)data;

	/* If registering.. popup is displaying, Back must be ignored. */
	if (ad->network_select_registering_pop) {
		SETTING_TRACE("registering... popup is displaying. back key ignored");
		return;
	}

	/* error check */
	retm_if(data == NULL,
			"[Setting > Network > Select] Data parameter is NULL");

	if (ad->view_to_load == &setting_view_network_select_provider) {
		/* exit */
		ug_destroy_me(ad->ug);
		return;
	} else {
		setting_view_change(&setting_view_network_select_provider,
				&setting_view_network_main, ad);
	}
	return;
}

