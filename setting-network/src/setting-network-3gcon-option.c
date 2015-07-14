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
#include <setting-network-3gcon-option.h>

static int setting_network_3gcon_create(void *cb);
static int setting_network_3gcon_destroy(void *cb);
static int setting_network_3gcon_update(void *cb);
static int setting_network_3gcon_cleanup(void *cb);

setting_view setting_view_network_3gcon = {
	.create = setting_network_3gcon_create,
	.destroy = setting_network_3gcon_destroy,
	.update = setting_network_3gcon_update,
	.cleanup = setting_network_3gcon_cleanup,
};

static void setting_network_3gcon_click_softkey_set_cb(void *data,
                                                       Evas_Object *obj,
                                                       void *event_info);
static void setting_network_3gcon_click_softkey_cancel_cb(void *data,
                                                          Evas_Object *obj,
                                                          void *event_info);

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_network_3gcon_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *scroller;
	setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
	                                     _("2G/3G connection"),
	                                     dgettext("sys_string", "IDS_ST_BUTTON_CANCEL_ABB"),
	                                     dgettext("sys_string", "IDS_ST_SK_SET_LITE"),
	                                     setting_network_3gcon_click_softkey_cancel_cb,
	                                     setting_network_3gcon_click_softkey_set_cb,
	                                     ad, &scroller, ad->navi_bar);

	/* scroller is a genlist */
	ad->chk_3gcon = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->chk_3gcon, -1);

	setting_create_Gendial_field_1radio(scroller, &itc_1text_1icon_2,
	                                    setting_mouse_up_Gendial_list_radio_cb,
	                                    ad->chk_3gcon, SWALLOW_Type_1RADIO,
	                                    ad->chk_3gcon,
	                                    SETTING_NETWORK_3GCON_NOT_USE,
	                                    "Do not use", NULL);

	setting_create_Gendial_field_1radio(scroller, &itc_1text_1icon_2,
	                                    setting_mouse_up_Gendial_list_radio_cb,
	                                    ad->chk_3gcon, SWALLOW_Type_1RADIO,
	                                    ad->chk_3gcon,
	                                    SETTING_NETWORK_3GCON_ASK_ON_CON,
	                                    _("IDS_ST_BODY_USB_ASK_ON_CONNECTION"), NULL);
	setting_create_Gendial_field_1radio(scroller, &itc_1text_1icon_2,
	                                    setting_mouse_up_Gendial_list_radio_cb,
	                                    ad->chk_3gcon, SWALLOW_Type_1RADIO,
	                                    ad->chk_3gcon,
	                                    SETTING_NETWORK_3GCON_USE_WO_ASK,
	                                    "Use without asking", NULL);

	int value = -1;
	int err = -1;
	err = vconf_get_int(VCONFKEY_3G_ENABLE, &value);
	if (err < 0) {
		elm_radio_value_set(ad->chk_3gcon,
		                    SETTING_NETWORK_3GCON_NOT_USE);
	} else {
		elm_radio_value_set(ad->chk_3gcon, value);
	}

	setting_view_network_3gcon.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_3gcon_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_3gcon.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_3gcon_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_3gcon_cleanup(void *cb)
{
	return setting_network_3gcon_destroy(cb);
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
setting_network_3gcon_click_softkey_set_cb(void *data, Evas_Object *obj,
                                           void *event_info)
{
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	int value;
	int err;

	value = elm_radio_value_get(ad->chk_3gcon);

	err = vconf_set_int(VCONFKEY_3G_ENABLE, value);

	if (err < 0) {
		SETTING_TRACE_DEBUG
		("%s*** [ERR] Failed to set 3G connection option ***%s",
		 SETTING_FONT_RED, SETTING_FONT_BLACK);
	}

	setting_view_change(&setting_view_network_3gcon,
	                    &setting_view_network_main, ad);
}

static void
setting_network_3gcon_click_softkey_cancel_cb(void *data, Evas_Object *obj,
                                              void *event_info)
{
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;

	setting_view_change(&setting_view_network_3gcon,
	                    &setting_view_network_main, ad);
}
