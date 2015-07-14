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

#include <setting-powersaving.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

static void setting_powersaving_ug_cb_resize(void *data, Evas *e,
					     Evas_Object *obj,
					     void *event_info)
{
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;
	setting_view_update(&setting_view_powersaving_main, ad);
}

static void *setting_powersaving_ug_on_create(ui_gadget_h ug,
					      enum ug_mode mode, service_h service,
					      void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!priv), NULL, "!priv");

	SettingPowersavingUG *powersavingUG = priv;
	powersavingUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	powersavingUG->win_main_layout =
	    (Evas_Object *) ug_get_parent_layout(ug);
	powersavingUG->win_get = (Evas_Object *) ug_get_window();

	evas_object_show(powersavingUG->win_main_layout);
	powersavingUG->evas =
	    evas_object_evas_get(powersavingUG->win_main_layout);

	setting_retvm_if(powersavingUG->win_main_layout == NULL, NULL,
			 "cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();	/* do detail registering in __get_motion_view_to_load() */

	/*  creating a view. */
	setting_view_node_table_register(&setting_view_powersaving_main, NULL);
	setting_view_node_table_register(&setting_view_powersaving_about,
					 &setting_view_powersaving_main);
	setting_view_node_table_register(&setting_view_powersaving_customed,
					 &setting_view_powersaving_main);
	setting_view_node_table_register(&setting_view_powersaving_brightness,
					 &setting_view_powersaving_customed);

	setting_view_node_set_cur_view(&setting_view_powersaving_main);
	setting_view_create(&setting_view_powersaving_main,
			    (void *)powersavingUG);
	evas_object_event_callback_add(powersavingUG->win_main_layout,
				       EVAS_CALLBACK_RESIZE,
				       setting_powersaving_ug_cb_resize,
				       powersavingUG);
	return powersavingUG->ly_main;
}

static void setting_powersaving_ug_on_start(ui_gadget_h ug, service_h service,
					    void *priv)
{
}

static void setting_powersaving_ug_on_pause(ui_gadget_h ug, service_h service,
					    void *priv)
{
}

static void setting_powersaving_ug_on_resume(ui_gadget_h ug,
					     service_h service, void *priv)
{
}

static void setting_powersaving_ug_on_destroy(ui_gadget_h ug,
					      service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingPowersavingUG *powersavingUG = priv;

	evas_object_event_callback_del(powersavingUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_powersaving_ug_cb_resize);	/* fix flash issue for gallery */
	powersavingUG->ug = ug;
	/*  called when this shared gadget is terminated. similar with app_exit */
	setting_view_destroy(&setting_view_powersaving_brightness,
			     powersavingUG);
	setting_view_destroy(&setting_view_powersaving_customed, powersavingUG);
	setting_view_destroy(&setting_view_powersaving_about, powersavingUG);
	setting_view_destroy(&setting_view_powersaving_main, powersavingUG);

	if (NULL != ug_get_layout(powersavingUG->ug)) {
		evas_object_hide((Evas_Object *)
				 ug_get_layout(powersavingUG->ug));
		evas_object_del((Evas_Object *)
				ug_get_layout(powersavingUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_powersaving_ug_on_message(ui_gadget_h ug,
					      service_h msg, service_h service,
					      void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_powersaving_ug_on_event(ui_gadget_h ug,
					    enum ug_event event, service_h service,
					    void *priv)
{
	SETTING_TRACE_BEGIN;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
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

static void setting_powersaving_ug_on_key_event(ui_gadget_h ug,
						enum ug_key_event event,
						service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

	switch (event) {
	case UG_KEY_EVENT_END:
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingPowersavingUG *powersavingUG =
	    calloc(1, sizeof(SettingPowersavingUG));
	setting_retvm_if(!powersavingUG, -1,
			 "Create SettingPowersavingUG obj failed");

	ops->create = setting_powersaving_ug_on_create;
	ops->start = setting_powersaving_ug_on_start;
	ops->pause = setting_powersaving_ug_on_pause;
	ops->resume = setting_powersaving_ug_on_resume;
	ops->destroy = setting_powersaving_ug_on_destroy;
	ops->message = setting_powersaving_ug_on_message;
	ops->event = setting_powersaving_ug_on_event;
	ops->key_event = setting_powersaving_ug_on_key_event;
	ops->priv = powersavingUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingPowersavingUG *powersavingUG;
	setting_retm_if(!ops, "ops == NULL");

	powersavingUG = ops->priv;
	if (powersavingUG) {
		FREE(powersavingUG);
	}
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_powersaving_ug_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;
	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd;
	if (data_parentItem == ad->data_saving_at) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     10,
						     SETTING_POWERSAVING_AT_10_STR,
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     20,
						     SETTING_POWERSAVING_AT_20_STR,
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     30,
						     SETTING_POWERSAVING_AT_30_STR,
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     40,
						     SETTING_POWERSAVING_AT_40_STR,
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     50,
						     SETTING_POWERSAVING_AT_50_STR,
						     NULL);
		setting_update_chk_status(rgd,data_parentItem->int_slp_setting_binded);
	}
	else if (data_parentItem == ad->data_screen_timeout) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_8SEC,
						     "IDS_ST_BODY_8_SECONDS",
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_15SEC,
						     "IDS_ST_BODY_15SEC", NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_30SEC,
						     "IDS_ST_BODY_30SEC", NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_1MIN,
						     "IDS_COM_BODY_1_MINUTE",
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_3MIN,
						     "IDS_ST_BODY_3MINUTES",
						     NULL);

		setting_create_Gendial_exp_sub_field(scroller,
						     &itc_1icon_1text_sub,
						     NULL, NULL, parentItem,
						     SWALLOW_Type_1RADIO, rgd,
						     SETTING_BACKLIGHT_TIME_10MIN,
						     "IDS_ST_BODY_10_MINUTES",
						     NULL);

		setting_update_chk_status(rgd,data_parentItem->int_slp_setting_binded);
	}
}

char *setting_powersaving_ug_get_brightness_str()
{
	int auto_value = 0;
	int back_value = 10;
	char str_back[MAX_DISPLAY_NAME_LEN_ON_UI + 1] = {0, };
	vconf_get_bool(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_AUTO_STATUS, &auto_value); //if failed, keep intial value
	vconf_get_int(VCONFKEY_SETAPPL_PWRSV_CUSTMODE_BRT_VALUE, &back_value);	//if failed, keep intial value
	if (auto_value)
	{
		snprintf(str_back, MAX_DISPLAY_NAME_LEN_ON_UI, "%s", _("IDS_COM_BODY_AUTOMATIC"));
	}
	else
	{
		snprintf(str_back, MAX_DISPLAY_NAME_LEN_ON_UI, "%d%% %s", back_value, _(KeyStr_Brightness));
	}
	return (char *)g_strdup(str_back);
}


UG_MODULE_API int setting_plugin_reset(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

	int ret = 0;
	ret += excuteCmd(SETTING_POSTINST_FILE, 1, "powersaving");
	return ret;
}
