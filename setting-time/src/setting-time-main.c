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

#include <setting-time-main.h>
#include <dd-deviced-managed.h>
#include <dd-deviced.h>

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#include <vconf.h>
#include <vconf-keys.h>

#include <unicode/udat.h>
#include <unicode/ustring.h>
#include <unicode/uloc.h>
#include <unicode/ucal.h>
#include <unicode/utmscale.h>

#include <bundle_internal.h>
#include <system_settings.h>

#define ONE_MINUTE 60
#define PROGRESS_POPUP_SHOW 0
#define DEF_BUF_SIZE 256

/*	ICU utilities*/
static UChar *uastrcpy(const char *chars);
static void ICU_set_timezone(const char *timezone);
static void get_gmt_offset(char *str_buf, int size);

static char *get_timezone_str();
static char *get_city_name_result();
static bool get_city_name(char *tzname);
static char *get_timezone_displayname();

static char *s_result;
static int query_callback(void *NotUsed, int argc, char **argv,
		char **azColName);
static bool setting_update_timezone(SettingTimeUG *ad);
static void __setting_update_datefield_cb(void *cb);


static int setting_time_main_create(void *cb);
static int setting_time_main_destroy(void *cb);
static int setting_time_main_update(void *cb);
static int setting_time_main_cleanup(void *cb);

setting_view setting_view_time_main = {
	.create = setting_time_main_create,
	.destroy = setting_time_main_destroy,
	.update = setting_time_main_update,
	.cleanup = setting_time_main_cleanup,
};


bool __update_timezone_idler(SettingTimeUG *ad)
{
	setting_update_timezone(ad);
	ecore_idler_del(ad->update_timezone_idler);
	ad->update_timezone_idler = NULL;
	return 0;
}


void system_settings_changed_timezone(system_settings_key_e key, void *data)
{
	SETTING_TRACE(">>>>>>>> NOTIFY CHANGE TIMEZONE ");
	SettingTimeUG *ad = (SettingTimeUG *)data;
	int ret;
	char *timezone = NULL;

	ret = system_settings_get_value_string(
			SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, &timezone);
	if (ret)
		SETTING_TRACE_ERROR("Cannot get TIMEZONE system key value");

	SETTING_TRACE(" timezone : (%s) ", timezone);

	tzset();
	ICU_set_timezone(timezone);

	if (!__setting_set_city_tzone(timezone)) {
		SETTING_TRACE("__setting_set_city_tzone ERROR");
		return;
	}
	FREE(timezone);

	/* update timezone */
	if (ad->update_timezone_idler) {
		ecore_idler_del(ad->update_timezone_idler);
		ad->update_timezone_idler = NULL;
	}
	ad->update_timezone_idler =
		ecore_idler_add((Ecore_Task_Cb) __update_timezone_idler, ad);
}



// ret = system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, system_settings_changed_timezone, ad);

void time_changed_callback(keynode_t *key, void *data)
{

	SETTING_TRACE_DEBUG("time and timezone have been changed: "
			"TIME_CHANGED_HEYNOTY_CALLBACK");
	SettingTimeUG *ad = (SettingTimeUG *)data;

	int ret = setting_time_update_cb(ad);
	if (ret != 0) {
		SETTING_TRACE("FAIL: setting_time_update_db(ad)\n");
	}

	/* update time */
	if (ad->refresh_time_idler) {
		ecore_idler_del(ad->refresh_time_idler);
		ad->refresh_time_idler = NULL;
	}
	ad->refresh_time_idler = ecore_idler_add(setting_update_datefield_cb,
			ad);
}

void setting_time_update_time_date_format_string(SettingTimeUG *ad)
{
	SETTING_TRACE_BEGIN;

	struct tm *ts;
	struct tm ts_ret;
	time_t ctime;

	int ret, value, err;

	char time_str[MAX_ARGS];

	ctime = time(NULL);
	ts = localtime_r(&ctime, &ts_ret);
	ret_if(!ts);

	/* set time in particular format */
	ret = setting_get_int_slp_key(INT_SLP_SETTING_REGIONFORMAT_TIME1224,
			&value, &err);
	if (ret == SETTING_RETURN_FAIL)
		value = VCONFKEY_TIME_FORMAT_12;

	switch (value) {
	case VCONFKEY_TIME_FORMAT_12: {
			/* Get Time */
			strftime(time_str, MAX_ARGS, "%l:%M %p ", &ts_ret);
		}
		break;

	case VCONFKEY_TIME_FORMAT_24: {
			strftime(time_str, MAX_ARGS, "%H:%M", &ts_ret);
		}
		break;
	default:
		/* do nothing */
		break;
	}

	if (ad->data_time) {
		elm_object_item_data_set(ad->data_time->item, ad->data_time);
		elm_genlist_item_update(ad->data_time->item);
	}
}

bool setting_update_timezone(SettingTimeUG *ad)
{
	/* SETTING_TRACE_BEGIN; */
	char time_zone_sub_str[CITY_BUF_SIZE + GMT_BUF_SIZE + 3] = { 0, };
	char *displayTimezone = NULL;
	char gmt_offset[GMT_BUF_SIZE] = {0, };
	int ret = 0;

	/*	get time zone */
	displayTimezone = get_timezone_displayname();
	get_gmt_offset(gmt_offset, GMT_BUF_SIZE);

	if (displayTimezone != NULL) {
		ret = snprintf(time_zone_sub_str,
				CITY_BUF_SIZE + GMT_BUF_SIZE + 2, "%s, GMT %s",
				displayTimezone, gmt_offset);
		retv_if(ret < 0, 0);
	} else {
		ret = snprintf(time_zone_sub_str,
				CITY_BUF_SIZE + GMT_BUF_SIZE + 2,
				"Korean Standard Time, GMT +9:00");
		retv_if(ret < 0, 0);
	}
	SETTING_TRACE("ad->data_tz:%p", ad->data_tz);

	if (ad->data_tz) {
		ad->data_tz->sub_desc = (char *)g_strdup(time_zone_sub_str);
		elm_object_item_data_set(ad->data_tz->item, ad->data_tz);
		elm_genlist_item_update(ad->data_tz->item);
	}

	FREE(displayTimezone);

	return 0;
}

static void setting_time_main_int_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);
	SettingTimeUG *ad = data;

	char *vconf_name = vconf_keynode_get_name(key);
	/* SETTING_TRACE("vconf_name:%s", vconf_name); */

	if (!safeStrCmp(vconf_name, VCONFKEY_REGIONFORMAT_TIME1224) ||
		!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DATE_FORMAT_INT)) {
		char *time;
		static char time_arr[DEF_BUF_SIZE];
		char *pa_time_format = get_pa_time_format_str();
		setting_retm_if(NULL == pa_time_format,
				"pa_time_format is NULL");

		char *lang = NULL;
		int err = system_settings_get_value_string(
				SYSTEM_SETTINGS_KEY_LOCALE_LANGUAGE, &lang);
		if (err != SYSTEM_SETTINGS_ERROR_NONE) {
			SETTING_TRACE("Failed to call system_settings_get_"
					"value_string with error code %d", err);
		}

		if (!strncmp(pa_time_format, "IDS_ST_BODY_12_HOURS",
				strlen("IDS_ST_BODY_12_HOURS")))
			time = __setting_phone_lang_get_by_pattern(lang,
					"hhmm");
		else
			time = __setting_phone_lang_get_by_pattern(lang, "Hm");

		strncpy(time_arr, time, DEF_BUF_SIZE);
		time_arr[DEF_BUF_SIZE - 1] = '\0';

		elm_object_text_set(ad->data_time->btn_right, time_arr);
		FREE(pa_time_format);
	}
}

static int _set_timezone_helper(char *tzdata)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("= %s", tzdata);
	int ret;
	ret = system_settings_set_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, tzdata);
	SETTING_TRACE("system_settings_set_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE) = %d, .", ret);

	return ret;
}

static int _alarmmgr_set_systime_helper(time_t t_current)
{
	int ret;
	ret = alarmmgr_set_systime(t_current);
	return ret;
}


static void __update_time_via_sim_card(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingTimeUG *ad = (SettingTimeUG *) data;
	int ret = 0;
	int t_nitz = 0;
	int t_offset = 0;
	ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_GMT, &t_nitz);
	ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT, &t_offset);
	char *tzpath = vconf_get_str(VCONFKEY_TELEPHONY_NITZ_ZONE);
	if (ret != 0 || isEmptyStr(tzpath)) {
		return;
	}
	ret = 0;/*reset.. */

	/* a.time zone */
	SETTING_TRACE("tzpath : %s", tzpath);
	ret = _set_timezone_helper(tzpath);
	if (ret < 0) {
		SETTING_TRACE("tzpath is not valid.");
		return;
	} else
		SETTING_TRACE("_set_timezone_helper - successful : "
				"%s \n", tzpath);
	if (!__setting_set_city_tzone(tzpath)) {
		SETTING_TRACE("__setting_set_city_tzone ERROR");
		return;
	}
	get_city_name(tzpath);
	char *city = get_city_name_result();
	SETTING_TRACE("city:%s", city);
	if (city) {
		ret = vconf_set_str(VCONFKEY_SETAPPL_CITYNAME_INDEX_INT, city);
		setting_retm_if(ret != 0, "set vconf[%s] failed",
				VCONFKEY_SETAPPL_CITYNAME_INDEX_INT);
	}
	setting_update_timezone(ad);

	/* b.TIME AND DATE */
	int t_uptime = 0;
	FILE *fp = fopen("/proc/uptime", "r");
	if (fp) {
		if (!fscanf(fp, "%d", &t_uptime)) {
			SETTING_TRACE_ERROR("fscanf error");
		}
		fclose(fp);
		fp = NULL;
	} else {
		SETTING_TRACE_ERROR("fopen error");
	}

	SETTING_TRACE("t_nitz:%d, t_offset:%d, t_uptime:%d", t_nitz, t_offset,
			t_uptime);
	time_t t_current = t_nitz + t_uptime - t_offset;
	SETTING_TRACE("t_current:%d", t_current);
	ret = _alarmmgr_set_systime_helper(t_current);
	setting_retm_if(ret == -1, "_alarmmgr_set_systime_helper call failed");
	if (ad->data_time) {
		setting_enable_evas_object(ad->data_time->btn_left);
		setting_enable_evas_object(ad->data_time->btn_right);
	}

	/* time change - event */
	static int t_event_val = -1;
	vconf_set_int(VCONFKEY_SYSTEM_TIME_CHANGED, t_event_val);

	FREE(tzpath);
}


static void setting_time_main_bool_vconf_change_cb(keynode_t *key, void *data)
{
	/* SETTING_TRACE_BEGIN; */
	ret_if(NULL == data);
	SettingTimeUG *ad = data;
	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name,
			VCONFKEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE_BOOL)) {
		if (ad->data_auto) {
			ad->is_datefield_selected = EINA_FALSE;

			setting_update_gl_item_chk_status(ad->data_auto,
					status);

			if (!ad->data_auto->chk_status) {
				if (ad->data_tz) {
					setting_enable_genlist_item(
							ad->data_tz->item);
				}

				if (ad->data_time) {
					ad->data_time->isItemDisableFlag = FALSE;
					/*setting_ensable_evas_object(
					 * ad->data_time->eo_check); */
					setting_enable_evas_object(
							ad->data_time->btn_left);
					setting_enable_evas_object(
							ad->data_time->btn_right);
					setting_enable_genlist_item(
							ad->data_time->item);
				}


			} else {
				if (ad->data_tz) {
					setting_disable_genlist_item(
							ad->data_tz->item);
				}

				if (ad->data_time) {
					ad->data_time->isItemDisableFlag = FALSE;
					/*setting_disable_evas_object(
					 * ad->data_time->eo_check); */
					setting_disable_evas_object(
							ad->data_time->btn_left);
					setting_disable_evas_object(
							ad->data_time->btn_right);
					setting_disable_genlist_item(
							ad->data_time->item);
				}

				__update_time_via_sim_card(ad);
			}
		}
	}
}

static void __sim_time_info_chg_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(NULL == data);
	SettingTimeUG *ad = data;
	char *vconf_name = vconf_keynode_get_name(key);
	/*SETTING_TRACE("vconf_name:%s", vconf_name);*/
	if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_NITZ_GMT)
		|| !safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_NITZ_EVENT_GMT)
		|| !safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_NITZ_ZONE)) {
		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(
				BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
				&value, &err);
		if (value) {
			int ret = 0;
			int t_nitz = 0;
			int t_offset = 0;
			ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_GMT,
					&t_nitz);
			ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT,
					&t_offset);
			char *tzpath = vconf_get_str(
					VCONFKEY_TELEPHONY_NITZ_ZONE);
			if (ret != 0 || isEmptyStr(tzpath)) {
				if (ad->data_auto) {
					setting_disable_genlist_item(
							ad->data_auto->item);
				}
				if (ad->data_tz) {
					setting_enable_genlist_item(
							ad->data_tz->item);
				}
				if (ad->data_time) {
					setting_enable_genlist_item(
							ad->data_time->item);
				}
				return;
			}
			FREE(tzpath);

			if (ad->data_auto) {
				setting_enable_genlist_item(
						ad->data_auto->item);
			}
			if (ad->data_tz) {
				setting_disable_genlist_item(
						ad->data_tz->item);
			}
			if (ad->data_time) {
				setting_disable_genlist_item(
						ad->data_time->item);
			}

			__update_time_via_sim_card(ad);
		}
	}
}

#if 0
static int __setting_time_check_enable_automatic_update()
{
	/*to read TAPI vconfs */
	int ret = 0;
	int t_nitz = 0;
	int t_offset = 0;
	ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_GMT, &t_nitz);
	ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT, &t_offset);
	char *tzpath = vconf_get_str(VCONFKEY_TELEPHONY_NITZ_ZONE);
	if (ret == 0 && !isEmptyStr(tzpath))
		return 1;
	else
		return 0;
}
#endif

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static void setting_time_main_datefield_set_cb(void *data, Evas_Object *object,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("SETTING_DATETIME");


	/*Setting_GenGroupItem_Data *item_data = param->item_data; */
	/* error check */
	ret_if(data == NULL);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) data;
	SettingTimeUG *ad = list_item->userdata;

	Evas_Object *obj = list_item->eo_check;	/* elm_datetime_add */

	if (obj) {
		elm_object_signal_emit(list_item->eo_check /*elm_datetime_xxx*/,
				"picker,action,value,set", "");
	} else {
		SETTING_TRACE_ERROR("list_item->eo_check is NULL");
	}

	ad->is_datefield_selected = EINA_TRUE;

#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
	if (ad->update_timer) {
		ecore_timer_reset(ad->update_timer);
		ecore_timer_freeze(ad->update_timer);
	}
#endif

	/* If auto update is on, don't anything while the datefield has been
	 * updated (i.e., time runs) */
	if (1 == setting_time_check_automatic_time_update_state()) {
		SETTING_TRACE("AUTO_TIME ON: no action");
		SETTING_TRACE_END;
#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
		if (ad->update_timer) {
			ecore_timer_thaw(ad->update_timer);
		}
#endif
		return;
	}

	struct tm _tm;
	elm_datetime_value_get(obj, &_tm);

	SETTING_TRACE("year : %d, month : %d, day : %d, hour : %d, min : %d",
				  _tm.tm_year, _tm.tm_mon, _tm.tm_mday,
				  _tm.tm_hour, _tm.tm_min);

	_tm.tm_isdst = -1;
	_tm.tm_sec = 0;

	/*	local time	-> gmtime */
	time_t the_time = mktime(&_tm);	/* represent local time */

	/* invoke API to change time */
	int ret = _alarmmgr_set_systime_helper(the_time);

#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
	if (ad->update_timer) {
		ecore_timer_thaw(ad->update_timer);
	}
#endif

#if 1
	time_t cctime = time(NULL);
	struct tm ts_ret;
	localtime_r(&cctime, &ts_ret);
	SETTING_TRACE("After _alarmmgr_set_systime_helper() ....year : %d, "
			"month : %d, day : %d, hour : %d, min : %d",
				  ts_ret.tm_year, ts_ret.tm_mon, ts_ret.tm_mday,
				  ts_ret.tm_hour, ts_ret.tm_min);
#endif
	Evas_Object *popup = NULL;
	popup = evas_object_data_get(obj, "popup");
	if (popup) {
		evas_object_del(popup);
		popup = NULL;
	}
	setting_retm_if(ret == -1, "_alarmmgr_set_systime_helper call failed");
}

#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
Eina_Bool __refresh_date_timer(void *data)
{
	SettingTimeUG *ad = data;

	__setting_update_datefield_cb(ad);
	return 1;
}
#endif

static int setting_time_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingTimeUG *ad = (SettingTimeUG *) cb;

	char *title = _(DATE_TIME_STR_DATE_AND_TIME);
	char *caller = NULL;
	ad->is_datefield_selected = EINA_FALSE;

	app_control_h service = ad->bundle_data;

	app_control_get_extra_data(service, "caller", &caller);
	app_control_get_extra_data(service, "title", &title);

	Evas_Object *scroller;	/*	scroller is a genlist */

	ad->caller = caller;

	title = DATE_TIME_STR_DATE_AND_TIME;
	gchar *title_str = NULL;

	if (title)
		title_str = title;

	ad->ly_main =
		setting_create_layout_navi_bar_genlist(
				ad->win_main_layout,
				ad->win_get, title_str,
				NULL,
				NULL,
				setting_time_main_click_softkey_left_cb,
				NULL, ad, &scroller,
				&ad->navi_bar);

	ad->scrl_main = scroller;
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF,
			ELM_SCROLLER_POLICY_OFF);

	int value;
	int err;
	int ret = 0;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE, &value,
			&err);

	/*int enable_automatic = __setting_time_check_enable_automatic_update();
	 * */
	bool auto_flag = value;

	if (auto_flag) { /*do special process only when auto mode */
		/*to read TAPI vconfs */
		int ret = 0;
		int t_nitz = 0;
		int t_offset = 0;
		ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_GMT, &t_nitz);
		ret += vconf_get_int(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT,
				&t_offset);
		char *tzpath = vconf_get_str(VCONFKEY_TELEPHONY_NITZ_ZONE);
		if (ret == 0 && !isEmptyStr(tzpath)) {
			/*1.to update timezone */
			SETTING_TRACE("tzpath : %s", tzpath);
			ret = _set_timezone_helper(tzpath);
			if (ret < 0) {
				SETTING_TRACE("tzpath is not valid.");
			} else
				SETTING_TRACE("_set_timezone_helper - "
						"successful : %s \n", tzpath);
			if (!__setting_set_city_tzone(tzpath)) {
				SETTING_TRACE("__setting_set_city_tzone ERROR");
			}
			get_city_name(tzpath);
			char *city = get_city_name_result();
			SETTING_TRACE("city:%s", city);
			ret = vconf_set_str(VCONFKEY_SETAPPL_CITYNAME_INDEX_INT,
					city);

			/* 2.to set current time */
			int t_uptime = 0;
			FILE *fp = fopen("/proc/uptime", "r");
			if (fp) {
				if (!fscanf(fp, "%d", &t_uptime)) {
					SETTING_TRACE_ERROR("fscanf error");
				}
				fclose(fp);
				fp = NULL;
			} else {
				SETTING_TRACE_ERROR("fopen error");
			}

			SETTING_TRACE("t_nitz:%d, t_offset:%d, t_uptime:%d",
					t_nitz, t_offset, t_uptime);
			time_t t_current = t_nitz + t_uptime - t_offset;
			SETTING_TRACE("t_current:%d", t_current);
			ret = _alarmmgr_set_systime_helper(t_current);
		}
		FREE(tzpath);
	}

	/*	UI automatic */
	/*if (!isEmulBin()) { */
	ad->data_auto =
		setting_create_Gendial_field_def(scroller,
				&(ad->itc_1text_1icon),
				setting_time_main_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_1ICON_1RADIO, NULL,
				NULL, value,
				DATE_TIME_STR_AUTOMATIC_UPDATE, NULL,
				setting_time_main_chk_btn_cb);
	if (ad->data_auto) {
		ad->data_auto->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_auto is NULL");
	}

	/*ADD_GL_HELP(scroller,SETTING_TIME_AUTO_UPDATE_DESC); */
	/*} */

	/* create DATE_AND_TIME */
	ad->data_time =
		setting_create_Gendial_field_def(scroller, &(ad->itc_layout),
				setting_time_main_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_LAYOUT_DATEFIELD,
				NULL, NULL, 0,
				DATE_TIME_STR_DATE_AND_TIME,
				NULL,
				NULL);

	if (ad->data_time) {
		ad->data_time->userdata = ad;
		ad->data_time->nf = ad->navi_bar;
		ad->data_time->isItemDisableFlag = auto_flag;
		ad->data_time->activated_cb = setting_time_main_datefield_set_cb;
		elm_genlist_item_select_mode_set(ad->data_time->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->data_time is NULL");
	}

	setting_set_i18n("worldclock", SETTING_TIME_SHARE_LOCAL_PATH);
	char time_zone_sub_str[CITY_BUF_SIZE + GMT_BUF_SIZE + 3] = { 0, };
	char *displayTimezone = NULL;

	displayTimezone = get_timezone_displayname();

	if (displayTimezone != NULL) {

		/* don't use hard coded value */
		/* get value from current timezone */

		char str_buf[GMT_BUF_SIZE] = {0, };
		get_gmt_offset(str_buf, GMT_BUF_SIZE);

		char *timezone_str = get_timezone_str();

		SETTING_TRACE("timezone : %s, displayTimezone :%s, "
				"gmt_offset : %s",
				timezone_str, displayTimezone, _(str_buf));
		ret = snprintf(time_zone_sub_str,
				CITY_BUF_SIZE + GMT_BUF_SIZE + 2, "%s, GMT %s",
				displayTimezone, _(str_buf));
		FREE(timezone_str);
		if (ret < 0) {
			SETTING_TRACE_ERROR("cannot snprintf");
		}
	} else {
		/* default code */
		ret = snprintf(time_zone_sub_str,
				CITY_BUF_SIZE + GMT_BUF_SIZE + 2,
				"Korean Standard Time, GMT +9:00");
		if (ret < 0) {
			SETTING_TRACE_ERROR("cannot snprintf");
		}
	}
	FREE(displayTimezone);

	/*	UI create time zone */
	ad->data_tz =
		setting_create_Gendial_field_def(scroller, &(ad->itc_2text_2),
				setting_time_main_mouse_up_Gendial_list_cb,
				ad, SWALLOW_Type_INVALID, NULL,
				NULL, 0, DATE_TIME_STR_TIME_ZONE,
				time_zone_sub_str, NULL);
	if (ad->data_tz) {
		ad->data_tz->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_tz is NULL");
	}

	/*	UI create "Time format" */
	value = VCONFKEY_TIME_FORMAT_12;
	setting_get_int_slp_key(INT_SLP_SETTING_REGIONFORMAT_TIME1224, &value,
			&err);
	ad->data_time_fmt = setting_create_Gendial_field_def(
			scroller,
			&(ad->itc_1text_1icon),
			setting_time_main_mouse_up_Gendial_list_cb,
			ad,
			SWALLOW_Type_1ICON_1RADIO,
			NULL,
			NULL,
			(value == VCONFKEY_TIME_FORMAT_24),
			"IDS_ST_MBODY_24_HOUR_CLOCK",
			NULL,
			setting_time_main_chk_btn_cb);
	if (ad->data_time_fmt) {
		__BACK_POINTER_SET(ad->data_time_fmt);
		ad->data_time_fmt->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->data_time_fmt is NULL");
	}
	setting_add_gl_help(scroller, "IDS_ST_SBODY_SHOW_THE_TIME_IN_24_HOUR_"
			"FORMAT_INSTEAD_OF_12_HOUR_HAM_PM_FORMAT");

#if APPLIED_DATATIME_DATA_FORMAT
	char *pa_date_format = get_pa_date_format_str();
	ad->data_date_fmt = setting_create_Gendial_exp_parent_field(scroller,
			&(ad->itc_2text_3_parent),
			setting_time_main_mouse_up_Gendial_list_cb,
			ad, SWALLOW_Type_INVALID,
			DATE_TIME_STR_TIME_FORMAT,
			pa_time_format);
	if (ad->data_date_fmt) {
		ad->data_date_fmt->int_slp_setting_binded =
			INT_SLP_SETTING_DATE_FORMAT;
	} else {
		SETTING_TRACE_ERROR("ad->data_date_fmt is NULL");
	}
	G_FREE(pa_date_format);
#endif

	if (auto_flag) {
		if (ad->data_tz) {
			setting_disable_genlist_item(ad->data_tz->item);
		}
		if (ad->data_time) {
			setting_disable_genlist_item(ad->data_time->item);
		}
	}

	setting_view_time_main.is_create = 1;


	ret = system_settings_set_changed_cb(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, system_settings_changed_timezone, (void*)ad);
	vconf_notify_key_changed(VCONFKEY_SYSTEM_TIME_CHANGED,
			time_changed_callback, (void *)ad);

	/* register vconf key to get event callback for converting 12/24H */
	vconf_notify_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
			setting_time_main_int_vconf_change_cb, ad);
	/* register vconf key to get event callback for date format */
	vconf_notify_key_changed(VCONFKEY_SETAPPL_DATE_FORMAT_INT,
			setting_time_main_int_vconf_change_cb, ad);

	vconf_notify_key_changed(VCONFKEY_TELEPHONY_NITZ_GMT,
			__sim_time_info_chg_cb, ad);
	vconf_notify_key_changed(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT,
			__sim_time_info_chg_cb, ad);
	vconf_notify_key_changed(VCONFKEY_TELEPHONY_NITZ_ZONE,
			__sim_time_info_chg_cb, ad);

	vconf_notify_key_changed(
			VCONFKEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE_BOOL,
			setting_time_main_bool_vconf_change_cb, ad);

	/* to avoid update time-diff, every per 3s to update. */
#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
	ad->update_timer = ecore_timer_add(3,
			(Ecore_Task_Cb) __refresh_date_timer, ad);
#endif

	return SETTING_RETURN_SUCCESS;
}

static int setting_time_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingTimeUG *ad = (SettingTimeUG *) cb;

	/*	__finalize_resource(ad); */
	if (ad->caller)
		FREE(ad->caller);

#ifdef USE_TIMER_UPDATE_TIME_IN_TIME_VIEW
	if (ad->update_timer) {
		ecore_timer_del(ad->update_timer);
		ad->update_timer = NULL;
	}
#endif

#if PROGRESS_POPUP_SHOW
	if (ad->pop_progress) {
		evas_object_del(ad->pop_progress);
		ad->pop_progress = NULL;
	}
#endif

	if (ad->update_timezone_idler) {
		ecore_idler_del(ad->update_timezone_idler);
		ad->update_timezone_idler = NULL;
	}

	/* timer */
	if (ad->refresh_time_idler) {
		ecore_idler_del(ad->refresh_time_idler);
		ad->refresh_time_idler = NULL;
	}

	int ret = system_settings_unset_changed_cb(
			SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE);
	if (ret)
		SETTING_TRACE_ERROR("Cannot unregister callback");

	vconf_ignore_key_changed(VCONFKEY_SYSTEM_TIME_CHANGED,
			time_changed_callback);

	vconf_ignore_key_changed(VCONFKEY_REGIONFORMAT_TIME1224,
			setting_time_main_int_vconf_change_cb);
	vconf_ignore_key_changed(VCONFKEY_SETAPPL_DATE_FORMAT_INT,
			setting_time_main_int_vconf_change_cb);

	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_NITZ_GMT,
			__sim_time_info_chg_cb);
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_NITZ_EVENT_GMT,
			__sim_time_info_chg_cb);
	vconf_ignore_key_changed(VCONFKEY_TELEPHONY_NITZ_ZONE,
			__sim_time_info_chg_cb);

	vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE_BOOL,
			setting_time_main_bool_vconf_change_cb);

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		setting_view_time_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_time_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingTimeUG *ad = (SettingTimeUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);
		/* update timezone */
		if (ad->update_timezone_idler) {
			ecore_idler_del(ad->update_timezone_idler);
			ad->update_timezone_idler = NULL;
		}
		ad->update_timezone_idler =
			ecore_idler_add((Ecore_Task_Cb) __update_timezone_idler,
					ad);
		/* update time */
		if (ad->refresh_time_idler) {
			ecore_idler_del(ad->refresh_time_idler);
			ad->refresh_time_idler = NULL;
		}
		ad->refresh_time_idler = ecore_idler_add(
				setting_update_datefield_cb, ad);
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_time_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}


/* ***************************************************
 **
 **general func
 **
 ****************************************************/

int setting_time_check_automatic_time_update_state()
{
	SETTING_TRACE_BEGIN;
	int bvalue = 0;
	int err = 0;

	setting_get_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
			&bvalue, &err);

	return bvalue;

}

static int __setting_set_city_tzone(const char *pTZPath)
{
	/* let's get city & timezone string from pTZPath */
	/* 1. city name */
	char szTimezone[GMT_BUF_SIZE] = {0,};
	char *pStr = strdup(pTZPath);

	/* 2. timezone string +/-<n> ex. +9, -1 */
	get_gmt_offset(szTimezone, GMT_BUF_SIZE);

	/* set timezone_id */
	/** @todo replace with vconf ID */
	//vconf_set_str(VCONFKEY_SETAPPL_TIMEZONE_ID, pStr);
	FREE(pStr);
	return TRUE;
}

void setting_time_main_launch_worldclock_destroy_ug_cb(ui_gadget_h ug,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	/* if(ug) ug_destroy(ug); */
	/* restore the '<-' button on the navigate bar */
	ret_if(!priv);
	SettingTimeUG *ad = (SettingTimeUG *) priv;
	if (ug) {
		setting_ug_destroy(ug);
		ad->ug_loading = NULL;
	}
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(ad->navi_bar);
	ret_if(!navi_it);

	elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);
}

void setting_time_main_launch_worldclock_layout_ug_cb(
		ui_gadget_h ug,
		enum ug_mode mode,
		void *priv)
{
	SETTING_TRACE_BEGIN;
	/*SettingTimeUG *ad = (SettingTimeUG *) priv; */
	Evas_Object *base;

	if (!priv)
		return;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		/*elm_win_resize_object_add(ad->win_get, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}
	SETTING_TRACE_END;
}

Eina_Bool ___time_freeze_event_timer_cb(void *cb)
{
	SettingTimeUG *ad = (SettingTimeUG*)cb;

	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, EINA_FALSE);

	evas_object_freeze_events_set(ad->navi_bar, EINA_FALSE);
	ad->event_freeze_timer = NULL;

	return EINA_FALSE;
}

static void _worldclock_reply_cb(app_control_h request, app_control_h reply,
		app_control_result_e result, void *user_data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(user_data == NULL, "Data parameter is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) user_data;

	char *city = NULL;
	char *tzpath = NULL;
	app_control_get_extra_data(reply, "city", &city);
	app_control_get_extra_data(reply, "tzpath", &tzpath);

	if (!tzpath) {
		SETTING_TRACE("tzpath from worldclock UG is null.");
		setting_update_timezone(ad);
		return;
	} else {
		SETTING_TRACE("tzpath : %s", tzpath);
	}
	SETTING_TRACE("city : %s", city);

	/* ----------------------------------------------------------------- */

	SETTING_TRACE("tzpath:(%s)", tzpath);

	int ret = _set_timezone_helper(tzpath);
	if (ret < 0) {
		SETTING_TRACE("tzpath is not valid.");
		if (tzpath)
			FREE(tzpath);
		if (city)
			FREE(city);
		return;
	} else
		SETTING_TRACE("_set_timezone_helper - successful : "
				"%s \n", tzpath);

	ret = vconf_set_str(VCONFKEY_SETAPPL_CITYNAME_INDEX_INT, city);
	setting_retm_if(ret != 0, "set vconf failed");

	/* parse city and GMT offset from tzpath and system time property */
	/* set the strings in vconf which will be used while updating display
	 * of timezone */
	if (!__setting_set_city_tzone(tzpath)) {
		SETTING_TRACE("__setting_set_city_tzone ERROR");
		if (tzpath)
			FREE(tzpath);
		if (city)
			FREE(city);
		return;
	}

	/* update the display for timezone */
	setting_update_timezone(ad);
	static int t_event_val = -1;
	vconf_set_int(VCONFKEY_SYSTEM_TIME_CHANGED, t_event_val);
	if (tzpath)
		FREE(tzpath);
	if (city)
		FREE(city);

	SETTING_TRACE_END;
}

void setting_time_main_launch_worldclock_sg(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	/* ad is point to data */
	SettingTimeUG *ad = (SettingTimeUG *) data;

	if (0 == app_launcher("org.tizen.worldclock-efl", _worldclock_reply_cb, ad)) {
		if (ad->event_freeze_timer) {
			ecore_timer_del(ad->event_freeze_timer);
			ad->event_freeze_timer = NULL;
		}
		ad->event_freeze_timer = ecore_timer_add(
				1, ___time_freeze_event_timer_cb, ad);
		evas_object_freeze_events_set(ad->navi_bar, EINA_TRUE);
	}
}

static void __setting_update_datefield_cb(void *cb)
{
	retm_if(cb == NULL, "Data parameter is NULL");
	SettingTimeUG *ad = (SettingTimeUG *) cb;

	if (!ad->data_time) {
		SETTING_TRACE_ERROR("data_time is null");
		return;
	}

	time_t cctime = time(NULL);
	struct tm ts_ret;
	struct tm *ts = localtime_r(&cctime, &ts_ret);
	/* SETTING_TRACE("year : %d, month : %d, day : %d, hour : %d, "
			"min : %d",
			ts_ret.tm_year, ts_ret.tm_mon, ts_ret.tm_mday,
			ts_ret.tm_hour, ts_ret.tm_min); */
	if (!ts) {
		SETTING_TRACE_ERROR("Cannot get local time");
		return;
	}

	char *region = NULL;
	int err = system_settings_get_value_string(
			SYSTEM_SETTINGS_KEY_LOCALE_COUNTRY,
			&region);
	if (err != SYSTEM_SETTINGS_ERROR_NONE) {
		SETTING_TRACE_ERROR("Failed to call system_settings_get_value_"
				"string with error code %d", err);
	}
	char *date = __setting_phone_lang_get_by_pattern(region, "MMM/d/yyyy");
	char *time;
	static char date_arr[DEF_BUF_SIZE];
	static char time_arr[DEF_BUF_SIZE];
	char *pa_time_format = get_pa_time_format_str();
	setting_retm_if(NULL == pa_time_format, "pa_time_format is NULL");
	if (!strncmp(pa_time_format, "IDS_ST_BODY_12_HOURS",
			strlen("IDS_ST_BODY_12_HOURS")))
		time = __setting_phone_lang_get_by_pattern(region, "hhmm");
	else
		time = __setting_phone_lang_get_by_pattern(region, "Hm");

	strncpy(date_arr, date, DEF_BUF_SIZE);
	strncpy(time_arr, time, DEF_BUF_SIZE);

	date_arr[DEF_BUF_SIZE - 1] = '\0';
	time_arr[DEF_BUF_SIZE - 1] = '\0';

	if (ad->data_time->btn_left) {
		elm_object_text_set(ad->data_time->btn_left, date_arr);
	} else {
		SETTING_TRACE_ERROR("ad->data_time->btn_left is NULL");
	}
	if (ad->data_time->btn_right) {
		elm_object_text_set(ad->data_time->btn_right, time_arr);
	} else {
		SETTING_TRACE_ERROR("ad->data_time->btn_right is NULL");
	}

	FREE(pa_time_format);
}

Eina_Bool setting_update_datefield_cb(void *data)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retvm_if(data == NULL, FALSE, "Data parameter is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) data;

	__setting_update_datefield_cb(ad);

	ad->refresh_time_idler = NULL;
	return ECORE_CALLBACK_CANCEL;
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/

void
setting_time_main_click_softkey_left_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) data;

	/*__finalize_resource(ad); */

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

/* ***************************************************
 **
 **call back func
 **
 ****************************************************/

void
setting_time_main_click_softkey_caller_exist_left_cb(void *data,
		Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) data;

	/*	Create Bundle and send message */
	app_control_h svc;
	if (app_control_create(&svc))
		return;

	app_control_add_extra_data(svc, "result", "lbutton_click");
	ug_send_result(ad->ug, svc);

	app_control_destroy(svc);

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

void
setting_time_main_click_softkey_caller_exist_right_cb(void *data,
		Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) data;

	/*	Create Bundle and send message */
	app_control_h svc;
	if (app_control_create(&svc))
		return;

	app_control_add_extra_data(svc, "result", "rbutton_click");
	ug_send_result(ad->ug, svc);

	app_control_destroy(svc);

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}


static void __time_auto_update(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingTimeUG *ad = (SettingTimeUG *) data;
	int err;
	setting_set_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
			SETTING_ON_OFF_BTN_ON, &err);
	if (ad->data_tz) {
		setting_disable_genlist_item(ad->data_tz->item);
	}

#if 0
	elm_object_text_set(ad->data_time->btn_left, date);
	evas_object_resize(ad->data_time->btn_left, 300, -1);
#endif

#if 1
	if (ad->data_time) {
		ad->data_time->isItemDisableFlag = FALSE;
		/*setting_disable_evas_object(ad->data_time->eo_check); */
		setting_disable_evas_object(ad->data_time->btn_left);
		setting_disable_evas_object(ad->data_time->btn_right);
		setting_disable_genlist_item(ad->data_time->item);
	}
#endif
	__update_time_via_sim_card(ad);
	SETTING_TRACE_END;
}

static void
setting_time_main_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingTimeUG *ad = NULL;
	Setting_GenGroupItem_Data *list_item =
		(Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	/* for genlist update status */
	list_item->chk_status = elm_check_state_get(obj);

	int err;
	if (obj == ad->data_auto->eo_check) {
		int bvalue;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
								 &bvalue, &err);
		if (bvalue) {
			(void) setting_set_bool_slp_key(
					BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
					SETTING_ON_OFF_BTN_OFF, &err);
			setting_update_gl_item_chk_status(ad->data_auto, 0);

			if (ad->data_tz) {
				setting_enable_genlist_item(ad->data_tz->item);
			}

			if (ad->data_time) {
				ad->data_time->isItemDisableFlag = FALSE;
				/*setting_disable_evas_object(
				 * ad->data_time->eo_check); */
				setting_enable_evas_object(
						ad->data_time->btn_left);
				setting_enable_evas_object(
						ad->data_time->btn_right);
				setting_enable_genlist_item(
						ad->data_time->item);
			}
		} else {
			__time_auto_update(ad);
		}
	}



	/*#define FUNCTION_SYSTEM_SETTING 1 */
#if FUNCTION_SYSTEM_SETTING
	/* Time format */
	if (ad->data_time_fmt == list_item) {
		if (list_item->chk_status) {
			int ret = SYSTEM_SETTINGS_ERROR_NONE;
			ret = system_settings_set_value_bool(
					SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR,
					true);
			if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
				SETTING_TRACE("Failed to call system_settings_"
						"set_value_bool with error "
						"code %d", ret);
			}
		} else {
			int ret = SYSTEM_SETTINGS_ERROR_NONE;
			ret = system_settings_set_value_bool(
					SYSTEM_SETTINGS_KEY_LOCALE_TIMEFORMAT_24HOUR,
					false);
			if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
				SETTING_TRACE("Failed to call system_settings_"
						"set_value_bool with error "
						"code %d", ret);
			}
		}
	}
#else
	/* Time format */
	if (ad->data_time_fmt == list_item) {
		int err = 0;
		int value = VCONFKEY_TIME_FORMAT_12;
		if (list_item->chk_status) {
			value = VCONFKEY_TIME_FORMAT_24;
		}
		setting_set_int_slp_key(INT_SLP_SETTING_REGIONFORMAT_TIME1224,
				value, &err);
	}
#endif
	SETTING_TRACE_END;
}

static void setting_time_main_mouse_up_Gendial_list_cb(
		void *data,
		Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
		(Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == list_item, "list_item is NULL");

	SettingTimeUG *ad = (SettingTimeUG *) data;
	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	if (ad->data_tz == list_item) {
		ad->is_datefield_selected = EINA_FALSE;
		if (1 == setting_time_check_automatic_time_update_state()) {
			setting_create_popup(ad, ad->win_get, NULL,
					_(AUTO_TIME_UPDATE_ON), NULL, 0, false,
					false, 0);
		} else {
			setting_time_main_launch_worldclock_sg(ad);
		}

	} else if (ad->data_time == list_item) {
		ad->is_datefield_selected = EINA_TRUE;
		if (1 == setting_time_check_automatic_time_update_state()) {
			setting_create_popup(ad, ad->win_get, NULL,
					_(AUTO_TIME_UPDATE_ON), NULL, 0, false,
					false, 0);
		} else {
			/* setting_view_change(&setting_view_time_main,
			 * &setting_view_time_time, ad); */
		}
	} else if (ad->data_auto == list_item) {
		ad->is_datefield_selected = EINA_FALSE;
		int old_status = list_item->chk_status;
		/* new status */
		setting_update_gl_item_chk_status(list_item, !old_status);
		int err = 0;
		if (!list_item->chk_status) {
			(void)
			setting_set_bool_slp_key
			(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
			 SETTING_ON_OFF_BTN_OFF, &err);

			if (ad->data_tz) {
				setting_enable_genlist_item(ad->data_tz->item);
			}

			if (ad->data_time) {
				ad->data_time->isItemDisableFlag = FALSE;
				setting_enable_evas_object(
						ad->data_time->eo_check);
				setting_enable_genlist_item(
						ad->data_time->item);
			}

		} else {
			__time_auto_update(ad);
		}
	} else if (ad->data_time_fmt == list_item)	{
		int err = 0;
		setting_update_gl_item_chk_status(list_item,
				!list_item->chk_status);
		int value = VCONFKEY_TIME_FORMAT_12;
		if (list_item->chk_status)
			value = VCONFKEY_TIME_FORMAT_24;
		setting_set_int_slp_key(INT_SLP_SETTING_REGIONFORMAT_TIME1224,
				value, &err);
	}
}


void setting_time_main_btn_update_ok_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) cb;
	int err;		/* error check */
	setting_set_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
			SETTING_ON_OFF_BTN_ON, &err);
	setting_update_gl_item_chk_status(ad->data_auto, 1);

	deviced_request_reboot();

	/* We'd better check return value from deviced_request_reboot()
	 * function. */
	/* It will return 0 on success or -1 if failed. */
}


void setting_time_main_btn_update_cancel_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	SettingTimeUG *ad = (SettingTimeUG *) cb;

	int err;		/* error check */

	retm_if(cb == NULL, "Data parameter is NULL");
	setting_set_bool_slp_key(BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,
			SETTING_ON_OFF_BTN_OFF, &err);
	setting_update_gl_item_chk_status(ad->data_auto, 0);

}


static UChar *uastrcpy(const char *chars)
{
	int len = 0;
	UChar *str = NULL;
	len = safeStrLen(chars);
	str = (UChar *) malloc(sizeof(UChar)*(len + 1));
	if (!str)
		return NULL;
	u_uastrcpy(str, chars);
	return str;
}

static void ICU_set_timezone(const char *timezone)
{
	SETTING_TRACE("ICU_set_timezone = %s ", timezone);
	UErrorCode ec = U_ZERO_ERROR;
	UChar *str = uastrcpy(timezone);

	if (str) {
		ucal_setDefaultTimeZone(str, &ec);
		if (U_SUCCESS(ec)) {
			SETTING_TRACE("ucal_setDefaultTimeZone() SUCCESS ");
		} else {
			SETTING_TRACE("ucal_setDefaultTimeZone() FAILED : %s ",
					u_errorName(ec));
		}
		FREE(str);
	}
}

/**
 * ex) timezone : Asia/Seoul --> returns "+09"
 *
 * char outstr2[4];
 */
static void get_gmt_offset(char *str_buf, int size)
{
	/* timezone string +/-<n> ex. +9, -1 */
	time_t t = time(0);		/* get unix time. sec. */

	struct tm *pdata, data;
	pdata = localtime_r(&t, &data);		/* save time as structure. */

	setting_retm_if(!pdata, "data is NULL");

	int gmtoffset_hour = (pdata->tm_gmtoff) / 3600;	/* result: hour */
	int gmtoffset_min = ((pdata->tm_gmtoff) % 3600) / 60; /* result: min */

#if 0
	if (gmtoffset_min != 0) {
		SETTING_TRACE("gmtoffset_min != 0 -------> %d", gmtoffset_min);
		gmtoffset_min = 30;
	}
#endif

	/*snprintf(str_buf, size, "%+d:%02u", gmtoffset_hour, gmtoffset_min); */
	snprintf(str_buf, size, "%+d:%02d", gmtoffset_hour, abs(gmtoffset_min));
	SETTING_TRACE_DEBUG("szTimezone is of a valid format: GMT: %s",
			str_buf);

	int ret = vconf_set_str(VCONFKEY_SETAPPL_TIMEZONE_INT, str_buf);
	if (ret == -1) {
		SETTING_TRACE_ERROR("set vconf failed");
	}

}


/* automatic */
static char *get_timezone_str()
{
	SETTING_TRACE_BEGIN;
	char *str = NULL;
	int ret = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_LOCALE_TIMEZONE, &str);
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		SETTING_TRACE("Failed to call system_settings_get_"
				"value_string with error code %d", ret);
	}
	return g_strdup(str);		/*Asia/Seoul */
}

static char *get_city_name_result()
{
	return s_result;
}


/* in_str = "Asia/Seoul" */
static bool get_city_name(char *tzname)
{
	sqlite3 *pSQLite3 = NULL;
	char	*szErrMsg = NULL;

	int rst = sqlite3_open(_TZ_SYS_DB"/.worldclock.db", &pSQLite3);
	if (rst) {
		SETTING_TRACE("Can't open database: %s",
				sqlite3_errmsg(pSQLite3));
		sqlite3_close(pSQLite3);
		pSQLite3 = NULL;
		return false;
	} else {
		SETTING_TRACE("Database opened!!");
		char query_str[DEF_BUF_SIZE];
		snprintf(query_str, DEF_BUF_SIZE, "SELECT city FROM city_table"
				" where tzpath=\"%s\"", tzname);
		SETTING_TRACE("%s \n", query_str);
		rst = sqlite3_exec(pSQLite3, query_str, query_callback, 0,
				&szErrMsg);
	}

	sqlite3_free(szErrMsg);
	sqlite3_close(pSQLite3);
	SETTING_TRACE("Database close!!\n");
	return true;
}

static int query_callback(void *NotUsed, int argc, char **argv,
		char **azColName)
{
	int i = 0;
	SETTING_TRACE("%s\n", argv[i] ? argv[i] : "NULL");
	s_result = g_strdup(argv[i] ? argv[i] : "NULL");
	return 0;
};

static char *get_timezone_displayname()
{
	SETTING_TRACE_BEGIN;

	UErrorCode status = U_ZERO_ERROR;;
	UChar displayName[SETTING_STR_SLP_LEN] = {0, };
	char *timezone_str = NULL;
	const char *localeID = uloc_getDefault();

	time_t t = time(0);
	struct tm *pdata, data;

	pdata= localtime_r(&t, &data);

	ICU_set_timezone(get_timezone_str());

	UCalendar *cal = ucal_open(NULL, -1, localeID, UCAL_GREGORIAN, &status);
	if (U_FAILURE(status))
		SETTING_TRACE_ERROR("ucal_open got an error : %d", status);

	status = U_ZERO_ERROR;

	/* get timezone display name (check dst) */
	if (cal) {
		if (pdata) {
			if (pdata->tm_isdst)
				ucal_getTimeZoneDisplayName(cal, UCAL_DST,
						localeID, displayName,
						SETTING_STR_SLP_LEN, &status);
			else
				ucal_getTimeZoneDisplayName(cal, UCAL_STANDARD,
						localeID, displayName,
						SETTING_STR_SLP_LEN, &status);
		}

		if (U_FAILURE(status)) {
			SETTING_TRACE_ERROR("ucal_getTimeZoneDisplayName got "
					"an error : %s, %d",
					displayName, status);
			status = U_ZERO_ERROR;
		} else {
			int len = 0;
			int len_str = 0;
			int len_utf8 = 0;

			len = u_strlen(displayName);
			len_str = sizeof(char)*4*(len+1);
			timezone_str = (char *)calloc(1, len_str);
			if (!timezone_str) {
				SETTING_TRACE_ERROR("timezone string is null");
				return NULL;
			}
			u_strToUTF8(timezone_str, len_str, &len_utf8,
					displayName, len, &status);
			SETTING_TRACE("*************************** timezone : "
					"%s, %s", displayName, timezone_str);
		}
		ucal_close(cal);
	}

	SETTING_TRACE_END;
	return timezone_str;

}

