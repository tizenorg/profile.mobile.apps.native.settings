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
#include <notification.h>
#include <notification_internal.h>

#define ADD_POPUP_BTN(btn_num, popup, response_cb, data) \
	if (btn_num > 0) { /*the case popup has button or buttons */\
		if (btn_num == 1) {\
			va_list args;\
			va_start(args, btn_num);\
			\
			char *btn_str;\
			btn_str = va_arg(args, char *);\
			\
			Evas_Object *btn = setting_create_button(popup, btn_str, NULL, response_cb, data);\
			elm_object_style_set(btn, "popup");\
			elm_object_part_content_set(popup, _("button1"), btn);\
			evas_object_data_set(popup, "button1", btn_str);\
			va_end(args);\
		} else if (btn_num == 2) {\
			va_list args;\
			va_start(args, btn_num);\
			\
			char *btn_str[3];\
			int argno = 0;\
			\
			char *btn_part_str[] = {\
				_("button1"), _("button2"), _("button3")\
			};\
			\
			int i = 0;\
			for (; i < btn_num; i++) {\
				btn_str[i] = va_arg(args, char *);\
			}\
			\
			for (; argno < btn_num; argno++) {\
				SETTING_TRACE("Parameter #%d is: %s, btn_part_str;%s", argno, btn_str[btn_num - 1 - argno], btn_part_str[argno]);\
				if (btn_str[btn_num - 1 - argno]) {\
					Evas_Object *btn = setting_create_button(popup, btn_str[btn_num - 1 - argno], NULL, response_cb, data);\
					elm_object_style_set(btn, "popup");\
					elm_object_part_content_set(popup, btn_part_str[argno], btn);\
					evas_object_data_set(popup, btn_part_str[argno], btn_str[btn_num - 1 - argno]);\
				}\
			}\
			va_end(args);\
		} else { /* btn_num > 3 */\
			SETTING_TRACE_ERROR("incorrect button number for popup");\
			return NULL;\
		}\
	} else if (btn_num == 0) {\
		SETTING_TRACE("btn_num is zero");\
	} else { /* btn_num < 0 */\
		SETTING_TRACE_ERROR("incorrect button number for popup");\
		return NULL;\
	}\

#define ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, button1_str, button2_str, button3_str) \
	evas_object_data_set(popup, "title", title);\
	evas_object_data_set(popup, "text", text);\
	evas_object_data_set(popup, "button1", button1_str);\
	evas_object_data_set(popup, "button2", button2_str);\
	evas_object_data_set(popup, "button3", button3_str);\
	vconf_notify_key_changed(VCONFKEY_LANGSET, _popup_lang_change_cb, popup);\
	evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, _popup_lang_del_cb, NULL);\

EXPORT_PUBLIC void __setting_popup_lang_update(Evas_Object *popup)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == popup, "data is NULL");
	char *title = evas_object_data_get(popup, "title");
	char *text1 = evas_object_data_get(popup, "text");
	char *text0 = evas_object_data_get(popup, "text0");
	char text[MAX_COMMON_BUFFER_LEN + 1] = {0,};

	if (text0) {
		snprintf(text, MAX_COMMON_BUFFER_LEN, "%s<br>%s", _(text0), _(text1));
	} else {
		snprintf(text, MAX_COMMON_BUFFER_LEN, "%s", text1);
	}

	if (title)
		elm_object_part_text_set(popup, "title,text", _(title));

	Evas_Object *layout = elm_object_content_get(popup);
	if (layout) {
		Evas_Object *eo_view = elm_object_part_content_get(layout, "elm.swallow.content");
		if (eo_view) {
			const char *eo_view_t = evas_object_type_get(eo_view);
			/*SETTING_TRACE("eo_view_t:%s", eo_view_t); */
			if (!safeStrCmp("elm_genlist", eo_view_t))
				elm_genlist_realized_items_update(eo_view);
			else if (!safeStrCmp("elm_label", eo_view_t))
				if (!isEmptyStr(text))
					elm_object_text_set(eo_view, _(text));
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
			char *btn_str = evas_object_data_get(popup, btn_part_str[i]);
			/*SETTING_TRACE("btn_str:%s", btn_str); */
			elm_object_text_set(btn, _(btn_str));
		}
	}
	SETTING_TRACE_END;
}

static void _popup_lang_change_cb(keynode_t *key, void *data)
{
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	Evas_Object *popup = data;
	char *vconf_name = vconf_keynode_get_name(key);
	if (!safeStrCmp(vconf_name, VCONFKEY_LANGSET)) {
		__setting_popup_lang_update(popup);
	}
}

static void _popup_lang_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!obj);
	(void)vconf_ignore_key_changed(VCONFKEY_LANGSET, _popup_lang_change_cb);
	evas_object_data_set(obj, "title", NULL);
	evas_object_data_set(obj, "text0", NULL);
	evas_object_data_set(obj, "text", NULL);
	evas_object_data_set(obj, "button1", NULL);
	evas_object_data_set(obj, "button2", NULL);
	evas_object_data_set(obj, "button3", NULL);
	evas_object_data_set(obj, "check_str", NULL);
}

EXPORT_PUBLIC void setting_popup_del_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE(" REMOVE THE POPUP OBJECT BY THIS FUNCTION ");
	if (obj) {
		evas_object_del(obj);
		obj = NULL;
	}
}

EXPORT_PUBLIC void __popup_keygrab_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)

{
	SETTING_TRACE_BEGIN;

#ifdef ECORE_X
	int ret = -1;
	Ecore_X_Display *disp = ecore_x_display_get();
	Ecore_X_Window xwin = elm_win_xwindow_get(obj);

	/*ungrab the "Home key" */
	ret = eext_win_keygrab_unset(xwin, "XF86Home");
	if (ret) {
		SETTING_TRACE_ERROR("KEY_HOME ungrab error ret[%d]", ret);
	}

	/*ungrab the "back key" */
	ret = eext_win_keygrab_unset(xwin, "XF86Back");
	if (ret) {
		SETTING_TRACE_ERROR("KEY_BACK ungrab error ret[%d]", ret);
	}
#endif
}

static void __ignore_back_key_cb(void *data, Evas_Object *obj,
                                 void *event_info)
{
	SETTING_TRACE_BEGIN;
	return;
}

EXPORT_PUBLIC void setting_popup_event_set2(Evas_Object *popup, void *data,
                                     setting_call_back_func response_cb,
                                     setting_call_back_func back_cb,
                                     int timeout,/*to control the timeout time */
                                     bool blocked_flag,/*to control whether to block the screen */
                                     bool keygrab_flag/*to control whether to block the 'Home key' */
                                    )
{
	if (timeout > 0) {
		if (response_cb) {
			evas_object_smart_callback_add(popup, "timeout", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "timeout", setting_popup_del_cb, data);
		}
	}

	if (!blocked_flag) {/* blocked_flag == FALSE */
		if (response_cb) {
			evas_object_smart_callback_add(popup, "block,clicked", response_cb, data);
		} else {
			evas_object_smart_callback_add(popup, "block,clicked", back_cb, data);
		}
	}

	if (keygrab_flag) {
#ifdef ECORE_X
		Ecore_X_Display *disp = ecore_x_display_get();
		Ecore_X_Window xwin = elm_win_xwindow_get(popup);

		int ret = eext_win_keygrab_set(xwin, "XF86Home");
		if (ret) {
			SETTING_TRACE_ERROR("KEY_HOME grab error ret[%d]", ret);
		}
#endif
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_keygrab_del_cb, NULL);
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, __ignore_back_key_cb, NULL);
	} else {
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, back_cb, data);
	}
}



EXPORT_PUBLIC void setting_popup_event_set(Evas_Object *popup, void *data,
                                     setting_call_back_func response_cb,
                                     int timeout,/*to control the timeout time */
                                     bool blocked_flag,/*to control whether to block the screen */
                                     bool keygrab_flag/*to control whether to block the 'Home key' */
                                    )
{
	setting_popup_event_set2(popup, data,
			response_cb,
			setting_popup_del_cb,
			timeout,
			blocked_flag,
			keygrab_flag);
}


EXPORT_PUBLIC
Evas_Object *setting_create_popup2(void *data,
                                  Evas_Object *parent,
                                  char *title,
                                  char *text,
                                  setting_call_back_func response_cb,
                                  setting_call_back_func back_cb,
                                  int timeout,
                                  bool blocked_flag,
                                  bool keygrab_flag,
                                  int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (text) {
		elm_object_text_set(popup, _(text));
	}
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	setting_popup_event_set2(popup, data, response_cb, back_cb, timeout, blocked_flag, keygrab_flag);

	if (keygrab_flag) {
#ifdef ECORE_X
		Ecore_X_Display *disp = ecore_x_display_get();
		Ecore_X_Window xwin = elm_win_xwindow_get(popup);

		int ret = eext_win_keygrab_set(xwin, "XF86Home");
		if (ret) {
			SETTING_TRACE_ERROR("KEY_HOME grab error ret[%d]", ret);
		}
#endif
		evas_object_event_callback_add(popup, EVAS_CALLBACK_DEL, __popup_keygrab_del_cb, NULL);
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, __ignore_back_key_cb, NULL);
	} else {
		eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, back_cb, data);
	}

	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, NULL, NULL, NULL);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);

	SETTING_TRACE("title...:%s", evas_object_data_get(popup, "title"));
	SETTING_TRACE("text...:%s", evas_object_data_get(popup, "text"));
	SETTING_TRACE("button1...:%s", evas_object_data_get(popup, "button1"));
	SETTING_TRACE("button2...:%s", evas_object_data_get(popup, "button2"));
	SETTING_TRACE("button3...:%s", evas_object_data_get(popup, "button3"));

	evas_object_show(popup);
	SETTING_TRACE_END;
	return popup;
}



/*********************************************************
* @brief The general API to create a default popup window
*
* @param data			application context
* @param parent			parent window
* @param title			popup title
* @param text			popup text
* @param response_cb	called When btn was clicked
* @param timeout		timeout sec
* @param blocked_flag	to control whether to block the screen
* @param keygrab_flag	to control whether to block key
* @param btn_num		btn number
* @param ...			btn text
*
* @return a certain popup window
**********************************************************/
EXPORT_PUBLIC
Evas_Object *setting_create_popup(void *data,
                                  Evas_Object *parent,
                                  char *title,
                                  char *text,
                                  setting_call_back_func response_cb,
                                  int timeout,
                                  bool blocked_flag,
                                  bool keygrab_flag,
                                  int btn_num, ...)
{
	SETTING_TRACE_BEGIN;

	Evas_Object *popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (text) {
		elm_object_text_set(popup, _(text));
	}
	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	setting_popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);

	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, text, NULL, NULL, NULL);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);

	SETTING_TRACE("title...:%s", evas_object_data_get(popup, "title"));
	SETTING_TRACE("text...:%s", evas_object_data_get(popup, "text"));
	SETTING_TRACE("button1...:%s", evas_object_data_get(popup, "button1"));
	SETTING_TRACE("button2...:%s", evas_object_data_get(popup, "button2"));
	SETTING_TRACE("button3...:%s", evas_object_data_get(popup, "button3"));

	evas_object_show(popup);
	SETTING_TRACE_END;
	return popup;
}

/******************************************************************
* @brief The general API to create a popup window which contents a progressbar
*
* @param data				application context
* @param parent 				parent window
* @param progressbar_style		progressbar style(process_small/process_medium/process_large)
* @param title					popup title
* @param text				popup text
* @param response_cb			called When btn was clicked
* @param timeout				timeout sec
* @param blocked_flag			to control whether to block the screen
* @param keygrab_flag			to control whether to block key
*
* @return a popup window which contents a progressbar
*******************************************************************/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_progressbar(void *data,
                                                   Evas_Object *parent,
                                                   char *progressbar_style,
                                                   char *title,
                                                   char *text,
                                                   setting_call_back_func response_cb,
                                                   int timeout,
                                                   bool blocked_flag,
                                                   bool keygrab_flag,
												   int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *popup = NULL;
	Evas_Object *layout;
	Evas_Object *progressbar;

	popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	/* layout */
	layout = elm_layout_add(popup);
	elm_layout_file_set(layout, SETTINGS_EDJ, "processing_view_layout");
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	if (title)
		elm_object_part_text_set(popup, "title,text", _(title));
	if (text)
		elm_object_part_text_set(layout, "elm.text", _(text));

	progressbar = elm_progressbar_add(layout);

	if (!safeStrCmp("default", progressbar_style)) {
		// do nothing
	} else {
		elm_object_style_set(progressbar, progressbar_style);
	}

	evas_object_size_hint_align_set(progressbar, EVAS_HINT_FILL, 0.5);
	evas_object_size_hint_weight_set(progressbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_progressbar_pulse(progressbar, EINA_TRUE);
	elm_object_part_content_set(layout, "processing", progressbar);

	elm_object_content_set(popup, layout);

	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}
	setting_popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);

	ADD_POPUP_BTN(btn_num, popup, response_cb, data);

	SETTING_TRACE("title...:%s", evas_object_data_get(popup, "title"));
	SETTING_TRACE("text...:%s", evas_object_data_get(popup, "text"));
	SETTING_TRACE("button1...:%s", evas_object_data_get(popup, "button1"));
	SETTING_TRACE("button2...:%s", evas_object_data_get(popup, "button2"));
	SETTING_TRACE("button3...:%s", evas_object_data_get(popup, "button3"));

	evas_object_show(popup);

	return popup;
}

/******************************************************************
* @brief The general API to create a popup window which contents a genlist
*
* @param genlist				genlist
* @param data				application context
* @param parent 				parent window
* @param title					popup title
* @param response_cb			called When btn was clicked
* @param timeout				timeout sec
* @param blocked_flag			to control whether to block the screen
* @param keygrab_flag			to control whether to block key
*
* @return a popup window which contents a genlist
*******************************************************************/
EXPORT_PUBLIC
Evas_Object *setting_create_popup_with_list(Evas_Object **genlist,
												void *data,
												Evas_Object *parent,
												const char *title,
												setting_call_back_func response_cb,
												int timeout,
												bool blocked_flag,
												bool keygrab_flag,
												int btn_num, ...)
{
	SETTING_TRACE_BEGIN;
	retv_if(NULL == parent, NULL);
	Evas_Object *popup = elm_popup_add(parent);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

	if (title) {
		elm_object_part_text_set(popup, "title,text", _(title));
	}
	if (timeout > 0) {
		elm_popup_timeout_set(popup, timeout);
	}

	setting_popup_event_set(popup, data, response_cb, timeout, blocked_flag, keygrab_flag);
	ADD_POPUP_MULTI_LANGUAGE_AUTO_UPDATE(popup, title, NULL, NULL, NULL, NULL);
	ADD_POPUP_BTN(btn_num, popup, response_cb, data);

	evas_object_show(popup);

	/* genlist */
	Evas_Object *scroller = elm_genlist_add(popup);
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);/*essential to auto compute the height of genlist */
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_scroller_content_min_limit(scroller, EINA_FALSE, EINA_TRUE);/*essential to auto compute the height of genlist */
	evas_object_show(scroller);

	if (genlist) {
		*genlist = scroller;
	}

	elm_object_content_set(popup, scroller);

	SETTING_TRACE_END;
	return popup;
}

