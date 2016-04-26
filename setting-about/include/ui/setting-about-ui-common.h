#ifndef __SETTING_ABOUT_UI_COMMON_H__
#define __SETTING_ABOUT_UI_COMMON_H__

#include <Elementary.h>

#define MAX_TXT_LEN 512

typedef enum {
	UI_ID_SOFTWARE_UPDATE = 0,
	UI_ID_MANAGE_CERTIFICATES,
	UI_ID_LICENCE,
	UI_ID_DEVICE_INFO,
	UI_ID_NAME,
	UI_ID_MY_NUMBER,
	UI_ID_MODEL,
	UI_ID_VERSION,
	UI_ID_IMEI,
	UI_ID_BLUETOOTH,
	UI_ID_WI_FI,
	UI_ID_SERIAL_NUMBER,
	UI_ID_BATTERY,
	UI_ID_CPU_USAGE,
	UI_ID_DEVICE_STATUS,
	UI_ID_SECURITY_STATUS
} ui_element_id;


typedef struct _seting_about_menu_node
{
	const char *title;
	const char txt[MAX_TXT_LEN];
	ui_element_id id;
	struct _seting_about_menu_node *submenu;
	Evas_Smart_Cb click_cb;
	const Elm_Genlist_Item_Class *item_class;
} seting_about_menu_node;

extern Elm_Genlist_Item_Class itc_1;
extern Elm_Genlist_Item_Class itc_2;
/*extern Elm_Genlist_Item_Class itc_2text_3_parent;
extern Elm_Genlist_Item_Class itc_1icon_1text_sub; */
extern Elm_Genlist_Item_Class itc_group;
//extern Elm_Genlist_Item_Class itc_help_style;

#endif /*__SETTING_ABOUT_UI_COMMON_H__*/
