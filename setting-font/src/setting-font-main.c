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
#include <setting-font-main.h>
#include <dd-deviced.h>
#include <fontconfig/fontconfig.h>
#include <unicode/uloc.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <setting-font-font-size.h>

#include <eventsystem.h>

#define SETTING_FONT_MAX_LEN 10
#define FONT_BUF_SIZE 256

static int setting_font_main_create(void *cb);
static int setting_font_main_destroy(void *cb);
static int setting_font_main_update(void *cb);
static int setting_font_main_cleanup(void *cb);

static Eina_Bool __setting_font_main_click_softkey_back_cb(void *data,
		Elm_Object_Item *it);
static void _eo_slider_mouse_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info);
static void _slider_mouse_cb(void *data, Evas_Object *obj, void *event_info);
static int setting_font_style_is_exist_string(const Eina_List *list,
		const char *key);
static void __setting_get_font_size_str(void *data, int size);
static void __setting_get_font_type_str(void *data, char *font_data);

setting_view setting_view_font_main = {
		.create = setting_font_main_create,
		.destroy = setting_font_main_destroy,
		.update = setting_font_main_update,
		.cleanup = setting_font_main_cleanup, };

static void setting_font_rot_changed_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *ad = (SettingFontUG *) data;
	if (ad == NULL || ad->win_get == NULL) {
		return;
	}
	int change_ang = elm_win_rotation_get(ad->win_get);

	switch (change_ang) {
	case APP_DEVICE_ORIENTATION_0:
		/*SETTING_TRACE("change_ang : 0"); */
		ad->rotate_angle = 0;
		break;
	case APP_DEVICE_ORIENTATION_180:
		/*SETTING_TRACE("change_ang : 180"); */
		ad->rotate_angle = 180;
		break;
	case APP_DEVICE_ORIENTATION_270:
		/*SETTING_TRACE("change_ang : 270"); */
		ad->rotate_angle = 270;
		break;
	case APP_DEVICE_ORIENTATION_90:
		/*SETTING_TRACE("change_ang : 90"); */
		ad->rotate_angle = 90;
		break;
	default:
		return;
	}
}

char *get_example_style_text(const char *font_size, const char *font_type)
{
	char default_example_str[MAX_COMMON_BUFFER_LEN + 1] = { 0, };
	snprintf(default_example_str, MAX_COMMON_BUFFER_LEN,
			"<color=#000000><font_size=%s><font=%s>%s<br>%s<br>%s</font></font_size></color>",
			font_size, font_type,
			"<align=middle>ABCDE</align>",
			"<align=middle>abcde</align>",
			"<align=middle>12345!@#$</align>");

	return (char *) g_strdup(default_example_str);

}
static int _format_font_name_by_id(char *font_data, char *font_name_id,
		char *temp, int bufsize)
{
	retvm_if(font_data == NULL, 0, "Invalid argument: event info is NULL");

	char *pos = font_data;
	char new_name[bufsize];
	int i = 0;
	int count = 0;
	while (*pos != '\0') {
		if (*pos == ' ') {
			SETTING_TRACE(">>>	empty ' ' ");
			new_name[count] = '\\';
			count++;
			new_name[count] = ' ';
		} else {
			new_name[count] = font_data[i];
		}

		count++;
		pos++;
		i++;
	}
	new_name[count] = '\0';
	char trans_font_type_name[FONT_BUF_SIZE] = { 0, };
	if (safeStrCmp(font_name_id, "IDS_ST_BODY_CHOCO_COOKY") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_CHOCO_COOKY"));
	} else if (safeStrCmp(font_name_id, "IDS_ST_BODY_COOL_JAZZ") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_COOL_JAZZ"));
	} else if (safeStrCmp(font_name_id, "IDS_ST_BODY_FONTSTYLE_ROSEMARY")
			== 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_FONTSTYLE_ROSEMARY"));
	} else if (safeStrCmp(font_name_id, "IDS_ST_BODY_DEFAULT_FONT") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_DEFAULT_FONT"));
	} else {
		snprintf(trans_font_type_name, bufsize, "%s", font_name_id);
	}
	snprintf(temp, bufsize, "<font=%s>%s</font>", new_name,
			trans_font_type_name);
	return 1;
}

char *_item_text_keystr2_get(void *data, Evas_Object *obj, const char *part)
{
	setting_retvm_if(data == NULL, NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *item_data =
			(Setting_GenGroupItem_Data *) data;
	char *ret_str = NULL;

	if (!safeStrCmp(part, "elm.text")) {
		if (item_data->keyStr2) {
			char temp[FONT_BUF_SIZE] = { 0, };

			int ret = _format_font_name_by_id(item_data->sub_desc,
					item_data->keyStr2, temp,
					FONT_BUF_SIZE);

			if (ret == 0) { /* error condition */
				SETTING_TRACE_ERROR("Error condition font");
			}

			SETTING_TRACE_DEBUG("temp : %s", (char * )temp);
			ret_str = (char *) g_strdup(temp);
		}
	}
	/*SETTING_TRACE_END; */
	return ret_str;
}

static Evas_Object *_font_size_slider_get(void *data, Evas_Object *obj,
		const char *part)
{
	SETTING_TRACE_BEGIN;

	retv_if(!data, NULL);
	Setting_GenGroupItem_Data *item_data = data;
	SETTING_TRACE(" --------------------> part:%s", part);

	if (!safeStrCmp(part, "elm.swallow.content")) {

		if (SWALLOW_Type_LAYOUT_5STEP_SLIDER
				== item_data->swallow_type) {
			SETTING_TRACE("Add 5Step Slider");

			Evas_Object *layout = elm_layout_add(obj);
			Eina_Bool
			ret = elm_layout_file_set(layout,
					SETTING_THEME_EDJ_NAME, "font_slider");
			if (ret == EINA_TRUE) { /* error condition */
				SETTING_TRACE("elm_layout_file_set - OK");
			} else {
				SETTING_TRACE_ERROR(
						"elm_layout_file_set - FAILED");
			}

			evas_object_size_hint_weight_set(layout,
					EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

			Evas_Object *label = elm_label_add(obj);
			elm_object_text_set(label,
					_("<color=#000000><font_size=23>"
							"A</font_size></color>"));
			evas_object_show(label);
			elm_object_part_content_set(layout, "left_text", label);
			Evas_Object *label2 = elm_label_add(obj);
			elm_object_text_set(label2,
					_("<color=#000000><font_size=30>"
							"A</font_size></color>"));
			evas_object_show(label2);
			elm_object_part_content_set(layout, "right_text",
					label2);
			Evas_Object *li_slider = setting_create_5step_slider(
					obj, item_data->evas,
					item_data->l_swallow_path,
					item_data->r_swallow_path,
					item_data->chk_status,
					item_data->isIndicatorVisible,
					item_data->slider_min,
					item_data->slider_max,
					item_data->chk_change_cb,
					item_data->start_change_cb,
					item_data->stop_change_cb, item_data);
			item_data->eo_check = li_slider;
			item_data->mouse_up_cb = _slider_mouse_cb;
			evas_object_event_callback_add(li_slider,
					EVAS_CALLBACK_MOUSE_UP,
					_eo_slider_mouse_cb, item_data);

			elm_object_part_content_set(layout, "slider",
					li_slider);
			return layout;
		}
	}
	else if (!safeStrCmp(part, "elm.text")) {
		SETTING_TRACE("item_data->keyStr:%s",
				item_data->keyStr);
		SETTING_TRACE("item_data->sub_desc:%s",
				item_data->sub_desc);
		int padding_h = 0;
		Evas_Object *box = elm_box_add(obj);
		evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND,
				EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(box, EVAS_HINT_FILL,
				EVAS_HINT_FILL);
		elm_box_padding_set(box, 10, padding_h);

		elm_box_pack_end(box,
				setting_create_blank_rect_customize(obj,
						1, 20));

		item_data->eo_check = setting_create_textbox(obj,
				item_data->sub_desc);
		elm_box_pack_end(box, item_data->eo_check);
		elm_box_pack_end(box,
				setting_create_blank_rect_customize(obj,
						1, 20));

		return box;

	}

	return NULL;
}

static int setting_font_style_is_exist_string(const Eina_List *list,
		const char *key)
{
	Eina_List *l = NULL;
	if ((key == NULL) || (list == NULL)) {
		return 0;
	}

	for (l = (Eina_List *) list; l; l = eina_list_next(l)) {
		if (l->data && (!strcmp((char *) l->data, key))) {
			return 1;
		}
	}

	return 0;
}

/**
 * [API] get a font list from current system
 * REFACTORING: move this feature to system-settings
 */
static Eina_List *__setting_font_main_available_list_get()
{
	SETTING_TRACE_BEGIN;

	FcObjectSet *os = NULL;
	FcFontSet *fs = NULL;
	FcPattern *pat = NULL;
	Eina_List *list = NULL;
	FcConfig *font_config = NULL;

	font_config = FcInitLoadConfigAndFonts();

	setting_retvm_if(font_config == NULL, NULL,
			"Failed: FcInitLoadConfigAndFonts");

	char *locale = setlocale(0, NULL);

	pat = FcPatternCreate();

	os = FcObjectSetBuild(FC_FAMILY, FC_FILE, FC_FAMILYLANG, (char *) 0);

	if (os) {
		fs = FcFontList(font_config, pat, os);
		FcObjectSetDestroy(os);
		os = NULL;
	}

	if (pat) {
		FcPatternDestroy(pat);
		pat = NULL;
	}

	if (fs) {
		int j;
		SETTING_TRACE_DEBUG("fs->nfont = %d", fs->nfont);

		for (j = 0; j < fs->nfont; j++) {

			FcChar8 *family = NULL;
			FcChar8 *file = NULL;
			if (FcPatternGetString(fs->fonts[j], FC_FILE, 0, &file)
					== FcResultMatch) {
				/*SETTING_TRACE_DEBUG("FC_FILE : %s\n", file);*/
				int preload_path_len = safeStrLen(
						SETTING_FONT_PRELOAD_FONT_PATH);
				int download_path_len = safeStrLen(
						SETTING_FONT_DOWNLOADED_FONT_PATH);
				if (!safeStrNCmp((const char *)file,
						SETTING_FONT_PRELOAD_FONT_PATH, preload_path_len)
						|| !safeStrNCmp((const char *)file,
								SETTING_FONT_DOWNLOADED_FONT_PATH,
								download_path_len)) {
					char *family_result = NULL;
					FcChar8 *lang = NULL;
					int id = 0;
					if (FcPatternGetString(fs->fonts[j],
							FC_FAMILY, id, &family)
							!= FcResultMatch) {
						break;
					}
					if (FcPatternGetString(fs->fonts[j],
							FC_FAMILYLANG, id,
							&lang)
							!= FcResultMatch) {
						break;
					}
					family_result = (char *) family;

					/* Find proper family name for current locale. */
					while (locale && family && lang) {
						if (!strncmp(locale,
								(char *) lang,
								strlen(
										(char *) lang))) {
							family_result = (char *) family;
							break;
						}

						/* I will set english as default family language. */
						/* If there is no proper family language for current locale, */
						/* we have to show the english family name. */
						if (!strcmp((char *) lang,
								"en")) {
							family_result = (char *) family;
						}
						id++;
						if (FcPatternGetString(
								fs->fonts[j],
								FC_FAMILY, id,
								&family)
								!= FcResultMatch) {
							break;
						}
						if (FcPatternGetString(
								fs->fonts[j],
								FC_FAMILYLANG,
								id, &lang)
								!= FcResultMatch) {
							break;
						}
					}
					if (family_result
							&& !setting_font_style_is_exist_string(
									list,
									family_result)) {
						list = eina_list_sorted_insert(
								list,
								(Eina_Compare_Cb) safeStrCmp,
								strdup(
										family_result));
						SETTING_TRACE_DEBUG(
								"-------- ADDED FONT - family = %s",
								family_result);
						/* list = eina_list_append(list, family_result); */
						/* for TEST because there's 1 font on target. */
					}
				}
			}
		}
		FcFontSetDestroy(fs);
		fs = NULL;
	}

	FcConfigDestroy(font_config);
	font_config = NULL;
	return list;
}
int get_font_name_id(char *font_data, char *temp, int bufsize)
{
	/*SETTING_TRACE("font data : %s", (char*)font_data); */
	if (safeStrCmp(font_data, "Choco cooky") == 0) {
		snprintf(temp, bufsize, "%s", "IDS_ST_BODY_CHOCO_COOKY");
	} else if (safeStrCmp(font_data, "Cool jazz") == 0) {
		snprintf(temp, bufsize, "%s", "IDS_ST_BODY_COOL_JAZZ");
	} else if (safeStrCmp(font_data, "Rosemary") == 0) {
		snprintf(temp, bufsize, "%s", "IDS_ST_BODY_FONTSTYLE_ROSEMARY");
	} else {
		snprintf(temp, bufsize, "%s", font_data);
	}
	return 1;
}

int get_font_name_str(char *font_type_name, char *trans_font_type_name,
		int bufsize)
{
	/*SETTING_TRACE_BEGIN; */
	if (safeStrCmp(font_type_name, "Choco cooky") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_CHOCO_COOKY"));
		/*SETTING_TRACE("%s", trans_font_type_name); */
	} else if (safeStrCmp(font_type_name, "Cool jazz") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_COOL_JAZZ"));
		/*SETTING_TRACE("%s", trans_font_type_name); */
	} else if (safeStrCmp(font_type_name, "Rosemary") == 0) {
		snprintf(trans_font_type_name, bufsize, "%s",
				_("IDS_ST_BODY_FONTSTYLE_ROSEMARY"));
		/*SETTING_TRACE("%s", trans_font_type_name); */
	} else {
		snprintf(trans_font_type_name, bufsize, "%s", font_type_name);
		/*SETTING_TRACE("%s", trans_font_type_name); */
	}
	return 1;
}

static void __setting_progress_popup_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SettingFontUG *ad = (SettingFontUG *) data;
	SETTING_TRACE("send ug_destroy_me by callback");
	if (ad->main_popup) {
		evas_object_del(ad->main_popup);
		ad->main_popup = NULL;
	}
	ad->font_change_status = SELECTED_FONT_CHANGE_DONE;

	ug_destroy_me(ad->ug);
}

static void _event_set_font_type_helper(char *font_name)
{
	int ret = system_settings_set_value_string(
			SYSTEM_SETTINGS_KEY_FONT_TYPE, font_name);
	if (ret == SYSTEM_SETTINGS_ERROR_NONE) {
		/* on success */
		SETTING_TRACE("SYSTEM_SETTINGS_KEY_FONT_TYPE is OK : %s",
				font_name);
	}
}

static Eina_Bool __slide_timer(void *data)
{
	SETTING_TRACE_BEGIN;

	Setting_GenGroupItem_Data *list_item = data;
	SettingFontUG *ad = list_item->userdata;

	double val = elm_slider_value_get(list_item->eo_check);
	int value = (int) (val + 0.5);
	SETTING_TRACE("val:%f", val);
	elm_slider_value_set(list_item->eo_check, value);
	list_item->chk_status = elm_slider_value_get(list_item->eo_check);

	if (ad->init_font_size == list_item->chk_status) {
		ad->size_change_flag = FALSE;
	} else {
		ad->size_change_flag = TRUE;
	}
	ad->ret_font_size = list_item->chk_status;

	if (ad->font_example) {
		__setting_get_font_size_str(ad, list_item->chk_status);
		char *tmp = ad->font_example->sub_desc;
		ad->font_example->sub_desc = get_example_style_text(
				ad->font_size_str, ad->font_type_str);
		G_FREE(tmp);
		elm_entry_entry_set(ad->font_example->eo_check,
				ad->font_example->sub_desc);

		elm_genlist_item_fields_update(ad->font_example->item, "elm.text",ELM_GENLIST_ITEM_FIELD_ALL);

	}

	/* finalize */
	ad->timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

static void __font_size_slider_value_change_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;

	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	SettingFontUG *ad = list_item->userdata;
	retm_if(ad == NULL, "ad parameter is NULL");
	/*for sliding performance */
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}
	ad->timer = ecore_timer_add(0.2, (Ecore_Task_Cb) __slide_timer,
			list_item);
}

/**
 * [UI][expandable list][event handler:selection][font change]
 */
void setting_font_main_list_sel_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;

	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(
			subitem);
	ret_if(NULL == data_subItem);

	SettingFontUG *ad = (SettingFontUG *) data;

	/*	if not change, return */
	if (ad->prev_font == data_subItem->chk_status) {
		SETTING_TRACE("NOT CHANGED ");
		return;
	}
	if (ad->font_change_status == SELECTED_FONT_CHANGE_IN_PROCESS) {
		return;
	}
	ad->subitem = data_subItem;
	/* Fix PLM P131016-04680
	 when user check the option, make sure the radio button is checked for
	 prev_font font item the radio of new font item will be checked when
	 user seleck "ok" on popup*/
	elm_radio_value_set(ad->subitem->rgd, data_subItem->chk_status);

	/* extract font name from tag mixed string */
	SETTING_TRACE("ad->subitem->keyStr --> popup - get FONT : %s ",
			ad->subitem->keyStr);
	SETTING_TRACE("sub_desc --> popup - get FONT : %s ",
			ad->subitem->sub_desc);

	/* set font name to font main and font config */
	/*for mincho font type, need to display "Mincho",but need to update
	 * the real font with "UDMincho"*/
	/*keyStr is used to store the string "Mincho", and sub_desc is used
	 * to store "UDMDu4"*/

	if (ad->font_type) {
		ad->font_type->sub_desc = (char *) g_strdup(
				ad->subitem->keyStr);
		elm_object_item_data_set(ad->font_type->item, ad->font_type);
		elm_genlist_item_update(ad->font_type->item);
	}

	if (ad->font_example) {
		__setting_get_font_type_str(ad, ad->subitem->sub_desc);
		char *tmp = ad->font_example->sub_desc;
		SETTING_TRACE_DEBUG("ad->font_type_str:%s", ad->font_type_str);
		ad->font_example->sub_desc = get_example_style_text(
				ad->font_size_str, ad->font_type_str);
		G_FREE(tmp);
		elm_entry_entry_set(ad->font_example->eo_check,
				ad->font_example->sub_desc);
	}

	if (ad->init_font_type == data_subItem->chk_status) {
		ad->type_change_flag = FALSE;
	} else {
		ad->type_change_flag = TRUE;
	}

	/* sub - descc */
	char *font_name = ad->subitem->sub_desc;

	ad->prev_font = ad->subitem->chk_status;
	G_FREE(ad->font_name);
	ad->font_name = (char *) g_strdup(_(font_name));
	SETTING_TRACE("ad->subitem->keyStr2 = %s", font_name);

	/*when the expandlist item is NOT expanded, the subitem object is not
	 * existed,so need to clear the list*/
	if (ad->font_type_list != NULL) {
		eina_list_free(ad->font_type_list);
		ad->font_type_list = NULL;
	}
}

static void __font_vconf_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;

	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	SettingFontUG *ad = (SettingFontUG *) data;

	char *vconf_name = vconf_keynode_get_name(key);

	if (!safeStrCmp(vconf_name, VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME)) {

		char *font_type_name = NULL;
		int ret = system_settings_get_value_string(
				SYSTEM_SETTINGS_KEY_FONT_TYPE, &font_type_name);
		if (ret != 0) {
			SETTING_TRACE("system-setting call failed with error code %d",
					ret);
		}
		SETTING_TRACE("---------------- system-setting call : (%s) ",
				font_type_name);
		char *default_font_name = NULL;
		default_font_name = _get_default_font();
		char trans_font_type_name[FONT_BUF_SIZE] = { 0, };
		if (!safeStrCmp(default_font_name, font_type_name)) {
			snprintf(trans_font_type_name, FONT_BUF_SIZE, "%s",
					SETTING_DEFAULT_FONT_TYPE);
		} else {
			ret = get_font_name_id(font_type_name,
					trans_font_type_name, FONT_BUF_SIZE);
		}

		if (ad->font_type) {
			G_FREE(ad->font_type->sub_desc);
			ad->font_type->sub_desc = (char *) g_strdup(
					trans_font_type_name);
			elm_object_item_data_set(ad->font_type->item,
					ad->font_type);
			elm_genlist_item_update(ad->font_type->item);
			SETTING_TRACE("ad->font_type->sub_desc:%s",
					trans_font_type_name);

			if (elm_genlist_item_expanded_get(
					ad->font_type->item)) {
				Eina_List *l = NULL;
				Setting_GenGroupItem_Data *font_type_subitem;
				EINA_LIST_FOREACH(ad->font_type_list, l,
						font_type_subitem)
				{
					if (font_type_subitem
							&& !safeStrCmp(font_type_subitem->sub_desc,
									font_type_name)) {

						if (font_type_subitem
								&& font_type_subitem->rgd) {
							elm_radio_value_set(
									font_type_subitem->rgd,
									font_type_subitem->chk_status);
							if (ad->font_change_status
									!= SELECTED_FONT_CHANGE_IN_PROCESS) {
								ad->subitem = font_type_subitem;
							}
						}
						break;

					}
				}
			}
		}
	} else {
		SETTING_TRACE_ERROR("vconf_name is error");
	}
	SETTING_TRACE_END;
}

#define SLIDER_MAX_VALUE 4

#define SLIDER_START_POINT_X( ad )	_slider_startpoint_x(ad)
#define SLIDER_END_POINT_X( ad ) (_slider_get_width(ad) - SLIDER_START_POINT_X(ad))
#define SLIDER_LENGTH(ad) (SLIDER_END_POINT_X(ad) - SLIDER_START_POINT_X(ad))

static int _slider_get_width(void *data)
{
	SETTING_TRACE_BEGIN;

	SettingFontUG *ad = (SettingFontUG *) data;

	int x, y, w, h;
	elm_win_screen_size_get(ad->win_get, &x, &y, &w, &h);
	SETTING_TRACE("-------> x : %d ", x);
	SETTING_TRACE("-------> y : %d ", y);
	SETTING_TRACE("-------> w : %d ", w);
	SETTING_TRACE("-------> h : %d ", h);

	switch (ad->rotate_angle) {
	case 0:
	case 360:
		return w;
		break;
	case 90:
	case 270:
		return h;
		break;
	default:
		return w;
	}
	return w;
}

#if 0
/* 720, 1280 */
static int _slider_endpoint_x()
{
#ifdef ECORE_X
	int w, h;
	ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
	return w;
#else
	int w, h;
	elm_win_screen_size_get(win, NULL, NULL, &w, &h);
	return w;
#endif
}
#endif

/**
 * ad->rotate_angle = 90;	// do it
 * ad->rotate_angle = 270;	// do it
 */
static int _slider_startpoint_x(void *data)
{
	SETTING_TRACE_BEGIN;

	int width = 47;
#if 0
	SettingFontUG *ad = (SettingFontUG *)data;
	if (_slider_get_width(ad) == 720) /* M0 */
	return 47;
#endif
	return width;
}

static void _eo_slider_mouse_cb(void *data, Evas *e, Evas_Object *obj,
		void *event_info)
{
	_slider_mouse_cb(data, obj, event_info);
}

static void _slider_mouse_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	if (data == NULL || obj == NULL || event_info == NULL)
		return;

	Setting_GenGroupItem_Data *list_item = data;
	SettingFontUG *ad = list_item->userdata;

	Evas_Object *slider = obj;
	Evas_Event_Mouse_Down *ev = event_info;

	int val = 0;
	int max_vol = SLIDER_MAX_VALUE;
	int start_x = SLIDER_START_POINT_X(ad);
	double current = 0.0;

	{
		current = ev->canvas.x - start_x;
		SETTING_TRACE("ev->canvas.x %d ", ev->canvas.x);

		double dval = current * max_vol / SLIDER_LENGTH(ad);
		val = current * max_vol / SLIDER_LENGTH(ad);

		SETTING_TRACE("dval = %lf, val = %d, dval-val=%lf ", dval, val,
				(dval - val));
		if ((dval - val) > 0.5)
			val = val + 1;
	}

	if (val < 0)
		val = 0;
	else if (val > max_vol)
		val = max_vol;

	SETTING_TRACE("val = %d", val);

	elm_slider_value_set(slider, val);

	/*for sliding performance */
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}
	ad->timer = ecore_timer_add(0.2, (Ecore_Task_Cb) __slide_timer,
			list_item);
	return;
}

static Eina_Bool __font_change_call(void *data)
{
	SETTING_TRACE_BEGIN;
	SettingFontUG *ad = (SettingFontUG *) data;

	/* logic3 */
	if (ad->size_change_flag == TRUE) {
		system_settings_set_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE,
				ad->ret_font_size);
	}

	if (ad->type_change_flag == TRUE) {
		/*	system_settings_set_value_string(
		 * SYSTEM_SETTINGS_KEY_FONT_TYPE, ad->font_name); */
		_event_set_font_type_helper(ad->font_name);
	}

	/* finalize */
	ad->font_type_timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

static void setting_font_done_click_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingFontUG *ad = (SettingFontUG *) data;

	SETTING_TRACE_DEBUG("ad->size_change_flag = %d", ad->size_change_flag);
	SETTING_TRACE_DEBUG("ad->type_change_flag = %d", ad->type_change_flag);

	if (ad->size_change_flag == TRUE || ad->type_change_flag == TRUE) {
		if (ad->main_popup) {
			evas_object_del(ad->main_popup);
			ad->main_popup = NULL;
		} /* font type popup */

		ad->font_change_status = SELECTED_FONT_CHANGE_IN_PROCESS;
		ad->main_popup = setting_create_popup_with_progressbar(ad,
				ad->win_get,
				PROGRESSBAR_STYLE,
				NULL, KeyStr_Loading,
				__setting_progress_popup_cb, 3/*0*/, TRUE, TRUE,
				0); /* 3 seconds to wait in maximum */

		/* ecore timer for change actual font */
		ad->font_type_timer = ecore_timer_add(1,
				(Ecore_Task_Cb) __font_change_call, ad);
	} else {
		/* Send destroy request */
		ug_destroy_me(ad->ug);
	}
}

static void setting_font_cancel_click_cb(void *data, Evas_Object *obj,
		void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	SettingFontUG *ad = (SettingFontUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
}

static void __setting_get_font_size_str(void *data, int size)
{
	SETTING_TRACE_BEGIN;

	ret_if(data == NULL);
	SettingFontUG *ad = (SettingFontUG *) data;

	switch (size) {
	case 0:
		ad->font_size_str = (char *) g_strdup("24");
		break;
	case 1:
		ad->font_size_str = (char *) g_strdup("30");
		break;
	case 2:
		ad->font_size_str = (char *) g_strdup("44");
		break;
	case 3:
		ad->font_size_str = (char *) g_strdup("56");
		break;
	case 4:
		ad->font_size_str = (char *) g_strdup("70");
		break;
	default:
		ad->font_size_str = (char *) g_strdup("30");
	}
}

static void __setting_get_font_type_str(void *data, char *font_data)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);
	ret_if(font_data == NULL);

	SettingFontUG *ad = (SettingFontUG *) data;

	char *pos = font_data;
	char new_name[FONT_BUF_SIZE];
	int i = 0;
	int count = 0;
	while (*pos != '\0') {
		if (*pos == ' ') {
			SETTING_TRACE(">>>	empty ' ' ");
			new_name[count] = '\\';
			count++;
			new_name[count] = ' ';
		} else {
			new_name[count] = font_data[i];
		}

		count++;
		pos++;
		i++;
	}
	new_name[count] = '\0';

	ad->font_type_str = g_strdup(new_name);
}

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
static int setting_font_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFontUG *ad = (SettingFontUG *) cb;

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1text_1icon_2));
	ad->itc_1text_1icon_2.func.text_get = _item_text_keystr2_get;

	setting_create_Gendial_itc(SETTING_GENLIST_LEFT_ICON_CONTENT_ICON_STYLE,
			&(ad->itc_bg_1icon));
	SETTING_TRACE("ad->itc_bg_1icon: %s ", ad->itc_bg_1icon);
	ad->itc_bg_1icon.func.content_get = _font_size_slider_get;

	setting_create_Gendial_itc(SETTING_GENLIST_ICON_1LINE_STYLE,
			&(ad->itc_1icon));
	ad->itc_1icon.func.content_get = _font_size_slider_get;

	retvm_if(ad->win_main_layout == NULL, SETTING_DRAW_ERR_FAIL_LOAD_EDJ,
			"win_main_layout is NULL");
	ad->font_change_status = SELECTED_FONT_CHANGE_NONE;
	ad->ly_main = setting_create_layout_navi_bar_genlist(
			ad->win_main_layout, ad->win_get, "IDS_ST_BODY_FONT",
			_("IDS_ST_BUTTON_BACK"),
			NULL,
			(setting_call_back_func) __setting_font_main_click_softkey_back_cb,
			NULL, ad, &ad->genlist, &ad->navibar);
	ad->navi_it_font = elm_naviframe_top_item_get(ad->navibar);

	/* Title Cancel Button */
	ad->btn_cancel = setting_create_button(ad->navibar,
			"IDS_MSG_ACBUTTON_CANCEL_ABB", "naviframe/title_cancel",
			setting_font_cancel_click_cb, ad);
	elm_object_item_part_content_set(ad->navi_it_font, "title_left_btn",
			ad->btn_cancel);
	/* Title Done Button */
	ad->btn_done = setting_create_button(ad->navibar,
			"IDS_MSG_ACBUTTON_DONE_ABB", "naviframe/title_done",
			setting_font_done_click_cb, ad);
	elm_object_item_part_content_set(ad->navi_it_font, "title_right_btn",
			ad->btn_done);

	elm_genlist_mode_set(ad->genlist, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(ad->genlist, "realized",
			__gl_realized_cb, ad);

	char *font_name = NULL;
	G_FREE(ad->font_type_str);
	int ret = system_settings_get_value_string(
			SYSTEM_SETTINGS_KEY_FONT_TYPE, &font_name);
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"Failed to call system_settings_get_value_string with error cdoe %d",
				ret);
		font_name = _get_default_font();
	}
	__setting_get_font_type_str(ad, font_name);
	int size = -1;
	ret = system_settings_get_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE,
			&size);
	setting_retvm_if(ret != 0, SETTING_RETURN_SUCCESS, "fail to get vconf");
	__setting_get_font_size_str(ad, size);

	SETTING_TRACE_DEBUG("ad->font_size_str = %s", ad->font_size_str);
	SETTING_TRACE_DEBUG("ad->font_type_str = %s", ad->font_type_str);

	/*1.Font Example */
	char *default_example_str = get_example_style_text(ad->font_size_str,
			ad->font_type_str);

	ad->font_example = setting_create_Gendial_field_def(ad->genlist,
			&(ad->itc_1icon),
			NULL, ad, SWALLOW_Type_INVALID, NULL,
			NULL, 0, NULL, default_example_str, NULL);

	if (ad->font_example) {
		__BACK_POINTER_SET(ad->font_example);
	} else {
		SETTING_TRACE_ERROR("ad->font_example is NULL");
	}
	G_FREE(default_example_str);

	/*2.Font Size..... */
	setting_create_Gendial_field_titleItem(ad->genlist, &(itc_group_item),
			"IDS_ST_BODY_SIZE", NULL);

	size = -1;
	ret = system_settings_get_value_int(SYSTEM_SETTINGS_KEY_FONT_SIZE,
			&size);
	setting_retvm_if(ret != 0, SETTING_RETURN_SUCCESS, "fail to get vconf");
	ad->init_font_size = size;
	ad->font_size = setting_create_Gendial_field_def(
			ad->genlist,
			&(ad->itc_bg_1icon),
			NULL,
			NULL,
			SWALLOW_Type_LAYOUT_5STEP_SLIDER,
			NULL,
			NULL,
			size,
			"IDS_ST_BODY_SIZE",
			NULL,
			__font_size_slider_value_change_cb);

	if (ad->font_size) {
		ad->font_size->win_main = ad->win_main_layout;
		ad->font_size->evas = ad->evas;
		ad->font_size->slider_min = 0;
		ad->font_size->slider_max = 4;
		ad->font_size->isIndicatorVisible = 0;
		ad->font_size->userdata = ad;
		ad->font_size->start_change_cb = NULL;
		ad->font_size->stop_change_cb = NULL;
		ad->font_size->mouse_up_cb = _slider_mouse_cb;
		__BACK_POINTER_SET(ad->font_size);
	} else {
		SETTING_TRACE_ERROR("ad->data_br_sli is NULL");
	}

	/*3.Font Type..... */
	setting_create_Gendial_field_titleItem(ad->genlist, &(itc_group_item),
			"IDS_ST_BODY_TYPE", NULL);
	Evas_Object *rgd = NULL;
	int i = 0;
	int matched_font = -1;
	G_FREE(ad->font_name);
	ret = system_settings_get_value_string(SYSTEM_SETTINGS_KEY_FONT_TYPE,
			&(ad->font_name));
	if (ret != SYSTEM_SETTINGS_ERROR_NONE) {
		SETTING_TRACE_ERROR(
				"Failed to call system_settings_get_value_string with error cdoe %d",
				ret);
		ad->font_name = _get_default_font();
	}

	rgd = elm_radio_add(ad->genlist);
	elm_radio_value_set(rgd, -1);

	char *default_font_name = NULL;
	default_font_name = _get_default_font();
	if (NULL != default_font_name) {
		/*char font_name_dispaly[FONT_BUF_SIZE] = {0,}; */
		SETTING_TRACE_DEBUG("default_font_name:%s", default_font_name);
		Setting_GenGroupItem_Data *item_data =
				(Setting_GenGroupItem_Data *) calloc(
				1, sizeof(Setting_GenGroupItem_Data));
		setting_retvm_if(!item_data, SETTING_RETURN_SUCCESS,
				"calloc failed");
		item_data->keyStr2 = (char *) g_strdup(
				SETTING_DEFAULT_FONT_TYPE);/*for display */
		/*for tts reading */
		item_data->keyStr = (char *) g_strdup(
				_(SETTING_DEFAULT_FONT_TYPE));
		item_data->swallow_type = SWALLOW_Type_1RADIO_RIGHT;
		item_data->chk_status = i;
		item_data->rgd = rgd;
		item_data->chk_change_cb = NULL;
		/*the real font type value */
		item_data->sub_desc = strdup(default_font_name);
		item_data->item = elm_genlist_item_append(ad->genlist,
				&(ad->itc_1text_1icon_2), item_data, NULL,
				ELM_GENLIST_ITEM_NONE,
				setting_font_main_list_sel_cb, ad);

		if (item_data) {
			item_data->userdata = ad;
			if (!safeStrCmp(ad->font_name,
					(const char *) item_data->sub_desc)) {
				matched_font = i;
			}
		}
		i++;
		ad->font_type_list = eina_list_append(ad->font_type_list,
				item_data);
	}

	Eina_List *font_list = NULL;
	Eina_List *l = NULL;
	char *font_data = NULL;
	font_list = __setting_font_main_available_list_get();
	EINA_LIST_FOREACH(font_list, l, font_data)
	{
		if (safeStrCmp(default_font_name, (char *) font_data) != 0) {
			char keyStr[FONT_BUF_SIZE] = { 0, };
			get_font_name_str((char *) font_data, keyStr,
					FONT_BUF_SIZE);
			char temp[FONT_BUF_SIZE] = { 0, };
			/*int ret = get_font_name_tag(font_data, temp,
			 * FONT_BUF_SIZE); */
			get_font_name_id((char *) font_data, temp,
					FONT_BUF_SIZE);
			SETTING_TRACE_DEBUG("font data : %s",
					(char * )font_data);
			Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *) calloc(
					1, sizeof(Setting_GenGroupItem_Data));
			setting_retvm_if(!item_data, SETTING_RETURN_SUCCESS,
					"calloc failed");
			/*for display */
			item_data->keyStr2 = (char *) g_strdup(temp);
			/*for tts reading */
			item_data->keyStr = (char *) g_strdup(keyStr);
			item_data->swallow_type = SWALLOW_Type_1RADIO_RIGHT;
			item_data->chk_status = i;
			item_data->rgd = rgd;
			item_data->chk_change_cb = NULL;
			item_data->sub_desc = strdup((char *) font_data);
			item_data->item = elm_genlist_item_append(ad->genlist,
					&(ad->itc_1text_1icon_2), item_data,
					NULL, ELM_GENLIST_ITEM_NONE,
					setting_font_main_list_sel_cb, ad);

			if (item_data) {
				item_data->userdata = ad;
				/*if (!safeStrCmp(ad->font_name,
				 * (const char*)font_data)) { */
				if (!safeStrCmp(ad->font_name,
						(const char *) item_data->sub_desc)) {
					matched_font = i;
				}
			}
			i++;
			ad->font_type_list = eina_list_append(
					ad->font_type_list, item_data);

			FREE(font_data);
		}
	}

	/* update check status */
	if (matched_font != -1) {
		elm_radio_value_set(rgd, matched_font);
		ad->init_font_type = matched_font;
	}

	ad->prev_font = matched_font;
	G_FREE(default_font_name);

	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME,
			__font_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}
	ret = vconf_notify_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE,
			__font_vconf_change_cb, ad);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_notify_key_changed failed");
	}
	/*__setting_font_main_genlist_item_create(ad); */

	evas_font_reinit();

	/*------------------------------------------------------------------- */
	evas_object_smart_callback_add(ad->win_get, "wm,rotation,changed",
			setting_font_rot_changed_cb, ad);

	setting_view_font_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_font_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFontUG *ad = (SettingFontUG *) cb;
	int ret = vconf_ignore_key_changed(
			VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME,
			__font_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}
	ret = vconf_ignore_key_changed(VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE,
			__font_vconf_change_cb);
	if (ret != 0) {
		SETTING_TRACE_ERROR("call vconf_ignore_key_changed failed");
	}
	if (ad->font_name) {
		G_FREE(ad->font_name);
		ad->font_name = NULL;
	}
	if (ad->font_size_str) {
		G_FREE(ad->font_size_str);
		ad->font_size_str = NULL;
	}
	if (ad->font_type_str) {
		G_FREE(ad->font_type_str);
		ad->font_type_str = NULL;
	}
	if (ad->timer) {
		ecore_timer_del(ad->timer);
		ad->timer = NULL;
	}
	if (ad->font_size_idler) {
		ecore_timer_del(ad->font_size_idler);
		ad->font_size_idler = NULL;
	}
	if (ad->font_type_timer) {
		ecore_timer_del(ad->font_type_timer);
		ad->font_type_timer = NULL;
	}

	if (ad->main_popup) {
		evas_object_del(ad->main_popup);
		ad->main_popup = NULL;
	}

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		ad->ly_main = NULL;
	}
	if (ad->navi_it_font != NULL) {
		ad->navi_it_font = NULL;
	}
	if (ad->font_type_list != NULL) {
		eina_list_free(ad->font_type_list);
		ad->font_type_list = NULL;
	}

	setting_view_font_main.is_create = 0;
	SETTING_TRACE_END;
	return SETTING_RETURN_SUCCESS;
}

static int setting_font_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFontUG *ad = (SettingFontUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_show(ad->ly_main);

		int value = -1;
		int err = -1;
		int ret = setting_get_int_slp_key(
				INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE, &value,
				&err);
		if (ret != 0) {
			SETTING_TRACE_ERROR("get vconf failed");
		}

		if (ad->font_size) {
			G_FREE(ad->font_size->sub_desc);
			ad->font_size->sub_desc = (char *) g_strdup(
					__get_font_size_str(value));
			elm_object_item_data_set(ad->font_size->item,
					ad->font_size);
			elm_genlist_item_update(ad->font_size->item);
		}
		/* setting_font_update_vconf_key(ad, SETTING_VCONF_INT_TYPE,
		 * INT_SLP_SETTING_LCD_TIMEOUT_NORMAL); */
	}
	if (ad->font_type) {
		elm_genlist_item_update(ad->font_type->item);
	}
	return SETTING_RETURN_SUCCESS;
}

static int setting_font_main_cleanup(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingFontUG *ad = (SettingFontUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_hide(ad->ly_main);
	}

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static Eina_Bool __setting_font_main_click_softkey_back_cb(void *data,
		Elm_Object_Item *it)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(data == NULL, EINA_FALSE);

	SettingFontUG *ad = (SettingFontUG *) data;

	if (ad->viewmode == FONT_SEL_VIEW_APPCONTROL) {
		app_control_h svc;
		if (app_control_create(&svc)) {
			return EINA_FALSE;
		}

		app_control_add_extra_data(svc, "category", "FontType");
		app_control_add_extra_data(svc, "FontType", ad->font_name);

		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "category",
				"FontType");
		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "FontType",
				ad->font_name);

		ug_send_result(ad->ug, svc);
		app_control_destroy(svc);
	} else if (ad->viewmode == FONT_SIZE_VIEW_APPCONTROL) {
		app_control_h svc;

		int err;
		int ret = setting_set_int_slp_key(
				INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE,
				ad->ret_font_size, &err);
		setting_retvm_if(ret == -1, EINA_FALSE, "Failed to set vconf");

		if (app_control_create(&svc)) {
			return EINA_FALSE;
		}

		char font_size[SETTING_FONT_MAX_LEN] = { 0, };
		snprintf(font_size, SETTING_FONT_MAX_LEN, "%d",
				ad->ret_font_size);

		app_control_add_extra_data(svc, "category", "FontSize");
		app_control_add_extra_data(svc, "FontSize", font_size);

		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "category",
				"FontSize");
		SETTING_TRACE(" SERVICE_ADD_EXTRA : %s %s", "FontSize",
				font_size);

		ug_send_result(ad->ug, svc);
		app_control_destroy(svc);
	}
	/* Send destroy request */
	ug_destroy_me(ad->ug);
	return EINA_FALSE;
}
