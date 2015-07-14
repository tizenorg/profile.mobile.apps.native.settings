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
#include <efl_assist.h>
#include <notification.h>
#include <notification_internal.h>

#include <Elementary.h>

#define SUPPORT_BACK_KEY_GRAB	0
static void __ignore_back_key_cb(void *data, Evas_Object *obj, void *event_info);

EXPORT_PUBLIC void popup_handle_hardkey(void *data, Evas_Object *popup)
{
	SETTING_TRACE_BEGIN;
	ret_if(!popup);
	evas_object_hide(popup);
	double timeout = elm_popup_timeout_get(popup);
	if (timeout > 0) {
		evas_object_smart_callback_call(popup, "timeout", data);
	} else {
		char *btn_part_str[3] = {
			_("button1"), _("button2"), _("button3")
		};
		int i = 0;
		Evas_Object *btn = NULL;
		Evas_Object *first_btn = NULL;
		for (; i < 3; i++) {
			btn = elm_object_part_content_get(popup, btn_part_str[i]);
			if (btn) {
				first_btn = btn;
				break;
			}
		}

		if (!first_btn) {
			evas_object_smart_callback_call(popup, "block,clicked", data);
		} else {
			const char *btn_str = elm_object_text_get(first_btn);
			if (0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_ONLY_VIBRATE"))
			    || 0 == safeStrCmp(btn_str, _("IDS_ST_BUTTON_OK"))) {
				/* 1. special process for "Vibration" button */
				/* 2. Assistive light popup has only one button, OK. In this case, do not call smart callback. */
				evas_object_del(popup);
			} else {
				evas_object_smart_callback_call(first_btn, "clicked", data);
			}
		}
	}
}


static void __hardkey_mouseup_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Event_Mouse_Up *ev = event_info;
	if (ev->button == 3) { /* if mouse right button is up */
		/*evas_object_hide(obj); // you can call evas_object_del(obj); to remove popup if you want */
		popup_handle_hardkey(data, obj);
	}
}
static void __hardkey_keyup_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	Evas_Event_Key_Down *ev = event_info;
	#if 0
	if (!strcmp(ev->keyname, KEY_BACK)) {
		/*evas_object_hide(obj); // you can call evas_object_del(obj); to remove popup if you want */
		popup_handle_hardkey(data, obj);
	}
	#endif
}

EXPORT_PUBLIC void ea_popup_back_cb_2(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	popup_handle_hardkey(data, obj);
	return;
}


static void __popup_del_cb2(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
	SETTING_TRACE_BEGIN;
	ret_if(!obj);
#if SUPPORT_BACK_KEY_GRAB
	/*ungrab the "back key" */
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get(obj);
	int ret = utilx_ungrab_key(disp, xwin, KEY_BACK);
	if (ret) {
		SETTING_TRACE_ERROR("KEY_BACK ungrab error ret[%d]", ret);
	}
#endif
	evas_object_event_callback_del(obj, EVAS_CALLBACK_KEY_UP, __hardkey_keyup_cb);
	evas_object_event_callback_del(obj, EVAS_CALLBACK_MOUSE_UP, __hardkey_mouseup_cb);
}

EXPORT_PUBLIC void setting_popup_lang_update(Evas_Object *popup)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == popup, "data is NULL");
	char *title = evas_object_data_get(popup, "title");
	char *text1 = evas_object_data_get(popup, "text");
	char *text0 = evas_object_data_get(popup, "text0");
	/*SETTING_TRACE("title:%s", title); */
	/*SETTING_TRACE("text1:%s", text1); */
	/*SETTING_TRACE("text0:%s", text0); */

	char text[MAX_COMMON_BUFFER_LEN + 1] = {0,};
	if (text0) {
		snprintf(text, MAX_COMMON_BUFFER_LEN, "%s<br>%s", _(text0), _(text1));
	} else {
		snprintf(text, MAX_COMMON_BUFFER_LEN, "%s", text1);
	}
	/*char *eo_view_t = evas_object_type_get(popup); */
	if (title)
		elm_object_part_text_set(popup, "title,text", _(title));

	Evas_Object *layout = elm_object_content_get(popup);
	if (layout) {
		Evas_Object *eo_view = elm_object_part_content_get(layout, "elm.swallow.content");
		if (eo_view) {
			const char *eo_view_t = evas_object_type_get(eo_view);
			/*SETTING_TRACE("eo_view_t:%s", eo_view_t); */
			if (!safeStrCmp("elm_gengrid", eo_view_t))
				elm_gengrid_realized_items_update(eo_view);
			else if (!safeStrCmp("elm_genlist", eo_view_t))
				elm_genlist_realized_items_update(eo_view);
			else if (!safeStrCmp("elm_label", eo_view_t))
				if (!isEmptyStr(text)) elm_object_text_set(eo_view, _(text));

		}

		eo_view = elm_object_part_content_get(layout, "elm.swallow.end");
		if (eo_view) {
			const char *eo_view_t = evas_object_type_get(eo_view);
			/*SETTING_TRACE("eo_view_t:%s", eo_view_t); */
			if (!safeStrCmp("elm_check", eo_view_t)) {

				char *check_str = evas_object_data_get(popup, "check_str");
				/*SETTING_TRACE("check_str:%s", check_str); */
				if (check_str) elm_object_text_set(eo_view, _(check_str));
			}
		}
	} else {
		/*SETTING_TRACE("text:%s", text); */
		if (!isEmptyStr(text))
			elm_object_text_set(popup, _(text));
	}

	char *btn_part_str[3] = {
		_("button1"), _("button2"), _("button3")
	};
	int i = 0;
	Evas_Object *btn = NULL;

	/*SETTING_TRACE("button1...:%s", evas_object_data_get(popup, "button1")); */
	/*SETTING_TRACE("button2...:%s", evas_object_data_get(popup, "button2")); */
	/*SETTING_TRACE("button3...:%s", evas_object_data_get(popup, "button3")); */

	for (; i < 3; i++) {
		btn = elm_object_part_content_get(popup, btn_part_str[i]);
		/*SETTING_TRACE("btn:%p", btn); */
		if (btn) {
			/*first_btn = btn; */
			char *btn_str = evas_object_data_get(popup, btn_part_str[i]);
			/*SETTING_TRACE("btn_str:%s", btn_str); */
			elm_object_text_set(btn, _(btn_str));
		}
	}
	SETTING_TRACE_END;
}

static void __popup_lang_change_cb(keynode_t *key, void *data)
{
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	Evas_Object *popup = data;
	char *vconf_name = vconf_keynode_get_name(key);
	if (!safeStrCmp(vconf_name, VCONFKEY_LANGSET)) {
		setting_popup_lang_update(popup);
	}
}

static void __popup_del_cb3(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
	SETTING_TRACE_BEGIN;
	ret_if(!obj);
	(void)vconf_ignore_key_changed(VCONFKEY_LANGSET, __popup_lang_change_cb);
	evas_object_data_set(obj, "title", NULL);
	evas_object_data_set(obj, "text0", NULL);
	evas_object_data_set(obj, "text", NULL);
	evas_object_data_set(obj, "button1", NULL);
	evas_object_data_set(obj, "button2", NULL);
	evas_object_data_set(obj, "button3", NULL);
	evas_object_data_set(obj, "check_str", NULL);
}


#define ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, button1_str, button2_str, button3_str) \
	{\
		evas_object_data_set(popup, "title", title);\
		evas_object_data_set(popup, "text", text);\
		evas_object_data_set(popup, "button1", button1_str);\
		evas_object_data_set(popup, "button2", button2_str);\
		evas_object_data_set(popup, "button3", button3_str);\
		vconf_notify_key_changed(VCONFKEY_LANGSET, __popup_lang_change_cb, popup);\
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb3, NULL);\
	}

EXPORT_PUBLIC void setting_add_hardkey_features(Evas_Object *popup, void *data)
{
	SETTING_TRACE_BEGIN;
	ret_if(!popup);

	int value = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &value);
	if (value) {
		return;
	}

#if SUPPORT_BACK_KEY_GRAB
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get(popup);
	int ret = utilx_grab_key(disp, xwin, KEY_BACK, TOP_POSITION_GRAB);
	if (ret) {
		SETTING_TRACE_ERROR("KEY_BACK grab error ret[%d]", ret);
	}
#endif
	evas_object_event_callback_add(popup, EVAS_CALLBACK_MOUSE_UP, __hardkey_mouseup_cb, data);
	evas_object_event_callback_add(popup, EVAS_CALLBACK_KEY_UP, __hardkey_keyup_cb, data);
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb2, NULL);
}

static void _setting_def_response_cb(void *data, Evas_Object *obj,
                                     void *event_info)
{
	SETTING_TRACE_BEGIN;

	SETTING_TRACE(" REMOVE THE POPUP OBJECT BY THIS FUNCTION ");
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
}

EXPORT_PUBLIC void __popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
	SETTING_TRACE_BEGIN;
#if SUPPORT_BACK_KEY_GRAB
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get(obj);
	int ret = utilx_ungrab_key(disp, xwin, KEY_HOME);
	if (ret) {
		SETTING_TRACE_ERROR("KEY_HOME ungrab error ret[%d]", ret);
	}
#endif
}

/**
* @ set the attribution of the specialized progressbar
* @See setting_create_custom_progressbar.
*/
EXPORT_PUBLIC void __popup_event_set(Evas_Object *popup, void *data,
                                     setting_call_back_func response_cb,
                                     int timeout,/*to control the timeout time */
                                     bool blocked_flag,/*to control whether to block the screen */
                                     bool keygrab_flag/*to control whether to block the 'Home key' */
                                    )
{
	if (timeout > 0) {
		if (response_cb) {
			evas_object_smart_callback_add(popup, "timeout", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "timeout", _setting_def_response_cb, data);
		}
	}

	if (!blocked_flag) {/*not blocked_flag == TRUE !!! */
		if (response_cb) {
			evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "block,clicked", _setting_def_response_cb, data);
		}
	}

	if (keygrab_flag) {
#if SUPPORT_BACK_KEY_GRAB
		Ecore_X_Display *disp = ecore_x_display_get();
		Ecore_X_Window xwin = elm_win_xwindow_get(popup);
		int ret = utilx_grab_key(disp, xwin, KEY_HOME, TOP_POSITION_GRAB);
		if (ret) {
			SETTING_TRACE_ERROR("KEY_HOME grab error ret[%d]", ret);
		}
#endif
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb, NULL);
		ea_object_event_callback_add(popup, EA_CALLBACK_BACK, __ignore_back_key_cb, NULL);
	}
}

static void __ignore_back_key_cb(void *data, Evas_Object *obj,
                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	return;
}


static Evas_Object *__add_progressbar(void *data, Evas_Object *parent,
                                      char *progressbar_style,
                                      char *progressbar_title,
                                      char *progressbar_lable)
{
	SETTING_TRACE_BEGIN;

	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, __ignore_back_key_cb, NULL);
	/*setting_add_hardkey_features(popup, data); */
	int value = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &value);
	if (!value) {
#if SUPPORT_BACK_KEY_GRAB
		Ecore_X_Display *disp = ecore_x_display_get();
		Ecore_X_Window xwin = elm_win_xwindow_get(popup);
		int ret = utilx_grab_key(disp, xwin, KEY_BACK, TOP_POSITION_GRAB);
		if (ret) {
			SETTING_TRACE_ERROR("KEY_BACK grab error ret[%d]", ret);
		}
#endif
		evas_object_event_callback_add(popup, EVAS_CALLBACK_MOUSE_UP, __hardkey_mouseup_cb, data);
		/*evas_object_event_callback_add(popup, EVAS_CALLBACK_KEY_UP, __hardkey_keyup_cb, data); */
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_del_cb2, NULL);
	}

	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (progressbar_title) {
		elm_object_part_text_set(popup, "title,text", _(progressbar_title));
	}

	Evas_Object *box = elm_box_add(popup);
	elm_box_horizontal_set(box, EINA_TRUE);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_align_set(box, 0.5, 0.5);
	elm_box_padding_set(box, 16, 0);

	/* [UI] progress icon */
	if (progressbar_style) {
		Evas_Object *progressbar = elm_progressbar_add(popup);
		elm_object_style_set(progressbar, progressbar_style);	/* "toolbar_process" or "pending_list" or "list_prosess" */
		/*evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5); */
		/*evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
		elm_progressbar_pulse(progressbar, EINA_TRUE);
		evas_object_show(progressbar);
		elm_box_pack_end(box, progressbar);
	}

	/* [UI] text */
	if (progressbar_lable) {
		Evas_Object *label = elm_label_add(popup);
		elm_object_text_set(label, _(progressbar_lable));	/* "Loading..." */
		evas_object_show(label);
		elm_box_pack_end(box, label);
	}
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, progressbar_title, progressbar_lable, NULL, NULL, NULL);
	/* [UI] add table */
	Evas_Object *table = elm_table_add(popup);
	evas_object_show(table);
	elm_table_homogeneous_set(table, EINA_FALSE);

	Evas_Object *rect_up;
	rect_up = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_up, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(20));

	Evas_Object *rect_down;
	rect_down = evas_object_rectangle_add(evas_object_evas_get(popup));
	evas_object_size_hint_min_set(rect_down, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(20));

	/* box */
	elm_table_pack(table, rect_up, 0, 0, 2, 1);/* rect as a padding */
	elm_table_pack(table, box, 0, 1, 2, 1);
	elm_table_pack(table, rect_down, 0, 2, 2, 1);/* rect as a padding */
	evas_object_show(box);
	elm_object_content_set(popup, table);
	return popup;
}


void _popup_show_cb(void *data)
{

	if (data) {
		evas_object_show(data);
	}
}


EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_progressbar_withobject(void *data,
                                                              Evas_Object *popup,
                                                              Evas_Object *parent,
                                                              char *progressbar_style,
                                                              char *title,
                                                              char *text,
                                                              setting_call_back_func response_cb,
                                                              int timeout,
                                                              bool blocked_flag,
                                                              bool keygrab_flag)
{
	SETTING_TRACE_BEGIN;
	if (popup) {
		evas_object_del(popup);
		popup = NULL;
	}

	popup = __add_progressbar(data, parent, progressbar_style,
	                          title, text);
	elm_object_style_set(popup, "no_effect");

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	__popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);
	/*ecore_idler_add(_popup_show_cb, popup); */
	evas_object_show(popup);
	return popup;
}


/**
* create a popup window which contents a progressbar
*
* @return a popup window which contents a progressbar
*/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_progressbar(void *data,
                                                   Evas_Object *parent,
                                                   char *progressbar_style,
                                                   char *title,
                                                   char *text,
                                                   setting_call_back_func response_cb,
                                                   int timeout,
                                                   bool blocked_flag,
                                                   bool keygrab_flag)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = NULL;
	popup = __add_progressbar(data, parent, progressbar_style,
	                          title, text);

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	__popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);
	/*ecore_idler_add(_popup_show_cb, popup); */
	evas_object_show(popup);

	return popup;
}

/**
* create a popup window which contents a progressbar and btn
*
* @return a popup window which contents a progressbar and btn
*/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_progressbar_btn(void *data,
                                                       Evas_Object *parent,
                                                       char *progressbar_style,
                                                       char *title,
                                                       char *text,
                                                       setting_call_back_func response_cb,
                                                       int timeout,
                                                       int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = NULL;
	popup = __add_progressbar(data, parent, progressbar_style,
	                          title, text);

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	if (btn_num > 0) { /*the case popup has button or buttons */
		if (btn_num == 1) {
			va_list args;
			va_start(args, btn_num);

			char *btn_str;
			btn_str = va_arg(args, char *);
			if (safeStrCmp(_(btn_str), _("IDS_ST_BUTTON_CANCEL_ABB")) != 0) {
				Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);
				elm_object_style_set(btn, "popup");
				elm_object_part_content_set(popup, _("button1"), btn);

				evas_object_data_set(popup, "button1", btn_str);
			} else {
				if (response_cb) {
					evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);
				} else {
					evas_object_smart_callback_add(popup, "block,clicked", _setting_def_response_cb, data);
				}
			}
			va_end(args);
		} else {
			va_list args;
			va_start(args, btn_num);

			char *btn_str;
			int argno = 0;
			char *btn_part_str[] = {
				_("button1"), _("button2"), _("button3")
			};

			/*int idx = 0; */
			for (; argno < btn_num; argno++) {
				btn_str = va_arg(args, char *);
				SETTING_TRACE("Parameter #%d is: %s, btn_part_str;%s", argno, btn_str, btn_part_str[argno]);

				if (btn_str) {
					Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);
					elm_object_style_set(btn, "popup");
					elm_object_part_content_set(popup, btn_part_str[argno], btn);
					evas_object_data_set(popup, btn_part_str[argno], btn_str);
				}
			}

			va_end(args);
		}
	} else { /*the case popup doesn't have any button. */
		__popup_event_set(popup, data, response_cb, timeout, FALSE, FALSE);
	}

	evas_object_show(popup);
	return popup;
}

/**
* The general API to create a certain popup window with more than one button.
* @return a certain popup window
*/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_btn(void *data,
                                           Evas_Object *parent,
                                           char *title,
                                           char *text,
                                           setting_call_back_func response_cb,
                                           int timeout,
                                           int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	setting_add_hardkey_features(popup, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
#if 0
	/*popup text's default align is left, add html lable will cause TTS reading incorrect */
	if (text) {
		char r_str_text[HELP_MSG_BUFF_SIZE] = { 0, };
		/**
		if (title)
			snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=left>");
		else
			snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=center>");
		**/
		snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=left>");
		g_strlcat(r_str_text, text, HELP_MSG_BUFF_SIZE);
		g_strlcat(r_str_text, "</align>", HELP_MSG_BUFF_SIZE);
		elm_object_text_set(popup, r_str_text);

	}
#endif
	if (text) {
		elm_object_text_set(popup, _(text));
	}
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, NULL, NULL, NULL);
	SETTING_TRACE("title...:%s", evas_object_data_get(popup, "title"));
	SETTING_TRACE("text...:%s", evas_object_data_get(popup, "text"));

	if (btn_num > 0) { /*the case popup has button or buttons */
		/*currently, most button number is 3*/
		if (btn_num > 3) {
			SETTING_TRACE_ERROR("incorrect button number for popup");
			return NULL;
		} else if (btn_num == 1) {
			va_list args;
			va_start(args, btn_num);

			char *btn_str;
			btn_str = va_arg(args, char *);
			if (safeStrCmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB")) != 0) {
				Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);
				elm_object_style_set(btn, "popup");
				elm_object_part_content_set(popup, _("button1"), btn);
				evas_object_data_set(popup, "button1", btn_str);
			} else {
				if (response_cb) {
					evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);
				} else {
					evas_object_smart_callback_add(popup, "block,clicked", _setting_def_response_cb, data);
				}
			}
			va_end(args);
		} else {
			va_list args;
			va_start(args, btn_num);

			char *btn_str[3];
			int argno = 0;

			char *btn_part_str[] = {
				_("button1"), _("button2"), _("button3")
			};

			int i = 0;
			for (; i < btn_num; i++) {
				btn_str[i] = va_arg(args, char *);
			}

			for (; argno < btn_num; argno++) {
				SETTING_TRACE("Parameter #%d is: %s, btn_part_str;%s", argno, btn_str[btn_num - 1 - argno], btn_part_str[argno]);
				if (btn_str[btn_num - 1 - argno]) {
					Evas_Object *btn = setting_create_button(popup, btn_str[btn_num - 1 - argno], NULL, response_cb, data);
					/*if(!safeStrCmp(btn_str[btn_num-1-argno], _("IDS_ST_BODY_DELETE"))){
						elm_object_style_set(btn,  "style1/delete");
					}
					else{*/
					elm_object_style_set(btn, "popup");
					/*} */
					elm_object_part_content_set(popup, btn_part_str[argno], btn);
					evas_object_data_set(popup, btn_part_str[argno], btn_str[btn_num - 1 - argno]);
				}
			}

			va_end(args);
		}

	} else { /*the case popup doesn't have any button. */
		__popup_event_set(popup, data, response_cb, timeout, FALSE, FALSE);
	}
	SETTING_TRACE("title...:%s", evas_object_data_get(popup, "title"));
	SETTING_TRACE("text...:%s", evas_object_data_get(popup, "text"));
	SETTING_TRACE("button1...:%s", evas_object_data_get(popup, "button1"));
	SETTING_TRACE("button2...:%s", evas_object_data_get(popup, "button2"));
	SETTING_TRACE("button3...:%s", evas_object_data_get(popup, "button3"));

	evas_object_show(popup);
	SETTING_TRACE_END;
	return popup;
}

/**
* The general API to create a certain popup window
*
* @return a certain popup window
*/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_without_btn(void *data,
                                              Evas_Object *parent,
                                              char *title,
                                              char *text,
                                              setting_call_back_func response_cb,
                                              int timeout,
                                              bool blocked_flag,
                                              bool keygrab_flag)
{
	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	setting_add_hardkey_features(popup, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
#if 0
	/*popup text's default align is left, add html lable will cause TTS reading incorrect */
	if (text) {
		char r_str_text[HELP_MSG_BUFF_SIZE] = { 0, };
		/**
		if (title)
			snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=left>");
		else
			snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=center>");
		**/
		snprintf(r_str_text, HELP_MSG_BUFF_SIZE, "<align=left>");
		g_strlcat(r_str_text, text, HELP_MSG_BUFF_SIZE);
		g_strlcat(r_str_text, "</align>", HELP_MSG_BUFF_SIZE);
		elm_object_text_set(popup, r_str_text);

	}
#endif
	if (text) {
		elm_object_text_set(popup, _(text));
	}
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, NULL, NULL, NULL);
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	__popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);
	evas_object_show(popup);
	return popup;
}

/**
* The API to create a certain popup window with title
*
* @return a certain popup window with title
* not used now
*/
EXPORT_PUBLIC
int setting_create_simple_popup(void *data,
                                Evas_Object *parent,
                                char *title,
                                char *text)
{
	setting_create_popup_without_btn(data, parent, title, text,
	                                 NULL, POPUP_INTERVAL, FALSE, FALSE);
	return 0;
}


#define ADD_POPUP_BTN(btn_num, popup, response_cb, data) \
	if (btn_num > 0) {\
		if (btn_num == 1) {\
			va_list args;\
			va_start(args, btn_num);\
			\
			char *btn_str;\
			btn_str = va_arg(args, char *);\
			if (safeStrCmp(btn_str, _("IDS_ST_BUTTON_CANCEL_ABB")) != 0) {\
				Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);\
				elm_object_style_set(btn, "popup");\
				elm_object_part_content_set(popup, _("button1"), btn);\
				evas_object_data_set(popup, "button1", btn_str);\
			} else {\
				if (response_cb) {\
					evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);\
				} else {\
					evas_object_smart_callback_add(popup, "block,clicked", _setting_def_response_cb, data);\
				} \
			} \
			va_end(args);\
		} \
		else {\
			va_list args;\
			va_start(args, btn_num);\
			\
			char *btn_str;\
			int argno = 0;\
			char *btn_part_str[] = {\
			                        _("button1"), _("button2"), _("button3")\
			                       };\
			\
			for (; argno < btn_num; argno++) {\
				btn_str = va_arg(args, char *);\
				SETTING_TRACE("Parameter #%d is: %s, btn_part_str;%s", argno, btn_str, btn_part_str[argno]);\
				if (btn_str) {\
					Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);\
					elm_object_style_set(btn, "popup");\
					elm_object_part_content_set(popup, btn_part_str[argno], btn);\
					evas_object_data_set(popup, btn_part_str[argno], btn_str);\
				} \
			} \
			va_end(args);\
		} \
	} else {\
		if (response_cb) {\
			evas_object_smart_callback_add(popup, "timeout", response_cb, data);\
			evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);\
		} else {\
			evas_object_smart_callback_add(popup, "timeout", _setting_def_response_cb, data);\
			evas_object_smart_callback_add(popup, "block,clicked", _setting_def_response_cb, data);\
		} \
	}

/*1Title-1Description-1Gif-nButton */
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
#if 0
static void __gif_popup_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	evas_object_data_set(obj, "popup_type", NULL);
}
#endif

EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_gif(void *data,
                                           Evas_Object *parent,
                                           char *title,
                                           char *text,
                                           const char **png_list,	/*array end with 'NULL'. */
                                           setting_call_back_func response_cb,
                                           int timeout,
                                           int btn_num, ...)
{
	SETTING_TRACE_BEGIN;

#if 0
	/*displaying image */
	Evas_Object *image = NULL;
	if (png_list && *png_list) {
		if (png_list[1]) {	/*png_list is an array end with 'NULL', and it has at least 2 png file, */
			image = setting_create_gif(parent, png_list);
		} else {	/*png_list just has one png file, */
			image = setting_create_image(parent, *png_list);
		}
	}

	Evas_Object *popup = elm_popup_add(parent);
	evas_object_show(popup);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	/*title */
	if (title) {
		elm_object_part_text_set(popup, "title,text", title);
	}

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	/*content */
	Evas_Object *content_box = elm_box_add(popup);
	/*instructing of displaying image */
	if (text) {
		Evas_Object *lable = setting_create_lable(popup, text,
		                                          "entry", "middle");
		elm_box_pack_end(content_box, lable);
	}
	/*displaying image */
	if (image) {
		elm_box_pack_end(content_box, image);
	}


	app_device_orientation_e m = elm_win_rotation_get(parent);
	SETTING_TRACE("m:%d", m);
	if (APP_DEVICE_ORIENTATION_90 == m || APP_DEVICE_ORIENTATION_270 == m) {
		Evas_Object *scroller = elm_scroller_add(parent);
		elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
		elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
		evas_object_show(scroller);
		elm_object_content_set(scroller, content_box);

		Evas_Object *checkview = elm_layout_add(parent);
		elm_layout_file_set(checkview, SETTING_THEME_EDJ_NAME, "popup_checkview_image");
		evas_object_size_hint_weight_set(checkview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_object_part_content_set(checkview, "elm.swallow.content", scroller);

		evas_object_data_set(popup, "popup_type", "scroller");
		/*set box into popup */
		elm_object_content_set(popup, checkview);
	} else {
		evas_object_data_set(popup, "popup_type", "auto_expand");
		elm_object_content_set(popup, content_box);
	}
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __gif_popup_del_cb, NULL);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);
	evas_object_show(popup);
#else
	Evas_Object *scroller = elm_genlist_add(parent);
	retvm_if(scroller == NULL, NULL, "Cannot set scroller object  as contento of layout");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller);	/* first to clear list */
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	ADD_GL_LABLE(scroller, text);

	SETTING_TRACE("png_list:%p", png_list);
	ADD_GL_GIF(scroller, png_list)

	/*elm_object_content_set(popup, scroller); */

	Evas_Object *popup = setting_create_popup_with_checkview(parent, title, NULL, scroller,
	                                                         "popup_checkview_image", NULL, timeout);

	setting_add_hardkey_features(popup, data);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	/*evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL); */
	/*evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); */
	/*evas_object_size_hint_min_set(scroller, ELM_SCALE_SIZE(480), ELM_SCALE_SIZE(700)); */

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);
	evas_object_show(popup);
#endif
	return popup;

}

EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_checkview(Evas_Object *parent,
                                                 char *title,
                                                 char *check_str,
                                                 Evas_Object *checkview_content,
                                                 const char *checkview_style,
                                                 Evas_Object **check,
                                                 int timeout)
{
	Evas_Object *popup = elm_popup_add(parent);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND,
	                                 EVAS_HINT_EXPAND);
	/*title */
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	/*set box into popup */

	Evas_Object *checkview = elm_layout_add(parent);
	elm_layout_file_set(checkview, SETTING_THEME_EDJ_NAME, checkview_style);
	evas_object_size_hint_weight_set(checkview, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/*instructing of displaying image */
	if (check_str) {
		edje_object_part_text_set(elm_layout_edje_get(checkview), "elm.text", _(check_str));
	}
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, NULL, NULL, NULL, NULL);
	evas_object_data_set(popup, "check_str", check_str);
	if (check) {
		*check = elm_check_add(popup);
		evas_object_size_hint_align_set(*check, EVAS_HINT_FILL,
		                                EVAS_HINT_FILL);
		evas_object_size_hint_weight_set(*check, EVAS_HINT_EXPAND,
		                                 EVAS_HINT_EXPAND);
		evas_object_show(*check);
		elm_object_part_content_set(checkview, "elm.swallow.end", *check);
	}
	/*content */
	if (checkview_content) {
		elm_object_part_content_set(checkview, "elm.swallow.content", checkview_content);
	}

	Evas_Object *sub_layout = elm_layout_add(popup);
	elm_layout_file_set(sub_layout, SETTING_THEME_EDJ_NAME, "screen_mode");
	evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, 0.0);
	evas_object_show(sub_layout);
	elm_object_part_content_set(sub_layout, "content", checkview);

	elm_object_content_set(popup, sub_layout);
	return popup;
}

EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_label_and_reverse_btn(void *data,
                                                             Evas_Object *parent,
                                                             char *title,
                                                             char *text,
                                                             setting_call_back_func response_cb,
                                                             int timeout,
                                                             int btn_num, ...)
{
	Evas_Object *popup;
	/*struct appdata *ad = (struct appdata *) data; */

	popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_text_set(popup, _(text));
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, NULL, text, NULL, NULL, NULL);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);

	evas_object_show(popup);

	return popup;
}

EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_label_check(void *data,
                                                   Evas_Object *parent,
                                                   char *title,
                                                   char *content,
                                                   char *check_str,
                                                   Evas_Object **check,
                                                   setting_call_back_func response_cb,
                                                   int timeout,
                                                   int btn_num, ...)
{
	Evas_Object *popup = elm_popup_add(parent);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	setting_add_hardkey_features(popup, data);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	Evas_Object *label = NULL;
	label = elm_label_add(popup);
	elm_label_line_wrap_set(label, ELM_WRAP_MIXED);

	char arr[2048] = {'\0',};
	snprintf(arr, 2048, "<color=#000000>%s</color>", _(content));
	elm_object_text_set(label, arr);

	evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_show(label);

	Evas_Object *layout = elm_layout_add(popup);
	elm_layout_file_set(layout, SETTING_THEME_EDJ_NAME, "popup_checkview");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (check) {
		*check = elm_check_add(popup);
		elm_object_style_set(*check, "popup");
		elm_object_text_set(*check, _(check_str));
		evas_object_show(*check);

		Evas_Object *box = elm_box_add(popup);
		elm_box_horizontal_set(box, EINA_TRUE);
		elm_box_pack_end(box, *check);

		evas_object_show(box);
		elm_object_part_content_set(layout, "elm.swallow.end", box);
	}

	elm_object_part_content_set(layout, "elm.swallow.content", label);

	elm_object_content_set(popup, layout);
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, content, NULL, NULL, NULL);
	evas_object_data_set(popup, "check_str", check_str);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);
	evas_object_show(popup);
	return popup;
}


/**
* The API to create a certain popup window with a image, a check and some buttons
* @return a certain popup window with title
*/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_image_check(void *data,
                                                   Evas_Object *parent,
                                                   char *title,
                                                   char *text1,
                                                   char *text2,
                                                   const char **png_list,	/*array end with 'NULL'. */
                                                   char *check_str,
                                                   Evas_Object **check,
                                                   setting_call_back_func response_cb,
                                                   int timeout,
                                                   int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
#if 0
	Evas_Object *lable = NULL;

	Evas_Object *inner_layout = elm_layout_add(parent);
	elm_layout_file_set(inner_layout, SETTING_THEME_EDJ_NAME, "popup_smartscreen_internal");
	evas_object_size_hint_weight_set(inner_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (text1) {
		lable = setting_create_lable(inner_layout, text1, "entry", "left");
		elm_object_part_content_set(inner_layout, "elm.swallow.content1", lable);
	}

	Evas_Object *image = NULL;
	if (png_list && *png_list) {
		if (png_list[1]) {	/*png_list is an array end with 'NULL', and it has at least 2 png file, */
			image = setting_create_gif(parent, png_list);
		} else {	/*png_list just has one png file, */
			image = setting_create_image(parent, *png_list);
		}

		elm_object_part_content_set(inner_layout, "elm.swallow.content2", image);
	}

	if (text2) {
		lable = setting_create_lable(inner_layout, text2, "entry", "left");
		elm_object_part_content_set(inner_layout, "elm.swallow.content3", lable);
	}

	Evas_Object *scroller = elm_scroller_add(parent);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, 0);
	elm_scroller_bounce_set(scroller, EINA_FALSE, EINA_TRUE);
	elm_scroller_policy_set(scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
	evas_object_show(scroller);
	elm_object_content_set(scroller, inner_layout);
#endif
	Evas_Object *scroller = elm_genlist_add(parent);
	retvm_if(scroller == NULL, NULL, "Cannot set scroller object  as contento of layout");
	//elm_genlist_realization_mode_set(scroller, EINA_TRUE);
	elm_object_style_set(scroller, "dialogue");
	elm_genlist_clear(scroller);	/* first to clear list */
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	ADD_GL_LABLE(scroller, text1);

	SETTING_TRACE("png_list:%p", png_list);
	ADD_GL_GIF(scroller, png_list)
	ADD_GL_LABLE(scroller, text2);
	/*ADD_GL_BUTTON(scroller, content_button_str, content_button_click_cb, cb_data) */
	Evas_Object *popup = setting_create_popup_with_checkview(parent, title, check_str, scroller,
	                                                         "popup_checkview_image", check, timeout);
	setting_add_hardkey_features(popup, data);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);
	evas_object_show(popup);
	return popup;
}

EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_list(Evas_Object **genlist, void *data, Evas_Object *parent, const char *title,
                                            setting_call_back_func response_cb, int timeout,
                                            int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	retv_if(NULL == parent, NULL);
	Evas_Object *popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_object_part_text_set(popup, "title,text", _(title));
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, NULL, NULL, NULL, NULL);

	evas_object_show(popup);

	/* genlist */
	Evas_Object *scroller = elm_genlist_add(popup);
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);/*essential to auto compute the height of genlist */
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_content_min_limit(scroller, EINA_FALSE, EINA_TRUE);/*essential to auto compute the height of genlist */
	evas_object_show(scroller);

	if (genlist) *genlist = scroller;

	elm_object_content_set(popup, scroller);
	setting_add_hardkey_features(popup, data);
	ea_object_event_callback_add(popup, EA_CALLBACK_BACK, ea_popup_back_cb_2, data);
	return popup;
}

/**
* Create selectinfo popup
*/
EXPORT_PUBLIC
void setting_create_sel_info_popup(Evas_Object *parent,
                                   Evas_Object **selInfoPop_layout,
                                   Evas_Object **selInfoPop)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("*selInfoPop:%p", *selInfoPop);
	if (!(*selInfoPop)) {
		/*  Add notify */
		*selInfoPop = elm_notify_add(parent);
		elm_notify_align_set(*selInfoPop, ELM_NOTIFY_ALIGN_FILL, 1.0);
		elm_notify_timeout_set(*selInfoPop, 3);

		/*  list_data->selInfoPop layout add */
		*selInfoPop_layout = elm_layout_add(*selInfoPop);
		elm_layout_theme_set(*selInfoPop_layout, "standard", "selectioninfo", "center_text");

		/*  list_data->selInfoPop layout content set to notify */
		elm_object_content_set(*selInfoPop, *selInfoPop_layout);
	}

	evas_object_show(*selInfoPop);
}

EXPORT_PUBLIC
void setting_create_select_info_popup(Evas_Object **popup, Evas_Object *parent, double timeout, char *content_text, char *style)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(parent == NULL, "parent is NULL");
	/*SETTING_TRACE("*popup:%p", *popup); */
	if (!(*popup)) {
		*popup = elm_notify_add(parent);
		elm_notify_align_set(*popup, ELM_NOTIFY_ALIGN_FILL, 1.0);

		Evas_Object *selectioninfo_layout = NULL;
		selectioninfo_layout = elm_layout_add(*popup);
		elm_object_content_set(*popup, selectioninfo_layout);

		elm_notify_timeout_set(*popup, timeout);

		Evas_Object *content_obj = elm_object_content_get(*popup);
		/* Set the layout theme */
		elm_layout_theme_set(content_obj, "standard", "selectioninfo", style);

		char text[MAX_DISPLAY_NAME_LEN_ON_UI] = {0,};
		snprintf(text, sizeof(text), "%s", content_text);
		elm_object_part_text_set(content_obj, "elm.text", text);
	}
	evas_object_show(*popup);
}

/*The API is going to register a message to NOTIFICATION_STATUS_MESSAGE_KEY, and the message will be shown in indicator
* @ Quickpannel App save the value of var message, and it supply notification_status_monitor_message_cb_set,
* @ in which, Indicator can set process of vconf(NOTIFICATION_STATUS_MESSAGE_KEY) change event,in the call __message_callback,
* @ it will invoke indicator_part_text_emit_by_win(win,"message.text.compare", text); to show message you registered
*/
EXPORT_PUBLIC void setting_create_indicator_notification(const char *message)
{
	notification_status_message_post(message);
}
