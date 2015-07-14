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
#include <setting-datausage-main.h>
#include <ail.h>
#include <math.h>

#define UNKNOWN_APP "Unknown"
#define DATAUSAGE_POPUP_GUIDE_TEXT "<font_size=32><color=#FF0000>Max limit is 99999</color></font_size>"

const char* data_usage_cycle_arr[] = {
		SETTING_DATAUSAGE_MONTH_STR,
		SETTING_DATAUSAGE_WEEK_STR,
		SETTING_DATAUSAGE_DAY_STR,
		NULL,
	};

const char *days_in_month[] = {
		"01", "02", "03", "04", "05", "06",
		"07", "08", "09", "10", "11", "12",
		"13", "14", "15", "16", "17", "18",
		"19", "20", "21", "22", "23", "24",
		"25", "26", "27", "28", "29", "30",
		"31",
};

static int setting_datausage_main_create(void *cb);
static int setting_datausage_main_destroy(void *cb);
static int setting_datausage_main_update(void *cb);
static int setting_datausage_main_cleanup(void *cb);

setting_view setting_view_datausage_main = {
	.create = setting_datausage_main_create,
	.destroy = setting_datausage_main_destroy,
	.update = setting_datausage_main_update,
	.cleanup = setting_datausage_main_cleanup,
};

static void __setting_datausage_main_data_items_recreate(void *data);

static int __setting_datausage_main_size_transition(unsigned long long size, char *size_desc)
{
	//SETTING_TRACE_BEGIN;
	//SETTING_TRACE("size = %lld", size);
	double tmp_size = 0.0;
	int ret = 0;
	if (size < SETTING_DATAUSAGE_MEGABYTE_VALUE) {	// size < 1MB: show x.xKB
		tmp_size = ((double)size) / SETTING_DATAUSAGE_KILOBYTE_VALUE;
		ret = snprintf(size_desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%4.2lf%s", tmp_size, "KB");
		retv_if(ret < 0, SETTING_RETURN_FAIL);
	} else {	// show x.xMB
		tmp_size = ((double)size) / SETTING_DATAUSAGE_MEGABYTE_VALUE;
		ret = snprintf(size_desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%4.2lf%s", tmp_size, "MB");
		retv_if(ret < 0, SETTING_RETURN_FAIL);
	}

	return SETTING_RETURN_SUCCESS;
}

static void __setting_datausage_main_left_time_desc_get(char *desc)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == desc, "desc is NULL");

	int err = 0;
	int cycle = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	time_t cur_time = time(0);
	time_t end_time;
	switch (cycle) {
	case 0: /* month */
		{
		__setting_datausage_main_days_in_month_get(NULL, &end_time);
		break;
		}
	case 1: /* week */
		{
		__setting_datausage_main_days_in_week_get(NULL, &end_time);
		break;
		}
	case 2: /* day */
		{
		end_time = time(0);
		break;
		}
	default:
		{
		/* do nothing */
		SETTING_TRACE_ERROR("wrong data usage cycle");
		return;
		}
	}

	struct tm cur_date;
	if (NULL == localtime_r(&cur_time, &cur_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return;
	}

	struct tm end_date;
	if (NULL == localtime_r(&end_time, &end_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return;
	}

	int start_day = cur_date.tm_mday;
	int end_day = end_date.tm_mday;

	if (start_day == end_day) {
		int cur_hour = cur_date.tm_hour;
		snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%d hours remaining", (SETTING_DATAUSAGE_HOURS_IN_DAY - cur_hour));
	} else {
		snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%d days remaining", (end_day - start_day));
	}
}

static void __setting_datausage_main_round_rectangle_draw(void *data, int width,
					int red, int green, int blue)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->limit_cr == NULL, "ad->limit_cr is NULL");

	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_set_line_width(ad->limit_cr, 1);

	int x = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X;
	int y = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y;
	int height = SETTING_DATAUSAGE_ROUND_RECT_HEIGHT;
	int r = SETTING_DATAUSAGE_ROUND_RECT_R;

	cairo_move_to(ad->limit_cr, x + r, y);
	cairo_line_to(ad->limit_cr, x + width - r, y);
	cairo_move_to(ad->limit_cr, x + width, y + r);
	cairo_line_to(ad->limit_cr, x + width, y + height - r);
	cairo_move_to(ad->limit_cr, x + width - r, y + height);
	cairo_line_to(ad->limit_cr, x + r, y + height);
	cairo_move_to(ad->limit_cr, x, y + height - r);
	cairo_line_to(ad->limit_cr, x, y + r);
	cairo_arc(ad->limit_cr, x + r, y + r, r, M_PI, 3 * M_PI / 2.0);
	cairo_arc(ad->limit_cr, x + width - r, y + r, r, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc(ad->limit_cr, x + width - r, y + height - r, r, 0, M_PI / 2);
	cairo_arc(ad->limit_cr, x + r, y + height - r, r, M_PI / 2, M_PI);

	cairo_stroke_preserve(ad->limit_cr);
	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_fill(ad->limit_cr);
}

static void __setting_datausage_main_round_rectangle_draw_1(void *data, int width,
					int red, int green, int blue)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->limit_cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_set_line_width(ad->limit_cr, 1);

	int x = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X;
	int y = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y;
	int height = SETTING_DATAUSAGE_ROUND_RECT_HEIGHT;
	int r = SETTING_DATAUSAGE_ROUND_RECT_R;

	double radian = acos((double)(r - width) / r);

	cairo_move_to(ad->limit_cr, x, y + height - r);
	cairo_line_to(ad->limit_cr, x, y + r);
	cairo_arc(ad->limit_cr, x + r, y + r, r, M_PI, M_PI + radian);
	cairo_arc(ad->limit_cr, x + r, y + height - r, r, M_PI - radian, M_PI);
	cairo_close_path(ad->limit_cr);

	cairo_stroke_preserve(ad->limit_cr);
	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_fill(ad->limit_cr);
}

static void __setting_datausage_main_triangle_draw(void *data, int width)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->limit_cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->limit_cr,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED/255.0,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN/255.0,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE/255.0,
		1.0);
	cairo_set_line_width(ad->limit_cr, 1);

	int x = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X + width;
	int y = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y + SETTING_DATAUSAGE_ROUND_RECT_HEIGHT + 4;
	int r = SETTING_DATAUSAGE_ROUND_RECT_R;

	cairo_move_to(ad->limit_cr, x, y);
	cairo_line_to(ad->limit_cr, x - r, y + r);
	cairo_line_to(ad->limit_cr, x + r, y + r);
	cairo_close_path(ad->limit_cr);

	cairo_stroke_preserve(ad->limit_cr);
	cairo_set_source_rgba(ad->limit_cr,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED/255.0,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN/255.0,
		SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE/255.0,
		1.0);
	cairo_fill(ad->limit_cr);
}

static void __setting_datausage_main_round_rectangle_draw_2(void *data, int width,
					int red, int green, int blue)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	setting_retm_if(width <= SETTING_DATAUSAGE_ROUND_RECT_R, "width is error");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->limit_cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_set_line_width(ad->limit_cr, 1);

	int x = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X;
	int y = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y;
	int height = SETTING_DATAUSAGE_ROUND_RECT_HEIGHT;
	int left_r = SETTING_DATAUSAGE_ROUND_RECT_R;
	int right_r = width - left_r;

	cairo_move_to(ad->limit_cr, x + width, y + right_r);
	cairo_line_to(ad->limit_cr, x + width, y + height - right_r);
	cairo_move_to(ad->limit_cr, x, y + height - left_r);
	cairo_line_to(ad->limit_cr, x, y + left_r);
	cairo_arc(ad->limit_cr, x + left_r, y + left_r, left_r, M_PI, 3 * M_PI / 2.0);
	cairo_arc(ad->limit_cr, x + left_r, y + right_r, right_r, 3 * M_PI / 2, 2 * M_PI);
	cairo_arc(ad->limit_cr, x + left_r, y + height - right_r, right_r, 0, M_PI / 2);
	cairo_arc(ad->limit_cr, x + left_r, y + height - left_r, left_r, M_PI / 2, M_PI);

	cairo_stroke_preserve(ad->limit_cr);
	cairo_set_source_rgba(ad->limit_cr, red/255.0, green/255.0, blue/255.0, 1.0);
	cairo_fill(ad->limit_cr);
}

static void __setting_datausage_main_text_in_round_rect_draw(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->limit_cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->limit_cr, 0.0, 0.0, 0.0, 1.0);

	int x = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_X + 20;
	int y = SETTING_DATAUSAGE_ROUND_RECT_HEAD_POINT_Y + 50;

	char total_size[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	__setting_datausage_main_size_transition(ad->total_size, total_size);

	char left_size[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	__setting_datausage_main_size_transition(ad->left_size, left_size);

	char desc[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%s used %s left", total_size, left_size);

	cairo_move_to(ad->limit_cr, x, y);
	cairo_set_font_size(ad->limit_cr, 38);
	cairo_show_text(ad->limit_cr, desc);

	int err = 0;
	int cycle = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	char left_time_desc[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	__setting_datausage_main_left_time_desc_get(left_time_desc);

	cairo_move_to(ad->limit_cr, x, y + 50);
	cairo_set_font_size(ad->limit_cr, 36);
	cairo_show_text(ad->limit_cr, left_time_desc);
}

static void __setting_datausage_main_table_draw(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->cr, 0.0, 0.0, 0.0, 1.0);
	cairo_set_line_width(ad->cr, 1);

	int begin_point_x = SETTING_DATAUSAGE_HEAD_POINT_X;
	int begin_point_y = SETTING_DATAUSAGE_HEAD_POINT_Y;
	int line_length = SETTING_DATAUSAGE_CHART_WIDTH;
	int line_height = SETTING_DATAUSAGE_CHART_HEIGHT;

	/* paint y-axis */
	cairo_move_to(ad->cr, begin_point_x, begin_point_y);
	cairo_line_to(ad->cr, begin_point_x, begin_point_y + line_height);

	/* paint horizontal lines */
	begin_point_x = SETTING_DATAUSAGE_HEAD_POINT_X;

	int line_count = line_height / SETTING_DATAUSAGE_LINE_TO_LINE_SPACE;
	int i = 0;
	for (; i <= line_count; i++) {
		if (i % SETTING_DATAUSAGE_LINES_IN_HORIZONTAL_GROUP == 0) {
			cairo_set_line_width(ad->cr, 3);
			cairo_move_to(ad->cr, begin_point_x - 12, begin_point_y);
		} else {
			cairo_set_line_width(ad->cr, 1);
			cairo_move_to(ad->cr, begin_point_x, begin_point_y);
		}

		cairo_line_to(ad->cr, begin_point_x + line_length, begin_point_y);
		begin_point_y += SETTING_DATAUSAGE_LINE_TO_LINE_SPACE;
		cairo_stroke(ad->cr);
	}
}

static void __setting_datausage_main_text_in_table_draw(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_retm_if(ad->cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->cr, 0.0, 0.0, 0.0, 1.0);

	int begin_point_x = SETTING_DATAUSAGE_HEAD_POINT_X;
	int begin_point_y = SETTING_DATAUSAGE_HEAD_POINT_Y;
	int group_space = SETTING_DATAUSAGE_CHART_HEIGHT / SETTING_DATAUSAGE_HORIZONTAL_GROUP;

	int interval = ad->maximum / SETTING_DATAUSAGE_HORIZONTAL_GROUP;

	int text_x = begin_point_x - 60;

	int i = 0;
	for (; i <= SETTING_DATAUSAGE_HORIZONTAL_GROUP; i++) {
		cairo_move_to(ad->cr, text_x, begin_point_y + 5);
		cairo_set_font_size(ad->cr, 20);
		char tmp[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
		if (i == SETTING_DATAUSAGE_HORIZONTAL_GROUP) {
			snprintf(tmp, MAX_DISPLAY_NAME_LEN_ON_UI, "%d", 0);
		} else {
			snprintf(tmp, MAX_DISPLAY_NAME_LEN_ON_UI, "%d", (ad->maximum - interval * i));
		}
		cairo_show_text(ad->cr, tmp);
		begin_point_y += group_space;
	}
}

static void __setting_datausage_main_pillar_draw(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	setting_retm_if(ad->cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->cr, 85/255.0, 145/255.0, 210/255.0, 1.0);
	cairo_set_line_width(ad->cr, 1);

	int begin_point_x = SETTING_DATAUSAGE_HEAD_POINT_X + 1;
	int begin_point_y = SETTING_DATAUSAGE_HEAD_POINT_Y + SETTING_DATAUSAGE_CHART_HEIGHT;
	int pillar_width = SETTING_DATAUSAGE_PILLAR_WIDTH;
	int pillar_max_height = SETTING_DATAUSAGE_CHART_HEIGHT;

	int i = 0;
	double tmp_size = 0.0;
	int height = 0;
	for (; i < ad->data_arr_len; i++) {
		if (ad->data_arr[i] == 0) {
			continue;
		}

		/* get the size xxMB */
		tmp_size = (double)ad->data_arr[i] / SETTING_DATAUSAGE_MEGABYTE_VALUE;
		height = pillar_max_height * (tmp_size / ad->maximum);
		cairo_rectangle(ad->cr, begin_point_x + pillar_width * i,
			begin_point_y - height, pillar_width, height - 1);
	}
	cairo_stroke_preserve(ad->cr);
	cairo_fill(ad->cr);
}

static void __setting_datausage_main_coord_draw(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	setting_retm_if(ad->cr == NULL, "ad->cr is NULL");

	cairo_set_source_rgba(ad->cr, 0.0, 0.0, 0.0, 1.0);
	cairo_set_line_width(ad->cr, 1);

	int offset = 1;
	int begin_point_x = SETTING_DATAUSAGE_HEAD_POINT_X + offset;
	int begin_point_y = SETTING_DATAUSAGE_HEAD_POINT_Y + SETTING_DATAUSAGE_CHART_HEIGHT;
	int line_space = SETTING_DATAUSAGE_PILLAR_WIDTH / 2;
	int line_height = SETTING_DATAUSAGE_PILLAR_WIDTH;

	cairo_move_to(ad->cr, begin_point_x, begin_point_y);
	cairo_line_to(ad->cr, begin_point_x, begin_point_y + line_height);

	int i = 0;
	for (; i < SETTING_DATAUSAGE_DAYS_IN_MONTH; i++) {
		begin_point_x = begin_point_x + line_space;
		cairo_move_to(ad->cr, begin_point_x, begin_point_y - line_height / 2);
		cairo_line_to(ad->cr, begin_point_x, begin_point_y + line_height / 2);

		begin_point_x = begin_point_x + line_space;
		cairo_move_to(ad->cr, begin_point_x, begin_point_y);
		cairo_line_to(ad->cr, begin_point_x, begin_point_y + line_height);
	}
	cairo_stroke(ad->cr);
}

static void __setting_datausage_main_pie_graph(Evas_Object *parent, const char* part, void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(parent == NULL, "parent is NULL");
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int value = 0;
	int err = 0;
	int ret = 0;

	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	int cairo_w = SETTING_DATAUSAGE_CAIRO_W;
	int cairo_h = SETTING_DATAUSAGE_CAIRO_H - SETTING_DATAUSAGE_DATA_LIMIT_GRAPH_HEIGHT;

	Evas* evas = evas_object_evas_get(parent);
	setting_retm_if(evas == NULL, "evas is NULL");

	Evas_Object *clip = evas_object_rectangle_add(evas);
	evas_object_color_set(clip, 255, 255, 255, 255);
	setting_resize_object(clip, cairo_w, cairo_h);

	Evas_Object *pie_image_object = evas_object_image_add(evas);
	setting_retm_if(pie_image_object == NULL, "pie_image_object is NULL");
	evas_object_color_set(pie_image_object, 255, 255, 255, 250);
	evas_object_clip_set(pie_image_object, clip);
	evas_object_show(pie_image_object);
	setting_resize_object(pie_image_object, cairo_w, cairo_h);

	evas_object_resize(parent, cairo_w, cairo_h);

	elm_object_part_content_set(parent, part, pie_image_object);

	if (ad->cairo_surface == NULL) {
		ad->cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, cairo_w, cairo_h);
		setting_retm_if(ad->cairo_surface == NULL, "ad->cairo_surface is NULL");
	}

	if (ad->cr == NULL) {
		ad->cr = cairo_create(ad->cairo_surface);
		setting_retm_if(ad->cr == NULL, "ad->cr is NULL");
	}

	__setting_datausage_main_table_draw(ad);

	__setting_datausage_main_text_in_table_draw(ad);

	__setting_datausage_main_pillar_draw(ad);

	__setting_datausage_main_coord_draw(ad);

	evas_object_image_size_set(pie_image_object, cairo_w, cairo_h);
	evas_object_image_data_set(pie_image_object, cairo_image_surface_get_data(ad->cairo_surface));
	evas_object_image_fill_set(pie_image_object, 0, 0, cairo_w, cairo_h);
}

static void __setting_datausage_main_limit_graph(Evas_Object *parent, const char* part, void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retm_if(parent == NULL, "parent is NULL");
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int cairo_w = SETTING_DATAUSAGE_CAIRO_W;
	int cairo_h = SETTING_DATAUSAGE_DATA_LIMIT_GRAPH_HEIGHT;

	Evas* evas = evas_object_evas_get(parent);
	setting_retm_if(evas == NULL, "evas is NULL");

	Evas_Object *clip = evas_object_rectangle_add(evas);
	evas_object_color_set(clip, 255, 255, 255, 255);
	setting_resize_object(clip, cairo_w, cairo_h);

	Evas_Object *pie_image_object = evas_object_image_add(evas);
	setting_retm_if(pie_image_object == NULL, "pie_image_object is NULL");
	evas_object_color_set(pie_image_object, 255, 255, 255, 250);
	evas_object_clip_set(pie_image_object, clip);
	evas_object_show(pie_image_object);
	setting_resize_object(pie_image_object, cairo_w, cairo_h);

	elm_object_part_content_set(parent, part, pie_image_object);

	if (ad->limit_cairo_surface == NULL) {
		ad->limit_cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, cairo_w, cairo_h);
		setting_retm_if(ad->limit_cairo_surface == NULL, "ad->cairo_surface is NULL");
	}

	if (ad->limit_cr == NULL) {
		ad->limit_cr = cairo_create(ad->limit_cairo_surface);
		setting_retm_if(ad->limit_cr == NULL, "ad->cr is NULL");
	}

	__setting_datausage_main_round_rectangle_draw(ad, SETTING_DATAUSAGE_ROUND_RECT_WIDTH,
				SETTING_DATAUSAGE_ROUND_RECT_BG_COLOR,
				SETTING_DATAUSAGE_ROUND_RECT_BG_COLOR,
				SETTING_DATAUSAGE_ROUND_RECT_BG_COLOR);

	int width = 0;
	if (ad->left_size > 0) {
		double percent = (double)ad->total_size / (ad->total_size + ad->left_size);
		width = SETTING_DATAUSAGE_ROUND_RECT_WIDTH * percent;
	} else {
		width = SETTING_DATAUSAGE_ROUND_RECT_WIDTH;
	}

	if (width > (2 * SETTING_DATAUSAGE_ROUND_RECT_R)) {
		__setting_datausage_main_round_rectangle_draw(ad, width,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE);
	} else if (width > SETTING_DATAUSAGE_ROUND_RECT_R && width <= (2 * SETTING_DATAUSAGE_ROUND_RECT_R)) {
		__setting_datausage_main_round_rectangle_draw_2(ad, width,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE);
	} else {
		__setting_datausage_main_round_rectangle_draw_1(ad, width,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_RED,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_GREEN,
					SETTING_DATAUSAGE_ROUND_RECT_COLOR_BLUE);
	}
	__setting_datausage_main_triangle_draw(ad, width);
	__setting_datausage_main_text_in_round_rect_draw(ad);

	evas_object_image_size_set(pie_image_object, cairo_w, cairo_h);
	evas_object_image_data_set(pie_image_object, cairo_image_surface_get_data(ad->limit_cairo_surface));
	evas_object_image_fill_set(pie_image_object, 0, 0, cairo_w, cairo_h);
}

static void __setting_datausage_main_dismissed_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	if(ad->ctxpopup) {
		evas_object_del(ad->ctxpopup);
		ad->ctxpopup = NULL;
	}
}

static void __setting_datausage_main_move_ctxpopup(Evas_Object *ctxpopup, Evas_Object *btn)
{
	Evas_Coord x, y, w , h;
	evas_object_geometry_get(btn, &x, &y, &w, &h);
	evas_object_move(ctxpopup, x + (w / 2), y + (h /2));
}

static void __setting_datausage_main_timepicker_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;
	setting_retm_if(ad->starting_date == NULL, "ad->starting_date is NULL");

	const char *label = elm_object_item_text_get((Elm_Object_Item *) event_info);
	setting_retm_if(label == NULL, "label is NULL");

	const char *btn_label = elm_object_text_get(ad->starting_date->eo_check);
	if(!safeStrCmp(label, btn_label)) {
		SETTING_TRACE("Not changed");
		return;
	} else {
		SETTING_TRACE("text(%s) is clicked!", label);
		elm_object_text_set(ad->starting_date->eo_check, label);

		int starting_date = (int)g_ascii_strtod(label, NULL);
		setting_retm_if(errno == ERANGE, "fail to call g_ascii_strtod");

		int err = 0;
		int ret = setting_set_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, starting_date, &err);
		setting_retm_if(ret != 0, "set vconf failed");

		if(ad->ctxpopup) {
			evas_object_del(ad->ctxpopup);
			ad->ctxpopup = NULL;
		}
	}
}

static void __setting_datausage_main_create_ctxpopup_timepicker(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	SettingDatausageUG *ad = (SettingDatausageUG *)item_data->userdata;

	int idx = 0;
	Evas_Object *diskselector = NULL;
	Elm_Object_Item *item = NULL;

	if(ad->ctxpopup) {
		evas_object_del(ad->ctxpopup);
		ad->ctxpopup = NULL;
	}

	Evas_Coord w = 0;
	evas_object_geometry_get(ad->win_get, NULL, NULL, &w, NULL);

	ad->ctxpopup = elm_ctxpopup_add(ad->navibar);
	elm_object_style_set(ad->ctxpopup, "timepicker");
	evas_object_size_hint_min_set(ad->ctxpopup, w, -1);
	evas_object_size_hint_weight_set(ad->ctxpopup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_smart_callback_add(ad->ctxpopup, "dismissed", __setting_datausage_main_dismissed_cb, ad);

	diskselector = elm_diskselector_add(ad->navibar);
	elm_diskselector_display_item_num_set(diskselector, 8);
	elm_object_style_set(diskselector, "timepicker");

	int value = 0;
	int err = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	int value_to_idx = value - 1;
	for(idx = 0; idx < sizeof(days_in_month) / sizeof(days_in_month[0]); idx++) {
		if(value_to_idx == idx) {
			item = elm_diskselector_item_append(diskselector, days_in_month[idx], NULL,
						__setting_datausage_main_timepicker_cb, ad);
		} else {
			elm_diskselector_item_append(diskselector, days_in_month[idx], NULL,
						__setting_datausage_main_timepicker_cb, ad);
		}
	}

	if(item != NULL) {
		elm_diskselector_item_selected_set(item, EINA_TRUE);
	}

	elm_diskselector_round_enabled_set(diskselector, EINA_TRUE);
	evas_object_size_hint_weight_set(diskselector, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(diskselector, EVAS_HINT_FILL, EVAS_HINT_FILL);

	evas_object_show(diskselector);
	elm_object_content_set(ad->ctxpopup, diskselector);

	elm_ctxpopup_direction_priority_set(ad->ctxpopup,
					ELM_CTXPOPUP_DIRECTION_DOWN,
					ELM_CTXPOPUP_DIRECTION_UP,
					ELM_CTXPOPUP_DIRECTION_LEFT,
					ELM_CTXPOPUP_DIRECTION_RIGHT);

	__setting_datausage_main_move_ctxpopup(ad->ctxpopup, obj);
	evas_object_show(ad->ctxpopup);
}

static char *_item_label_get(void *data, Evas_Object *obj, const char *part)
{
	//SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {
		if (item_data->keyStr) {
			return (char *)g_strdup(_(item_data->keyStr));
		}
	} else {
		if (item_data->sub_desc) {
			return (char *)g_strdup(_(item_data->sub_desc));
		}
	}

	return NULL;
}

static Evas_Object *_item_content_get(void *data, Evas_Object *obj, const char *part)
{
	//SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	int width = 0;
	evas_object_geometry_get(obj, NULL, NULL, &width, NULL);

	if (!safeStrCmp(part, "elm.icon")) {
		if (!safeStrCmp(item_data->keyStr, "limit_graph")) {
			Evas_Object *sub_layout = elm_layout_add(obj);
			elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME, "screen_mode");
			evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, 0.0);

			__setting_datausage_main_limit_graph(sub_layout, "content", item_data->userdata);
			setting_resize_object(sub_layout, width - 40, SETTING_DATAUSAGE_GRAPH_H);
			evas_object_show(sub_layout);
			return sub_layout;
		} else if (!safeStrCmp(item_data->keyStr, "graph_image")) {
			Evas_Object *sub_layout = elm_layout_add(obj);
			elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME, "screen_mode");
			evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, 0.0);

			__setting_datausage_main_pie_graph(sub_layout, "content", item_data->userdata);
			setting_resize_object(sub_layout, width - 40, SETTING_DATAUSAGE_GRAPH_H);
			evas_object_show(sub_layout);
			return sub_layout;
		} else {
			int offset = 50;

			Evas_Object *label = NULL;
			label = elm_label_add(obj);
			evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
			elm_label_line_wrap_set(label, EINA_TRUE);
			elm_label_wrap_width_set(label, width - offset);
			elm_object_text_set(label, _(item_data->keyStr));
			setting_resize_object(label, width - offset, 250);
			evas_object_show(label);
			return label;
		}
	}

	return NULL;
}

static Evas_Object *_item_content_date_get(void *data, Evas_Object *obj, const char *part)
{
	//SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (!safeStrCmp(part, "elm.icon")) {
		Evas_Object *timepicker_btn = NULL;
		timepicker_btn = elm_button_add(obj);
		item_data->eo_check = timepicker_btn;

		int value = 0;
		int err = 0;
		int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, &value, &err);
		if (ret != 0) {
			SETTING_TRACE_ERROR("get vconf failed");
		}

		int count = sizeof(days_in_month) / sizeof(days_in_month[0]);

		int value_to_idx = value - 1;

		if (value_to_idx >= 0 && value_to_idx < count) {
			elm_object_text_set(timepicker_btn, days_in_month[value_to_idx]);
		} else {
			elm_object_text_set(timepicker_btn, days_in_month[0]);
		}

		evas_object_smart_callback_add(timepicker_btn, "clicked", __setting_datausage_main_create_ctxpopup_timepicker, item_data);

		return timepicker_btn;
	}

	return NULL;
}

static Evas_Object *_item_content3_get(void *data, Evas_Object *obj, const char *part)
{
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (!strcmp(part, "elm.icon")) {
		Evas_Object *progressbar = NULL;
		progressbar = elm_progressbar_add(obj);
		elm_object_style_set(progressbar, "list_progress");
		elm_progressbar_horizontal_set(progressbar, EINA_TRUE);
		evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

		double percent = item_data->slider_min;

		//SETTING_TRACE("byte_size = %ld, percent = %f", byte_size, percent);
		if (percent <= 0.01) {
			percent =  0.01;
		}

		elm_progressbar_value_set(progressbar, percent);

		return progressbar;
	}
	return NULL;
}

static void _item_del(void *data, Evas_Object *obj)
{
	setting_retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	if (item_data) {
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		FREE(item_data);
	}
}

static void __entry_focused(void *data, Evas_Object *obj, void *event_info) // Focused callback will show X marked button and hide guidetext.
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	Evas_Object *entry_container = (Evas_Object *)data;
	if (!elm_entry_is_empty(obj)) {
		elm_object_signal_emit(entry_container, "elm,state,eraser,show", "elm");
	}
	elm_object_signal_emit(entry_container, "elm,state,guidetext,hide", "elm");
}
static void __eraser_clicked(void *data, Evas_Object *obj, const char *emission, const char *source) // When X marked button is clicked, empty entry's contents.
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	Evas_Object *entry = data;
	elm_entry_entry_set(entry, "");
	elm_object_focus_set(entry, EINA_TRUE);
}

static void __entry_changed(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(obj == NULL, "obj is NULL");

	Evas_Object *entry_container = elm_object_parent_widget_get(obj);
	if (entry_container) {
		if (elm_entry_is_empty(obj)) {
			//elm_object_signal_emit(entry_container, "elm,state,guidetext,show", "elm");
			elm_object_signal_emit(entry_container, "elm,state,eraser,hide", "elm");
		} else {
			elm_object_signal_emit(entry_container, "elm,state,guidetext,hide", "elm");
			elm_object_signal_emit(entry_container, "elm,state,eraser,show", "elm");
		}
	}
}

static void __setting_datausage_main_cairo_destroy(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	if (ad->cr) {
		cairo_destroy(ad->cr);
		ad->cr = NULL;
	}
	if (ad->cairo_surface) {
		cairo_surface_destroy(ad->cairo_surface);
		ad->cairo_surface = NULL;
	}
}

static void __setting_datausage_main_limit_cairo_destroy(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	if (ad->limit_cr) {
		cairo_destroy(ad->limit_cr);
		ad->limit_cr = NULL;
	}
	if (ad->limit_cairo_surface) {
		cairo_surface_destroy(ad->limit_cairo_surface);
		ad->limit_cairo_surface = NULL;
	}
}

static void __setting_datausage_main_apps_list_destroy(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	if (ad->app_list) {
		Eina_List *l = NULL;
		rsml_perf_info *new_item;
		EINA_LIST_FOREACH(ad->app_list, l, new_item)
		{
			FREE(new_item->interval);
			G_FREE(new_item->app_id);
			FREE(new_item);
		}
		eina_list_free(ad->app_list);
		ad->app_list = NULL;
	}
}

static void __setting_datausage_main_popup_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	int response_type = btn_type(obj);
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	if (POPUP_RESPONSE_OK == response_type) {
		setting_retm_if(ad->data_limit_entry == NULL, "ad->data_limit_entry is NULL");
		const char *text = elm_entry_entry_get(ad->data_limit_entry);
		if (isEmptyStr(text)) {
			SETTING_TRACE_DEBUG("text is NULL");
			return;
		}

		int date_limit = (int)g_ascii_strtod(text, NULL);
		setting_retm_if(errno == ERANGE, "fail to call g_ascii_strtod");

		int err = 0;
		int ret = setting_set_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_LIMIT_INT, date_limit, &err);
		setting_retm_if(ret != 0, "set vconf failed");

		setting_hide_input_pannel_cb(ad->data_limit_entry);
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		SETTING_TRACE("POPUP_RESPONSE_CANCEL");
	}

	if (ad->data_limit_conformant) {
		evas_object_del(ad->data_limit_conformant);
		ad->data_limit_conformant = NULL;
	}

	if (ad->data_limit_popup) {
		evas_object_del(ad->data_limit_popup);
		ad->data_limit_popup = NULL;
	}
}

static void __setting_datausage_main_data_limit_popup(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	Evas_Object *popup = NULL;
	Evas_Object *entry = NULL;
	Evas_Object *btn1 = NULL;
	Evas_Object *btn2 = NULL;
	//Evas_Object *content = NULL;

	if (ad->data_limit_conformant) {
		evas_object_del(ad->data_limit_conformant);
		ad->data_limit_conformant = NULL;
	}
	//ad->data_limit_conformant = elm_conformant_add(ad->win_get);
	//elm_win_conformant_set(ad->win_get, EINA_TRUE);
	//elm_win_resize_object_add(ad->win_get, ad->data_limit_conformant);
	//evas_object_size_hint_weight_set(ad->data_limit_conformant, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	//evas_object_size_hint_align_set(ad->data_limit_conformant, EVAS_HINT_FILL, EVAS_HINT_FILL);
	//evas_object_show(ad->data_limit_conformant);

	//content = elm_layout_add(ad->win_get);
	//elm_object_content_set(ad->data_limit_conformant, content);
	popup = elm_popup_add(ad->navibar);
	elm_object_part_text_set(popup, "title,text", _(SETTING_DATAUSAGE_DATA_LIMIT_STR));

	Evas_Object* pLy = elm_layout_add(popup);
	elm_layout_theme_set(pLy, "layout", "application", "noindicator" );
	evas_object_size_hint_weight_set(pLy, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	Evas_Object* pBg = elm_bg_add(pLy);
	evas_object_size_hint_weight_set(pBg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(pLy, "elm.swallow.bg", pBg);
	elm_bg_color_set(pBg, 255, 255, 255);
	evas_object_show(pBg);

	Evas_Object *box = elm_box_add(popup);
	elm_box_horizontal_set(box, EINA_TRUE);

	Evas_Object *editfiled = setting_create_editfiled(popup, NULL, NULL);
	entry = elm_object_part_content_get(editfiled, "elm.swallow.content");
	ad->data_limit_entry = entry;

	elm_entry_single_line_set(entry, EINA_TRUE);
	elm_entry_scrollable_set(entry, EINA_TRUE);
	elm_entry_autocapital_type_set(entry, ELM_AUTOCAPITAL_TYPE_ALLCHARACTER);
	elm_entry_input_panel_layout_set(entry, ELM_INPUT_PANEL_LAYOUT_NUMBERONLY);

	Elm_Entry_Filter_Limit_Size limit_filter_data;
	limit_filter_data.max_char_count = 0;
	limit_filter_data.max_byte_count = 5;
	elm_entry_markup_filter_append(entry, elm_entry_filter_limit_size, &limit_filter_data);

	/* default handle */
	evas_object_smart_callback_add(entry, "changed", __entry_changed, NULL);
	evas_object_smart_callback_add(entry, "focused", __entry_focused, editfiled);
	elm_object_signal_callback_add(editfiled, "elm,eraser,clicked", "elm", __eraser_clicked, entry);

	elm_box_pack_end(box, editfiled);

	Evas_Object *label = elm_label_add(popup);
	elm_object_text_set(label, "MB");
	evas_object_show(label);
	elm_box_pack_end(box, label);

	evas_object_show(box);

	Evas_Object *content_layout = elm_layout_add(pLy);
	elm_layout_file_set(content_layout, SETTING_THEME_EDJ_NAME, "custom_popup");
	evas_object_size_hint_weight_set(content_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_content_set(content_layout, "content", box);

	/* guide text */
	Evas_Object *guide_label = elm_label_add(popup);
	evas_object_size_hint_weight_set(guide_label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(guide_label, 0.0, EVAS_HINT_FILL);
	elm_label_line_wrap_set(guide_label, EINA_TRUE);
	elm_label_wrap_width_set(guide_label, 530);
	elm_object_text_set(guide_label, DATAUSAGE_POPUP_GUIDE_TEXT);
	evas_object_show(guide_label);
	elm_object_part_content_set(content_layout, "content2", guide_label);

	evas_object_show(content_layout);

	elm_object_part_content_set(pLy, "elm.swallow.content", content_layout);
	evas_object_show(pLy);

	elm_object_content_set(popup, pLy);

	btn1 = elm_button_add(popup);
	elm_object_text_set(btn1, _(KeyStr_Save));
	elm_object_part_content_set(popup, "button1", btn1);
	elm_object_style_set(btn1, "popup_button/default");
	evas_object_smart_callback_add(btn1, "clicked", __setting_datausage_main_popup_resp_cb, ad);

	btn2 = elm_button_add(popup);
	elm_object_text_set(btn2, _("IDS_COM_SK_CANCEL"));
	elm_object_part_content_set(popup, "button2", btn2);
	elm_object_style_set(btn2, "popup_button/default");
	evas_object_smart_callback_add(btn2, "clicked", __setting_datausage_main_popup_resp_cb, ad);

	evas_object_show(popup);
	ad->data_limit_popup = popup;
}

static int __setting_datausage_main_datausage_quota_set()
{
	int err = 0;
	int ret = 0;

	int time_period = 0;
	ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &time_period, &err);
	setting_retvm_if(ret == -1, SETTING_RETURN_FAIL, "get vconf failed");

	int data_limit = 0;
	ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_LIMIT_INT, &data_limit, &err);
	setting_retvm_if(ret == -1, SETTING_RETURN_FAIL, "get vconf failed");

	rsml_datausage_quota quota;

	switch(time_period) {
	case 0:
		quota.time_period = RESMAN_PERIOD_MONTH;
		break;
	case 1:
		quota.time_period = RESMAN_PERIOD_WEEK;
		break;
	case 2:
		quota.time_period = RESMAN_PERIOD_DAY;
		break;
	default:
		quota.time_period = RESMAN_PERIOD_UNDEF;
		break;
	}
	quota.snd_quota = data_limit;
	quota.rcv_quota = data_limit;
	quota.quota_type = RESMAN_STATE_UNKNOWN;

	SETTING_TRACE("quota.time_period = %d", quota.time_period);
	SETTING_TRACE("quota.snd_quota = %d", quota.snd_quota);
	SETTING_TRACE("quota.rcv_quota = %d", quota.rcv_quota);
	SETTING_TRACE("quota.quota_type = %d", quota.quota_type);

	rsml_ret_c rsml_ret = set_datausage_quota(SETTING_PKG_NAME, &quota);
	SETTING_TRACE("rsml_ret = %d", rsml_ret);
	setting_retvm_if(rsml_ret != RESMAN_ERROR_OK, SETTING_RETURN_FAIL, "fail to call set_datausage_quota");

	return SETTING_RETURN_SUCCESS;
}

static void __setting_datausage_main_mouse_up_Gendial_list_cb(void *data, Evas_Object * obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int opposite = !list_item->chk_status;

	int err = 0;
	int ret = 0;

	if (!safeStrCmp(SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_STR, list_item->keyStr)) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		elm_check_state_set(list_item->eo_check, opposite);
		list_item->chk_status = opposite;

		/* if set_data_usage_limit's vconf is false, disable data_limit */
		if (!opposite) {
			setting_disable_genlist_item(ad->data_limit->item);

			rsml_ret_c rsml_ret = remove_datausage_quota(SETTING_PKG_NAME);
			if (rsml_ret != RESMAN_ERROR_OK) {
				SETTING_TRACE_ERROR("fail to call remove_datausage_quota");
			}

			__setting_datausage_main_data_items_recreate(ad);
		} else {
			ret = __setting_datausage_main_datausage_quota_set();
			if (ret == SETTING_RETURN_FAIL) {
				SETTING_TRACE_ERROR("failed to set datausage quota");
			}

			setting_enable_genlist_item(ad->data_limit->item);

			__setting_datausage_main_data_items_recreate(ad);
		}
	} else if (!safeStrCmp(SETTING_DATAUSAGE_DATA_LIMIT_STR, list_item->keyStr)) {
		__setting_datausage_main_data_limit_popup(ad);
	} else if (!safeStrCmp(KeyStr_UsePacketData, list_item->keyStr)) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		elm_check_state_set(list_item->eo_check, opposite);
		list_item->chk_status = opposite;
	}
}

static void __setting_datausage_main_chk_btn_cb(void *data, Evas_Object * obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Data parameter is NULL");
	SettingDatausageUG *ad = NULL;

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;
	ad = list_item->userdata;
	list_item->chk_status = elm_check_state_get(obj);	// for genlist update status

	int opposite  = (list_item->chk_status == 1) ? SETTING_ON_OFF_BTN_ON : SETTING_ON_OFF_BTN_OFF;

	int err = -1;
	int ret = -1;

	if (obj == ad->use_packet_data->eo_check) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");
	} else if (obj == ad->set_data_usage_limit->eo_check) {
		ret = setting_set_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, opposite, &err);
		setting_retm_if(ret == -1, "set vconf failed");

		/* if set_data_usage_limit's vconf is false, disable data_limit */
		if (!opposite) {
			rsml_ret_c rsml_ret = remove_datausage_quota(SETTING_PKG_NAME);
			if (rsml_ret != RESMAN_ERROR_OK) {
				SETTING_TRACE_ERROR("fail to call remove_datausage_quota");
			}

			__setting_datausage_main_data_items_recreate(ad);

			setting_disable_genlist_item(ad->data_limit->item);
		} else {
			ret = __setting_datausage_main_datausage_quota_set();
			if (ret == SETTING_RETURN_FAIL) {
				SETTING_TRACE_ERROR("failed to set datausage quota");
			}

			setting_enable_genlist_item(ad->data_limit->item);

			__setting_datausage_main_data_items_recreate(ad);
		}
	}
}

static char *__setting_datausage_main_data_usage_cycle_get()
{
	int value = 0;
	int err = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	if (value >= 0 && value < 3) {
		return (char *)data_usage_cycle_arr[value];
	} else {
		return (char *)data_usage_cycle_arr[0];
	}
}

static void __setting_datausage_main_maximum_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL,"Data parameter is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	/* get the max data */
	unsigned long long maximum = ad->data_arr[0];

	unsigned long long total = 0;
	int i = 0;
	for (; i < ad->data_arr_len; i++) {
		if (ad->data_arr[i] > maximum) {
			maximum = ad->data_arr[i];
		}

		total += ad->data_arr[i];
	}

	ad->total_size = total;

	/* get the left size */
	int date_limit = 0;
	int err = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_LIMIT_INT, &date_limit, &err);
	setting_retm_if(ret != 0, "set vconf failed");

	double date_limit_d = ((double)date_limit) * SETTING_DATAUSAGE_MEGABYTE_VALUE;

	if (date_limit_d <= ad->total_size) {
		ad->left_size = 0;
	} else {
		ad->left_size = date_limit_d - ad->total_size;
	}

	/*get maximum 10% more than the highest of data */
	SETTING_TRACE("maximum = %lld", maximum);
	unsigned long long tmp = maximum + maximum * 0.1;
	SETTING_TRACE("tmp = %lld", tmp);

	/* get the size xxMB */
	int tmp_size = 0;
	tmp_size = (int)(tmp / SETTING_DATAUSAGE_MEGABYTE_VALUE);
	SETTING_TRACE("tmp_size = %d", tmp_size);

	if (tmp_size <= 5) {
		ad->maximum = 5;    /* 5 MB */
	} else {
		ad->maximum = tmp_size;
	}
}

static rsml_cb_ret __setting_datausage_main_apps_network_stats_get_cb(const rsml_perf_info *info,
					    void *user_data)
{
	Eina_List **list = (Eina_List **) user_data;
	rsml_perf_info *new_item = (rsml_perf_info *)calloc(1, sizeof(rsml_perf_info));
	rsml_tm_interval *interval_copy = (rsml_tm_interval *)calloc(1, sizeof(rsml_tm_interval));
	char *app_id_copy = info->app_id ? g_strdup(info->app_id) : g_strdup(UNKNOWN_APP);

	*interval_copy = *(info->interval);
	*new_item = *info;
	new_item->app_id = app_id_copy;
	new_item->interval = interval_copy;

	*list = eina_list_append(*list, new_item);
	return RESMAN_CONTINUE;
}

/**
  * This function get the start day and the end day in current month
  * @start : output param, start day in current month, according to the vconf 'starting date'
  * @end: output param, end day in current month
  * @return days between start and end
  */
static int __setting_datausage_main_days_in_month_get(time_t *start, time_t *end)
{
	int starting_date = 0;
	int err = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, &starting_date, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	//SETTING_TRACE("starting date is %d", starting_data);

	time_t cur_time = time(0);

	struct tm start_date;
	if (NULL == localtime_r(&cur_time, &start_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return 0;
	}
	start_date.tm_mday = 1;    /* set the first day in the month*/
	time_t from_time = mktime(&start_date);

	/* get the start day in the year */
	int from_yday = start_date.tm_yday;
	SETTING_TRACE("start_data :%s", asctime(&start_date));
	SETTING_TRACE("from_yday :%d", from_yday);

	/* get the last day in current month, 2 step:
	  * 1. get the first day in next month, get the day in year;
	  * 2. the last day in  current month = the day(in step 1) - 1;
	  */
	/* step 1 */
	struct tm tmp_date;
	if (NULL == localtime_r(&from_time, &tmp_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return 0;
	}
	int cur_month = tmp_date.tm_mon;
	tmp_date.tm_mon = cur_month + 1;
	time_t tmp_time = mktime(&tmp_date);
	//SETTING_TRACE("tmp_data :%s", ctime(&tmp_time));

	/* step 2 */
	struct tm end_date;
	if (NULL == localtime_r(&tmp_time, &end_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return 0;
	}
	int end_yday = end_date.tm_yday;
	int end_month = end_date.tm_mon;
	end_date.tm_mon = end_month - 1;
	/* if end_yday < from_yday, means it is the day in next year, so the last month in the year must be Dec*/
	if (end_yday < from_yday) {
		end_date.tm_mday = 31; /* Dec must have 31 days */
	} else {
		end_date.tm_mday = end_yday - from_yday;
	}
	end_date.tm_hour = 23;
	end_date.tm_min = 59;
	end_date.tm_sec = 59;
	time_t to_time = mktime(&end_date);

	start_date.tm_mday = starting_date;    /* set the start day as vconf */
	start_date.tm_hour = 0;
	start_date.tm_min = 0;
	start_date.tm_sec = 0;
	time_t start_time = mktime(&start_date);

	SETTING_TRACE("start_time :%s", ctime(&start_time));
	SETTING_TRACE("to_time :%s", ctime(&to_time));

	if (start) {
		*start = start_time;
	}
	if (end) {
		*end = to_time;
	}

	// days in current month
	int day_in_month = end_date.tm_mday;
	SETTING_TRACE("day_in_month :%d", day_in_month);

	int left_day = day_in_month - starting_date;

	return left_day > 0 ? left_day : 0;
}

/**
  * This function get the start day and the end day in current week
  * @start : output param, start day in current week
  * @end: output param, end day in current week
  * @return days between start and end
  */
static int __setting_datausage_main_days_in_week_get(time_t *start, time_t *end)
{
	time_t cur_time = time(0);

	struct tm start_date;
	if (NULL == localtime_r(&cur_time, &start_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return 0;
	}
	int cur_wday = start_date.tm_wday;  /* current day in week */
	int cur_mday = start_date.tm_mday;  /* current day in month */

	start_date.tm_mday = cur_mday - cur_wday;
	start_date.tm_hour = 0;
	start_date.tm_min = 0;
	start_date.tm_sec = 0;
	time_t from_time = mktime(&start_date);

	struct tm end_data;
	if (NULL == localtime_r(&from_time, &end_data)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return 0;
	}
	cur_mday = end_data.tm_mday;
	end_data.tm_mday = cur_mday + 6;   /* get the last day in current week */
	end_data.tm_hour = 23;
	end_data.tm_min = 59;
	end_data.tm_sec = 59;
	time_t end_time = mktime(&end_data);

	SETTING_TRACE("from_time :%s", ctime(&from_time));
	SETTING_TRACE("end_time :%s", ctime(&end_time));

	if (start) {
		*start = from_time;
	}
	if (end) {
		*end = end_time;
	}

	return SETTING_DATAUSAGE_DAYS_IN_WEEK;
}


/**
  * This function get the apps info in current month.
  */
static Eina_List * __setting_datausage_main_apps_info_in_cur_month_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	time_t from_time;
	time_t to_time;
	int match_days  = __setting_datausage_main_days_in_month_get(&from_time, &to_time);

	SETTING_TRACE("from_time :%s", ctime(&from_time));
	SETTING_TRACE("to_time :%s", ctime(&to_time));

	Eina_List *list = NULL;
	data_usage_selection_rule rule = { 0 };

	/* if  match_days is 0, means start day is same with end day*/
	match_days = match_days > 0 ? match_days : 1;
	SETTING_TRACE("match_days :%d", match_days);

	rule.granularity = 3600 * 24 * match_days;
	rule.iftype = ad->iftype;
	rule.from = from_time;
	rule.to = to_time;

	data_usage_foreach(&rule, __setting_datausage_main_apps_network_stats_get_cb,
			   (void *)&list);

	return list;
}

static Eina_List * __setting_datausage_main_apps_info_in_cur_week_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	time_t from_time;
	time_t to_time;
	int match_days = __setting_datausage_main_days_in_week_get(&from_time, &to_time);

	SETTING_TRACE("from_time :%s", ctime(&from_time));
	SETTING_TRACE("to_time :%s", ctime(&to_time));

	Eina_List *list = NULL;
	data_usage_selection_rule rule = { 0 };

	rule.granularity = 3600 * 24 * match_days;
	rule.iftype = ad->iftype;
	rule.from = from_time;
	rule.to = to_time;

	data_usage_foreach(&rule, __setting_datausage_main_apps_network_stats_get_cb,
			   (void *)&list);

	return list;
}

static Eina_List * __setting_datausage_main_apps_info_in_cur_day_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	time_t cur_time = time(0);

	struct tm start_date;
	if (NULL == localtime_r(&cur_time, &start_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return NULL;
	}
	start_date.tm_hour = 0;
	start_date.tm_min = 0;
	start_date.tm_sec = 0;
	time_t from_time = mktime(&start_date);
	SETTING_TRACE("start_data :%s", asctime(&start_date));

	struct tm end_date;
	if (NULL == localtime_r(&from_time, &end_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return NULL;
	}
	end_date.tm_hour = 23;
	end_date.tm_min = 59;
	end_date.tm_sec = 59;
	time_t end_time = mktime(&end_date);
	SETTING_TRACE("end_time :%s", ctime(&end_time));

	Eina_List *list = NULL;
	data_usage_selection_rule rule = { 0 };

	rule.granularity = 3600 * 24;
	rule.iftype = ad->iftype;
	rule.from = from_time;
	rule.to = end_time;

	data_usage_foreach(&rule, __setting_datausage_main_apps_network_stats_get_cb,
			   (void *)&list);

	return list;
}

static Eina_List *__setting_datausage_main_apps_network_stats_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(data == NULL, NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	int err = 0;
	int cycle = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	__setting_datausage_main_apps_list_destroy(ad);

	switch (cycle) {
	case 0: /* month */
		return __setting_datausage_main_apps_info_in_cur_month_get(ad);
	case 1: /* week */
		return __setting_datausage_main_apps_info_in_cur_week_get(ad);
	case 2: /* day */
		return __setting_datausage_main_apps_info_in_cur_day_get(ad);
	default:
		/* do nothing */
		return NULL;
	}
}

static rsml_cb_ret __setting_datausage_main_data_get_cb(const rsml_perf_info *info, void *user_data)
{
	unsigned long long sum = 0;
	sum += (unsigned long long)info->foreground.cnt.incoming_bytes;
	sum += (unsigned long long)info->background.cnt.incoming_bytes;

	*((unsigned long long *)user_data) = sum;
	return RESMAN_CONTINUE;
}

/**
  * This function get the data of per day in current month.
  */
static void __setting_datausage_main_days_info_in_cur_month_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int starting_date = 0;
	int err = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, &starting_date, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	//SETTING_TRACE("starting date is %d", starting_data);

	int days = __setting_datausage_main_days_in_month_get(NULL, NULL);
	SETTING_TRACE("days is %d", days);

	ad->data_arr_len = days + 1;
	int i = 0;
	for (; i <= days; i++) {
		if (i >= SETTING_DATAUSAGE_DAYS_IN_MONTH) {
			break;
		}

		time_t cur_time = time(0);

		struct tm start_date;
		if (NULL == localtime_r(&cur_time, &start_date)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		start_date.tm_mday = starting_date + i;    /* set the start day as vonf value*/
		start_date.tm_hour = 0;
		start_date.tm_min = 0;
		start_date.tm_sec = 0;
		time_t from_time = mktime(&start_date);

		struct tm end_date;
		if (NULL == localtime_r(&from_time, &end_date)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		end_date.tm_hour = 23;
		end_date.tm_min = 59;
		end_date.tm_sec = 59;
		time_t to_time = mktime(&end_date);

		SETTING_TRACE("from_time :%s", ctime(&from_time));
		SETTING_TRACE("to_time :%s", ctime(&to_time));

		unsigned long long cnt = 0;
		data_usage_selection_rule rule = { 0 };

		rule.granularity = 3600 * 24;
		rule.iftype = ad->iftype;
		rule.from = from_time;
		rule.to = to_time;

		rsml_ret_c ret = data_usage_foreach(&rule, __setting_datausage_main_data_get_cb,
					   (void *)&cnt);
		SETTING_TRACE("ret : %d, cnt : %lld", ret, cnt);
		ad->data_arr[i] = cnt;
	}
}

/**
  * This function get the data of per day in current week.
  */
static void __setting_datausage_main_days_info_in_cur_week_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	time_t cur_time = time(0);

	struct tm begin_date;
	if (NULL == localtime_r(&cur_time, &begin_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return;
	}
	int cur_wday = begin_date.tm_wday;  /* current day in week */
	SETTING_TRACE("cur_wday :%d", cur_wday);
	int cur_mday = begin_date.tm_mday;  /* current day in month */

	SETTING_TRACE("cur_mday :%d", cur_mday);
	begin_date.tm_mday = cur_mday - cur_wday;
	time_t begin_time = mktime(&begin_date);
	SETTING_TRACE("from_time :%s", ctime(&begin_time));

	int starting_date = cur_mday - cur_wday;

	ad->data_arr_len = SETTING_DATAUSAGE_DAYS_IN_WEEK;
	int i = 0;
	for (; i < ad->data_arr_len; i++) {
		time_t cur_time = time(0);

		struct tm start_date;
		if (NULL == localtime_r(&cur_time, &start_date)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		start_date.tm_mday = starting_date + i;    /* set the start day as vonf value*/
		start_date.tm_hour = 0;
		start_date.tm_min = 0;
		start_date.tm_sec = 0;
		time_t from_time = mktime(&start_date);

		struct tm end_date;
		if (NULL == localtime_r(&from_time, &end_date)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		end_date.tm_hour = 23;
		end_date.tm_min = 59;
		end_date.tm_sec = 59;
		time_t to_time = mktime(&end_date);

		SETTING_TRACE("from_time :%s", ctime(&from_time));
		SETTING_TRACE("to_time :%s", ctime(&to_time));

		unsigned long long cnt = 0;
		data_usage_selection_rule rule = { 0 };

		rule.granularity = 3600 * 24;
		rule.iftype = ad->iftype;
		rule.from = from_time;
		rule.to = to_time;

		rsml_ret_c ret = data_usage_foreach(&rule, __setting_datausage_main_data_get_cb,
					   (void *)&cnt);
		SETTING_TRACE("ret : %d", ret);
		ad->data_arr[i] = cnt;
	}
}

/**
  * This function get the data of per day in current week.
  */
static void __setting_datausage_main_hours_info_in_cur_day_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	time_t cur_time = time(0);

	ad->data_arr_len = SETTING_DATAUSAGE_HOURS_IN_DAY;
	int i = 0;
	for (; i < ad->data_arr_len; i++) {
		struct tm start;
		if (NULL == localtime_r(&cur_time, &start)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		start.tm_hour = i;
		start.tm_min = 0;
		start.tm_sec = 0;
		time_t from_time = mktime(&start);

		struct tm end;
		if (NULL == localtime_r(&from_time, &end)) {
			SETTING_TRACE_ERROR("fail to call localtime_r");
			return;
		}
		end.tm_hour = i;
		end.tm_min = 59;
		end.tm_sec = 59;
		time_t to_time = mktime(&end);

		SETTING_TRACE("from_time :%s", ctime(&from_time));
		SETTING_TRACE("to_time :%s", ctime(&to_time));

		unsigned long long cnt = 0;
		data_usage_selection_rule rule = { 0 };

		rule.granularity = 3600;
		rule.iftype = ad->iftype;
		rule.from = from_time;
		rule.to = to_time;

		rsml_ret_c ret = data_usage_foreach(&rule, __setting_datausage_main_data_get_cb,
					   (void *)&cnt);
		SETTING_TRACE("ret : %d", ret);
		ad->data_arr[i] = cnt;
	}
}


/**
  * This function will get the data
  * 1. If "Data usage cycle" is month, get the data of per day in current month
  * 2. If "Data usage cycle" is week, get the data of per day in current week
  * 3. If "Data usage cycle" is day, get the data of per hour in current day
  * 'Starting date" is only activated when user selected "Month".
  * So If user selected starting date by 1, graph shows on every month that day.
  */
static void __setting_datausage_main_data_stats_get(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int err = 0;
	int cycle = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	switch (cycle) {
	case 0: /* month */
		__setting_datausage_main_days_info_in_cur_month_get(ad);
		return;
	case 1: /* week */
		__setting_datausage_main_days_info_in_cur_week_get(ad);
		return;
	case 2: /* day */
		__setting_datausage_main_hours_info_in_cur_day_get(ad);
		return;
	default:
		/* do nothing */
		return;
	}
}


static void __setting_datausage_main_app_list_item_append(rsml_perf_info *pNode, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	setting_retm_if(NULL == pNode, "pNode is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	unsigned long long byte_size = 0;
	byte_size += pNode->foreground.cnt.outgoing_bytes;
	byte_size += pNode->foreground.cnt.incoming_bytes;
	char size[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	__setting_datausage_main_size_transition(byte_size, size);
	//SETTING_TRACE("pkg = %s, size = %s ", pNode->app_id, size);

	double percent = (double)byte_size / ad->apps_total_size;
	char desc[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%s/%4.2lf%%", size, percent * 100);

	Setting_GenGroupItem_Data *item_data = NULL;
	item_data = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retm_if(!item_data, "calloc failed");

	if (item_data) {
		item_data->keyStr = (char *)g_strdup(pNode->app_id);
		item_data->sub_desc = (char *)g_strdup(desc);
		item_data->userdata = pNode;
		item_data->slider_min = percent; /* use slider_min to store the percent here */
		item_data->item = elm_genlist_item_append(ad->genlist, &(ad->itc_2text_1icon_4), item_data, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("item_data is NULL");
	}

}

static Eina_Bool __setting_datausage_main_animator_cb(void *data)
{
	//SETTING_TRACE_BEGIN;
	setting_retvm_if(NULL == data, ECORE_CALLBACK_CANCEL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	int count = eina_list_count(ad->app_list);

	//SETTING_TRACE("app_list_index= %d", ad->app_list_index);
	if (ad->app_list_index >= count) {
		if (ad->animator) {
			ecore_animator_del(ad->animator);
			ad->animator = NULL;
		}

		Elm_Object_Item *item = NULL;
		item = elm_genlist_item_append(ad->genlist, &(ad->itc_seperator), NULL, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

		return ECORE_CALLBACK_CANCEL;
	}

	int i = ad->app_list_index++;

	rsml_perf_info *pNode = NULL;
	pNode = (rsml_perf_info *)eina_list_nth(ad->app_list, i);
	setting_retvm_if(NULL == pNode, ECORE_CALLBACK_RENEW, "pNode is NULL");

	__setting_datausage_main_app_list_item_append(pNode, ad);
	return ECORE_CALLBACK_RENEW;
}

static void __setting_datausage_main_date_desc_get(char *desc)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == desc, "desc is NULL");

	int err = 0;
	int cycle = 0;
	int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	time_t from_time;
	time_t to_time;
	switch (cycle) {
	case 0: /* month */
		{
		__setting_datausage_main_days_in_month_get(&from_time, &to_time);
		break;
		}
	case 1: /* week */
		{
		__setting_datausage_main_days_in_week_get(&from_time, &to_time);
		break;
		}
	case 2: /* day */
		{
		from_time = time(0);
		to_time = time(0);
		break;
		}
	default:
		{
		/* do nothing */
		SETTING_TRACE_ERROR("wrong data usage cycle");
		return;
		}
	}

	struct tm start_date;
	if (NULL == localtime_r(&from_time, &start_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return;
	}

	struct tm end_date;
	if (NULL == localtime_r(&to_time, &end_date)) {
		SETTING_TRACE_ERROR("fail to call localtime_r");
		return;
	}

	char start_date_desc[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
	strftime(start_date_desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%b", &start_date);

	int start_day = start_date.tm_mday;
	int end_day = end_date.tm_mday;

	if (start_day == end_day) {
		snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%s %d", start_date_desc, start_day);
	} else {
		snprintf(desc, MAX_DISPLAY_NAME_LEN_ON_UI, "%s %d-%d", start_date_desc, start_day, end_day);
	}
}

static void __setting_datausage_main_data_items_append(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;
	Elm_Object_Item *item = NULL;

	__setting_datausage_main_data_stats_get(ad);
	__setting_datausage_main_maximum_get(ad);

	int value = 0;
	int err = 0;
	int ret = 0;

	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	if (value && ad->iftype == RESMAN_IFACE_DATACALL) {
		/* limit graph */
		ad->limit_graph = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
		setting_retm_if(!ad->limit_graph, "calloc failed");

		if (ad->limit_graph) {
			ad->limit_graph->userdata = ad;
			ad->limit_graph->keyStr = (char *)g_strdup("limit_graph");
			ad->limit_graph->item = elm_genlist_item_append(ad->genlist, &(ad->itc_variable_height), ad->limit_graph, NULL,
						    ELM_GENLIST_ITEM_NONE, NULL, NULL);
			elm_genlist_item_select_mode_set(ad->limit_graph->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
		} else {
			SETTING_TRACE_ERROR("ad->limit_graph is NULL");
			return;
		}
	}

	/* graph image */
	ad->graph_image = (Setting_GenGroupItem_Data *) calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retm_if(!ad->graph_image, "calloc failed");

	if (ad->graph_image) {
		ad->graph_image->userdata = ad;
		ad->graph_image->keyStr = (char *)g_strdup("graph_image");
		ad->graph_image->item = elm_genlist_item_append(ad->genlist, &(ad->itc_variable_height), ad->graph_image, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(ad->graph_image->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->graph_image is NULL");
		return;
	}

	/* description */
	char date_info[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	__setting_datausage_main_date_desc_get(date_info);

	char used_size[MAX_DISPLAY_NAME_LEN_ON_UI] = {0};
	__setting_datausage_main_size_transition(ad->total_size, used_size);

	char used_str[SETTING_DATAUSAGE_DESC_LEN_MAX + 1] = {0,};
	snprintf(used_str, SETTING_DATAUSAGE_DESC_LEN_MAX, SETTING_DATAUSAGE_GRAPH_DESC_STR,
			date_info, used_size);

	char desc_str[2 * SETTING_DATAUSAGE_DESC_LEN_MAX + 1] = {0,};
	snprintf(desc_str, 2 * SETTING_DATAUSAGE_DESC_LEN_MAX, "%s<br><br>%s<br>",
			used_str, _(SETTING_DATAUSAGE_DESC_STR));
	ad->graph_desc = setting_create_Gendial_field_def(ad->genlist, &(ad->itc_variable_height), NULL,
					 ad, SWALLOW_Type_INVALID,
					 NULL, NULL, 0, desc_str, NULL,
					 NULL);
	if (ad->graph_desc) {
		ad->graph_desc->userdata = ad;
		elm_genlist_item_select_mode_set(ad->graph_desc->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->graph_desc is NULL");
		return;
	}

	Eina_List *app_list = __setting_datausage_main_apps_network_stats_get(ad);
	ad->app_list = app_list;

	int count = eina_list_count(ad->app_list);
	SETTING_TRACE("count = %d", eina_list_count(ad->app_list));

	ad->app_list_index = 0;

	/* calculate the total size of all apps */
	ad->apps_total_size = 0;
	Eina_List *l = NULL;
	rsml_perf_info *new_item;
	EINA_LIST_FOREACH(ad->app_list, l, new_item)
	{
		ad->apps_total_size += new_item->foreground.cnt.outgoing_bytes;
		ad->apps_total_size += new_item->foreground.cnt.incoming_bytes;
	}

	int i = 0;
	for (; i < SETTING_DATAUSAGE_ANIMATOR_START_INDEX; i++) {
		rsml_perf_info *pNode = NULL;
		pNode = (rsml_perf_info *)eina_list_nth(ad->app_list, i);
		if (pNode == NULL)
			continue;

		__setting_datausage_main_app_list_item_append(pNode, ad);
	}

	ad->app_list_index = i;

	if(count > SETTING_DATAUSAGE_ANIMATOR_START_INDEX) {
		/* to continue adding items using animator */
		if (ad->animator) {
			ecore_animator_del(ad->animator);
			ad->animator = NULL;
		}
		ad->animator = ecore_animator_add(__setting_datausage_main_animator_cb, ad);
	} else if (count <= SETTING_DATAUSAGE_ANIMATOR_START_INDEX && count > 0) {
		item = elm_genlist_item_append(ad->genlist, &(ad->itc_seperator), NULL, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		Setting_GenGroupItem_Data *no_app_item =  NULL;
		no_app_item = setting_create_Gendial_field_def(ad->genlist, &(ad->itc_1text),
						     NULL,
						     NULL, SWALLOW_Type_INVALID, NULL,
						     NULL, 0, SETTING_DATAUSAGE_NO_APPS_USE_DATA_STR,
						     NULL, NULL);
		if (no_app_item) {
			setting_disable_genlist_item(no_app_item->item);
		} else {
			SETTING_TRACE_ERROR("no_app_item is NULL");
			return;
		}

		item = elm_genlist_item_append(ad->genlist, &(ad->itc_seperator), NULL, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}

static void __setting_datausage_main_data_items_recreate(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	/* delete all data items in genlist */
	setting_retm_if(ad->starting_date == NULL, "ad->starting_date is NULL");
	Elm_Object_Item *item = elm_genlist_item_next_get(ad->starting_date->item);
	while (item) {
		elm_object_item_del(item);
		item = elm_genlist_item_next_get(ad->starting_date->item);
	}

	__setting_datausage_main_cairo_destroy(ad);
	__setting_datausage_main_limit_cairo_destroy(ad);

	/* recreate the data items in genlist */
	__setting_datausage_main_data_items_append(ad);
}

static void __setting_datausage_main_common_list_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(ad->genlist, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	/* Data usage cycle */
	ad->data_usage_cycle = setting_create_Gendial_exp_parent_field(ad->genlist,
							&(ad->itc_2text_3_parent),
							NULL, NULL,
							SWALLOW_Type_INVALID,
							SETTING_DATAUSAGE_DATA_USAGE_CYCLE_STR,
							__setting_datausage_main_data_usage_cycle_get());
	if (ad->data_usage_cycle) {
		ad->data_usage_cycle->userdata = ad;
		ad->data_usage_cycle->int_slp_setting_binded = INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT;
	} else {
		SETTING_TRACE_ERROR("ad->data_usage_cycle is NULL");
		return;
	}

	/* Starting date */
	ad->itc_1text_1icon_date.item_style = "dialogue/1text.1icon";
	ad->itc_1text_1icon_date.func.text_get = _item_label_get;
	ad->itc_1text_1icon_date.func.content_get = _item_content_date_get;
	ad->itc_1text_1icon_date.func.state_get = NULL;
	ad->itc_1text_1icon_date.func.del = _item_del;

	Setting_GenGroupItem_Data *starting_date = (Setting_GenGroupItem_Data *)calloc(1, sizeof(Setting_GenGroupItem_Data));
	setting_retm_if(!starting_date, "calloc failed");
	ad->starting_date = starting_date;
	starting_date->userdata = ad;
	starting_date->keyStr = (char *)g_strdup(SETTING_DATAUSAGE_STARTING_DATE_STR);

	starting_date->item = elm_genlist_item_append(ad->genlist, &(ad->itc_1text_1icon_date), starting_date, NULL,
					    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(starting_date->item, ELM_OBJECT_SELECT_MODE_NONE);

	__setting_datausage_main_data_items_append(ad);
}


static void __setting_datausage_main_mobile_list_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	elm_genlist_clear(ad->genlist); /* first to clear list */

	__setting_datausage_main_cairo_destroy(ad);
	__setting_datausage_main_limit_cairo_destroy(ad);

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(ad->genlist, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int value = 0;
	int err = 0;
	int ret = 0;

	/* Use packet data - ON/OFF */
	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_USE_PACKET_DATA, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->use_packet_data = setting_create_Gendial_field_def(ad->genlist, &(ad->itc_1text_1icon),
					     __setting_datausage_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, value,
					     KeyStr_UsePacketData, NULL,
					     __setting_datausage_main_chk_btn_cb);
	if (ad->use_packet_data) {
		ad->use_packet_data->userdata = ad;

		/* if flight mode is on, disable Use packet data */
		int value = 0;
		int err = 0;
		setting_get_bool_slp_key(BOOL_SLP_SETTING_FLIGHT_MODE, &value, &err);
		if (value) {
			setting_disable_genlist_item(ad->use_packet_data->item);
		}
	} else {
		SETTING_TRACE_ERROR("ad->data_use_packet is NULL");
		return;
	}

	/* Set data usage limit -ON/OFF */
	ret = setting_get_bool_slp_key(BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}
	ad->set_data_usage_limit = setting_create_Gendial_field_def(ad->genlist, &(ad->itc_1text_1icon),
					     __setting_datausage_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_1TOGGLE, NULL,
					     NULL, value, SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_STR,
					     NULL, __setting_datausage_main_chk_btn_cb);
	if (ad->set_data_usage_limit) {
		ad->set_data_usage_limit->userdata = ad;
	} else {
		SETTING_TRACE_ERROR("ad->set_data_usage_limit is NULL");
		return;
	}

	/* Data limit */
	ad->data_limit = setting_create_Gendial_field_def(ad->genlist, &(ad->itc_1text),
					     __setting_datausage_main_mouse_up_Gendial_list_cb,
					     ad, SWALLOW_Type_INVALID, NULL,
					     NULL, 0, SETTING_DATAUSAGE_DATA_LIMIT_STR,
					     NULL, NULL);
	if (ad->data_limit) {
		ad->data_limit->userdata = ad;

		/* if set_data_usage_limit's vconf is false, disable data_limit */
		if (!value) {
			setting_disable_genlist_item(ad->data_limit->item);
		}
	} else {
		SETTING_TRACE_ERROR("ad->data_limit is NULL");
		return;
	}

	__setting_datausage_main_common_list_create(ad);
}

static void __setting_datausage_main_wifi_list_create(void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	elm_genlist_clear(ad->genlist); /* first to clear list */
	__setting_datausage_main_cairo_destroy(ad);
	__setting_datausage_main_limit_cairo_destroy(ad);

	__setting_datausage_main_common_list_create(ad);
}

static void _first_tab_cb(void* data, Evas_Object* obj, void* event_info)
{
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	ad->iftype = RESMAN_IFACE_DATACALL;
	__setting_datausage_main_mobile_list_create(ad);
}

static void _second_tab_cb(void* data, Evas_Object* obj, void* event_info)
{
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	ad->iftype = RESMAN_IFACE_WIFI;
	__setting_datausage_main_wifi_list_create(ad);
}

static void __setting_datausage_main_sub_list_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);
	int err = 0;
	setting_set_int_slp_key(data_parentItem->int_slp_setting_binded,
				data_subItem->chk_status, &err);
	setting_retm_if(0 != err, "Set vconf error[%d]",data_parentItem->int_slp_setting_binded);

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);

	/* recreate the data items in genlist */
	__setting_datausage_main_data_items_recreate(ad);
}

static void __setting_datausage_main_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == data, "data is NULL");
	setting_retm_if(NULL == event_info, "event_info is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	Elm_Object_Item *parentItem = event_info;	/* parent item */

	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);

	Evas_Object *rgd = NULL;
	if (data_parentItem == ad->data_usage_cycle) {
		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		int i = 0;
		while (data_usage_cycle_arr[i]) {
			setting_create_Gendial_exp_sub_field(scroller, &(ad->itc_1icon_1text_sub),
						     __setting_datausage_main_sub_list_sel_cb, ad,
						     parentItem, SWALLOW_Type_1RADIO, rgd,
						     i, data_usage_cycle_arr[i],
						     NULL);
			i++;
		}

		setting_update_chk_status(rgd, data_parentItem->int_slp_setting_binded);
	}
}

static void __setting_datausage_main_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *)data;

	int status = vconf_keynode_get_bool(key);
	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DATA_LIMIT_INT)) {
		__setting_datausage_main_datausage_quota_set();
		__setting_datausage_main_data_items_recreate(ad);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_DATA_EACH_MONTH_INT)) {
		if (ad->iftype == RESMAN_IFACE_DATACALL) {
			__setting_datausage_main_datausage_quota_set();
		}

		int err = 0;
		int cycle = 0;
		int ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, &cycle, &err);
		if (ret != 0) {
			SETTING_TRACE_ERROR("get vconf failed");
		}

		/* if cycle is not month, don't need to recreate the data items in genlist */
		if (cycle != 0) {
			return;
		}

		__setting_datausage_main_data_items_recreate(ad);
	} else if (!safeStrCmp(vconf_name, VCONFKEY_TELEPHONY_FLIGHT_MODE)) {
		if (ad->use_packet_data) {
			if (status) {
				setting_disable_genlist_item(ad->use_packet_data->item);
			} else {
				setting_enable_genlist_item(ad->use_packet_data->item);
			}
		}
	} else {
		SETTING_TRACE_ERROR("vconf_name is wrong");
	}
}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_datausage_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER, "cb is NULL");
	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	ad->itc_variable_height.item_style = "1icon";
	ad->itc_variable_height.func.text_get = NULL;
	ad->itc_variable_height.func.content_get = _item_content_get;
	ad->itc_variable_height.func.state_get = NULL;
	ad->itc_variable_height.func.del = _item_del;

	ad->itc_2text_1icon_4.item_style = "dialogue/2text.1icon.4";
	ad->itc_2text_1icon_4.func.text_get = _item_label_get;
	ad->itc_2text_1icon_4.func.content_get = _item_content3_get;
	ad->itc_2text_1icon_4.func.state_get = NULL;
	ad->itc_2text_1icon_4.func.del = _item_del;

	Evas_Object *layout = setting_create_win_layout(ad->win_main_layout, ad->win_get);
	setting_retvm_if(layout == NULL, SETTING_RETURN_FAIL, "layout is NULL");
	ad->ly_main = layout;

	/* create a navigation bar */
	ad->navibar = setting_create_navi_bar(layout);

	/*  add controlbar */
	/* create a GenList */
	ad->genlist = elm_genlist_add(ad->navibar);

	if ( ! ad->genlist)
	{
		SETTING_TRACE(">>>>> ad->genlist is NULL ");
	}

	elm_object_style_set(ad->genlist, "dialogue");
	setting_retvm_if(ad->genlist == NULL, SETTING_RETURN_FAIL, "genlist is NULL");

	setting_enable_expandable_genlist(ad->genlist, ad, __setting_datausage_main_exp_cb, NULL);

	Elm_Object_Item *navi_it = NULL;
	Evas_Object *l_button = NULL;
	l_button = setting_create_button(ad->navibar, _("IDS_COM_BODY_BACK"), NAVI_BACK_BUTTON_STYLE, __setting_datausage_main_click_softkey_back_cb, ad);

	navi_it = elm_naviframe_item_push(ad->navibar, _(KeyStr_DataUsage), l_button, NULL, ad->genlist, "tabbar");
	if ( ! navi_it)
	{
		SETTING_TRACE(">>>>> navi_it is NULL ");
	}

	ad->tabbar = elm_toolbar_add(ad->navibar);
	if ( ! ad->tabbar )
	{
		SETTING_TRACE(">>>> ad->tabbar is NULL");
	}

	elm_toolbar_shrink_mode_set(ad->tabbar, ELM_TOOLBAR_SHRINK_EXPAND);
	elm_object_style_set(ad->tabbar, "tabbar");
	elm_toolbar_transverse_expanded_set(ad->tabbar, EINA_TRUE);

	elm_object_item_part_content_set(navi_it, "elm.swallow.tabbar", ad->tabbar);

	if (ad->tabbar)
	{
		ad->tabbar_item[0] = elm_toolbar_item_append(ad->tabbar, NULL, SETTING_DATAUSAGE_MOBILE_STR, _first_tab_cb,ad);
		ad->tabbar_item[1] = elm_toolbar_item_append(ad->tabbar, NULL, _("IDS_ST_HEADER_WI_FI"), _second_tab_cb,ad);
		elm_object_item_part_content_set(navi_it, "tabbar", ad->tabbar);
	}

	elm_toolbar_select_mode_set(ad->tabbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
	elm_toolbar_item_selected_set(ad->tabbar_item[0], EINA_TRUE);

	int ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_DATA_LIMIT_INT, __setting_datausage_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_DATA_EACH_MONTH_INT,
			__setting_datausage_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	ret = vconf_notify_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE,
			__setting_datausage_main_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}

	setting_view_datausage_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_datausage_main_destroy(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	int ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_DATA_LIMIT_INT,
			__setting_datausage_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_DATA_EACH_MONTH_INT,
			__setting_datausage_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	ret = vconf_ignore_key_changed(VCONFKEY_TELEPHONY_FLIGHT_MODE,
			__setting_datausage_main_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}

	__setting_datausage_main_apps_list_destroy(ad);

	if (ad->animator) {
		ecore_animator_del(ad->animator);
		ad->animator = NULL;
	}

	if (ad->ctxpopup) {
		evas_object_del(ad->ctxpopup);
		ad->ctxpopup = NULL;
	}

	if (ad->data_limit_conformant) {
		evas_object_del(ad->data_limit_conformant);
		ad->data_limit_conformant = NULL;
	}

	if (ad->data_limit_popup) {
		evas_object_del(ad->data_limit_popup);
		ad->data_limit_popup = NULL;
	}

	__setting_datausage_main_cairo_destroy(ad);
	__setting_datausage_main_limit_cairo_destroy(ad);

	if (ad->tabbar)
	{
		evas_object_del(ad->tabbar);
		ad->tabbar = NULL;
	}

	if (ad->ly_main) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
		setting_view_datausage_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_datausage_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	if (ad->ly_main) {
		evas_object_show(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_datausage_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	if (ad->ly_main) {
		evas_object_hide(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

static void __setting_datausage_main_click_softkey_back_cb(void *data, Evas_Object *obj,
					void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	SettingDatausageUG *ad = (SettingDatausageUG *) data;
	/* Send destroy request */
	ug_destroy_me(ad->ug);
}
