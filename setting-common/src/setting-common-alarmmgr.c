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
#include <setting-debug.h>
#include <setting-common-alarmmgr.h>
#define EXPORT_PUBLIC __attribute__((visibility("default")))


/**********************************************************************
******************Global function ref*************************************
***********************************************************************/

/**
* send
* This function is	used to create mgr
* @param		data[in]			pointer to AData
* @return		when success, return EINA_TRUE or EINA_FALSE if error
* @exception
*/
EXPORT_PUBLIC
int setting_common_alarmmgr_create(BM_AData *alarm)
{
	/*SETTING_TRACE_BEGIN; */
	int nErr = -1;	/*no err */
	alarm_entry_t *alarm_entry = NULL;
	alarm_date_t alarm_data;

	/*create alarm */
	alarm_entry = alarmmgr_create_alarm();
	/*set repeat */
	int repeat_value = 0;
	alarm_repeat_mode_t repeat_mode = 0;
	setting_retvm_if(NULL == alarm_entry, 0, "alarm_entry is NULL");

	nErr = alarmmgr_set_repeat_mode(alarm_entry, repeat_mode, repeat_value);

	/*set time_data */
	struct tm pt;
	memset(&pt, 0, sizeof(struct tm));
	time_t ctime = time(NULL);
	if (NULL == localtime_r(&ctime, &pt)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
	}

	SETTING_TRACE("alarm->hour[%d], alarm->min[%d]", alarm->hour,
			alarm->min);
	pt.tm_hour = alarm->hour;
	pt.tm_min = alarm->min;
	SET_BM_TIME_DATA_T(&alarm_data, pt.tm_year + 1900, pt.tm_mon + 1,
			pt.tm_mday, pt.tm_hour, pt.tm_min, 0);
	nErr |= alarmmgr_set_time(alarm_entry, alarm_data);

	/*set type	 */
	nErr = alarmmgr_set_type(alarm_entry, ALARM_TYPE_DEFAULT);

	/* bundle */
	/*bundle *b = NULL; */
	/*b = bundle_create(); */
	/*appsvc_set_operation(b, APPSVC_OPERATION_DEFAULT); */
	/*appsvc_set_pkgname(b, SETTING_BM_ALARM_APP); */

	/*create new	*/
	int alarm_mgr_id = 0;
	nErr = alarmmgr_add_alarm_with_localtime(alarm_entry,
			SETTING_BM_ALARM_APP, &alarm_mgr_id);
	/*nErr = alarmmgr_add_alarm_appsvc_with_localtime(
	 * alarm_entry, (void *)b, &alarm_mgr_id); */
	SETTING_TRACE("alarm_mgr_id [%d]", alarm_mgr_id);
	if (nErr) {
		SETTING_TRACE("*** [ERR] alarmmgr_add_alarm_with_localtime failed ***");
	}

	alarm->alarm_mgr_id = alarm_mgr_id;
	/*vconf_set_int(VCONFKEY_SETAPPL_BM_ALARM_ID_START,
	 * alarm->alarm_mgr_id); */

	if (alarm_entry) {
		alarmmgr_free_alarm(alarm_entry);
	}
	return nErr;
}

/**
* send
* This function is	used to update mgr
* @param		   data[in]			pointer to AData
* @return		   when success, return EINA_TRUE or EINA_FALSE if error
* @exception
*/
EXPORT_PUBLIC
int setting_common_alarmmgr_update(BM_AData *alarm)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = -1;
	ret = setting_common_alarmmgr_create(alarm);
	SETTING_TRACE("*** [ERR] setting_common_alarmmgr_update failed ***");

	return ret;
}
