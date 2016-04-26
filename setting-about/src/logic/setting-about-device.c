#include "logic/setting-about-device.h"

#include <stdlib.h>
#include <device/battery.h>
#include <system_info.h>
#include <runtime_info.h>
#include <app_i18n.h>
#include <stdio.h>
#include <wifi.h>
#include <bluetooth.h>
#include <telephony/telephony.h>
#include <vconf.h>
#include <dbus/dbus.h>
#include "setting-common-general-func.h"

#define DEVICED_BUS_NAME			"org.tizen.system.deviced"
#define DEVICED_OBJECT_PATH			"/Org/Tizen/System/DeviceD"
#define DEVICED_INTERFACE_NAME		DEVICED_BUS_NAME
#define DEVICED_PATH_BOARD			DEVICED_OBJECT_PATH"/Board"
#define DEVICED_INTERFACE_BOARD		DEVICED_INTERFACE_NAME".Board"


static DBusMessage *__dbus_method_sync_with_reply(const char *dest, const char *path,
												const char *interface, const char *method,
												const char *sig, char *param[]);

bool setting_about_device_get_device_name(char *str, int size)
{
	char *name = NULL;

	if (!str || size <= 0)
		return false;

	name = vconf_get_str(VCONFKEY_SETAPPL_DEVICE_NAME_STR);
	snprintf(str, size, "%s", name);

	return true;
}

bool setting_about_device_get_phone_number_and_imei(char *str, int size, char *str2, int size2)
{
	char *num = NULL;
	char *imei = NULL;
	int i = 0;
	int ret = TELEPHONY_ERROR_NONE;
	telephony_handle_list_s tapi_handle_list;

	if (!str || size <= 0)
		return false;

	ret = telephony_init(&tapi_handle_list);

	if (ret != TELEPHONY_ERROR_NONE) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return true;
	}

	SETTING_TRACE_DEBUG("tapi_handle_list.count:%d", tapi_handle_list.count);

	for (i = 0; i < tapi_handle_list.count; i++) {
		ret = telephony_sim_get_subscriber_number(tapi_handle_list.handle[i], &num);

		if (ret != TELEPHONY_ERROR_NONE || isEmptyStr(num)) {
			strncat(str, _("IDS_ST_HEADER_UNAVAILABLE"), size);
			size -= (strlen(_("IDS_ST_HEADER_UNAVAILABLE")) + 1);
			strcat(str, " ");
		} else {
			strncat(str, num, size);
			size -= (strlen(num) + 1);
			strcat(str, " ");
		}

		ret = telephony_modem_get_imei(tapi_handle_list.handle[i], &imei);
		if (ret != TELEPHONY_ERROR_NONE || isEmptyStr(imei)) {
			strncat(str2, _("IDS_ST_HEADER_UNAVAILABLE"), size2);
			size2 -= (strlen(_("IDS_ST_HEADER_UNAVAILABLE")) + 1);
			strcat(str2, " ");
		} else {
			strncat(str2, imei, size2);
			size2 -= (strlen(imei) + 1);
			strcat(str2, " ");
		}
		free(num);
		free(imei);
		num = NULL;
		imei = NULL;
	}

	telephony_deinit(&tapi_handle_list);

	SETTING_TRACE_DEBUG("SIM and IMEI strings: %s %s", str, str2);

	return true;
}

/**
 * @brief get Model Name.
 *		- No ini: Unavailable
 *		- I9500 @ target: GT-I9500
 *		- I9500 @ emul: SDK
 *		- SLP   @ target: GT-SLP
 *		- SLP   @ emul: SDK
 *
 * @param str the buffer for store the model name
 * @param size buffer size
 */
bool setting_about_device_get_phone_model_name(char *str, int size)
{
	int ret = SYSTEM_INFO_ERROR_NONE;
	char *value = NULL;

	if (!str || size <= 0)
		return false;

	ret = system_info_get_platform_string("http://tizen.org/system/model_name", &value);

	if (ret == SYSTEM_INFO_ERROR_NONE && value) {
		snprintf(str, size, "%s", value);
	} else {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	}

	free(value);
	return true;
}

/**
 * @brief get CPU usage.
 *
 * @param percent Overall system cpu usage as string
 * @param size output buffer size in bytes
 *
 * @return false means failed to get CPU info, otherwise ok.
 */
bool setting_about_main_stat_get_cpuinfo(char *percent, int size)
{
	runtime_cpu_usage_s usage = {0.0,};

	if (!percent || size <= 0)
		return false;

	if (RUNTIME_INFO_ERROR_NONE != runtime_info_get_cpu_usage(&usage)) {
		snprintf(percent, size, "%s%%", _("IDS_ST_HEADER_UNAVAILABLE"));
		return true;
	}

	snprintf(percent, size, "%3.0f%%", usage.system);

	return true;
}

/**
 * @brief get software version.
 *
 * @param str the buffer
 * @param size buffer size
 */
bool setting_about_device_get_wifi_mac_addr(char *str, int size)
{
	char *mac_addr = NULL;
	int status = WIFI_ERROR_NONE;

	if (!str || size <= 0)
		return false;

	status = wifi_get_mac_address(&mac_addr);
	if (WIFI_ERROR_NONE == status) {
		safeCopyStr(str, mac_addr, size);
	} else {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	}

	free(mac_addr);

	return true;
}

/**
 * @brief get bluetooth address.
 *
 * @param str the buffer for store bluetooth address
 * @param size buffer size
 */
bool setting_about_device_get_bt_mac_address(char *str, int size)
{
	int ret = 0;
	bt_adapter_state_e state = BT_ADAPTER_DISABLED;
	char *local_address = NULL;

	if (!str || size <= 0)
		return false;

	ret = bt_initialize();

	if (BT_ERROR_NONE != ret) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return true;
	}
	ret = bt_adapter_get_state(&state);
	if (BT_ERROR_NONE != ret || BT_ADAPTER_DISABLED == state) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	} else {

		ret = bt_adapter_get_address(&local_address);
		if (BT_ERROR_NONE != ret) {
			snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		} else {
			snprintf(str, size, "%s", local_address);
		}
	}

	free(local_address);
	bt_deinitialize();

	SETTING_TRACE_DEBUG("bt address : %s", local_address);

	return true;
}

/**
 * @brief get serial number.
 *
 * @param str the buffer for store SN
 * @param size buffer size
 */
bool setting_about_device_get_sn(char *str, int size)
{
	DBusError err = {0,};
	DBusMessage *msg = NULL;
	int ret = 0;
	int len = 0;
	char *serial_num = NULL;

	if (!str || size <= 0)
		return false;

	msg = __dbus_method_sync_with_reply(DEVICED_BUS_NAME, DEVICED_PATH_BOARD,
									  DEVICED_INTERFACE_BOARD, "GetSerial", NULL, NULL);
	if (!msg) {
		SETTING_TRACE_ERROR("fail send message");
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return true;
	}

	dbus_error_init(&err);

	ret = dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &serial_num, DBUS_TYPE_INT32, &len, DBUS_TYPE_INVALID);
	if (!ret) {
		SETTING_TRACE_ERROR("no message : [%s:%s]", err.name, err.message);
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
		return true;
	}

	dbus_message_unref(msg);
	dbus_error_free(&err);

	if (serial_num && strlen(serial_num)) {
		snprintf(str, size, "%s", serial_num);
	} else {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	}

	return true;
}

/**
 * @brief get software version.
 *
 * @param str the buffer for store SW version
 * @param size buffer size
 */
bool setting_about_device_get_sw_version(char *str, int size)
{
	int ret = SYSTEM_INFO_ERROR_NONE;
	char *version = NULL;

	if (!str || size <= 0)
		return false;

	ret = system_info_get_platform_string("http://tizen.org/feature/platform.version", &version);
	if (ret != SYSTEM_INFO_ERROR_NONE) {
		SETTING_TRACE_ERROR("fail to call system_info_get_platform_string");
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	} else {
		snprintf(str, size, "TIZEN %s", version);
	}

	free(version);
	return true;
}

/**
 * @brief get current battery percentage.
 *
 * @param str the buffer for store battery percentage
 * @param size buffer size
 */
bool setting_about_device_get_battery_level(char *str, int size)
{
	int level = 0;
	int ret = DEVICE_ERROR_NONE;

	if (!str || size <= 0)
		return false;

	device_battery_get_percent(&level);//battery_get_percent();
	if (DEVICE_ERROR_NONE != ret) {
		snprintf(str, size, "%s", _("IDS_ST_HEADER_UNAVAILABLE"));
	} else {
		snprintf(str, size, "%d%%", level);
	}

	return true;
}

static DBusMessage *__dbus_method_sync_with_reply(const char *dest, const char *path,
												const char *interface, const char *method,
												const char *sig, char *param[])
{
	DBusConnection *conn = NULL;
	DBusMessage *msg = NULL;
	DBusMessageIter iter;
	DBusMessage *reply = NULL;
	DBusError err;

	conn = dbus_bus_get(DBUS_BUS_SYSTEM, NULL);
	if (!conn)
		return NULL;

	msg = dbus_message_new_method_call(dest, path, interface, method);
	if (!msg)
		return NULL;

	dbus_message_iter_init_append(msg, &iter);

	dbus_error_init(&err);

	reply = dbus_connection_send_with_reply_and_block(conn, msg, -1, &err);

	if (dbus_error_is_set(&err)) {
		SETTING_TRACE_ERROR("dbus_connection_send error(%s:%s)", err.name, err.message);
		dbus_error_free(&err);
		reply = NULL;
	}

	dbus_message_unref(msg);
	return reply;
}
