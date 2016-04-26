#include "setting-about-controller.h"
#include "ui/setting-about-ui.h"
#include "logic/setting-about-device.h"

#define BUFF_MAX 256

bool setting_about_controller_init(void)
{
	char tmp[BUFF_MAX] = {0,};
	char tmp2[BUFF_MAX] = {0,};

	if(!setting_about_ui_init())
		return false;

	if (setting_about_device_get_device_name(tmp, sizeof(tmp)))
		setting_about_ui_display_name(tmp);

	if (setting_about_device_get_phone_number_and_imei(tmp, sizeof(tmp), tmp2, sizeof(tmp2))) {
		setting_about_ui_display_phone_no(tmp);
		setting_about_ui_display_imei(tmp);
	}
	if (setting_about_device_get_phone_model_name(tmp, sizeof(tmp)))
		setting_about_ui_display_model(tmp);

	if (setting_about_device_get_sw_version(tmp, sizeof(tmp)))
		setting_about_ui_display_version(tmp);

	if (setting_about_device_get_bt_mac_address(tmp, sizeof(tmp)))
		setting_about_ui_display_bt_addr(tmp);

	if (setting_about_device_get_wifi_mac_addr(tmp, sizeof(tmp)))
		setting_about_ui_display_wifi_mac(tmp);

	if (setting_about_device_get_sn(tmp, sizeof(tmp)))
		setting_about_ui_display_sn(tmp);

	if (setting_about_device_get_battery_level(tmp, sizeof(tmp)))
		setting_about_ui_display_batt_pow(tmp);

	if (setting_about_main_stat_get_cpuinfo(tmp, sizeof(tmp)))
		setting_about_ui_display_cpu_usage(tmp);

	return true;
}

