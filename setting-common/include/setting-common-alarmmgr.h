/*
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	   http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __SETTING_COMMON_ALARMMGR_H__
#define __SETTING_COMMON_ALARMMGR_H__

#include <alarm.h>
#include <bundle.h>
#include <time.h>
#include <vconf.h>

typedef struct bm_alarm_data BM_AData;
struct bm_alarm_data {
	int alarm_mgr_id;	/**< index from alarm-manager */
	int hour;
	int min;
};

#define SET_BM_TIME_DATA_T(pTime_data, YEAR, MONTH, DAY, HOUR, MIN, SEC) \
	do {\
		(pTime_data)->year = (YEAR);\
		(pTime_data)->month = (MONTH);\
		(pTime_data)->day = (DAY);\
		(pTime_data)->hour = (HOUR);\
		(pTime_data)->min = (MIN);\
		(pTime_data)->sec = (SEC);\
	} while (0)

#define SETTING_BM_ALARM_APP "org.tizen.setting-blockingmode-alarm"
int setting_common_alarmmgr_create(BM_AData *alarm);
int setting_common_alarmmgr_remove(BM_AData *alarm);
int setting_common_alarmmgr_update(BM_AData *alarm);

#endif				/*__SETTING_COMMON_ALARMMGR_H__ */