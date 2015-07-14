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
/**
 *default group setting-common-draw-widget
 *common UI code for reuse
 */
#ifndef __SETTING_COMMON_DRAW_WIDGET_H__
#define __SETTING_COMMON_DRAW_WIDGET_H__
#include <setting-common-data-type.h>	/* including all the data structure */
#include <setting-common-general-func.h>	/* including one class of common functions */
#include <setting-common-data-slp-setting.h>	/* including another class of common functions */

#include <unicode/ucal.h>
#include <unicode/uloc.h>
#include <unicode/udat.h>
#include <unicode/ustring.h>
#include <unicode/udatpg.h>
#include <unicode/utmscale.h>
#include <unicode/ucol.h>



#define EXPORT_PUBLIC __attribute__ ((visibility ("default")))

#define SETTING_COMMON_DRAW_WIDGET_CONTROLBAR_ITEM_LEN 3
#define POPUP_INTERVAL 2.0

extern const Elm_Genlist_Item_Class itc_seperator;
extern const Elm_Genlist_Item_Class itc_bottom_seperator;
extern const Elm_Genlist_Item_Class itc_sep_line;

extern const Elm_Genlist_Item_Class itc_1text;
extern const Elm_Genlist_Item_Class itc_1text_1icon_2;
extern const Elm_Genlist_Item_Class itc_1text_1icon_2_tb;

extern const Elm_Genlist_Item_Class itc_dialogue_editfield_title;
extern const Elm_Genlist_Item_Class itc_1icon;
extern const Elm_Genlist_Item_Class itc_1icon_1text_sub;
extern const Elm_Genlist_Item_Class itc_1text_1icon;
extern const Elm_Genlist_Item_Class itc_1text_1icon_3;
extern const Elm_Genlist_Item_Class itc_1text_2icon;
extern const Elm_Genlist_Item_Class itc_1text_2icon_2;
extern const Elm_Genlist_Item_Class itc_1text_2icon_10;
extern const Elm_Genlist_Item_Class itc_1text_parent;

extern const Elm_Genlist_Item_Class itc_2text_2;
extern const Elm_Genlist_Item_Class itc_2text;
extern const Elm_Genlist_Item_Class itc_multiline_2text_1icon;
extern const Elm_Genlist_Item_Class itc_2text_1icon_3;
extern const Elm_Genlist_Item_Class itc_2text_1icon_2;
extern const Elm_Genlist_Item_Class itc_2text_1icon_6;
extern const Elm_Genlist_Item_Class itc_2text_1icon_7;
extern const Elm_Genlist_Item_Class itc_2text_1icon_10;

extern const Elm_Genlist_Item_Class itc_2text_3_parent;
extern const Elm_Genlist_Item_Class itc_2text_3;
extern const Elm_Genlist_Item_Class itc_2text_2icon_3;

extern const Elm_Genlist_Item_Class itc_multiline_text;
extern const Elm_Genlist_Item_Class itc_1text_1icon_3_tb;
extern const Elm_Genlist_Item_Class itc_cm_1text_1icon_2;
extern const Elm_Genlist_Item_Class itc_sel_all;
extern const Elm_Genlist_Item_Class itc_bg_1icon;
extern const Elm_Genlist_Item_Class itc_1icon_with_no_line;
extern const Elm_Genlist_Item_Class itc_1icon_with_no_padding;
extern const Elm_Genlist_Item_Class itc_group_item;
extern const Elm_Genlist_Item_Class itc_multiline_1text_1icon;
extern const Elm_Genlist_Item_Class itc_1text_2icon_divider;
extern const Elm_Genlist_Item_Class itc_no_group_1text_1icon_2;

extern const Elm_Genlist_Item_Class itc_editfield;
extern const Elm_Genlist_Item_Class itc_multiline_2text;
extern const Elm_Genlist_Item_Class itc_1text_1icon_divider;
extern const Elm_Genlist_Item_Class itc_normal_1text;
extern const Elm_Genlist_Item_Class itc_bottom_line;


#define ADD_GL_SEPARATOR(scroller) \
	{\
		Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_seperator), NULL, NULL,\
		                                                ELM_GENLIST_ITEM_NONE, NULL, NULL);\
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
	}

#define ADD_GL_SEPARATOR_2(scroller) \
	{\
		Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_sep_line), NULL, NULL,\
		                                                ELM_GENLIST_ITEM_NONE, NULL, NULL);\
		elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
	}

#define ADD_GL_LABLE(scroller, str) \
	{\
		if (str) {\
			Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_bottom_seperator), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL);\
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &itc_multiline_text,\
			                                                                        NULL,\
			                                                                        NULL,\
			                                                                        SWALLOW_Type_LAYOUT_SPECIALIZTION,\
			                                                                        NULL, NULL, 0, str, NULL, NULL);\
			setting_retvm_if(NULL == item_data, NULL, "item_data is NULL");\
			elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		}\
	}

#define ADD_GL_HELP(scroller, str) \
	{\
		if (str) {\
			Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_sep_line), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL);\
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &itc_multiline_text,\
			                                                                        NULL,\
			                                                                        NULL,\
			                                                                        SWALLOW_Type_LAYOUT_SPECIALIZTION_X,\
			                                                                        NULL, NULL, 0, str, NULL, NULL);\
			setting_retvm_if(NULL == item_data, NULL, "item_data is NULL");\
			elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		}\
	}

#define ADD_GL_HELP_RET(scroller, str, item_data) \
	{\
		if (str) {\
			Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_sep_line), NULL, NULL,ELM_GENLIST_ITEM_NONE, NULL, NULL);\
			elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
			item_data = setting_create_Gendial_field_def(scroller, &itc_multiline_text,\
			                                             NULL,\
			                                             NULL,\
			                                             SWALLOW_Type_LAYOUT_SPECIALIZTION_X,\
			                                             NULL, NULL, 0, str, NULL, NULL);\
			setting_retm_if(NULL == item_data, "item_data is NULL");\
			elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		}\
	}

#define ADD_GL_HELP_NO_SEP(scroller, str) \
	{\
		if (str) {\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &itc_multiline_text,\
			                                                                        NULL,\
			                                                                        NULL,\
			                                                                        SWALLOW_Type_LAYOUT_SPECIALIZTION_X,\
			                                                                        NULL, NULL, 0, str, NULL, NULL);\
			setting_retvm_if(NULL == item_data, NULL, "item_data is NULL");\
			elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		}\
	}

#define ADD_GL_BUTTON(scroller, btn_str, btn_cb, cb_data) \
	{\
		if (btn_str) {\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &(itc_1icon_with_no_line),\
			                                                                        NULL,cb_data, SWALLOW_Type_1BUTTON,\
			                                                                        NULL, NULL, 0, btn_str, NULL,\
			                                                                        btn_cb);\
			\
			item_data->userdata = cb_data;\
		}\
	}

#define ADD_GL_GIF(scroller, png_list) \
	{\
		if (png_list && *png_list) {\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &(itc_1icon_with_no_line),\
			                                                                        NULL,NULL, SWALLOW_Type_1GIF,\
			                                                                        NULL, NULL, 0, NULL, NULL,\
			                                                                        NULL);\
			item_data->belongs_to = (int)png_list;\
			if (item_data)\
				elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
		}\
	}
#define ADD_GL_GIF_2(scroller, png_array) \
	{\
		Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(scroller, &(itc_1icon_with_no_line),\
		                                                                        NULL,NULL, SWALLOW_Type_1GIF,\
		                                                                        NULL, NULL, 0, NULL, NULL,\
		                                                                        NULL);\
		item_data->belongs_to = (int)png_array;\
		elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);\
	}

#define ADD_GL_RADIO(scroller, rdg, radio_idx, keyStr, sel_cb, cb_data) \
	{\
		if(!rdg)\
		{\
			rdg = elm_radio_add(scroller);\
			elm_radio_state_value_set(rdg, -1);\
		}\
		if (keyStr && rdg)\
		{\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_1radio(scroller, &(itc_1text_1icon_2),\
			                                                                           sel_cb,\
			                                                                           cb_data,\
			                                                                           SWALLOW_Type_1RADIO,\
			                                                                           rdg, radio_idx,\
			                                                                           keyStr,\
			                                                                           NULL);\
			if (item_data) {\
				item_data->userdata = cb_data;\
			}\
		}\
	}

#define ADD_GL_RADIO_GROUP_STYLE(scroller, rdg, radio_idx, keyStr, sel_cb, cb_data, style) \
	{\
		if(!rdg)\
		{\
			rdg = elm_radio_add(scroller);\
			elm_radio_state_value_set(rdg, -1);\
		}\
		if (keyStr && rdg)\
		{\
			Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_1radio(scroller, &(itc_1text_1icon_2),\
			                                                                           sel_cb,\
			                                                                           cb_data,\
			                                                                           SWALLOW_Type_1RADIO,\
			                                                                           rdg, radio_idx,\
			                                                                           keyStr,\
			                                                                           NULL);\
			if (item_data) {\
				item_data->userdata = cb_data;\
				item_data->group_style = style;\
			}\
		}\
	}

void __toogle_gl_sel(void *data, Evas_Object *obj, void *event_info);
void __toogle_chk_changed(void *data, Evas_Object *obj, void *event_info);

#define ADD_GL_TOOGLE(scroller, use_vconf, keyStr, isel_cb, cb_data, ichk_cb, item_data) \
	{\
		if (keyStr)\
		{\
			int value = 1;\
			vconf_get_bool(use_vconf, &value);\
			setting_call_back_func sel_cb = isel_cb;\
			setting_call_back_func chk_cb = ichk_cb;\
			if (!sel_cb) sel_cb = __toogle_gl_sel;\
			if (!chk_cb) chk_cb = __toogle_chk_changed;\
			\
			item_data = setting_create_Gendial_field_def(scroller,\
			                                             &itc_1text_2icon,\
			                                             sel_cb,\
			                                             cb_data,\
			                                             SWALLOW_Type_1ICON_1RADIO,\
			                                             NULL, NULL,\
			                                             value,\
			                                             keyStr,\
			                                             NULL,\
			                                             chk_cb);\
			if (item_data) {\
				item_data->userdata = cb_data;\
				item_data->int_slp_setting_binded = (int)use_vconf;\
			}\
		}\
	}

#define _P(pEvasObj) do { \
		evas_object_event_callback_add(pEvasObj, EVAS_CALLBACK_DEL, setting_protect_cb, &(pEvasObj));\
	}while(0);

/**
 * Do process when clicking radio in subitem of expandable item
 */
extern void setting_sub_list_rd_change(void *data, Evas_Object *obj,
                                       void *event_info);

/**
 * Hide the input pannel
 */
extern void setting_hide_input_pannel_cb(Evas_Object *entry);

/**
 * Do process when clicking on subitem of expandable item
 */
extern void setting_sub_list_sel_cb(void *data, Evas_Object *obj,
                                    void *event_info);

/**
 * Do process when clicking on a common genlist item
 */
extern void setting_mouse_up_Gendial_list_radio_cb(void *data,
                                                   Evas_Object *obj,
                                                   void *event_info);

/**
 * To intialize an Elm_Genlist_Item_Class, according to item style
 */
extern bool setting_create_Gendial_itc(const char *item_style,
                                       Elm_Genlist_Item_Class *itc);

/**
 * Create group style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_group_titleItem(Evas_Object *genlist,
                                                                               const Elm_Genlist_Item_Class *itc,
                                                                               Elm_Object_Item *parent,
                                                                               Elm_Genlist_Item_Type flag,	/* ELM_GENLIST_ITEM_GROUP or ELM_GENLIST_ITEM_NONE */
                                                                               const char *keyStr,
                                                                               setting_call_back_func gl_sel);

/**
 * Create separator style item with title
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_titleItem(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    const char *keyStr,
    setting_call_back_func gl_sel);

/**
 * Create 1radion-1text style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func chk_change_cb);

/**
 * Create 1radio-1button style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio_1button(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *button_style,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func chk_change_cb,
    setting_call_back_func btn_clicked_cb);
/**
 * Create 2radion style item
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_2radio(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    const char *sub_desc,
    setting_call_back_func chk_change_cb);

extern Setting_GenGroupItem_Data *setting_create_Gendial_field_1radio_2text(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    char *sub_desc,
    setting_call_back_func chk_change_cb);

/**
 * Create common style item with menu icon
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_groupitem(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    Elm_Object_Item *parent,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *l_icon_path,
    char *r_icon_path, int chk_status,
    const char *keyStr, char *sub_desc,
    setting_call_back_func
    chk_change_cb);

extern Setting_GenGroupItem_Data *setting_create_Gendial_field_helpitem_without_bottom_separator(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    SWALLOW_Type swallow_type,
    const char *keyStr);

extern Setting_GenGroupItem_Data *setting_create_Gendial_field_insert_help_without_bottom_separator(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    Elm_Object_Item *after_it,
    SWALLOW_Type swallow_type,
    const char *keyStr);

/**
 * Common function to create a genlist item
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_def(Evas_Object *
                                                                   genlist,
                                                                   const Elm_Genlist_Item_Class *itc,
                                                                   setting_call_back_func gl_sel,
                                                                   void *sel_data,
                                                                   SWALLOW_Type swallow_type,
                                                                   char *l_icon_path,
                                                                   char *r_icon_path,
                                                                   int chk_status,
                                                                   const char *keyStr,
                                                                   char *sub_desc,
                                                                   setting_call_back_func chk_change_cb);

/**
 * Create 1entry style item - for focusing out
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_entry_fo(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    char *l_icon_path,
    char *r_icon_path,
    int chk_status,
    const char *keyStr,
    char *sub_desc,
    setting_call_back_func chk_change_cb,
    setting_call_back_func chk_focus_out_cb,
    Elm_Input_Panel_Layout input_type,
    bool isPasswordFlag,
    bool isFocusFlag,
    int min_char_count,
    int max_char_count,
    int max_byte_count,
    char *accepted,
    char *rejected);

/**
 * Create 1entry style item
 * @return a pointer to Setting_GenGroupItem_Data
 * @todo duplicated with setting_create_Gendial_field_entry_fo
 *       merge it
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_field_entry(Evas_Object *genlist,
                                                                     const Elm_Genlist_Item_Class *itc,
                                                                     setting_call_back_func gl_sel,
                                                                     void *sel_data,
                                                                     SWALLOW_Type swallow_type,
                                                                     char *l_icon_path,
                                                                     char *r_icon_path,
                                                                     int chk_status,
                                                                     const char *keyStr,
                                                                     char *sub_desc,
                                                                     setting_call_back_func chk_change_cb,
                                                                     Elm_Input_Panel_Layout input_type,
                                                                     bool isPasswordFlag,
                                                                     bool isFocusFlag,
                                                                     int max_char_count,
                                                                     int max_byte_count,
                                                                     char *accepted,
                                                                     char *rejected);

extern Setting_GenGroupItem_Data *setting_create_Gendial_field_entry_with_return_key(Evas_Object *
        genlist,
        const Elm_Genlist_Item_Class *itc,
        setting_call_back_func gl_sel,
        void *sel_data,
        SWALLOW_Type
        swallow_type,
        char *l_icon_path,
        char *r_icon_path,
        int chk_status,
        const char *keyStr,
        char *sub_desc,
        setting_call_back_func chk_change_cb,
        setting_call_back_func max_reach_cb,
        Elm_Input_Panel_Layout input_type,
        bool isPasswordFlag,
        bool isFocusFlag,
        int max_char_count,
        int max_byte_count,
        char *accepted,
        char *rejected,
        Elm_Input_Panel_Return_Key_Type return_key_type,
        setting_call_back_func activated_cb,
        setting_call_back_func x_callback_cb);

extern Setting_GenGroupItem_Data *setting_create_Gendial_field_editfield(Evas_Object *
                                                                         genlist,
                                                                         const Elm_Genlist_Item_Class *itc,
                                                                         setting_call_back_func gl_sel,
                                                                         void *sel_data,
                                                                         SWALLOW_Type
                                                                         swallow_type,
                                                                         const char *keyStr,
                                                                         char *sub_desc,
                                                                         setting_call_back_func
                                                                         chk_change_cb,
                                                                         setting_call_back_func
                                                                         focused_cb,
                                                                         setting_call_back_func
                                                                         unfocused_cb,
                                                                         setting_call_back_func
                                                                         activated_cb,
                                                                         setting_call_back_func
                                                                         max_reached_cb,
                                                                         Elm_Input_Panel_Layout
                                                                         input_type,
                                                                         bool isPasswordFlag,
                                                                         bool isFocusFlag,
                                                                         int max_char_count,
                                                                         int max_byte_count,
                                                                         char *accepted,
                                                                         char *rejected);

/**
 * General function to create a subitem of expandable item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_exp_sub_field(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    Elm_Object_Item *parent,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func
    chk_change_cb);

/**
 * General function to create a subitem of expandable item with group style
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_exp_sub_field_with_group_style(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    Elm_Object_Item *parent,
    SWALLOW_Type swallow_type,
    Evas_Object *rgd, int chk_status,
    const char *keyStr,
    setting_call_back_func
    chk_change_cb,
    setting_group_style group_style);

/**
 * General function to create a parent item of expandable item
 *
 * @return a pointer to Setting_GenGroupItem_Data
 */
extern Setting_GenGroupItem_Data *setting_create_Gendial_exp_parent_field(
    Evas_Object *genlist,
    const Elm_Genlist_Item_Class *itc,
    setting_call_back_func gl_sel,
    void *sel_data,
    SWALLOW_Type swallow_type,
    const char *keyStr,
    char *sub_desc,
    setting_group_style exp_style,
    setting_group_style con_style);

/**
 * To make genlist support expandable style item
 */
extern void setting_enable_expandable_genlist(Evas_Object *genlist, void *data,
                                              setting_call_back_func gl_exp_cb,
                                              setting_call_back_func
                                              gl_smart_cb);

/**
* The API to let an item which with processbar go into "processing"
* @return
*/
extern void setting_begin_progress_genlist_item(Setting_GenGroupItem_Data *
                                                data_item);
/**
* The API to let an item which with processbar go into "processed"
* @return
*/
extern void setting_finish_progress_genlist_item(Setting_GenGroupItem_Data *
                                                 data_item);

/**
 * To create slider object of a genlist item
 * @return a slider container object
 */
extern Evas_Object *setting_create_slider(Evas_Object *win_main, Evas *evas,
                                          const char *l_swallow_path,
                                          const char *r_swallow_path,
                                          double value,
                                          bool indicator,
                                          double slider_min,
                                          double slider_max,
                                          setting_call_back_func
                                          slider_change_cb,
                                          setting_call_back_func
                                          slider_start_change_cb,
                                          setting_call_back_func
                                          slider_stop_change_cb, void *cb_data);

/**
 * To create slider object of a genlist item
 * @return a slider container object
 */
extern Evas_Object *setting_create_5step_slider(Evas_Object *win_main, Evas *evas,
                                                const char *l_swallow_path,
                                                const char *r_swallow_path,
                                                double value,
                                                bool indicator,
                                                double slider_min,
                                                double slider_max,
                                                setting_call_back_func
                                                slider_change_cb,
                                                setting_call_back_func
                                                slider_start_change_cb,
                                                setting_call_back_func
                                                slider_stop_change_cb, void *cb_data);

/**
 * The main implement body of create a certain size icon
 */
extern void create_image_box_add_ex(void *data, Evas_Object *win_main,
                                    Evas *evas, char *img_path, int img_w,
                                    int img_h,
                                    Evas_Object_Event_Cb mouse_down_cb,
                                    Evas_Object_Event_Cb mouse_up_cb,
                                    Evas_Object **image_box,
                                    Evas_Object **img);

/**
 * To create a size fixed icon
 * @return a image container object
 */
extern Evas_Object *create_image_box_add(void *data, Evas_Object *win_main,
                                         Evas *evas, char *img_path, int img_w,
                                         int img_h,
                                         Evas_Object_Event_Cb mouse_down_cb,
                                         Evas_Object_Event_Cb mouse_up_cb);

/**
* @ To create a size specialized icon
* @return a image container object
*/
extern Evas_Object *create_bgimage_box_add(void *data, Evas_Object *win_main,
                                           Evas *evas, char *img_path,
                                           int img_w, int img_h,
                                           Evas_Object_Event_Cb mouse_down_cb,
                                           Evas_Object_Event_Cb mouse_up_cb);

/**
* @ create a box container
*
* @param[in] win_main
*
* @return a box container
*/
extern Evas_Object *setting_create_client_bx(Evas_Object *win_main);

/**
* The API to Create a lable
* @return a lable
*/
extern Evas_Object *setting_create_lable(Evas_Object *parent, const char *text, const char *style, const char *align);

/**
* The API to Create an object to show an image
* @return an object to show an image
*/
extern Evas_Object *setting_create_image(Evas_Object *parent, const char *img_path);

/**
* The API to Create an object to show an image with round corner
* @return an object to show an image
*/
extern Evas_Object *setting_create_image_with_round_corner(Evas *evas, const char *img_path, int w, int h);

/**
* The API to Create an object to show anamition image
* @return an object to show anamition image
*/
extern Evas_Object *setting_create_gif(Evas_Object *parent, const char **png_list);

extern void setting_add_hardkey_features(Evas_Object *popup, void *data);
extern void ea_popup_back_cb_2(void *data, Evas_Object *obj, void *event_info);
/**
* @ create a popup window which contents a progressbar
* @return a popup window which contents a progressbar
*/
extern Evas_Object *setting_create_popup_with_progressbar(void *data,
                                                          Evas_Object *parent,
                                                          char *progressbar_style,
                                                          char *title,
                                                          char *text,
                                                          setting_call_back_func response_cb,
                                                          int timeout,
                                                          bool blocked_flag,
                                                          bool keygrab_flag);
extern void setting_popup_lang_update(Evas_Object *popup);
extern void popup_handle_hardkey(void *data, Evas_Object *popup);
/**
* @ create a popup window which contents a progressbar and button
* @return a popup window which contents a progressbar and button
*/
extern Evas_Object *setting_create_popup_with_progressbar_btn(void *data,
                                                              Evas_Object *parent,
                                                              char *progressbar_style,
                                                              char *title,
                                                              char *text,
                                                              setting_call_back_func response_cb,
                                                              int timeout,
                                                              int btn_num, ...);

/**
* The general API to create a certain popup window with more than one button.
* @return a certain popup window
*/
extern Evas_Object *setting_create_popup_with_btn(void *data,
                                                  Evas_Object *parent,
                                                  char *title,
                                                  char *text,
                                                  setting_call_back_func response_cb,
                                                  int timeout,
                                                  int btn_num, ...);
/**
* The general API to create a certain popup window
* @return a certain popup window
*/
extern Evas_Object *setting_create_popup_without_btn(void *data,
                                                     Evas_Object *parent,
                                                     char *title,
                                                     char *text,
                                                     setting_call_back_func response_cb,
                                                     int timeout,
                                                     bool blocked_flag,
                                                     bool keygrab_flag);

extern Evas_Object *setting_create_popup_with_progressbar_withobject(void *data,
                                                                     Evas_Object *popup,
                                                                     Evas_Object *parent,
                                                                     char *progressbar_style,
                                                                     char *title,
                                                                     char *text,
                                                                     setting_call_back_func response_cb,
                                                                     int timeout,
                                                                     bool blocked_flag,
                                                                     bool keygrab_flag);

/**
* The API to create a certain popup window with title
* @return a certain popup window with title
*/
extern int setting_create_simple_popup(void *data,
                                       Evas_Object *parent,
                                       char *title,
                                       char *text);
/**
* The API to create a certain popup window with a gif and some buttons
* @return a certain popup window with title
*/
extern Evas_Object *setting_create_popup_with_gif(void *data,
                                                  Evas_Object *parent,
                                                  char *title,
                                                  char *text,
                                                  const char **png_list,/*array end with 'NULL'. */
                                                  setting_call_back_func response_cb,
                                                  int timeout,
                                                  int btn_num, ...);

/**
* The API to create a certain popup window with a check
* @return a certain popup window with title
*/
extern Evas_Object *setting_create_popup_with_checkview(Evas_Object *parent,
                                                        char *title,
                                                        char *check_str,
                                                        Evas_Object *checkview_content,
                                                        const char *checkview_style,
                                                        Evas_Object **check,
                                                        int timeout);
extern Evas_Object *setting_create_popup_with_label_and_reverse_btn(void *data,
                                                                    Evas_Object *parent,
                                                                    char *title,
                                                                    char *text,
                                                                    setting_call_back_func response_cb,
                                                                    int timeout,
                                                                    int btn_num, ...);

extern Evas_Object *setting_create_popup_with_label_check(void *data,
                                                          Evas_Object *parent,
                                                          char *title,
                                                          char *content,
                                                          char *check_str,
                                                          Evas_Object **check,
                                                          setting_call_back_func response_cb,
                                                          int timeout,
                                                          int btn_num, ...);

/**
* The API to create a certain popup window with a image, a check and some buttons
* @return a certain popup window with title
*/
extern Evas_Object *setting_create_popup_with_image_check(void *data,
                                                          Evas_Object *parent,
                                                          char *title,
                                                          char *text1,
                                                          char *text2,
                                                          const char **png_list,
                                                          char *check_str,
                                                          Evas_Object **check,
                                                          setting_call_back_func response_cb,
                                                          int timeout,
                                                          int btn_num, ...);

extern Evas_Object *setting_create_popup_with_list(Evas_Object **genlist, void *data, Evas_Object *parent, const char *title,
                                                   setting_call_back_func response_cb, int timeout,
                                                   int btn_num, ...);
/**
* Alternate the check object status and its binded vcof value via original vcofn value
*/
extern int setting_draw_onoff_status(Evas_Object *obj,
                                     setting_bool_slp_list key);

/**
* Set the vconf value via its binded check object.
* @return 0: sucess, other: failed
*/
extern int setting_reset_slp_key_by_status(Evas_Object *obj,
                                           setting_bool_slp_list key);

/**
* Set the status of check object via its binded vconf value.
* @return  0: sucess, other: failed
*/
extern int setting_check_onoff_status(Evas_Object *obj,
                                      setting_bool_slp_list key);

/**
* Set the status of radio group object via its binded vconf value.
* @return  0: sucess, other: failed
*/
extern int setting_update_chk_status(Evas_Object *chk,
                                     setting_int_slp_list type);

/**
* Create a size specilized rectangle object
* @return a size specilized rectangle object
*/
extern Evas_Object *setting_create_blank_rect_customize(Evas_Object *layout,
                                                        int w, int h);

/**
* Create a size fixed rectangle object
* @param[in] layout
* @return a size fixed rectangle object
*/
extern Evas_Object *setting_create_blank_rect(Evas_Object *layout);

/**
* The API to Create an editfiled object
* @return an editfiled object
*/
extern Evas_Object *setting_create_editfiled(Evas_Object *win_main,
                                             char *title, char *def_str, char *guide_text);
/**
* The API to Create a standard layout
* @return a standard layout
*/
extern Evas_Object *setting_create_win_layout(Evas_Object *win_layout,
                                              Evas_Object *win_obj);
extern void setting_create_more_btn_click_cb(void *data, Evas_Object *obj, void *event_info);
extern void setting_create_more_btn_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
/**
* The API to Create a naviframe
* @return a naviframe
*/
extern Evas_Object *setting_create_navi_bar(Evas_Object *layout);

extern Elm_Object_Item *setting_create_guild_layout(Evas_Object *navi_bar,
                                                    char *title_str,
                                                    char *lbutton_str,
                                                    char *rbutton_str,
                                                    char *mbutton_str,
                                                    setting_call_back_func lbutton_click_cb,
                                                    setting_call_back_func rbutton_click_cb,
                                                    setting_call_back_func mbutton_click_cb,
                                                    char *content_str1,
                                                    char **png_list,
                                                    char *content_str2,
                                                    char *content_button_str,
                                                    setting_call_back_func content_button_click_cb,
                                                    void *cb_data);
extern void setting_navi_items_update(Evas_Object *navigate_bar);

extern Elm_Object_Item *setting_create_guild_layout2(Evas_Object *navi_bar,
                                                     char *title_str,
                                                     char *lbutton_str,
                                                     char *rbutton_str,
                                                     char *mbutton_str,
                                                     setting_call_back_func lbutton_click_cb,
                                                     setting_call_back_func rbutton_click_cb,
                                                     setting_call_back_func mbutton_click_cb,
                                                     char *content_str1,
                                                     Evas_Object **ug_layout,
                                                     char *content_str2,
                                                     char *content_button_str,
                                                     setting_call_back_func content_button_click_cb,
                                                     void *cb_data);
/**
* The API to Create a button
* @return a button
*/
extern Evas_Object *setting_create_button(Evas_Object *parent, const char *btn_str,
                                          const char *btn_style,
                                          setting_call_back_func btn_click_cb,
                                          void *cb_data);

extern void setting_nf_check_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
extern void setting_nf_check_change_cb(void *data, Evas_Object *obj, void *event_info);
extern void setting_nf_check_vconf_cb(keynode_t *key, void *data);
#define ADD_NAVIFRAME_CHECK(parent, navi_it, vconf) \
	{\
		Evas_Object *check = elm_check_add(parent);\
		int value = 0;\
		vconf_get_bool(vconf, &value);\
		elm_check_state_set(check, value);\
		elm_object_style_set(check, "on&off");\
		elm_object_item_part_content_set(navi_it, "title_right_btn", check);\
		(void)vconf_notify_key_changed(vconf, setting_nf_check_vconf_cb, check);\
		evas_object_smart_callback_add(check, "changed", setting_nf_check_change_cb, vconf);\
		evas_object_event_callback_add(check, EVAS_CALLBACK_DEL, setting_nf_check_del_cb, vconf);\
	}

/**
* The API to Create title buttons of naviframe
* if sip is shown,the buttons created by this API will be shown,it sip is hidden,they will be hiden too
* @return non
*/
extern void setting_append_naviframe_title_buttons(Elm_Object_Item *navi_it,
                                                   Evas_Object *navigate_bar,
                                                   char *more_button_str,
                                                   char *left_button_str,
                                                   char *right_button_str,
                                                   char *back_button_str,
                                                   setting_call_back_func more_button_click_cb,
                                                   setting_call_back_func left_button_click_cb,
                                                   setting_call_back_func right_button_click_cb,
                                                   setting_call_back_func back_button_click_cb,
                                                   void *cb_data);
/**
* The API to Create top buttons onto naviframe
* @return a naviframe item
*/
extern Elm_Object_Item *setting_create_navi_bar_top_buttons(char *title_str,
                                                            char *lbutton_str,
                                                            char *rbutton_str,
                                                            char *mbutton_str,
                                                            setting_call_back_func lbutton_click_cb,
                                                            setting_call_back_func rbutton_click_cb,
                                                            setting_call_back_func mbutton_click_cb,
                                                            void *cb_data, Evas_Object *eo_view,/*any container obj */
                                                            Evas_Object *navigate_bar);

/**
* Create buttons on the specialized navigation bar
* @param[in] eo_view	navigation bar's content
* @param[in] controlbar   title obj of navigation bar
*/
extern void
setting_create_navi_bar_buttons(char *title_str,
                                char *lbutton_str, char *rbutton_str,
                                char *mbutton_str,
                                setting_call_back_func lbutton_click_cb,
                                setting_call_back_func rbutton_click_cb,
                                setting_call_back_func mbutton_click_cb,
                                void *cb_data, Evas_Object *eo_view,
                                Evas_Object *navigate_bar,
                                Evas_Object *titleobj);

/**
* The general API to create a layout with navigation bar,
*   which contents any evas container object(@param[eo_view]) as its content
*
* @param[in] eo_view	navigation bar's content
* @param[out] titleobj   title obj of navigation bar
*
* return main layout of UG or App..
*/

Evas_Object *setting_create_layout_navi_bar(Evas_Object *win_layout,
                                            Evas_Object *win_obj,
                                            char *title_str,
                                            char *lbutton_str,
                                            char *rbutton_str,
                                            char *mbutton_str,
                                            setting_call_back_func lbutton_click_cb,
                                            setting_call_back_func rbutton_click_cb,
                                            setting_call_back_func mbutton_click_cb,
                                            void *cb_data,
                                            Evas_Object *eo_view,
                                            Evas_Object **navi_bar,
                                            Evas_Object **titleobj);

/**
* The API to create a layout with navigation bar,
*   which contents a scroller object as its content
*
* @param[out] scroller   navigation bar's content
*
* @return a layout with a special navigation bar and at most 2 buttons
*/
extern Evas_Object *setting_create_layout_navi_bar_scroller(Evas_Object *win_layout,
                                                            Evas_Object *win_obj,
                                                            char *title_str,
                                                            char *lbutton_str,
                                                            char *rbutton_str,
                                                            setting_call_back_func lbutton_click_cb,
                                                            setting_call_back_func rbutton_click_cb,
                                                            void *cb_data,
                                                            Evas_Object **scroller,
                                                            Evas_Object **navi_bar);

/**
* The API to create a layout with navigation bar,
*	which contents a genlist object as its content
*
* @return a layout with a special navigation bar and at most 2 buttons
*/
extern Evas_Object *setting_create_layout_navi_bar_genlist(Evas_Object *win_layout,
                                                           Evas_Object *win_obj,
                                                           char *title_str,
                                                           char *lbutton_str,
                                                           char *rbutton_str,
                                                           setting_call_back_func lbutton_click_cb,
                                                           setting_call_back_func rbutton_click_cb,
                                                           void *cb_data,
                                                           Evas_Object **genlist,
                                                           Evas_Object **navi_bar);

/**
* The general API to push any evas object(@param[in][eo_view]) to the specialized navi_bar,
*   which contents at most 3 buttons
*/
extern Elm_Object_Item *setting_push_layout_navi_bar(char *title_str,
                                                     char *lbutton_str, char *rbutton_str,
                                                     char *mbutton_str,
                                                     setting_call_back_func lbutton_click_cb,
                                                     setting_call_back_func rbutton_click_cb,
                                                     setting_call_back_func mbutton_click_cb,
                                                     void *cb_data,
                                                     Evas_Object *eo_view,
                                                     Evas_Object *navi_bar,
                                                     Evas_Object **titleobj);

/**
* The API to push a scroller object(@param[in][scroller]) to the specialized navi_bar,
*   which contents at most 2 buttons
*/
extern Elm_Object_Item *setting_push_layout_navi_bar_scroller(Evas_Object *win_main,
                                                              char *title_str,
                                                              char *lbutton_str,
                                                              char *rbutton_str,
                                                              setting_call_back_func lbutton_click_cb,
                                                              setting_call_back_func rbutton_click_cb,
                                                              void *cb_data,
                                                              Evas_Object **scroller,
                                                              Evas_Object *navi_bar);

/**
* The API to push a genlist object(@param[in][genlist]) to the specialized navi_bar,
*   which contents at most 2 buttons
*/
extern Elm_Object_Item *
setting_push_layout_navi_bar_genlist(Evas_Object *win_layout,
                                     Evas_Object *win_obj,
                                     char *title_str,
                                     char *lbutton_str,
                                     char *rbutton_str,
                                     setting_call_back_func lbutton_click_cb,
                                     setting_call_back_func rbutton_click_cb,
                                     void *cb_data,
                                     Evas_Object **genlist,
                                     Evas_Object *navi_bar);

/**
* Create a text box, which cannot be edited
* @return a text box, which cannot be edited
*/
extern Evas_Object *setting_create_textbox(Evas_Object *parent,
                                           const char *content);

/****done_list****/

/**
* Create selectinfo popup
*/
extern void setting_create_sel_info_popup(Evas_Object *parent,
                                          Evas_Object **selInfoPop_layout,
                                          Evas_Object **selInfoPop);

extern void setting_create_select_info_popup(Evas_Object **popup, Evas_Object *parent, double timeout, char *content_text, char *style);


/*The API is going to register a message to NOTIFICATION_STATUS_MESSAGE_KEY, and the message will be shown in indicator
* @ Quickpannel App save the value of var message, and it supply notification_status_monitor_message_cb_set,
* @ in which, Indicator can set process of vconf(NOTIFICATION_STATUS_MESSAGE_KEY) change event,in the call __message_callback,
* @ it will invoke indicator_part_text_emit_by_win(win,"message.text.compare", text); to show message you registered
*/
extern void setting_create_indicator_notification(const char *message);
/**
* Check if some item is selected in done-list
*
* @param[in] list_data
* @return
*/
extern bool setting_done_list_is_some_item_selected(Setting_Done_List_Data *
                                                    list_data);

/**
* Do process when clicking the check object in the done-list item
*/
extern void setting_done_list_Gendial_chk_btn_cb(void *data, Evas_Object *obj,
                                                 void *event_info);

/**
* The API to create done-list.
* @param[in] list_data
* @return a done-list
*/
extern Evas_Object *setting_create_done_list(Setting_Done_List_Data *list_data, const char *title);

extern Evas_Object *setting_create_done_list_with_image(Setting_Done_List_Data *list_data, const char *title);

/**
* Get searchbar text and register redraw_cb by idler.
*
* @param[in] data
* @param[in] search_bar
* @param[in] search_text
* @param[in] search_idler
* @param[in] redraw_cb
*/
extern int setting_searchbar_redraw(void *data,
                                    Evas_Object *search_bar,
                                    char *search_text,
                                    Ecore_Idler **search_idler,
                                    Ecore_Task_Cb redraw_cb);

/**
* Create searchar, register change cb and cancel cb.
*
* @param[in] data
* @param[in] parent
* @param[in] change_cb
* @param[in] cancel_cb
*/
extern Evas_Object *setting_create_searchbar(void *data, Evas_Object *parent,
                                             Evas_Smart_Cb change_cb,
                                             Evas_Smart_Cb cancel_cb);

/**
* Make it no effect when clicking on a evas object, such as, layout, genlist
*
*/
extern void setting_disable_evas_object(Evas_Object *obj);

/**
* Make it normal acting when clicking on a evas object, such as, layout, genlist
*
*/
extern void setting_enable_evas_object(Evas_Object *obj);

/**
* To disable a specialized genlist item
*
*/
extern void setting_disable_genlist_item(Elm_Object_Item *item);

/**
* To enable a specialized genlist item
*
*/
extern void setting_enable_genlist_item(Elm_Object_Item *item);

extern void setting_genlist_item_disabled_set(Setting_GenGroupItem_Data *item_data, int disable);

extern void __add_gl_tts_feature(Elm_Object_Item *item);
extern void setting_genlist_item_groupstyle_set(Setting_GenGroupItem_Data *list_item, setting_group_style group_style);


/**
* To resize a evas object, such as, icon, button..
*
*/
extern void setting_resize_object(Evas_Object *obj, Evas_Coord w, Evas_Coord h);

/**
* To create a bg object
*
*/
extern Evas_Object *setting_create_bg(Evas_Object *parent, Evas_Object *win, const char *bg_style);

/**
* To make an object which is operated by setting_dim_evas_object normal color
*
* @param[in] obj
* @param[in] b_transparenct:
*	1:the appearence of obj is made up of transparent backgroud color.
*	0:the appearence of obj is made up of non transparent color.
*/
extern void setting_undo_dim_evas_object(Evas_Object *obj, bool b_transparenct);

/**
* To make an object dim color
*
* @param[in] obj
* @param[in] b_transparenct:
*	1:the appearence of obj is made up of transparent backgroud color.
*	0:the appearence of obj is made up of non transparent color.
*/
extern void setting_dim_evas_object(Evas_Object *obj, bool b_transparenct);

/**
* To make an object looks clickable: if pressed, it is dim color;if unpressed, it is normal color;
*
* @param[in] obj
*/
extern void setting_make_evas_object_clickable(Evas_Object *obj);
/**
* To create an icon which looks clickable(if pressed, it is dim color;if unpressed, it is normal color)
*
* @param[in] parent
* @param[in] img_path
* @param[in] up_cb
* @param[in] down_cb
* @param[in] move_cb
* @param[in] data
*/
extern Evas_Object *setting_create_icon(Evas_Object *parent, const char *img_path,
                                        Evas_Object_Event_Cb up_cb,
                                        Evas_Object_Event_Cb down_cb,
                                        Evas_Object_Event_Cb move_cb,
                                        void *data);
/**
* to make sbu-obj the same size as parent-obj.Notice: subobj is the only sub-obj @parent wants to display
*
* @param[in] parent
* @param[in] subobj
*/

/**
* To create a button which only has a image and looks clickable
* (if pressed, it is dim color;if unpressed, it is normal color)
* @param[in] parent
* @param[in] img_path
* @param[in] up_cb
* @param[in] down_cb
* @param[in] move_cb
* @param[in] data
*/
extern Evas_Object *setting_create_image_button(Evas_Object *parent,
                                                const char *btn_img,
                                                setting_call_back_func clicked_cb,
                                                setting_call_back_func unpressed_cb,
                                                void *data);

/*
* set font_size, color or align(right, left, middle),if need, you
* can add </font_siz>, </color> or </align> to controll a segment words; or else
* the text will take the same effect until the end of text.
*/
extern char *setting_customize_text(const char *input_str, const int font_size,
                                    const char *color, const char *align);

extern void setting_set_tts_info(Evas_Object *obj, const char *label,
                                 const char *traits, const char *state,
                                 const char *guide);
extern void setting_go_to_top(Evas_Object *content);

/*
* To go to top of list in top view of navibar.
* @param[in] navibar
*/
extern void setting_go_to_navibar_list_top(Evas_Object *navibar);

extern void setting_check_genlist_item_bottom_line(Elm_Object_Item *item);
extern void __gl_realized_cb(void *data, Evas_Object *obj, void *event_info);
extern void setting_set_tts_info(Evas_Object *obj, const char *label,
                                 const char *traits, const char *state,
                                 const char *guild);
extern void setting_entry_entry_set(Evas_Object *entry, const char *text, const int font_size);
extern void setting_genlist_item_groupstyle_set(Setting_GenGroupItem_Data *list_item, setting_group_style group_style);
extern void setting_update_gl_item_chk_status(Setting_GenGroupItem_Data *item_data, int status);
extern void setting_force_update_gl_item_chk_status(Setting_GenGroupItem_Data *item_data, int status);

extern void setting_conformant_keypad_state(Evas_Object *conform, bool enable);

extern void __popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);
extern void __popup_event_set(Evas_Object *popup, void *data,
                              setting_call_back_func response_cb,
                              int timeout,/*to control the timeout time */
                              bool blocked_flag,/*to control whether to block the screen */
                              bool keygrab_flag/*to control whether to block the 'Home key' */
                             );


extern Evas_Object *setting_create_navibar_title_text_btn(Evas_Object *parent, const char *text, Evas_Smart_Cb func, void *data);
/*help widget */
#if SUPPORT_HELPUI
extern void setting_help_popup_circle_block_create(Evas_Object *win, Evas_Object *parent, Evas_Object **circle,
                                                   Evas_Object **popup, char *string_id, int x, int y, Evas_Coord_Rectangle *rect);
extern void setting_help_popup_circle_block_delete(Evas_Object *win, Evas_Object **circle, Evas_Object **popup);
extern void setting_help_popup_circle_block_move(Evas_Object *win, Evas_Object *circle, Evas_Object *popup, int x,
                                                 int y, Evas_Coord_Rectangle *rect);
extern void setting_help_popup_circle_unblock_create(Evas_Object *parent, Evas_Object **circle,
                                                     Evas_Object **popup, char *string_id, int x, int y);
extern void setting_help_popup_block_create(Evas_Object *win, Evas_Object *parent,
                                            Evas_Object **popup, char *string_id, int x, int y, Evas_Coord_Rectangle *rect);
extern void setting_help_popup_unblock_create(Evas_Object *parent,
                                              Evas_Object **popup, char *string_id, int x, int y);
extern void setting_help_popup_block_move(Evas_Object *win, Evas_Object *popup, int x, int y,
                                          Evas_Coord_Rectangle *rect);
#endif

#define SWIP_EFFECT_ENABLE(ly_main, tabbar) setting_tabbar_enable_swip_effect(ly_main, tabbar)
#define SWIP_EFFECT_DISABLE(ly_main, tabbar) setting_tabbar_disable_swip_effect(ly_main, tabbar)

extern void setting_tabbar_enable_swip_effect(Evas_Object *ly_main, Evas_Object *tabbar);
extern void setting_tabbar_disable_swip_effect(Evas_Object *ly_main, Evas_Object *tabbar);
extern void setting_obj_listen_on(Evas_Object *obj, const char *vconf, vconf_callback_fn cb, void *data);
extern void setting_genlist_unregister_tts_access_highlight(Evas_Object *obj, const char *itc);
extern void setting_decorate_image_RGBA(Evas_Object *obj, int r, int g, int b, int a);
extern void setting_protect_cb(void *data, Evas *e, Evas_Object *obj, void *event_info);

extern char *__setting_phone_lang_get_by_pattern(const char *locale, const char *skeleton);

#endif				/* __SETTING_COMMON_DRAW_WIDGET_H__ */
