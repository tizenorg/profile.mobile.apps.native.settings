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
		setting_about_ui_display_attribute(UI_ID_NAME, tmp);

	if (setting_about_device_get_phone_number_and_imei(tmp, sizeof(tmp), tmp2, sizeof(tmp2))) {
		setting_about_ui_display_attribute(UI_ID_MY_NUMBER, tmp);
		setting_about_ui_display_attribute(UI_ID_IMEI, tmp);
	}
	if (setting_about_device_get_phone_model_name(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_MODEL, tmp);

	if (setting_about_device_get_sw_version(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_VERSION, tmp);

	if (setting_about_device_get_bt_mac_address(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_BLUETOOTH, tmp);

	if (setting_about_device_get_wifi_mac_addr(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_WI_FI, tmp);

	if (setting_about_device_get_sn(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_SERIAL_NUMBER, tmp);

	if (setting_about_device_get_battery_level(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_BATTERY, tmp);

	if (setting_about_main_stat_get_cpuinfo(tmp, sizeof(tmp)))
		setting_about_ui_display_attribute(UI_ID_CPU_USAGE, tmp);

	return true;
}

void setting_about_controller_deinit(void *user_data)
{
	setting_about_ui_deinit();
}
