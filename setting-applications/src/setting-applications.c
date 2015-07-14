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

#include <setting-applications.h>

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
 * viewtype == 'brightness' --> setting_view_applications_brightness
 * viewtype != NULL && viewtype --> setting_view_applications_main
 */
setting_view *__get_applications_view_to_load(void *data, app_control_h service)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "!data");

	SettingApplicationsUG *applicationsUG = (SettingApplicationsUG *)data;
	char *viewtype = NULL;
	char *uri = NULL;

	setting_view_node_table_register(&setting_view_applications_main, NULL);
	setting_view_node_table_register(&setting_view_applications_defaultapp, &setting_view_applications_main);
	return &setting_view_applications_main;
}

/**
 * return view layout
 *
 * viewtype != 'wallpaper' --> ly_main
 */
Evas_Object *__get_applications_layout_to_return(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *applicationsUG = priv;
	return applicationsUG->ly_main;

}


static void setting_applications_ug_cb_resize(void *data, Evas *e,
                                              Evas_Object *obj, void *event_info)
{
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	setting_view_update(ad->view_to_load, ad);
}
/*help smart scren popup*/
Eina_Bool __show_smartstay_guide_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retvm_if(data == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Applications] Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	setting_retvm_if(ad->smart_stay_sublayout == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Applications] Dad->smart_stay_sublayout parameter is NULL");

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
	                 "[Setting > Applications] Data parameter is NULL");

	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;
	setting_retvm_if(ad->smart_rotation_sublayout == NULL, ECORE_CALLBACK_CANCEL,
	                 "[Setting > Applications] ad->smart_rotation_sublayout parameter is NULL");
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
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

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
	SettingApplicationsUG *ad = (SettingApplicationsUG *) data;

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
	SettingApplicationsUG *ad = priv;
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

static void *setting_applications_ug_on_create(ui_gadget_h ug,
                                               enum ug_mode mode, app_control_h service,
                                               void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;

	SettingApplicationsUG *applicationsUG = priv;
	applicationsUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	applicationsUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	applicationsUG->win_get = (Evas_Object *) ug_get_window();

	/*evas_object_show(applicationsUG->win_main_layout); */
	applicationsUG->evas = evas_object_evas_get(applicationsUG->win_main_layout);

	setting_retvm_if(applicationsUG->win_main_layout == NULL, NULL,
	                 "cannot get main window ");

	(void)___help_popup_view(ug, mode, service, priv);
	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc("2line.top", &(applicationsUG->itc_2text_2));
	setting_create_Gendial_itc("1line", &(applicationsUG->itc_1text_1icon));

	setting_create_Gendial_itc("1line", &(applicationsUG->itc_1icon_1text_sub));

	setting_create_Gendial_itc("1line", &(applicationsUG->itc_1text));
	setting_create_Gendial_itc("1line", &(applicationsUG->itc_2text_3));
	setting_create_Gendial_itc("1line", &(applicationsUG->itc_1text_1icon_divider));

	/*  creating a view. */
	applicationsUG->view_to_load = __get_applications_view_to_load(applicationsUG, service);
	setting_retvm_if(NULL == applicationsUG->view_to_load, NULL, "NULL == applicationsUG->view_to_load");

	setting_view_node_set_cur_view(applicationsUG->view_to_load);
	setting_view_create(applicationsUG->view_to_load, (void *)applicationsUG);
	evas_object_event_callback_add(applicationsUG->win_main_layout,
	                               EVAS_CALLBACK_RESIZE,
	                               setting_applications_ug_cb_resize, applicationsUG);
	return __get_applications_layout_to_return(service, applicationsUG);
}

static void setting_applications_ug_on_start(ui_gadget_h ug, app_control_h service,
                                             void *priv)
{
}

static void setting_applications_ug_on_pause(ui_gadget_h ug, app_control_h service,
                                             void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingApplicationsUG *applicationsUG = priv;
}

static void setting_applications_ug_on_resume(ui_gadget_h ug, app_control_h service,
                                              void *priv)
{
}

static void setting_applications_ug_on_destroy(ui_gadget_h ug, app_control_h service,
                                               void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingApplicationsUG *applicationsUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(applicationsUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_applications_ug_cb_resize);
	applicationsUG->ug = ug;

	FREE(applicationsUG->uri);
	if (applicationsUG->help_popup) {
		evas_object_del(applicationsUG->help_popup);
		applicationsUG->help_popup = NULL;
	}

	/*  called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_applications_main == applicationsUG->view_to_load) {
		setting_view_destroy(&setting_view_applications_main, applicationsUG);
	} else {
		/* do nothing */
	}

	if (NULL != ug_get_layout(applicationsUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(applicationsUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(applicationsUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_applications_ug_on_message(ui_gadget_h ug, app_control_h msg,
                                               app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_applications_ug_on_event(ui_gadget_h ug,
                                             enum ug_event event, app_control_h service,
                                             void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = priv;
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

static void setting_applications_ug_on_key_event(ui_gadget_h ug,
                                                 enum ug_key_event event,
                                                 app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = (SettingApplicationsUG *) priv;

	switch (event) {
		case UG_KEY_EVENT_END: {
				if (elm_naviframe_top_item_get(ad->navi_bar) ==
				    elm_naviframe_bottom_item_get(ad->navi_bar)) {
					ug_destroy_me(ug);
				} else {
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
	SettingApplicationsUG *applicationsUG = calloc(1, sizeof(SettingApplicationsUG));
	setting_retvm_if(!applicationsUG, -1, "Create SettingApplicationsUG obj failed");

	memset(applicationsUG, 0x00, sizeof(SettingApplicationsUG));

	ops->create = setting_applications_ug_on_create;
	ops->start = setting_applications_ug_on_start;
	ops->pause = setting_applications_ug_on_pause;
	ops->resume = setting_applications_ug_on_resume;
	ops->destroy = setting_applications_ug_on_destroy;
	ops->message = setting_applications_ug_on_message;
	ops->event = setting_applications_ug_on_event;
	ops->key_event = setting_applications_ug_on_key_event;
	ops->priv = applicationsUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingApplicationsUG *applicationsUG;
	setting_retm_if(!ops, "ops == NULL");

	applicationsUG = ops->priv;
	if (applicationsUG) {
		FREE(applicationsUG);
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
#define DEVICED_PATH_DISPLAY        OBJECT_PATH"/Applications"
#define DEVICED_INTERFACE_DISPLAY   INTERFACE_NAME".applications"
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

int setting_applications_set_auto_adjust_tone(int value)
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
			SETTING_TRACE_DEBUG("no message : [%s:%s]", err.name, err.message);
			ret = SETTING_RETURN_FAIL;
		} else {
			SETTING_TRACE_DEBUG("%s-%s : %d", DEVICED_INTERFACE_DISPLAY, "setautotone", state);
		}
		dbus_message_unref(msg);
	}
	e_dbus_shutdown();

	return ret;
}

void setting_applications_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
                                       void *priv)
{
	/*SettingApplicationsUG *ad = (SettingApplicationsUG *) priv; */
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

