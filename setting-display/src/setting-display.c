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

#include <setting-display.h>

#include <Eina.h>

#include <setting-cfg.h>
#include <dbus/dbus.h>

#define SETTING_BRIGHTNESS_DEFAULT_LEVEL 60

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

/**
 * view selection by service variable
 *
 * viewtype == 'brightness' --> setting_view_display_brightness
 * viewtype != NULL && viewtype --> setting_view_display_main
 */
setting_view *__get_display_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "!data");

	SettingDisplayUG *displayUG = (SettingDisplayUG *)data;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;
	ret = app_control_get_uri(service, &uri);
	if (!ret && uri) {
		SETTING_TRACE("uri:%s", uri);
		if (!safeStrCmp(uri, URI_SMART_STAY)) {
			/*setting_view_node_table_register(&setting_view_display_smart_screen_smart_stay, NULL); */
			FREE(uri);
			return NULL;
		} else if (!safeStrCmp(uri, URI_SMART_ROTATION)) {
			/*setting_view_node_table_register(&setting_view_display_smart_screen_smart_rotation, NULL); */
			FREE(uri);
			return NULL;
		}
	} else {
		app_control_get_extra_data(service, "viewtype", &viewtype);
		if (!viewtype) {
			return NULL;
		}

		SETTING_TRACE_DEBUG("viewtype:%s", viewtype);

		if (!safeStrCmp(viewtype, "brightness")) {
			setting_view_node_table_register(&setting_view_display_brightness, NULL);
			FREE(viewtype);
			return &setting_view_display_brightness;
		} else if (!safeStrCmp(viewtype, "backlight")) {
			setting_view_node_table_register(&setting_view_display_backlight, NULL);
			FREE(viewtype);
			return &setting_view_display_backlight;
#if SUPPORT_SMARTSCREEN
		} else if (!safeStrCmp(viewtype, "smartscreen")) {
			setting_view_node_table_register(&setting_view_display_smart_screen, NULL);
			setting_view_node_table_register(&setting_view_display_smart_screen_help,
			                                 &setting_view_display_smart_screen);
			FREE(viewtype);
			return &setting_view_display_smart_screen;
		} else if (!safeStrCmp(viewtype, "screenmode")) {
			setting_view_node_table_register(&setting_view_display_screen_mode, NULL);
			FREE(viewtype);
			return &setting_view_display_screen_mode;
#endif
		} else {
			if (!safeStrCmp(viewtype, "frontpage_backlight")) {
				displayUG->display_expand_state = DISPLAY_EXPAND_BACKLIGHT;
			} else if (!safeStrCmp(viewtype, "frontpage_duration")) {
				displayUG->display_expand_state = DISPLAY_EXPAND_TOUCH_KEY_LIGHT_DURATION;
			} else {
				displayUG->display_expand_state = DISPLAY_EXPAND_NONE;
			}
			setting_view_node_table_register(&setting_view_display_main, NULL);
			setting_view_node_table_register(&setting_view_display_brightness,
			                                 &setting_view_display_main);
#if SUPPORT_SMARTSCREEN
			setting_view_node_table_register(&setting_view_display_screen_mode,
			                                 &setting_view_display_main);
			setting_view_node_table_register(&setting_view_display_smart_screen,
			                                 &setting_view_display_main);
			setting_view_node_table_register(&setting_view_display_smart_screen_smart_stay,
			                                 &setting_view_display_smart_screen);
			setting_view_node_table_register(&setting_view_display_smart_screen_smart_rotation,
			                                 &setting_view_display_smart_screen);
#endif
			FREE(viewtype);
			return &setting_view_display_main;
		}
	}
	return NULL;
}

/**
 * return view layout
 *
 * viewtype != 'wallpaper' --> ly_main
 */
Evas_Object *__get_display_layout_to_return(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *displayUG = priv;
	char *viewtype = NULL;
	char *uri = NULL;
	int ret = 0;
	ret = app_control_get_uri(service, &uri);
	if (!ret && uri) {
		return displayUG->ly_main;
	}
	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype) {
		return NULL;
	}

	SETTING_TRACE("viewtype:%s", viewtype);

	FREE(viewtype);
	return displayUG->ly_main;

}


static void setting_display_ug_cb_resize(void *data, Evas *e,
                                         Evas_Object *obj, void *event_info)
{
	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	setting_view_update(ad->view_to_load, ad);
}
/*help smart scren popup*/
Eina_Bool __show_smartstay_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Display] Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	setting_retvm_if(ad->smart_stay_sublayout == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Display] Dad->smart_stay_sublayout parameter is NULL");

	/*hide top popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout, "elm,state,top,hide", "elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_stay_sublayout, "elm,state,center,show", "elm");
	return ECORE_CALLBACK_CANCEL;
}
Eina_Bool __show_smartrotation_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Display] Data parameter is NULL");

	SettingDisplayUG *ad = (SettingDisplayUG *) data;
	setting_retvm_if(ad->smart_rotation_sublayout == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Display] ad->smart_rotation_sublayout parameter is NULL");
	/*hide top popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout, "elm,state,top,hide", "elm");
	/*show center popup*/
	elm_object_signal_emit(ad->smart_rotation_sublayout, "elm,state,center,show", "elm");
	return ECORE_CALLBACK_CANCEL;
}

static void
__help_popup_on_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	int response_type = btn_type(obj);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	/* prevent CID35750 (by sunyeop.hwang@20131109) */
	/*const char *btnstr = elm_entry_markup_to_utf8(elm_object_text_get(obj)); */

	if (POPUP_RESPONSE_OK == response_type) {	/*need set vconf */
#if SUPPORT_SMARTSCREEN
		SETTING_TRACE("need to set vconf");
		int ret = 0;
		if (!safeStrCmp(ad->uri, URI_SMART_STAY)) {
			ret += vconf_set_int(VCONFKEY_SETAPPL_SMARTSCREEN_SMARTSTAY_STATUS, 1);
			SETTING_TRACE("ret:%d", ret);
			if (ad->smart_stay_sublayout) {
				elm_object_signal_emit(ad->smart_stay_sublayout, "elm,state,top,show", "elm");
				ad->timer_show_guide = ecore_timer_add(3, (Ecore_Task_Cb)__show_smartstay_guide_popup, ad);
			}
		} else if (!safeStrCmp(ad->uri, URI_SMART_ROTATION)) {
			ret += vconf_set_bool(VCONFKEY_SETAPPL_SMARTSCREEN_SMART_ROTATION, TRUE);
			if (ad->smart_rotation_sublayout) {
				elm_object_signal_emit(ad->smart_rotation_sublayout, "elm,state,top,show", "elm");
				ad->timer_show_guide = ecore_timer_add(3, (Ecore_Task_Cb)__show_smartrotation_guide_popup, ad);
			}
		}
#endif
		if (ad->help_popup) {
			evas_object_del(ad->help_popup);
			ad->help_popup = NULL;
		}
	} else if (POPUP_RESPONSE_CANCEL == response_type) {	/*need rollback */
		evas_object_hide(ad->help_popup);
		SETTING_TRACE("ad->ug:%p", ad->ug);
		ug_destroy_me(ad->ug);
	}
}

static Eina_Bool __help_key_press_cb(void *data, int type, void *event)
{
	SETTING_TRACE_BEGIN;
	Evas_Event_Key_Down *ev = event;
	if (!ev || !data) {
		SETTING_TRACE("Invalid event object");
		return ECORE_CALLBACK_RENEW;
	}
	SETTING_TRACE("Pressed %s", ev->keyname);
	SettingDisplayUG *ad = (SettingDisplayUG *) data;

	#if 0
	if (!strcmp(ev->keyname, KEY_BACK)) {
		/*evas_object_hide(obj); // you can call evas_object_del(obj); to remove popup if you want */
		if (ad->help_popup) {
			popup_handle_hardkey(ad, ad->help_popup);
		}
	}
	#endif

	return ECORE_CALLBACK_RENEW;
}

static void *___help_popup_view(ui_gadget_h ug,
                                enum ug_mode mode, app_control_h service,
                                void *priv)

{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *ad = priv;
	int sub_value = 0;

	char buf[MAX_COMMON_BUFFER_LEN] = {0,};

	int ret = app_control_get_uri(service, &ad->uri);
	if (!ret && ad->uri) {
#if SUPPORT_SMARTSCREEN
		if (!safeStrCmp(ad->uri, URI_SMART_STAY)) {
			vconf_get_int(VCONFKEY_SETAPPL_SMARTSCREEN_SMARTSTAY_STATUS, &sub_value);
			snprintf(buf, sizeof(buf) , "%s", SETTING_DISPLAY_SMART_SCREEN_TURN_ON_SMART_STAY_POPUP);
		} else if (!safeStrCmp(ad->uri, URI_SMART_ROTATION)) {
			vconf_get_bool(VCONFKEY_SETAPPL_SMARTSCREEN_SMART_ROTATION, &sub_value);
			snprintf(buf, sizeof(buf) , "%s", SETTING_DISPLAY_SMART_SCREEN_TURN_ON_SMART_ROATION_POPUP);
		}
#endif

		ad->help_event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, __help_key_press_cb, ad);

		if (!sub_value) {
			SETTING_TRACE("uri:%s", ad->uri);
			if (ad->help_popup) {
				evas_object_del(ad->help_popup);
				ad->help_popup = NULL;
			}

			ad->help_popup =
			    setting_create_popup_with_btn(ad, ad->win_main_layout,
			                                  NULL, buf,
			                                  __help_popup_on_resp_cb, 0,
			                                  2, POPUP_TURN_ON_STR, "IDS_ST_BUTTON_CANCEL_ABB");
			/*evas_object_data_set(ad->popup, "text0", MOTION_DISABLED_DSC); */
			/*evas_object_data_set(ad->popup, "text", USE_MOTION_TO_TRY_TUTORIAL); */

			return ad->help_popup;
		} else {

		}
	}

	return NULL;
}

static void *setting_display_ug_on_create(ui_gadget_h ug,
                                          enum ug_mode mode, app_control_h service,
                                          void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;

	SettingDisplayUG *displayUG = priv;
	displayUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	displayUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	displayUG->win_get = (Evas_Object *) ug_get_window();

	/*evas_object_show(displayUG->win_main_layout); */
	displayUG->evas = evas_object_evas_get(displayUG->win_main_layout);

	setting_retvm_if(displayUG->win_main_layout == NULL, NULL,
	                 "cannot get main window ");

	(void)___help_popup_view(ug, mode, service, priv);
	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc("1line.top", &(displayUG->itc_1text));
	setting_create_Gendial_itc("1line", &(displayUG->itc_1text_1icon));

	setting_create_Gendial_itc("1line", &(displayUG->itc_1icon_1text_sub));

	setting_create_Gendial_itc("1line", &(displayUG->itc_1text));
	setting_create_Gendial_itc("1line", &(displayUG->itc_2text_3));
	setting_create_Gendial_itc("1line", &(displayUG->itc_1text_1icon_divider));

	/*  creating a view. */
	displayUG->view_to_load = __get_display_view_to_load(displayUG, service);
	setting_retvm_if(NULL == displayUG->view_to_load, NULL,
	                 "NULL == displayUG->view_to_load");
	setting_view_node_set_cur_view(displayUG->view_to_load);
	setting_view_create(displayUG->view_to_load, (void *)displayUG);
	evas_object_event_callback_add(displayUG->win_main_layout,
	                               EVAS_CALLBACK_RESIZE,
	                               setting_display_ug_cb_resize, displayUG);
	return __get_display_layout_to_return(service, displayUG);
}

static void setting_display_ug_on_start(ui_gadget_h ug, app_control_h service,
                                        void *priv)
{
}

static void setting_display_ug_on_pause(ui_gadget_h ug, app_control_h service,
                                        void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingDisplayUG *displayUG = priv;
}

static void setting_display_ug_on_resume(ui_gadget_h ug, app_control_h service,
                                         void *priv)
{
}

static void setting_display_ug_on_destroy(ui_gadget_h ug, app_control_h service,
                                          void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingDisplayUG *displayUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(displayUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_display_ug_cb_resize);
	displayUG->ug = ug;

	FREE(displayUG->uri);
	if (displayUG->help_popup) {
		evas_object_del(displayUG->help_popup);
		displayUG->help_popup = NULL;
	}

	/*  called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_display_main == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, displayUG);
#if SUPPORT_SMARTSCREEN
		setting_view_destroy(&setting_view_display_smart_screen_smart_rotation, displayUG);
		setting_view_destroy(&setting_view_display_smart_screen_smart_stay, displayUG);
		setting_view_destroy(&setting_view_display_screen_mode, displayUG);
		setting_view_destroy(&setting_view_display_smart_screen, displayUG);
#endif
		setting_view_destroy(&setting_view_display_main, displayUG);
	} else if (&setting_view_display_brightness == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_brightness, displayUG);
	} else if (&setting_view_display_backlight == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_backlight, displayUG);
#if SUPPORT_SMARTSCREEN
	} else if (&setting_view_display_screen_mode == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_screen_mode, displayUG);
	} else if (&setting_view_display_smart_screen == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_smart_screen_smart_rotation, displayUG);
		setting_view_destroy(&setting_view_display_smart_screen_smart_stay, displayUG);
		setting_view_destroy(&setting_view_display_smart_screen, displayUG);
	} else if (&setting_view_display_smart_screen_smart_stay == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_smart_screen_smart_stay, displayUG);
	} else if (&setting_view_display_smart_screen_smart_rotation == displayUG->view_to_load) {
		setting_view_destroy(&setting_view_display_smart_screen_smart_rotation, displayUG);
#endif
	}

	if (NULL != ug_get_layout(displayUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(displayUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(displayUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_display_ug_on_message(ui_gadget_h ug, app_control_h msg,
                                          app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_display_ug_on_event(ui_gadget_h ug,
                                        enum ug_event event, app_control_h service,
                                        void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *ad = priv;
	setting_retm_if(NULL == ad, "ad is NULL");
	switch (event) {
		case UG_EVENT_LOW_MEMORY:
			break;
		case UG_EVENT_LOW_BATTERY:
			break;
		case UG_EVENT_LANG_CHANGE:
			setting_navi_items_update(ad->navi_bar);
			break;
		case UG_EVENT_ROTATE_PORTRAIT:
		case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN: {

				int h = 0;
				int w = 0;
#if 0
				app_device_orientation_e m = elm_win_rotation_get(ad->win_get);

				if (APP_DEVICE_ORIENTATION_90 == m || APP_DEVICE_ORIENTATION_270 == m) {
					ecore_x_window_size_get(ecore_x_window_root_first_get(), &h, &w);

				} else {
					ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
					h = h - 60;
				}
				SETTING_TRACE("m:%d", m);
#endif
				h = SAMRT_SCREEN_TRY_IT_IAMGE_PORTRAIT_H;
				w = SAMRT_SCREEN_TRY_IT_IAMGE_PORTRAIT_W;
				if (ad->smart_rotation_image) {
					elm_win_indicator_opacity_set(ad->win_get, ELM_WIN_INDICATOR_TRANSPARENT);
					/*elm_win_indicator_mode_set(ad->win_get,ELM_WIN_INDICATOR_HIDE); */
					SETTING_TRACE("indicator is in TRANSPARENT mode");
					evas_object_image_file_set(ad->smart_rotation_image, SMART_SCREEN_TRY_IT_IMAGE_P, NULL);
					evas_object_size_hint_weight_set(ad->smart_rotation_image, 0.0, EVAS_HINT_EXPAND);
					evas_object_image_fill_set(ad->smart_rotation_image, 0, 0, w, h);
					evas_object_image_border_set(ad->smart_rotation_image, 16, 16, 16, 16); /*for round corner */
					evas_object_image_filled_set(ad->smart_rotation_image, 1); /*to full fill */
					evas_object_size_hint_min_set(ad->smart_rotation_image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
					evas_object_size_hint_max_set(ad->smart_rotation_image, w, h);
				}
				if (ad->smart_stay_image) {
					SETTING_TRACE("indicator is in TRANSPARENT mode");
					elm_win_indicator_opacity_set(ad->win_get, ELM_WIN_INDICATOR_TRANSPARENT);
					/*elm_win_indicator_mode_set(ad->win_get,ELM_WIN_INDICATOR_HIDE); */
					evas_object_image_file_set(ad->smart_stay_image, SMART_SCREEN_TRY_IT_IMAGE_P, NULL);
					evas_object_size_hint_weight_set(ad->smart_stay_image, 0.0, EVAS_HINT_EXPAND);
					evas_object_image_fill_set(ad->smart_stay_image, 0, 0, w, h);
					evas_object_image_border_set(ad->smart_stay_image, 16, 16, 16, 16); /*for round corner */
					evas_object_image_filled_set(ad->smart_stay_image, 1); /*to full fill */
					evas_object_size_hint_min_set(ad->smart_stay_image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
					evas_object_size_hint_max_set(ad->smart_stay_image, w, h);
				}
			}
			break;
		case UG_EVENT_ROTATE_LANDSCAPE:
		case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN: {
				int h = 0;
				int w = 0;
#if 0
				app_device_orientation_e m = elm_win_rotation_get(ad->win_get);

				if (APP_DEVICE_ORIENTATION_90 == m || APP_DEVICE_ORIENTATION_270 == m) {
					ecore_x_window_size_get(ecore_x_window_root_first_get(), &h, &w);

				} else {
					ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);

				}
				SETTING_TRACE("m:%d", m);
#endif
				h = SAMRT_SCREEN_TRY_IT_IAMGE_LANDSCAPE_H;
				w = SAMRT_SCREEN_TRY_IT_IAMGE_LANDSCAPE_W;
				if (ad->smart_rotation_image) {
					/*elm_win_indicator_opacity_set(win, ELM_WIN_INDICATOR_OPAQUE) */
					evas_object_image_file_set(ad->smart_rotation_image, SMART_SCREEN_TRY_IT_IMAGE_L, NULL);
					evas_object_size_hint_weight_set(ad->smart_rotation_image, 0.0, EVAS_HINT_EXPAND);
					evas_object_image_fill_set(ad->smart_rotation_image, 0, 0, w, h);
					evas_object_image_border_set(ad->smart_rotation_image, 16, 16, 16, 16); /*for round corner */
					evas_object_image_filled_set(ad->smart_rotation_image, 1); /*to full fill */
					evas_object_size_hint_min_set(ad->smart_rotation_image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
					evas_object_size_hint_max_set(ad->smart_rotation_image, w, h);
				}
				if (ad->smart_stay_image) {
					evas_object_image_file_set(ad->smart_stay_image, SMART_SCREEN_TRY_IT_IMAGE_L, NULL);
					evas_object_size_hint_weight_set(ad->smart_stay_image, 0.0, EVAS_HINT_EXPAND);
					evas_object_image_fill_set(ad->smart_stay_image, 0, 0, w, h);
					evas_object_image_border_set(ad->smart_stay_image, 16, 16, 16, 16); /*for round corner */
					evas_object_image_filled_set(ad->smart_stay_image, 1); /*to full fill */
					evas_object_size_hint_min_set(ad->smart_stay_image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
					evas_object_size_hint_max_set(ad->smart_stay_image, w, h);
				}

			}
			break;
		case UG_EVENT_REGION_CHANGE:
			break;
		default:
			break;
	}
}

static void setting_display_ug_on_key_event(ui_gadget_h ug,
                                            enum ug_key_event event,
                                            app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *ad = (SettingDisplayUG *) priv;

	switch (event) {
		case UG_KEY_EVENT_END: {
				if (elm_naviframe_top_item_get(ad->navi_bar) ==
				    elm_naviframe_bottom_item_get(ad->navi_bar)) {
					ug_destroy_me(ug);
				} else {
					if (&setting_view_display_brightness ==
					    setting_view_node_get_cur_view()) {
					}
					setting_view_cb_at_endKey(ad);
				}
			}
			break;
		default:
			break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingDisplayUG *displayUG = calloc(1, sizeof(SettingDisplayUG));
	setting_retvm_if(!displayUG, -1, "Create SettingDisplayUG obj failed");

	memset(displayUG, 0x00, sizeof(SettingDisplayUG));

	ops->create = setting_display_ug_on_create;
	ops->start = setting_display_ug_on_start;
	ops->pause = setting_display_ug_on_pause;
	ops->resume = setting_display_ug_on_resume;
	ops->destroy = setting_display_ug_on_destroy;
	ops->message = setting_display_ug_on_message;
	ops->event = setting_display_ug_on_event;
	ops->key_event = setting_display_ug_on_key_event;
	ops->priv = displayUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingDisplayUG *displayUG;
	setting_retm_if(!ops, "ops == NULL");

	displayUG = ops->priv;
	if (displayUG) {
		FREE(displayUG);
	}
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

#define BUS_NAME       				"org.tizen.system.deviced"
#define INTERFACE_NAME 				BUS_NAME
#define OBJECT_PATH    				"/Org/Tizen/System/DeviceD"
#define DEVICED_PATH_DISPLAY        OBJECT_PATH"/Display"
#define DEVICED_INTERFACE_DISPLAY   INTERFACE_NAME".display"
#define DBUS_REPLY_TIMEOUT 			(120 * 1000)

static int __append_param(DBusMessage *msg, const char *sig, char *param[])
{
	DBusMessageIter iter;
	/*DBusMessageIter cont_iter; */
	char *ch;
	char *str_type;
	int int_type;
	int i;
	if (sig == NULL || param == NULL) {
		return 0;
	}

	dbus_message_iter_init_append(msg, &iter);
	for (ch = (char *)sig, i = 0; *ch != '\0'; ++i, ++ch) {
		SETTING_TRACE_DEBUG("sig : %c", *ch);
		switch (*ch) {
			case 'i':
				int_type = atoi(param[i]);
				SETTING_TRACE_DEBUG("param[%2d] : %d", i, int_type);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32, &int_type);
				break;
			case 's':
				str_type = param[i];
				SETTING_TRACE_DEBUG("param[%2d] : %s", i, str_type);
				dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &str_type);
				break;
			default:
				break;
		}
	}
	return 0;
}

static DBusMessage *__invoke_dbus_method(const char *dest, const char *path,
                                         const char *interface, const char *method,
                                         const char *sig, char *param[])
{
	DBusError err;
	DBusConnection *conn;
	DBusMessage *reply;
	DBusMessage *msg;
	int r;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	if (conn == NULL) {
		SETTING_TRACE_DEBUG("dbus_bus_get error");
		return NULL;
	}
	msg = dbus_message_new_method_call(dest, path, interface, method);
	if (msg == NULL) {
		SETTING_TRACE_DEBUG("dbus_message_new_method_call error");
		return NULL;
	}
	r = __append_param(msg, sig, param);
	if (r < 0) {
		SETTING_TRACE_DEBUG("append_param error");
		return NULL;
	}
	dbus_error_init(&err);
	reply = dbus_connection_send_with_reply_and_block(conn, msg, DBUS_REPLY_TIMEOUT, &err);
	dbus_message_unref(msg);
	if (dbus_error_is_set(&err)) {
		SETTING_TRACE_DEBUG("dbus_connection_send_with_reply_and_block error");
		SETTING_TRACE_DEBUG("error [%s:%s]", err.name, err.message);
		dbus_error_free(&err);
		return NULL;
	}
	return reply;
}

int setting_display_set_auto_adjust_tone(int value)
{
	SETTING_TRACE_BEGIN;
	if (value < 0 || value > 1) {
		SETTING_TRACE_DEBUG("value has invalid range. [0 or 1]");
		return SETTING_RETURN_FAIL;
	}

	int ret = 0;
	int state;
	DBusMessage *msg;
	DBusError err;
	char *arr[4] = {0,};
	char temp[2] = {0,};

	e_dbus_init();
	dbus_error_init(&err);

	snprintf(temp, 2, "%1d", value);
	temp[1] = '\0';
	SETTING_TRACE_DEBUG("set as [%s]", temp);
	arr[0] = temp;
	msg = __invoke_dbus_method(BUS_NAME, DEVICED_PATH_DISPLAY, DEVICED_INTERFACE_DISPLAY, "setautotone", "i", arr);
	if (msg) {
		if (!dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &state, DBUS_TYPE_INVALID)) {
			//SETTING_TRACE_DEBUG("no message : [%s:%s]", err.name, err.message);
			ret = SETTING_RETURN_FAIL;
		} else {
			SETTING_TRACE_DEBUG("%s-%s : %d", DEVICED_INTERFACE_DISPLAY, "setautotone", state);
		}
		dbus_message_unref(msg);
	}
	e_dbus_shutdown();

	return ret;
}

void setting_display_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
                                  void *priv)
{
	/*SettingDisplayUG *ad = (SettingDisplayUG *) priv; */
	Evas_Object *base;

	if (!priv) {
		return;
	}
	SETTING_TRACE_BEGIN;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base) {
		return;
	}

	switch (mode) {
		case UG_MODE_FULLVIEW:
			evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
			                                 EVAS_HINT_EXPAND);
			evas_object_show(base);
			break;
		default:
			break;
	}

	SETTING_TRACE_END;
}

/*  NESTED*/
static int setting_reset_display_brightness(void)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
	return ret;
}

/*  NESTED*/
static int setting_reset_display_backlight(void)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
#if SUPPORT_LCD_TIMEOUT_KEEPING
	ret += vconf_set_int(VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, 0);
#else
	if (isEmulBin()) {
		ret += vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, 0);
	} else {
		ret += vconf_set_int(VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, 30);
	}
#endif
	SETTING_TRACE_END;
	return ret;
}

/*  NESTED*/
/** @todo setting_display_main_effect_set_value work? */
static int setting_reset_display_main(void)
{
	SETTING_TRACE_BEGIN;
	int ret = 0;
#if 0
	ret += setting_reset_display_backlight();
#if !SUPPOR_SEPARATE_BRIGHTNESS
	ret += setting_reset_display_brightness();
#endif

	ret += vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, TRUE);
#if SUPPORT_SMARTSCREEN
	/* smart screen */
	ret += vconf_set_int(VCONFKEY_SETAPPL_SMARTSCREEN_SMARTSTAY_STATUS, 0);
	ret += vconf_set_bool(VCONFKEY_SETAPPL_SMARTSCREEN_SMART_ROTATION, FALSE);
#endif
	/* screen mode */
	ret += vconf_set_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME, "Dynamic");
	display_set_image_enhance(ENHANCE_MODE, MODE_DYNAMIC);	/* 0 => Dynamic */

	/*touch key light duration */
	ret += vconf_set_int(VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, 90);
#endif
	return ret;
}

#define SUBINDEX_NAME "Display"

#if 1
static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_ST_BODY_BRIGHTNESS_M_POWER_SAVING", IMG_Display, "viewtype:brightness",  Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_BODY_BACKLIGHT_TIME", IMG_Display, "viewtype:frontpage_backlight", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_HEADER_TOUCH_KEY_LIGHT_DURATION", IMG_Display, "viewtype:frontpage_duration", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_MBODY_AUTO_ADJUST_SCREEN_TONE", IMG_Display, "viewtype:frontpage", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_HEADER_AUTO_ROTATE_SCREEN_ABB", IMG_Display, "viewtype:frontpage", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_BODY_DYNAMIC_STATUS_BAR", IMG_Display, "viewtype:frontpage", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_MBODY_EDIT_AFTER_SCREEN_CAPTURE", IMG_Display, "viewtype:frontpage", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC", IMG_Display, "viewtype:brightness", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_MBODY_SMART_STAY", IMG_SmartScreen, "viewtype:smartscreen", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
	{"IDS_ST_MBODY_SMART_ROTATION", IMG_SmartScreen, "viewtype:smartscreen", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node_Toggle, NULL, SUBINDEX_NAME, NULL, NULL},
};
#endif

UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-display-efl DB search code");
	setting_retvm_if(!priv || !applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "pplist/applocale is NULL");

	*applocale = strdup("setting:/usr/apps/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, s_cfg_node_array[i].icon_path, s_cfg_node_array[i].item_type, s_cfg_node_array[i].data, s_cfg_node_array[i].sub_index);
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

EXPORT_PUBLIC
int get_display_ug_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;


	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, &value);

	if (value == 1) {
		*stat = Cfg_Item_On;
	} else {
		*stat = Cfg_Item_Off;
	}
	return ret;
}

/**************************************************************
 toggle state get/set function for "auto rotate screen"
************************************************************/
EXPORT_PUBLIC
int set_display_ug_state(Cfg_Item_State stat, void *item, void *data)
{
	SETTING_TRACE_BEGIN;
	int value = -1;

	if (stat == Cfg_Item_On) {
		value = 1;
	} else if (stat == Cfg_Item_Off) {
		value = 0;
	} else {
		value = 0;
	}
	int ret = vconf_set_bool(VCONFKEY_SETAPPL_AUTO_ROTATE_SCREEN_BOOL, value);
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_update_ui(Cfg_Item_State stat, void *data)
{
	SETTING_TRACE_BEGIN;

	return 0;
}

/**************************************************************
 toggle state get/set function for "auto adjust screen tone"
************************************************************/
EXPORT_PUBLIC
int get_display_ug_auto_adjust_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;

	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT, &value);

	if (value == 1) {
		*stat = Cfg_Item_On;
	} else {
		*stat = Cfg_Item_Off;
	}
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_auto_adjust_state(Cfg_Item_State stat, void *item, void *data)
{
	SETTING_TRACE_BEGIN;
	int value = -1;

	if (stat == Cfg_Item_On) {
		value = 1;
	} else if (stat == Cfg_Item_Off) {
		value = 0;
	} else {
		value = 0;
	}
	int ret = vconf_set_bool(VCONFKEY_SETAPPL_LCD_AUTO_DISPLAY_ADJUSTMENT, value);
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_auto_adjust_update_ui(Cfg_Item_State stat, void *data)
{
	SETTING_TRACE_BEGIN;

	return 0;
}

/**************************************************************
 toggle state get/set function for "dynamic status bar"
************************************************************/
EXPORT_PUBLIC
int get_display_ug_dynamic_status_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;

	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, &value);

	if (value == 1) {
		*stat = Cfg_Item_On;
	} else {
		*stat = Cfg_Item_Off;
	}
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_dynamic_status_state(Cfg_Item_State stat, void *item, void *data)
{
	SETTING_TRACE_BEGIN;
	int value = -1;

	if (stat == Cfg_Item_On) {
		value = 1;
	} else if (stat == Cfg_Item_Off) {
		value = 0;
	} else {
		value = 0;
	}
	int ret = vconf_set_bool(VCONFKEY_SETAPPL_DYNAMIC_STATUS_BAR, value);
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_dynamic_status_update_ui(Cfg_Item_State stat, void *data)
{
	SETTING_TRACE_BEGIN;

	return 0;
}

/**************************************************************
 toggle state get/set function for "edit after screen capture"
************************************************************/
EXPORT_PUBLIC
int get_display_ug_edit_screen_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;

	int value = -1;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE, &value);

	if (value == 1) {
		*stat = Cfg_Item_On;
	} else {
		*stat = Cfg_Item_Off;
	}
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_edit_screen_state(Cfg_Item_State stat, void *item, void *data)
{
	SETTING_TRACE_BEGIN;
	int value = -1;

	if (stat == Cfg_Item_On) {
		value = 1;
	} else if (stat == Cfg_Item_Off) {
		value = 0;
	} else {
		value = 0;
	}
	int ret = vconf_set_bool(VCONFKEY_SETAPPL_SCREEN_CAPTURE_EDIT_AFTER_CAPTURE, value);
	return ret;
}

EXPORT_PUBLIC
int set_display_ug_edit_screen_update_ui(Cfg_Item_State stat, void *data)
{
	SETTING_TRACE_BEGIN;

	return 0;
}

EXPORT_PUBLIC
cfg_func_table opt_tab_rotation = {
	.get_item_state = get_display_ug_state,
	.set_item_state = set_display_ug_state,
	.set_item_update_ui = set_display_ug_update_ui,
};

EXPORT_PUBLIC
cfg_func_table opt_tab_auto_adjust = {
	.get_item_state = get_display_ug_auto_adjust_state,
	.set_item_state = set_display_ug_auto_adjust_state,
	.set_item_update_ui = set_display_ug_auto_adjust_update_ui,
};

EXPORT_PUBLIC
cfg_func_table opt_tab_dynamic_status = {
	.get_item_state = get_display_ug_dynamic_status_state,
	.set_item_state = set_display_ug_dynamic_status_state,
	.set_item_update_ui = set_display_ug_dynamic_status_update_ui,
};
EXPORT_PUBLIC
cfg_func_table opt_tab_edit_screen = {
	.get_item_state = get_display_ug_edit_screen_state,
	.set_item_state = set_display_ug_edit_screen_state,
	.set_item_update_ui = set_display_ug_edit_screen_update_ui,
};

/* cfg_func_table* tfunc; */
UG_MODULE_API int setting_plugin_search_query_ops(char *str_id, void **tfunc_obj)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> get tfunc operation via plugin-model 1");
	if (str_id && !safeStrCmp(str_id, _("IDS_ST_HEADER_AUTO_ROTATE_SCREEN_ABB"))) {
		*tfunc_obj = (void *)&opt_tab_rotation;
	} else if (str_id && !safeStrCmp(str_id, _("IDS_ST_MBODY_AUTO_ADJUST_SCREEN_TONE"))) {
		*tfunc_obj = (void *)&opt_tab_auto_adjust;
	} else if (str_id && !safeStrCmp(str_id, _("IDS_ST_BODY_DYNAMIC_STATUS_BAR"))) {
		*tfunc_obj = (void *)&opt_tab_dynamic_status;
	}
	if (str_id && !safeStrCmp(str_id, _("IDS_ST_MBODY_EDIT_AFTER_SCREEN_CAPTURE"))) {
		*tfunc_obj = (void *)&opt_tab_edit_screen;
	}
	SETTING_TRACE(">> get tfunc operation via plugin-model 2");
	return 0;
}

/*****/
