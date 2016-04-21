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
#include <Elementary.h>
#include <efl_extension.h>

extern Eina_List *elm_widget_scrollable_children_get(Evas_Object *obj);

#define HALF_SCREEN_STR_LEN 20
#define ONE_THIRD_SCREEN_STR_LEN 13

#if 0
#define ADD_MULTI_LANGUAGE_AUTO_UPDATE(eo_view, navi_it, title_str, lbutton_str, mbutton_str, rbutton_str) \
	{\
		evas_object_data_set(eo_view, "navi_it", navi_it);\
		evas_object_data_set(eo_view, "navi_title", title_str);\
		evas_object_data_set(eo_view, "lbutton_str", lbutton_str);\
		evas_object_data_set(eo_view, "mbutton_str", mbutton_str);\
		evas_object_data_set(eo_view, "rbutton_str", rbutton_str);\
		/*vconf_notify_key_changed(VCONFKEY_LANGSET, __navi_eo_view_lang_change_cb, eo_view);*/\
		evas_object_event_callback_add(eo_view, EVAS_CALLBACK_DEL, __eo_view_del_cb, NULL);\
	}
#endif

/*
static void __navi_eo_view_lang_change_cb(keynode_t *key, void *data)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	Evas_Object *eo_view = data;
	char *vconf_name = vconf_keynode_get_name(key);
	if (!safeStrCmp(vconf_name, VCONFKEY_LANGSET))
	{
		Elm_Object_Item *navi_it = evas_object_data_get(eo_view, "navi_it");
		char *navi_title = evas_object_data_get(eo_view, "navi_title");
		char *lbutton_str = evas_object_data_get(eo_view, "lbutton_str");
		char *mbutton_str = evas_object_data_get(eo_view, "mbutton_str");
		char *rbutton_str = evas_object_data_get(eo_view, "rbutton_str");
		const char *eo_view_t = evas_object_type_get(eo_view);
		SETTING_TRACE("navi_title  [%s]:%s", _(navi_title), navi_title);
		SETTING_TRACE("eo_view_t:%s", eo_view_t);
		SETTING_TRACE("lbutton_str:%s", lbutton_str);
		SETTING_TRACE("mbutton_str:%s", mbutton_str);
		SETTING_TRACE("rbutton_str:%s", rbutton_str);
		if (!safeStrCmp("elm_gengrid", eo_view_t))
			elm_gengrid_realized_items_update(eo_view);
		else if(!safeStrCmp("elm_genlist", eo_view_t))
			elm_genlist_realized_items_update(eo_view);

		if (navi_it)
		{
			SETTING_TRACE("Update navi_item, navi_title:%s", navi_title);
			if (navi_title)
				elm_object_item_text_set(navi_it, _(navi_title));

			//l,m,r
			Evas_Object *toolbar = elm_object_item_part_content_get(navi_it, "toolbar");

			char *btn_str[3] = {0, };
			int idx = 0;
			if (lbutton_str && 0 != safeStrCmp(_(lbutton_str), _("IDS_ST_BUTTON_BACK")))
				btn_str[idx++] = g_strdup(lbutton_str);

			if (rbutton_str)
				btn_str[idx++] = g_strdup(rbutton_str);

			if (mbutton_str)
				btn_str[idx++] = g_strdup(mbutton_str);


			Elm_Object_Item *it = NULL;
			Elm_Object_Item *next_it = NULL;
			//char *text = NULL;
			it = elm_toolbar_first_item_get(toolbar);
			idx = 0;
			while (it) {
				next_it = elm_toolbar_item_next_get(it);
				//text = (char *)elm_object_item_text_get(it);
				elm_object_item_text_set(it, _(btn_str[idx]));
				idx++;
				it = next_it;
			}
		}
	}
}
*/

#if 0
static void __eo_view_del_cb(void *data, Evas *e, Evas_Object *eo_view, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!eo_view);
	/*(void)vconf_ignore_key_changed(VCONFKEY_LANGSET, __navi_eo_view_lang_change_cb); */
	evas_object_data_set(eo_view, "navi_it", NULL);
	evas_object_data_set(eo_view, "navi_title", NULL);
	evas_object_data_set(eo_view, "lbutton_str", NULL);
	evas_object_data_set(eo_view, "mbutton_str", NULL);
	evas_object_data_set(eo_view, "rbutton_str", NULL);

	SETTING_TRACE_END;
}

#endif


EXPORT_PUBLIC
void setting_navi_items_update(Evas_Object *navigate_bar)
{
	SETTING_TRACE_BEGIN;
	/*setting_retm_if(NULL == key, "key is NULL"); */
	setting_retm_if(NULL == navigate_bar, "data is NULL");
	Eina_List *list = elm_naviframe_items_get(navigate_bar);
	Evas_Object *eo_view = NULL;
	Evas_Object *btn = NULL;
	char *btn_text = NULL;

	while (list) {
		Elm_Object_Item *item = (Elm_Object_Item *) eina_list_data_get(list);
		if (NULL == item) {
			SETTING_TRACE("item is null");
			list = eina_list_next(list);
			continue;
		}

		btn = elm_object_item_part_content_get(item, "title_right_btn");
		if (btn) {
			btn_text = evas_object_data_get(btn, "text");
			if (btn_text)
				elm_object_text_set(btn, _(btn_text));
		}

		btn = elm_object_item_part_content_get(item, "title_left_btn");
		if (btn) {
			btn_text = evas_object_data_get(btn, "text");
			if (btn_text)
				elm_object_text_set(btn, _(btn_text));
		}

		eo_view = elm_object_item_content_get(item);
		if (NULL == eo_view) {
			SETTING_TRACE("eo_view is null");
			list = eina_list_next(list);
			continue;
		}

		Elm_Object_Item *navi_it = evas_object_data_get(eo_view, "navi_it");
		if (navi_it != item) {
			SETTING_TRACE("navi_it is null");
			list = eina_list_next(list);
			continue;
		}

		const char *navi_title = evas_object_data_get(eo_view, "navi_title");
		const char *lbutton_str = evas_object_data_get(eo_view, "lbutton_str");
		const char *mbutton_str = evas_object_data_get(eo_view, "mbutton_str");
		const char *rbutton_str = evas_object_data_get(eo_view, "rbutton_str");
		const char *eo_view_t = evas_object_type_get(eo_view);
		/*SETTING_TRACE("navi_title  [%s]:%s", _(navi_title), navi_title); */
		/*SETTING_TRACE("eo_view_t:%s", eo_view_t); */
		/*SETTING_TRACE("lbutton_str:%s", lbutton_str); */
		/*SETTING_TRACE("mbutton_str:%s", mbutton_str); */
		/*SETTING_TRACE("rbutton_str:%s", rbutton_str); */
		if (!safeStrCmp("elm_gengrid", eo_view_t))
			elm_gengrid_realized_items_update(eo_view);
		else if (!safeStrCmp("elm_genlist", eo_view_t))
			elm_genlist_realized_items_update(eo_view);

		if (navi_it) {
			/*SETTING_TRACE("Update navi_item, navi_title:%s", navi_title); */
			if (navi_title)
				elm_object_item_text_set(navi_it, _(navi_title));

			/*l,m,r */
			Evas_Object *toolbar = elm_object_item_part_content_get(navi_it, "toolbar");

			char *btn_str[3] = {0, };
			int idx = 0;
			if (lbutton_str && 0 != safeStrCmp(_(lbutton_str), _("IDS_ST_BUTTON_BACK")))
				btn_str[idx++] = g_strdup(lbutton_str);

			if (rbutton_str)
				btn_str[idx++] = g_strdup(rbutton_str);

			if (mbutton_str)
				btn_str[idx++] = g_strdup(mbutton_str);


			Elm_Object_Item *it = NULL;
			Elm_Object_Item *next_it = NULL;
			/*char *text = NULL; */
			it = elm_toolbar_first_item_get(toolbar);
			idx = 0;
			while (it) {
				next_it = elm_toolbar_item_next_get(it);
				/*text = (char *)elm_object_item_text_get(it); */
				elm_object_item_text_set(it, _(btn_str[idx]));
				idx++;
				it = next_it;
			}
			G_FREE(btn_str[0]);
			G_FREE(btn_str[1]);
			G_FREE(btn_str[2]);
		}

		/*if not matched,to check next node. */
		list = eina_list_next(list);
		item = NULL;
	}
	SETTING_TRACE_END;
}


#if 0
static void __rotate_ctxpopup_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

#ifdef ECORE_X
	Setting_Btn_Data *btn_data = (Setting_Btn_Data *)data;
	Evas_Coord w, h;
	int pos = -1;

	ecore_x_window_size_get(ecore_x_window_root_first_get(), &w, &h);
	pos = elm_win_rotation_get(ug_get_window());
	SETTING_TRACE_DEBUG("pos: %d, w: %d, h: %d", pos, w, h);
	if (pos == 90) {
		evas_object_move(btn_data->ctxpopup, 0, w);
	} else if (pos == 270) {
		evas_object_move(btn_data->ctxpopup, h, w);
	} else {
		evas_object_move(btn_data->ctxpopup, 0, h);
	}
#else
	// @todo : repace codes using X with codes tizen 3.0 API
#endif
}


static void __ctxpopup_lang_change_cb(keynode_t *key, void *data)
{
	setting_retm_if(NULL == key, "key is NULL");
	setting_retm_if(NULL == data, "data is NULL");
	Setting_Btn_Data *btn_data = (Setting_Btn_Data *)data;
	char *vconf_name = vconf_keynode_get_name(key);
	if (!safeStrCmp(vconf_name, VCONFKEY_LANGSET)) {
		/*setting_popup_lang_update(popup); */
		int i = 0;
		while (btn_data->btn_text[i]) {
			elm_object_item_text_set(btn_data->item[i], _(btn_data->btn_text[i]));
			i++;
		}
	}
}
#endif


EXPORT_PUBLIC
Evas_Object *setting_create_win_layout(Evas_Object *win_obj)
{
	Evas_Object *layout = NULL;
	/*  Base Layout */
	layout = elm_layout_add(win_obj);
	evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	setting_retvm_if(layout == NULL, FALSE, "layout == NULL");

	elm_layout_theme_set(layout, "layout", "application", "default");

	Evas_Object *bg = setting_create_bg(layout, win_obj, NULL);
	elm_object_part_content_set(layout, "elm.swallow.bg", bg);
	evas_object_show(layout);
	return layout;
}

EXPORT_PUBLIC
Elm_Object_Item *setting_create_guild_layout(Evas_Object *navi_bar,
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
                                             void *cb_data)
{
	Evas_Object *scroller = elm_genlist_add(navi_bar);
	retvm_if(scroller == NULL, NULL, "Cannot set scroller object  as contento of layout");
	elm_genlist_clear(scroller);	/* first to clear list */
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);

	ADD_GL_LABLE(scroller, content_str1);
	ADD_GL_GIF(scroller, png_list)
	ADD_GL_LABLE(scroller, content_str2);
	ADD_GL_BUTTON(scroller, content_button_str, content_button_click_cb, cb_data)

	return setting_push_layout_navi_bar(title_str,
	                                    lbutton_str, rbutton_str, NULL,
	                                    lbutton_click_cb,
	                                    rbutton_click_cb,
	                                    NULL, cb_data, scroller, navi_bar, NULL);
}

EXPORT_PUBLIC
Elm_Object_Item *setting_create_guild_layout2(Evas_Object *navi_bar,
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
                                              void *cb_data)
{
	Evas_Object *scroller;
	Elm_Object_Item *item = setting_push_layout_navi_bar_scroller(navi_bar,
	                                                              title_str,
	                                                              lbutton_str, rbutton_str,
	                                                              lbutton_click_cb,
	                                                              rbutton_click_cb,
	                                                              cb_data, &scroller,
	                                                              navi_bar);
	Evas_Object *outer_box = setting_create_client_bx(navi_bar);
	/*add top text window.. */
	if (content_str1) {
		const char *str = setting_customize_text(content_str1, 0, NULL, NULL);
		Evas_Object *lable = setting_create_textbox(navi_bar, str);
		setting_disable_evas_object(lable);
		elm_box_pack_end(outer_box, lable);
		FREE(str);
	}

	/*add UG window.. */
	if (ug_layout) {
		Evas_Object *ug_win = elm_layout_add(navi_bar);
		evas_object_size_hint_weight_set(ug_win, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		setting_retvm_if(ug_win == NULL, NULL, "layout == NULL");
		elm_layout_theme_set(ug_win, "layout", "application", "default");
		elm_object_part_content_set(ug_win, "elm.swallow.bg", setting_create_blank_rect_customize(navi_bar, 720, 850));
		/*elm_object_part_content_set(layout, "elm.swallow.content", navi); */
		elm_box_pack_end(outer_box, ug_win);
		evas_object_show(ug_win);
		*ug_layout = ug_win;
	}

	/*add bottom text */
	if (content_str2) {
		const char *str = setting_customize_text(content_str2, 0, NULL, NULL);
		Evas_Object *lable = setting_create_textbox(navi_bar, str);
		setting_disable_evas_object(lable);
		elm_box_pack_end(outer_box, lable);
		FREE(str);
	}

	/*add bottom button */
	if (content_button_str) {
		Evas_Object *layout = elm_layout_add(navi_bar);
		evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		elm_layout_theme_set(layout, "layout", "application", "default");
		elm_object_part_content_set(layout, "elm.swallow.bg", setting_create_blank_rect_customize(navi_bar, 680, 100));
		Evas_Object *btn = elm_button_add(navi_bar);
		elm_object_text_set(btn, content_button_str);
		evas_object_propagate_events_set(btn, EINA_FALSE);
		evas_object_smart_callback_add(btn, "clicked", content_button_click_cb, cb_data);
		elm_object_part_content_set(layout, "elm.swallow.content", btn);
		evas_object_show(btn);
		evas_object_show(layout);
		elm_box_pack_end(outer_box, layout);
	}

	elm_object_content_set(scroller, outer_box);
	return item;
}

EXPORT_PUBLIC
Evas_Object *setting_create_navi_bar(Evas_Object *layout)
{

	Evas_Object *navi = NULL;
	navi = elm_naviframe_add(layout);

	elm_naviframe_prev_btn_auto_pushed_set(navi, EINA_FALSE);

	eext_object_event_callback_add(navi, EEXT_CALLBACK_BACK, eext_naviframe_back_cb, NULL);
	eext_object_event_callback_add(navi, EEXT_CALLBACK_MORE, eext_naviframe_more_cb, NULL);

	/*elm_object_item_signal_callback_event(navi */

	if (navi == NULL) {
		SETTING_TRACE(" *** elm_naviframe_add returns NULL *** ");
		return NULL;
	}
	elm_object_part_content_set(layout, "elm.swallow.content", navi);
	evas_object_show(navi);
	return navi;
}

/**
* The API to Create title buttons of naviframe
* if sip is shown,the buttons created by this API will be shown,it sip is hidden,they will be hiden too
* @return non
* eg:
* 	//4 common buttons
* 	setting_append_naviframe_title_buttons(ad->navi_it, ad->navi_bar,
* 					       "1", "2", "3" ,"4",
* 					       _back_cb,
* 					       _back_cb,
* 					       _back_cb,
* 					       _back_cb);
*
* 	//more button +2 common buttons + '<-'button
* 	setting_append_naviframe_title_buttons(ad->navi_it, ad->navi_bar,
* 					       NULL, "2", "3" ,NULL,
* 					       _back_cb,
* 					       _back_cb,
* 					       _back_cb,
* 					       _back_cb);
*
* 	//1common buton + '<-'button
* 	setting_append_naviframe_title_buttons(ad->navi_it, ad->navi_bar,
* 					       NULL, NULL, _("IDS_SA_BUTTON_DONE_ABB") ,NULL,
* 					       NULL,
* 					       NULL,
* 					       _back_cb,
* 					       _back_cb);
*/
EXPORT_PUBLIC
void setting_append_naviframe_title_buttons(Elm_Object_Item *navi_it,
                                            Evas_Object *navigate_bar,
                                            char *more_button_str,
                                            char *left_button_str,
                                            char *right_button_str,
                                            char *back_button_str,
                                            setting_call_back_func more_button_click_cb,
                                            setting_call_back_func left_button_click_cb,
                                            setting_call_back_func right_button_click_cb,
                                            setting_call_back_func back_button_click_cb,
                                            void *cb_data)
{
	SETTING_TRACE_BEGIN;
	Evas_Object *btn = NULL;
	/*  create buttons */
	if (more_button_click_cb) {
		if (!more_button_str) {

			/*btn = setting_create_button(navigate_bar, NULL, "naviframe/more/default", more_button_click_cb, cb_data); */
			btn = elm_button_add(navigate_bar);
			elm_object_style_set(btn, "naviframe/more/default");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
			evas_object_smart_callback_add(btn, "clicked", more_button_click_cb, cb_data);
			evas_object_show(btn);
		} else {
			btn = setting_create_button(navigate_bar, more_button_str, "naviframe/title/default", more_button_click_cb, cb_data);

		}
		elm_object_item_part_content_set(navi_it, "title_more_btn", btn);
	}

	if (left_button_click_cb) {
		btn = setting_create_button(navigate_bar, left_button_str, "naviframe/title/default", left_button_click_cb, cb_data);
		elm_object_item_part_content_set(navi_it, "title_toolbar_button1", btn);
	}

	if (right_button_click_cb) {
		btn = setting_create_button(navigate_bar, right_button_str, "naviframe/title/default", right_button_click_cb, cb_data);
		elm_object_item_part_content_set(navi_it, "title_toolbar_button2", btn);
	}

	if (back_button_click_cb) {
		if (!back_button_str) {
			/*btn = setting_create_button(navigate_bar, NULL, "naviframe/back_btn/default", back_button_click_cb, cb_data); */
			btn = elm_button_add(navigate_bar);
			elm_object_style_set(btn, "naviframe/back_btn/default");
			evas_object_size_hint_weight_set(btn, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
			evas_object_smart_callback_add(btn, "clicked", back_button_click_cb, cb_data);
			evas_object_show(btn);
		} else {
			btn = setting_create_button(navigate_bar, back_button_str, "naviframe/title/default", back_button_click_cb, cb_data);

		}
		elm_object_item_part_content_set(navi_it, "title_prev_btn", btn);
	}
}

/*"title_more_btn" won't be supported any more, so don't pass mbutton_str */
EXPORT_PUBLIC
Elm_Object_Item *setting_create_navi_bar_top_buttons(char *title_str,
                                                     char *lbutton_str,
                                                     char *rbutton_str,
                                                     char *mbutton_str,
                                                     setting_call_back_func lbutton_click_cb,
                                                     setting_call_back_func rbutton_click_cb,
                                                     setting_call_back_func mbutton_click_cb,
                                                     void *cb_data, Evas_Object *eo_view,/*any container obj */
                                                     Evas_Object *navigate_bar)
{
	/*SETTING_TRACE_BEGIN; */
	Evas_Object *l_button = NULL;
	Evas_Object *r_button = NULL;
	Evas_Object *m_button = NULL;
	Elm_Object_Item *navi_it = NULL;

	if (lbutton_str) {
		/*  create buttons */
		l_button = setting_create_button(navigate_bar, lbutton_str,
		                                 "naviframe/toolbar/default",
		                                 lbutton_click_cb, cb_data);
	}
	if (rbutton_str) {
		r_button = setting_create_button(navigate_bar, rbutton_str,
		                                 "naviframe/end_btn/default",
		                                 rbutton_click_cb, cb_data);
	}
	if (mbutton_str) {
		m_button = setting_create_button(navigate_bar, mbutton_str,
		                                 NULL,
		                                 mbutton_click_cb, cb_data);
	}
	navi_it = elm_naviframe_item_push(navigate_bar, title_str,
	                                  NULL, NULL, eo_view,
	                                  NULL);
	retv_if(!navi_it, NULL);

	/*  arrange buttons into  navi_it*/
	if (l_button) { /* done */
		elm_object_item_part_content_set(navi_it, "title_toolbar_button1", l_button);
	}
	if (r_button) { /* cancel */
		elm_object_item_part_content_set(navi_it, "title_prev_btn", r_button);
	}
	if (m_button) { /* none */
		elm_object_item_part_content_set(navi_it, "title_more_btn", m_button);
	}
	return navi_it;
}

#if 0
static void _move_ctxpopup(Evas_Object *ctxpopup, Evas_Object *btn)
{
	Evas_Coord x, y, w , h;
	evas_object_geometry_get(btn, &x, &y, &w, &h);
	evas_object_move(ctxpopup, x + (w / 2), y + (h / 2));
}

static void _ctxpopup_dismissed_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object* ctxpopup = (Evas_Object *)data;
	evas_object_del(ctxpopup);
	ctxpopup = NULL;
}

/* Icon + Text (More button style : Naviframe Toolbar) */
static void
_ctxpopup_more_button_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

}
#endif

Elm_Object_Item *__create_navi_bar_bottom_buttons(char *title_str,
												  char *lbutton_str,
												  setting_call_back_func lbutton_click_cb,
                                                  void *cb_data, Evas_Object *eo_view,/*any container obj */
                                                  Evas_Object *navigate_bar)
{
	SETTING_TRACE_BEGIN;

	Elm_Object_Item *navi_it = NULL;
	Evas_Object *lbtn = NULL;

#define USE_BACK 0
#if USE_BACK
	/*  arrange buttons into  controlbar */
	if (0 == safeStrCmp(_(lbutton_str), _("IDS_ST_BUTTON_BACK"))) {
		lbtn = setting_create_button(navigate_bar, _(lbutton_str),
		                             NAVI_BACK_BUTTON_STYLE,
		                             /*NULL, NULL); */
		                             lbutton_click_cb, cb_data);
		navi_it = elm_naviframe_item_push(navigate_bar, title_str, NULL, NULL, eo_view, NULL);	/* add new button */
		elm_object_item_part_content_set(navi_it, "title_left_btn", lbtn);

		if (navi_it) {
			elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE, EINA_TRUE);
			elm_naviframe_item_pop_cb_set(navi_it, (Elm_Naviframe_Item_Pop_Cb)lbutton_click_cb, cb_data);
		}

		//----------------------
		// CREATE TOOLBAR
		//----------------------
	} else {
#endif
		// ARROW here back
		lbtn = setting_create_button(navigate_bar, _(lbutton_str),
		                             NAVI_BACK_ARROW_BUTTON_STYLE,
		                             /*NULL, NULL); */
		                             lbutton_click_cb, cb_data);

		// create naviframe with Arror button
		navi_it = elm_naviframe_item_push(navigate_bar, title_str, lbtn, NULL, eo_view, NULL);	/* add new button */

		if (lbutton_click_cb)
			elm_naviframe_item_pop_cb_set(navi_it, (Elm_Naviframe_Item_Pop_Cb)lbutton_click_cb, cb_data);

		if (navi_it) {
			if (0 != safeStrCmp(title_str, dgettext(SETTING_PACKAGE, title_str)))
				elm_object_item_domain_text_translatable_set(navi_it, SETTING_PACKAGE, EINA_TRUE);
		}
		//----------------------
		// CREATE TOOLBAR
		//----------------------
#if USE_BACK
	}
#endif
	return navi_it;
}


/**
* Create buttons on the specialized navigation bar
*
* @param[in] eo_view	navigation bar's content
* @param[in] controlbar   title obj of navigation bar
*
*/
EXPORT_PUBLIC
void setting_create_navi_bar_buttons(char *title_str,
									char *lbutton_str,
									setting_call_back_func lbutton_click_cb,
                                     void *cb_data, Evas_Object *eo_view,/*any container obj */
                                     Evas_Object *navigate_bar,	/*the specialized navigation bar */
                                     Evas_Object *titleobj)
{
	__create_navi_bar_bottom_buttons(title_str, lbutton_str,
	                                 lbutton_click_cb,
	                                 cb_data, eo_view,
	                                 navigate_bar);
	return;
}

/**
* The general API to create a layout with navigation bar,
*   which contents any evas container object(@param[eo_view]) as its content
*
* @param[in] eo_view	navigation bar's content
* @param[out] titleobj   title obj of navigation bar
*
* return main layout of UG or App..
*/
EXPORT_PUBLIC
Evas_Object *setting_create_layout_navi_bar(Evas_Object *win_layout,
											Evas_Object *win_obj,
											char *title_str,
											char *lbutton_str,
											setting_call_back_func lbutton_click_cb,
											void *cb_data,
											Evas_Object *eo_view,	/*any container obj constructed on any evas obj */
                                            Evas_Object **navi_bar,
                                            Evas_Object **titleobj)
{

	Evas_Object *layout = setting_create_win_layout(win_obj);
	*navi_bar = setting_create_navi_bar(layout);

	setting_create_navi_bar_buttons(title_str,
	                                lbutton_str,
	                                lbutton_click_cb,
	                                cb_data, eo_view /*content */ ,
	                                *navi_bar, NULL);
	return layout;
}

/**
* The API to create a layout with navigation bar,
*   which contents a scroller object as its content
*
* @param[out] scroller   navigation bar's content
*
* @return a layout with a special navigation bar and at most 2 buttons
*/
EXPORT_PUBLIC
Evas_Object *setting_create_layout_navi_bar_scroller(Evas_Object *win_layout,
                                                     Evas_Object *win_obj,
                                                     char *title_str,
                                                     char *lbutton_str,
                                                     char *rbutton_str,
                                                     setting_call_back_func
                                                     lbutton_click_cb,
                                                     setting_call_back_func
                                                     rbutton_click_cb,
                                                     void *cb_data,
                                                     Evas_Object **scroller,
                                                     Evas_Object **navi_bar)
{

	Evas_Object *layout = setting_create_win_layout(win_obj);
	*navi_bar = setting_create_navi_bar(layout);

	/*****/
	/*  create scroller as CONTENT */
	Evas_Object *scrl = NULL;
	/* Evas_Object *ret = NULL; */
	if (scroller != NULL) {
		/* scrl =*scroller = elm_scroller_add(win_layout); */
		scrl = *scroller = elm_scroller_add(layout);
		retvm_if(*scroller == NULL, NULL,
		         "Cannot set scroller object as contento of layout");
		elm_scroller_bounce_set(*scroller, EINA_FALSE, EINA_TRUE);
		elm_scroller_policy_set(*scroller, ELM_SCROLLER_POLICY_OFF,
		                        ELM_SCROLLER_POLICY_AUTO);
		evas_object_show(*scroller);
	}

	/*****/

	setting_create_navi_bar_buttons(title_str,
	                                lbutton_str,
	                                lbutton_click_cb,
	                                cb_data, scrl /*content */ ,
	                                *navi_bar, NULL);
	return layout;
}

/**
* The API to create a layout with navigation bar ,
*	which contents a genlist object as its content
*
* @return a layout with a special navigation bar and at most 2 buttons
*/
EXPORT_PUBLIC
Evas_Object *setting_create_layout_navi_bar_genlist(Evas_Object *win_layout,
                                                    Evas_Object *win_obj,
                                                    char *title_str,
                                                    char *lbutton_str,
                                                    char *rbutton_str, /* do nothing */
                                                    void *lbutton_click_cb,
                                                    void *rbutton_click_cb,/* do nothing */
                                                    void *cb_data,
                                                    Evas_Object **genlist,
                                                    Evas_Object **navi_bar)
{
	/*  win_layout -> layout */
	Evas_Object *layout = setting_create_win_layout(win_obj);
	*navi_bar = setting_create_navi_bar(layout);

	*genlist = elm_genlist_add(*navi_bar);
	retvm_if(*genlist == NULL, NULL,
	         "Cannot set scroller object as contento of layout");
	elm_genlist_mode_set(*genlist, ELM_LIST_COMPRESS);
	elm_genlist_clear(*genlist);	/* first to clear list */
	evas_object_smart_callback_add(*genlist, "realized", __gl_realized_cb, NULL);

	setting_create_navi_bar_buttons(title_str,
	                                lbutton_str,
	                                lbutton_click_cb,
	                                cb_data, *genlist, *navi_bar,
	                                NULL);
	return layout;
}

/**
* The general API to push any evas object(@param[eo_view]) to the specialized navi_bar,
*   which contents at most 3 buttons
*/
EXPORT_PUBLIC
Elm_Object_Item *
setting_push_layout_navi_bar(char *title_str,
                             char *lbutton_str, char *rbutton_str,
                             char *mbutton_str,
                             void *lbutton_click_cb,
                             void *rbutton_click_cb,
                             setting_call_back_func mbutton_click_cb,
                             void *cb_data, Evas_Object *eo_view,
                             Evas_Object *navi_bar, Evas_Object **titleobj)
{

	return __create_navi_bar_bottom_buttons(title_str,
	                                      lbutton_str,
	                                      lbutton_click_cb,
	                                      cb_data, eo_view, navi_bar);
}

/**
* The API to push a scroller object(@param[scroller]) to the specialized navi_bar,
*   which contents at most 2 buttons
*/

EXPORT_PUBLIC
Elm_Object_Item *
setting_push_layout_navi_bar_scroller(Evas_Object *win_main, char *title_str,
                                      char *lbutton_str, char *rbutton_str,
                                      setting_call_back_func lbutton_click_cb,
                                      setting_call_back_func rbutton_click_cb,
                                      void *cb_data,
                                      Evas_Object **scroller,
                                      Evas_Object *navi_bar)
{
	/*  create scroller */
	Evas_Object *scrl = NULL;
	if (scroller != NULL) {
		scrl = *scroller = elm_scroller_add(navi_bar);
		retvm_if(*scroller == NULL, NULL,
		         "Cannot set scroller as contento of layout");
		elm_scroller_bounce_set(*scroller, EINA_FALSE, EINA_TRUE);
		elm_scroller_policy_set(*scroller, ELM_SCROLLER_POLICY_OFF,
		                        ELM_SCROLLER_POLICY_AUTO);

		evas_object_show(*scroller);
	}

	return setting_push_layout_navi_bar(title_str,
	                                    lbutton_str, rbutton_str, NULL,
	                                    lbutton_click_cb,
	                                    rbutton_click_cb,
	                                    NULL, cb_data, scrl, navi_bar, NULL);
}

/**
* @ The API to push a genlist object(@param[genlist]) to the specialized navi_bar,
*   which contents at most 2 buttons
*/
EXPORT_PUBLIC
Elm_Object_Item *
setting_push_layout_navi_bar_genlist(Evas_Object *win_layout,
                                     Evas_Object *win_obj, char *title_str,
                                     char *lbutton_str, char *rbutton_str,
                                     void *lbutton_click_cb,
                                     void *rbutton_click_cb,
                                     void *cb_data, Evas_Object **genlist,
                                     Evas_Object *navi_bar)
{
	/*  create scroller */
	Evas_Object *gl = NULL;
	if (genlist != NULL) {
		*genlist = elm_genlist_add(navi_bar);
		retvm_if(*genlist == NULL, NULL,
		         "Cannot set scroller object  as contento of layout");
		elm_genlist_mode_set(*genlist, ELM_LIST_COMPRESS);

		elm_genlist_homogeneous_set(*genlist, EINA_TRUE);

		gl = *genlist;
		elm_genlist_clear(gl);	/* first to clear list */
		evas_object_smart_callback_add(gl, "realized", __gl_realized_cb, NULL);
	}

	return setting_push_layout_navi_bar(title_str,
	                                    lbutton_str, rbutton_str, NULL,
	                                    lbutton_click_cb,
	                                    rbutton_click_cb,
	                                    NULL, cb_data, gl, navi_bar, NULL);
}

EXPORT_PUBLIC
void setting_go_to_top(Evas_Object *content)
{
	setting_retm_if(content == NULL, "content is NULL");
	const char *type = elm_object_widget_type_get(content);
	SETTING_TRACE("content type:%s", type);

	if (0 == safeStrCmp(type, "elm_genlist")) {
		Elm_Object_Item *first_item = elm_genlist_first_item_get(content);
		if (first_item) {
			elm_genlist_item_show(first_item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
		}
	} else if (0 == safeStrCmp(type, "elm_scroller")) {
		elm_scroller_region_show(content, 0, 0, 0, 0);
	} else if (0 == safeStrCmp(type, "elm_gengrid")) {
		Elm_Object_Item *first_item = elm_gengrid_first_item_get(content);
		if (first_item) {
			elm_gengrid_item_show(first_item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
		}
	} else if (NULL != type) {
		Eina_List *sub_objs = (Eina_List *)elm_widget_scrollable_children_get(content);
		ret_if(!sub_objs);
		Evas_Object *sub_obj = NULL;
		Eina_List *l = NULL;
		const char *sub_type = NULL;

		/*just only search the first objects level */
		EINA_LIST_FOREACH(sub_objs, l, sub_obj) {
			if (!sub_obj) {
				continue;
			}

			sub_type = elm_object_widget_type_get(sub_obj);
			SETTING_TRACE("sub content type :%s", sub_type);
			if (0 == safeStrCmp(sub_type, "elm_genlist")) {
				Elm_Object_Item *first_item = elm_genlist_first_item_get(sub_obj);
				if (first_item) {
					elm_genlist_item_show(first_item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
				}
			} else if (0 == safeStrCmp(sub_type, "elm_scroller")) {
				elm_scroller_region_show(sub_obj, 0, 0, 0, 0);
			} else if (0 == safeStrCmp(sub_type, "elm_gengrid")) {
				Elm_Object_Item *first_item = elm_gengrid_first_item_get(sub_obj);
				if (first_item) {
					elm_gengrid_item_show(first_item, ELM_GENLIST_ITEM_SCROLLTO_TOP);
				}
			} else {
				/* do nothing */
			}
		}
	} else {
		/* do nothing */
	}
}

/*
* To go to top of list in top view of navibar.
* @param[in] navibar
*/
EXPORT_PUBLIC
void setting_go_to_navibar_list_top(Evas_Object *navibar)
{
	Elm_Object_Item *navi_it = elm_naviframe_top_item_get(navibar);
	if (navi_it) {
		Evas_Object *content = elm_object_item_part_content_get(navi_it, "default");

		setting_go_to_top(content);
	}
}

EXPORT_PUBLIC void setting_nf_check_vconf_cb(keynode_t *key, void *data)
{
	ret_if(data == NULL);
	Evas_Object *check = data;
	char *vconf_name = vconf_keynode_get_name(key);
	int status = vconf_keynode_get_bool(key);
	SETTING_TRACE("Vconf[%s] has changed to be %d just now", vconf_name, status);
	elm_check_state_set(check, status);
}

EXPORT_PUBLIC void setting_nf_check_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	const char *vconf = data;
	Eina_Bool state = elm_check_state_get(obj);
	vconf_set_bool(vconf, state);
}

EXPORT_PUBLIC void setting_nf_check_del_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);
	const char *vconf = data;
	(void)vconf_ignore_key_changed(vconf, setting_nf_check_vconf_cb);
}

