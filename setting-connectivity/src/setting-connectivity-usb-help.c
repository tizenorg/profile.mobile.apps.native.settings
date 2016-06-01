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
#include <setting-connectivity-usb-help.h>
#define HELP_LEN 2048

static int setting_connectivity_usb_help_create(void *cb);
static int setting_connectivity_usb_help_destroy(void *cb);
static int setting_connectivity_usb_help_update(void *cb);
static int setting_connectivity_usb_help_cleanup(void *cb);

setting_view setting_view_connectivity_usb_help = {
	.create = setting_connectivity_usb_help_create,
	.destroy = setting_connectivity_usb_help_destroy,
	.update = setting_connectivity_usb_help_update,
	.cleanup = setting_connectivity_usb_help_cleanup, };

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_connectivity_usb_help_create(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingConnectivityUG *ad = (SettingConnectivityUG *)cb;

	char samsung_kies_help[HELP_LEN];
	char mass_storage_help[HELP_LEN];
	char debugging_mode_help[HELP_LEN];

	char *kies_help_mk = evas_textblock_text_utf8_to_markup(NULL,
			_("IDS_ST_BODY_SAMSUNG_KIES_CONNECTION_CONNECTING_USB_"\
					"HELP_MSG"));
	char *storage_help_mk = evas_textblock_text_utf8_to_markup(NULL,
			_("IDS_ST_BODY_MASS_STORAGE_HELP_MSG"));
	char *debugmode_help_mk = evas_textblock_text_utf8_to_markup(NULL,
			_("IDS_ST_BODY_USB_DEBUGGING_IS_INTENDED_FOR_DEVELOPME"\
					"NT_PURPOSES_ONLY_MSG"));

	snprintf(samsung_kies_help, HELP_LEN, "%s%s%s%s", "<b>",
			_("IDS_ST_HEADER_SAMSUNG_KIES"), "</b><br>",
			kies_help_mk);
	snprintf(mass_storage_help, HELP_LEN, "%s%s%s%s", "<b>",
			_("IDS_ST_OPT_MASS_STORAGE_ABB"), "</b><br>",
			storage_help_mk);
	snprintf(debugging_mode_help, HELP_LEN, "%s%s%s%s", "<b>",
			_("IDS_ST_BODY_USB_DEBUGGING"), "</b><br>",
			debugmode_help_mk);

	FREE(kies_help_mk);
	FREE(storage_help_mk);
	FREE(debugmode_help_mk);

	Evas_Object *genlist;
	setting_push_layout_navi_bar_genlist(ad->win_main_layout, ad->win_get,
			"IDS_ST_HEADER_HELP", _("IDS_ST_BUTTON_BACK"),
			NULL, setting_connectivity_usb_help_click_back_cb,
			NULL, ad, &genlist, ad->navi_bar);

	char *strsamsung_kies_help = samsung_kies_help;
	char *strmass_storage_help = mass_storage_help;
	char *strdebugging_mode_help = debugging_mode_help;
	setting_add_gl_help(genlist, strsamsung_kies_help);
	setting_add_gl_help(genlist, strmass_storage_help);
	setting_add_gl_help(genlist, strdebugging_mode_help);

	setting_view_connectivity_usb_help.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_connectivity_usb_help_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingConnectivityUG *ad = (SettingConnectivityUG *)cb;

	if (setting_view_connectivity_usb_help.is_create) {
		elm_naviframe_item_pop(ad->navi_bar);
		setting_view_connectivity_usb_help.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_connectivity_usb_help_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_connectivity_usb_help_cleanup(void *cb)
{
	return setting_connectivity_usb_help_destroy(cb);
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void setting_connectivity_usb_help_click_back_cb(void *data,
		Evas_Object *obj, void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, " Data parameter is NULL");

	SettingConnectivityUG *ad = (SettingConnectivityUG *)data;
	setting_view_change(&setting_view_connectivity_usb_help,
			&setting_view_connectivity_usb, ad);
}
