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

#define SETTING_APPLICATIONS_PACKAGE_NAME "org.tizen.setting-applications"
#define APPLICATIONS_LOCALEDIR _TZ_SYS_RO_APP"/org.tizen.setting-applications/res/locale"
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
setting_view *__get_applications_view_to_load(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if((!data), NULL, "!data");

	setting_view_node_table_register(&setting_view_applications_main, NULL);
	setting_view_node_table_register(&setting_view_applications_defaultapp,
			&setting_view_applications_main);
	return &setting_view_applications_main;
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

	if (NULL != info->pkgid) {
		ret = app_control_unset_defapp(info->pkgid);
		if (ret != APP_CONTROL_ERROR_NONE)
			SETTING_TRACE_ERROR("app_control_unset_defapp() Fail(%d)", ret);
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

	if (!safeStrCmp(part, "elm.swallow.icon")) {
		Evas_Object *icon = NULL;
		icon = elm_icon_add(obj);
		SETTING_TRACE("path:[%s]", info->icon_path);
		if (NULL == info->icon_path) {
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

	} else if (!safeStrCmp(part, "elm.swallow.end")) {
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

static bool app_create(void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = priv;
	setting_retvm_if((!priv), NULL, "!priv");

	setting_set_i18n(SETTING_PACKAGE, APPLICATIONS_LOCALEDIR);

	if (app_init(&ad->md, SETTING_APPLICATIONS_PACKAGE_NAME)
			!= SETTING_RETURN_SUCCESS) {
		SETTING_TRACE_ERROR("Cannot initialize application");
		return false;
	}

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE,
			&(ad->itc_2text_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1icon_1text_sub));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_2text_3));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon_divider));
	setting_create_Gendial_itc(SETTING_GENLIST_GROUP_INDEX_STYLE,
			&(ad->itc_grp_title));

	ad->itc_1icon_1button.item_style = SETTING_GENLIST_2LINE_STYLE;
	ad->itc_1icon_1button.func.text_get = _gl_label_new_get;
	ad->itc_1icon_1button.func.content_get =
			_gl_1button1_icon_get;
	ad->itc_1icon_1button.func.state_get = NULL;
	ad->itc_1icon_1button.func.del = NULL;

	/*	creating a view. */
	ad->view_to_load = __get_applications_view_to_load(ad);
	setting_retvm_if(NULL == ad->view_to_load, NULL,
			"NULL == applicationsUG->view_to_load");

	setting_view_node_set_cur_view(ad->view_to_load);
	setting_view_create(ad->view_to_load, (void *)ad);
	evas_object_event_callback_add(ad->md.view_layout, EVAS_CALLBACK_RESIZE,
			setting_applications_ug_cb_resize, ad);
	return true;
}

static void app_pause(void *data)
{
	SETTING_TRACE_BEGIN;
}

static void app_resume(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");

#if 0
	SettingApplicationsUG *applicationsUG = data;
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

static void app_reset(app_control_h service, void *data)
{
	SETTING_TRACE_BEGIN;
}

static void app_terminate(void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingApplicationsUG *ad = priv;

	/* fix flash issue for gallery */
	evas_object_event_callback_del(ad->md.view_layout,
			EVAS_CALLBACK_RESIZE,
			setting_applications_ug_cb_resize);
	FREE(ad->uri);

	if (&setting_view_applications_main == ad->view_to_load) {
		setting_view_destroy(&setting_view_applications_main, ad);
	}

	SETTING_TRACE_END;
}

/*
static void setting_applications_ug_on_key_event(ui_gadget_h ug,
		enum ug_key_event event, app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = (SettingApplicationsUG *)priv;

	switch (event) {
	case UG_KEY_EVENT_END: {
		if (elm_naviframe_top_item_get(ad->md.navibar_main)
				== elm_naviframe_bottom_item_get(
						ad->md.navibar_main)) {
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
*/

static void _lang_changed(app_event_info_h event_info, void *data)
{
	SETTING_TRACE_BEGIN;
	SettingApplicationsUG *ad = data;
	setting_retm_if(NULL == ad, "ad is NULL");

	char *lang = NULL;
	if (app_event_get_language(event_info, &lang) == APP_ERROR_NONE) {
		SETTING_TRACE_DEBUG("Setting - language is changed : %s", lang);
		elm_language_set(lang);
		elm_config_all_flush();
		free(lang);
		setting_navi_items_update(ad->md.navibar_main);
	} else {
		SETTING_TRACE_ERROR("Cannot get language from event_info");
	}
}

EXPORT_PUBLIC
int main(int argc, char *argv[])
{
	app_event_handler_h handlers[5] = {NULL, };
	ui_app_lifecycle_callback_s ops = {
		.create = app_create,
		.pause = app_pause,
		.resume = app_resume,
		.terminate = app_terminate,
		.app_control = app_reset,
	};
	SettingApplicationsUG app_data;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY],
			APP_EVENT_LOW_MEMORY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY],
			APP_EVENT_LOW_BATTERY, NULL, NULL);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED],
			APP_EVENT_LANGUAGE_CHANGED, _lang_changed, &app_data);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED],
			APP_EVENT_REGION_FORMAT_CHANGED, NULL, NULL);
	ui_app_add_event_handler(
			&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED],
			APP_EVENT_DEVICE_ORIENTATION_CHANGED, NULL, NULL);
	memset(&app_data, 0x0, sizeof(app_data));

	return ui_app_main(argc, argv, &ops, &app_data);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/*
#define BUS_NAME			"org.tizen.system.deviced"
#define INTERFACE_NAME			BUS_NAME
#define OBJECT_PATH			"/Org/Tizen/System/DeviceD"
#define DEVICED_PATH_DISPLAY		OBJECT_PATH"/Applications"
#define DEVICED_INTERFACE_DISPLAY	INTERFACE_NAME".applications"
#define DBUS_REPLY_TIMEOUT		(120 * 1000)
*/
