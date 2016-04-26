#include "ui/setting-about-ui.h"
#include <Elementary.h>
#include <app_i18n.h>
#include "setting-common-general-func.h"
#include "setting-common-draw-widget.h"
#include "ui/setting-about-ui-common.h"

#define ABOUT_LOCALEDIR	_TZ_SYS_RO_APP"/org.tizen.setting-about/res/locale"
#define ABOUTEDJ	_TZ_SYS_RO_APP"/org.tizen.setting-about/res/edje"
#define ABOUT_THEME_EDJ_NAME	ABOUTEDJ"/setting-theme.edj"
#define ABOUT_NEWUX_EDJ_NAME	ABOUTEDJ"/setting-newux.edj"
#define ABOUT_GENLIST_EDJ_NAME	ABOUTEDJ"/setting-genlist.edj"
#define ABOUT_NEW_GENLIST_EDJ_NAME	ABOUTEDJ"/setting-new-genlist.edj"
#define ABOUT_SLIDER_EDJ_NAME	ABOUTEDJ"/setting-slider.edj"

typedef struct
{
	Evas_Object *win;
	Evas_Object *main_list;
	Evas_Object *naviframe;
} seting_about_data;


static char *__item_class_get_text(void *data, Evas_Object *obj, const char *part);
static Evas_Object *__item_calss_get_content(void *data, Evas_Object *obj, const char *part);
static void __create_main_menu(void);
static void __create_itc(const char *style, Elm_Genlist_Item_Class *itc);
static void _device_name_change_cb(void *data, Evas_Object *obj, void *event_info);

static seting_about_data view_data = {0,};

static seting_about_menu_node main_menu[] = {
	[UI_ID_SOFTWARE_UPDATE] =
	{"IDS_ST_MBODY_SOFTWARE_UPDATE", {'\0',}, UI_ID_SOFTWARE_UPDATE, NULL, NULL, &(itc_1)},
	[UI_ID_MANAGE_CERTIFICATES] =
	{"IDS_ST_HEADER_MANAGE_CERTIFICATES_ABB", {'\0',}, UI_ID_MANAGE_CERTIFICATES, NULL, NULL, &(itc_1)},
	[UI_ID_LICENCE] =
	{"IDS_ST_MBODY_LEGAL_INFORMATION_ABB", {'\0',}, UI_ID_LICENCE, NULL, NULL, &(itc_1)},
	[UI_ID_DEVICE_INFO] =
	{"IDS_ST_BODY_DEVICE_INFORMATION", {'\0',}, UI_ID_DEVICE_INFO, NULL, NULL, &(itc_group)},
	[UI_ID_NAME] =
	{"IDS_ST_BODY_NAME", {'\0',}, UI_ID_NAME, NULL, &_device_name_change_cb, &(itc_2)},
	[UI_ID_MY_NUMBER] =
	{"IDS_ST_BODY_MY_NUMBER", {'\0',}, UI_ID_MY_NUMBER, NULL, NULL, &(itc_2)},
	[UI_ID_MODEL] =
	{"IDS_CAM_POP_MODEL", {'\0',}, UI_ID_MODEL, NULL, NULL, &(itc_2)},
	[UI_ID_VERSION] =
	{"IDS_ST_BODY_VERSION", {'\0',}, UI_ID_VERSION, NULL, NULL, &(itc_2)},
	[UI_ID_IMEI] =
	{"IDS_ST_BODY_IMEI", {'\0',}, UI_ID_IMEI, NULL, NULL, &(itc_2)},
	[UI_ID_BLUETOOTH] =
	{"IDS_ST_BODY_BLUETOOTH", {'\0',}, UI_ID_BLUETOOTH, NULL, NULL, &(itc_2)},
	[UI_ID_WI_FI] =
	{"IDS_ST_HEADER_WI_FI", {'\0',}, UI_ID_WI_FI, NULL, NULL, &(itc_2)},
	[UI_ID_SERIAL_NUMBER] =
	{"IDS_ST_BODY_SERIAL_NUMBER", {'\0',}, UI_ID_SERIAL_NUMBER, NULL, NULL, &(itc_2)},
	[UI_ID_BATTERY] =
	{"IDS_ST_BODY_BATTERY", {'\0',}, UI_ID_BATTERY, NULL, NULL, &(itc_2)},
	[UI_ID_CPU_USAGE] =
	{"IDS_ST_BODY_CPU_USAGE", {'\0',}, UI_ID_CPU_USAGE, NULL, NULL, &(itc_2)},
	[UI_ID_DEVICE_STATUS] =
	{"IDS_ST_MBODY_DEVICE_STATUS", {'\0',}, UI_ID_DEVICE_STATUS, NULL, NULL, &(itc_2)},
	[UI_ID_SECURITY_STATUS] =
	{"IDS_ST_TMBODY_SECURITY_STATUS", {'\0',}, UI_ID_SECURITY_STATUS, NULL, NULL, &(itc_2)}
};

bool setting_about_ui_init(void)
{
	Evas_Object *conform = NULL;

	setting_set_i18n("setting", ABOUT_LOCALEDIR);
	elm_theme_extension_add(NULL, ABOUT_THEME_EDJ_NAME);
	elm_theme_extension_add(NULL, ABOUT_NEWUX_EDJ_NAME);
	elm_theme_extension_add(NULL, ABOUT_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, ABOUT_NEW_GENLIST_EDJ_NAME);
	elm_theme_extension_add(NULL, ABOUT_SLIDER_EDJ_NAME);

	view_data.win = elm_win_util_standard_add("Setting about", "Setting about");
	elm_win_indicator_mode_set(view_data.win, ELM_WIN_INDICATOR_SHOW);
	elm_win_indicator_opacity_set(view_data.win, ELM_WIN_INDICATOR_OPAQUE);
	evas_object_show(view_data.win);

    conform = elm_conformant_add(view_data.win);
    evas_object_size_hint_weight_set(conform, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	elm_win_resize_object_add(view_data.win, conform);
	evas_object_show(conform);

	view_data.naviframe = elm_naviframe_add(conform);
	elm_naviframe_prev_btn_auto_pushed_set(view_data.naviframe, EINA_TRUE);
	elm_object_part_content_set(conform, "elm.swallow.content", view_data.naviframe);
	evas_object_show(view_data.naviframe);

	view_data.main_list = elm_genlist_add(view_data.naviframe);
	__create_itc(SETTING_GENLIST_ICON_1LINE_STYLE, &(itc_1));
	__create_itc(SETTING_GENLIST_2LINE_STYLE, &(itc_2));
	//__create_itc(SETTING_GENLIST_2LINE_STYLE, &(itc_2text_3_parent));
	//__create_itc(SETTING_GENLIST_2LINE_STYLE, &(itc_1icon_1text_sub));
	__create_itc(SETTING_GENLIST_GROUP_INDEX_STYLE, &(itc_group));
	//__create_itc("multiline/1text", &(itc_help_style));
	__create_main_menu();
	elm_naviframe_item_push(view_data.naviframe, _("IDS_ST_BODY_ABOUT_DEVICE"), NULL, NULL, view_data.main_list, NULL);
	evas_object_show(view_data.main_list);

	return true;
}

void setting_about_ui_deinit(void)
{
	evas_object_del(view_data.win);
}

void setting_about_ui_display_attribute(ui_element_id uid, const char *str)
{
	strncpy(main_menu[uid].txt, (char *)str, MAX_TXT_LEN);
}

static char *__item_class_get_text(void *data, Evas_Object *obj, const char *part)
{
	seting_about_menu_node *menu_node = (seting_about_menu_node *)data;

	if (!menu_node)
		return NULL;

	/* style == multiline ---------> "elm.text.multiline" */
	/*------------------------------------------------------------- */
	/* style != multiline ---------> "elm.text" */
	/*                               "elm.text.sub" */

	if (!safeStrCmp(part, "elm.text.multiline") ||
		!safeStrCmp(part, "elm.text") ||
		!safeStrCmp(part, "elm.text.main") ||
		!safeStrCmp(part, "elm.text.main.left") ||
		!safeStrCmp(part, "elm.text.main.left.top") ||
		!safeStrCmp(part, "elm.text.multiline")
		)
		return (char *)g_strdup(_(menu_node->title));

	if (!safeStrCmp(part, "elm.text.sub") ||
		!safeStrCmp(part, "elm.text.2") ||
		!safeStrCmp(part, "elm.text.sub.left.bottom")
			)
		return (char *)g_strdup(_(menu_node->txt));

	return NULL;
}

static Evas_Object *__item_calss_get_content(void *data, Evas_Object *obj, const char *part)
{
	return NULL;
}

static void __create_main_menu(void)
{
	int i = 0;
	for(i = 0; i < (sizeof(main_menu) / sizeof(seting_about_menu_node)); i++) {
		elm_genlist_item_append(view_data.main_list,
								main_menu[i].item_class, (void *)&(main_menu[i]),
								NULL, ELM_GENLIST_ITEM_NONE,
								main_menu[i].click_cb, (void *)&(main_menu[i])
							);
		}
}

static void __create_itc(const char *style, Elm_Genlist_Item_Class *itc)
{
	if (!itc)
		return;

	itc->item_style = style;
	itc->func.text_get = __item_class_get_text;
	itc->func.content_get = __item_calss_get_content;
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
