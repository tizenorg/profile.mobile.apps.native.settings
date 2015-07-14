/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <setting-menuscreen.h>

#ifndef UG_MODULE_API
#define UG_MODULE_API __attribute__ ((visibility("default")))
#endif

setting_view *__get_menuscreen_view_to_load(service_h service)
{
	SETTING_TRACE_BEGIN;
	//setting_retvm_if(NULL == data, NULL, "NULL == data");
	return &setting_view_menuscreen_main;
}

Evas_Object *__get_menuscreen_layout_to_return(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingMenuscreenUG *menuscreenUG = priv;
	return menuscreenUG->ly_main;
}

static void setting_menuscreen_ug_cb_resize(void *data, Evas *e,
					 Evas_Object *obj, void *event_info)
{
	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) data;
	/* setting_view_update(&setting_view_about_main, ad); */
	setting_view_update(ad->view_to_load, ad);
}

static void *setting_menuscreen_ug_on_create(ui_gadget_h ug,
					  enum ug_mode mode, service_h service,
					  void *priv)
{
	setting_retvm_if((!priv), NULL, "!priv");

	SettingMenuscreenUG *menuscreenUG = priv;
	menuscreenUG->ug = ug;

	setting_set_i18n(SETTING_PACKAGE, SETTING_LOCALEDIR);

	menuscreenUG->win_main_layout = (Evas_Object *) ug_get_parent_layout(ug);
	menuscreenUG->win_get = (Evas_Object *) ug_get_window();

	evas_object_show(menuscreenUG->win_main_layout);
	menuscreenUG->evas = evas_object_evas_get(menuscreenUG->win_main_layout);

	setting_retvm_if(menuscreenUG->win_main_layout == NULL, NULL, "cannot get main window ");

	/* register view node table */
	setting_view_node_table_intialize();
	setting_view_node_table_register(&setting_view_menuscreen_main, NULL);
	setting_view_node_table_register(&setting_view_menuscreen_password, &setting_view_menuscreen_main);

	//setting_create_Gendial_itc("dialogue/1text.1icon.2", &(menuscreenUG->itc_1text_1icon));
	setting_create_Gendial_itc("dialogue/1text.1icon.2", &(menuscreenUG->itc_1text_1icon_2));

	menuscreenUG->itc_seperator.item_style = "dialogue/separator";
	menuscreenUG->itc_seperator.func.text_get = NULL;
	menuscreenUG->itc_seperator.func.content_get = NULL;
	menuscreenUG->itc_seperator.func.state_get = NULL;
	menuscreenUG->itc_seperator.func.del = NULL;


	setting_create_Gendial_itc("dialogue/1text", &(menuscreenUG->itc_1text));

	/*  creating a view. */
	menuscreenUG->view_to_load = __get_menuscreen_view_to_load(service);
	setting_retvm_if(NULL == menuscreenUG->view_to_load, NULL, "NULL == menuscreenUG->view_to_load");
	setting_view_node_set_cur_view(menuscreenUG->view_to_load);
	setting_view_create(menuscreenUG->view_to_load, (void *)menuscreenUG);
	evas_object_event_callback_add(menuscreenUG->win_main_layout,EVAS_CALLBACK_RESIZE, setting_menuscreen_ug_cb_resize, menuscreenUG);
	return __get_menuscreen_layout_to_return(service, menuscreenUG);
}

static void setting_menuscreen_ug_on_start(ui_gadget_h ug, service_h service,
					void *priv)
{
}

static void setting_menuscreen_ug_on_pause(ui_gadget_h ug, service_h service,
					void *priv)
{
}

static void setting_menuscreen_ug_on_resume(ui_gadget_h ug, service_h service,
					 void *priv)
{
}


static void _clear_menuscreen_list(Eina_List *menu_screen_list)
{
	SETTING_TRACE_BEGIN;
    Eina_List *elist = NULL;
    ug_menuscreen_info* pnode = NULL;

    EINA_LIST_FOREACH( menu_screen_list, elist, pnode ) {
        menu_screen_list = eina_list_remove(menu_screen_list, pnode);
    }
    eina_list_free( menu_screen_list );
}

static void setting_menuscreen_ug_on_destroy(ui_gadget_h ug, service_h service,
					  void *priv)
{
	SETTING_TRACE_BEGIN;
	setting_retm_if((!priv), "!priv");
	SettingMenuscreenUG *menuscreenUG = priv;

	evas_object_event_callback_del(menuscreenUG->win_main_layout, EVAS_CALLBACK_RESIZE, setting_menuscreen_ug_cb_resize);	/* fix flash issue for gallery */
	menuscreenUG->ug = ug;

    if (!ug || !priv)
        return;

    if(menuscreenUG->menuscreen_list) {
        _clear_menuscreen_list( menuscreenUG->menuscreen_list);
        menuscreenUG->menuscreen_list = NULL;
    }

	/*  called when this shared gadget is terminated. similar with app_exit */
	if (&setting_view_menuscreen_main == menuscreenUG->view_to_load) {
		setting_view_destroy(&setting_view_menuscreen_main, menuscreenUG);
	}

	if (NULL != ug_get_layout(menuscreenUG->ug)) {
		evas_object_hide((Evas_Object *) ug_get_layout(menuscreenUG->ug));
		evas_object_del((Evas_Object *) ug_get_layout(menuscreenUG->ug));
	}

	SETTING_TRACE_END;
}

static void setting_menuscreen_ug_on_message(ui_gadget_h ug, service_h msg,
					  service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
}

static void setting_menuscreen_ug_on_event(ui_gadget_h ug,
					enum ug_event event, service_h service,
					void *priv)
{
	SETTING_TRACE_BEGIN;
	switch (event) {
	case UG_EVENT_LOW_MEMORY:
		break;
	case UG_EVENT_LOW_BATTERY:
		break;
	case UG_EVENT_LANG_CHANGE:
		break;
	case UG_EVENT_ROTATE_PORTRAIT:
		break;
	case UG_EVENT_ROTATE_PORTRAIT_UPSIDEDOWN:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE:
		break;
	case UG_EVENT_ROTATE_LANDSCAPE_UPSIDEDOWN:
		break;
	case UG_EVENT_REGION_CHANGE:
		break;
	default:
		break;
	}
}

static void setting_menuscreen_ug_on_key_event(ui_gadget_h ug,
					    enum ug_key_event event,
					    service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) priv;

	switch (event) {
	case UG_KEY_EVENT_END:
		{
			if (elm_naviframe_top_item_get(ad->navi_bar) ==
			   elm_naviframe_bottom_item_get(ad->navi_bar)) {
				ug_destroy_me(ug);
			} else {
				/* elm_naviframe_item_pop(ad->navi_bar); */
				setting_view_cb_at_endKey(ad);
			}
		}
		break;
	default:
		break;
	}
}

UG_MODULE_API int UG_MODULE_INIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	SettingMenuscreenUG *menuscreenUG = calloc(1, sizeof(SettingMenuscreenUG));
	setting_retvm_if(!menuscreenUG, -1, "Create SettingMenuscreenUG obj failed");

	ops->create = setting_menuscreen_ug_on_create;
	ops->start = setting_menuscreen_ug_on_start;
	ops->pause = setting_menuscreen_ug_on_pause;
	ops->resume = setting_menuscreen_ug_on_resume;
	ops->destroy = setting_menuscreen_ug_on_destroy;
	ops->message = setting_menuscreen_ug_on_message;
	ops->event = setting_menuscreen_ug_on_event;
	ops->key_event = setting_menuscreen_ug_on_key_event;
	ops->priv = menuscreenUG;
	ops->opt = UG_OPT_INDICATOR_ENABLE;

	return 0;
}

UG_MODULE_API void UG_MODULE_EXIT(struct ug_module_ops *ops)
{
	SETTING_TRACE_BEGIN;
	struct SettingMenuscreenUG *menuscreenUG;
	setting_retm_if(!ops, "ops == NULL");

	menuscreenUG = ops->priv;
	if (menuscreenUG) {
		FREE(menuscreenUG);
	}
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

void setting_menuscreen_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
				  void *priv)
{
	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) priv;
	Evas_Object *base;

	if (!priv)
		return;
	SETTING_TRACE_BEGIN;

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base)
		return;

	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
						 EVAS_HINT_EXPAND);
		//elm_win_resize_object_add(ad->win_get, base);
		evas_object_show(base);
		break;
	default:
		break;
	}

	SETTING_TRACE_END;
}


/*****/
UG_MODULE_API int setting_plugin_reset(service_h service, void *priv)
{
	SETTING_TRACE_BEGIN;
	return 0;
}

/*****/
