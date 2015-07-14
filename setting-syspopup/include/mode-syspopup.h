/*
 * mode-syspopup
 *
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _MODE_SYSPOPUP_APP_H__
#define _MODE_SYSPOPUP_APP_H__

#include <Evas.h>
#include <Elementary.h>
#include <ui-gadget.h>
#include <ui-gadget-module.h>
#include <dlog.h>
#include <app.h>
#include <app_preference.h>
#include <notification.h>
#include <vconf.h>
#include <vconf-keys.h>
#include <app.h>
/*#include <libhelpui.h> */
#include <util.h>
#include <tts.h>

#define REBOOT_METHOD_FROM_SHELL 1
#define LCD_FREQ_CTRL 0

typedef void (*_cb_func)(void *data, Evas_Object *obj, void *event_info);
/*domain for notification */
#define SETTING_PACKAGE		"setting"
#define SETTING_LOCALEDIR		"/usr/apps/org.tizen.setting/res/locale"
#define SYSTEM_PACKAGE			"sys_string"

#define MODE_SYSPOPUP_PACKAGE                  "mode-syspopup"
#define MODE_SYSPOPUP_LOCALEDIR                "/usr/apps/org.tizen.mode-syspopup/res/locale"

#ifdef _(s)
#undef _(s)
#define _(s)			modesyspopup_gettext(s)
#endif

/*dbus for lcd freq control */
#define BUS_NAME       				"org.tizen.system.deviced"
#define INTERFACE_NAME 			BUS_NAME
#define OBJECT_PATH    				"/Org/Tizen/System/DeviceD"
#define DEVICED_PATH_DISPLAY    	OBJECT_PATH"/Display"
#define DEVICED_INTERFACE_DISPLAY INTERFACE_NAME".display"
#define METHOD_NAME				"SetRefreshRate"
#define DBUS_REPLY_TIMEOUT 		(120 * 1000)
#define LCD_FRQ_CONTROL_OFF		60
#define LCD_FRQ_CONTROL_ON		40


#define PROGRESSBAR_STYLE "process_large"
#define POPUP_EDJ		"/usr/apps/org.tizen.mode-syspopup/res/edje/popup-custom.edj"
#define MODE_POPUP_EDJ	"/usr/apps/org.tizen.mode-syspopup/res/edje/mode-syspopup.edj"
#define UPMS_EDJ		"/usr/apps/org.tizen.mode-syspopup/res/edje/ultra-popup.edj"

#define DEFAULT_BG_PATH "/opt/share/settings/Wallpapers/Home_default.jpg"
#define EMERGENCY_UIKEY "file/private/org.tizen.emergency-home/mode"

/*power saving text */
#define KeyStr_Off "IDS_ST_BODY_ALERTTYPE_OFF"
#define KeyStr_PowerSaving_Mode "IDS_ST_OPT_CUSTOM_MODE_ABB"
#define KeyStr_Enable_custom_Mode "IDS_ST_OPT_CUSTOM_MODE_ABB"
#define KeyStr_Enable_emergency_Mode "IDS_ST_BODY_EMERGENCY_MODE_ABB"
#define KeyStr_Survivor_Mode "IDS_ST_BODY_EMERGENCY_MODE_ABB"
#define KeyStr_Emergency_off_Mode_POP_DESC "IDS_ST_POP_EMERGENCY_MODE_WILL_BE_DISABLED"
#define KeyStr_Ultra_off_Mode_POP_DESC "IDS_ST_POP_ULTRA_POWER_SAVING_MODE_WILL_BE_DISABLED"	/*"IDS_ST_POP_EMERGENCY_MODE_WILL_BE_DISABLED" */
#define KeyStr_powersaving_Mode_POP_DESC "IDS_ST_BODY_SAVES_POWER_BY_LIMITING_THE_MAXIMUM_PERFORMANCE_OF_THE_DEVICE"
#define KeyStr_Survivor_Mode_POP_DESC "IDS_ST_POP_EMERGENCY_MODE_SAVES_BATTERY_POWER_BY_C_MSG"
#define KeyStr_Loading				"IDS_ST_BODY_DISABLING_ING"

/*blockingmode text */
#define KeyStr_Blocking_Mode_POP_DESC "IDS_ST_BODY_TO_ENABLE_BLOCKING_MODE_ENABLE_AT_LEAST_ONE_OPTION"

#define SETTING_ICON_PATH_CFG	"/usr/apps/org.tizen.setting/res/icons/"

#define MODE_SYSTEM_POPUP_TYPE "_MODE_SYSTEM_POPUP_TYPE_" /*bundle type */
#define MODE_SYSTEM_POPUP_EMERGENCY "POPUP_EMERGENCY_PSMODE"
#define MODE_SYSTEM_POPUP_ULTRA "POPUP_ULTRA_PSMODE"
#define MODE_SYSTEM_POPUP_POWERFUL "POPUP_POWERFUL_PSMODE"
#define MODE_SYSTEM_POPUP_NORMAL "POPUP_NORMAL_PSMODE"
#define MODE_SYSTEM_POPUP_NORMAL_SETTING "POPUP_NORMAL_SETTING"
#define MODE_SYSTEM_POPUP_BACKUP_SETTING "POPUP_BACKUP_SETTING"
#define MODE_SYSTEM_POPUP_EMERGENCY_SETTING "POPUP_EMERGENCY_PSMODE_SETTING"
#define MODE_SYSTEM_POPUP_EMERGENCY_SAFETY "POPUP_EMERGENCY_PSMODE_SAFETY"
#define MODE_SYSTEM_POPUP_POWERFUL_SETTING "POPUP_POWERFUL_PSMODE_SETTING"
#define MODE_SYSTEM_POPUP_REMOVE_FLAG "POPUP_PSMODE_REMOVE_FLAG"

/*Help text */
#define MODE_SYSTEM_POPUP_MSG "IDS_ST_POP_TAP_OK"

/*blockingmode */
#define MODE_SYSTEM_POPUP_BLOCKINGMODE_ON "MODE_SYSTEM_BLOCKINGMODE_ON"
#define MODE_SYSTEM_POPUP_BLOCKINGMODE_OFF "MODE_SYSTEM_BLOCKINGMODE_OFF"
#define PKG_BLOCKING_MODE "setting-blockingmode-efl"
#define IMG_BlockingMode		SETTING_ICON_PATH_CFG"settings_block.png"
#define INDI_IMG_BlockingMode	SETTING_ICON_PATH_CFG"B03_event_blocking_mode.png"

/*drivingmode */
#define MODE_SYSTEM_POPUP_DRIVINGMODE_ON "MODE_SYSTEM_DRIVINGMODE_ON"
#define MODE_SYSTEM_POPUP_DRIVINGMODE_OFF "MODE_SYSTEM_DRIVINGMODE_OFF"
#define PKG_DRIVING_MODE "setting-drivingmode-efl"

/*firewall */
#define PKG_FIREWALL "setting-firewall-efl"
#define IMG_Firewall				SETTING_ICON_PATH_CFG"noti_firewall_activated.png"
#define INDI_IMG_Firewall		SETTING_ICON_PATH_CFG"noti_firewall_activated_indicator.png"

#define VIB_FEEDBACK_RATE	20
#define DEFAULT_NOTI_PREVIEW_VIB			"/opt/share/settings/Vibrations/message.ivt"
#define DEFAULT_HAPTIC_PREVIEW_VIB		"/opt/share/settings/Vibrations/touch.ivt"
#define VCONF_HOMESCREEN_PATH "db/menu_widget/bgset"
#define VCONF_LOCKSCREEN_PATH "db/idle_lock/bgset"
#define SETTING_IMAGE_PATH_CFG	"/usr/apps/org.tizen.setting/res/images/"
#define HOMESCREEN_FILE SETTING_IMAGE_PATH_CFG"Long_life_bg_home_720.png"
#define LOCKSCREEN_FILE SETTING_IMAGE_PATH_CFG"Long_life_bg_lock_720.png"
#define PKG_EMERGENCY "org.tizen.emergency-home"
#define PKG_EASYHOME "org.tizen.easy-home"
#define PKG_CLUSTERHOME "org.tizen.homescreen"
#define VCONF_PKG_PATH "db/setting/menuscreen/package_name"

#define PREFKEY_SAVINGPOWER_BACKUP_FIRST_PWR "setting/savingpower_back/first"	/*int */
#define PREFKEY_SAVINGPOWER_BACKUP_PSMODE "setting/savingpower_back/psmode"	/*int */

/*normal */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_WIFI "setting/savingpower_normal/wifi"				/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BT "setting/savingpower_normal/bt"					/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GPS "setting/savingpower_normal/gps"				/*bool */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_3G "setting/savingpower_normal/3g"					/*bool */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_MOTION "setting/savingpower_normal/motion"			/*bool */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_BRIGHT "setting/savingpower_normal/auto_bright"			/*bool */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BRIGHT "setting/savingpower_normal/bright"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TIMEOUT "setting/savingpower_normal/timeout"		/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_NOTI "setting/savingpower_normal/noti"				/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HAPTIC "setting/savingpower_normal/haptic"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HAPTIC_FEEDBACK "setting/savingpower_normal/haptic"			/*bool */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_AUTO_ROTATE "setting/savingpower_normal/auto_rotate"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_SMART_SCREEN "setting/savingpower_normal/smart_screen"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_SMART_ROTATE "setting/savingpower_normal/smart_rotate"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_CPU_PERF "setting/savingpower_normal/cpu_perf"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_PKG_HOME "setting/savingpower_normal/pkg_home"			/*str */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LAUNCH_EFFECT "setting/savingpower_normal/launch_effect"	/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FAKE_EFFECT "setting/savingpower_normal/fake_effect"	/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HOMESCREEN_PATH "setting/savingpower_normal/home_path"
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LOCKSCREEN_PATH "setting/savingpower_normal/lock_path"
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LCD_FREQ_CONTROL "setting/savingpower_normal/lcd_freq_control"
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_GRAYTONE "setting/savingpower_normal/win_gray"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_FPS "setting/savingpower_normal/win_fps"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_TOUCH_KEY_LIGHT "setting/savingpower_normal/touch_key_light"			/*int */

#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_LED_INDICATOR_NOTI "setting/savingpower_normal/led_indicator_noti"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BATTERY_PERCENTAGE "setting/savingpower_normal/battery_percent"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_VOICE_CONTROL "setting/savingpower_normal/voice_ctrl"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_MULTIWINDOW "setting/savingpower_normal/multi_window"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_HANDS_FREE_MODE "setting/savingpower_normal/hands_free"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_BLOCKING_MODE "setting/savingpower_normal/blocking_mode"			/*int */
#define PREFKEY_SAVINGPOWER_NORMAL_BACKUP_NET_RESRICT_MODE "setting/savingpower_normal/net_restrict"			/*int */


/*power */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_HAPTIC "setting/savingpower_power/haptic"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_HAPTIC_FEEDBACK "setting/savingpower_power/haptic"			/*bool */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_LAUNCH_EFFECT "setting/savingpower_power/launch_effect"	/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_GPS "setting/savingpower_power/gps"				/*bool */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_BRIGHT "setting/savingpower_power/bright"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_BRIGHT "setting/savingpower_power/auto_bright"			/*bool */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_AUTO_ROTATE "setting/savingpower_power/auto_rotate"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_TIMEOUT "setting/savingpower_power/timeout"		/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_SMART_SCREEN "setting/savingpower_power/smart_screen"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_SMART_ROTATE "setting/savingpower_power/smart_rotate"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT "setting/savingpower_power/touch_key_light"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_GRAYTONE "setting/savingpower_power/win_gray"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_CPU_PERF "setting/savingpower_power/cpu_perf"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_FPS "setting/savingpower_power/win_fps"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_PKG_HOME "setting/savingpower_power/pkg_home"			/*str */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_HOMESCREEN_PATH "setting/savingpower_power/home_path"
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_LOCKSCREEN_PATH "setting/savingpower_power/lock_path"
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_TOUCH_KEY_LIGHT "setting/savingpower_power/touch_key_light"			/*int */

#define PREFKEY_SAVINGPOWER_POWER_BACKUP_LED_INDICATOR_NOTI "setting/savingpower_power/led_indicator_noti"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_BATTERY_PERCENTAGE "setting/savingpower_power/battery_percent"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_VOICE_CONTROL "setting/savingpower_power/voice_ctrl"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_MULTIWINDOW "setting/savingpower_power/multi_window"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_HANDS_FREE_MODE "setting/savingpower_power/hands_free"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_BLOCKING_MODE "setting/savingpower_power/blocking_mode"			/*int */
#define PREFKEY_SAVINGPOWER_POWER_BACKUP_NET_RESRICT_MODE "setting/savingpower_power/net_restrict"			/*int */




/* network restriction mode */
#define MODE_SYSTEM_POPUP_NETRESTRICTIONMODE_CHANGE "MODE_SYSTEM_NETRESTRICTIONMODE_CHANGE"
/*flight mode */
#define MODE_SYSTEM_POPUP_FLIGHTMODE_ON "MODE_SYSTEM_FLIGHTMODE_ON"
#define MODE_SYSTEM_POPUP_FLIGHTMODE_OFF "MODE_SYSTEM_FLIGHTMODE_OFF"
#define KeyStr_Fligtmode_Popup_On_Text "IDS_ST_BODY_FLIGHT_MODE_MSG"
#define KeyStr_Fligtmode_Popup_Off_Text "IDS_COM_POP_FLIGHT_MODE_WILL_BE_DISABLED"
#define KeyStr_Fligtmode_Popup_Title "IDS_ST_BODY_FLIGHT_MODE"
/*mobile data */
#define MODE_SYSTEM_POPUP_MOBILEDATA_ON "MODE_SYSTEM_MOBILEDATA_ON"
#define MODE_SYSTEM_POPUP_MOBILEDATA_OFF "MODE_SYSTEM_MOBILEDATA_OFF"
#define KeyStr_MOBILEDATA_Popup_On_Text "IDS_ST_BODY_FLIGHT_MODE_MSG"
#define KeyStr_MOBILEDATA_Popup_Off_Text "IDS_QP_POP_YOU_WILL_NO_LONGER_BE_ABLE_TO_USE_APPLICATIONS_NOTI_MSG"
#define KeyStr_DO_NOT_ASK_AGAIN "IDS_ST_POP_DO_NOT_ASK_AGAIN"
#define SETTING_NETWORK_DOT "IDS_WIFI_BODY_DOT"
#define Data_packet_Popup_desc "IDS_ST_POP_MOBILE_DATA_ENABLED_CONNECTING_TO_MOBILE_NETWORKS_MAY_INCUR_EXTRA_CHARGES_DEPENDING_ON_YOUR_DATA_PLAN_CONTINUE_Q"
#define SETTING_NETWORK_CONTINUE "IDS_COM_OPT_CONTINUE_Q"

#define KeyStr_Survivor_Mode_POP_DESC1 "IDS_ST_BODY_CHANGING_THE_COLOUR_OF_THE_SCREEN_TO_GREYSCALE"
#define KeyStr_Survivor_Mode_POP_DESC2 "IDS_ST_BODY_RESTRICTING_APPLICATION_USAGE_TO_ONLY_ESSENTIAL_APPLICATIONS_AND_THOSE_SELECTED_BY_YOU"
#define KeyStr_Survivor_Mode_POP_DESC3 "IDS_ST_BODY_TURNING_OFF_MOBILE_DATA_WHEN_THE_SCREEN_IS_OFF"
#define KeyStr_Survivor_Mode_POP_DESC4 "IDS_ST_BODY_TURNING_OFF_CONNECTIVITY_FEATURES_SUCH_AS_WI_FI_AND_BLUETOOTH"

#define DATA_KEY_RUNNING_LIST "ra_pid_l"
#define DATA_KEY_HISTORY_LIST "hi_l"

typedef enum {
    HELP_NONE = 0,
    HELP_PS_MODE,
    HELP_EM_MODE,
} help_type_t;

struct appdata {
	ui_gadget_h ug;

	Evas_Object *win;
	Evas_Object *bg;
	Evas_Object *ly_main;
	Evas_Object *popup;
	Evas_Object *popup_chk;
	Evas_Object *disable_popup;
	Evas_Object *enable_popup;
	Evas_Object *timeout_lay;
	int is_ok;

	bundle *b;
	char *type;
	int is_lcd_ctrl;

	/*for reading text */
	tts_h tts;
	Ecore_Timer *play_timer;

	/*for help */
	Evas_Object *help_circle;
	Evas_Object *help_popup;
	Ecore_Timer *help_timer;
	Ecore_Timer *lcd_freq_timer;
	help_type_t *help_type;
	Evas_Object *help_eo;

	int noti_id;
	int all_day, call, noti, alarmNtimer, led;
	int b_call, b_noti, b_alarmNtimer, b_led;
	int start_h, start_m, end_h, end_m;
	int in_range;
	int timeoutCnt;
	/* add more variables here */
};

char *mode_syspopup_gettext(const char *s);

#define _(s)	mode_syspopup_gettext(s)

#endif /* _MODE_SYSPOPUP_APP_H__ */
