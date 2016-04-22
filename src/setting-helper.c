#include <setting-helper.h>

bool is_selected_menu(Eina_List *list, const char *keystr)
{
	while (list) {
		char *str = eina_list_data_get(list);
		if (NULL == str)
			continue;
		if (0 == safeStrCmp(keystr, str))
			return TRUE;
		list = eina_list_next(list);
	}
	return FALSE;
}

Eina_Bool setting_main_freeze_event_timer_cb(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, EINA_FALSE);

	setting_main_appdata *ad = (setting_main_appdata *)cb;

	evas_object_freeze_events_set(ad->navibar_main, EINA_FALSE);
	ad->event_freeze_timer = NULL;
	return EINA_FALSE;
}

/**
 * @brief preset all pointers to NULL
 *
 * @bug this code may make dangling pointers,
 * in setting_vconf_int_xxx callback refer to ad->data_xxxx ptrs.
 * if it's NULL, what happen?
 */
void setting_main_genlist_handler_reset(void *data)
{
	SETTING_TRACE_BEGIN;
	retm_if(data == NULL, "Invalid argument: data is NULL");
	setting_main_appdata *ad = data;


	ad->data_network = NULL;

	SETTING_TRACE_END;
}

void setting_main_genlist_handler_set(void *data, int viewtype)
{
	retm_if(data == NULL, "Invalid argument: data is NULL");
	/* setting_main_appdata *ad = data; */

}

bool is_searchmode_app(Cfg_Item_Type type)
{
	bool ret = false;

	switch (type % 10) {
	case Cfg_Item_Ug_Node:				/* ug run */
	case Cfg_Item_App_Node:				/* docomo run */
	case Cfg_Item_View_Node:			/* n-depth search */
	case Cfg_Item_AppLauncher_Node:		/* app launch */
		ret = true;
		break;

	case Cfg_Item_Ui_Node:
	case Cfg_Item_Title_Node:
		ret = false;
		break;

	default:
		ret = false;
		break;
	}
	return ret;
}

void setting_main_click_list_item_reset_data(void *data,
											 Evas_Object *obj)
{
	/*SETTING_TRACE_BEGIN; */
	setting_main_appdata *ad = (setting_main_appdata *) data;
	ad->isInUGMode = FALSE;
	/*SETTING_TRACE_END; */
}


void setting_main_destroy_ug_cb(ui_gadget_h ug, void *priv)
{
	setting_main_appdata *ad = (setting_main_appdata *) priv;

	if (!ad) {
		return; 	/*  do nothing if ad is NULL */
	}

	evas_object_show(ad->navibar_main);

	if (ug) {
		setting_ug_destroy(ug);
	}

	SETTING_TRACE("ad->ug:%p", ad->ug);
	SETTING_TRACE("g_main_ad->isInUGMode:%d", g_main_ad->isInUGMode);
	SETTING_TRACE("g_main_ad->ug:%p", g_main_ad->ug);

	if (is_searchmode_app(ad->is_searchmode)) {
		/*ug_destroy(parent); */
		if (ad->ug) {
			ug_destroy_me(ad->ug);
			ad->ug = NULL;
		}

		/*SETTING_TRACE("terminate app !!!!!! "); */
		elm_exit();
		return;
	}

	elm_object_tree_focus_allow_set(ad->ly_main, TRUE);
	/*ea_screen_reader_support_set(ad->ly_main, TRUE); */

	setting_main_click_list_item_reset_data(ad, NULL);
	SETTING_TRACE("update main genlist in unloading UG");
	SETTING_TRACE("ad->ug:%p", ad->ug);
	SETTING_TRACE("g_main_ad->isInUGMode:%d", g_main_ad->isInUGMode);
	SETTING_TRACE("g_main_ad->ug:%p", g_main_ad->ug);
	SETTING_TRACE_END;
}

void setting_main_result_ug_cb(ui_gadget_h ug, app_control_h service,
							   void *priv)
{
	if (!priv) {
		return;
	}
	SETTING_TRACE_BEGIN;
	/* setting_main_appdata *ad = (setting_main_appdata *) priv; */

	SETTING_TRACE_END;
}

void setting_main_layout_ug_cb(ui_gadget_h ug, enum ug_mode mode,
							   void *priv)
{
	/*setting_main_appdata *ad = (setting_main_appdata *) priv; */
	Evas_Object *base;

	if (!priv) {
		return;
	}
	SETTING_TRACE_BEGIN;

	setting_main_appdata *ad = (setting_main_appdata *) priv;
	evas_object_hide(ad->navibar_main);

	base = (Evas_Object *) ug_get_layout(ug);
	if (!base) {
		return;
	}


	switch (mode) {
	case UG_MODE_FULLVIEW:
		evas_object_size_hint_weight_set(base, EVAS_HINT_EXPAND,
										 EVAS_HINT_EXPAND);
		/*elm_win_resize_object_add(ad->win_main, base); */
		evas_object_show(base);
		break;
	default:
		break;
	}

	/*SETTING_TRACE_END; */
}
