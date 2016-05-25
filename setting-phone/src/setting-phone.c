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

/**
 *@defgroup setting-phone
 *UG creation code for setting-phone
 *
 * - display
 * - event notification
 * - dispaly language
 * - region format
 */

#include <setting-phone.h>
#include <libxml/xmlmemory.h>
#include "unicode/unum.h"
#include <iniparser.h>
#include <locale.h>
#include <setting-cfg.h>

#include <eventsystem.h>
#include <system_settings.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

#define SETTING_PHONE_EDJ_PATH		EDJDIR
#define SETTING_PHONE_GENLIST_EDJ_NAME	SETTING_PHONE_EDJ_PATH"/setting-genlist.edj"


setting_view *__get_phone_view_to_load(app_control_h service)
{
	SETTING_TRACE_BEGIN;

	char *viewtype = NULL;
	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype) {
		return NULL;
	}

	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "language")) {
		char *caller = NULL;

		app_control_get_extra_data(service, "caller", &caller);
		if (caller) {
			setting_view_node_table_register(&setting_view_phone_display_language, NULL);
			FREE(viewtype);
			FREE(caller);
			return &setting_view_phone_display_language;
		} else {
			setting_view_node_table_register(&setting_view_phone_language_region, NULL);
			setting_view_node_table_register(&setting_view_phone_display_language,
											 &setting_view_phone_language_region);
			setting_view_node_table_register(&setting_view_phone_region_format,
											 &setting_view_phone_language_region);
			FREE(viewtype);
			return &setting_view_phone_language_region;
		}
	} else if (!safeStrCmp(viewtype, "license")) {
		setting_view_node_table_register(&setting_view_phone_license_main, NULL);
		FREE(viewtype);
		return &setting_view_phone_license_main;
	}
#ifdef ENABLE_TICKER_NOTI
	else if (!safeStrCmp(viewtype, "notification")) {
		setting_view_node_table_register(&setting_view_phone_ticker_notification, NULL);
		setting_view_node_table_register(&setting_view_phone_ticker_notification_details, &setting_view_phone_ticker_notification);
		FREE(viewtype);
		return &setting_view_phone_ticker_notification;
	}
#endif
	else if (!safeStrCmp(viewtype, "region")) {
		setting_view_node_table_register(&setting_view_phone_region_format, NULL);
		FREE(viewtype);
		return &setting_view_phone_region_format;
	} else {
		FREE(viewtype);
		return NULL;	/* &setting_view_phone_main; */
	}
}

Evas_Object *__get_phone_layout_to_return(app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingPhoneUG *phoneUG = priv;
	char *viewtype = NULL;

	app_control_get_extra_data(service, "viewtype", &viewtype);
	if (!viewtype) {
		return NULL;
	}
	SETTING_TRACE("viewtype:%s", viewtype);

	if (!safeStrCmp(viewtype, "language")) {
		FREE(viewtype);
		return phoneUG->ly_language;
	} else if (!safeStrCmp(viewtype, "license")) {
		FREE(viewtype);
		return phoneUG->ly_license;
	} else if (!safeStrCmp(viewtype, "notification")) {
		FREE(viewtype);
		return phoneUG->ly_ticker;
	} else if (!safeStrCmp(viewtype, "region")) {
		FREE(viewtype);
		return phoneUG->ly_region;
	} else {
		FREE(viewtype);
		return NULL;	/* &setting_view_phone_main; */
	}

}


const char *get_language_by_mobile_country_code(char *mcc)
{
	const char *simLanguage = NULL;
	char temp[128] = {0,};
	Eina_List *elist = NULL;
	setting_lang_entry *pnode = NULL;
	Eina_List *langlist = setting_get_language_list();

	EINA_LIST_FOREACH(langlist, elist, pnode) {
		if (strstr(pnode->mcc, mcc) != NULL) {
			snprintf(temp, 127, "%s.UTF-8", pnode->locale);
		}
	}

	if (safeStrLen(temp) > 0) {
		simLanguage = (char *)strdup(temp);
		SETTING_TRACE_DEBUG("simLanguage: %s", simLanguage);
	}

	return simLanguage;
}


static void setting_phone_update_item(void *data)
{
	setting_retm_if(data == NULL, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *) data;

	Setting_GenGroupItem_Data *item_to_update = NULL;

	item_to_update = ad->data_display_language;
	retm_if(!item_to_update, "ad->data_display_language is NULL");

	if (item_to_update) {
		item_to_update->sub_desc = get_pa_display_language_str();
		elm_object_item_data_set(item_to_update->item, item_to_update);
		elm_genlist_item_update(item_to_update->item);
	}

	char *pa_region = vconf_get_str(VCONFKEY_REGIONFORMAT);
	if (pa_region == NULL) {
		SETTING_TRACE_ERROR
		("[Error] get value of VCONFKEY_REGIONFORMAT fail.");
		return;
	}

	item_to_update = ad->data_region_fmt;
	retm_if(!item_to_update, "ad->data_region_fmt is NULL");

	if (item_to_update) {
		int region_automatic = 1;
		int ret = vconf_get_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, &region_automatic);
		if (ret == VCONF_ERROR) {
			SETTING_TRACE_ERROR("get vconf region automatic err.");
		}

		if (region_automatic == 1) {
			item_to_update->sub_desc = (char *)g_strdup(_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"));
		} else {
			item_to_update->sub_desc = setting_phone_lang_get_region_str(pa_region);
		}
		item_to_update->keyStr = (char *)g_strdup(_(REGION_FORMAT_LOWER));
		elm_object_item_data_set(item_to_update->item, item_to_update);
		elm_genlist_item_update(item_to_update->item);
	}

	if (ad->data_region_fmt_example) {

		char *example_desc =
			setting_phone_lang_get_example_desc(pa_region, ad);
		ad->data_region_fmt_example->keyStr =
			(char *)g_strdup(example_desc);
		FREE(example_desc);
		elm_object_item_data_set(ad->data_region_fmt_example->item, ad->data_region_fmt_example);
		elm_genlist_item_update(ad->data_region_fmt_example->item);
	}

	FREE(pa_region);
}

char *setting_phone_lang_get_region_str(const char *region)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(region == NULL, NULL, "region parameter is NULL");
	const char *localeID = NULL;
	UChar displayname[SETTING_STR_SLP_LEN] = { 0, };
	UErrorCode status = U_ZERO_ERROR;
	char *ret_str = NULL;

	const char *pa_lang = vconf_get_str(VCONFKEY_LANGSET);
	if (pa_lang) {
		char *q = strchr(pa_lang, '.');
		if (q) {
			*q = '\0';
		}
		SETTING_TRACE("pa_lang:%s", pa_lang);
		int err = -1;
		uloc_setDefault(pa_lang, &err);
		free((void *)pa_lang);
		pa_lang = NULL;
	} else {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	localeID = uloc_getDefault();
	uloc_getDisplayName(region, localeID, displayname, SETTING_STR_SLP_LEN,
						&status);

	char *str_displayname = setting_phone_lang_str_to_utf8(displayname);
	if (str_displayname == NULL) {
		SETTING_TRACE("Get displayname in utf8 failed");
		return NULL;
	}
	int len = 0;
	UErrorCode errcode;
	char source[256] = {0,};
	UChar dest[256] = {0,};
	if (strlen(str_displayname) < 256) {
		strncpy(source, str_displayname,256);
	} else {
		FREE(str_displayname);
		return NULL;
	}
	u_strFromUTF8(dest, 256, &len, source, 256, &errcode);
	u_strToUpper(dest, 1, dest, 1, "", &errcode);
	u_strToUTF8(source, 256, NULL, dest, 256, &errcode);
	ret_str = (char *)g_strdup(source);

	FREE(str_displayname);
	return ret_str;
}

/**
* Change language string to UTF8 format, the invoker need to free the return string
*
* @param unichars The source string will be translated.
*
* @return UTF8 format string.
*/
char *setting_phone_lang_str_to_utf8(const UChar *unichars)
{
	retv_if(unichars == NULL, NULL);
	int len = 0;
	int len_str = 0;
	int len_utf8 = 0;
	char *str = NULL;
	UErrorCode status = U_ZERO_ERROR;

	len = u_strlen(unichars);
	len_str = sizeof(char)*4*(len+1);
	str = (char *)calloc(1, len_str);
	if (!str) {
		return NULL;
	}

	u_strToUTF8(str, len_str, &len_utf8, unichars, len, &status);
	return str;
}

char *setting_phone_lang_get_by_pattern(const char *locale, const char *skeleton, void *data)
{
	setting_retvm_if(!locale, NULL, "locale parameter is NULL");
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");

	SettingPhoneUG *ad = (SettingPhoneUG *) data;

	/*remove ".UTF-8" in locale */
	char locale_tmp[32] = {0,};
	if (strlen(locale) < 32)	{
		strncpy(locale_tmp, locale,32);
	} else {
		return NULL;
	}
	char *p = safeStrStr(locale_tmp, ".UTF-8");
	if (p) {
		*p = 0;
	}

	char *ret_str = NULL;
	UChar customSkeleton[SETTING_STR_SLP_LEN] = { 0, };
	UErrorCode status = U_ZERO_ERROR;
	UDateFormat *formatter = NULL;

	UChar bestPattern[SETTING_STR_SLP_LEN] = { 0, };
	UChar formatted[SETTING_STR_SLP_LEN] = { 0, };

	char bestPatternString[SETTING_STR_SLP_LEN] = { 0, };
	char formattedString[SETTING_STR_SLP_LEN] = { 0, };

	u_uastrncpy(customSkeleton, skeleton, safeStrLen(skeleton));

	if (ad->pattern_generator == NULL
		|| safeStrCmp(locale_tmp, ad->prev_locale) != 0) {
		if (ad->pattern_generator != NULL) {
			udatpg_close(ad->pattern_generator);
			ad->pattern_generator = NULL;
		}
		ad->pattern_generator = udatpg_open(locale_tmp, &status);
	}

	int32_t bestPatternCapacity =
		(int32_t)(sizeof(bestPattern) / sizeof((bestPattern)[0]));
	(void)udatpg_getBestPattern(ad->pattern_generator, customSkeleton,
								u_strlen(customSkeleton), bestPattern,
								bestPatternCapacity, &status);

	u_austrcpy(bestPatternString, bestPattern);
	SETTING_TRACE("BestPattern is %s", bestPatternString);

	UDate date = ucal_getNow();
	formatter =
		udat_open(UDAT_IGNORE, UDAT_IGNORE, locale_tmp, NULL, -1, bestPattern,
				  -1, &status);
	int32_t formattedCapacity =
		(int32_t)(sizeof(formatted) / sizeof((formatted)[0]));
	(void)udat_format(formatter, date, formatted, formattedCapacity, NULL,
					  &status);
	u_austrcpy(formattedString, formatted);
	SETTING_TRACE("DATE & TIME is %s", formattedString);

	udat_close(formatter);

	ret_str = g_strdup(formattedString);
	return ret_str;
}

#if 1
static char *__setting_phone_number_format_get(const char *region)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(region == NULL, NULL, "region parameter is NULL");
	UNumberFormat *fmt = NULL;
	UErrorCode status = U_ZERO_ERROR;
	char decimal[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
	char separator[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
	char decimal_ch = 0;
	char separator_ch = 0;
	UChar u_decimal[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
	UChar u_thousand[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0 };
	fmt = unum_open(UNUM_DEFAULT, NULL, -1, region, NULL, &status);
	unum_getSymbol(fmt, UNUM_DECIMAL_SEPARATOR_SYMBOL, u_decimal, MAX_DISPLAY_NAME_LEN_ON_UI, &status);
	u_austrcpy(decimal, u_decimal);
	unum_getSymbol(fmt, UNUM_GROUPING_SEPARATOR_SYMBOL, u_thousand, MAX_DISPLAY_NAME_LEN_ON_UI, &status);
	u_austrcpy(separator, u_thousand);

	int len_seq = safeStrLen(separator);
	decimal_ch = decimal[0];
	separator_ch = separator[0];
	if (len_seq == 2 || len_seq == 0) {
		separator_ch = 32;
	}
	if (len_seq == 3 && !safeStrCmp(separator, "\xe2\x80\x99")) {
		separator_ch = 39;
	}

	len_seq = safeStrLen(decimal);
	if (len_seq == 2 || len_seq == 0) {
		decimal_ch = 44;
	}
	SETTING_TRACE("separator, decimal = %c, %c", separator_ch, decimal_ch);

	unum_close(fmt);

	/*make example */
	char loc_number[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	snprintf(loc_number, sizeof(loc_number), "1%c234%c567%c89", separator_ch, separator_ch, decimal_ch);
	SETTING_TRACE("loc_number = %s", loc_number);
	return g_strdup(loc_number); /* should be freed by caller */
}
#endif


#if 0
static char *__setting_phone_number_format_get(const char *region)
{
	setting_retvm_if(region == NULL, NULL, "region parameter is NULL");
	UNumberFormat *fmt = NULL;
	UErrorCode status = U_ZERO_ERROR;
	UChar number_uchar[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	char number_char[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	double example_number = 1234567.89;

	fmt = unum_open(UNUM_DEFAULT, NULL, -1, region, NULL, &status);
	unum_formatDouble(fmt, example_number, number_uchar, MAX_DISPLAY_NAME_LEN_ON_UI, NULL, &status);
	u_austrcpy(number_char, number_uchar);
	SETTING_TRACE("number_char = %s", number_char);
	unum_close(fmt);

	return g_strdup(number_char); /* should be freed by caller */
}
#endif

#if 0
static char *__setting_phone_number_format_get(const char *region)
{
	SETTING_TRACE_BEGIN;
	struct lconv *locale = NULL;
	char *decimal = NULL;
	char *separator = NULL;
	char decimal_ch = 0;
	char separator_ch = 0;

	locale = localeconv();
	SETTING_TRACE("check localeconv");
	decimal = locale->decimal_point;
	separator = locale->thousands_sep;
	int len_seq = safeStrLen(separator);
	decimal_ch = decimal[0];
	separator_ch = separator[0];
	if (len_seq == 2 || len_seq == 0) {
		separator_ch = 32;
	}
	SETTING_TRACE("separator, decimal = %c, %c", separator_ch, decimal_ch);

	/*make example */
	char loc_number[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	snprintf(loc_number, sizeof(loc_number), "1%c234%c567%c89", separator_ch, separator_ch, decimal_ch);
	SETTING_TRACE("loc_number = %s", loc_number);
	return g_strdup(loc_number); /* should be freed by caller */
}
#endif

char *setting_phone_lang_get_example_desc(const char *region, void *data)
{
	setting_retvm_if(region == NULL, NULL, "region parameter is NULL");
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	char *date = setting_phone_lang_get_by_pattern(region,
												   UDAT_YEAR_MONTH_WEEKDAY_DAY,
												   data);

	enum appcore_time_format timeformat = APPCORE_TIME_FORMAT_12;
	int ret = appcore_get_timeformat(&timeformat);
	if (ret == -1) {
		SETTING_TRACE_ERROR("call appcore_get_timeformat fail.");
		timeformat = APPCORE_TIME_FORMAT_12;
	}

	char *time_skeleton = "hhmm";
	if (timeformat == APPCORE_TIME_FORMAT_24) {
		time_skeleton = "HHmm";
	}

	/*char *region_jp = vconf_get_str(VCONFKEY_REGIONFORMAT); */
	char *time = NULL;

	/*if (safeStrStr(region_jp, "ja_JP") == NULL) { */
	time = setting_phone_lang_get_by_pattern(region, time_skeleton, data);
	/*}else{
		time = setting_phone_lang_get_by_pattern("en_GB.UTF-8", time_skeleton, data);
	}*/

	char *number = __setting_phone_number_format_get(region);

	Eina_Strbuf *temp_str = eina_strbuf_new();
	char *r_str_text = NULL;
	eina_strbuf_append_printf(temp_str,
							  "%s", _("IDS_ST_BODY_REGIONAL_EXAMPLE"));
	eina_strbuf_append_printf(temp_str, "<br>%s : %s", _("IDS_ST_BODY_DATE"), date);
	eina_strbuf_append_printf(temp_str, "<br>%s : %s", _("IDS_ST_HEADER_TIME"), time);
	eina_strbuf_append_printf(temp_str, "<br>%s : %s", _("IDS_ST_BODY_NUMBER"), number);

	SETTING_TRACE("temp_str: %s", temp_str);
	r_str_text = eina_strbuf_string_steal(temp_str);
	/* eina_strbuf_string_free(temp_str); */
	eina_strbuf_free(temp_str);
	SETTING_TRACE("r_str_text: %s", r_str_text);

	G_FREE(date);
	G_FREE(time);
	G_FREE(number);
	return r_str_text;	/* will be freed in calling place */
}


static void setting_phone_ug_cb_resize(void *data, Evas *e, Evas_Object *obj,
									   void *event_info)
{
	SettingPhoneUG *ad = (SettingPhoneUG *) data;
	/* setting_view_update(&setting_view_about_main, ad); */
	setting_view_update(ad->view_to_load, ad);
}


#if SUPPORT_APP_ROATION
static void _rot_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingPhoneUG *phoneUG = (SettingPhoneUG *)data;
	if (phoneUG  == NULL || phoneUG ->win_get == NULL) {
		return;
	}
	int change_ang = elm_win_rotation_get(phoneUG->win_get);
	SETTING_TRACE_DEBUG("....change_ang:%d", change_ang);
	SETTING_TRACE_DEBUG("current_rotation:%d", phoneUG->current_rotation);
	/*Send the rotation event to UGs.. */
	enum ug_event event = UG_EVENT_ROTATE_PORTRAIT;
	switch (change_ang) {
	case APP_DEVICE_ORIENTATION_0:
		event = UG_EVENT_ROTATE_PORTRAIT;
		break;
	case APP_DEVICE_ORIENTATION_180:
		event = UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN;
		break;
	case APP_DEVICE_ORIENTATION_270:
		event = UG_EVENT_ROTATE_LANDSCAPE;
		break;
	case APP_DEVICE_ORIENTATION_90:
		event = UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN;
		break;
	default:
		return;
	}
	SETTING_TRACE_DEBUG("diff:%d", elm_win_rotation_get(phoneUG->win_get) - phoneUG->current_rotation);

	if (change_ang != phoneUG->current_rotation) {
		int diff = change_ang - phoneUG->current_rotation;
		if (diff < 0) {
			diff = -diff;
		}
		/**
		* @todo if app didn't launch UG, is the call required to invoke?
		*/
		ug_send_event(event);
		if (diff == 180) {
			/* do nothing */
		}
		phoneUG->current_rotation = change_ang;
	}
}
#endif

static void *setting_phone_ug_on_create(ui_gadget_h ug, enum ug_mode mode,
										app_control_h service, void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");
	SETTING_TRACE_BEGIN;
	SettingPhoneUG *phoneUG = priv;
	phoneUG->ug = ug;

	phoneUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	phoneUG->win_get = (Evas_Object *) ug_get_window();
	evas_object_show(phoneUG->win_main_layout);
	phoneUG->evas = evas_object_evas_get(phoneUG->win_main_layout);

	setting_retvm_if(phoneUG->win_main_layout == NULL, NULL,
					 "cannot get main window ");




	phoneUG->current_rotation = elm_win_rotation_get(phoneUG->win_get);
	SETTING_TRACE_DEBUG("ad->current_rotation:%d", phoneUG->current_rotation);
	if (elm_win_wm_rotation_supported_get(phoneUG->win_get)) {
		int rots[4] = { 0, 90, 180, 270 };	/* rotation value that app may want */
		elm_win_wm_rotation_available_rotations_set(phoneUG->win_get, rots, 4);
	}
	evas_object_smart_callback_add(phoneUG->win_get, "wm,rotation,changed", _rot_changed_cb, phoneUG);


	/* --------------------------------------------------------- */
	char *pa_path = NULL;;

	/*elm_theme_extension_add(NULL, SETTING_PHONE_GENLIST_EDJ_NAME); */

	pa_path = vconf_get_str(VCONFKEY_LANGSET);
	if (!pa_path) {
		SETTING_TRACE
		("%s*** language setting has no proper value (nil) ***%s",
		 SETTING_FONT_BGREEN, SETTING_FONT_BLACK);
		return NULL;
	}
	/* set launguage */
	bindtextdomain(SETTING_PACKAGE, SETTING_LOCALEDIR);
	FREE(pa_path);
	/* ---------------------------------------------------------	 */
	int value = 0;
	int err;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_SIM_SLOT, &value, &err);
	if (ret != 0) {
		SETTING_TRACE("fail to get vconf");
	}
	if (value == VCONFKEY_TELEPHONY_SIM_INSERTED) {
		phoneUG->handle = tel_init(NULL);
		SETTING_TRACE("phoneUG->handle:%d", phoneUG->handle);
		memset(&(phoneUG->imsi), 0, sizeof(TelSimImsiInfo_t));
		if (phoneUG->handle
			&& TAPI_API_SUCCESS == tel_get_sim_imsi(phoneUG->handle, &(phoneUG->imsi))) {
			SETTING_TRACE_SECURE_DEBUG("********************************************");
			SETTING_TRACE_SECURE_DEBUG("imsi.szMcc: [%s]", phoneUG->imsi.szMcc);
			SETTING_TRACE_SECURE_DEBUG("imsi.szMnc: [%s]", phoneUG->imsi.szMnc);
			SETTING_TRACE_SECURE_DEBUG("imsi.szMsin: [%s]", phoneUG->imsi.szMsin);
			SETTING_TRACE_SECURE_DEBUG("********************************************");

			phoneUG->sim_lang = get_language_by_mobile_country_code(phoneUG->imsi.szMcc);
		}
	}
	if (!phoneUG->sim_lang) {
		/* if CSC has updated configuration for setting, initial vconf value changes. */
		phoneUG->sim_lang = vconf_get_str(VCONFKEY_LANGSET); /* allocated */
	}
	SETTING_TRACE("sim_lang: [%s]", phoneUG->sim_lang);

	/* register view node table */
	setting_view_node_table_intialize();

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(phoneUG->itc_1text_1icon_2));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(phoneUG->itc_1text_1icon));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(phoneUG->itc_1text_tb));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(phoneUG->itc_1text));
	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(phoneUG->itc_1icon_1text_sub));

	/* 2 text - genlist > items */
	setting_create_Gendial_itc("dialogue/2text", &(phoneUG->itc_1icon_2text));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(phoneUG->itc_2text_3_parent));
	setting_create_Gendial_itc(SETTING_GENLIST_2LINE_STYLE, &(phoneUG->itc_2text_2));
	setting_create_Gendial_itc("multiline/1text",
							   &(phoneUG->itc_bg_1icon));
	phoneUG->pattern_generator = NULL;
	phoneUG->prev_locale = NULL;
	/*	creating a view. */
	phoneUG->bundle_data = service;
	phoneUG->view_to_load = __get_phone_view_to_load(service);
	setting_retvm_if(NULL == phoneUG->view_to_load, NULL,
					 "NULL == phoneUG->view_to_load");
	setting_view_node_set_cur_view(phoneUG->view_to_load);
	setting_view_create(phoneUG->view_to_load, (void *)phoneUG);
	evas_object_event_callback_add(phoneUG->win_main_layout,
								   EVAS_CALLBACK_RESIZE,
								   setting_phone_ug_cb_resize, phoneUG);
	return __get_phone_layout_to_return(service, phoneUG);
}

static void setting_phone_ug_on_start(ui_gadget_h ug, app_control_h service,
									  void *priv)
{
}

static void setting_phone_ug_on_pause(ui_gadget_h ug, app_control_h service,
									  void *priv)
{
}

static void setting_phone_ug_on_resume(ui_gadget_h ug, app_control_h service,
									   void *priv)
{
	SETTING_TRACE_BEGIN;
	ret_if(!priv);
	SettingPhoneUG *phoneUG = priv;

	Eina_Bool is_freezed = evas_object_freeze_events_get(phoneUG->navi_bar);
	SETTING_TRACE_DEBUG("is_freezed : %d", is_freezed);
	if (is_freezed)
		evas_object_freeze_events_set(phoneUG->navi_bar, EINA_FALSE);
}

static void setting_phone_ug_on_destroy(ui_gadget_h ug, app_control_h service,
										void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!ug || !priv), "!ug || !priv");
	SettingPhoneUG *phoneUG = priv;

	evas_object_event_callback_del(phoneUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_phone_ug_cb_resize);	/* fix flash issue for gallery */
	phoneUG->ug = ug;

	if (phoneUG->handle && tel_deinit(phoneUG->handle) != TAPI_API_SUCCESS) {
		SETTING_TRACE_ERROR("tel_deinit failed");
	}
	/*	called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_phone_language_region == phoneUG->view_to_load) {
		setting_view_destroy(&setting_view_phone_display_language,
							 phoneUG);
		setting_view_destroy(&setting_view_phone_region_format,
							 phoneUG);
		setting_view_destroy(&setting_view_phone_language_region, phoneUG);
	}
#ifdef ENABLE_TICKER_NOTI
	else if (&setting_view_phone_ticker_notification ==
			 phoneUG->view_to_load) {
		setting_view_destroy(&setting_view_phone_ticker_notification,
							 phoneUG);
		setting_view_destroy(&setting_view_phone_ticker_notification_details, phoneUG);

	}
#endif
	else if (&setting_view_phone_license_main == phoneUG->view_to_load) {
		setting_view_destroy(&setting_view_phone_license_main, phoneUG);
	}

	FREE(phoneUG->sim_lang);

	if (NULL != ug_get_layout(phoneUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(phoneUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(phoneUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_phone_ug_on_message(ui_gadget_h ug, app_control_h msg,
										app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == priv, "priv is NULL");
	char *pid_str = NULL;

	app_control_get_extra_data(msg, "DEADPID", &pid_str);
	if (pid_str) {
		errno = 0;
		int pid = (int)g_ascii_strtod(pid_str, NULL);
		SETTING_TRACE_DEBUG("pid = %d", pid);
		setting_retm_if(errno == ERANGE, "call g_ascii_strtod fail");

		int ret = app_terminate_dead_cb(pid, priv);
		setting_retm_if(ret != 0, "call app_terminate_dead_cb fail");
	}
}

static void setting_phone_ug_on_event(ui_gadget_h ug, enum ug_event event,
									  app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;

	SettingPhoneUG *ad = (SettingPhoneUG *) priv;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_REGION_CHANGE: {
			if (ad->gl_lang_region && setting_view_phone_language_region.is_create) {
				elm_genlist_realized_items_update(ad->gl_lang_region);
				setting_phone_update_item(ad);
			}
		}
		break;
	case UG_EVENT_LANG_CHANGE: {
			ad->lang_change = EINA_TRUE;

			if (ad->gl_lang_region && setting_view_phone_language_region.is_create) {
				elm_genlist_realized_items_update(ad->gl_lang_region);
				setting_phone_update_item(ad);
			}

			setting_navi_items_update(ad->navi_bar);
		}
		break;

	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	default:
		break;
	}
}

static void setting_phone_ug_on_key_event(ui_gadget_h ug,
										  enum ug_key_event event,
										  app_control_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingPhoneUG *ad = (SettingPhoneUG *) priv;

	SETTING_TRACE("event:%d", event);
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
	SettingPhoneUG *phoneUG = calloc(1, sizeof(SettingPhoneUG));
	setting_retvm_if(!phoneUG, -1, "Create SettingPhoneUG obj failed");

	ops->create = setting_phone_ug_on_create;
	ops->start = setting_phone_ug_on_start;
	ops->pause = setting_phone_ug_on_pause;
	ops->resume = setting_phone_ug_on_resume;
	ops->destroy = setting_phone_ug_on_destroy;
	ops->message = setting_phone_ug_on_message;
	ops->event = setting_phone_ug_on_event;
	ops->key_event = setting_phone_ug_on_key_event;
	ops->priv = phoneUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingPhoneUG *phoneUG;
	setting_retm_if(!ops, "ops == NULL");

	phoneUG = ops->priv;
	if (phoneUG) {
		FREE(phoneUG);
	}
}

/************* n-depth search **************/
static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{"IDS_ST_HEADER_DISPLAY_LANGUAGE", NULL, "viewtype:language;caller:setting", Cfg_Item_Pos_Level0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_REGION", NULL, "viewtype:region", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
	{"IDS_ST_BODY_KEYBOARD", NULL, "viewtype:language", 0, 0, 0, Cfg_Item_View_Node, NULL, NULL, NULL, NULL},
};


UG_MODULE_API int setting_plugin_search_init(app_control_h service, void *priv, char **applocale)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE(">> setting-phone-efl DB search code");
	setting_retvm_if(!priv || !applocale, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "pplist/applocale is NULL");

	*applocale = strdup("setting:"_TZ_SYS_RO_APP"/org.tizen.setting/res/locale");

	Eina_List **pplist = (Eina_List **)priv;
	int i;
	int size = sizeof(s_cfg_node_array) / sizeof(s_cfg_node_array[0]);
	for (i = 0; i < size; i++) {
		Setting_Cfg_Node_T *node = setting_plugin_search_item_subindex_add(s_cfg_node_array[i].key_name, s_cfg_node_array[i].ug_args, IMG_LanguageInput, s_cfg_node_array[i].item_type,	 s_cfg_node_array[i].data, "Language and Region");
		*pplist = eina_list_append(*pplist, node);
	}
	return 0;
}

int set_language_helper(const char *lang)
{
	int ret = system_settings_set_value_string(SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, lang);
	return ret;
}

int set_regionformat_helper(const char *region)
{
	int ret = system_settings_set_value_string(SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY, region);
	return ret;
}

