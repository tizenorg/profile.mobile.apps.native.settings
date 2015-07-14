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
#include <setting-reset.h>
#include <ITapiModem.h>
#include <TapiUtility.h>
#include <tapi_common.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_reset_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
				       void *event_info)
{
	SettingResetUG *ad = (SettingResetUG *) data;
	setting_view_update(&setting_view_reset_main, ad);
}

static void *setting_reset_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
					service_h service, void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingResetUG *resetUG = priv;
	resetUG->ug = ug;

	resetUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	resetUG->win_get = (Evas_Object *) ug_get_window();

	evas_object_show(resetUG->win_main_layout);
	resetUG->evas = evas_object_evas_get(resetUG->win_main_layout);

	setting_retvm_if(resetUG->win_main_layout == NULL, NULL,
			 "cannot get main window ");

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	setting_create_Gendial_itc("multiline/1text", &(resetUG->itc_1text));
	setting_create_Gendial_itc("dialogue/bg/1icon", &(resetUG->itc_1icon));

	/*  creating a view. */
	setting_view_node_table_register(&setting_view_reset_main, NULL);
	setting_view_node_table_register(&setting_view_reset_settings, &setting_view_reset_main);
	setting_view_node_set_cur_view(&setting_view_reset_main);

	setting_view_create(&setting_view_reset_main, (void *)resetUG);
	evas_object_event_callback_add(resetUG->win_main_layout,
				       EVAS_CALLBACK_RESIZE,
				       setting_reset_ug_cb_resize, resetUG);
	return resetUG->ly_main;
}

static void setting_reset_ug_on_start(ui_gadget_h ug, service_h service,
				      void *priv)
{
}

static void setting_reset_ug_on_pause(ui_gadget_h ug, service_h service,
				      void *priv)
{
}

static void setting_reset_ug_on_resume(ui_gadget_h ug, service_h service,
				       void *priv)
{
}

static void setting_reset_ug_on_destroy(ui_gadget_h ug, service_h service,
					void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingResetUG *resetUG = priv;

	evas_object_event_callback_del(resetUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_reset_ug_cb_resize);	/* fix flash issue for gallery */
	resetUG->ug = ug;
	/*  delete the allocated objects. */
	setting_view_destroy(&setting_view_reset_settings, resetUG);
	setting_view_destroy(&setting_view_reset_main, resetUG);

	if (NULL != ug_get_layout(resetUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(resetUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(resetUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_reset_ug_on_message(ui_gadget_h ug, service_h msg,
					service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_reset_ug_on_event(ui_gadget_h ug, enum ug_event event,
				      service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(!priv, "!priv");
	SettingResetUG *ad = (SettingResetUG *) priv;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
	{
		/* navigation item in reset setting view */
		Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
		ret_if(!navi_it);
		elm_object_item_text_set(navi_it, _(RESET_SETTINGS_STR));

		/* navigation item in reset main view */
		navi_it = elm_naviframe_bottom_item_get(ad->navi_bar);
		ret_if(!navi_it);
		elm_object_item_text_set(navi_it, _("IDS_COM_BODY_RESET"));

		if (ad->main_scroller)
		{
			elm_genlist_realized_items_update(ad->main_scroller);
		}
		SETTING_TRACE("ad->main_scroller:%p", ad->main_scroller);

		if (ad->controllbar)
		{
			Elm_Object_Item *item = NULL;
			item = elm_toolbar_first_item_get(ad->controllbar);
			if (item)
			{
#if SUPPORT_BOTTOM_BTNS
				elm_object_item_text_set(item, _("IDS_COM_BODY_RESET"));
#else
				Evas_Object *eo_btn = elm_object_item_part_content_get(item, "object");
				setting_retm_if(eo_btn == NULL, "get eo_lbtn failed");
				elm_object_text_set(eo_btn, _("IDS_COM_SK_CANCEL"));
#endif
			}

			item = elm_toolbar_last_item_get(ad->controllbar);
			if (item)
			{
#if SUPPORT_BOTTOM_BTNS
				elm_object_item_text_set(item, _("IDS_COM_SK_CANCEL"));
#else
				Evas_Object *eo_btn = elm_object_item_part_content_get(item, "object");
				setting_retm_if(eo_btn == NULL, "get eo_lbtn failed");
				elm_object_text_set(eo_btn, _("IDS_COM_BODY_RESET"));
#endif
			}
		}
		Evas_Object *sub_layout = elm_object_part_content_get(ad->view_layout, "elm.swallow.contents");
		if (sub_layout)
		{
			Evas_Object *select_all_layout = elm_object_part_content_get(sub_layout, "search_bar");
			if (select_all_layout) {
				elm_object_part_text_set(select_all_layout, "elm.text", _(KeyStr_Select_All));
			}
		}

		if (ad->reset_list)
			elm_genlist_realized_items_update(ad->reset_list);

		// setting_reset_result_popup_update(ad);

		break;
	}

	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_reset_ug_on_key_event(ui_gadget_h ug,
					  enum ug_key_event event,
					  service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

	switch (event) {
	case UG_KEY_EVENT_END:
		ug_destroy_me(ug);
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingResetUG *resetUG = calloc(1, sizeof(SettingResetUG));
	setting_retvm_if(!resetUG, -1, "Create SettingResetUG obj failed");

	ops->create = setting_reset_ug_on_create;
	ops->start = setting_reset_ug_on_start;
	ops->pause = setting_reset_ug_on_pause;
	ops->resume = setting_reset_ug_on_resume;
	ops->destroy = setting_reset_ug_on_destroy;
	ops->message = setting_reset_ug_on_message;
	ops->event = setting_reset_ug_on_event;
	ops->key_event = setting_reset_ug_on_key_event;
	ops->priv = resetUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingResetUG *resetUG;
	setting_retm_if(!ops, "ops == NULL");

	resetUG = ops->priv;
	if (resetUG)
		FREE(resetUG);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
#if DISABLED_CODE
static void __ask_popup_response_cb(void *data, Evas_Object *obj, void *event_info)
{
	if (btn_type(obj) == POPUP_RESPONSE_OK) {
		elm_exit();//exit the Setting app
	}

	ret_if(!data);
	SettingResetUG *ad = data;
	if (ad->ask_popup) {
		evas_object_del(ad->ask_popup);
		ad->ask_popup = NULL;
	}
}
#endif

void setting_reset_result_popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingResetUG *ad = data;
	if (btn_type(obj) == POPUP_RESPONSE_OK) {
	}
	if (ad->notify) {
		evas_object_del(ad->notify);
		ad->notify = NULL;
	}

	if (ad->old_notify) {
		evas_object_del(ad->old_notify);
		ad->old_notify = NULL;
	}
	ad->pop_btn = NULL;
}

/**
 * language change makes B/S on resetting "Language&Region UG"
 */
void setting_reset_result_popup_update(void *data)
{
#if 1
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	SettingResetUG *ad = data;
	if (ad->notify) {
		elm_object_part_text_set(ad->notify, "title,text", _(RESET_RESULT_STR));
		Evas_Object *btn = elm_object_part_content_get(ad->notify, "button1");
		elm_object_style_set(btn, "popup_button/default");
		if (btn)
		{
			elm_object_text_set(btn, _("IDS_COM_SK_OK"));
		}
		Evas_Object *genlist = elm_object_content_get(ad->notify);
		if(genlist)
		{
			elm_genlist_realized_items_update(genlist);
			elm_object_content_set(ad->notify, genlist);
		}
		//evas_object_show(ad->notify);
	}
#endif
}


static void __tapi_flight_mode_cb(TapiHandle *handle, int result, void *data, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("result:%d", result);
}
int setting_reset_flight_mode()
{
	int ret = -1;
	TapiHandle *handle = tel_init(NULL);
	if (!handle) {
		SETTING_TRACE_ERROR("tel_init error");
	}

	//if callback is NULL,it will return failed directly.
	if (tel_set_flight_mode(handle, TAPI_POWER_FLIGHT_MODE_LEAVE, __tapi_flight_mode_cb, NULL) == 0) {
#if SUPPORT_RUN_SYSTEM_COMMAND
		ret = excuteCmd(SETTING_POSTINST_FILE, 1, "flightmode");
#else
		ret = vconf_set_bool(VCONFKEY_TELEPHONY_FLIGHT_MODE, FALSE);
#endif
	} else {
		SETTING_TRACE_ERROR("tel_set_flight_mode error");
	}


	if (tel_deinit(handle) != 0) {
		SETTING_TRACE_ERROR("tel_deinit error");
	}
	return ret;
}

int setting_reset_rotation_lock(void)
{
#if SUPPORT_RUN_SYSTEM_COMMAND
	return excuteCmd(SETTING_POSTINST_FILE, 1, "rotationLock");
#else
	return vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, FALSE);
#endif
}

