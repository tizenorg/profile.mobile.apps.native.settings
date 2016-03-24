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
#include <setting-common-data-type.h>
#include <setting-common-data-slp-setting.h>
#include <setting-common-draw-widget.h>

#include <setting-security-enc-processing.h>
#include <setting-debug.h>

#include <ode.h>
#include <dd-deviced.h>

static int setting_security_enc_processing_create(void *cb);
static int setting_security_enc_processing_destroy(void *cb);
static int setting_security_enc_processing_update(void *cb);
static int setting_security_enc_processing_cleanup(void *cb);

setting_view setting_view_security_enc_processing = {
	.create = setting_security_enc_processing_create,
	.destroy = setting_security_enc_processing_destroy,
	.update = setting_security_enc_processing_update,
	.cleanup = setting_security_enc_processing_cleanup,
};

/* ***************************************************
 **
 **basic func
 **
 ****************************************************/

static void __encrypting_key_grab(SettingSecurityUG *ad)
{
	SETTING_TRACE_BEGIN;
#ifdef ECORE_X
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;
#endif

	int ret = 0;

	if (ad == NULL)
		return;

#ifdef ECORE_X
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get((Evas_Object *)ug_get_window());


	ret = eext_win_keygrab_set(xwin, "XF86PowerOff");
	if (ret)
		return;
	ret = eext_win_keygrab_set(xwin, "XF86Home");
	if (ret)
		return;
	ret = eext_win_keygrab_set(xwin, "XF86AudioRaiseVolume");
	if (ret)
		return;
	ret = eext_win_keygrab_set(xwin, "XF86AudioLowerVolume");
	if (ret)
		return;
#endif
	ad->event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, NULL, NULL);
	SETTING_TRACE_END;
}
static void __encrypting_key_ungrab(SettingSecurityUG *ad)
{
	SETTING_TRACE_BEGIN;
#ifdef ECORE_X
	Ecore_X_Window xwin = 0;
	Ecore_X_Display *disp = NULL;
#endif

	int ret = 0;

	if (ad == NULL)
		return;

#ifdef ECORE_X
	disp = ecore_x_display_get();
	xwin = elm_win_xwindow_get((Evas_Object *)ug_get_window());
#endif

	ret = eext_win_keygrab_unset(xwin, "XF86PowerOff");
	if (ret)
		return;
	ret = eext_win_keygrab_unset(xwin, "XF86Home");
	if (ret)
		return;
	ret = eext_win_keygrab_unset(xwin, "XF86AudioRaiseVolume");
	if (ret)
		return;
	ret = eext_win_keygrab_unset(xwin, "XF86AudioLowerVolume");
	if (ret)
		return;

	ecore_event_handler_del(ad->event_handler);
}

static void __vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	SettingSecurityUG *ad = (SettingSecurityUG *) data;

	/* To do : update progressbar */
	char *vconf_name = vconf_keynode_get_name(key);
	char *vconf_value = NULL;
	char temp[MAX_COMMON_BUFFER_LEN] = {0,};

	/*if (!safeStrCmp(vconf_name, VCONFKEY_ODE_ENCRYPT_PROGRESS)) {*/
	if (!safeStrCmp(vconf_name, "db/ode/encrypt_progress")) {
		vconf_value = (char *)g_strdup(vconf_keynode_get_str(key));
		if (vconf_value == NULL)
			return;

		SETTING_TRACE_DEBUG("progress : %s", vconf_value);
		float i_value = atof(vconf_value);
		if (i_value <= 0)
			return;

		if (ad->cur_enc_mode == 0) {
			snprintf(temp, MAX_COMMON_BUFFER_LEN, _(ENCRYPTING_GUIDE_02), (int)i_value);
		} else if (ad->cur_enc_mode == 1) {
			snprintf(temp, MAX_COMMON_BUFFER_LEN, _(DECRYPTING_GUIDE_02), (int)i_value);
		}
		elm_object_part_text_set(ad->enc_layout, "description", temp);
		SETTING_TRACE_DEBUG("current status : %s", temp);

		elm_progressbar_value_set(ad->enc_progressbar, i_value / 100.0);
		SETTING_TRACE_DEBUG("progressbar is updated. %f", i_value / 100.0);
		G_FREE(vconf_value);

		if (i_value == 100) {
			SETTING_TRACE_DEBUG("100%");
			/* To do : Encryption completed. Restart target. */
			if (ode_crypto_complete() == 0) {
				SETTING_TRACE_DEBUG("ode_crypto_complete() success.");
				ode_deinit();
				/*system("reboot"); */
			}
		}
	}
}

int __listen_vconf_change(void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	int ret = 0;

	ret = vconf_notify_key_changed(VCONFKEY_ODE_ENCRYPT_PROGRESS,
	                               __vconf_change_cb, data);
	setting_retvm_if(ret < 0, ret, "%s notifications Failed(%d)",
	                 (char *)VCONFKEY_ODE_ENCRYPT_PROGRESS, ret);

	return ret;
}

static int setting_security_enc_processing_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *) cb;

	if (ad->input_pwd == NULL)
		return SETTING_RETURN_FAIL;

	Evas_Object *window = NULL;
	Evas_Object *layout = NULL;
	Evas_Object *progressbar = NULL;
	char enc_type[16] = {0,};

	layout = elm_layout_add(ad->win_main_layout);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME, "encryption_layout");
	/*elm_win_center(layout, EINA_TRUE, EINA_TRUE); */
	/*elm_win_fullscreen_set(layout, EINA_TRUE); */
	/*setting_resize_object(layout, 720, 1280); */

	/*elm_win_resize_object_add(ad->win_get, layout); */

	progressbar = elm_progressbar_add(layout);
	elm_progressbar_value_set(progressbar, 0);
	elm_object_part_content_set(layout, "progressbar", progressbar);

	evas_object_show(layout);

	ad->enc_progressbar = progressbar;
	ad->enc_layout = layout;

	/* grab home key and volume key */
	__encrypting_key_grab(ad);
	__listen_vconf_change(ad);

	if (ode_init() < 0) {
		SETTING_TRACE_DEBUG("ode_init() failed");
		setting_security_enc_processing_destroy(ad);
		return SETTING_RETURN_FAIL;
	}

	if (ad->cur_enc_mode == 0) {
		elm_object_part_text_set(layout, "title", _(Keystr_Encrypting));
		elm_object_part_text_set(layout, "description", _(ENCRYPTING_GUIDE_01));
		safeCopyStr(enc_type, "inplace", 16);
#if TEMP
		if (ode_enable(enc_type, ad->input_pwd) == 0) {
			SETTING_TRACE_DEBUG("ode_enable(%s, %s) success", ad->input_pwd, enc_type);
		} else {
			SETTING_TRACE_DEBUG("ode_enable(%s, %s) failed", ad->input_pwd, enc_type);
		}
#endif
	} else if (ad->cur_enc_mode == 1) {
		elm_object_part_text_set(layout, "title", _(Keystr_Decrypting));
		elm_object_part_text_set(layout, "description", _(DECRYPTING_GUIDE_02));
		safeCopyStr(enc_type, "disable", 16);
#if TEMP
		if (ode_disable(ad->input_pwd) == 0) {
			SETTING_TRACE_DEBUG("ode_disable(%s, %s) success", ad->input_pwd, enc_type);
		} else {
			SETTING_TRACE_DEBUG("ode_disable(%s, %s) failed", ad->input_pwd, enc_type);
		}
#endif
	}

	setting_view_security_enc_processing.is_create = 1;

	return SETTING_RETURN_SUCCESS;

}

static int setting_security_enc_processing_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingSecurityUG *ad = (SettingSecurityUG *) cb;

	vconf_ignore_key_changed(VCONFKEY_ODE_ENCRYPT_PROGRESS, __vconf_change_cb);

	if (ad->enc_layout) {
		evas_object_del(ad->enc_layout);
		ad->enc_layout = NULL;
	}

	setting_view_security_enc_processing.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_enc_processing_update(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_security_enc_processing_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_security_enc_processing_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}
