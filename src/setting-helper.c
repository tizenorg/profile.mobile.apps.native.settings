#include <setting-helper.h>

static bool __vcui_doc_rcs_ug_account_cb(account_h account, void *user_data)
{
	SETTING_TRACE_BEGIN;
	bool *temp = (bool *)user_data;
	*temp = TRUE;
	return FALSE;
}

bool vcui_doc_rcs_get_install_status(void)
{
	SETTING_TRACE_BEGIN;
	int ret;
	bool b_rcs_support = FALSE;

	ret = account_connect();
	if (ret != ACCOUNT_ERROR_NONE) {
		SETTING_TRACE_ERROR("Failed to connect [%d]", ret);
		return FALSE;
	}

	ret = account_query_account_by_capability_type(__vcui_doc_rcs_ug_account_cb, "joyn.samsung.com", (void *)&b_rcs_support);
	switch (ret) {
		case ACCOUNT_ERROR_INVALID_PARAMETER:
			SETTING_TRACE("Failed to query account [INVALID PARAM]");
			break;
		case ACCOUNT_ERROR_DB_FAILED:
			SETTING_TRACE("Failed to query account [DB OP FAIL]");
			break;
		case ACCOUNT_ERROR_NONE:
			SETTING_TRACE("Success to query account");
			break;
		default:
			SETTING_TRACE("Failed to query account [0x%08X]", ret);
			break;
	}

	ret = account_disconnect();
	if (ret != ACCOUNT_ERROR_NONE) {
		SETTING_TRACE_ERROR("Failed to disconnect [%d]", ret);
	}

	SETTING_TRACE("b_rcs_support is (%d)...", b_rcs_support);
	return b_rcs_support;
}
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

	ad->data_flight = NULL;
	ad->data_netrestrictionmode = NULL;
	ad->data_wifi = NULL;
	ad->data_bt = NULL;
	ad->data_mobileApp = NULL;
	ad->data_bright = NULL;
	ad->data_backlight = NULL;
	ad->data_battery = NULL;
	ad->data_landscape = NULL;
	ad->data_screentone = NULL;
	ad->data_screenmode = NULL;
	ad->data_touchkey_light_duration = NULL;
	ad->data_powersaving = NULL;
	ad->data_driving = NULL;
	ad->data_blocking = NULL;
	ad->data_personalpage = NULL;
	ad->data_guest = NULL;
	ad->data_high_touch_sensitivity = NULL;

	ad->data_moreSystem = NULL;
	ad->data_moreConnect = NULL;
	ad->data_nfc = NULL;
	ad->data_motion = NULL;
	ad->data_nearby_devices = NULL;
	ad->data_network = NULL;

	ad->data_language_input = NULL;
	ad->data_storage = NULL;
	ad->data_datetime = NULL;
	ad->data_about = NULL;

	SETTING_TRACE_END;
}

void setting_main_genlist_handler_set(void *data, int viewtype)
{
	retm_if(data == NULL, "Invalid argument: data is NULL");
	setting_main_appdata *ad = data;

	if (ad->data_wifi) {
		ad->gl_data_item[viewtype][GL_WiFi] = ad->data_wifi;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_WiFi]);
	}
	if (ad->data_bt) {
		ad->gl_data_item[viewtype][GL_BT] = ad->data_bt;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_BT]);
	}
	if (ad->data_flight) {
		ad->gl_data_item[viewtype][GL_FlightMode] = ad->data_flight;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_FlightMode]);
	}
	if (ad->data_netrestrictionmode) {
		ad->gl_data_item[viewtype][GL_NetRestriction] = ad->data_netrestrictionmode;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_NetRestriction]);
	}
	if (ad->data_mobileApp) {
		ad->gl_data_item[viewtype][GL_MobileApp] = ad->data_mobileApp;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_MobileApp]);
	}
	if (ad->data_bright) {
		ad->gl_data_item[viewtype][GL_Bright] = ad->data_bright;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Bright]);
	}
	if (ad->data_backlight) {
		ad->gl_data_item[viewtype][GL_Backlight] = ad->data_backlight;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Backlight]);
	}
	if (ad->data_battery) {
		ad->gl_data_item[viewtype][GL_Battery] = ad->data_battery;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Battery]);
	}
	if (ad->data_landscape) {
		ad->gl_data_item[viewtype][GL_Landscape] = ad->data_landscape;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Landscape]);
	}
	if (ad->data_screentone) {
		ad->gl_data_item[viewtype][GL_Screentone] = ad->data_screentone;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Screentone]);
	}
	if (ad->data_screenmode) {
		ad->gl_data_item[viewtype][GL_Screenmode] = ad->data_screenmode;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Screenmode]);
	}
	if (ad->data_touchkey_light_duration) {
		ad->gl_data_item[viewtype][GL_TouchkeyLight] = ad->data_touchkey_light_duration;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_TouchkeyLight]);
	}
	if (ad->data_powersaving) {
		ad->gl_data_item[viewtype][GL_PM] = ad->data_powersaving;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_PM]);
	}
	if (ad->data_driving) {
		ad->gl_data_item[viewtype][GL_DM] = ad->data_driving;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_DM]);
	}
	if (ad->data_blocking) {
		ad->gl_data_item[viewtype][GL_BM] = ad->data_blocking;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_BM]);
	}
	if (ad->data_personalpage) {
		ad->gl_data_item[viewtype][GL_PersonalPage] = ad->data_personalpage;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_PersonalPage]);
	}
	if (ad->data_guest) {
		ad->gl_data_item[viewtype][GL_GuestMode] = ad->data_guest;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_GuestMode]);
	}
	if (ad->data_high_touch_sensitivity) {
		ad->gl_data_item[viewtype][GL_TouchSens] = ad->data_high_touch_sensitivity;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_TouchSens]);
	}
	if (ad->data_nfc) {
		ad->gl_data_item[viewtype][GL_NFC] = ad->data_nfc;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_NFC]);
	}
	if (ad->data_motion) {
		ad->gl_data_item[viewtype][GL_Motion] = ad->data_motion;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Motion]);
	}

	if (ad->data_nearby_devices) {
		ad->gl_data_item[viewtype][GL_NearByDev] = ad->data_nearby_devices;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_NearByDev]);
	}
	if (ad->data_network) {
		ad->gl_data_item[viewtype][GL_Network] = ad->data_network;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Network]);
	}

	if (ad->data_language_input) {
		ad->gl_data_item[viewtype][GL_LanguageInput] = ad->data_language_input;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_LanguageInput]);
	}
	if (ad->data_storage) {
		ad->gl_data_item[viewtype][GL_Storage] = ad->data_storage;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_Storage]);
	}
	if (ad->data_datetime) {
		ad->gl_data_item[viewtype][GL_DateTime] = ad->data_datetime;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_DateTime]);
	}
	if (ad->data_about) {
		ad->gl_data_item[viewtype][GL_About] = ad->data_about;
		__BACK_POINTER_SET(ad->gl_data_item[viewtype][GL_About]);
	}

	int index = 0;
	for (; index < GL_Max; index++) {
		SETTING_TRACE("****** index = %d ---> value(%x) ", index, ad->gl_data_item[viewtype][index]);
	}
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

	if (ad->search_selected_item_data && ad->search_selected_item_data->item) {
		elm_genlist_item_fields_update(ad->search_selected_item_data->item,
		                               "elm.icon.2", ELM_GENLIST_ITEM_FIELD_CONTENT);
		ad->search_selected_item_data = NULL;
	}

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
	ea_screen_reader_support_set(ad->ly_main, TRUE);

	/* restoring keypad state */
	/*setting_conformant_keypad_state(ad->win_main, FALSE); */

	setting_main_click_list_item_reset_data(ad, NULL);
	Evas_Object *eo = elm_object_item_access_object_get(ad->ctrl_item[ad->view_type]);
	if (eo) {
		elm_object_focus_set(eo, TRUE);
	}
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
	setting_main_appdata *ad = (setting_main_appdata *) priv;

	if (service) {
		char *webkit_address = NULL;
		app_control_get_extra_data(service, "webkit_address", &webkit_address);
		if (webkit_address == NULL) {
			SETTING_TRACE("webkit_address is NULL");
			ad->webkit = NULL;
			return;
		}
		SETTING_TRACE("webkit_address = %s", webkit_address);

		int adress = (int) g_ascii_strtod(webkit_address, NULL);
		if (errno == ERANGE) {
			SETTING_TRACE_ERROR("fail to call g_ascii_strtod");
			ad->webkit = NULL;
		} else {
			SETTING_TRACE("adress = %d", adress);
			ad->webkit = (Evas_Object *)adress;
			SETTING_TRACE("webkit = %p", ad->webkit);
		}
		FREE(webkit_address);
	}

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
