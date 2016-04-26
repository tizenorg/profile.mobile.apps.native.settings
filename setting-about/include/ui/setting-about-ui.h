#ifndef __SETTING_ABOUT_UI_H__
#define __SETTING_ABOUT_UI_H__

#include <stdbool.h>

typedef enum {
	UI_ID_SOFTWARE_UPDATE = 0,
	UI_ID_MANAGE_CERTIFICATES,
	UI_ID_LICENCE,
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
	UI_ID_DEVICE_STATUS
} ui_element_id;

extern bool setting_about_ui_init(void);
extern void setting_about_ui_deinit(void);

extern void setting_about_ui_display_attribute(ui_element_id uid, const char *str);

#endif /*__SETTING_ABOUT_UI_H__*/
