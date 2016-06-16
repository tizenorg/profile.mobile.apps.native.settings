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
/**
 *@defgroup setting-cfg
 *configuration file management.
 *Setting app is based on JSON scripting for configuration management.
 */

#ifndef __SETTING_COMMON_STRING_H__
#define __SETTING_COMMON_STRING_H__

#include <setting-common-po-string.h>

#define TTS_CONTENT_MENU_CLICK_INFO \
	"IDS_ST_BODY_TO_OPEN_THE_MENU_DOUBLE_TAP_IT_T_TTS"
#define TTS_CONTENT_BACK_CLICK_INFO \
	"IDS_ST_BODY_DOUBLE_TAP_TO_CLOSE_THE_MENU_T_TTS"

#define EXCEED_LIMITATION_STR "IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"
#define EXCEED_MINLENGTH_LIMITATION_STR "The content is too short"

#define SETTING_ICON_PATH_CFG	_TZ_SYS_RO_APP"/org.tizen.setting/res/icons/"
#define SETTING_LIST_ICON_PATH_CFG SETTING_ICON_PATH_CFG"list_icon/"

#define SETTING_IMAGE_PATH_CFG	_TZ_SYS_RO_APP"/org.tizen.setting/res/images/"
#define SETTING_OPEN_SOURCE_LICENSE_PATH "file:///usr/share/license.html"

#define SETTING_OTHER_UG_ICON_PATH \
	_TZ_SYS_RO_ICONS"/default/small/"
#define SETTING_DEFAULT_CALL_TONE \
	_TZ_SYS_SHARE"/settings/Ringtones/ringtone_sdk.mp3"
#define SETTING_DEFAULT_CALL_TONE_SDK \
	_TZ_SYS_SHARE"/settings/Ringtones/ringtone_sdk.mp3"
#define SETTING_DEFAULT_CALL_VIB \
	_TZ_SYS_SHARE"/settings/Vibrations/haptic/default/Ticktock.ivt"
#define DEF_RINGTONE\
	_TZ_SYS_SHARE"/settings/Ringtones/ringtone_sdk.mp3"
#define SETTING_MMC_PATH \
	_TZ_SYS_MEDIA"/sdcard/"

/***************new V7.0*************/
/* be used to compare with model_str from system_info_get_platform_string(
 * SYSTEM_INFO_KEY_MODEL, &model_str) */
#define KeyStr_Emulator		"Emulator"
#define KeyStr_Back		"IDS_ST_BUTTON_BACK"
/* connectivity keyStr*/
#define KeyStr_Accessibility	"IDS_ST_BODY_ACCESSIBILITY"
#define KeyStr_Connections	"IDS_ST_BODY_CONNECTIONS"
#define KeyStr_Personal		"IDS_ST_HEADER_PERSONALISATION"

#define KeyStr_Device		"IDS_ST_BODY_DEVICE"
#define KeyStr_FlightMode	"IDS_ST_BODY_FLIGHT_MODE"

#define KeyStr_WiFi		"IDS_ST_BODY_WI_FI"
#define KeyStr_WiFiDirect	"IDS_ST_HEADER_WI_FI_DIRECT"
#define KeyStr_Bluetooth	"IDS_TPLATFORM_OPT_BLUETOOTH"
#define KeyStr_MobileAP		"IDS_MOBILEAP_BODY_TETHERING"
#define KeyStr_Location		"IDS_LBS_HEADER_LOCATION"
#define KeyStr_Network		"IDS_ST_BODY_NETWORK"
#define KeyStr_MobileNetworks	"IDS_ST_BODY_MOBILE_NETWORKS"
#define KeyStr_DataRoaming	"IDS_ST_BODY_DATA_ROAMING"
#define KeyStr_UseMobileData	"IDS_ST_MBODY_MOBILE_DATA"
#define KeyStr_DeveloperOption	"IDS_ST_BODY_DEVELOPER_OPTIONS"
#define KeyStr_MoreConnectionSettings "IDS_ST_HEADER_MORE_CONNECTION_SETTINGS"

/* system keyStr*/
#define KeyStr_System		"IDS_ST_BODY_SYSTEM"
#define KeyStr_Menuscreen	"IDS_ST_HEADER_HOME_SCREEN_ABB"
#define KeyStr_Homescreen	"IDS_ST_HEADER_HOME_SCREEN_ABB"
#define KeyStr_Sounds		"IDS_ST_HEADER_SOUND"
#define KeyStr_Wallpaper	"IDS_LCKSCN_BODY_WALLPAPERS"
#define KeyStr_LockScreen	"IDS_ST_HEADER_LOCK_SCREEN"
#define KeyStr_Font		"IDS_ST_BODY_FONT"



#define KeyStr_MotionControl		"IDS_ST_BODY_MOTION"
#define KeyStr_Motions			"IDS_ST_MBODY_MOTIONS_AND_GESTURES"

#define KeyStr_Display			"IDS_ST_HEADER_DISPLAY"
#define KeyStr_ApplicationsItem		"IDS_ST_BODY_APPLICATIONS"
#define KeyStr_ApplicationManager	"IDS_ST_BODY_APPLICATION_MANAGER"
#define KeyStr_Accessibility		"IDS_ST_BODY_ACCESSIBILITY"
#define KeyStr_DefaultApplications	"IDS_ST_HEADER_DEFAULT_APPLICATIONS_ABB"
#define KeyStr_Home			"IDS_ST_BODY_HOME"
#define KeyStr_BacklightTime		"IDS_ST_BODY_SCREEN_TIMEOUT_ABB2"
#define KeyStr_Backlight_Always_On	"IDS_ST_BODY_ALWAYS_ON"
#define KeyStr_Backlight_15_SEC_STR	"IDS_ST_BODY_15SEC"
#define KeyStr_Backlight_30_SEC_STR	"IDS_ST_BODY_30SEC"
#define KeyStr_Backlight_1_MIN_STR	"IDS_ST_BODY_1_MINUTE"
#define KeyStr_Backlight_2_MIN_STR	"IDS_ST_BODY_2_MINUTES"
#define KeyStr_Backlight_5_MIN_STR	"IDS_ST_BODY_5_MINUTES"
#define KeyStr_Backlight_10_MIN_STR	"IDS_ST_BODY_10_MINUTES"

#define KeyStr_DateTime			"IDS_ST_BODY_DATE_AND_TIME"
#define KeyStr_VoiceInOut		"IDS_ST_BODY_VOICE"
#define KeyStr_LanguageInput		"IDS_ST_HEADER_LANGUAGE_AND_INPUT"

#define KeyStr_LanguageRegion		"IDS_ST_BODY_LANGUAGE_AND_REGION_ABB"
#define KeyStr_License			"IDS_ST_BODY_LICENCE"
#define KeyStr_EventsNotifications	"IDS_CLD_HEADER_EVENT_NOTIFICATION"
#define KeyStr_Security			"IDS_ST_BODY_SECURITY"
#define KeyStr_Memory			"IDS_ST_BODY_MEMORY"
#define KeyStr_Reset			"IDS_ST_BUTTON_RESET"
#define KeyStr_AboutDevice		"IDS_ST_BODY_ABOUT_DEVICE"
#define KeyStr_MenuWidgets		"IDS_ST_BODY_MENU_AND_WIDGETS"
#define KeyStr_Accessibility		"IDS_ST_BODY_ACCESSIBILITY"
#define KeyStr_Screen_reader		"IDS_ST_MBODY_SCREEN_READER_HTTS"
#define KeyStr_Guestmode		"Guest mode"	/*unused 2.0 */
#define KeyStr_Accessories		"IDS_HEALTH_OPT_ACCESSORIES"
#define keystr_LedIndicator		"IDS_ST_BODY_LED_INDICATOR"
#define KeyStr_EasyMode			"IDS_ST_BODY_EASY_MODE"
#define KeyStr_FingerScanner		"IDS_ST_HEADER_FINGER_SCANNER_ABB"



/* applications keyStr*/
#define KeyStr_Applications		"IDS_ST_BODY_APPLICATIONS"
#define KeyStr_Accounts			"IDS_ST_BODY_ACCOUNTS"
#define KeyStr_Privacy			"IDS_ST_HEADER_PRIVACY_AND_SAFETY_ABB"

#define KeyStr_NFC			"IDS_ST_BODY_NFC"
#define KeyStr_Storage			"IDS_ST_BODY_STORAGE"

/* applications keyStr*/
#define KeyStr_DownloadedAPPs		"IDS_ST_BODY_APPLICATION"
#define KeyStr_ManageApps		"IDS_ST_BODY_APPLICATION_MANAGER"


#define KeyStr_Save			"IDS_ST_BODY_SAVE"

/*//////////////Common Strings/////////////// */
#define KeyStr_Item_Sel			"Item(s) Selected"
#define UNSUPPORTED_FUNCTION		"IDS_MSGF_POP_UNSUPPORTED"
#define KeyStr_Select_All		"IDS_ST_BODY_SELECT_ALL"
#define KeyStr_Select_Item	"IDS_ST_HEADER_SELECT_VIBRATION_PATTERNS_ABB"
#define KeyStr_INFO			"IDS_PB_BODY_INFO"
#define KeyStr_DeviceMange		"IDS_ST_HEADER_DEVICE_MANAGER"
#define KeyStr_Regulatory_Info		"IDS_ST_BODY_REGULATORY_INFORMATION_ABB"
#define KeyStr_Loading			"IDS_ST_POP_LOADING_ING"

#endif /* __SETTING_COMMON_STRING_H__ */
