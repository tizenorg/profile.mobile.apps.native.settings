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
#include <setting-time-engine.h>

/* To set system time takes some time. It makes some time before firing
 * vconf event */
int setting_time_update_cb(void *data)
{
	retv_if(data == NULL, -1);
	SettingTimeUG *ad = data;
	/*
	char time_str[SETTING_TIME_ENGINE_TIME_STR_LEN];
	struct tm *ts;
	struct tm ts_ret;
	time_t ctime;
	// get time from datefield UI
	ctime = ad->changed_time;
	//ctime = time(NULL);
	ts = localtime_r(&ctime, &ts_ret);
	retv_if(!ts, 1);
	strftime(time_str, SETTING_TIME_ENGINE_TIME_STR_LEN, "%H:%M", &ts_ret);
	SETTING_TRACE("After setting time ::: tm is_dst --> %d",
			ts_ret.tm_isdst);
	SETTING_TRACE("After Setting time time_str:%s", time_str);*/
#if SUPPORT_SCREEN_PROTECTED
	display_unlock_state(LCD_NORMAL, STAY_CUR_STATE);
#endif

	if (ad->pop_progress) {
		evas_object_del(ad->pop_progress);
		ad->pop_progress = NULL;
	}
	return 0;
}
