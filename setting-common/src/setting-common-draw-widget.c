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
#include <setting-common-draw-widget.h>
#include <glib.h>
#include <efl_extension.h>

/**
 * Hide the input pannel
 *
 * @param[in] entry
 */
EXPORT_PUBLIC
void setting_hide_input_pannel_cb(Evas_Object *entry)
{
	ret_if(entry == NULL);
	Ecore_IMF_Context *imf_context =
			(Ecore_IMF_Context *)elm_entry_imf_context_get(entry);
	if (imf_context)
		ecore_imf_context_input_panel_hide(imf_context);

	elm_object_focus_set(entry, EINA_FALSE);
	Evas_Object *entry_container = elm_object_parent_widget_get(entry);
	if (entry_container) {
		if (elm_entry_is_empty(entry))
			elm_object_signal_emit(entry_container,
					"elm,state,guidetext,show", "elm");
		elm_object_signal_emit(entry_container,
				"elm,state,eraser,hide", "elm");
	}
}

EXPORT_PUBLIC
Evas_Object *setting_create_button(
		Evas_Object *parent,
		const char *btn_str,
		const char *btn_style,
		void *btn_click_cb,
		void *cb_data)
{
	LAUNCH_SETTING_IN();
	retv_if(!parent || !btn_str, NULL);
	if ('\0' == btn_str[0]) {/*Empty rectangle */
		return setting_create_blank_rect_customize(
				parent,
				SETTING_PADDING_WIDTH,
				SETTING_PADDING_WIDTH);
	}

	Evas_Object *button = elm_button_add(parent);
	retv_if(!button, NULL);

	if (btn_style)
		elm_object_style_set(button, btn_style);

	/* not '<-' button */
	if (0 != safeStrCmp("naviframe/end_btn/default", btn_style)) {
		elm_object_text_set(button, _(btn_str));

		evas_object_size_hint_weight_set(
				button, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(button, EVAS_HINT_FILL, 0.5);
	}

	if (btn_click_cb) {
		evas_object_smart_callback_add(
				button, "clicked", btn_click_cb, cb_data);
	}
	evas_object_show(button);
	LAUNCH_SETTING_OUT();
	return button;
}

static double _step_size_calculate(Evas_Object *obj, double min, double max)
{
	double step = 0.0;
	int steps = 0;

	steps = max - min;
	if (steps)
		step = (1.0 / steps);

	return step;
}

/**
 * To create slider object of a genlist item
 *
 *@return a slider container object
 */
EXPORT_PUBLIC Evas_Object *setting_create_5step_slider(
		Evas_Object *parent,
		Evas *evas,
		const char *l_swallow_path,
		const char *r_swallow_path,
		double value,
		bool indicator,
		double slider_min,
		double slider_max,
		setting_call_back_func slider_change_cb,
		setting_call_back_func slider_start_change_cb,
		setting_call_back_func slider_stop_change_cb,
		void *cb_data)
{

	Evas_Object *layout = elm_layout_add(parent);
	int r = elm_layout_file_set(layout, SETTING_5STEP_SLIDER_EDJ_NAME,
			"elm/slider/horizontal/music/soundalive");
	if (!r) {
		SETTING_TRACE_ERROR("elm_layout_add failed : %s",
				"elm/slider/horizontal/music/soundalive");
		evas_object_del(layout);
		return NULL;
	}

	/* "elm/slider/horizontal/default" */
	Evas_Object *slider = elm_slider_add(layout);
	retv_if(slider == NULL, NULL);
	elm_object_style_set(slider, "music/soundalive");

	elm_slider_indicator_format_set(slider, "%1.0f");
	elm_slider_indicator_show_set(slider, 5);

	evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, 0.5);

	double step = _step_size_calculate(slider, slider_min, slider_max);
	elm_slider_step_set(slider, step);

	SETTING_TRACE("slider_change_cb:%p", slider_change_cb);

	if (slider_change_cb) evas_object_smart_callback_add(
			slider, "changed", slider_change_cb, cb_data);
	if (slider_stop_change_cb) evas_object_smart_callback_add(
			slider, "slider,drag,stop", slider_stop_change_cb,
			cb_data);
	if (slider_start_change_cb) evas_object_smart_callback_add(
			slider, "slider,drag,start", slider_start_change_cb,
			cb_data);

	if (l_swallow_path) {
		Evas_Object *icon1 = elm_icon_add(slider);
		elm_image_file_set(icon1, l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(
				icon1, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_content_set(slider, icon1);
	}

	if (r_swallow_path) {
		Evas_Object *icon2 = elm_icon_add(slider);
		elm_image_file_set(icon2, r_swallow_path, NULL);
		evas_object_size_hint_aspect_set(
				icon2, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_part_content_set(slider, "end", icon2);
	}

	elm_slider_min_max_set(slider, slider_min, slider_max);
	elm_slider_value_set(slider, value);
	return slider;
}

/**
 * To create slider object of a genlist item
 *
 *@return a slider container object
 */
EXPORT_PUBLIC Evas_Object *setting_create_slider(
		Evas_Object *parent,
		Setting_GenGroupItem_Data *item_data)
{
	/* "elm/slider/horizontal/default" */
	Evas_Object *slider = elm_slider_add(parent);
	retv_if(slider == NULL, NULL);

	if (item_data->isIndicatorVisible) {
		elm_slider_indicator_format_set(slider, "%1.0f");
		elm_slider_indicator_show_set(slider, EINA_TRUE);
	} else {
		/* for brightness slider */
		elm_slider_indicator_show_set(slider, EINA_FALSE);
		/*elm_object_style_set(slider, "tap_to_drag");*/
	}

	evas_object_size_hint_weight_set(slider, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(slider, EVAS_HINT_FILL, 0.5);

	double step = _step_size_calculate(
			slider, item_data->slider_min, item_data->slider_max);
	elm_slider_step_set(slider, step);

	SETTING_TRACE("slider_change_cb:%p", item_data->chk_change_cb);
	if (item_data->chk_change_cb)
		evas_object_smart_callback_add(
				slider, "changed", item_data->chk_change_cb,
				item_data);

	if (item_data->stop_change_cb)
		evas_object_smart_callback_add(
				slider, "slider,drag,stop",
				item_data->stop_change_cb, item_data);

	if (item_data->start_change_cb)
		evas_object_smart_callback_add(
				slider, "slider,drag,start",
				item_data->start_change_cb, item_data);

	if (item_data->l_swallow_path) {
		Evas_Object *icon1 = elm_icon_add(slider);
		elm_image_file_set(icon1, item_data->l_swallow_path, NULL);
		evas_object_size_hint_aspect_set(
				icon1, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_content_set(slider, icon1);
	}

	if (item_data->r_swallow_path) {
		Evas_Object *icon2 = elm_icon_add(slider);
		elm_image_file_set(icon2, item_data->r_swallow_path, NULL);
		evas_object_size_hint_aspect_set(
				icon2, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
		elm_object_part_content_set(slider, "end", icon2);
	}

	elm_slider_min_max_set(
			slider, item_data->slider_min, item_data->slider_max);
	elm_slider_value_set(slider, item_data->chk_status);
	return slider;
}

/**
 * The main implement body of create a certain size icon
 */
EXPORT_PUBLIC
void create_image_box_add_ex(
		void *data,
		Evas_Object *win_main,
		Evas *evas,
		char *img_path,
		int img_w,
		int img_h,
		Evas_Object_Event_Cb mouse_down_cb,
		Evas_Object_Event_Cb mouse_up_cb,
		Evas_Object **image_box,
		Evas_Object **img)
{
	/* SETTING_TRACE_BEGIN; */
	*image_box = elm_box_add(win_main);
	retm_if(*image_box == NULL, "Cannot get box");
	elm_box_horizontal_set(*image_box, 1);
	evas_object_size_hint_align_set(*image_box, 0.0, EVAS_HINT_FILL);
	evas_object_show(*image_box);

	*img = evas_object_image_add(evas);
	evas_object_size_hint_weight_set(*img, 0.0, EVAS_HINT_EXPAND);

	int temp_h = img_h;
	int temp_w = img_w;

	evas_object_image_file_set(*img, img_path, NULL);
	/*error handling. load the image as it's own size */
	if (img_h <= 0 || temp_w <= 0)
		evas_object_image_size_get(*img, &temp_w, &temp_h);

	evas_object_image_load_size_set(*img, temp_w, temp_h);
	evas_object_image_fill_set(*img, 0, 0, temp_w, temp_h);

	evas_object_size_hint_min_set(
			*img, ELM_SCALE_SIZE(temp_w), ELM_SCALE_SIZE(temp_h));
	evas_object_show(*img);

	evas_object_event_callback_add(
			*img, EVAS_CALLBACK_MOUSE_DOWN, mouse_down_cb, data);
	evas_object_event_callback_add(
			*img, EVAS_CALLBACK_MOUSE_UP, mouse_up_cb, data);

	elm_box_pack_end(*image_box, *img);
}

/**
 * To create a size fixed icon
 * @return a image container object
 */

EXPORT_PUBLIC
Evas_Object *create_image_box_add(
		void *data,
		Evas_Object *win_main,
		Evas *evas,
		char *img_path,
		int img_w,
		int img_h,
		Evas_Object_Event_Cb mouse_down_cb,
		Evas_Object_Event_Cb mouse_up_cb)
{
	/* SETTING_TRACE_BEGIN; */
	Evas_Object *image_box = NULL;
	Evas_Object *img = NULL;
	create_image_box_add_ex(
			data,
			win_main,
			evas,
			img_path,
			img_w * WIDGET_SCALE_FACTOR,
			img_h * WIDGET_SCALE_FACTOR,
			mouse_down_cb,
			mouse_up_cb,
			&image_box,
			&img);
	return image_box;

}

/**
* To create a size specialized icon
*
* @return a image container object
*/

EXPORT_PUBLIC
Evas_Object *create_bgimage_box_add(
		void *data,
		Evas_Object *win_main,
		Evas *evas,
		char *img_path,
		int img_w,
		int img_h,
		Evas_Object_Event_Cb mouse_down_cb,
		Evas_Object_Event_Cb mouse_up_cb)
{
	/* if img_w<0 or img_h <0, load the image as it's own size */
	Evas_Object *image_box = NULL;
	Evas_Object *img = NULL;
	create_image_box_add_ex(
			data,
			win_main,
			evas,
			img_path,
			img_w,
			img_h,
			mouse_down_cb,
			mouse_up_cb,
			&image_box, &
			img);
	return image_box;
}

/**
* @ create a box container
*
* @param[in] win_main
*
* @return a box container
*/
EXPORT_PUBLIC
Evas_Object *setting_create_client_bx(Evas_Object *win_main)
{
	Evas_Object *client_bx = NULL;

	client_bx = elm_box_add(win_main);
	retvm_if(client_bx == NULL, NULL, "Cannot create client box object");

	elm_box_horizontal_set(client_bx, 0);
	evas_object_size_hint_weight_set(client_bx, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(client_bx, EVAS_HINT_FILL, 0.0);

	return client_bx;
}


EXPORT_PUBLIC
Evas_Object *setting_create_lable(
		Evas_Object *parent, const char *text,
		const char *style, const char *align)
{
	retvm_if(!parent || !text, NULL, "Cannot create client box object");
	Evas_Object *label = NULL;
	if (0 == safeStrCmp(style, "entry")) {
		SETTING_TRACE("style:%s", style);
		label = elm_entry_add(parent);
		elm_entry_editable_set(label, EINA_FALSE);
	} else {
		label = elm_label_add(parent);
		elm_object_style_set(label, style);
		elm_label_line_wrap_set(label, ELM_WRAP_MIXED);
	}
	const char *str = setting_customize_text(text, 0, NULL, align);
	elm_object_text_set(label, str);
	FREE(str);
	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(label);
	return label;
}

#define DEMO_STEP 0.8
typedef struct _setting_gif_data {
	Ecore_Timer *update_timer;
	const char	**png_list;
	int		cur_png_idx;
} setting_gif_data;

static void __gif_del_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	setting_gif_data *gif_data = data;
	if (gif_data->update_timer) {/*first delete the timer */
		ecore_timer_del(gif_data->update_timer);
		gif_data->update_timer = NULL;
	}
	FREE(gif_data);/*delete the date. */
	evas_object_data_set(obj, "gif_data", NULL);
}

Eina_Bool __gif_updater(void *data)
{
	retv_if(!data, FALSE);
	Evas_Object *gif =	data;
	setting_gif_data *gif_data = evas_object_data_get(gif, "gif_data");
	retv_if(!gif_data, FALSE);
	/*SETTING_TRACE_BEGIN; */

	gif_data->cur_png_idx++;
	/*the last element of current array. */
	if (NULL == gif_data->png_list[gif_data->cur_png_idx])
		gif_data->cur_png_idx = 0;

	elm_image_file_set(
			gif, gif_data->png_list[gif_data->cur_png_idx], NULL);
	int w = 0;
	int h = 0;
	elm_image_object_size_get(gif, &w, &h);
	setting_resize_object(gif, w, h);

	return TRUE;
}

EXPORT_PUBLIC
Evas_Object *setting_create_image(Evas_Object *parent, const char *img_path)
{
	Evas_Object *image = elm_image_add(parent);
	/*SETTING_TRACE("icon_path:%s", icon_path); */
	elm_image_file_set(image, img_path, NULL);
	elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);

	int w = 0;
	int h = 0;
	elm_image_object_size_get(image, &w, &h);
	setting_resize_object(image, w, h);
	evas_object_show(image);
	return image;
}

EXPORT_PUBLIC
Evas_Object *setting_create_image_with_round_corner(
		Evas *evas, const char *img_path, int w, int h)
{
	Evas_Object *image = evas_object_image_add(evas);
	evas_object_size_hint_weight_set(image, 0.0, EVAS_HINT_EXPAND);
	evas_object_image_load_size_set(image, w, h);
	evas_object_image_file_set(image, img_path, NULL);

	evas_object_image_fill_set(image, 0, 0, w, h);

	/*Set the dimensions for an image object's border, a region which @b
	* @param obj The given image object.
	* @param l The border's left width.
	* @param r The border's right width.
	* @param t The border's top width.
	* @param b The border's bottom width.
	*/
	/*for round corner */
	evas_object_image_border_set(image, 16, 16, 16, 16);
	evas_object_image_filled_set(image, 1); /*to full fill */
	evas_object_size_hint_min_set(
			image, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
	evas_object_size_hint_max_set(image, w, h);
	return image;
}

/* @png_list is an array end with 'NULL'. like ,
		const char *png_list[] = {
			SETTING_ICON_PATH_CFG"motions/motion_overturn_01.png",
			SETTING_ICON_PATH_CFG"motions/motion_overturn_02.png",
			SETTING_ICON_PATH_CFG"motions/motion_overturn_03.png",
			SETTING_ICON_PATH_CFG"motions/motion_overturn_04.png",
			SETTING_ICON_PATH_CFG"motions/motion_overturn_05.png",
			NULL //must end with 'NULL'
		};
*/
EXPORT_PUBLIC
Evas_Object *setting_create_gif(Evas_Object *parent, const char **png_list)
{
	retv_if(!png_list, NULL);
	setting_gif_data *gif_data = calloc(1, sizeof(setting_gif_data));
	setting_retvm_if(!gif_data, NULL, "calloc failed");
	gif_data->cur_png_idx = 0;
	gif_data->png_list = png_list;

	SETTING_TRACE_BEGIN;

	/*the first element */
	Evas_Object *gif = setting_create_image(parent, *png_list);
	/*bind object data */
	evas_object_data_set(gif, "gif_data", gif_data);
	evas_object_event_callback_add(
			gif, EVAS_CALLBACK_DEL, __gif_del_cb, gif_data);

	if (gif_data->update_timer) {
		ecore_timer_del(gif_data->update_timer);
		gif_data->update_timer = NULL;
	}
	gif_data->update_timer = ecore_timer_add(
			DEMO_STEP, (Ecore_Task_Cb)__gif_updater, gif);

	/* W011:bg, T051:text */
	setting_decorate_image_RGBA(gif, 14, 41, 73, 255);
	return gif;
}

/**
* Alternate the check object status and its binded vcof value via original
* vcofn value
*/
EXPORT_PUBLIC
int setting_draw_onoff_status(Evas_Object *obj, setting_bool_slp_list key)
{
	int value;
	int err;
	int ret;

	setting_get_bool_slp_key(key, &value, &err);

	elm_check_state_set(obj, !value);
	ret = setting_set_bool_slp_key(key, !value, &err);
	return ret;
}

/**
* Set the status of check object via its binded vconf value.
* @return  0: sucess, other: failed
*/
EXPORT_PUBLIC
int setting_check_onoff_status(Evas_Object *obj, setting_bool_slp_list key)
{
	int value = 0;
	int err = 0;
	int ret = 0;

	ret = setting_get_bool_slp_key(key, &value, &err);
	elm_check_state_set(obj, value);
	return ret;
}

/**
* Set the status of radio group object via its binded vconf value.
* @return  0: sucess, other: failed
*/
EXPORT_PUBLIC
int setting_update_chk_status(Evas_Object *chk, setting_int_slp_list type)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	if (chk == NULL)
		return SETTING_GENERAL_ERR_NULL_DATA_PARAMETER;

	SETTING_TRACE_BEGIN;

	int ret = SETTING_RETURN_FAIL;
	int value;
	int err;

	/* value set */
	ret = setting_get_int_slp_key(type, &value, &err);
	SETTING_TRACE("binded: %d, checked: %d, err: %d", type, value, err);

	elm_radio_value_set(chk, value);

	return ret;
}

/**
* Create a size specilized rectangle object
* @return a size specilized rectangle object
*/
EXPORT_PUBLIC
Evas_Object *setting_create_blank_rect_customize(
		Evas_Object *layout,
		int w,
		int h)
{
	Evas_Object *rect;

	rect = evas_object_rectangle_add(evas_object_evas_get(layout));
	evas_object_size_hint_min_set(
			rect, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
	evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, 0);
	/*evas_object_color_set(rect, 0, 0, 0, 0); */
	evas_object_color_set(rect, 8, 8, 8, 8);

	return rect;
}

/**
* Create a size fixed rectangle object
*
* @param[in] layout
*
* @return a size fixed rectangle object
*/
EXPORT_PUBLIC
Evas_Object *setting_create_blank_rect(Evas_Object *layout)
{
	return setting_create_blank_rect_customize(
			layout,
			SETTING_PADDING_WIDTH,
			80);
}

/**
* The API to Create an editfiled object
* @return an editfiled object
*
* multi-line is default, use elm_editfield_entry_single_line_set(ef, EINA_TRUE)
* to set single-line
*/
EXPORT_PUBLIC
Evas_Object *
setting_create_editfiled(Evas_Object *win_main, char *title, char *def_str,
		char *guide_text)
{
	Evas_Object *layout = NULL;
	Evas_Object *entry = NULL;
	layout = elm_layout_add(win_main);

	if (title) {
		elm_layout_theme_set(layout, "layout", "editfield", "title");
		elm_object_part_text_set(layout, "elm.text", title);
	} else {
		elm_layout_theme_set(layout, "layout", "editfield", "default");
	}
	entry = elm_entry_add(win_main);
	elm_object_part_content_set(layout, "elm.swallow.content", entry);
	/*elm_entry_context_menu_disabled_set(entry, TRUE); */
	/*elm_entry_magnifier_disabled_set(entry, TRUE); */
	elm_entry_prediction_allow_set(entry, FALSE);

	if (def_str && safeStrLen(def_str) > 0) {
		elm_entry_entry_set(entry, def_str);
	} else {
		if (guide_text) {
			elm_object_part_text_set(
					layout,
					"elm.guidetext",
					_(guide_text));
		} else {
			elm_object_part_text_set(
					layout,
					"elm.guidetext",
					_("IDS_ST_BODY_TAP_TO_INSERT"));
		}
	}

	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, 0);
	evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, 0);

	evas_object_show(layout);
	return layout;
}

/**
* Create a text box, which cannot be edited, just to show a specilization
* segment.
*
* @return a text box, which cannot be edited
*/
EXPORT_PUBLIC
Evas_Object *setting_create_textbox(Evas_Object *parent, const char *content)
{
	Evas_Object *obj = NULL;

	obj = elm_entry_add(parent);
	evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(obj, EVAS_HINT_FILL, 0.5);
	elm_entry_single_line_set(obj, 0);
	/* elm_entry_line_wrap_set(obj, 1); */
	elm_entry_editable_set(obj, 0);
	elm_entry_context_menu_disabled_set(obj, 1);

	/* setting_disable_evas_object(obj); ***disable any events on entry */

	elm_entry_entry_set(obj, content);
	evas_object_show(obj);
	return obj;
}

/**
* Make it no effect when clicking on a evas object and its sub-objects,
* such as, layout, genlist
*
* @param[in] obj
*/
EXPORT_PUBLIC
void setting_disable_evas_object(Evas_Object *obj)
{
	/* layout\genlist\scroller\elm_entry  etc.. */
	if (obj)
		evas_object_pass_events_set(obj, EINA_TRUE);
}

/**
* Make it normal acting when clicking on a evas object, such as, layout,
* genlist
*
* @param[in] obj
*/
EXPORT_PUBLIC
void setting_enable_evas_object(Evas_Object *obj)
{
	if (obj)
		evas_object_pass_events_set(obj, EINA_FALSE);
}

/**
* To resize a evas object, such as, icon, button..
*
* @param[in] obj
* @param[in] w
* @param[in] h
*/
EXPORT_PUBLIC
void setting_resize_object(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	if (obj) {
		evas_object_size_hint_min_set(
				obj, ELM_SCALE_SIZE(w), ELM_SCALE_SIZE(h));
		evas_object_size_hint_max_set(obj, w, h);
		evas_object_resize(obj, w, h);
	}
}

/**
* To create a bg object
*
* @param[in] parent
* @param[in] bg_style
* @return bg object
*/
EXPORT_PUBLIC
Evas_Object *setting_create_bg(
		Evas_Object *parent, Evas_Object *win, const char *bg_style)
{
	retv_if(!parent, NULL);
	Evas_Object *bg = elm_bg_add(parent);
	retv_if(!bg, NULL);
	if (bg_style) elm_object_style_set(bg, bg_style);
	evas_object_size_hint_weight_set(
			bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	/*elm_win_resize_object_add(win, bg); */
	evas_object_show(bg);
	return bg;
}


#define DIFF_TIMES	2
/**
* To make an object which is operated by setting_dim_evas_object normal color
*
* @param[in] obj
* @param[in] b_transparency_set:
*	1:the appearence of obj is made up of transparent backgroud color.
*	0:the appearence of obj is made up of non transparent color.
*/
EXPORT_PUBLIC
void setting_undo_dim_evas_object(Evas_Object *obj, bool b_transparenct)
{
	if (obj) {
		int r;
		int g;
		int b;
		int a;
		evas_object_color_get(obj, &r, &g, &b, &a);
		/*the appearence of obj is made up of transparent backgroud
		 * color. */
		if (b_transparenct)
			a *= DIFF_TIMES;

		evas_object_color_set(
				obj,
				r * DIFF_TIMES,
				g * DIFF_TIMES,
				b * DIFF_TIMES,
				a);
	}
}

/**
* To make an object dim color
*
* @param[in] obj
* @param[in] b_transparency_set:
*	1:the appearence of obj is made up of transparent backgroud color.
*	0:the appearence of obj is made up of non transparent color.
*/
EXPORT_PUBLIC
void setting_dim_evas_object(Evas_Object *obj, bool b_transparenct)
{
	if (obj) {
		int r;
		int g;
		int b;
		int a;
		evas_object_color_get(obj, &r, &g, &b, &a);
		/*the appearence of obj is made up of transparent backgroud
		 * color. */
		if (b_transparenct)
			a /= DIFF_TIMES;

		evas_object_color_set(
				obj,
				r / DIFF_TIMES,
				g / DIFF_TIMES,
				b / DIFF_TIMES,
				a);
	}
}

static void __btn_select_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	setting_dim_evas_object(obj, FALSE);
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	retm_if(data == NULL, "Invalid argument: data is NULL");
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
	Evas_Point *point_down = data;
	point_down->x = ev->output.x;
	point_down->y = ev->output.y;
}

static void __btn_release_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	setting_undo_dim_evas_object(obj, FALSE);
}

static void __btn_mouse_move_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	retm_if(data == NULL, "Invalid argument: data is NULL");
	Evas_Point *point_down = data;
	Evas_Event_Mouse_Move *ev = (Evas_Event_Mouse_Move *)event_info;
	int x_offset = ev->cur.output.x - point_down->x;
	int y_offset = ev->cur.output.y - point_down->y;
	if (x_offset < 0) x_offset = -x_offset;
	if (y_offset < 0) y_offset = -y_offset;
	if (x_offset <= MIN_MOVE_DISTANCE && y_offset <= MIN_MOVE_DISTANCE)
		return;

	setting_undo_dim_evas_object(obj, FALSE);
}
static void __btn_del_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	ret_if(!data);
	Evas_Point *point_down = data;
	FREE(point_down);/*delete the date. */
}

/**
* To make an object looks clickable: if pressed, it is dim color;if unpressed,
* it is normal color;
*
* @param[in] obj
*/
EXPORT_PUBLIC
void setting_make_evas_object_clickable(Evas_Object *obj)
{
	if (obj) {
		Evas_Point *point_down = calloc(1, sizeof(Evas_Point));
		evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_DOWN,
				__btn_select_cb, point_down);
		evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_UP,
				__btn_release_cb, NULL);
		evas_object_event_callback_add(obj, EVAS_CALLBACK_MOUSE_MOVE,
				__btn_mouse_move_cb, point_down);
		evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL,
				__btn_del_cb, point_down);
	}
}

/**
* To create an icon which looks clickable(if pressed, it is dim color;
* if unpressed, it is normal color)
*
* @param[in] parent
* @param[in] img_path
* @param[in] up_cb
* @param[in] down_cb
* @param[in] move_cb
* @param[in] data
*/
EXPORT_PUBLIC
Evas_Object *setting_create_icon(
		Evas_Object *parent,
		const char *img_path,
		Evas_Object_Event_Cb up_cb,
		Evas_Object_Event_Cb down_cb,
		Evas_Object_Event_Cb move_cb,
		void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!parent || !img_path, NULL);
	Evas_Object *icon = elm_icon_add(parent);
	retvm_if(!icon, NULL, "Failed Create icon!");
	elm_image_file_set(icon, img_path, NULL);
	setting_make_evas_object_clickable(icon);

	if (up_cb) {
		evas_object_event_callback_add(
				icon, EVAS_CALLBACK_MOUSE_UP, up_cb, data);
	}
	if (down_cb) {
		evas_object_event_callback_add(
				icon, EVAS_CALLBACK_MOUSE_DOWN, down_cb, data);
	}
	if (move_cb) {
		evas_object_event_callback_add(
				icon, EVAS_CALLBACK_MOUSE_MOVE, move_cb, data);
	}

	return icon;
}

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
EXPORT_PUBLIC
Evas_Object *setting_create_image_button(
		Evas_Object *parent,
		const char *btn_img,
		setting_call_back_func clicked_cb,
		setting_call_back_func unpressed_cb,
		void *data)
{
	SETTING_TRACE_BEGIN;
	retv_if(!parent, NULL);
	Evas_Object *btn = elm_button_add(parent);
	retvm_if(!btn, NULL, "Failed Create button!");
	if (clicked_cb) {
		evas_object_smart_callback_add(
				btn, "clicked", clicked_cb, data);
	}

	if (unpressed_cb) {
		evas_object_smart_callback_add(
				btn, "unpressed", unpressed_cb, data);
	}

	if (btn_img) {
		Evas_Object *icon = elm_icon_add(parent);
		retvm_if(!icon, NULL, "Failed Create icon!");
		elm_image_file_set(icon, btn_img, NULL);
		elm_object_part_content_set(btn, "elm.swallow.content", icon);
		setting_make_evas_object_clickable(btn);
	}

	return btn;
}

EXPORT_PUBLIC void setting_entry_entry_set(
		Evas_Object *entry, const char *text, const int font_size)
{
	if (isEmptyStr(text)) return;
	if (entry) {
		char buf[200];
		if (font_size > 0) {
			snprintf(buf, sizeof(buf), "DEFAULT='font_size=%d'",
					font_size);
		}
		elm_entry_text_style_user_push(entry, buf);
		elm_entry_entry_set(entry, text);
	}
}


/*
* set font_size, color or align(right, left, middle),if need, you
* can add </font_siz>, </color> or </align> to controll a segment words;
* or else the text will take the same effect until the end of text.
* @param[in] input_str
* @param[in] font_size
* @param[in] color
* @param[in] align :may be right, left, middle/center
*/
EXPORT_PUBLIC
char *setting_customize_text(
		const char *input_str, const int font_size,
		const char *color, const char *align)
{
	/*SETTING_TRACE_BEGIN; */
	retv_if(isEmptyStr(input_str), NULL);
	/*SETTING_TRACE("input_str:%s, color:%s", input_str, color); */
	char speciliztion[MAX_SPECIALIZITION_LEN] = { 0, };
	/*<font_size=%d><align=middle><color=#9C9C9C>bbb</color></align>
	 * </font_size> */
	if (font_size > 0) {
		snprintf(speciliztion, sizeof(speciliztion),
				 "<font_size=%d>", font_size);
	}

	if (align) {
		g_strlcat(speciliztion, "<align=", MAX_SPECIALIZITION_LEN);
		g_strlcat(speciliztion, align, MAX_SPECIALIZITION_LEN);
		g_strlcat(speciliztion, ">", MAX_SPECIALIZITION_LEN);
	}

	if (color) {
		char input_str1[MAX_SPECIALIZITION_LEN] = { 0, };
		safeCopyStr(input_str1, input_str, MAX_SPECIALIZITION_LEN);
		char *p = strstr(input_str1, "<color=");
		/*already has customed color,to replace color sub_string */
		if (p) {
			p += strlen("<color=");
			/*memcpy(p, color, strlen(color)); */
			char *q = (char *)color;
			while ('\0' != p[0] && '\0' != q[0]) {
				*p = *q;
				p++;
				q++;
			}
			if ('>' != p[0]) { /*invalid format */
				g_strlcat(speciliztion, "<color=",
						MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, color,
						MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, ">",
						MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, input_str,
						MAX_SPECIALIZITION_LEN);
				g_strlcat(speciliztion, "</color>",
						MAX_SPECIALIZITION_LEN);
			} else
				g_strlcat(speciliztion, input_str1,
						MAX_SPECIALIZITION_LEN);
		} else {
			g_strlcat(speciliztion, "<color=",
					MAX_SPECIALIZITION_LEN);
			g_strlcat(speciliztion, color,
					MAX_SPECIALIZITION_LEN);
			g_strlcat(speciliztion, ">",
					MAX_SPECIALIZITION_LEN);
			g_strlcat(speciliztion, input_str,
					MAX_SPECIALIZITION_LEN);
			g_strlcat(speciliztion, "</color>",
					MAX_SPECIALIZITION_LEN);
		}
	} else {
		g_strlcat(speciliztion, input_str,
				MAX_SPECIALIZITION_LEN);
	}
	if (align) {
		g_strlcat(speciliztion, "</align>",
				MAX_SPECIALIZITION_LEN);
	}
	if (font_size > 0) {
		g_strlcat(speciliztion, "</font_size>",
				MAX_SPECIALIZITION_LEN);
	}

	/*SETTING_TRACE("Exit %s, return :%s", __FUNCTION__, speciliztion); */
	return (char *)strdup(speciliztion);;
}


EXPORT_PUBLIC
void __toogle_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
	/* SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
			(Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	ret_if(!list_item);

	/* new status */
	setting_update_gl_item_chk_status(list_item, !(list_item->chk_status));
	/*int err = 0; */
	/*int ret = setting_set_bool_slp_key(list_item->int_slp_setting_binded,
	 * list_item->chk_status, &err); */
	const char *use_vconf = (char *)(list_item->int_slp_setting_binded);
	int ret = vconf_set_bool(use_vconf, list_item->chk_status);

	setting_retm_if(0 != ret, "Failed to set vconf [%s]",
			list_item->int_slp_setting_binded);
}


EXPORT_PUBLIC
void __toogle_chk_changed(void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
		(Setting_GenGroupItem_Data *) data;
	list_item->chk_status = elm_check_state_get(obj); /* for update */

	/*int err = 0; */
	/*int ret = setting_set_bool_slp_key(list_item->int_slp_setting_binded,
	 * list_item->chk_status, &err); */
	const char *use_vconf = (char *)(list_item->int_slp_setting_binded);
	int ret = vconf_set_bool(use_vconf, list_item->chk_status);
	if (0 != ret) {	/*rollback */
		setting_update_gl_item_chk_status(
				list_item, !(list_item->chk_status));
		return;
	}
}

EXPORT_PUBLIC void setting_conformant_keypad_state(
		Evas_Object *win_main, bool enable)
{
	Evas_Object *conform = evas_object_data_get(win_main, "conformant");
	if (conform) {
		SETTING_TRACE("conformant keypad %d", enable);
		if (enable) {
			elm_object_signal_emit(conform,
					"elm,state,virtualkeypad,enable", "");
			elm_object_signal_emit(conform,
					"elm,state,clipboard,enable", "");
		} else {
			elm_object_signal_emit(conform,
					"elm,state,virtualkeypad,disable", "");
			elm_object_signal_emit(conform,
					"elm,state,clipboard,disable", "");
		}
	}
}

static void __title_btn_del_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)

{
	SETTING_TRACE_BEGIN;
	ret_if(!obj);
	evas_object_data_set(obj, "text", NULL);
}

EXPORT_PUBLIC Evas_Object *setting_create_navibar_title_text_btn(
		Evas_Object *parent,
		const char *text,
		Evas_Smart_Cb func,
		void *data)
{
	Evas_Object *btn = elm_button_add(parent);
	retv_if(!btn, NULL);

	elm_object_style_set(btn, "naviframe/title_text");
	evas_object_event_callback_add(btn, EVAS_CALLBACK_DEL,
			__title_btn_del_cb, NULL);
	if (text) {
		elm_object_text_set(btn, _(text));
		evas_object_data_set(btn, "text", text);
	}
	if (func)
		evas_object_smart_callback_add(btn, "clicked", func, data);

	return btn;
}

#define MIN_SWIP_DISTANCE_X 300
#define MIN_SWIP_DISTANCE_Y 75

static void __tabbar_mouse_down_cb(
		void *data, Evas *e,
		Evas_Object *obj,
		void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	retm_if(data == NULL, "Invalid argument: data is NULL");
	Evas_Event_Mouse_Down *ev = (Evas_Event_Mouse_Down *)event_info;
	Evas_Point *point_down = data;
	point_down->x = ev->output.x;
	point_down->y = ev->output.y;
}

static void __tabbar_mouse_up_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	retm_if(data == NULL, "Invalid argument: data is NULL");
	Evas_Point *point_down = data;
	Evas_Event_Mouse_Up *ev = (Evas_Event_Mouse_Up *)event_info;

	int x_offset = ev->output.x - point_down->x;
	int y_offset = ev->output.y - point_down->y;

	bool skip = TRUE;

	Evas_Object *navi = NULL;
	Elm_Object_Item *top_item = NULL;
	Evas_Object *eo_view = NULL;
	Evas_Object *tabbar = NULL;
	/*char *tabbar_t = NULL; */
	if ((navi = elm_object_part_content_get(obj, "elm.swallow.content"))
		&& (top_item = elm_naviframe_top_item_get(navi))) {
		/*SETTING_TRACE("top_item:%p", top_item); */
		if ((eo_view = elm_object_item_content_get(top_item))
			&& (tabbar = elm_object_part_content_get(
					eo_view, "elm.swallow.tabbar"))
			&& 0 == safeStrCmp(evas_object_type_get(tabbar),
					"elm_toolbar")) {
			/*SETTING_TRACE("Skip Case 1"); */
			skip = FALSE;
		}

		if (skip && (tabbar = elm_object_item_part_content_get(
				top_item, "tabbar"))
			&& 0 == safeStrCmp(evas_object_type_get(tabbar),
					"elm_toolbar")) {
			/*SETTING_TRACE("Skip case 2"); */
			skip = FALSE;
		}
	}
	if (skip)
		return;
	/*SETTING_TRACE("tabbar:%p", tabbar); */
	/*SETTING_TRACE("SWIPE VALUE : x=(%d) : y=(%d)", x_offset, y_offset); */
	if (-x_offset > MIN_SWIP_DISTANCE_X
			&& abs(y_offset) < MIN_SWIP_DISTANCE_Y) {

		Elm_Object_Item *cur_item =
				elm_toolbar_selected_item_get(tabbar);
		if (cur_item) {
			Elm_Object_Item *dest_item =
					elm_toolbar_item_next_get(cur_item);
			elm_toolbar_item_selected_set(dest_item, EINA_TRUE);
		}

	} else if (x_offset > MIN_SWIP_DISTANCE_X
			&& abs(y_offset) < MIN_SWIP_DISTANCE_Y) {
		Elm_Object_Item *cur_item =
				elm_toolbar_selected_item_get(tabbar);
		if (cur_item) {
			Elm_Object_Item *dest_item =
					elm_toolbar_item_prev_get(cur_item);
			elm_toolbar_item_selected_set(dest_item, EINA_TRUE);
		}

	}

}
static void __tabbar_btn_del_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	ret_if(!data);
	Evas_Point *point_down = data;
	FREE(point_down);/*delete the date. */
	evas_object_data_set(obj, "tabbar", NULL);
}

static void __tabbar_mouse_move_cb(
		void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
}

EXPORT_PUBLIC void setting_tabbar_enable_swip_effect(
		Evas_Object *ly_main, Evas_Object *tabbar)
{
	SETTING_TRACE_BEGIN;
	if (ly_main) {
		evas_object_data_set(ly_main, "tabbar", tabbar);
		Evas_Point *point_down = calloc(1, sizeof(Evas_Point));
		evas_object_event_callback_add(
				ly_main,
				EVAS_CALLBACK_MOUSE_DOWN,
				__tabbar_mouse_down_cb,
				point_down);
		evas_object_event_callback_add(
				ly_main,
				EVAS_CALLBACK_MOUSE_UP,
				__tabbar_mouse_up_cb,
				point_down);
		evas_object_event_callback_add(
				ly_main,
				EVAS_CALLBACK_MOUSE_MOVE,
				__tabbar_mouse_move_cb,
				point_down);
		evas_object_event_callback_add(
				ly_main,
				EVAS_CALLBACK_DEL,
				__tabbar_btn_del_cb,
				point_down);
	}
}

EXPORT_PUBLIC void setting_tabbar_disable_swip_effect(
		Evas_Object *ly_main, Evas_Object *tabbar)
{
	SETTING_TRACE_BEGIN;
	if (ly_main) {
		evas_object_data_set(ly_main, "tabbar", tabbar);
		/*Evas_Point *point_down = calloc(1,sizeof(Evas_Point)); */
		evas_object_event_callback_del(
				ly_main,
				EVAS_CALLBACK_MOUSE_DOWN,
				__tabbar_mouse_down_cb);
		evas_object_event_callback_del(
				ly_main,
				EVAS_CALLBACK_MOUSE_UP,
				__tabbar_mouse_up_cb);
		evas_object_event_callback_del(
				ly_main,
				EVAS_CALLBACK_MOUSE_MOVE,
				__tabbar_mouse_move_cb);
		evas_object_event_callback_del(
				ly_main,
				EVAS_CALLBACK_DEL,
				__tabbar_btn_del_cb);
		/* evas_object_event_callback_del(
		 * Evas_Object *obj,
		 * Evas_Callback_Type type,
		 * Evas_Object_Event_Cb func) */
	}
}

EXPORT_PUBLIC void
setting_decorate_image_RGBA(Evas_Object *obj, int r, int g, int b, int a)
{
	if (obj)
		evas_object_color_set(obj, r, g, b, a);
}



EXPORT_PUBLIC void
setting_protect_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	/*SETTING_TRACE("data:%p", data); */
	if (NULL != data) *((void **)data) = NULL;
}

EXPORT_PUBLIC void
setting_add_gl_help(Evas_Object *scroller, const char *str)
{
	if (str) {
		Setting_GenGroupItem_Data *item_data = setting_create_Gendial_field_def(
				scroller,
				&itc_multiline_text,
				NULL,
				NULL,
				SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
				NULL,
				NULL,
				0,
				str,
				NULL,
				NULL);
		if (NULL == item_data) {
			SETTING_TRACE_ERROR("item_data is NULL");
			return;
		}
		elm_genlist_item_select_mode_set(
				item_data->item,
				ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	}
}
