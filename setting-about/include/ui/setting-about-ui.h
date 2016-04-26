#ifndef __SETTING_ABOUT_UI_H__
#define __SETTING_ABOUT_UI_H__

#include <stdbool.h>

typedef enum {
	UI_ID_SOFTWARE_UPDATE,
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

extern void setting_about_ui_display_name(const char *str);
extern void setting_about_ui_display_phone_no(const char *str);
extern void setting_about_ui_display_model(const char *str);
extern void setting_about_ui_display_version(const char *str);
extern void setting_about_ui_display_imei(const char *str);
extern void setting_about_ui_display_bt_addr(const char *str);
extern void setting_about_ui_display_wifi_mac(const char *str);
extern void setting_about_ui_display_sn(const char *str);
extern void setting_about_ui_display_batt_pow(const char *str);
extern void setting_about_ui_display_cpu_usage(const char *str);
extern void setting_about_ui_display_dev_status(const char *str);
extern void setting_about_ui_display_sec_status(const char *str);


#endif /*__SETTING_ABOUT_UI_H__*/
