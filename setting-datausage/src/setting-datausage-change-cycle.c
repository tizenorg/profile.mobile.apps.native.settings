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

#include <setting-datausage-change-cycle.h>

static int __setting_datausage_change_cycle_create(void *cb);
static int __setting_datausage_change_cycle_destroy(void *cb);
static int __setting_datausage_change_cycle_update(void *cb);
static int __setting_datausage_change_cycle_cleanup(void *cb);

setting_view setting_view_datausage_change_cycle = {
	.create = __setting_datausage_change_cycle_create,
	.destroy = __setting_datausage_change_cycle_destroy,
	.update = __setting_datausage_change_cycle_update,
	.cleanup = __setting_datausage_change_cycle_cleanup,
};

static char *_item_label_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;

	if (!safeStrCmp(part, "elm.text") || !safeStrCmp(part, "elm.text.1")) {	/* title */
		if (item_data->keyStr) {
			return (char *)g_strdup(_(item_data->keyStr));
		}
	} else if (!safeStrCmp(part, "elm.text.2")) {	/* bottom or right */
		if (item_data->sub_desc) {
			return (char *)g_strdup(item_data->sub_desc);
		}
	}


	return NULL;
}

static Evas_Object *_item_content_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	//int width = (int) (690 * elm_scale_get());

	if (!safeStrCmp(part, "elm.icon") || !safeStrCmp(part, "elm.icon.1")) {
		Evas_Object *datetimefield = elm_datetime_add(obj);
		item_data->eo_check = datetimefield;

		elm_datetime_field_visible_set(datetimefield, ELM_DATETIME_YEAR, EINA_FALSE);
		elm_datetime_field_visible_set(datetimefield, ELM_DATETIME_MONTH, EINA_FALSE);
		elm_datetime_field_visible_set(datetimefield, ELM_DATETIME_HOUR, EINA_FALSE);
		elm_datetime_field_visible_set(datetimefield, ELM_DATETIME_MINUTE, EINA_FALSE);
		elm_datetime_field_visible_set(datetimefield, ELM_DATETIME_AMPM, EINA_FALSE);

		evas_object_size_hint_weight_set(datetimefield, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(datetimefield, 0.5, 0.5);
		evas_object_show(datetimefield);

		char date_format[MAX_DATETIME_FORMAT_LEN + 1] = {0, };
		const char *time_format = "%H : %M";

		snprintf(date_format, MAX_DATETIME_FORMAT_LEN,
				 "%s %s", item_data->sub_desc, time_format);
		elm_datetime_format_set(datetimefield, date_format);

		struct tm ts_ret;
		time_t ctime = time(NULL);
		struct tm *ts = localtime_r(&ctime, &ts_ret);
		retv_if(!ts, NULL);
		ts_ret.tm_mday = item_data->chk_status;

		elm_datetime_value_set(datetimefield, &ts_ret);

		if (item_data->chk_change_cb) {
			evas_object_smart_callback_add(datetimefield, "changed",
						       item_data->chk_change_cb, item_data);
		}
		return datetimefield;
	}

	return NULL;
}

static void _item_del(void *data, Evas_Object *obj)
{
	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) data;
	if (item_data) {
		G_FREE(item_data->keyStr);
		G_FREE(item_data->sub_desc);
		FREE(item_data);
	}
}

/* ***************************************************
**
** basic func
**
** **************************************************/

static int __setting_datausage_change_cycle_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingDatausageUG *ad = (SettingDatausageUG *) cb;
	Evas_Object *scroller;

	setting_push_layout_navi_bar_genlist(ad->win_main_layout,
					ad->win_get,
					_(SETTING_DATAUSAGE_CHANGE_CYCLE_STR),
					_("IDS_COM_BODY_BACK"),
					NULL,
					__setting_datausage_change_cycle_back_cb,
					NULL,
					ad, &scroller, ad->navibar);

	setting_retvm_if(scroller == NULL, SETTING_RETURN_FAIL, "scroller is NULL");

	Elm_Object_Item *item = NULL;;

	item = elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->itc_layout.item_style = "dialogue/1text.1icon.5";
	ad->itc_layout.func.text_get = _item_label_get;
	ad->itc_layout.func.content_get = _item_content_get;
	ad->itc_layout.func.state_get = NULL;
	ad->itc_layout.func.del = _item_del;


	int value = 0;
	int err = 0;
	int ret = 0;

	ret = setting_get_int_slp_key(INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, &value, &err);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
	}

	ad->change_data = setting_create_Gendial_field_def(scroller, &(ad->itc_layout),
					     NULL,
					     ad, SWALLOW_Type_LAYOUT_DATEFIELD,
					     NULL, NULL, value,
					     SETTING_DATAUSAGE_ON_THIS_DATE_EACH_MONTH_STR,
					     "%Y %b %d",
					     NULL);

	setting_view_datausage_change_cycle.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_change_cycle_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingDatausageUG *ad = (SettingDatausageUG *) cb;

	elm_naviframe_item_pop(ad->navibar);
	setting_view_datausage_change_cycle.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_change_cycle_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	return SETTING_RETURN_SUCCESS;
}

static int __setting_datausage_change_cycle_cleanup(void *cb)
{
	//error check
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	__setting_datausage_change_cycle_destroy(cb);
	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
**
** call back func
**
** **************************************************/
static void __setting_datausage_change_cycle_back_cb(void *data, Evas_Object * obj, void *event_info)
{
	//error check
	ret_if(data == NULL);
	SettingDatausageUG *ad = (SettingDatausageUG *) data;

	setting_view_change(&setting_view_datausage_change_cycle, &setting_view_datausage_main, ad);
}
