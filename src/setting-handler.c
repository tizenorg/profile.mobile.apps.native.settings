#include <setting-handler.h>
#include <setting-helper.h>

#include <wifi.h>
#include <bluetooth.h>
#include <bluetooth_internal.h>
#include <dbus/dbus.h>
#include <app_control_internal.h>


static Setting_GenGroupItem_Data *__mobileap_handler(void *data, char *keyStr, char *icon_path, char *ug_args, Evas_Object *genlist, Elm_Object_Item *parent)
{
	/*SETTING_TRACE_BEGIN; */
	setting_main_appdata *ad = (setting_main_appdata *)data;
	Setting_GenGroupItem_Data *obj = NULL;

	obj = setting_create_Gendial_field_def(genlist,
												&(ad->itc_table[GENDIAL_Type_1text_1icon_2]),
												setting_main_click_list_ex_ug_cb,
												ug_args,
												SWALLOW_Type_1ICON_1IMAGE,
												icon_path,
												NULL,
												0,
												keyStr,
												NULL,
												NULL);
	__BACK_POINTER_SET(obj);
	if (obj) {
		obj->userdata = ad;
	}

	return obj;
}

static Setting_GenGroupItem_Data *__backlight_time_handler(void *data, char *keyStr, char *icon_path, char *ug_args, Evas_Object *genlist, Elm_Object_Item *parent)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *)data;
	Setting_GenGroupItem_Data *obj = NULL;

	char *pa_backlight = get_pa_backlight_time_str();
	obj = setting_create_Gendial_field_def(genlist,
	                                             &(ad->itc_table[GENDIAL_Type_1icon_2text]),
	                                             setting_main_click_list_ex_ug_cb,
	                                             ug_args,
	                                             SWALLOW_Type_1ICON_1IMAGE,
	                                             icon_path,
	                                             NULL,
	                                             0,
	                                             keyStr,
	                                             pa_backlight,
	                                             NULL);
	G_FREE(pa_backlight);
	__BACK_POINTER_SET(obj);
	if (obj) {
		obj->userdata = ad;
//		ad->data_backlight = obj;
	}

	return obj;
}

static Setting_GenGroupItem_Data *__developer_option_handler(void *data, char *keyStr, char *icon_path, char *ug_args, Evas_Object *genlist, Elm_Object_Item *parent)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *)data;
	Setting_GenGroupItem_Data *obj = NULL;
#ifdef BINARY_RELEASE_TYPE_ENG
	/*for eng binary: always show <developer option>*/
	obj = setting_create_Gendial_field_def(genlist,
	                                             &(ad->itc_table[GENDIAL_Type_1text_1icon_2]),
	                                             setting_main_click_list_ex_ug_cb,
	                                             ug_args,
	                                             SWALLOW_Type_1ICON_1IMAGE,
	                                             icon_path,
	                                             NULL, 0,
	                                             keyStr,
	                                             NULL,
	                                             NULL);

#else
	/*for user binary: need to check develop_option_state vconf value*/
	int dev_op_state = 0;
	int ret = 0;
	ret = vconf_get_bool(VCONFKEY_SETAPPL_DEVELOPER_OPTION_STATE, &dev_op_state);
	if (ret != 0) {
		SETTING_TRACE_ERROR("Failed to ger vconf value %d", ret);
	}
	if (dev_op_state) {
		obj = setting_create_Gendial_field_def(genlist,
		                                             &(ad->itc_table[GENDIAL_Type_1text_1icon_2]),
		                                             setting_main_click_list_ex_ug_cb,
		                                             ug_args,
		                                             SWALLOW_Type_1ICON_1IMAGE,

		                                             icon_path,
		                                             NULL, 0,
		                                             keyStr,
		                                             NULL,
		                                             NULL);
	} else {
		SETTING_TRACE("dev_op_state is 0");
	}
#endif
	return obj;
}

static Setting_GenGroupItem_Data *__default_handler(void *data, char *keyStr, char *icon_path, char *ug_args, Evas_Object *genlist, Elm_Object_Item *parent)
{
	/*SETTING_TRACE_BEGIN; */
	setting_main_appdata *ad = (setting_main_appdata *)data;
	Setting_GenGroupItem_Data *obj = setting_create_Gendial_field_def(genlist,
	                                                                        &(ad->itc_table[GENDIAL_Type_1text_1icon_2]),
	                                                                        setting_main_click_list_ex_ug_cb,
	                                                                        ug_args,
	                                                                        SWALLOW_Type_1ICON_1IMAGE,
	                                                                        icon_path,
	                                                                        NULL, 0,
	                                                                        keyStr,
	                                                                        NULL,
	                                                                        NULL);

	return obj;
}

/*//////////////////////////////////////////////////////////////////////////////////////////// */
/* list handler */
static mainlist_entry mainlist_table[] = {
	{KeyStr_FlightMode, 	__default_handler, UG_HANDLE, NULL},			/* 0 --> NOT UG */
	{KeyStr_WiFi, 			__default_handler, UI_PROC, NULL},				/* 1 --> UG */
	{KeyStr_Bluetooth, 		__default_handler, UI_PROC, NULL},
	{KeyStr_MobileAP, 		__mobileap_handler, UG_HANDLE, NULL},			/* 1 --> UG */
	{KeyStr_Location, 		__default_handler, UI_PROC, NULL},				/* 1 --> UG */
	{KeyStr_Network, 		__default_handler, UG_HANDLE, NULL},				/* 1 --> UG */
	{KeyStr_DeveloperOption, __developer_option_handler, UI_PROC, NULL},
	{KeyStr_BacklightTime, __backlight_time_handler, UI_PROC, NULL},
	{"Default", 			__default_handler, DEFAULT_UI, NULL},				/* 1 --> UG */
	/*-------------------------------------------------------------- */
	{NULL, NULL, ERROR_STATE, NULL},
};

/*----------------------------------------------------------------------------------------- */
/* hash table utility */
static void __list_hash_free_cb(void *obj)
{
	SETTING_TRACE_BEGIN;
	/*const char *name = key; */
	/*const char *number = data; */
	/*printf("%s: %s\n", name, number); */

}

/* hash table utility */
void settinig_drawer_hash_init(void *cb)
{
	SETTING_TRACE_BEGIN;

	setting_main_appdata *ad = (setting_main_appdata *) cb;
	eina_init();

	mainlist_entry *pnode = NULL;
	ad->main_list_hash = eina_hash_string_superfast_new(__list_hash_free_cb);

	for (pnode = &mainlist_table[0]; pnode->title != NULL; pnode++) {
		eina_hash_add(ad->main_list_hash , pnode->title, pnode);
		/*SETTING_TRACE("init and add data to hash : %s ", pnode->title); */
	}
}

/* hash table utility */
mainlist_entry *settinig_drawer_hash_find(void *cb, char *search_str)
{
	/*SETTING_TRACE_BEGIN; */
	/*SETTING_TRACE("------------------ HASH SEARCH ----------------------, %s", search_str); */
	setting_main_appdata *ad = (setting_main_appdata *) cb;
	mainlist_entry *pnode = NULL;

	pnode = eina_hash_find(ad->main_list_hash, search_str);
	return pnode;	/* statically allocated */
}
/*----------------------------------------------------------------------------------------- */


/* ***************************************************
 *
 *call back func
 *
 ***************************************************/


/* obj is the layout clicked */
void
setting_main_click_list_item_ug_cb(void *data, Evas_Object *obj,
                                   char *ug_to_load, app_control_h svc,
                                   struct ug_cbs *cbs)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *) data;
	if (!ug_to_load) {
		setting_create_popup(ad, ad->win_main, NULL, NO_UG_FOUND_MSG, NULL, 0, false, false, 0);
		return;
	}
	SETTING_TRACE("to create libug-%s.so", ug_to_load);
	elm_object_tree_focus_allow_set(ad->ly_main, EINA_FALSE);

	/*setting_conformant_keypad_state(ad->win_main, TRUE); */
	ad->ug = setting_ug_create(NULL, ug_to_load, UG_MODE_FULLVIEW, svc, cbs);
	if (ad->ug) {
		ad->isInUGMode = TRUE;
	} else {
		elm_object_tree_focus_allow_set(ad->ly_main, EINA_TRUE);
		evas_object_show(ad->ly_main);
		/*don't going to access globle var errno */
		/*SETTING_TRACE_ERROR("errno:%d", errno); */
		/*SETTING_TRACE_ERROR("Failed to load /usr/ug/lib/libug-%s.so", ug_to_load); */
		/*SETTING_TRACE_ERROR("Failed to load lib-%s.so", ug_to_load); */
		setting_create_popup(ad, ad->win_main, NULL, NO_UG_FOUND_MSG, NULL, 0, false, false, 0);
	}
}

void setting_main_click_list_ex_ug_cb(void *data, Evas_Object *obj,
                                      void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;

	elm_genlist_item_selected_set(item, EINA_FALSE);


	SETTING_TRACE("g_main_ad->isInUGMode:%d", g_main_ad->isInUGMode);
	SETTING_TRACE("g_main_ad->ug:%p", g_main_ad->ug);
	if (g_main_ad->isInUGMode && g_main_ad->ug) {
		SETTING_TRACE
		("[ad->ug non-NULL]skip genlist click event!!");

		/*SETTING_TRACE_END; */
		return;
	}

	setting_main_appdata *ad = g_main_ad;

	/*  if UG is created by Setting app, setting_main_click_list_ex_ug_cb is diabled. */
	if (ad->isInUGMode) {
		SETTING_TRACE("isInUGMode : TRUE - another UG is running now.");
		ad->isInUGMode = FALSE;
		return;
	} else {
		SETTING_TRACE ("isInUGMode : FALSE - another UG is NOT running now.")
	}

	Setting_GenGroupItem_Data *selected_item_data =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == selected_item_data, "selected_item_data is NULL");
	setting_main_click_list_item_reset_data(ad, obj);
	const char *item_name = _(selected_item_data->keyStr);
	SETTING_TRACE("item_name:%s", item_name);
	ret_if(!item_name);

	/* if current selection is for App control lauching style */
	Setting_Cfg_Node_T *pnode = get_cfg_node_by_keystr(selected_item_data->keyStr);
	if (pnode == NULL) {
		SETTING_TRACE_ERROR (" *** pnode is NULL *** ");
		return;
	}
	if (pnode && pnode->item_type == Cfg_Item_AppLauncher_Node) {
		if (app_launcher(data) == 0) {
			ad->event_freeze_timer = ecore_timer_add(1, setting_main_freeze_event_timer_cb, ad);
			evas_object_freeze_events_set(ad->navibar_main, EINA_TRUE);
		}
		int click_times = setting_cfg_get_click_times(selected_item_data->keyStr);
		setting_cfg_set_click_times(selected_item_data->keyStr, ++click_times);
		return;
	}

	/* handling of UG style */
	struct ug_cbs *cbs = (struct ug_cbs *)calloc(1, sizeof(struct ug_cbs));
	setting_retm_if(!cbs, "calloc failed");
	cbs->layout_cb = setting_main_layout_ug_cb;
	cbs->result_cb = setting_main_result_ug_cb;
	cbs->destroy_cb = setting_main_destroy_ug_cb;
	cbs->priv = (void *)ad;

	char *path = get_ug_path_from_ug_args(data);
	app_control_h svc = get_bundle_from_ug_args(data);
	setting_main_click_list_item_ug_cb(ad, obj, path, svc, cbs);
	if (path) {
		FREE(path);
	}
	if (cbs) {
		FREE(cbs);
	}
	path = NULL;
	cbs = NULL;
	app_control_destroy(svc);
}
