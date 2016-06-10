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
#include <appfw/app_control_internal.h>
#include <Eina.h>

#include <setting-applications.h>
#include <setting-cfg.h>

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

	setting_view_node_table_register(&setting_view_applications_main, NULL);
	setting_view_node_table_register(&setting_view_applications_defaultapp,
			&setting_view_applications_main);
	return &setting_view_applications_main;
}

/**
 * return view layout
 *
 * viewtype != 'wallpaper' --> ly_main
 */
Evas_Object *__get_applications_layout_to_return(app_control_h service,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *applicationsUG = priv;
	return applicationsUG->ly_main;

}

static void setting_applications_ug_cb_resize(void *data, Evas *e,
		Evas_Object *obj, void *event_info)
{
	SettingApplicationsUG *ad = (SettingApplicationsUG *)data;
	setting_view_update(ad->view_to_load, ad);
}

char *_gl_label_new_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;
	default_app *info = data;
	retv_if(data == NULL, NULL);
	char *label = NULL;
	SETTING_TRACE("part:[%s]", part);

	if (0 == strcmp(part, "elm.text")) {
		label = SAFE_STRDUP(info->pkg_label);
		SETTING_TRACE("##label:[%s]", label);
	}
	return label;
}

char *_get_defualt_icon(pkgmgrinfo_appinfo_h handle)
{
	int ret;
	char *type;
	const char *icon;
	const char *svc_icon = SETTING_ICON_PATH
	"/default_icon_service.png";
	const char *app_icon = SETTING_ICON_PATH
	"/mainmenu.png";

	ret = pkgmgrinfo_appinfo_get_component_type(handle, &type);
	if (PMINFO_R_OK == ret) {
		if (0 == safeStrCmp(type, "svcapp"))
			icon = svc_icon;
		else
			icon = app_icon;
	} else {
		SETTING_TRACE_ERROR(
				"pkgmgrinfo_appinfo_get_component_type() Fail(%d)",
				ret);
		icon = app_icon;
	}

	return strdup(icon);
}
static void _clear_default_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	default_app *info = data;
	int ret = -1;

	if (NULL != info->pkgid){
		ret = app_control_unset_defapp(info->pkgid);
		if (ret != APP_CONTROL_ERROR_NONE) {
			SETTING_TRACE_ERROR("app_control_unset_defapp() Fail(%d)", ret);
		}
	}

	elm_object_item_del(info->item);
}


Evas_Object *_gl_1button1_icon_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, NULL);
	default_app *info = data;

	SETTING_TRACE("part:[%s]", part);

	if (!safeStrCmp(part, "elm.swallow.icon"))
	{
		Evas_Object *icon = NULL;
		icon = elm_icon_add(obj);
		SETTING_TRACE("path:[%s]", info->icon_path);
		if (NULL == info->icon_path){
			char *_icon;
			pkgmgrinfo_appinfo_h handle = NULL;

			int ret = pkgmgrinfo_appinfo_get_appinfo(info->pkgid, &handle);
			warn_if(PMINFO_R_OK != ret, "pkgmgrinfo_appinfo_get_appinfo() Fail(%d)",
					ret);

			_icon = _get_defualt_icon(handle);

			SETTING_TRACE(" ==> appid [%s], icon [%s]", info->pkgid, _icon);

			pkgmgrinfo_appinfo_destroy_appinfo(handle);
		}

		elm_image_file_set(icon, info->icon_path, NULL);
		elm_image_resizable_set(icon, EINA_TRUE, EINA_TRUE);
		evas_object_size_hint_align_set(icon, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_min_set(icon, SETTING_APP_MGR_ICON_SIZE,
				SETTING_APP_MGR_ICON_SIZE);
		evas_object_show(icon);
		SETTING_TRACE_BEGIN;
		return icon;

	} else if(!safeStrCmp(part, "elm.swallow.end")) {
		SETTING_TRACE_BEGIN;
		Evas_Object *button = NULL;

		button = elm_button_add(obj);
		SETTING_TRACE_BEGIN;
		elm_object_text_set(button, Keystr_Clear);
		evas_object_size_hint_align_set(button, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_min_set(button, 160, 60);
		evas_object_size_hint_max_set(button, 160, 60);
		evas_object_show(button);
		evas_object_smart_callback_add(button, "clicked", _clear_default_cb, info);

		return button;
	} else
		return NULL;
}

static void *setting_applications_ug_on_create(ui_gadget_h ug,
		enum ug_mode mode, app_control_h service, void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;

	SettingApplicationsUG *applicationsUG = priv;
	applicationsUG->ug = ug;
	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	applicationsUG->win_main_layout = (Evas_Object *)ug_get_parent_layout(
			ug);
	applicationsUG->win_get = (Evas_Object *)ug_get_window();

	/*evas_object_show(applicationsUG->win_main_layout); */
	applicationsUG->evas = evas_object_evas_get(
			applicationsUG->win_main_layout);

	setting_retvm_if(applicationsUG->win_main_layout == NULL, NULL,
			"cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(applicationsUG->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(applicationsUG->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(applicationsUG->itc_1icon_1text_sub));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(applicationsUG->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(applicationsUG->itc_2text_3));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(applicationsUG->itc_1text_1icon_divider));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(applicationsUG->itc_grp_title));

	applicationsUG->itc_1icon_1button.item_style = SETTING_GENLIST_2LINE_STYLE;
	applicationsUG->itc_1icon_1button.func.text_get = _gl_label_new_get;
	applicationsUG->itc_1icon_1button.func.content_get =
			_gl_1button1_icon_get;
	applicationsUG->itc_1icon_1button.func.state_get = NULL;
	applicationsUG->itc_1icon_1button.func.del = NULL;


	/*	creating a view. */
	applicationsUG->view_to_load = __get_applications_view_to_load(
			applicationsUG, service);
	setting_retvm_if(NULL == applicationsUG->view_to_load, NULL,
			"NULL == applicationsUG->view_to_load");

	setting_view_node_set_cur_view(applicationsUG->view_to_load);
	setting_view_create(applicationsUG->view_to_load,
			(void *)applicationsUG);
	evas_object_event_callback_add(applicationsUG->win_main_layout,
			EVAS_CALLBACK_RESIZE, setting_applications_ug_cb_resize,
			applicationsUG);
	return __get_applications_layout_to_return(service, applicationsUG);
}

static void setting_applications_ug_on_start(ui_gadget_h ug,
		app_control_h service, void *priv)
{
}

static void setting_applications_ug_on_pause(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
}

static void setting_applications_ug_on_resume(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

#if 0
	SettingApplicationsUG *applicationsUG = priv;
	if (applicationsUG->data_home) {
		char *appid = vconf_get_str(
				VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);
		char *sub_desc = setting_application_get_defaultapp_name(appid);
		if (sub_desc)
		applicationsUG->data_home->sub_desc = (char *)strdup(sub_desc);
		elm_object_item_data_set(applicationsUG->data_home->item,
				applicationsUG->data_home);
		elm_genlist_item_update(applicationsUG->data_home->item);
		free(sub_desc);
	}
#endif
}

static void setting_applications_ug_on_destroy(ui_gadget_h ug,
		app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingApplicationsUG *applicationsUG = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(applicationsUG->win_main_layout,
			EVAS_CALLBACK_RESIZE,
			setting_applications_ug_cb_resize);
	applicationsUG->ug = ug;

	FREE(applicationsUG->uri);

	/* called when this shared gadget is terminated. similar with
	 * app_exit */
	if (&setting_view_applications_main == applicationsUG->view_to_load) {
		setting_view_destroy(&setting_view_applications_main,
				applicationsUG);
	} else {
		/* do nothing */
	}

	if (NULL != ug_get_layout(applicationsUG->ug)) {
		evas_object_hide(
				(Evas_Object *)ug_get_layout(
						applicationsUG->ug));
		evas_object_del(
				(Evas_Object *)ug_get_layout(
						applicationsUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_applications_ug_on_message(ui_gadget_h ug,
		app_control_h msg, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_applications_ug_on_event(ui_gadget_h ug,
		enum ug_event event, app_control_h service, void *priv)
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
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_applications_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = (SettingApplicationsUG *)priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
		if (elm_naviframe_top_item_get(ad->navi_bar)
				== elm_naviframe_bottom_item_get(
						ad->navi_bar)) {
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
	SettingApplicationsUG *applicationsUG = calloc(1,
			sizeof(SettingApplicationsUG));
	setting_retvm_if(!applicationsUG, -1,
			"Create SettingApplicationsUG obj failed");

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

#define BUS_NAME			"org.tizen.system.deviced"
#define INTERFACE_NAME			BUS_NAME
#define OBJECT_PATH			"/Org/Tizen/System/DeviceD"
#define DEVICED_PATH_DISPLAY		OBJECT_PATH"/Applications"
#define DEVICED_INTERFACE_DISPLAY	INTERFACE_NAME".applications"
#define DBUS_REPLY_TIMEOUT		(120 * 1000)

static UNUSED int __append_param(DBusMessage *msg, const char *sig,
		char *param[])
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
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_INT32,
					&int_type);
			break;
		case 's':
			str_type = param[i];
			SETTING_TRACE_DEBUG("param[%2d] : %s", i, str_type);
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING,
					&str_type);
			break;
		default:
			break;
		}
	}
	return 0;
}

static UNUSED DBusMessage *__invoke_dbus_method(const char *dest,
		const char *path, const char *interface, const char *method,
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
	reply = dbus_connection_send_with_reply_and_block(conn, msg,
			DBUS_REPLY_TIMEOUT, &err);
	dbus_message_unref(msg);
	if (dbus_error_is_set(&err)) {
		SETTING_TRACE_DEBUG(
				"dbus_connection_send_with_reply_and_block error");
		SETTING_TRACE_DEBUG("error [%s:%s]", err.name, err.message);
		dbus_error_free(&err);
		return NULL;
	}
	return reply;
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

	base = (Evas_Object *)ug_get_layout(ug);
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

