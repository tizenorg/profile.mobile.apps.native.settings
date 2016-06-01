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
#include <setting-phone-region-format.h>
#include <setting-common-draw-widget.h>
#include <eventsystem.h>

#include <system_settings.h>

#define MAX_REGION_STRLEN 256

static int setting_phone_region_format_create(void *cb);
static int setting_phone_region_format_destroy(void *cb);
static int setting_phone_region_format_cleanup(void *cb);

setting_view setting_view_phone_region_format = {
	.create = setting_phone_region_format_create,
	.destroy = setting_phone_region_format_destroy,
	.update = NULL,
	.cleanup = setting_phone_region_format_cleanup, };

/**
 * @brief Function of add color to display string by html charactor
 *
 * @param data The view data passed between all callbacks
 * @param old_str The source of the display string
 * @return The string add color
 */
static char *__setting_phone_region_format_get_str_colr(void *data,
		char *old_str)
{
	setting_retvm_if(NULL == data, ECORE_CALLBACK_CANCEL, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	int len_search;
	/*int len_display; */
	char *pos;
	char display[1024] = { 0, };

	/*len_display = strlen(old_str); */
	len_search = strlen(ad->search_text);
	pos = strcasestr(old_str, ad->search_text);

	if (0 < pos) {
		char first[1024] = { 0 };
		char middle[1024] = { 0 };

		snprintf(first, (int)(pos - old_str) + 1, "%s", old_str);
		snprintf(middle, len_search + 1, "%s", (char *)pos);

		snprintf(display, sizeof(display), "%s<match>%s</>%s", first,
				middle, (char *)(pos + len_search));
		return strdup(display);
	}

	return strdup(old_str);
}

/**
 * @brief Function of remove html charactor in entry string
 *
 * @param src The source of the entry string
 * @param dst The return string
 */
int setting_entry_str_get_displaying_part(const char *src, char *dst)
{
	retv_if(NULL == src || NULL == dst, -1);
	int len = strlen(src);
	int sub_num = 0;
	char *pre_flag = NULL;
	char *end_flag = NULL;
	char tmp_buf1[MAX_SEARCH_STR_LEN + 1] = { '\0' };
	char tmp_buf2[MAX_SEARCH_STR_LEN + 1] = { '\0' };

	snprintf(tmp_buf1, MAX_SEARCH_STR_LEN, "%s", src);

	/* get the first position where '<' found */
	pre_flag = strchr(tmp_buf1, '<');

	while (pre_flag) {
		/* get the begin position of '<' found */
		int begin_pos = pre_flag - tmp_buf1;
		snprintf(tmp_buf2, begin_pos + 1, "%s", tmp_buf1);
		/*CLK_ERR("tmp_buf2 : %s\n", tmp_buf2); */
		sub_num = 1;
		/* get the position of '>' which match previous '<' */
		end_flag = strchr(pre_flag, '>');
		if (end_flag) {
			snprintf(tmp_buf2 + begin_pos, MAX_SEARCH_STR_LEN, "%s",
					end_flag + 1);
			sub_num += end_flag - pre_flag;
		} else {
			break;
		}
		len -= sub_num;
		snprintf(tmp_buf1, MAX_SEARCH_STR_LEN, "%s", tmp_buf2);
		pre_flag = strchr(tmp_buf1, '<');
		/*CLK_ERR("tmp_buf1 : %s\n", tmp_buf1); */
	}

	snprintf(dst, MAX_SEARCH_STR_LEN, "%s", tmp_buf1);
	return len;
}

/**
 * @brief Callback of compare region in list
 *
 * @param d1 The first one of the region to be compare
 * @param d2 The second one of the region to be compare
 */
static int setting_phone_region_format_compare_cb(const void *d1,
		const void *d2)
{
	UChar region1[128] = { 0, };
	UChar region2[128] = { 0, };

	Setting_Region_Table *r1 = (Setting_Region_Table *)d1;
	Setting_Region_Table *r2 = (Setting_Region_Table *)d2;

	u_uastrcpy(region1, r1->desc);
	u_uastrcpy(region2, r2->desc);

	UErrorCode status = U_ZERO_ERROR;
	UCollator *coll = ucol_open(getenv("LANG"), &status);
	if (coll) {
		UCollationResult ret = ucol_strcoll(coll, region1, -1, region2,
				-1);

		ucol_close(coll);

		switch (ret) {
		case UCOL_EQUAL:
			return 0;
		case UCOL_GREATER:
			return 1;
		case UCOL_LESS:
			return -1;
		}
	}
	return SETTING_RETURN_FAIL;
}

/**
 * @brief Function of get region list
 *
 * @param list[] The display string of the region
 * @param item_idx[] The index of the region in list
 * @param region_keyStr[] The key of the region
 * @param region_num The count of region list items
 * @return #0 is correct
 */
static int setting_phone_region_format_get_region_fmt(char *list[],
		int item_idx[],
		/*[OUT]*/char *region_keyStr[], int *region_num)
{
	const char *loc_list = NULL;
	int loc_count = 0;
	UChar lang[LANGUAGE_STR_LEN] = { 0, };
	UChar country[COUNTRY_STR_LEN] = { 0, };
	UChar variant[SETTING_STR_SLP_LEN] = { 0, };
	UChar displayname[SETTING_STR_SLP_LEN] = { 0, };
	const char *localeID = NULL;
	UErrorCode language_status = U_ZERO_ERROR;
	UErrorCode country_status = U_ZERO_ERROR;
	UErrorCode variant_status = U_ZERO_ERROR;
	UErrorCode displayname_status = U_ZERO_ERROR;

	int tmp_region_num = 0;
	*region_num = 0;

	char *str_lang = NULL;
	char *str_country = NULL;
	char *str_variant = NULL;
	char *str_displayname = NULL;
	Eina_List *region_list = NULL;

	loc_count = uloc_countAvailable();

	int i = 0;
	localeID = uloc_getDefault();
	SETTING_TRACE("localeID = %s", localeID);

	for (; i < loc_count; i++) {

		if (tmp_region_num >= REGION_ITEM_MAX) {
			break;
		}

		loc_list = uloc_getAvailable(i);
		setting_retvm_if(NULL == loc_list,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
				"loc_list is NULL");

		char *e = strchr(loc_list, '_');
		const char *result;
		if (e) {
			int index = (int)(e - loc_list);
			result = substring(loc_list, 0, index);
			/*SETTING_TRACE("lang code : %s", result); */
		} else {
			/*SETTING_TRACE("lang code itself : %s", loc_list); */
			result = loc_list;
		}

		/************************************************************ */
		/************************************************************ */
		/************************************************************ */
		/* current font doesn't support the font */
		/* filter out if result == 'bo' */
		if (0 == safeStrCmp(result, "bo") || 0 == safeStrCmp(result,
				"dz"))
			continue;
		/************************************************************ */
		/************************************************************ */
		/************************************************************ */

		uloc_getDisplayLanguage(loc_list, localeID, lang,
				LANGUAGE_STR_LEN, &language_status);
		if (U_FAILURE(language_status)) {
			SETTING_TRACE_ERROR(
					"language_status got an error %s:%d ",
					language_status, language_status);
			/* set the value to the initial value */
			language_status = U_ZERO_ERROR;
		}

		uloc_getDisplayCountry(loc_list, localeID, country,
				COUNTRY_STR_LEN, &country_status);
		if (U_FAILURE(country_status)) {
			SETTING_TRACE_ERROR(
					"country_status got an error %s:%d ",
					country, country_status);
			/* set the value to the initial value */
			country_status = U_ZERO_ERROR;
		}

		uloc_getDisplayVariant(loc_list, localeID, variant,
				SETTING_STR_SLP_LEN, &variant_status);
		if (U_FAILURE(variant_status)) {
			SETTING_TRACE_ERROR(
					"variant_status got an error %s:%d ",
					variant, variant_status);
			/* set the value to the initial value */
			variant_status = U_ZERO_ERROR;
		}

		uloc_getDisplayName(loc_list, localeID, displayname,
				SETTING_STR_SLP_LEN, &displayname_status);
		if (U_FAILURE(displayname_status)) {
			SETTING_TRACE_ERROR(
					"displayname_status got an error %s:%d ",
					displayname, displayname_status);
			/* set the value to the initial value */
			displayname_status = U_ZERO_ERROR;
		}
		UErrorCode errcode;
		u_strToUpper(displayname, 1, displayname, 1, "", &errcode);
		str_lang = setting_phone_lang_str_to_utf8(lang);
		str_country = setting_phone_lang_str_to_utf8(country);
		str_variant = setting_phone_lang_str_to_utf8(variant);
		str_displayname = setting_phone_lang_str_to_utf8(displayname);

		/*SETTING_TRACE(
		 * " lang : %s, country : %s ---> str_displayname : %s ",
		 * str_lang, str_country, str_displayname); */

		if (0 == safeStrCmp(str_lang, result)) {
			/*SETTING_TRACE(" str_lang is equal to result %s %s ",
			 * str_lang, result); */
			FREE(str_lang);
			FREE(str_country);
			FREE(str_variant);
			FREE(str_displayname);
			continue;

		}

		if (str_lang == NULL) {
			/*SETTING_TRACE(" str_lang is NULL. "); */
			FREE(str_lang);
			FREE(str_country);
			FREE(str_variant);
			FREE(str_displayname);
			continue;
		} else if (0 == safeStrCmp(str_lang, "")) {
			/*SETTING_TRACE(" str_lang is Empty string. "); */
			FREE(str_lang);
			FREE(str_country);
			FREE(str_variant);
			FREE(str_displayname);
			continue;
		}

		/* filter which has variant */
		if (str_lang != NULL && str_country != NULL
				&& safeStrLen(str_country) > 1
				&& safeStrLen(str_variant) == 0) {
			Setting_Region_Table *item_data = (Setting_Region_Table *)calloc(
					1, sizeof(Setting_Region_Table));
			/*setting_retvm_if(item_data == NULL, -1,
			 * "calloc failed") */

			if (item_data == NULL) {
				SETTING_TRACE_ERROR("calloc failed");
				FREE(str_lang);
				FREE(str_country);
				FREE(str_variant);
				FREE(str_displayname);
				return -1;
			}

			item_data->desc = (char *)g_strdup(str_displayname);
			item_data->key = (char *)g_strdup(loc_list);
			region_list = eina_list_append(region_list, item_data);
			tmp_region_num++;
		}

		FREE(str_lang);
		FREE(str_country);
		FREE(str_variant);
		FREE(str_displayname);
	}

	region_list = eina_list_sort(region_list, eina_list_count(region_list),
			setting_phone_region_format_compare_cb);

	Eina_List *iter = NULL;
	Setting_Region_Table *item_data = NULL;
	i = 0;
	EINA_LIST_FOREACH(region_list, iter, item_data)
	{
		if (item_data) {
			/*list[i] = (char *)g_strdup(item_data->desc); */
			list[i] = elm_entry_utf8_to_markup(item_data->desc);
			item_idx[i] = i;
			region_keyStr[i] = (char *)g_strdup(item_data->key);

			/*SETTING_TRACE(" list[i] : %s, region_keyStr[i]",
			 * list[i], region_keyStr[i]); */

			free(item_data);
			item_data = NULL;
			i++;
		}
	}

	eina_list_free(region_list);
	SETTING_TRACE("region_num = %d", tmp_region_num);
	*region_num = tmp_region_num;
	return 0;
}

/**
 * @brief Function of set date_format vconf by param region
 *
 * @param region The input param to set date_format
 * @param data The view data passed between all callbacks
 */
EXPORT_PUBLIC int setting_phone_region_format_set_dateformat(const char *region,
		void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(!region, SETTING_RETURN_FAIL,
			"region parameter is NULL");
	setting_retvm_if(data == NULL, SETTING_RETURN_FAIL,
			"Data parameter is NULL");

	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	char *ret_str = NULL;
	UChar *uret = NULL;
	UChar customSkeleton[SETTING_STR_SLP_LEN] = { 0, };
	UErrorCode status = U_ZERO_ERROR;

	UChar bestPattern[SETTING_STR_SLP_LEN] = { 0, };
	char bestPatternString[SETTING_STR_SLP_LEN] = { 0, };
	char *skeleton = DATA_FORMAT_SKELETON;

	uret = u_uastrncpy(customSkeleton, skeleton, safeStrLen(skeleton));
	setting_retvm_if(!uret, SETTING_RETURN_FAIL, "u_uastrncpy is fail");

	if (ad->pattern_generator == NULL
			|| safeStrCmp(region, ad->prev_locale) != 0) {
		if (ad->pattern_generator != NULL) {
			udatpg_close(ad->pattern_generator);
			ad->pattern_generator = NULL;
		}
		ad->pattern_generator = udatpg_open(region, &status);
	}

	int32_t bestPatternCapacity = (int32_t)
			(sizeof(bestPattern) / sizeof((bestPattern)[0]));
	(void)udatpg_getBestPattern(ad->pattern_generator, customSkeleton,
			u_strlen(customSkeleton), bestPattern,
			bestPatternCapacity, &status);

	ret_str = u_austrcpy(bestPatternString, bestPattern);
	setting_retvm_if(!ret_str, SETTING_RETURN_FAIL, "u_uastrncpy is fail");

	int i = 0;
	int j = 0;
	int len = safeStrLen(bestPatternString);
	char region_format[4] = { 0, };
	int ymd[3] = { 0, };
	/* only save 'y', 'M', 'd' charactor */
	for (; i < len; i++) {
		if (bestPatternString[i] == 'y' && ymd[0] == 0) {
			region_format[j++] = bestPatternString[i];
			ymd[0] = 1;
		} else if (bestPatternString[i] == 'M' && ymd[1] == 0) {
			region_format[j++] = bestPatternString[i];
			ymd[1] = 1;
		} else if (bestPatternString[i] == 'd' && ymd[2] == 0) {
			region_format[j++] = bestPatternString[i];
			ymd[2] = 1;
		}
	}

	region_format[3] = '\0';

	char *date_format_str[DATA_FORMAT_CATEGORY_NUM] = {
		"dMy",
		"Mdy",
		"yMd",
		"ydM" };
	/* default is "Mdy" */
	int date_format_vconf_value = DATA_FORMAT_DEFAULT;
	for (i = 0; i < DATA_FORMAT_CATEGORY_NUM; i++) {
		if (!safeStrCmp(region_format, date_format_str[i])) {
			date_format_vconf_value = i;
		}
	}

	/* if region_format is null, should be set as default */
	if (isEmptyStr(region_format) || isSpaceStr(region_format)) {
		date_format_vconf_value = 1;
	}

	SETTING_TRACE("bestPatternString : %s, format: %s, index: %d",
			bestPatternString, region_format,
			date_format_vconf_value);

	int err = SETTING_RETURN_SUCCESS;
	setting_set_int_slp_key(INT_SLP_SETTING_DATE_FORMAT,
			date_format_vconf_value, &err);
	setting_retvm_if(err == SETTING_RETURN_FAIL, SETTING_RETURN_FAIL,
			"set vconf fail");
	G_FREE(ad->prev_locale);
	ad->prev_locale = (char *)g_strdup(region);
	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief Function of get current region_format
 *
 * @return current region_format string
 */
static char *__region_format_vconf_str_get()
{
	SETTING_TRACE_BEGIN;
	char *tempstr = vconf_get_str(VCONFKEY_REGIONFORMAT);
	setting_retvm_if(NULL == tempstr, NULL, "NULL == tempstr");

	char *str_region = NULL;
	char *p = strchr(tempstr, '.');

	int len = safeStrLen(tempstr) - safeStrLen(".UTF-8");
	if (p) {
		if (len > 0) {
			str_region = strndup(tempstr, len); /*.UTF8 => 5 */
		} else {
			str_region = (char *)g_strdup(tempstr);
			SETTING_TRACE_ERROR("region format vconf stored error");
		}
	} else {
		str_region = (char *)g_strdup("en_US");
	}

	FREE(tempstr);
	return str_region;
}

/**
 * @brief Function of genlist update when entry change
 *
 * @param data The view data passed between all callbacks
 */
static Eina_Bool __region_genlist_update(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, ECORE_CALLBACK_CANCEL, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	SETTING_TRACE_DEBUG("ad->search_text: %s", ad->search_text);

	elm_genlist_clear(ad->gl_region);
	G_FREE(ad->region_vconf_str);
	ad->region_vconf_str = __region_format_vconf_str_get();

	Setting_GenGroupItem_Data *item_data = NULL;
	ad->selected_region_idx = -1;

	int search_count = 0;
	if (isEmptyStr(ad->search_text) || is_substr_ncase(
			_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"),
			ad->search_text)) {

		char *buf = __setting_phone_region_format_get_str_colr(ad,
				_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"));
		/*SETTING_TRACE_DEBUG("buf: %s", buf); */

		item_data = setting_create_Gendial_field_def(ad->gl_region,
				&(ad->itc_1text_tb),
				setting_phone_region_format_mouse_up_Gendial_list_radio_cb,
				ad->chk_region, SWALLOW_Type_INVALID, NULL,
				NULL, -1, buf, NULL, NULL);
		if (item_data) {
			item_data->userdata = ad;
			item_data->keyStr2 = _(
					"IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC");
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}

		search_count++;
		FREE(buf);
	}

	int i = 0;
	for (; i < ad->region_num; i++) {
		if (isEmptyStr(ad->search_text) || is_substr_ncase(
				ad->region_desc[i], ad->search_text)) {

			char *buf = __setting_phone_region_format_get_str_colr(
					ad, ad->region_desc[i]);
			/*SETTING_TRACE_DEBUG("buf: %s", buf); */

			item_data = setting_create_Gendial_field_def(
					ad->gl_region, &(ad->itc_1text_tb),
					setting_phone_region_format_mouse_up_Gendial_list_radio_cb,
					ad->chk_region, SWALLOW_Type_INVALID,
					NULL, NULL, ad->region_index[i], buf,
					NULL, NULL);
			if (item_data) {
				item_data->userdata = ad;
				item_data->keyStr2 = ad->region_desc[i];
			} else {
				SETTING_TRACE_ERROR("item_data is NULL");
			}

			FREE(buf);
			setting_retvm_if(NULL == ad->region_vconf_str, FALSE,
					"ad->region_vconf_str is NULL");
			if (!safeStrCmp(ad->region_vconf_str,
					ad->region_keyStr[i])) {
				ad->selected_region_idx = i;
			}
			search_count++;
		}
	}

	if (search_count > 1) {
		/*? Setting_GenGroupItem_Data *last_item =
		 * (Setting_GenGroupItem_Data *) */
		elm_object_item_data_get(
				elm_genlist_last_item_get(ad->gl_region));
	}

	if (search_count == 0) {
		if (!ad->nocontents) {
			ad->nocontents = elm_layout_add(ad->win_get);
			elm_layout_theme_set(ad->nocontents, "layout",
					"nocontents", "search");
			elm_object_part_text_set(ad->nocontents, "elm.text",
					_("IDS_ST_BODY_NO_RESULTS_FOUND"));
		}
		if (ad->nocontents != elm_object_part_content_get(
				ad->ly_sub_region, "elm.swallow.content")) {
			ad->gl_region = elm_object_part_content_unset(
					ad->ly_sub_region,
					"elm.swallow.content");
			evas_object_hide(ad->gl_region);
			elm_object_part_content_set(ad->ly_sub_region,
					"elm.swallow.content", ad->nocontents);
		}
	} else {
		if (ad->gl_region != elm_object_part_content_get(
				ad->ly_sub_region, "elm.swallow.content")) {
			ad->nocontents = elm_object_part_content_unset(
					ad->ly_sub_region,
					"elm.swallow.content");
			evas_object_hide(ad->nocontents);
			elm_object_part_content_set(ad->ly_sub_region,
					"elm.swallow.content", ad->gl_region);
		}
	}

#if 0
	int automatic_select = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL,
			&automatic_select);
	if (automatic_select) {
		ad->selected_region_idx = 0;
	}

	if (ad->selected_region_idx > -1) {
		elm_radio_value_set(ad->chk_region, ad->selected_region_idx);
	}
#endif

	ad->search_idler = NULL;
	ad->searchlist_update_timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

/**
 * @brief Callback of entry change
 *
 * @param data The view data passed between all callbacks
 */
static void __searchbar_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	Evas_Object *entry = elm_object_part_content_get(ad->search_bar,
			"elm.swallow.content");
	if (!entry) {
		return;
	}

	const char *str = elm_entry_entry_get(entry);
	/* empty string is useful */
	if (!str) {
		return;
	}

	SETTING_TRACE_DEBUG("str: %s", str);

	int len = setting_entry_str_get_displaying_part(str, ad->search_text);
	if (len < 0) {
		return;
	}

	SETTING_TRACE_DEBUG("ad->search_text: %s", ad->search_text);

	/* we use idle to delay, this may cause some input search will be delay
	 * and then query together so we use timer to delay*/
	if (ad->searchlist_update_timer) {
		ecore_timer_del(ad->searchlist_update_timer);
		ad->searchlist_update_timer = NULL;
	}

	ad->searchlist_update_timer = ecore_timer_add(0.01,
			__region_genlist_update, ad);
}

/**
 * @brief Callback of entry prediction change
 *
 * @param data The view data passed between all callbacks
 */
static void __searchbar_prediction_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	Evas_Object *entry = elm_object_part_content_get(ad->search_bar,
			"elm.swallow.content");
	if (!entry) {
		return;
	}

	const char *str = elm_entry_entry_get(entry);
	/* empty string is useful */
	if (!str) {
		return;
	}
	SETTING_TRACE_DEBUG("str: %s", str);

	int len = setting_entry_str_get_displaying_part(str, ad->search_text);
	if (len < 0) {
		return;
	}

	SETTING_TRACE_DEBUG("ad->search_text: %s", ad->search_text);

	if (ad->searchlist_update_timer) {
		ecore_timer_del(ad->searchlist_update_timer);
		ad->searchlist_update_timer = NULL;
	}

	ad->searchlist_update_timer = ecore_timer_add(0.01,
			__region_genlist_update, ad);
}

/**
 * @brief Callback of genlist item click event
 *
 * @param data The view data passed between all callbacks
 * @param event_info The select item
 */
static void setting_phone_region_format_mouse_up_Gendial_list_radio_cb(
		void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL,
			"Invalid argument: event info is NULL");

	Evas_Object *radio = (Evas_Object *)data;
	Elm_Object_Item *item = (Elm_Object_Item *)event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *)elm_object_item_data_get(
					item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingPhoneUG *ad = (SettingPhoneUG *)list_item->userdata;

	int ret;
	char region[MAX_REGION_STRLEN] = { 0, };
	SETTING_TRACE_DEBUG("item keystr: %s", list_item->keyStr2);
	if (!safeStrCmp(_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"),
			list_item->keyStr2)) {
		vconf_set_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, TRUE);

		/*set region same to language */
		char *lang = NULL;
		lang = vconf_get_str(VCONFKEY_LANGSET);
		if (lang) {
			if (strlen(region) < 256) {
				strncpy(region, lang, 255);
			} else {
				FREE(lang);
				return;
			}
			FREE(lang);
		}
		SETTING_TRACE_DEBUG("auto region: %s", region);
	} else {
		vconf_set_bool(VCONFKEY_SETAPPL_REGION_AUTOMATIC_BOOL, FALSE);
		elm_radio_value_set(radio, list_item->chk_status);

		ad->prev_region = list_item->chk_status;
		SETTING_TRACE("cur_chk_region = %d", ad->prev_region);

		snprintf(region, MAX_REGION_STRLEN, "%s.UTF-8",
				ad->region_keyStr[ad->prev_region]);

		ret = setting_phone_region_format_set_dateformat(
				ad->region_keyStr[ad->prev_region], ad);
		setting_retm_if(ret == SETTING_RETURN_FAIL,
				"[Error] set date format fail");
	}
	ret = set_regionformat_helper(region);
	setting_retm_if(ret != 0,
			"[Error] set value of VCONFKEY_REGIONFORMAT fail");

	if (!safeStrCmp(ad->region_search_id, "region")) {
		/* after changed, desroy ug */
		ug_destroy_me(ad->ug);
	} else {
		/* after changed, go back to the previous page */
		setting_view_change(&setting_view_phone_region_format,
				&setting_view_phone_language_region, ad);
	}
}

/**
 * @brief Callback of add genlist item use animator
 *
 * @param data The view data passed between all callbacks
 */
static Eina_Bool __region_animator_cb(void *data)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(NULL == data, ECORE_CALLBACK_CANCEL, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	Setting_GenGroupItem_Data *item_data = NULL;
	if (ad->gl_region_cur_index >= ad->region_num) {
		/*? Setting_GenGroupItem_Data *last_item =
		 * (Setting_GenGroupItem_Data *) */
		elm_object_item_data_get(
				elm_genlist_last_item_get(ad->gl_region));

		if (ad->animator) {
			ecore_animator_del(ad->animator);
			ad->animator = NULL;
		}
		elm_radio_value_set(ad->chk_region, ad->selected_region_idx);
		ad->prev_region = ad->selected_region_idx;

		/*
		 Elm_Object_Item *item = NULL;
		 item = elm_genlist_item_append(ad->gl_region,
		 &itc_bottom_seperator, NULL, NULL,
		 ELM_GENLIST_ITEM_NONE, NULL, NULL);
		 elm_genlist_item_select_mode_set(item,
		 ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		 */
		return ECORE_CALLBACK_CANCEL;
	}

	int i = ad->gl_region_cur_index++;
	if (isEmptyStr(ad->search_text) || is_substr_ncase(ad->region_desc[i],
			ad->search_text)) {
		item_data = setting_create_Gendial_field_def(ad->gl_region,
				&(ad->itc_1text_tb),
				setting_phone_region_format_mouse_up_Gendial_list_radio_cb,
				ad->chk_region, SWALLOW_Type_INVALID, NULL,
				NULL, ad->region_index[i],
				ad->region_desc[i]/* HERE */, NULL, NULL);

		if (item_data) {
			item_data->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}

		if (!safeStrCmp(ad->region_vconf_str, ad->region_keyStr[i])) {
			ad->selected_region_idx = i;
			if (item_data) {
				ad->selected_item = item_data->item;
			}
		}
	}

	return ECORE_CALLBACK_RENEW;

}

/**
 * @brief Callback of popup_timer
 *
 * @param data The view data passed between all callbacks
 * @return #1 means call it again, else means call it only once
 */
static Eina_Bool __region_popup_timer_cb(void *data)
{
	/*SETTING_TRACE_BEGIN; */
	setting_retvm_if(NULL == data, ECORE_CALLBACK_CANCEL, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	if (ad->pop_progress) {
		evas_object_del(ad->pop_progress);
		ad->pop_progress = NULL;
	}

	/* recover it when popup disappear */
	if (ad->ly_region) {
		elm_object_tree_focus_allow_set(ad->ly_region, EINA_TRUE);
	}

	ad->popup_timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

/**
 * @brief Function of create region genlist
 *
 * @param data The view data passed between all callbacks
 */
static void __region_genlist_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;

	/* disable layout operation event */
	elm_object_tree_focus_allow_set(ad->ly_region, EINA_FALSE);

	if (ad->pop_progress) {
		evas_object_del(ad->pop_progress);
		ad->pop_progress = NULL;
	}
	ad->pop_progress = setting_create_popup_with_progressbar(ad,
			ad->win_get,
			PROGRESSBAR_STYLE,
			NULL, KeyStr_Loading, NULL, 0, TRUE, TRUE, 0);

	if (ad->popup_timer) {
		ecore_timer_del(ad->popup_timer);
		ad->popup_timer = NULL;
	}

	ad->popup_timer = ecore_timer_add(2.0, __region_popup_timer_cb, ad);

	elm_genlist_clear(ad->gl_region);
	G_FREE(ad->region_vconf_str);
	ad->region_vconf_str = __region_format_vconf_str_get();

	Setting_GenGroupItem_Data *item_data = NULL;
	ad->gl_region_index = 0;
	ad->selected_item = NULL;
	ad->selected_region_idx = -1;

	item_data = setting_create_Gendial_field_def(ad->gl_region,
			&(ad->itc_1text_tb),
			setting_phone_region_format_mouse_up_Gendial_list_radio_cb,
			ad->chk_region, SWALLOW_Type_INVALID, NULL, NULL, -1,
			_("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC"), NULL, NULL);
	if (item_data) {
		item_data->userdata = ad;
		item_data->keyStr2 = _("IDS_ST_BODY_ANSWERINGMODE_AUTOMATIC");
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

	int i = 0;
	for (; i < ad->region_num; i++) {
		if (isEmptyStr(ad->search_text) || is_substr_ncase(
				ad->region_desc[i], ad->search_text)) {
			item_data = setting_create_Gendial_field_def(
					ad->gl_region, &(ad->itc_1text_tb),
					setting_phone_region_format_mouse_up_Gendial_list_radio_cb,
					ad->chk_region, SWALLOW_Type_INVALID,
					NULL, NULL, ad->region_index[i],
					ad->region_desc[i], NULL, NULL);
			if (item_data) {
				item_data->userdata = ad;
				item_data->keyStr2 = ad->region_desc[i];
			} else {
				SETTING_TRACE_ERROR("item_data is NULL");
			}

			if (!safeStrCmp(ad->region_vconf_str,
					ad->region_keyStr[i])) {
				ad->selected_region_idx = i;
				if (item_data) {
					ad->selected_item = item_data->item;
				}
			}
			ad->gl_region_index++;
		}
	}
	ad->gl_region_cur_index = i;

	if (ad->selected_region_idx > -1) {
		elm_radio_value_set(ad->chk_region, ad->selected_region_idx);
		ad->prev_region = ad->selected_region_idx;
	}
	if (ad->gl_region_cur_index < ad->region_num
			&& ad->gl_region_index > SETTING_PHONE_REGION_FORMAT_ANIMATOR_START_INDEX) {
		/*to continue adding items using animator */
		SETTING_TRACE("to continue adding items using animator");
		if (ad->animator) {
			ecore_animator_del(ad->animator);
			ad->animator = NULL;
		}

		ad->animator = ecore_animator_add(__region_animator_cb, ad);
	} else { /*had finished adding items */
		SETTING_TRACE("had finished adding items");

		/*? Setting_GenGroupItem_Data *last_item =
		 * (Setting_GenGroupItem_Data *) */
		elm_object_item_data_get(
				elm_genlist_last_item_get(ad->gl_region));
	}
}

/**
 * @brief Callback of cancel button
 *
 * @param data The view data passed between all callbacks
 */
static Eina_Bool setting_phone_region_format_click_softkey_cancel_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");
	SettingPhoneUG *ad = (SettingPhoneUG *)data;
	setting_view_change(&setting_view_phone_region_format,
			&setting_view_phone_language_region, ad);

	return EINA_TRUE;
}

static Evas_Object *_item_content_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(!data || !obj, NULL, "!data || !obj");

	if (!safeStrCmp(part, "elm.swallow.content")) {
		Evas_Object *sub_layout = elm_layout_add(obj);
		elm_layout_theme_set(sub_layout, "layout", "nocontents",
				"search");
		evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND,
				0);
		elm_object_part_text_set(sub_layout, "elm.text",
				_("IDS_ST_BODY_NO_RESULTS_FOUND"));
		evas_object_show(sub_layout);

		return sub_layout;
	}

	return NULL;
}

/**
 * @brief Callback of cancel button by caller
 *
 * @param data The view data passed between all callbacks
 */
static Eina_Bool setting_phone_region_format_caller_exist_right_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, EINA_FALSE, "Data parameter is NULL");

	SettingPhoneUG *ad = (SettingPhoneUG *)data;
	/* Create Bundle and send message */
	app_control_h svc;
	if (app_control_create(&svc)) {
		return EINA_FALSE;
	}

	app_control_add_extra_data(svc, "result", "rbutton_click");
	ug_send_result(ad->ug, svc);

	app_control_destroy(svc);

	ug_destroy_me(ad->ug);
	return EINA_FALSE;
}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

/**
 * @brief Callback of view creating
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_region_format_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhoneUG *ad = (SettingPhoneUG *)cb;

	Evas_Object *sub_layout = NULL;
	Elm_Naviframe_Item_Pop_Cb gl_sel_cb = NULL;

	ad->region_search_id = NULL;
	app_control_h service = ad->bundle_data;
	app_control_get_extra_data(service, "viewtype", &ad->region_search_id);
	if (!safeStrCmp(ad->region_search_id, "region")) {
		Evas_Object *layout_main = setting_create_win_layout(
				ad->win_get);
		retvm_if(layout_main == NULL,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
				"layout_main create fail");
		ad->ly_region = layout_main;

		ad->navi_bar = setting_create_navi_bar(layout_main);
		retvm_if(ad->navi_bar == NULL,
				SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
				"navi_bar create fail");

		sub_layout = elm_layout_add(ad->navi_bar);
		elm_layout_theme_set(sub_layout, "layout", "application",
				"searchbar_base");
		elm_object_signal_emit(sub_layout, "elm,state,show,searchbar",
				"elm");
		evas_object_show(sub_layout);

		gl_sel_cb = setting_phone_region_format_caller_exist_right_cb;
	} else {
		sub_layout = elm_layout_add(ad->navi_bar);
		elm_layout_theme_set(sub_layout, "layout", "application",
				"searchbar_base");
		elm_object_signal_emit(sub_layout, "elm,state,show,searchbar",
				"elm");
		ad->ly_region = sub_layout;

		gl_sel_cb = setting_phone_region_format_click_softkey_cancel_cb;
	}

	/* */
	setting_create_Gendial_itc(SETTING_GENLIST_1ICON_STYLE,
			&(ad->itc_1icon_with_no_line));
	ad->itc_1icon_with_no_line.func.content_get = _item_content_get;

	Elm_Object_Item *navi_it = setting_push_layout_navi_bar(
			REGION_FORMAT_LOWER, "IDS_ST_BUTTON_BACK",
			NULL,
			NULL,
			setting_phone_region_format_click_softkey_cancel_cb,
			NULL,
			NULL, ad, sub_layout, ad->navi_bar, NULL);
	elm_naviframe_item_pop_cb_set(navi_it, gl_sel_cb, ad);
	evas_object_data_set(ad->navi_bar, "sip.naviframe.title_obj", "SEARCH");
	ad->gl_region = elm_genlist_add(ad->navi_bar);
	/* resolve abnormal height issue */
	elm_genlist_mode_set(ad->gl_region, ELM_LIST_COMPRESS);
	elm_genlist_homogeneous_set(ad->gl_region, EINA_TRUE);
	/* first to clear list */
	elm_genlist_clear(ad->gl_region);

	retvm_if(ad->gl_region == NULL, SETTING_DRAW_ERR_FAIL_SCROLLER,
			"ad->gl_region is NULL");
	/*evas_object_smart_callback_add(ad->gl_region, "realized",
	 * __gl_realized_cb, NULL); */

	ad->chk_region = elm_radio_add(ad->gl_region);
	elm_radio_state_value_set(ad->chk_region, -1);
	elm_radio_value_set(ad->chk_region, -1);

	int ret = setting_phone_region_format_get_region_fmt(ad->region_desc,
			ad->region_index, ad->region_keyStr, &ad->region_num);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get region format list failed");
	}

	ad->prev_region = 0;
	memset(ad->search_text, '\0', MAX_SEARCH_STR_LEN + 1);

	__region_genlist_create(ad);
	ad->search_bar = setting_create_searchbar(ad, sub_layout,
			__searchbar_changed_cb, NULL);
	if (ad->search_bar) {
		Evas_Object *entry = evas_object_data_get(ad->search_bar,
				"entry");
		if (entry) {
			evas_object_smart_callback_add(entry, "preedit,changed",
					__searchbar_prediction_changed_cb, ad);
		}
	}

	/* SET SEARCHBAR AREA SIZE */
	int value = 0;
	ret = system_settings_get_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE,
			&value);

	if (value == SYSTEM_SETTINGS_FONT_SIZE_GIANT) {
		elm_object_signal_emit(ad->search_bar, "set,show,giant", "*");
	} else {
		elm_object_signal_emit(ad->search_bar, "set,show,normal", "*");
	}

	elm_object_part_content_set(sub_layout, "elm.swallow.content",
			ad->gl_region);

	ad->ly_sub_region = sub_layout;

	setting_view_phone_region_format.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief Callback of view destroy
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_region_format_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingPhoneUG *ad = (SettingPhoneUG *)cb;

	evas_object_smart_callback_del(ad->gl_region, "realized",
			__gl_realized_cb);

	/*Apply new SIP concept */
	evas_object_data_set(ad->navi_bar, "sip.naviframe.title_obj", NULL);

	/* FIXED : destroy only if it was created. */
	if (setting_view_phone_region_format.is_create) {
		if (ad->region_search_id) {
			FREE(ad->region_search_id);
		}
		if (ad->search_idler) {
			ecore_idler_del(ad->search_idler);
			ad->search_idler = NULL;
		}
		if (ad->animator) {
			ecore_animator_del(ad->animator);
			ad->animator = NULL;
		}
		if (ad->popup) {
			evas_object_del(ad->popup);
			ad->popup = NULL;
		}
		if (ad->pop_progress) {
			evas_object_del(ad->pop_progress);
			ad->pop_progress = NULL;
		}

		if (ad->popup_timer) {
			ecore_timer_del(ad->popup_timer);
			ad->popup_timer = NULL;
		}

		if (ad->searchlist_update_timer) {
			ecore_timer_del(ad->searchlist_update_timer);
			ad->searchlist_update_timer = NULL;
		}

		elm_naviframe_item_pop(ad->navi_bar);
		setting_view_phone_region_format.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

/**
 * @brief Callback of view cleanup
 *
 * @param cb The view data passed between all callbacks
 * @return #1 on success, else on failed
 */
static int setting_phone_region_format_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_phone_region_format_destroy(cb);
	return SETTING_RETURN_SUCCESS;
}
