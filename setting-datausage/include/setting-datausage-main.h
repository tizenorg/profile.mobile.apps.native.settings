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
#ifndef __SETTING_DATAUSAGE_MAIN_H__
#define __SETTING_DATAUSAGE_MAIN_H__

#include <setting-datausage.h>

#define SETTING_DATAUSAGE_DESC_LEN_MAX 256

#define SETTING_DATAUSAGE_GRAPH_W ((int)680 * WIDGET_SCALE_FACTOR)
#define SETTING_DATAUSAGE_GRAPH_H ((int)500 * WIDGET_SCALE_FACTOR)

#define SETTING_DATAUSAGE_CAIRO_W ((int)630 * WIDGET_SCALE_FACTOR)
#define SETTING_DATAUSAGE_CAIRO_H 630

#define SETTING_DATAUSAGE_DATA_LIMIT_GRAPH_HEIGHT 160

#define SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X ((int)15 * WIDGET_SCALE_FACTOR)
#define SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y 10
#define SETTING_DATAUSAGE_ROUND_RECT_WIDTH ((int)600 * WIDGET_SCALE_FACTOR)
#define SETTING_DATAUSAGE_ROUND_RECT_HEIGHT 130
#define SETTING_DATAUSAGE_ROUND_RECT_R 20
#define SETTING_DATAUSAGE_ROUND_RECT_BG_COLOR 187
#define SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED 143
#define SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN 148
#define SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE 97

#define SETTING_DATAUSAGE_HEAD_POINT_X ((int)70 * WIDGET_SCALE_FACTOR)
#define SETTING_DATAUSAGE_HEAD_POINT_Y 30

#define SETTING_DATAUSAGE_PILLAR_WIDTH 18

#define SETTING_DATAUSAGE_CHART_WIDTH (SETTING_DATAUSAGE_PILLAR_WIDTH * SETTING_DATAUSAGE_DAYS_IN_MONTH)
#define SETTING_DATAUSAGE_CHART_HEIGHT 400

#define SETTING_DATAUSAGE_HORIZONTAL_GROUP 5        /* group count in HORIZONTAL */
#define SETTING_DATAUSAGE_LINES_IN_HORIZONTAL_GROUP 5 /* line count in per group in HORIZONTAL */

#define SETTING_DATAUSAGE_HORIZONTAL_LINE_COUNT (SETTING_DATAUSAGE_HORIZONTAL_GROUP * SETTING_DATAUSAGE_LINES_IN_HORIZONTAL_GROUP)
#define SETTING_DATAUSAGE_LINE_TO_LINE_SPACE (SETTING_DATAUSAGE_CHART_HEIGHT / SETTING_DATAUSAGE_HORIZONTAL_LINE_COUNT)


#define SETTING_DATAUSAGE_KILOBYTE_VALUE 1024
#define SETTING_DATAUSAGE_MEGABYTE_VALUE 1048576

#define SETTING_DATAUSAGE_ANIMATOR_START_INDEX 5

static void __setting_datausage_main_click_softkey_back_cb(void *data, Evas_Object *obj,
					void *event_info);
static void __setting_datausage_main_data_items_append(void *data);
static void __setting_datausage_main_cairo_destroy(void *data);
static int __setting_datausage_main_days_in_month_get(time_t *start, time_t *end);
static int __setting_datausage_main_days_in_week_get(time_t *start, time_t *end);

#endif				/* __SETTING_DATAUSAGE_MAIN_H__ */
