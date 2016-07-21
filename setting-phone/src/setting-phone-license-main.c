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

#include <setting-phone-license-main.h>
#define TEXT_CLEAR_LICENSE	 "IDS_ST_BODY_REMOVE_LICENCE"

static int setting_phone_license_main_create(void *cb);
static int setting_phone_license_main_destroy(void *cb);
static int setting_phone_license_main_update(void *cb);
static int setting_phone_license_main_cleanup(void *cb);

setting_view setting_view_phone_license_main = {
	.create = setting_phone_license_main_create,
	.destroy = setting_phone_license_main_destroy,
	.update = setting_phone_license_main_update,
	.cleanup = setting_phone_license_main_cleanup, };

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_phone_license_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhone *ad = (SettingPhone *)cb;
	int ret;

	ret = view_init(&ad->md, _("IDS_ST_MBODY_LEGAL_INFORMATION_ABB"));
	if (ret != SETTING_RETURN_SUCCESS)
		return ret;

	char setBtnStr[MAX_DISPLAY_NAME_LEN_ON_UI];
	snprintf(setBtnStr, sizeof(setBtnStr), "%s",
			(char *)dgettext("sys_string", "IDS_ST_BUTTON_BACK"));
	setting_create_navi_bar_buttons(
			_("IDS_ST_MBODY_LEGAL_INFORMATION_ABB"), setBtnStr,
			setting_phone_license_main_click_softkey_cancel,
			ad, ad->md.genlist, ad->md.navibar_main, NULL);

	/* [UI] open source license */
	setting_create_Gendial_field_def(ad->md.genlist, &(ad->itc_1text),
			setting_phone_license_main_mouse_up_Gendial_list_cb, ad,
			SWALLOW_Type_INVALID, NULL, NULL, 0,
			"IDS_ST_BODY_OPEN_SOURCE_LICENCES", NULL, NULL);

	setting_view_phone_license_main.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_phone_license_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhone *ad = (SettingPhone *)cb;

	if (ad->ly_license != NULL) {
		SETTING_TRACE("ad->ly_license:%p", ad->ly_license);
		/* SETTING_TRACE("ad->roaming_nw_dialData:%p",
		 * ad->roaming_nw_dialData); */
		if (ad->popup_clear_license) {
			evas_object_del(ad->popup_clear_license);
			ad->popup_clear_license = NULL;
		}

		evas_object_del(ad->ly_license);
		/* if(ad->roaming_nw_dialData) FREE(ad->roaming_nw_dialData); */
		setting_view_phone_license_main.is_create = 0;
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_phone_license_main_update(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPhone *ad = (SettingPhone *)cb;

	if (ad->ly_license != NULL) {
		evas_object_show(ad->ly_license);
		/* setting_check_onoff_status(ad->chk_expiry,
		 * BOOL_SLP_SETTING_EXPIRY_REMINDER); */

		/* setting_update_chk_status(ad->chk_roaming_nw,
		 * INT_SLP_SETTING_ROAMING_NETWORK); */
		/* edje_object_part_text_set(_EDJ(ad->li_roaming_nw),
		 * "bottom.text", get_pa_roaming_network_str()); */
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_phone_license_main_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	/*
	SettingPhone *ad = (SettingPhone *)cb;

	if (ad->ly_license != NULL)
		evas_object_hide(ad->ly_license);
	*/

	return SETTING_RETURN_SUCCESS;
}

static void __ug_layout_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *base;
	ret_if(!priv);
	base = (Evas_Object *)ug_get_layout(ug);
	ret_if(!base);

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
		EVAS_HINT_EXPAND);
		/*elm_win_resize_object_add(ad->md.win_main, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}

	SETTING_TRACE_END;
}

static void __ug_destroy_cb(ui_gadget_h ug, void *priv)
{
	SETTING_TRACE_BEGIN;

	/* restore the '<-' button on the navigate bar */
	ret_if(!priv);
	SettingPhone *ad = (SettingPhone *)priv; /* ad is point to priv */

	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_loading = NULL;
	}

}

static void __ug_result_cb(ui_gadget_h ug, app_control_h result, void *priv)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(priv == NULL, "Data parameter is NULL");

	SettingPhone *ad = (SettingPhone *)priv;

	if (result) {
		char *webkit_address = NULL;
		app_control_get_extra_data(result, "webkit_address",
				&webkit_address);
		SETTING_TRACE("webkit_address = %s", webkit_address);

		app_control_h svc;
		if (app_control_create(&svc)) {
			FREE(webkit_address);
			return;
		}

		app_control_add_extra_data(svc, "webkit_address",
				webkit_address);

		ug_send_result(ad->ug, svc);
		app_control_destroy(svc);
		FREE(webkit_address);
	}
}

static void __main_license_clicked(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	int ret;
	SettingPhoneUG *ad = (SettingPhoneUG *)data;
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	setting_retm_if(cbs == NULL, "calloc failed");
	cbs->layout_cb = __ug_layout_cb;
	cbs->result_cb = __ug_result_cb;
	cbs->destroy_cb = __ug_destroy_cb;
	cbs->priv = (void *)ad;

	/*bundle *b = bundle_create(); */
	app_control_h svc;
	if (app_control_create(&svc)) {
		FREE(cbs);
		return;
	}
	app_control_add_extra_data(svc, "file",
			SETTING_OPEN_SOURCE_LICENSE_PATH);
	app_control_add_extra_data(svc, "title",
			"IDS_ST_BODY_OPEN_SOURCE_LICENCES");

	if (ad->ly_language)
		elm_object_tree_focus_allow_set(ad->ly_language, EINA_FALSE);

	ret = app_launcher_svc("org.tizen.setting-fileview", NULL, NULL, svc);
	/*
	ad->ug_loading = setting_ug_create(ad->ug, "setting-fileview-efl",
			UG_MODE_FULLVIEW, svc, cbs);
	if (NULL == ad->ug_loading)	// error handling
	*/
	if (ret != 0)
		setting_create_popup(ad, ad->win_get, NULL,
				_(UNSUPPORTED_FUNCTION), NULL, 0, false, false,
				0);

	app_control_destroy(svc);
	FREE(cbs);
}

void setting_phone_license_main_mouse_up_Gendial_list_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	/*SettingPhone *ad = (SettingPhone *) data; */

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (!safeStrCmp("IDS_ST_BODY_OPEN_SOURCE_LICENCES",
			list_item->keyStr)) {
		__main_license_clicked(data);
	}
}

void setting_phone_license_main_popup_resp_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPhone *ad = (SettingPhone *)data;
	if (ad->popup) {
		evas_object_del(ad->popup);
		ad->popup = NULL;
	}
}

void setting_phone_license_main_click_softkey_cancel(void *data,
		Evas_Object *obj, void *event_info)
{
	SettingPhone *ad = (SettingPhone *)data;
	ret_if(!ad);
	if (ad->md.win_main)
		elm_win_lower(ad->md.win_main);
}
