#ifndef __SETTING_ABOUT_DEVICE_H__
#define __SETTING_ABOUT_DEVICE_H__

#include <stdbool.h>

extern bool setting_about_device_get_device_name(char *str, int size);
extern bool setting_about_device_get_phone_number_and_imei(char *str, int size, char *str2, int size2);
extern bool setting_about_device_get_phone_model_name(char *str, int size);
extern bool setting_about_main_stat_get_cpuinfo(char *percent, int size);
extern bool setting_about_device_get_wifi_mac_addr(char *str, int size);
extern bool setting_about_device_get_bt_mac_address(char *str, int size);
extern bool setting_about_device_get_sn(char *str, int size);
extern bool setting_about_device_get_sw_version(char *str, int size);
extern bool setting_about_device_get_battery_level(char *str, int size);

#endif /*__SETTING_ABOUT_DEVICE_H__*/
