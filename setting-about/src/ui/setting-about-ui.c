#include "ui/setting-about-ui.h"
#include <Elementary.h>
#include <app_i18n.h>

typedef struct
{
	Evas_Object *win;
	Evas_Object *main_list;
	Elm_Genlist_Item_Class *item_class;
} seting_about_data;

typedef struct
{
	const char *title;
	ui_element_id id;
	seting_about_data *submenu;
	Evas_Smart_Cb click_cb;
} seting_about_menu_node;

static char *__item_class_get_text(void *data, Evas_Object *obj, const char *part);
static Evas_Object *__item_calss_get_content(void *data, Evas_Object *obj, const char *part);
static void __create_main_menu(void);
static void _device_name_change_cb(void *data, Evas_Object *obj, void *event_info);

static seting_about_data view_data = {0,};

static seting_about_menu_node main_menu[] = {
	{"IDS_ST_MBODY_SOFTWARE_UPDATE", UI_ID_SOFTWARE_UPDATE, NULL, NULL},
	{"IDS_ST_HEADER_MANAGE_CERTIFICATES_ABB", UI_ID_MANAGE_CERTIFICATES, NULL, NULL},
	{"IDS_ST_BODY_LICENCE", UI_ID_LICENCE, NULL, NULL},
	{"IDS_ST_BODY_NAME", UI_ID_NAME, NULL, &_device_name_change_cb},
	{"IDS_ST_BODY_MY_NUMBER", UI_ID_MY_NUMBER, NULL, NULL},
	{"IDS_CAM_POP_MODEL", UI_ID_MODEL, NULL, NULL},
	{"IDS_ST_BODY_VERSION", UI_ID_VERSION, NULL, NULL},
	{"IDS_ST_BODY_IMEI", UI_ID_IMEI, NULL, NULL},
	{"IDS_ST_BODY_BLUETOOTH", UI_ID_BLUETOOTH, NULL, NULL},
	{"IDS_ST_HEADER_WI_FI", UI_ID_WI_FI, NULL, NULL},
	{"IDS_ST_BODY_SERIAL_NUMBER", UI_ID_SERIAL_NUMBER, NULL, NULL},
	{"IDS_ST_BODY_BATTERY", UI_ID_BATTERY, NULL, NULL},
	{"IDS_ST_BODY_CPU_USAGE", UI_ID_CPU_USAGE, NULL, NULL},
	{"IDS_ST_MBODY_DEVICE_STATUS", UI_ID_DEVICE_STATUS, NULL, NULL}
};

bool setting_about_ui_init(void)
{
	view_data.win = elm_win_util_standard_add("Setting about", "Setting about");
	evas_object_show(view_data.win);

	view_data.main_list =  elm_genlist_add(view_data.win);

	view_data.item_class = elm_genlist_item_class_new();
	view_data.item_class->item_style = "default";
	view_data.item_class->func.text_get = __item_class_get_text;
	view_data.item_class->func.content_get = __item_calss_get_content;
	view_data.item_class->func.state_get = NULL;
	view_data.item_class->func.del = NULL;

	__create_main_menu();
	evas_object_size_hint_weight_set(view_data.main_list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(view_data.win, view_data.main_list);
	evas_object_show(view_data.main_list);

	return true;
}

void setting_about_ui_deinit(void)
{
	evas_object_del(view_data.win);
}

void setting_about_ui_display_name(const char *str)
{

}

void setting_about_ui_display_phone_no(const char *str)
{

}

void setting_about_ui_display_model(const char *str)
{

}

void setting_about_ui_display_version(const char *str)
{

}

void setting_about_ui_display_imei(const char *str)
{

}

void setting_about_ui_display_bt_addr(const char *str)
{

}

void setting_about_ui_display_wifi_mac(const char *str)
{

}

void setting_about_ui_display_sn(const char *str)
{

}

void setting_about_ui_display_batt_pow(const char *str)
{

}

void setting_about_ui_display_cpu_usage(const char *str)
{

}

void setting_about_ui_display_dev_status(const char *str)
{

}

void setting_about_ui_display_sec_status(const char *str)
{

}

static char *__item_class_get_text(void *data, Evas_Object *obj, const char *part)
{
	seting_about_menu_node *menu_node = (seting_about_menu_node *)data;
	if (!menu_node)
		return NULL;

	return strdup(_(menu_node->title));
}

static Evas_Object *__item_calss_get_content(void *data, Evas_Object *obj, const char *part)
{
   return NULL;
}

static void __create_main_menu(void)
{
	int i = 0;
	for(i = 0; i < (sizeof(main_menu) / sizeof(seting_about_menu_node)); i++)
		elm_genlist_item_append(view_data.main_list,
								view_data.item_class, (void *)&(main_menu[i]),
								NULL, ELM_GENLIST_ITEM_NONE,
								main_menu[i].click_cb, (void *)&(main_menu[i])
								);
}

static void _device_name_change_cb(void *data, Evas_Object *obj, void *event_info)
{
	Evas_Object *btn = NULL;
	Evas_Object *popup = elm_popup_add(view_data.win);
	evas_object_size_hint_weight_set(popup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_popup_align_set(popup, ELM_NOTIFY_ALIGN_FILL, 1.0);
	elm_object_part_text_set(popup, "title,text", "Rename device");
	elm_object_text_set(popup, "Device names are displayed to distinguish each "\
								"of the devices available in the nearby devices list, "\
								"and via Bluetooth, Wi-Fi Direct, and other methods."
						);


	btn = elm_button_add(popup);
	elm_object_text_set(btn, "Cancel");
	elm_object_part_content_set(popup, "button1", btn);
	//evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	btn = elm_button_add(popup);
	elm_object_text_set(btn, "OK");
	elm_object_part_content_set(popup, "button2", btn);
	//evas_object_smart_callback_add(btn, "clicked", _response_cb, popup);

	//eext_object_event_callback_add(popup, EEXT_CALLBACK_BACK, eext_popup_back_cb, NULL);

	evas_object_show(popup);
}
/*
static void __setting_about_main_creat_name_view(void *data)
{
	SETTING_TRACE_BEGIN;

	setting_retm_if(data == NULL, "Data parameter is NULL");
	SettingAboutUG *ad =	data;

	Evas_Object *scroller = NULL;
	scroller = elm_genlist_add(ad->navi_bar);
	setting_retm_if(scroller == NULL, "Cannot set scroller object  as content of layout");
	elm_genlist_clear(scroller);
	elm_genlist_mode_set(scroller, ELM_LIST_COMPRESS);
	evas_object_smart_callback_add(scroller, "realized", __gl_realized_cb, NULL);


	char *name_value = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
	char *pa_sub_desc = elm_entry_utf8_to_markup(name_value);
	FREE(name_value);

	ad->empty_flag = FALSE;
	if (NULL == pa_sub_desc || '\0' == pa_sub_desc[0]) {
		ad->empty_flag = TRUE;
	}

	Setting_GenGroupItem_Data *item_data = (Setting_GenGroupItem_Data *)calloc(1, sizeof(Setting_GenGroupItem_Data));
	if (!item_data) {
		FREE(pa_sub_desc);
	}
	setting_retm_if(!item_data, "calloc failed");

	item_data->keyStr = (char *)g_strdup(SETTING_ABOUT_DEVICE_NAME_STR);
	item_data->sub_desc = (char *)g_strdup(pa_sub_desc);
	item_data->swallow_type = SWALLOW_Type_LAYOUT_EDITFIELD;
	item_data->chk_status = 0;
	item_data->chk_change_cb = __setting_about_entry_device_name_changed_cb;
	item_data->userdata = ad;

	item_data->isSinglelineFlag = TRUE;
	item_data->stop_change_cb = __setting_about_entry_unfocus_cb;
	item_data->maxlength_reached_cb = __setting_about_entry_max_len_reached;
	item_data->focus_cb = __setting_about_entry_focused;
	item_data->x_callback_cb = __setting_about_entry_input_panel_event_cb;
	item_data->guide_text = (char *)g_strdup(EMPTY_LIMITATION_STR);
	item_data->focus_unallowed = get_tethering_status();
	item_data->return_key_type = ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE;

	item_data->limit_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Limit_Size));
	if (item_data->limit_filter_data) {

		item_data->limit_filter_data->max_byte_count = MAX_DEVICE_NAME_LEN;
		item_data->win_main = ad->win_get;
	} else {
		SETTING_TRACE_ERROR("fail to calloc");
	}

	item_data->digits_filter_data = calloc(1, sizeof(Elm_Entry_Filter_Accept_Set));
	if (item_data->digits_filter_data) {
		item_data->digits_filter_data->accepted = NULL;
	} else {
		SETTING_TRACE_ERROR("fail to calloc");
	}

	item_data->input_panel_disable_flag = EINA_TRUE;


	item_data->item = elm_genlist_item_append(scroller, &(itc_editfield), item_data, NULL, ELM_GENLIST_ITEM_NONE,
											  NULL, ad);

	ad->item_dev_name = item_data;
	if (ad->item_dev_name) {
		__BACK_POINTER_SET(ad->item_dev_name);
		elm_genlist_item_select_mode_set(ad->item_dev_name->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	} else {
		SETTING_TRACE_ERROR("ad->item_dev_name is NULL");
	}

	FREE(pa_sub_desc);

	Elm_Object_Item *navi_it = setting_push_layout_navi_bar(_(DEVICE_NAME),
															NULL, NULL, NULL,
															NULL, NULL, NULL,
															ad, scroller, ad->navi_bar, NULL);
	elm_naviframe_item_pop_cb_set(navi_it, __setting_about_child_view_back_cb, ad);

	Evas_Object *btn;
	btn = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn, "naviframe/title_cancel");
	evas_object_smart_callback_add(btn, "clicked", setting_about_naviframe_btn_cancel_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_left_btn", btn);

	btn = elm_button_add(ad->navi_bar);
	elm_object_style_set(btn, "naviframe/title_done");
	evas_object_smart_callback_add(btn, "clicked", setting_about_naviframe_btn_done_cb, ad);
	elm_object_item_part_content_set(navi_it, "title_right_btn", btn);

	ad->event_handler = ecore_event_handler_add(ECORE_EVENT_KEY_DOWN, __setting_about_name_view_key_down, ad);
	ad->name_update_idler = ecore_idler_add(setting_about_name_focus_update_cb, ad);
}
*/
