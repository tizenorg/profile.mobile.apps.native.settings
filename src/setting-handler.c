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

	ad->data_mobileApp = setting_create_Gendial_field_groupitem(genlist,
	                                                            &(ad->itc[GENDIAL_Type_1text_1icon_2]),
	                                                            parent,
	                                                            setting_main_click_list_ex_ug_cb,
	                                                            ug_args,
	                                                            SWALLOW_Type_INVALID,
	                                                            icon_path,
	                                                            NULL,
	                                                            0,
	                                                            keyStr,
	                                                            NULL,
	                                                            NULL);
	__BACK_POINTER_SET(ad->data_mobileApp);
	if (ad->data_mobileApp) {
		ad->data_mobileApp->userdata = ad;
	}

	return ad->data_mobileApp;
}

static Setting_GenGroupItem_Data *__backlight_time_handler(void *data, char *keyStr, char *icon_path, char *ug_args, Evas_Object *genlist, Elm_Object_Item *parent)
{
	SETTING_TRACE_BEGIN;
	setting_main_appdata *ad = (setting_main_appdata *)data;
	Setting_GenGroupItem_Data *obj = NULL;

	char *pa_backlight = get_pa_backlight_time_str();
	obj = setting_create_Gendial_field_groupitem(genlist,
	                                             &(ad->itc[GENDIAL_Type_1icon_2text]),
	                                             parent,
	                                             setting_main_click_list_ex_ug_cb,
	                                             ug_args,
	                                             SWALLOW_Type_INVALID,
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
		ad->data_backlight = obj;
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
	obj = setting_create_Gendial_field_groupitem(genlist,
	                                             &(ad->itc[GENDIAL_Type_1text_1icon_2]),
	                                             parent,
	                                             setting_main_click_list_ex_ug_cb,
	                                             ug_args,
	                                             SWALLOW_Type_INVALID,
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
		obj = setting_create_Gendial_field_groupitem(genlist,
		                                             &(ad->itc[GENDIAL_Type_1text_1icon_2]),
		                                             parent,
		                                             setting_main_click_list_ex_ug_cb,
		                                             ug_args,
		                                             SWALLOW_Type_INVALID,
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
	Setting_GenGroupItem_Data *obj = setting_create_Gendial_field_groupitem(genlist,
	                                                                        &(ad->itc[GENDIAL_Type_1text_1icon_2]),
	                                                                        parent,
	                                                                        setting_main_click_list_ex_ug_cb,
	                                                                        ug_args,
	                                                                        SWALLOW_Type_INVALID,
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
		setting_create_simple_popup(ad, ad->win_main, NULL, NO_UG_FOUND_MSG);
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
		setting_create_simple_popup(ad, ad->win_main, NULL, NO_UG_FOUND_MSG);
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
		/*SETTING_TRACE_END; */
		return;
	} else {
		SETTING_TRACE
		("isInUGMode : FALSE - another UG is NOT running now.")
	}

	Setting_GenGroupItem_Data *selected_item_data =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	setting_retm_if(NULL == selected_item_data, "selected_item_data is NULL");
	setting_main_click_list_item_reset_data(ad, obj);
	const char *item_name = _(selected_item_data->keyStr);
	SETTING_TRACE("item_name:%s", item_name);
	ret_if(!item_name);

	if (!safeStrCmp(selected_item_data->keyStr, KeyStr_Call)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_WiFi)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_Location)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_Bluetooth)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_MobileAP)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_WiFiDirect)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_Wallpaper)
	    || !safeStrCmp(selected_item_data->keyStr, KeyStr_Accessibility)
	   ) {
		if (app_launcher(data) == 0) {
			ad->event_freeze_timer = ecore_timer_add(1, setting_main_freeze_event_timer_cb, ad);
			evas_object_freeze_events_set(ad->navibar_main, EINA_TRUE);
		}
		int click_times = setting_cfg_get_click_times(selected_item_data->keyStr);
		setting_cfg_set_click_times(selected_item_data->keyStr, ++click_times);
		return;
	}

	/*2.Quick command cannot work when TTS on */
	int tts_state = 0;
	vconf_get_bool(VCONFKEY_SETAPPL_ACCESSIBILITY_TTS, &tts_state);
	if (tts_state) {
		if (!safeStrCmp(selected_item_data->keyStr, KeyStr_QUICK_CMD)) {
			char noti_str[MAX_SPECIALIZITION_LEN + 1] = { 0, };
			snprintf(noti_str, sizeof(noti_str), _(FEATURE_UNAVALIABLE_WHILE_TTS_ON), _(KeyStr_QUICK_CMD));
			setting_create_popup_without_btn(NULL, ad->win_main, NULL, _(noti_str),
			                                 NULL, 0.0, TRUE, FALSE);
			return;
		}
	}

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

void setting_main_list_mouse_up_cb(void *data, Evas_Object *eo,
                                   void *event_info)
{
	/*SETTING_TRACE_BEGIN; */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SETTING_TRACE("process item [%s]", list_item->keyStr);
	list_item->chk_status = elm_check_state_get(eo);	/* for update new state */

	SETTING_TRACE("change radio[%s], status is:%d", _(list_item->keyStr),
	              list_item->chk_status);
	setting_main_appdata *ad = g_main_ad;

	if (!safeStrCmp(KeyStr_UsePacketData, list_item->keyStr)) {
		int err;
		if (elm_check_state_get(eo)) {
			setting_set_bool_slp_key
			(BOOL_SLP_SETTING_USE_PACKET_DATA,
			 1, &err);
		} else {
			setting_set_bool_slp_key
			(BOOL_SLP_SETTING_USE_PACKET_DATA,
			 0, &err);
		}
	}
	return;
}




#if SUPPORT_TETHERING
static void ___wifi_activated_cb(wifi_error_e result, void *user_data)
{
	SETTING_TRACE_BEGIN;
	if (result == WIFI_ERROR_NONE) {
		SETTING_TRACE("Wi-Fi Activation Succeeded");
	} else {
		SETTING_TRACE("Wi-Fi Activation Failed! error : %d", result);
	}

	(void)wifi_deinitialize();
}

static void __tethering_disabled_cb(tethering_error_e error, tethering_type_e type, tethering_disabled_cause_e code, void *data)
{
	int ret;

	if (data) {
		tethering_h th = (tethering_h)data;
		tethering_destroy(th);
	}

	ret = wifi_initialize();
	setting_retm_if(ret < 0, "*** [ERR] wifi_initialize() ***");

	ret = wifi_activate(___wifi_activated_cb, NULL);
	if (ret < 0) {
		SETTING_TRACE_ERROR("*** [ERR] wifi_activate() ***");
		(void)wifi_deinitialize();
		return;
	}
}
#endif

static Eina_Bool __wifi_timeout(void *data)
{
	/*SETTING_TRACE_BEGIN; */
	retvm_if(!data, ECORE_CALLBACK_CANCEL, "Invalid argument: data is NULL");
	Setting_GenGroupItem_Data *list_item = data;
	setting_retm_if(NULL == list_item, "list_item is NULL");
	int status, err;
	setting_get_int_slp_key(INT_SLP_SETTING_WIFI_STATUS, &status, &err);
	SETTING_TRACE("value:%d", status);

	int i;
	for (i = VIEW_All_List; i < VIEW_Max; i++) {
		list_item = g_main_ad->gl_data_item[i][GL_WiFi];
		if (list_item) {
			list_item->chk_status = status;
			list_item->swallow_type = SWALLOW_Type_1ICON_1RADIO;
			elm_object_item_data_set(list_item->item, list_item);
			elm_genlist_item_update(list_item->item);
		}
	}

	if (list_item) {
		setting_main_appdata *ad = list_item->userdata;
		ad->wifi_timer = NULL;
	}

	return ECORE_CALLBACK_CANCEL;
}

#if SUPPORT_TETHERING
static void __tethering_turn_off_resp_cb(void *data, Evas_Object *obj, void *event_info)
{
	setting_retm_if(NULL == obj, "NULL == obj");
	int response_type = btn_type(obj);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *)data;

	if (g_main_ad) {
		evas_object_del(g_main_ad->popup_wifi_mode);
		g_main_ad->popup_wifi_mode = NULL;
	}

	int i = 0;
	Setting_GenGroupItem_Data *update_item = NULL;
	if (POPUP_RESPONSE_OK == response_type) {
		/*add timeout strategy.. */
		ret_if(!list_item->userdata);
		setting_main_appdata *ad = list_item->userdata;
		if (ad->wifi_timer) {
			ecore_timer_del(ad->wifi_timer);
			ad->wifi_timer = NULL;
		}
		ad->wifi_timer = ecore_timer_add(5.0, (Ecore_Task_Cb) __wifi_timeout, list_item);

/*#if SUPPORT_TETHERING*/
		int type = tethering_is_enabled(NULL, TETHERING_TYPE_WIFI) ? TETHERING_TYPE_WIFI : TETHERING_TYPE_WIFI_AP;

		tethering_h th = NULL;
		tethering_error_e ret = TETHERING_ERROR_NONE;

		ret = tethering_create(&th);
		if (ret != TETHERING_ERROR_NONE) {
			for (i = VIEW_All_List; i < VIEW_Max; i++) {
				update_item = ad->gl_data_item[i][GL_WiFi];
				if (update_item) {
					update_item->chk_status = EINA_FALSE;
					update_item->swallow_type = SWALLOW_Type_1ICON_1RADIO;
					elm_object_item_data_set(update_item->item, update_item);
					elm_genlist_item_update(update_item->item);
				}
			}
		}

		ret = tethering_set_disabled_cb(th, type, __tethering_disabled_cb, th);
		if (ret != TETHERING_ERROR_NONE) {
			for (i = VIEW_All_List; i < VIEW_Max; i++) {
				update_item = ad->gl_data_item[i][GL_WiFi];
				if (update_item) {
					update_item->chk_status = EINA_FALSE;
					update_item->swallow_type = SWALLOW_Type_1ICON_1RADIO;
					elm_object_item_data_set(update_item->item, update_item);
					elm_genlist_item_update(update_item->item);
				}
			}

			tethering_destroy(th);
			return;
		}

		(void) tethering_disable(th, type);
/*#endif*/
	} else if (POPUP_RESPONSE_CANCEL == response_type) {
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			update_item = g_main_ad->gl_data_item[i][GL_WiFi];
			if (update_item) {
				update_item->chk_status = EINA_FALSE;
				update_item->swallow_type = SWALLOW_Type_1ICON_1RADIO;
				elm_object_item_data_set(update_item->item, update_item);
				elm_genlist_item_update(update_item->item);
			}
		}
	}
}
#endif


void __alternate_wifi_mode(Setting_GenGroupItem_Data *list_item, Evas_Object *check)
{
	/*SETTING_TRACE_BEGIN; */
	/*int ret; */
	Eina_Bool status =  elm_check_state_get(check);
	SETTING_TRACE("wifi mode status : %d", status);

	/*int value, err; */
	int i = 0;
	Setting_GenGroupItem_Data *item_to_update = NULL;

#if SUPPORT_TETHERING
	if (tethering_is_enabled(NULL, TETHERING_TYPE_WIFI)
	    || tethering_is_enabled(NULL, TETHERING_TYPE_WIFI_AP)) {
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = g_main_ad->gl_data_item[i][GL_WiFi];
			if (item_to_update) {
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1PROCESS;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_fields_update(item_to_update->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
			}
		}

		g_main_ad->popup_wifi_mode = setting_create_popup_with_btn(list_item,
		                                                           g_main_ad->win_main, NULL,
		                                                           "IDS_MOBILEAP_POP_TURNING_ON_WI_FI_WILL_DISABLE_MOBILE_HOTSPOT_CONTINUE_Q_VZW",
		                                                           __tethering_turn_off_resp_cb,
		                                                           0, 2, "IDS_ST_BUTTON_OK", "IDS_ST_BUTTON_CANCEL_ABB");
	} else
#endif
	{
		int wifi_state = VCONFKEY_WIFI_OFF;
		vconf_get_int(VCONFKEY_WIFI_STATE, &wifi_state);
		DBusMessageIter iter;
		DBusMessage *message = NULL;
		DBusConnection *connection = NULL;
		SETTING_TRACE("status:%d", status);
		if (status) {
			if (wifi_state != VCONFKEY_WIFI_OFF) {
				/* Wi-Fi is enabled */
				SETTING_TRACE("Wi-Fi is already enabled");
				return ;
			}
			connection = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
			if (connection == NULL) {
				SETTING_TRACE_ERROR("Failed to get system bus");
				return;
			}

			message = dbus_message_new_method_call(NETCONFIG_SERVICE, NETCONFIG_WIFI_PATH,
			                                       NETCONFIG_WIFI_INTERFACE, "LoadDriver");
			if (message == NULL) {
				SETTING_TRACE_ERROR("Failed DBus method call");
				dbus_connection_unref(connection);
				return ;
			}

			g_main_ad->wifi_op = OP_WIFI_TURNING_ON;

			dbus_message_iter_init_append(message, &iter);
			dbus_bool_t val = FALSE;
			dbus_message_iter_append_basic(&iter, DBUS_TYPE_BOOLEAN, &val);

			dbus_connection_send_with_reply(connection, message, NULL, 10000);

			dbus_message_unref(message);
			dbus_connection_unref(connection);

		} else {
			if (wifi_state == VCONFKEY_WIFI_OFF) {
				/* Wi-Fi is disabled */
				SETTING_TRACE("Wi-Fi is already disabled");
				return ;
			}

			connection = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
			if (connection == NULL) {
				SETTING_TRACE_ERROR("Failed to get system bus");
				return;
			}

			message = dbus_message_new_method_call(NETCONFIG_SERVICE, NETCONFIG_WIFI_PATH,
			                                       NETCONFIG_WIFI_INTERFACE, "RemoveDriver");
			if (message == NULL) {
				SETTING_TRACE_ERROR("Failed DBus method call");
				dbus_connection_unref(connection);
				return;
			}
			g_main_ad->wifi_op = OP_WIFI_TURNING_OFF;

			dbus_connection_send_with_reply(connection, message, NULL, 10000);

			dbus_message_unref(message);
			dbus_connection_unref(connection);

		}

		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = g_main_ad->gl_data_item[i][GL_WiFi];
			if (item_to_update) {
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1PROCESS;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				elm_genlist_item_fields_update(item_to_update->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);

			}
		}

		/*add timeout strategy.. */
		ret_if(!list_item->userdata);
		setting_main_appdata *ad = list_item->userdata;
		if (ad->wifi_timer) {
			ecore_timer_del(ad->wifi_timer);
			ad->wifi_timer = NULL;
		}
		ad->wifi_timer = ecore_timer_add(5.0, (Ecore_Task_Cb) __wifi_timeout, list_item);
	}
}

static Eina_Bool __bt_timeout(void *data)
{
	/*SETTING_TRACE_BEGIN; */
	retvm_if(!data, ECORE_CALLBACK_CANCEL, "Invalid argument: data is NULL");
	int status, err;
	setting_get_bool_slp_key(INT_SLP_SETTING_BT_STATUS, &status, &err);
	SETTING_TRACE("value:%d", status);

	setting_main_appdata *ad = data;
	Setting_GenGroupItem_Data *item_to_update;
	int i;
	for (i = VIEW_All_List; i < VIEW_Max; i++) {
		item_to_update = ad->gl_data_item[i][GL_BT];
		SETTING_TRACE("[%d]item_to_update:%p", i, item_to_update);
		if (item_to_update) {
			item_to_update->chk_status = status;
			item_to_update->swallow_type = SWALLOW_Type_1ICON_1RADIO;
			elm_object_item_data_set(item_to_update->item, item_to_update);
			/*elm_genlist_item_fields_update(item_to_update->item, "*", ELM_GENLIST_ITEM_FIELD_STATE); */
			elm_genlist_item_update(item_to_update->item);
		}
	}


	ad->bt_timer = NULL;
	return ECORE_CALLBACK_CANCEL;
}

void __alternate_bt_mode(Setting_GenGroupItem_Data *list_item, Evas_Object *check)
{
	/*SETTING_TRACE_BEGIN; */
	int ret;
	Eina_Bool status =  elm_check_state_get(check);
	SETTING_TRACE("bt mode status : %d", status);

	bt_adapter_state_e value;
	ret = bt_initialize();
	if (bt_adapter_get_state(&value) != BT_ERROR_NONE) {
		SETTING_TRACE_ERROR("bt_adapter_get_state() failed ");
		return;
	}

	setting_main_appdata *ad = g_main_ad;

	if (status == value) {
		SETTING_TRACE("status == value");
		int i;
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_force_update_gl_item_chk_status(ad->gl_data_item[i][GL_BT], value);
		}
		return;
	}
	do {
		/*add timeout strategy.. */
		Setting_GenGroupItem_Data *item_to_update;
		int i;
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			item_to_update = ad->gl_data_item[i][GL_BT];
			SETTING_TRACE("[%d]item_to_update:%p", i, item_to_update);
			if (item_to_update) {
				item_to_update->swallow_type = SWALLOW_Type_1ICON_1PROCESS;
				elm_object_item_data_set(item_to_update->item, item_to_update);
				/*elm_genlist_item_update(list_item->item); */
				elm_genlist_item_fields_update(item_to_update->item, "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
			}
		}
		SETTING_TRACE("1111");
		if (ad->bt_timer) {
			ecore_timer_del(ad->bt_timer);
			ad->bt_timer = NULL;
		}
		ad->bt_timer = ecore_timer_add(5.0, (Ecore_Task_Cb) __bt_timeout, ad);

		if (ret < 0) {
			break;
		}
		status ? (ret = bt_adapter_enable()) : (ret = bt_adapter_disable());
		if (ret < 0) {
			break;
		}
		ret = bt_deinitialize();
		if (ret < 0) {
			break;
		}

		return;
	} while (0);

	int err;
	int val = 0;
	setting_get_int_slp_key(INT_SLP_SETTING_BT_STATUS, &val, &err);
	setting_force_update_gl_item_chk_status(list_item, val);
}


void setting_main_high_touch_sens_chk_btn_cb(void *data, Evas_Object *obj, void *event_info)
{
#if 0
	/*SETTING_TRACE_BEGIN; */
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */
	//display_set_enhanced_touch(list_item->chk_status);

	//int status = display_get_enhanced_touch();
	int status = display_get_enhanced_touch();
	SETTING_TRACE("display_get_enhanced_touch():%d", status);
	if (status >= 0) {
		vconf_set_bool(VCONFKEY_SETAPPL_ENHANCED_TOUCH, (1 == status)); /*sync to vconf */
	} else {
		setting_create_simple_popup(NULL, g_main_ad->win_main,
		                            NULL, "IDS_IM_POP_UNEXPECTED_ERROR");
		/*rollback status */
		int i;
		for (i = VIEW_All_List; i < VIEW_Max; i++) {
			setting_force_update_gl_item_chk_status(g_main_ad->gl_data_item[i][GL_TouchSens], !(list_item->chk_status));
		}
	}
#endif
}

