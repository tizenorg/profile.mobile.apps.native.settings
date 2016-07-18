/*
 * setting
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd.
 *
 * Contact: MyoungJune Park <mj2004.park@samsung.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */


#ifndef __SETTING_COMMON_DATA_SLP_SETTING_H__
#define __SETTING_COMMON_DATA_SLP_SETTING_H__

#include <vconf.h>
#include <setting-common-general-func.h>

#define LANGLIST_FILE_PATH \
	_TZ_SYS_RO_APP"/org.tizen.setting/def_config/langlist.xml"
#define LANGLIST_ALL_FILE_PATH \
	_TZ_SYS_RW_APP"/org.tizen.setting/data/langlist_all.xml"


#define SETTING_STR_SLP_LEN 256
#define VCONFKEY_SETAPPL_USE_PACKET_DATA_BOOL \
	VCONFKEY_SETAPPL_PREFIX"/use_packet_data"



#ifndef VCONFKEY_SETAPPL_SIM_LANGUAGE_STR
#define VCONFKEY_SETAPPL_SIM_LANGUAGE_STR	"memory/setting/sim_language"
#endif

#ifndef VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME
#define VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_NAME \
	"db/setting/accessibility/font_name"
#endif

/**
 * @brief status of LTE feature
 *
 * 1 : lte on operator \n
 * 0 : Not lte on operator \n
 */
#define VCONF_KEY_PRIVATE_LTE_ON "db/private/telephony/lte_on"

/** @todo verify if the below keys need to be added in libslp-setting*/

/* added 11.07.11 */
#define VCONFKEY_SETAPPL_STATE_PARENTAL_MODE_BOOL \
	VCONFKEY_SETAPPL_PREFIX"/parental_mode"
#define VCONFKEY_SETAPPL_DATA_EACH_MONTH_INT \
	VCONFKEY_SETAPPL_PREFIX"/data_each_month"
#define VCONFKEY_SETAPPL_DATA_EACH_MONTH_APP_INT \
	VCONFKEY_SETAPPL_PREFIX"/data_each_month_app"

#define VCONFKEY_SETAPPL_TAP_TWIST VCONFKEY_SETAPPL_PREFIX"/motion/tap_twist"
#ifndef VCONFKEY_SETAPPL_USE_MUTE_PAUSE
#define VCONFKEY_SETAPPL_USE_MUTE_PAUSE \
	VCONFKEY_SETAPPL_PREFIX"db/setting/use_mute_pause"
#endif

/*Driving Mode(DM) */
#define VCONFKEY_SETAPPL_DM_DRIVING_MODE \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/drivingmode"
#define VCONFKEY_SETAPPL_DM_INCOMING_CALL \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/incomingcall"
#define VCONFKEY_SETAPPL_DM_MESSAGE \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/message"
#define VCONFKEY_SETAPPL_DM_EMAIL \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/newemails"
#define VCONFKEY_SETAPPL_DM_VOICEMAIL \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/newvoicemails"
#define VCONFKEY_SETAPPL_DM_ALARM \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/alarm"
#define VCONFKEY_SETAPPL_DM_SCHEDULE \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/schedule"
#define VCONFKEY_SETAPPL_DM_UNLOCK_SCREEN \
	VCONFKEY_SETAPPL_PREFIX"/drivingmode/unlockscreen"

/*about phone ug */
#define VCONFKEY_SETAPPL_ASSISTIVE_LIGHT_REMINDER \
	VCONFKEY_SETAPPL_PREFIX"/accessibility/assistive_light_reminder"

/* firewall block type */
#define VCONFKEY_SETAPPL_FIREWALL_NETWORK_BLOCK_TYPE \
	VCONFKEY_SETAPPL_PREFIX"/firewall_network_block"
#define VCONFKEY_SETAPPL_FIREWALL_PROTOCOL_BLOCK_TYPE \
	VCONFKEY_SETAPPL_PREFIX"/firewall_protocol_block"

#ifndef VCONFKEY_SETAPPL_DEFAULT_MEM_WIFI_DIRECT_INT
#define VCONFKEY_SETAPPL_DEFAULT_MEM_WIFI_DIRECT_INT \
	VCONFKEY_SETAPPL_PREFIX"/default_memory/wifi_direct"
#endif

#ifndef VCONFKEY_SETAPPL_DEFAULT_MEM_INSTALL_APPLICATIONS_INT
#define VCONFKEY_SETAPPL_DEFAULT_MEM_INSTALL_APPLICATIONS_INT \
	VCONFKEY_SETAPPL_PREFIX"/default_memory/install_applications"
#endif


/*already defined by hyejin kim,need to removed after next binary */
#ifndef VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS
#define VCONFKEY_SETAPPL_LCD_AUTOMATIC_BRIGHTNESS \
	VCONFKEY_SETAPPL_PREFIX"/automatic_brightness_level"
#endif

/*for data usage */
#define VCONFKEY_SETAPPL_SET_DATA_USAGE_LIMIT_ROAMING_BOOL \
	VCONFKEY_SETAPPL_PREFIX"/set_data_usage_limit_roaming"

#ifndef VCONFKEY_SETAPPL_BOOL_AUTO_ADJUST_SCREEN_TONE
#define VCONFKEY_SETAPPL_BOOL_AUTO_ADJUST_SCREEN_TONE \
	VCONFKEY_SETAPPL_PREFIX"/auto_adjust_screen_tone"
#endif



typedef enum {
	SETTING_SEC_PW_PASSWORD,
	SETTING_SEC_PW_SIMPLE_PASSWD,
	SETTING_SEC_PW_ENTER_LOCK_TYPE,
	SETTING_SEC_PW_ENCRYPTION,
	SETTING_SEC_PW_SIM_LOCK_ON,
	SETTING_SEC_PW_SIM_LOCK_OFF,
	SETTING_SEC_PW_SIM_LOCK_DISABLED,
	SETTING_SEC_PW_PIN_LOCK_ON,
	SETTING_SEC_PW_PIN_LOCK_OFF,
	SETTING_SEC_PW_CHANGE_PIN1,
#if SUPPORT_FDN
	SETTING_SEC_PW_FDN_MODE_ON,
	SETTING_SEC_PW_FDN_MODE_OFF,
#endif
	SETTING_SEC_PW_CHANGE_PIN2,
	SETTING_SEC_PW_PIN1_BLOCKED,
	SETTING_SEC_PW_PIN2_BLOCKED,
	SETTING_SEC_PW_PIN1_UNBLOCKED,
	SETTING_SEC_PW_PIN2_UNBLOCKED,
	SETTING_SEC_PW_PUK1_BLOCKED,
	SETTING_SEC_PW_PUK2_BLOCKED,
	SETTING_SEC_PW_MAX
} setting_sec_pw_type;

typedef enum {
	/* display and light */
	INT_SLP_SETTING_LCD_BRIGHTNESS = 0,	/* the value set by manul */
	INT_SLP_SETTING_LCD_TIMEOUT_NORMAL,
	INT_SLP_SETTING_LCD_TIMEOUT_NORMAL_BACKUP,

	/* set by manul or not. 0:manul, 1:not */
	INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS,

	INT_SLP_SETTING_SELECT_NETWORK,
	INT_SLP_SETTING_NETWORK_MODE,
	INT_SLP_SETTING_REGIONFORMAT_TIME1224,
	INT_SLP_SETTING_DATE_FORMAT,
	INT_SLP_SETTING_WEEK_FORMAT,

	/* CALL_ALERT_VIB */
	INT_SLP_SETTING_CALL_ALERT_VIB,

	/*****/
	INT_SLP_SETTING_MSG_ALERT_REPEAT,

	/*****/
	INT_SLP_SETTING_LANG,
	INT_SLP_SETTING_BT_STATUS,
	INT_SLP_SETTING_WIFI_STATUS,
	INT_SLP_SETTING_MOBILE_AP_STATUS,
	INT_SLP_SETTING_ROAMING_NETWORK,
	INT_SLP_SETTING_USB_MODE,
#ifdef UNUSED_VCONFKEY
	INT_SLP_SETTING_TASKSWITCHER_VIEWTYPE,
#endif
	/* memory */
	INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH,
	INT_SLP_SETTING_DEFAULT_MEM_WIFIDIRECT,
	INT_SLP_SETTING_DEFAULT_MEM_INSTALLAPPLICATIONS,

	/* other module */
	INT_SLP_SETTING_SIM_SLOT,
	INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT,
	INT_SLP_SETTING_SIM_LOCK_ATTEMPTS_LEFT,

	/* accessibility setting */
	INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE,
	INT_SLP_SETTING_ACCESSIBILITY_SOUND_BALANCE,

	/* developer option > Limit background process */
	INT_SLP_SETTING_DEVOPTION_BGPROCESS,

	/* data usage */
	INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT,
	INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT,
	INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_APP_INT,

	/*accessories */
	INT_SLP_SETTING_TOUCHKEY_LIGHT_DURATION_INT,
	INT_SLP_SETTING_SCREEN_CAPTURE_DESTINATION_INT,

	SETTING_INT_SLP_LIST_MAX
} setting_int_slp_list;

typedef enum {

	BOOL_SLP_SETTING_POWER_ON_LOCK = 0,
	BOOL_SLP_SETTING_SIMPLE_PASSWORD,

	BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE,

	/*BOOL_SLP_SETTING_TICKER_NOTI_MESSAGES,
	BOOL_SLP_SETTING_TICKER_NOTI_EMAIL,
	BOOL_SLP_SETTING_TICKER_NOTI_IM,
	BOOL_SLP_SETTING_TICKER_NOTI_FACEBOOK,
	BOOL_SLP_SETTING_TICKER_NOTI_TWITTER,

	BOOL_SLP_SETTING_NOTI_DISPLAY_CONTENT_MESSASGES,
	BOOL_SLP_SETTING_NOTI_DISPLAY_CONTENT_EMAIL,
	BOOL_SLP_SETTING_NOTI_DISPLAY_CONTENT_IM,
	BOOL_SLP_SETTING_NOTI_DISPLAY_CONTENT_FACEBOOK,
	BOOL_SLP_SETTING_NOTI_DISPLAY_CONTENT_TWITTER,*/

	BOOL_SLP_SETTING_FLIGHT_MODE,
	BOOL_SLP_SETTING_NET_RESTRICTION_MODE,
	BOOL_SLP_SETTING_DATA_ROAMING,
	BOOL_SLP_SETTING_NFC_STATUS,
	BOOL_SLP_SETTING_USE_PACKET_DATA,

	BOOL_SLP_SETTING_ACCESSIBILITY_HIGH_CONTRAST,
	BOOL_SLP_SETTING_ACCESSIBILITY_SCREEN_ZOOM,
	BOOL_SLP_SETTING_ACCESSIBILITY_ASSISTIVE_LIGHT,
	/*BOOL_SLP_SETTING_ACCESSIBILITY_COLORBLIND, */
	BOOL_SLP_SETTING_ACCESSIBILITY_MONO_AUDIO,
	BOOL_SLP_SETTING_ACCESSIBILITY_TURN_OFF_ALL_SOUNDS,
	BOOL_SLP_SETTING_ACCESSIBILITY_LED_NOTIFY,
	BOOL_SLP_SETTING_ACCESSIBILITY_ACCEPT_CALL,
	BOOL_SLP_SETTING_ACCESSIBILITY_POWERKEY_END_CALLS,
	BOOL_SLP_SETTING_ACCESSIBILITY_EASY_TOUCH_MODE,
	BOOL_SLP_SETTING_ACCESSIBILITY_RAPID_KEY_INPUT,

	/* data usage */
	BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL,
	BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_ROAMING_BOOL,

	/* motion */
	BOOL_SLP_SETTING_MOTION_ACTIVATION,
	BOOL_SLP_SETTING_MOTION_USE_PICK_UP_CALL,
	BOOL_SLP_SETTING_MOTION_USE_PICK_UP,
	BOOL_SLP_SETTING_MOTION_USE_DOUBLE_TAP,
	BOOL_SLP_SETTING_MOTION_USE_TILT,
	BOOL_SLP_SETTING_MOTION_USE_PANNING,
	BOOL_SLP_SETTING_MOTION_USE_PANNING_BROWSER,
	BOOL_SLP_SETTING_MOTION_USE_SHAKE,
	BOOL_SLP_SETTING_MOTION_USE_TURN_OVER,
	BOOL_SLP_SETTING_MOTION_TAP_TWIST,
	BOOL_SLP_SETTING_MOTION_USE_MUTE_PAUSE,
	/* end */
	SETTING_BOOL_SLP_LIST_MAX
} setting_bool_slp_list;

typedef enum {
	STR_SLP_LIST_PHONE_PASSWORD = 0,
	STR_SLP_SETTING_PHONE_LOCK_TIMESTAMP,
	STR_SLP_SETTING_SIM_LOCK_TIMESTAMP,
	STR_SLP_SETTING_SELECT_NUM,
	STR_SLP_LIST_MAX
} setting_str_slp_list;

extern int setting_set_int_slp_key(
		setting_int_slp_list key, int value, int *err);
extern int setting_get_int_slp_key(
		setting_int_slp_list key, int *value, int *err);
extern int setting_set_bool_slp_key(
		setting_bool_slp_list key, int value, int *err);
extern int setting_get_bool_slp_key(
		setting_bool_slp_list key, int *value, int *err);
extern int setting_set_string_slp_key(
		setting_str_slp_list key, char *value, int *err);
extern int setting_get_string_slp_key(
		setting_str_slp_list key, char *value, int *err);

extern int setting_store_init_password(char *in);
extern int setting_read_password(char *md_result);
extern int setting_compare_password(char *md_result, const char *in_later);

extern setting_lang_entry *setting_get_language_table();
extern Eina_List *setting_get_language_list();
extern Eina_List *setting_get_language_list2();
extern void	 setting_get_language_list_destroy();

/*------------------------------------------------------------------------- */
/* export&import function */
typedef int (*status_handler_fp)(int total, int current, void *data);

extern char *setting_export_json(status_handler_fp fp, void *data);
extern void setting_import_json(status_handler_fp fp, void *data);



#endif				/* __SETTING_COMMON_DATA_SLP_SETTING_H__ */
