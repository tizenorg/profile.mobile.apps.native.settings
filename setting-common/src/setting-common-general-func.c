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

#include <setting-common-general-func.h>
#include <setting-common-data-slp-setting.h>

#include <glib.h>
#include <dlfcn.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <system_info.h>

#include <system_settings.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include <pkgmgr-info.h>

#include <sys/stat.h>
#include <sys/mount.h>

/*#include <data_usage.h> */
/*#include <resourced.h> */
#include <setting-cfg.h>
#include <player.h>

#include <bundle.h>
#include <bundle_internal.h>
#include <app_control_internal.h>

EXPORT_PUBLIC
char *setting_file_basename(char *path)
{
	if (NULL == path || '\0' == path[0]) {
		return NULL; /* invalid arguement */
	}
	char *p = strrchr(path, '/');
	if (!p) {
		return (char *)g_strdup(path); /*	cannot find '/' */
	}
	if ('\0' == p[1]) {
		return NULL; /* end with '/' */
	}
	return (char *)g_strdup(p + 1);
}

/*********************
 **Function name:	setting_file_attr_is_valid_name
 **Parameter:
 **	const char *filename:	the file/dir name we need to check
 **
 **Return value:
 **	-1	if the name is invalid
 **	0		if the name is valid
 **
 **Action:
 **	check if the name is valid by file name
 **
 *********************/
EXPORT_PUBLIC
int setting_file_attr_is_valid_name(const char *filename)
{
	char *pattern;
	int ret, z, cflags = 0;
	char ebuf[128];
	regex_t reg;
	regmatch_t pm[1];
	const size_t nmatch = 1;
	/*ToDo: ignore the file star with . */
	if (strncmp(filename, ".", 1) == 0) {
		return -1;
	}

	pattern = SETTING_FILE_NAME_PATTERN;
	z = regcomp(&reg, pattern, cflags);

	if (z != 0) {
		regerror(z, &reg, ebuf, sizeof(ebuf));
		fprintf(stderr, "%s: pattern '%s' \n", ebuf, pattern);
		return -1;
	}

	z = regexec(&reg, filename, nmatch, pm, 0);
	if (z == REG_NOMATCH) {
		ret = 0;
	} else {
		ret = -1;
	}
	regfree(&reg);
	return ret;
}

EXPORT_PUBLIC
char *get_pa_usb_connect_mode_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value;

	ret = vconf_get_int(VCONFKEY_SETAPPL_USB_MODE_INT, &value);
	/* file system exceptional handle */
	setting_retvm_if(SETTING_RETURN_FAIL == ret, NULL,
			"Failed to get vconf value");

	switch (value) {
	case SETTING_USB_DEBUG_MODE:
		return (char *)g_strdup(_("IDS_ST_BODY_USB_DEBUGGING"));
	default:
		return (char *)g_strdup(_("IDS_ST_BODY_DISCONNECTED"));
	}
}

EXPORT_PUBLIC
char *get_brightness_mode_str()
{
	int value, err;
	setting_get_int_slp_key(INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, &value,
			&err);
	if (value != SETTING_BRIGHTNESS_AUTOMATIC_OFF) {
		return "IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC";
	} else {
		return "IDS_ST_BODY_MANUAL";
	}
}

#define MAX_SCREEN_MODE_NUM 4
EXPORT_PUBLIC
char *get_pa_screen_mode_str()
{
	const char *str_in_arr[MAX_SCREEN_MODE_NUM] = {
		"Dynamic",
		"Standard",
		"Natural",
		"Movie" };
	const char *str_out_arr[MAX_SCREEN_MODE_NUM] = {
		"IDS_ST_BODY_DYNAMIC_T_DISPLAY_EFFECT",
		"IDS_ST_BODY_STANDARD_T_DISPLAY_EFFECT",
		"IDS_ST_BODY_NATURAL_T_DISPLAY_EFFECT",
		"IDS_ST_BODY_MOVIE_T_DISPLAY_EFFECT" };

	char *curmode = NULL;
	curmode = vconf_get_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME);

	int screen_mode = -1;
	char *ret_str = NULL;
	if ((curmode == NULL) || (safeStrLen(curmode) < 5)) {
		SETTING_TRACE(">>> CUR SCREEN MODE : NULL ");
		if (vconf_get_int("db/private/sysman/enhance_mode",
				&screen_mode) != 0) {
			SETTING_TRACE_DEBUG(
					"vconf_get_int() returns an error.");
		}
		SETTING_TRACE_DEBUG("screen_mode : %d", screen_mode);
		if (screen_mode >= 0) {
			ret_str = (char *)str_out_arr[screen_mode];
			vconf_set_str(VCONFKEY_SETAPPL_SCREENMODE_SELNAME,
					str_in_arr[screen_mode]);
		}
	} else {
		SETTING_TRACE_DEBUG(">>> CUR SCREEN MODE : %s ", curmode);
		/* choose one style */
		int i = 0;
		for (; i < MAX_SCREEN_MODE_NUM; i++) {
			if (0 == safeStrCmp(str_in_arr[i], curmode)) {
				ret_str = (char *)str_out_arr[i];
				break;
			}
		}
		FREE(curmode);
	}
	SETTING_TRACE("_(ret_str):%s", _(ret_str));
	return (char *)g_strdup(_(ret_str));
}

EXPORT_PUBLIC
char *get_pa_backlight_time_str()
{
	int value = 0;
	char backlight_time_str[MAX_DISPLAY_NAME_LEN_ON_UI];

#if SUPPORT_LCD_TIMEOUT_KEEPING
	if (0 != vconf_get_int(VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, &value)) {
		/* value = 600; */
		if (isEmulBin())
		value = 0; /* keep agree with postin file */
		else
		value = 30;
		vconf_set_int(VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, value);
	}
#else
	int err;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL,
			&value, &err);

#if NORMAL
	if (SETTING_RETURN_FAIL == ret || value < 0) {
		/* file system exceptional handle, eg.
		 * The vconf file lost due to file system. */
		/* value = 600; */
		if (isEmulBin())
		value = 0; /* keep agree with postin file */
		else
		value = 30;
		setting_set_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL,
				value, &err);
	}
#else /* for MDM server */
	/* adjust value. */
	/* if < 15, 15 */
	/* if 15 < < 30, 15 */
	/* if 30 < < 60, 30 */
	/* if 60 < < 120, 60 */
	/* if 120 < < 300, 120 */
	/* if 300 < < 600, 300 */
	/* if > 600, 600 */
	if (isEmulBin()) {
		if (SETTING_RETURN_FAIL == ret || value < 15)
			value = 0;
		else if (value >= 15 && value < 30)
			value = 15;
		else if (value >= 30 && value < 60)
			value = 30;
		else if (value >= 60 && value < 120)
			value = 60;
		else if (value >= 120 && value < 300)
			value = 120;
		else if (value >= 300 && value < 600)
			value = 300;
		else
			value = 600;
	} else {
		if (SETTING_RETURN_FAIL == ret || value < 30)
			value = 15;
		else if (value >= 30 && value < 60)
			value = 30;
		else if (value >= 60 && value < 120)
			value = 60;
		else if (value >= 120 && value < 300)
			value = 120;
		else if (value >= 300 && value < 600)
			value = 300;
		else
			value = 600;
	}

	setting_set_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL, value,
			&err);
#endif
#endif
	switch (value) {
	case 0:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_Always_On));
		break;
	case 15:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_15_SEC_STR));
		break;
	case 30:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_30_SEC_STR));
		break;
	case 60:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_1_MIN_STR));
		break;
	case 120:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_2_MIN_STR));
		break;
	case 300:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_5_MIN_STR));
		break;
	case 600:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_10_MIN_STR));
		break;
	default:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_30_SEC_STR));
		break;
	}
	return (char *)g_strdup(backlight_time_str);
}

EXPORT_PUBLIC
int adjust_backlight_time_value(int value)
{
#if NORMAL
	/* value = 600; */
	if (isEmulBin())
	value = 0; /* keep agree with postin file */
	else
	value = 30;
#else /* for MDM server */
	/* adjust value. */
	/* if < 15, 15 */
	/* if 15 < < 30, 15 */
	/* if 30 < < 60, 30 */
	/* if 60 < < 120, 60 */
	/* if 120 < < 300, 120 */
	/* if 300 < < 600, 300 */
	/* if > 600, 600 */
	if (isEmulBin()) {
		if (value < 15)
			value = 0;
		else if (value >= 15 && value < 30)
			value = 15;
		else if (value >= 30 && value < 60)
			value = 30;
		else if (value >= 60 && value < 120)
			value = 60;
		else if (value >= 120 && value < 300)
			value = 120;
		else if (value >= 300 && value < 600)
			value = 300;
		else
			value = 600;
	} else {
		if (value < 30)
			value = 15;
		else if (value >= 30 && value < 60)
			value = 30;
		else if (value >= 60 && value < 120)
			value = 60;
		else if (value >= 120 && value < 300)
			value = 120;
		else if (value >= 300 && value < 600)
			value = 300;
		else
			value = 600;
	}
#endif
	return value;
}

EXPORT_PUBLIC
char *get_pa_backlight_time_value_str()
{

	int value = 0;
	char backlight_time_str[MAX_DISPLAY_NAME_LEN_ON_UI];
	int err;

	setting_get_int_slp_key(INT_SLP_SETTING_LCD_TIMEOUT_NORMAL, &value,
			&err);

	value = adjust_backlight_time_value(value);

	switch (value) {
	case 0:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_Always_On));
		break;
	case 15:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_15_SEC_STR));
		break;
	case 30:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_30_SEC_STR));
		break;
	case 60:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_1_MIN_STR));
		break;
	case 120:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_2_MIN_STR));
		break;
	case 300:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_5_MIN_STR));
		break;
	case 600:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_10_MIN_STR));
		break;
	default:
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_(KeyStr_Backlight_30_SEC_STR));
		break;
	}
	return (char *)g_strdup(backlight_time_str);

}

EXPORT_PUBLIC
char *format_backlight_time_str(char *backligt_time)
{

	char backlight_time_str[MAX_DISPLAY_NAME_LEN_ON_UI];
	int value = atoi(backligt_time);
	SETTING_TRACE("backligt_time:%s--value:%d", backligt_time, value);
	if (value == 0) {
		snprintf(backlight_time_str, sizeof(backlight_time_str), "%s",
				_("IDS_ST_BODY_ALWAYS_ON"));
	} else if (value == 60) {
		snprintf(backlight_time_str, sizeof(backlight_time_str),
				"%d %s", 1, (char *)(_("IDS_CLD_OPT_MINUTE")));
	} else if (value > 60) {
		snprintf(backlight_time_str, sizeof(backlight_time_str),
				"%d %s", value / 60,
				(char *)(_("IDS_CLD_OPT_MINUTES_LC_ABB")));
	} else {
		snprintf(backlight_time_str, sizeof(backlight_time_str),
				"%d %s", value,
				(char *)(_("IDS_ST_BODY_SECONDS")));
	}
	SETTING_TRACE("backlight_time_str:%s", backlight_time_str);
	return (char *)g_strdup(backlight_time_str);
}

EXPORT_PUBLIC
char *get_pa_display_language_str()
{
	int ret = SETTING_RETURN_FAIL;
	/*int err; */
	/*int index; */
	int lang_automatic;

	ret = vconf_get_bool(VCONFKEY_SETAPPL_LANG_AUTOMATIC_BOOL,
			&lang_automatic);
	SETTING_TRACE_DEBUG("vconf_get_bool() returns %d", ret);
	if (lang_automatic == 1) {
		return (char *)g_strdup(setting_gettext(
				"IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"));
	} else {
		Eina_List *list = setting_get_language_list();
		Eina_List *elist = NULL;

		const char *locale = NULL;
		locale = vconf_get_str(VCONFKEY_LANGSET);
		if (locale == NULL)
			return g_strdup("Error");

		char *q = strchr(locale, '.');
		if (q) {
			/*SETTING_TRACE_DEBUG("find (.) char"); */
			*q = '\0';
		}
		/*SETTING_TRACE_DEBUG("current locale : %s", locale); */

		setting_lang_entry *pnode;

		char *title = NULL;
		EINA_LIST_FOREACH(list, elist, pnode)
		{
			/*SETTING_TRACE_DEBUG("pnode->locale : %s",
			 * pnode->locale); */
			if (strcmp(pnode->locale, locale) == 0)
				title = g_strdup(pnode->title);
		}
		FREE(locale);
		return title;
	}
}

EXPORT_PUBLIC
char *get_pa_Wi_Fi_on_off_str()
{
	int value, err;
	int ret = SETTING_RETURN_FAIL;
	ret = setting_get_int_slp_key(INT_SLP_SETTING_WIFI_STATUS, &value,
			&err);

	if (SETTING_RETURN_FAIL == ret) {
		/* file system exceptional handle,
		 * eg. The vconf file lost due to file system. */
		value = VCONFKEY_WIFI_OFF;
		setting_set_int_slp_key(INT_SLP_SETTING_WIFI_STATUS,
				VCONFKEY_WIFI_OFF, &err);
	}

	if (value) {
		char *pa_wifi_device = vconf_get_str(
				VCONFKEY_WIFI_CONNECTED_AP_NAME);
		SETTING_TRACE("pa_wifi_device:%s", pa_wifi_device);
		if (NULL != pa_wifi_device && '\0' != pa_wifi_device[0]) {
			return pa_wifi_device;
		}

		return (char *)strdup(setting_gettext("IDS_ST_BODY_ON"));
	} else {
		return (char *)strdup(
				setting_gettext("IDS_ST_BODY_ALERTTYPE_OFF"));
	}
}

EXPORT_PUBLIC
char *get_Mobile_AP_on_off_str()
{
	int value = -1, err;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_MOBILE_AP_STATUS,
			&value, &err);

	/*	7 means bitwise-operation */
	/*
	 - 1 : wi-fi
	 - 2 : bluetooth
	 - 4 : USB
	 4, 2, 1 = 7
	 */
	if (0 != ret || value < 0 || value > 7) {
		value = 0;
		setting_set_int_slp_key(INT_SLP_SETTING_MOBILE_AP_STATUS, value,
				&err);
	}

	if (value != 0) {
		return setting_gettext("IDS_ST_BODY_ON");
	} else { /*	0 */
		return setting_gettext("IDS_ST_BODY_ALERTTYPE_OFF");
	}
}

EXPORT_PUBLIC
char *get_BT_on_off_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value, err;
	ret = setting_get_int_slp_key(INT_SLP_SETTING_BT_STATUS, &value, &err);

	if (SETTING_RETURN_FAIL == ret) {
		/* file system exceptional handle,
		 * eg. The vconf file lost due to file system. */
		value = VCONFKEY_BT_STATUS_OFF;
		setting_set_int_slp_key(INT_SLP_SETTING_BT_STATUS,
				VCONFKEY_BT_STATUS_OFF, &err);
	}

	if (VCONFKEY_BT_STATUS_OFF == value) {
		return setting_gettext("IDS_ST_BODY_ALERTTYPE_OFF");
	} else {
		return setting_gettext("IDS_ST_BODY_ON");
	}
}

EXPORT_PUBLIC
char *get_NFC_on_off_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value = 0, err = 0;
	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_NFC_STATUS, &value,
			&err);

	if (SETTING_RETURN_FAIL == ret) {
		/* file system exceptional handle,
		 * eg. The vconf file lost due to file system. */
		SETTING_TRACE_DEBUG("fail to get nfc status from vconf");
		value = VCONFKEY_NFC_STATE_OFF;
	}

	if (VCONFKEY_NFC_STATE_OFF == value) {
		return setting_gettext("IDS_ST_BODY_ALERTTYPE_OFF");
	} else {
		return setting_gettext("IDS_ST_BODY_ON");
	}
}

EXPORT_PUBLIC
char *get_pa_time_format_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value, err;
	char *format_str[] = { "IDS_ST_BODY_12_HOURS", "IDS_ST_BODY_24_HOURS" };
	ret = setting_get_int_slp_key(INT_SLP_SETTING_REGIONFORMAT_TIME1224,
			&value, &err);
	/* file system exceptional handle */
	setting_retvm_if(SETTING_RETURN_FAIL == ret, NULL,
			"Failed to get vconf value");

	/*	scope */
	/*	value 1:12H	 2:24H */
	if (value > 0 && value <= 2) {
		/*	1, 2 */
		return (char *)g_strdup(format_str[value - 1]);
	} else {
		/* set to 12H compelsery */
		return (char *)g_strdup(format_str[0]);
	}
}

/** @deprecated */
EXPORT_PUBLIC char *get_pa_date_format_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value, err;
	char *format_str[] = { _("IDS_ST_BODY_DDMMYYYY_DOT"),
		_("IDS_ST_BODY_MM_DD_YYYY_DOT"),
		_("IDS_ST_BODY_YYYY_MM_DD_DOT"),
		_("IDS_ST_BODY_YYYY_DD_MM_DOT") };
	ret = setting_get_int_slp_key(INT_SLP_SETTING_DATE_FORMAT, &value,
			&err);
	/* file system exceptional handle */
	setting_retvm_if(SETTING_RETURN_FAIL == ret, NULL,
			"Failed to get vconf value");
	return (char *)g_strdup(format_str[value]);
}

static inline const char *_check_language_env_value(const char *value)
{
	retv_if(NULL == value, NULL);

	switch (*value) {
	case 'a':
		if (0 == strcmp(value, "ar"))
			return "ar";
		else if (0 == strcmp(value, "az"))
			return "az";
		break;
	case 'b':
		if (0 == strcmp(value, "bg"))
			return "bg";
		break;
	case 'c':
		if (0 == strcmp(value, "ca"))
			return "ca";
		else if (0 == strcmp(value, "cs"))
			return "cs";
		break;
	case 'd':
		if (0 == strcmp(value, "da"))
			return "da";
		else if (0 == strcmp(value, "de_DE"))
			return "de_DE";
		break;
	case 'e':
		if (0 == strcmp(value, "el_GR"))
			return "el_GR";
		else if (0 == strcmp(value, "en"))
			return "en";
		else if (0 == strcmp(value, "en_PH"))
			return "en_PH";
		else if (0 == strcmp(value, "en_US"))
			return "en_US";
		else if (0 == strcmp(value, "es_ES"))
			return "es_ES";
		else if (0 == strcmp(value, "es_MX"))
			return "es_MX";
		else if (0 == strcmp(value, "et"))
			return "et";
		else if (0 == strcmp(value, "eu"))
			return "eu";
		break;
	case 'f':
		if (0 == strcmp(value, "fi"))
			return "fi";
		else if (0 == strcmp(value, "fr_CA"))
			return "fr_CA";
		else if (0 == strcmp(value, "fr_FR"))
			return "fr_FR";
		break;
	case 'g':
		if (0 == strcmp(value, "ga"))
			return "ga";
		else if (0 == strcmp(value, "gl"))
			return "gl";
		break;
	case 'h':
		if (0 == strcmp(value, "hi"))
			return "hi";
		else if (0 == strcmp(value, "hr"))
			return "hr";
		else if (0 == strcmp(value, "hu"))
			return "hu";
		else if (0 == strcmp(value, "hy"))
			return "hy";
		break;
	case 'i':
		if (0 == strcmp(value, "is"))
			return "is";
		else if (0 == strcmp(value, "it_IT"))
			return "it_IT";
		break;
	case 'j':
		if (0 == strcmp(value, "ja_JP"))
			return "ja_JP";
		break;
	case 'k':
		if (0 == strcmp(value, "ka"))
			return "ka";
		else if (0 == strcmp(value, "kk"))
			return "kk";
		else if (0 == strcmp(value, "ko_KR"))
			return "ko_KR";
		break;
	case 'l':
		if (0 == strcmp(value, "lt"))
			return "lt";
		else if (0 == strcmp(value, "lv"))
			return "lv";
		break;
	case 'm':
		if (0 == strcmp(value, "mk"))
			return "mk";
		break;
	case 'n':
		if (0 == strcmp(value, "nb"))
			return "nb";
		else if (0 == strcmp(value, "nl_NL"))
			return "nl_NL";
		break;
		/*case 'o': */
	case 'p':
		if (0 == strcmp(value, "pl"))
			return "pl";
		else if (0 == strcmp(value, "pt_BR"))
			return "pt_BR";
		else if (0 == strcmp(value, "pt_PT"))
			return "pt_PT";
		break;
		/*case 'q': */
	case 'r':
		if (0 == strcmp(value, "ro"))
			return "ro";
		else if (0 == strcmp(value, "ru_RU"))
			return "ru_RU";
		break;
	case 's':
		if (0 == strcmp(value, "sk"))
			return "sk";
		else if (0 == strcmp(value, "sl"))
			return "sl";
		else if (0 == strcmp(value, "sr"))
			return "sr";
		else if (0 == strcmp(value, "sv"))
			return "sv";
		break;
	case 't':
		if (0 == strcmp(value, "tr_TR"))
			return "tr_TR";
		break;
	case 'u':
		if (0 == strcmp(value, "uk"))
			return "uk";
		else if (0 == strcmp(value, "uz"))
			return "uz";
		break;
		/*case 'v': */
		/*case 'w': */
		/*case 'x': */
		/*case 'y': */
	case 'z':
		if (0 == strcmp(value, "zh_CN"))
			return "zh_CN";
		else if (0 == strcmp(value, "zh_HK"))
			return "zh_HK";
		else if (0 == strcmp(value, "zh_SG"))
			return "zh_SG";
		else if (0 == strcmp(value, "zh_TW"))
			return "zh_TW";
		break;
	default:
		break;
	}

	return NULL;
}

EXPORT_PUBLIC
char *get_pa_datetime_format_str()
{
	const char *clean_lang;
	char *dt_fmt, *region_fmt, *lang = NULL;
	char buf[256] = { 0, };
	int time_val = 0, is_hour24 = 0, ret;

	lang = getenv("LANGUAGE");
	setenv("LANGUAGE", "en_US", 1);

	region_fmt = vconf_get_str(VCONFKEY_REGIONFORMAT);
	ret = vconf_get_int(VCONFKEY_REGIONFORMAT_TIME1224, &time_val);
	if (ret < 0)
		is_hour24 = 0;
	else if (time_val == VCONFKEY_TIME_FORMAT_12
			|| time_val == VCONFKEY_TIME_FORMAT_24)
		is_hour24 = time_val - 1;

	if (is_hour24)
		snprintf(buf, sizeof(buf), "%s_DTFMT_24HR", region_fmt);
	else
		snprintf(buf, sizeof(buf), "%s_DTFMT_12HR", region_fmt);

	dt_fmt = dgettext("dt_fmt", buf);

	clean_lang = _check_language_env_value(lang);

	if (NULL == clean_lang)
		unsetenv("LANGUAGE");
	else
		setenv("LANGUAGE", clean_lang, 1);

	return strdup(dt_fmt);
}

EXPORT_PUBLIC
char *get_pa_week_format_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value, err;
	char *format_str[] = { _("IDS_ST_BODY_SUNDAY"), _("IDS_ST_BODY_MONDAY"),
	};

	ret = setting_get_int_slp_key(INT_SLP_SETTING_WEEK_FORMAT, &value,
			&err);
	/* file system exceptional handle */
	setting_retvm_if(SETTING_RETURN_FAIL == ret, NULL,
			"Failed to get vconf value");
	return (char *)g_strdup(format_str[value]);
}

EXPORT_PUBLIC
char *get_pa_roaming_network_str()
{
	int ret = SETTING_RETURN_FAIL;
	int value, err;
	char *roaming_str[] = {
		"IDS_ST_BODY_AUTO_DOWNLOAD",
		"IDS_ST_BODY_MANUAL",
		"IDS_ST_BODY_ALWAYS_REJECT" };
	ret = setting_get_int_slp_key(INT_SLP_SETTING_ROAMING_NETWORK, &value,
			&err);
	/* file system exceptional handle */
	setting_retvm_if(SETTING_RETURN_FAIL == ret, NULL,
			"Failed to get vconf value");
	return (char *)g_strdup(roaming_str[value]);
}

EXPORT_PUBLIC
char *get_pa_select_network()
{
	int ret = 0;
	int state = 0;

	char *net_name = NULL;
	int service_type = VCONFKEY_TELEPHONY_SVCTYPE_SEARCH;
	if (vconf_get_int(VCONFKEY_TELEPHONY_SVCTYPE, &service_type) != 0) {
		SETTING_TRACE("fail to get VCONFKEY_TELEPHONY_SVCTYPE");
	}

	retv_if(service_type == VCONFKEY_TELEPHONY_SVCTYPE_NOSVC,
			(char *)strdup(_("IDS_ST_BODY_NOSERVICE")));
	retv_if(service_type == VCONFKEY_TELEPHONY_SVCTYPE_EMERGENCY,
			(char *)strdup(_("IDS_CALL_POP_CALLING_EMERG_ONLY")));
	retv_if(service_type == VCONFKEY_TELEPHONY_SVCTYPE_SEARCH
			|| service_type == VCONFKEY_TELEPHONY_SVCTYPE_NONE,
			(char *)strdup(_("IDS_ST_POP_NETWORKSERVICES_SEARCHING")
					));

	ret = vconf_get_int(VCONFKEY_TELEPHONY_SPN_DISP_CONDITION, &state);
	if (ret == 0) {
		/*SETTING_TRACE("VCONFKEY(%s) = %d",
		 * VCONFKEY_TELEPHONY_SPN_DISP_CONDITION, state); */
		char name[MAX_COMMON_BUFFER_LEN] = { 0, };
		const char *kv = NULL;

		if (state != VCONFKEY_TELEPHONY_DISP_INVALID) {
			/*counting. */
			int i = 0;
			if (state & VCONFKEY_TELEPHONY_DISP_SPN) {
				kv = vconf_get_str(VCONFKEY_TELEPHONY_SPN_NAME);
				g_strlcat(name, kv, MAX_COMMON_BUFFER_LEN);
				FREE(kv);
				i++;
			}

			if (state & VCONFKEY_TELEPHONY_DISP_PLMN) {
				if (i != 0)
					g_strlcat(name, " - ",
							MAX_COMMON_BUFFER_LEN);

				kv = vconf_get_str(VCONFKEY_TELEPHONY_NWNAME);
				g_strlcat(name, kv, MAX_COMMON_BUFFER_LEN);
				FREE(kv);
				i++;
			}
			net_name = isEmptyStr(name) ? NULL :
					(char *)strdup(name);
		}
	}
	return net_name;
}

EXPORT_PUBLIC
char *get_pa_screen_lock_type_str()
{
	int lock_type = 0;

	vconf_get_int(VCONFKEY_SETAPPL_SCREEN_LOCK_TYPE_INT, &lock_type);

	switch (lock_type) {
	case SETTING_SCREEN_LOCK_TYPE_NONE:
		return setting_gettext("IDS_ST_MBODY_NONE");
	case SETTING_SCREEN_LOCK_TYPE_SWIPE:
		return setting_gettext("IDS_ST_MBODY_SWIPE");
	case SETTING_SCREEN_LOCK_TYPE_MOTION:
		return setting_gettext("IDS_ST_BODY_MOTION");
	case SETTING_SCREEN_LOCK_TYPE_FACE_AND_VOICE:
		return setting_gettext("IDS_ST_BODY_FACE_AND_VOICE");
	case SETTING_SCREEN_LOCK_TYPE_SIMPLE_PASSWORD:
		return setting_gettext("IDS_ST_BODY_SIMPLE_PASSWORD");
	case SETTING_SCREEN_LOCK_TYPE_PASSWORD:
		return setting_gettext("IDS_ST_BODY_PASSWORD");
	case SETTING_SCREEN_LOCK_TYPE_OTHER:
		return vconf_get_str(VCONFKEY_SETAPPL_3RD_LOCK_PKG_NAME_STR);
	default:
		return NULL;
	}
}

EXPORT_PUBLIC
double get_widgets_factor()
{
	return elm_config_scale_get();
}

EXPORT_PUBLIC char *setting_gettext(const char *s)
{
	/* fisrt find in app pg */

	if (s == NULL) {
		return "NULL";
	}

	char *p = dgettext(SETTING_PACKAGE, s);

	if (!safeStrCmp(s, p)) { /* not found */
		/* find in system pkg */
		p = dgettext(SYSTEM_PACKAGE, s);
	}
	return p;
}

EXPORT_PUBLIC char *modesyspopup_gettext(const char *s)
{
	/* fisrt find in app pg */

	if (s == NULL) {
		return "NULL";
	}

	char *p = dgettext(MODE_SYSPOPUP_PACKAGE, s);

	if (!safeStrCmp(s, p)) { /* not found */
		/* find in system pkg */
		p = dgettext(SYSTEM_PACKAGE, s);
	}
	return p;
}

EXPORT_PUBLIC char *setting_gettext2(const char *s, const char *domainname)
{
	/* fisrt find in app pg */

	if (s == NULL) {
		return "NULL";
	}

	char *p = dgettext(domainname, s);

	if (!safeStrCmp(s, p)) { /* not found */
		/* find in system pkg */
		p = dgettext(SYSTEM_PACKAGE, s);
	}
	return p;
}

EXPORT_PUBLIC bool is_digital_str(const char *cstr)
{
	if (cstr == NULL || cstr[0] == 0) {
		return FALSE;
	}

	int len = (int)(safeStrLen(cstr));
	int pos = 0;
	if (cstr[0] == '-' || cstr[0] == '+') {
		if (len <= 1) {
			return FALSE;
		}
		pos++;
	}

	while (pos < len) {
		if (cstr[pos] < '0' || cstr[pos] > '9') {
			return FALSE;
		}
		pos++;
	}

	return TRUE;
}

EXPORT_PUBLIC
bool is_substr_ncase(const char *parentstr, const char *substr)
{
	if (NULL == parentstr || '\0' == parentstr[0])
		return FALSE;

	int word_len = safeStrLen(parentstr);
	int search_len = safeStrLen(substr);
	int i;
	bool result = FALSE;
	for (i = 0; i < word_len; i++) {
		if (!strncasecmp(substr, &parentstr[i], search_len)) {
			result = TRUE;
			break;
		}
	}
	return result;
}

EXPORT_PUBLIC
bool is_string_belong_to_array(const char *partern, const char **array,
		int array_num)
{
	int idx = 0;
	for (; idx < array_num; idx++) {
		if (!safeStrCmp(partern, array[idx])) {
			return TRUE;
		}
	}
	return FALSE;
}

/*remove sub string from a parent string */
EXPORT_PUBLIC char *remove_first_substring(const char *parent, const char *pat)
{
	setting_retvm_if(!parent, NULL,
			"NULL == parent, Exit %s with return NULL",
			__FUNCTION__);
	/*to process the case parent pointing to const string */
	char *str = strdup(parent);
	setting_retvm_if(!pat, str, "NULL == pat, Exit %s with return [%s]",
			__FUNCTION__, str);

	int idx = 0;
	int str_len = safeStrLen(str);
	int pat_len = safeStrLen(pat);
	setting_retvm_if(pat_len > str_len, str,
			"patlen[%d] > strlen[%d], Exit %s with return [%s]",
			pat_len, str_len, __FUNCTION__, str);
	char *p = NULL;
	char *q = NULL;
	for (; idx < str_len; idx++) {
		if (0 == safeStrNCmp(pat, str + idx, pat_len)) {
			p = str + idx;
			q = str + idx + pat_len;
			while ('\0' != *q) {
				*p = *q;
				p++;
				q++;
			}
			*p = '\0';
			break;
		}
	}

	/*now str keeps the result string */
	SETTING_TRACE("Exit %s with return str:%s", __FUNCTION__, str);
	return str;
}

/*return True just when NULL or '\0' */
EXPORT_PUBLIC
bool isEmptyStr(const char *str)
{
	if (NULL == str || '\0' == str[0])
		return TRUE;
	return FALSE;
}

/*return True just when str is NULL, '\0' or all strings made up of spaces */
EXPORT_PUBLIC
bool isSpaceStr(const char *str)
{
	/*if (NULL == str) */
	/*	return TRUE; */
	while (str) {
		if (*str != '\0' && *str != ' ') {
			return FALSE;
		} else if (*str == '\0') {
			return TRUE;
		}
		str++;
	}
	return TRUE;
}

int EXPORT_PUBLIC safeStrCmp(const char *s1, const char *s2)
{
	/*	Check NULL value first */
	if (isEmptyStr(s1) && isEmptyStr(s2)) {
		return 0;
	} else if (isEmptyStr(s1)) {
		return 1;
	} else if (isEmptyStr(s2)) {
		return SETTING_RETURN_FAIL;
	}

	return strcmp(s1, s2);
}

EXPORT_PUBLIC
int safeStrNCmp(const char *s1, const char *s2, int len)
{

	/*	Check NULL value first */
	if (isEmptyStr(s1) && isEmptyStr(s2)) {
		return 0;
	} else if (isEmptyStr(s1)) {
		return 1;
	} else if (isEmptyStr(s2)) {
		return SETTING_RETURN_FAIL;
	}

	if (0 == len) {
		return 0;
	}

	return strncmp(s1, s2, len);
}

EXPORT_PUBLIC
char *safeStrNCat(char *dst, const char *src, int maxlen)
{
	if (dst && !isEmptyStr(src) && maxlen > 0) {
		(void)g_strlcat(dst, src, maxlen + 1);
	}

	return dst;
}

EXPORT_PUBLIC
char *safeCopyStr(char *dst, const char *src, int maxlen)
{
	if (maxlen < 0) {
		return NULL;
	}

	if (dst) {
		int len = 0;

		if (src) {
			int temp = (int)safeStrLen(src);
			len = (temp <= maxlen) ? temp : maxlen;
			memcpy(dst, src, len);
		}
		dst[len] = '\0';
	}
	return dst;
}

EXPORT_PUBLIC
int safeStrLen(const char *str)
{
	if (isEmptyStr(str)) {
		SETTING_TRACE_DEBUG("string is empty");
		return 0;
	} else {
		return strlen(str);
	}
}

EXPORT_PUBLIC char *safeStrStr(const char *str, const char *strSearch)
{
	if (isEmptyStr(str))
		return NULL;

	return strstr(str, strSearch);
}

EXPORT_PUBLIC char *safeStrRStr(const char *str, const char *strSearch)
{
	if (isEmptyStr(str))
		return NULL;

	char *ptr = (char *)str;
	char *last = NULL;
	while ((ptr = strstr(ptr, strSearch))) {
		last = ptr;
		ptr++;
	}

	return last;
}

EXPORT_PUBLIC char *multilanguage_text_upper(const char *s)
{
	/* fisrt find in app pg */
	SETTING_TRACE_BEGIN;
	return isEmptyStr(s) ? NULL : (char *)g_utf8_strup(s, safeStrLen(s));
}

/**
 * get the int value of substring before delim
 */
EXPORT_PUBLIC bool get_substring_int(const char **ipStr, int *ipValue,
		char delim)
{
	int iValue = *ipValue = 0;
	const char *str = *ipStr;

	if (str == NULL || str[0] == 0) { /* empty string */
		return FALSE;
	}

	bool bNegative = FALSE;
	if ('-' == str[0]) { /* allow Negative number.. */
		bNegative = TRUE;
		str++;
	}
	if (str[0] < '0' || str[0] > '9') { /* first elementy is not digital */
		*ipStr = str;
		return FALSE;
	}
	if (str[0] == '0') { /* first elementy is 0 */
		if (str[1] == '\0') {
			str++;
			*ipStr = str;
			return TRUE;
		} else if (str[1] == delim) {
			str += 2;
			*ipStr = str;
			return TRUE;
		} else if (str[1] == 0) {
			if (str[2] == 0) {
				str += 3;
				*ipStr = str;
				return TRUE;
			} else if (str[2] == '\0') {
				str += 2;
				*ipStr = str;
				return TRUE;
			} else {
				str += 2;
				/**ipStr = str; */
			}
		} else {
			str++;
			*ipStr = str;

		}
		/*return FALSE; */
	}

	for (;;) {
		/****first elementy is not 0*/
		iValue = iValue * 10 + str[0] - '0';
		*ipValue = iValue;
		/* think about overloading */
		if (((unsigned int)iValue & 0x80000000) != 0) {
			break;
		}
		str++;
		if (str[0] == delim) {
			str++;
			if (bNegative) {
				iValue = -iValue;
			}
			*ipStr = str;
			*ipValue = iValue;
			return TRUE;
		}
		if (str[0] == 0) {
			if (bNegative) {
				iValue = -iValue;
			}
			*ipStr = str;
			*ipValue = iValue;
			return TRUE;
		}
		if (str[0] < '0' || str[0] > '9') {
			break;
		}
	}

	*ipStr = str;
	return FALSE;
}

#define MaxIPAddressLength	15

/**
 * CHeck Whether a special string Is An IP String
 *
 * @param ipstr string representing IP numbers like "aaa.bbb.ccc.ddd"
 * @return true if it's IP format string otherwise false
 */
EXPORT_PUBLIC
bool is_ip_string(const char *ipstr, char **output)
{
	if (NULL == ipstr || 0 == ipstr[0])
		return FALSE;
	int len = (int)safeStrLen(ipstr);
	if (len > MaxIPAddressLength) {
		return FALSE;
	}

	if (ipstr[len - 1] == '.') {
		return FALSE;
	}

	char output_ipstr[MAX_DISPLAY_NAME_LEN_ON_UI] = { 0, };
	char speciliztion[MAX_DISPLAY_NAME_LEN_ON_UI / 4] = { 0, };
	int ip;
	int i = 0;
	for (; i < 4; i++) {
		if (!get_substring_int(&ipstr, &ip, '.') || ip > 255) {
			SETTING_TRACE_ERROR("ipstr:%s", ipstr);
			return FALSE;
		}
		snprintf(speciliztion, sizeof(speciliztion), "%d", ip);
		g_strlcat(output_ipstr, speciliztion, MAX_SPECIALIZITION_LEN);
		if (i < 3)
			g_strlcat(output_ipstr, ".", MAX_SPECIALIZITION_LEN);
	}
	if (ipstr[0] != 0) {
		return FALSE;
	}
	SETTING_TRACE("....output_ipstr:%s", output_ipstr);
	if (output) {
		*output = g_strdup(output_ipstr);
	}

	return TRUE;
}

EXPORT_PUBLIC
int setting_invoke_reset_function(char *ug_name, app_control_h service,
		void *ext)
{
	SETTING_TRACE("Enter %s with ug_name:%s", __FUNCTION__, ug_name);
	int (*reset)(app_control_h pair, void *ptr);
	int ret = OPERATE_LIB_SUCESS;

	/*1.first do exist-checking in /opt/ug/lib */
	char ug_file[PATH_MAX + 1];
	snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH, ug_name);
	struct stat st;
	if (stat(ug_file, &st) != 0) {

		/*2.if it does not exit in /opt/ug/lib, then do exist-checking
		 * in (_TZ_SYS_RO_UG)/lib */
		memset(ug_file, 0x00, PATH_MAX + 1);
		snprintf(ug_file, PATH_MAX, "%s/libug-%s.so",
				SETTING_UG_PATH_USR, ug_name);
		if (stat(ug_file, &st) != 0) {
			/*both not exist,skip it */
			SETTING_TRACE_ERROR(
					" libug-%s.so is *NOT* present, so ski"\
					"p it..\n",
					ug_name);
			return OPERATE_LIB_SUCESS;
		}
	}
	/*------------------------------------------------------------------ */
	void *handle = dlopen(ug_file, RTLD_LAZY);
	if (!handle) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", dlerror());
		return LOAD_LIB_FAILED;
	}

	char *error = NULL;
	reset = dlsym(handle, "setting_plugin_reset");
	if ((error = dlerror()) != NULL) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", error);
		dlclose(handle);
		return UNDEFINED_LIB_SYMBOL;
	}

	if (reset)
		ret = (*reset)(service, ext); /*	 CALL */

	if (ret < 0)
		ret += UNDEFINED_LIB_SYMBOL;/*from -2 on */

	dlclose(handle);
	return ret;
}

EXPORT_PUBLIC
int excuteCmd(char *exec_path, int option_num, ...)
{
	char cmd[MAX_COMMON_BUFFER_LEN + 1] = { 0, };
	snprintf(cmd, MAX_COMMON_BUFFER_LEN, "%s", exec_path);

	va_list args;
	va_start(args, option_num);

	char *para;
	int argno = 0;
	for (; argno < option_num; argno++) {
		para = va_arg(args, char *);
		SETTING_TRACE("Parameter #%d is: %s", argno, para);
		if (para) {
			g_strlcat(cmd, " ", MAX_COMMON_BUFFER_LEN + 1);
			g_strlcat(cmd, para, MAX_COMMON_BUFFER_LEN + 1);
		}
	}

	va_end(args);

	SETTING_TRACE("excute:%s", cmd);
	return system(cmd);
}

EXPORT_PUBLIC
char *get_icon_path(const char *package)
{
	pkgmgrinfo_appinfo_h handle;
	char *icon_path = NULL;
	int r = pkgmgrinfo_appinfo_get_appinfo(package, &handle);
	if (r != PMINFO_R_OK) {
		SETTING_TRACE_ERROR("failed pkgmgrinfo_appinfo_get_appinfo");
		return NULL;
	}

	r = pkgmgrinfo_appinfo_get_icon(handle, &icon_path);
	if (r != PMINFO_R_OK) {
		SETTING_TRACE_ERROR("failed pkgmgrinfo_appinfo_get_icon.");
		pkgmgrinfo_appinfo_destroy_appinfo(handle);
		return NULL;
	}

	SETTING_TRACE_ERROR("package:%s, icon_path:%s", package, icon_path);
	char *ret_str = strdup(icon_path);
	pkgmgrinfo_appinfo_destroy_appinfo(handle);
	handle = NULL;
	return ret_str;
}

/* SLP : 1 */
/* not SLP : 0 */
EXPORT_PUBLIC
int is_slp_binary()
{
	char *str = NULL;
	int ret = system_info_get_platform_string(
			"http://tizen.org/system/build.string", &str);
	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"fail to call system_info_get_platform_string");
		FREE(str);
		return 0;
	}

	char *pos = str;
	if (str) {
		while (*pos++) {
			if ('_' == *pos) {
				*pos = '\0';
				if (!strncmp(str, "SLP", 3)) {
					FREE(str);
					return 1;
				}
			}
		}
	}
	FREE(str);
	return 0;
}

EXPORT_PUBLIC
bool isEmulBin()
{
	char *model_str = NULL;
	int ret = system_info_get_platform_string("tizen.org/system/model_name",
			&model_str);

	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"fail to call system_info_get_platform_string");
		FREE(model_str);
		return FALSE;
	}

	if (0 == safeStrCmp(KeyStr_Emulator, model_str)
			|| 0 == safeStrCmp("EMULATOR", model_str)) {
		FREE(model_str);
		return TRUE;
	} else {
		FREE(model_str);
		return FALSE;
	}
}

EXPORT_PUBLIC
int get_popup_btn_response_type(Evas_Object *obj)
{
	retv_if(obj == NULL, -1);

	POPUP_BTN_RESPONSE_TYPE rsp_type = POPUP_RESPONSE_CANCEL;
	char *btn_str = elm_entry_markup_to_utf8(elm_object_text_get(obj));
	if (btn_str) {
		SETTING_TRACE("btn_str:%s", btn_str);
		if (0 == safeStrCmp(btn_str, _("IDS_CST_BUTTON_CLOSE"))
			/*KeyStr_Try */
			|| 0 == safeStrCmp(btn_str, _("IDS_SAPPS_SK_TRY_ABB"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_OK"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_OK"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_SK_YES"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_STOP"))
			|| 0 == safeStrCmp(btn_str,
					_("IDS_ST_BUTTON_DISCONNECT"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BODY_USE_MOTION"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_TPOP_TRY_AGAIN"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BODY_DELETE"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_HEADER_RESET"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_SK_FORMAT_ABB"))
			|| 0 == safeStrCmp(btn_str,
					_("IDS_ST_HEADER_FORMAT_MVERB_ABB"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_CONTINUE"))
			|| 0 == safeStrCmp(btn_str, _(KeyStr_Save))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_ENABLE"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_DISABLE"))
			|| 0 == safeStrCmp(btn_str, _("IDS_SA_BUTTON_DONE_ABB"))
			|| 0 == safeStrCmp(btn_str, _("IDS_ST_BODY_TURN_OFF"))
			|| 0 == safeStrCmp(btn_str, _("IDS_CLD_BUTTON_EXPORT"))
			|| 0 == safeStrCmp(btn_str,
					_("IDS_SMEMO_BUTTON_IMPORT"))
			|| 0 == safeStrCmp(btn_str,
					_("IDS_ST_HEADER_CLEAR_CACHE_ABB"))
			|| 0 == safeStrCmp(btn_str,
					_("IDS_MTTRL_BUTTON_ENABLE_MOTION"))) {
			rsp_type = POPUP_RESPONSE_OK;
		} else { /*for others, */
			rsp_type = POPUP_RESPONSE_CANCEL;
		}
		FREE(btn_str);
	} else {
		rsp_type = POPUP_RESPONSE_INVALID;
	}
	FREE(btn_str);
	return rsp_type;
}

#define SUPPORT_STATIC_LOAD 1
#if SUPPORT_STATIC_LOAD
static char *system_service_list[MAX_SYSTEM_SERVICE_NUM] = { NULL, };
#endif
EXPORT_PUBLIC
EXPORT_PUBLIC void clear_system_service_data()
{
#if SUPPORT_STATIC_LOAD
	int idx;
	for (idx = 0; idx < MAX_SYSTEM_SERVICE_NUM && system_service_list[idx];
			idx++) {
		FREE(system_service_list[idx]);
	}
#endif
}

/**
 * [API] internal API (system-settings)
 * should use g_free to free return string
 * @see font_size_set()
 * REFACTORING:
 */
EXPORT_PUBLIC
char *cur_font_get()
{

	/*SETTING_TRACE_BEGIN; */
	char *font_name = NULL;
	int retcode = system_settings_get_value_string(
			SYSTEM_SETTINGS_KEY_FONT_TYPE, &font_name);
	if (SYSTEM_SETTINGS_ERROR_NONE != retcode)
		return NULL;
	/*SETTING_TRACE("---------------- system-setting call : (%s) ",
	 * font_name); */
	return font_name;
}

/**
 * [API] system-settings
 * @see font_size_set()
 */
static int __font_size_get()
{
	int font_size = -1;
	int value = -1;
	int err = -1;
	int ret = setting_get_int_slp_key(
			INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE, &value, &err);
	retvm_if(ret != 0, -1, "get vconf failed");

	switch (value) {
	case SYSTEM_SETTINGS_FONT_SIZE_SMALL:
		font_size = SMALL_FONT_DPI;
		break;
	case SYSTEM_SETTINGS_FONT_SIZE_NORMAL:
		font_size = MIDDLE_FONT_DPI;
		break;
	case SYSTEM_SETTINGS_FONT_SIZE_LARGE:
		font_size = LARGE_FONT_DPI;
		break;
	case SYSTEM_SETTINGS_FONT_SIZE_HUGE:
		font_size = HUGE_FONT_DPI;
		break;
	case SYSTEM_SETTINGS_FONT_SIZE_GIANT:
		font_size = GIANT_FONT_DPI;
		break;
	default:
		font_size = MIDDLE_FONT_DPI;
		break;
	}
	return font_size;
}

/**
 * change current font
 * [API] system-settings
 */
EXPORT_PUBLIC void font_config_set(char *font_name)
{
	Eina_List *text_classes = NULL;
	Elm_Text_Class *etc = NULL;
	const Eina_List *l = NULL;
	Eina_List *fo_list = NULL;
	Elm_Font_Overlay *efo = NULL;
	int font_size = __font_size_get();
	int size = 0;

	text_classes = elm_config_text_classes_list_get();

	fo_list = (Eina_List *)elm_config_font_overlay_list_get();

	Eina_List *ll = NULL;
	Eina_List *l_next = NULL;

	Eina_Bool slp_medium_exist = EINA_FALSE;
	Eina_Bool slp_roman_exist = EINA_FALSE;
	Eina_Bool slp_bold_exist = EINA_FALSE;
	Eina_Bool slp_regular_exist = EINA_FALSE;

	/* Tizen */
	Eina_Bool tizen_exist = EINA_FALSE;

	EINA_LIST_FOREACH_SAFE(fo_list, ll, l_next, efo)
	{
		if (!safeStrCmp(efo->text_class, "tizen_medium")) {
			elm_config_font_overlay_set(efo->text_class,
					(const char *)font_name, efo->size);
			slp_medium_exist = EINA_TRUE;
		} else if (!safeStrCmp(efo->text_class, "tizen_roman")) {
			elm_config_font_overlay_set(efo->text_class,
					(const char *)font_name, efo->size);
			slp_roman_exist = EINA_TRUE;
		} else if (!safeStrCmp(efo->text_class, "tizen_bold")) {
			elm_config_font_overlay_set(efo->text_class,
					(const char *)font_name, efo->size);
			slp_bold_exist = EINA_TRUE;
		} else if (!safeStrCmp(efo->text_class, "tizen_regular")) {
			elm_config_font_overlay_set(efo->text_class,
					(const char *)font_name, efo->size);
			slp_regular_exist = EINA_TRUE;
		}

		/* Tizen */
		if (!safeStrCmp(efo->text_class, "tizen")) {
			elm_config_font_overlay_set(efo->text_class,
					(const char *)font_name, efo->size);
			tizen_exist = EINA_TRUE;
		}

	}

	/* if slp_XX do not exist, need to set them, font size is -100(100%) */
	if (slp_medium_exist == EINA_FALSE) {
		elm_config_font_overlay_set("tizen_medium",
				(const char *)font_name, MIDDLE_FONT_DPI);
	}
	if (slp_roman_exist == EINA_FALSE) {
		elm_config_font_overlay_set("tizen_roman",
				(const char *)font_name, MIDDLE_FONT_DPI);
	}
	if (slp_bold_exist == EINA_FALSE) {
		elm_config_font_overlay_set("tizen_bold",
				(const char *)font_name, MIDDLE_FONT_DPI);
	}
	if (slp_regular_exist == EINA_FALSE) {
		elm_config_font_overlay_set("tizen_regular",
				(const char *)font_name, MIDDLE_FONT_DPI);
	}

	/* Tizen */
	if (tizen_exist == EINA_FALSE) {
		elm_config_font_overlay_set("tizen", (const char *)font_name,
				MIDDLE_FONT_DPI);
	}

	elm_config_font_overlay_set("tizen", (const char *)font_name,
			MIDDLE_FONT_DPI);

	/* Tizen */
	elm_config_font_overlay_set("tizen", (const char *)font_name,
			MIDDLE_FONT_DPI);

	EINA_LIST_FOREACH(text_classes, l, etc)
	{
		ll = NULL;

		size = font_size;
		EINA_LIST_FOREACH(fo_list, ll, efo)
		{
			if (!safeStrCmp(etc->name, efo->text_class)) {
				size = efo->size;
			}
		}
		elm_config_font_overlay_set(etc->name, (const char *)font_name,
				size);
	}

	elm_config_font_overlay_apply();
	elm_config_all_flush();
	/*	elm_config_engine_set("software_x11"); //deprecated API */
	elm_config_save();
	elm_config_text_classes_list_free(text_classes);
	text_classes = NULL;

	/* vconf update */
	vconf_set_str(VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME, font_name);
}

EXPORT_PUBLIC char *_get_default_font()
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	xmlNodePtr cur2 = NULL;
	xmlNodePtr cur3 = NULL;
	xmlChar *key = NULL;
	struct _xmlAttr *properties = NULL;
	char *default_font_name = NULL;

	doc = xmlParseFile(SETTING_DEFAULT_FONT_CONF_FILE);

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		xmlFreeDoc(doc);
		doc = NULL;
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"fontconfig")) {
		xmlFreeDoc(doc);
		doc = NULL;
		return NULL;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"match"))) {
			cur2 = cur->xmlChildrenNode;
			while (cur2 != NULL) {
				if ((!xmlStrcmp(cur2->name,
						(const xmlChar *)"edit"))) {
					properties = cur2->properties;
					/*find the "name" property*/
					while (NULL != properties) {
						if (!xmlStrcmp(properties->name,
								(const xmlChar *)"name")) {
							break;
						}
						properties = properties->next;
					}

					/*If the value of "name" property is "family", then get the child node string,
					 it shall be the default font type*/
					if (NULL != properties && !xmlStrcmp(
							properties->children->content,
							(const xmlChar *)"family")) {
						cur3 = cur2->xmlChildrenNode;
						while (cur3 != NULL) {
							if ((!xmlStrcmp(
									cur3->name,
									(const xmlChar *)"string"))) {
								key = xmlNodeListGetString(
										doc,
										cur3->xmlChildrenNode,
										1);
								default_font_name = g_strdup(
										(char *)key);
								xmlFree(key);
								key = NULL;
								xmlFreeDoc(doc);
								doc = NULL;
								return default_font_name;
							}
							cur3 = cur3->next;
						}
					}
				}
				cur2 = cur2->next;
			}
		}
		cur = cur->next;
	}

	xmlFreeDoc(doc);
	doc = NULL;
	return NULL;
}

/**
 * @see setting_font_font_size_list_mouse_up_cb
 */
EXPORT_PUBLIC
void font_size_set()
{
	SETTING_TRACE_BEGIN;
	Eina_List *text_classes = NULL;
	Elm_Text_Class *etc = NULL;
	const Eina_List *l = NULL;
	int font_size = __font_size_get();
	char *font_name = cur_font_get();

	if (font_size == -1) {
		SETTING_TRACE_DEBUG("failed to call font_size_get");
		return;
	}

	/* update all screen */
	text_classes = elm_config_text_classes_list_get();
	EINA_LIST_FOREACH(text_classes, l, etc)
	{
		elm_config_font_overlay_set(etc->name, font_name, font_size);
	}

	elm_config_all_flush();
	/* elm_config_engine_set("software_x11"); //deprecated API */
	elm_config_save();
	elm_config_text_classes_list_free(text_classes);
	text_classes = NULL;
	G_FREE(font_name);
	SETTING_TRACE_END;
}

static int _set;
EXPORT_PUBLIC void update_lang(void)
{
	char *lang;
	char *r;

	lang = vconf_get_str(VCONFKEY_LANGSET);
	if (lang) {
		setenv("LANG", lang, 1);
		setenv("LC_MESSAGES", lang, 1);
		r = setlocale(LC_ALL, "");
		if (r == NULL) {
			setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
		}
		free(lang);
	}
}

EXPORT_PUBLIC void update_region(void)
{
	char *region;

	region = vconf_get_str(VCONFKEY_REGIONFORMAT);
	if (region) {
		setenv("LC_CTYPE", region, 1);
		setenv("LC_NUMERIC", region, 1);
		setenv("LC_TIME", region, 1);
		setenv("LC_COLLATE", region, 1);
		setenv("LC_MONETARY", region, 1);
		setenv("LC_PAPER", region, 1);
		setenv("LC_NAME", region, 1);
		setenv("LC_ADDRESS", region, 1);
		setenv("LC_TELEPHONE", region, 1);
		setenv("LC_MEASUREMENT", region, 1);
		setenv("LC_IDENTIFICATION", region, 1);
		free(region);
	}
}

static int __set_i18n(const char *domain, const char *dir)
{
	char *r;
	if (domain == NULL) {
		errno = EINVAL;
		return -1;
	}

	r = setlocale(LC_ALL, "");
	/* if locale is not set properly, try again to set as language base */
	if (r == NULL) {
		r = setlocale(LC_ALL, vconf_get_str(VCONFKEY_LANGSET));
	}
	bindtextdomain(domain, dir);
	textdomain(domain);
	return 0;
}

EXPORT_PUBLIC
int setting_set_i18n(char *pkgname, char *localedir)
{
	if (_set)
		return 0;
	update_lang();
	update_region();

	return __set_i18n(pkgname, localedir);
}

EXPORT_PUBLIC
int setting_set_i18n_force(char *pkgname, char *localedir)
{
	/*if (_set) return 0; */
	update_lang();
	update_region();

	return __set_i18n(pkgname, localedir);
}

/*example 1:  app_launcher("setting-myaccount-efl|mode:account_list"); */
/*example 2:  app_launcher("emergency-msg-setting-efl"); */
/*example 3:  app_launcher(
 * "sevenemail-setting-efl|caller:setting;cmd:main option"); */
EXPORT_PUBLIC int app_launcher(const char *pkg_name, app_control_reply_cb callback, void *user_data)
{
	int ret = -1;
	char *path = NULL;
	path = get_ug_path_from_ug_args((void *)pkg_name);
	app_control_h service = get_bundle_from_ug_args((void *)pkg_name);
	if (!service) {
		/*get no bundle from ug args */
		app_control_create(&service);
	}

	app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
	if (path)
		app_control_set_app_id(service, path);
	app_control_set_window(service, elm_win_xwindow_get(ug_get_window()));

	SETTING_TRACE("service name  - %s", path);
	int launch_ret = app_control_send_launch_request(service, callback, user_data);
	SETTING_TRACE("after app_service_create - %s : %d ", pkg_name,
			launch_ret);
	if (launch_ret == APP_CONTROL_ERROR_NONE) {
		/* on success */
		SETTING_TRACE("on success");
		ret = 0;
	} else {
		/* on error */
		SETTING_TRACE("on error");
		ret = -1;
	}
	app_control_destroy(service);
	service = NULL;

	FREE(path);

	return ret;
}

EXPORT_PUBLIC int app_group_launcher(const char *pkg_name)
{
	int ret = -1;
	char *path = NULL;
	path = get_ug_path_from_ug_args((void *)pkg_name);
	app_control_h service = get_bundle_from_ug_args((void *)pkg_name);
	if (!service) {
		/*get no bundle from ug args */
		app_control_create(&service);
	}

	app_control_set_operation(service, APP_CONTROL_OPERATION_PICK);
	app_control_set_launch_mode(service, APP_CONTROL_LAUNCH_MODE_GROUP);
	/*service_set_operation(service, SERVICE_OPERATION_DEFAULT); */
	if (path)
		app_control_set_app_id(service, path);
	app_control_set_window(service, elm_win_xwindow_get(ug_get_window()));

	int launch_ret = app_control_send_launch_request(service, NULL, NULL);
	SETTING_TRACE("after app_service_create - %s : %d ", pkg_name,
			launch_ret);
	if (launch_ret == APP_CONTROL_ERROR_NONE) {
		/* on success */
		SETTING_TRACE("on success");
		ret = 0;
	} else {
		/* on error */
		SETTING_TRACE("on error");
		ret = -1;
	}
	app_control_destroy(service);
	service = NULL;

	FREE(path);

	return ret;
}

EXPORT_PUBLIC char *substring(const char *str, size_t begin, size_t len)
{
	if (str == 0 || strlen(str) == 0 || strlen(str) < begin
			|| strlen(str) < (begin + len))
		return 0;

	return strndup(str + begin, len);
}

EXPORT_PUBLIC bool get_tethering_status()
{
	/* check status of tethering */
	bool therthering_on = false;
	int mobile_ap_status = VCONFKEY_MOBILE_HOTSPOT_MODE_NONE;
	int err = -1;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_MOBILE_AP_STATUS,
			&mobile_ap_status, &err);
	if (ret == SETTING_RETURN_FAIL) {
		SETTING_TRACE_ERROR("fail to get vconf");
	}

	/**
	 *  - 1 : wi-fi
	 *  - 2 : bluetooth
	 *  - 4 : USB
	 *  - 8 : Wifi AP
	 *  4, 2, 1 = 7 --7 means bitwise-operation
	 */
	if (mobile_ap_status & (
			VCONFKEY_MOBILE_HOTSPOT_MODE_WIFI
			| VCONFKEY_MOBILE_HOTSPOT_MODE_WIFI_AP)) {
		therthering_on = true;
	}

	return therthering_on;
}
/**
 * [API] font-size
 */
EXPORT_PUBLIC char *__get_font_size_str(int font_size)
{
	const char *font_size_arr[] = { "IDS_ST_BODY_SMALL_M_TEXTSIZE", /* 0 */
	"IDS_ST_BODY_MEDIUM", /* 1 */
	"IDS_ST_BODY_TEXTSTYLE_LARGE", /* 2 */
	"IDS_EMAIL_OPT_HUGE_M_TEXTSIZE", /* 3 */
	"IDS_EMAIL_POP_GIANT_M_TEXTSIZE", /* 4 */
	};
	switch (font_size) {
	case SYSTEM_SETTINGS_FONT_SIZE_SMALL:
		return (char *)font_size_arr[0];
	case SYSTEM_SETTINGS_FONT_SIZE_NORMAL:
		return (char *)font_size_arr[1];
	case SYSTEM_SETTINGS_FONT_SIZE_LARGE:
		return (char *)font_size_arr[2];
	case SYSTEM_SETTINGS_FONT_SIZE_HUGE:
		return (char *)font_size_arr[3];
	case SYSTEM_SETTINGS_FONT_SIZE_GIANT:
		return (char *)font_size_arr[4];
	default:
		return (char *)font_size_arr[1];
	}
}

EXPORT_PUBLIC
void setting_ug_create_cb(char *ug, char *mem, char *parent, void *user_data)
{
	SETTING_TRACE("ug : %s, mem : %s, parent : %s, data : %p", ug, mem,
			parent, user_data);
}

EXPORT_PUBLIC
ui_gadget_h setting_ug_create(ui_gadget_h parent, const char *name,
		enum ug_mode mode, app_control_h service, struct ug_cbs *cbs)
{
	ui_gadget_h handle = ug_create(parent, name, mode, service, cbs);

	return handle;
}

EXPORT_PUBLIC
void setting_ug_destroy(ui_gadget_h parent)
{
	ug_destroy(parent);
	/*ug_create_cb(NULL, NULL); */
}

EXPORT_PUBLIC
void setting_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode, void *priv)
{
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
}

EXPORT_PUBLIC
void setting_play_sound(const char *sound_file, int vol, void **mp_handle)
{
	SETTING_TRACE_BEGIN;
	player_h *player = calloc(1, sizeof(player_h));
	if (!player) {
		SETTING_TRACE_ERROR("failed to calloc player_h");
		return;
	}

	sound_manager_set_session_type(SOUND_SESSION_TYPE_MEDIA);
	sound_manager_set_media_session_option(
			SOUND_SESSION_OPTION_PAUSE_OTHERS_WHEN_START,
			SOUND_SESSION_OPTION_INTERRUPTIBLE_DURING_PLAY);

	int err = player_create(player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("creating the player handle failed[%d]",
				err);
		FREE(player);
		return;
	}

	err = player_set_uri(*player, sound_file);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("error to set attribute---profile_uri[%d]",
				err);
		player_destroy(*player);
		FREE(player);
		return;
	}

	err = player_prepare(*player);
	if (err != PLAYER_ERROR_NONE) {
		SETTING_TRACE_ERROR("realizing the player handle failed[%d]",
				err);
		player_destroy(*player);
		FREE(player);
		return;
	}
	/* sleep(0.001); */
	player_state_e state = -1;
	player_get_state(*player, &state);
	SETTING_TRACE("state:%d", state);

	if (state != PLAYER_STATE_READY) {
		SETTING_TRACE_ERROR("state of player is invalid %d", err);
		player_unprepare(*player);
		player_destroy(*player);
		FREE(player);
		return;
	}

	player_set_completed_cb(*player, NULL, (void *)player);
	err = player_start(*player);
	if (err != PLAYER_ERROR_NONE) { /* if directly return error.. */
		SETTING_TRACE_ERROR("player_start [%d]", err);
		player_unset_completed_cb(*player);
		player_unprepare(*player);
		player_destroy(*player);
		FREE(player);
		return;
	}

	SETTING_TRACE("waiting..");
	*mp_handle = player;
}

