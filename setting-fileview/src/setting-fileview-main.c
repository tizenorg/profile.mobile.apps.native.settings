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
#include <setting-fileview-main.h>

#define SETTING_FILEVIEW_LEN 20

static int setting_fileview_create(void *cb);
static int setting_fileview_destroy(void *cb);
static int setting_fileview_update(void *cb);
static int setting_fileview_cleanup(void *cb);

setting_view setting_view_fileview_main = {
	.create = setting_fileview_create,
	.destroy = setting_fileview_destroy,
	.update = setting_fileview_update,
	.cleanup = setting_fileview_cleanup, };

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static void __setting_fileview_policy_navigation_decide(void *data,
		Evas_Object *webview, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	setting_retm_if(event_info == NULL, "event_info is NULL");

	/*SettingFileview *ad = (SettingFileview *)data; */

	/* open url in local window */
	Ewk_Policy_Decision *policy_decision = (Ewk_Policy_Decision *)event_info;

	char *url = (char *)ewk_policy_decision_url_get(policy_decision);
	char *scheme = (char *)ewk_policy_decision_scheme_get(policy_decision);
	SETTING_TRACE("url : %s", url);
	SETTING_TRACE("scheme : %s", scheme);

	/* if not launch broswer, do not call ewk_policy_decision_ignore */
	/*ewk_policy_decision_ignore(policy_decision); */
}

static int setting_fileview_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	int ret;
	Evas_Object *scroller = NULL;
	SettingFileview *ad = (SettingFileview *)cb;
	retv_if(!ad, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	ret = view_init(&ad->md, _(ad->input_title));
	if (ret != SETTING_RETURN_SUCCESS)
		return ret;

	setting_create_navi_bar_buttons(
			_(ad->input_title),
			_("IDS_ST_BUTTON_BACK"),
			setting_fileview_click_back_cb,
			ad, ad->md.genlist, ad->md.navibar_main,
			NULL);

	Evas_Object *webview = ewk_view_add(ad->md.evas);
	ad->webkit = webview;

	SETTING_TRACE("ad->input_file:%s", ad->input_file);
	if (ewk_view_url_set(ad->webkit, ad->input_file) != EINA_TRUE)
		SETTING_TRACE_ERROR("Cnnot set view url %s", ad->input_file);

	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_FALSE);
	elm_object_content_set(scroller, ad->webkit);

	elm_access_object_register(ad->webkit, ad->md.win_main);
	evas_object_show(ad->webkit);
	ad->scroller = scroller;

	evas_object_smart_callback_add(ad->webkit, "policy,navigation,decide",
			__setting_fileview_policy_navigation_decide, ad);

	setting_view_fileview_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_fileview_destroy(void *cb)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingFileview *ad = (SettingFileview *)cb;
	evas_object_smart_callback_del(ad->webkit, "policy,navigation,decide",
			__setting_fileview_policy_navigation_decide);

	if (ad->webkit) {

		elm_object_content_unset(ad->webkit);
		evas_object_del(ad->webkit);
		ad->webkit = NULL;
	}

	if (ad->scroller) {
		evas_object_del(ad->scroller);
		ad->scroller = NULL;
	}

	if (ad->md.ly_main != NULL) {
		evas_object_del(ad->md.ly_main);
		ad->md.ly_main = NULL;
		setting_view_fileview_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_fileview_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingFileview *ad = (SettingFileview *)cb;

	if (ad->md.ly_main != NULL)
		evas_object_show(ad->md.ly_main);

	return SETTING_RETURN_SUCCESS;

}

static int setting_fileview_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingFileview *ad = (SettingFileview *)cb;

	if (ad->md.ly_main != NULL)
		evas_object_hide(ad->md.ly_main);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void setting_fileview_click_back_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	ui_app_exit();
	return;
}
