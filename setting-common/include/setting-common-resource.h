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

#ifndef __SETTING_COMMON_RESOURCE
#define __SETTING_COMMON_RESOURCE

#define TTS_CONTENT_MENU_CLICK_INFO "IDS_ST_BODY_TO_OPEN_THE_MENU_DOUBLE_TAP_IT_T_TTS"
#define TTS_CONTENT_BACK_CLICK_INFO "IDS_ST_BODY_DOUBLE_TAP_TO_CLOSE_THE_MENU_T_TTS"

#define EXCEED_LIMITATION_STR					"IDS_ST_TPOP_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED"
#define EXCEED_MINLENGTH_LIMITATION_STR		"The content is too short"

#define SETTING_ICON_PATH_CFG					"/usr/apps/org.tizen.setting/res/icons/"
#define SETTING_LIST_ICON_PATH_CFG					SETTING_ICON_PATH_CFG"list_icon/"

#define SETTING_IMAGE_PATH_CFG				"/usr/apps/org.tizen.setting/res/images/"
#define SETTING_OPEN_SOURCE_LICENSE_PATH 	"file:///usr/share/license.html"

#define SETTING_OTHER_UG_ICON_PATH			"/usr/share/icons/default/small/"
#define SETTING_DEFAULT_CALL_TONE		"/opt/usr/share/settings/Ringtones/Over_the_horizon.ogg"
#define SETTING_DEFAULT_CALL_TONE_SDK		"/opt/usr/share/settings/Ringtones/ringtone_sdk.mp3"
#define SETTING_DEFAULT_CALL_VIB		"/opt/usr/share/settings/Vibrations/haptic/default/Ticktock.ivt"
#define DEF_RINGTONE 					"/opt/usr/share/settings/Ringtones/Over_the_horizon.ogg"
#define SETTING_MMC_PATH 				"/opt/storage/sdcard/"

/***************new V7.0*************/
#define KeyStr_Emulator 				"Emulator" /* be used to compare with model_str from system_info_get_platform_string(SYSTEM_INFO_KEY_MODEL, &model_str) */
#define KeyStr_Back					"IDS_ST_BUTTON_BACK"
/* connectivity keyStr*/
#define KeyStr_Accessibility        "IDS_ST_BODY_ACCESSIBILITY"
#define KeyStr_Connections			"IDS_ST_BODY_CONNECTIONS"
#define KeyStr_MoreConnections		"IDS_ST_HEADER_MORE_CONNECTION_SETTINGS"
#define KeyStr_Personal				"IDS_ST_HEADER_PERSONALISATION"

#define KeyStr_Device				"IDS_ST_BODY_DEVICE"
#define KeyStr_FlightMode			"IDS_ST_BODY_FLIGHT_MODE"

#define KeyStr_NetRestrictionMode	"IDS_ST_HEADER_NETWORK_RESTRICTION_MODE_ABB_JPN"
#define KeyStr_WiFi					"IDS_ST_BODY_WI_FI"
#define KeyStr_WiFiDirect				"IDS_ST_HEADER_WI_FI_DIRECT"
#define KeyStr_ReadyToShare			"IDS_ECON_BODY_EASY_CONNECT_M_SHARING_ABB"
#define KeyStr_Bluetooth			"IDS_TPLATFORM_OPT_BLUETOOTH"
#define KeyStr_MobileAP				"IDS_MOBILEAP_BODY_TETHERING"
#define KeyStr_AllShare				"IDS_ST_HEADER_ALLSHARE"
#define KeyStr_ScreenMirroring			"IDS_ST_HEADER_SCREEN_MIRRORING" /*allshare cast -> screen mirroring */
#define KeyStr_Synchronise			"IDS_ST_HEADER_SYNCHRONISE"
#define KeyStr_Location				"IDS_LBS_HEADER_LOCATION"
#define KeyStr_Network				"IDS_ST_BODY_NETWORK"
#define KeyStr_MobileNetworks		"IDS_ST_BODY_MOBILE_NETWORKS"
#define KeyStr_Vpn				"Vpn"
#define KeyStr_DataRoaming			"IDS_ST_BODY_DATA_ROAMING"
#define KeyStr_UsePacketData		"IDS_ST_MBODY_MOBILE_DATA"
#define KeyStr_USBconnection			"IDS_ST_HEADER_USB_UTILITIES"
#define KeyStr_DeveloperOption		"IDS_ST_BODY_DEVELOPER_OPTIONS"
#define KeyStr_MoreConnectionSetting	"IDS_ST_HEADER_MORE_CONNECTION_SETTINGS"

/* system keyStr*/
#define KeyStr_System				"IDS_ST_BODY_SYSTEM"
#define KeyStr_Menuscreen 			"IDS_ST_HEADER_HOME_SCREEN_ABB"
#define KeyStr_Homescreen 			"IDS_ST_HEADER_HOME_SCREEN_ABB"
#define KeyStr_Profile				"IDS_ST_BODY_PROFILE"
#define KeyStr_Theme				"IDS_ST_BODY_WINSET"
#define KeyStr_Sounds				"IDS_ST_HEADER_SOUND"
#define KeyStr_Wallpaper				"IDS_LCKSCN_BODY_WALLPAPERS"
#define KeyStr_Color_Theme			"IDS_ST_MBODY_COLOUR_THEME"
#define KeyStr_HomeAndLockscreen	"IDS_ST_BODY_HOME_AND_LOCK_SCREEN"
#define KeyStr_HomeScreen			"IDS_ST_HEADER_HOME_SCREEN_ABB"
#define KeyStr_LockScreen			"IDS_ST_HEADER_LOCK_SCREEN"
#define KeyStr_Font					"IDS_ST_BODY_FONT"
#define KeyStr_Noti_Panel			"IDS_ST_BODY_NOTIFICATION_PANEL"
#define KeyStr_QUICK_CMD "IDS_QCP_HEADER_QUICK_COMMAND"


#define KeyStr_PalmMotion  "IDS_MTTRL_MBODY_PALM_MOTION"
#define KeyStr_Enable_PlamMotion "IDS_ST_BODY_TO_ENABLE_THIS_FEATURE_ENABLE_AT_LEAST_ONE_RELEVANT_FUNCTION"
#define KeyStr_PlamMotion_TTS "Palm motion and Accessibility cannot both be enabled. Accessibility will be disabled."



/*#define KeyStr_MotionControl			"IDS_ST_ITAB4_CONTROLS_M_INPUT_AND_CONTROL" */
/*#define KeyStr_Motions			"IDS_ST_BODY_MOTION" */
#define KeyStr_MotionControl			"IDS_ST_BODY_MOTION"
#define KeyStr_Motions				"IDS_ST_MBODY_MOTIONS_AND_GESTURES"

#define KeyStr_Powerful_Mode "IDS_ST_BUTTON2_POWER_SAVING_MODE_ABB"
#define KeyStr_Display				"IDS_ST_HEADER_DISPLAY"
#define KeyStr_ApplicationsItem			"Applications"
#define KeyStr_BacklightTime			"IDS_ST_BODY_SCREEN_TIMEOUT_ABB2"
#define KeyStr_Backlight_Alays_On	"IDS_ST_BODY_ALWAYS_ON"
#define KeyStr_Backlight_15_SEC_STR "IDS_ST_BODY_15SEC"
#define KeyStr_Backlight_30_SEC_STR "IDS_ST_BODY_30SEC"
#define KeyStr_Backlight_1_MIN_STR 	"IDS_ST_BODY_1_MINUTE"
#define KeyStr_Backlight_2_MIN_STR 	"IDS_ST_BODY_2_MINUTES"
#define KeyStr_Backlight_5_MIN_STR 	"IDS_ST_BODY_5_MINUTES"
#define KeyStr_Backlight_10_MIN_STR "IDS_ST_BODY_10_MINUTES"

#define KeyStr_DateTime				"IDS_ST_BODY_DATE_AND_TIME"
#define KeyStr_VoiceInOut			"IDS_ST_BODY_VOICE"
#define KeyStr_LanguageInput		"IDS_ST_HEADER_LANGUAGE_AND_INPUT"
#define keystr_Drivingmode			"IDS_VC_MBODY_NOTIFICATION_READ_OUT"/*"IDS_ST_MBODY_HANDS_FREE_MODE"//"IDS_ST_BODY_DRIVING_MODE" */

#define KeyStr_LanguageRegion		"IDS_ST_BODY_LANGUAGE_AND_REGION_ABB"
#define KeyStr_KeyboradLanguage	"IDS_ST_BODY_KEYBOARD"
#define KeyStr_Gestures				"IDS_ST_BODY_GESTURES"
#define KeyStr_Touch					"IDS_ST_BODY_TOUCH"
#define KeyStr_License				"IDS_ST_BODY_LICENCE"
#define KeyStr_EventsNotifications	"IDS_CLD_HEADER_EVENT_NOTIFICATION"
#define KeyStr_Security				"IDS_ST_BODY_SECURITY"
#define KeyStr_PrivacySafety		"IDS_ST_HEADER_PRIVACY_AND_SAFETY_ABB"
#define KeyStr_Memory				"IDS_ST_BODY_MEMORY"
#define KeyStr_Reset				"IDS_ST_BUTTON_RESET"
#define KeyStr_AboutDevice			"IDS_ST_BODY_ABOUT_DEVICE"
#define KeyStr_MenuWidgets			"IDS_ST_BODY_MENU_AND_WIDGETS"
#define KeyStr_Accessibility		"IDS_ST_BODY_ACCESSIBILITY"
#define KeyStr_Screen_reader		"IDS_ST_MBODY_SCREEN_READER_HTTS"
#define KeyStr_Guestmode 			"Guest mode"	/*unused 2.0 */
#define KeyStr_Accessories			"IDS_HEALTH_OPT_ACCESSORIES"
#define keystr_LedIndicator			"IDS_ST_BODY_LED_INDICATOR"
#define KeyStr_EasyMode   			"IDS_ST_BODY_EASY_MODE"
#define KeyStr_FingerScanner   		"IDS_ST_HEADER_FINGER_SCANNER_ABB"



/* applications keyStr*/
#define KeyStr_Applications			"IDS_ST_BODY_APPLICATIONS"
#define KeyStr_Accounts				"IDS_ST_BODY_ACCOUNTS"

/* docomo keystr */
#define KeyStr_Docomo				"IDS_ST_BODY_DOCOMO_JPN_DCM"

#define KeyStr_Call					"IDS_ST_MBODY_CALL_SETTINGS"	/*"IDS_ST_BODY_CALL_SETTINGS"//"IDS_ST_BODY_CALL" */
#define KeyStr_Gallery				"IDS_ST_BODY_GALLERY"
#define KeyStr_Magazine				"Magazine Gallery"
#define KeyStr_Messages				"IDS_ST_BODY_APPLICATIONSETTINGS_MESSAGE"
#define KeyStr_Email					"IDS_ST_BODY_EMAIL"
#define KeyStr_SevenEmail			"Samsung Email"
#define KeyStr_Contacts				"IDS_ST_BODY_CONTACTS"
#define KeyStr_Contact				"IDS_PB_BUTTON_CONTACT"
#define KeyStr_Calendar				"IDS_ST_BUTTON2_S_PLANNER"
#define KeyStr_Internet				"IDS_ST_BODY_CONNECTIONSETTINGS_INTERNET"
#define KeyStr_MediaPlayer			"IDS_ST_BODY_MUSIC"
#define KeyStr_SamsungApps			"IDS_ST_HEADER_SAMSUNG_APPS"
#define KeyStr_PhotoUpload			"IDS_ST_BODY_PHOTO_UPLOAD_SETTINGS"
#define KeyStr_SocialNetwork			"IDS_ST_BODY_SOCIAL_NETWORK_PROFILES"
#define KeyStr_Installation			"IDS_ST_BODY_INSTALLATION_SETTINGS"
#define KeyStr_TaskSwitcher			"IDS_ST_BODY_TASK_SWITCHER"
#define KeyStr_VoiceRecorder			"IDS_ST_BODY_DEFAULTMEMORY_VOICE_RECORDER"
#define KeyStr_FMRadio				"IDS_ST_BODY_FM_RADIO"
#define KeyStr_Twitter				"IDS_ST_BODY_TWITTER"
#define KeyStr_NFC					"IDS_ST_BODY_NFC"
#define KeyStr_SBeam				"IDS_NFC_OPT_S_BEAM"
#define KeyStr_Facebook				"IDS_ST_BODY_FACEBOOK"
#define KeyStr_DataUsage			"IDS_MOBILEAP_MBODY_DATA_USAGE"
#define KeyStr_Storage				"IDS_ST_BODY_STORAGE"

/* applications keyStr*/
#define KeyStr_DownloadedAPPs		"IDS_ST_BODY_APPLICATION"
#define KeyStr_AppsInformation		"IDS_ST_BODY_APPLICATION_INFO"
#define KeyStr_ManageApps			"IDS_ST_BODY_APPLICATION_MANAGER"
#define KeyStr_WebApps				"IDS_ST_BODY_WEB_APPLICATIONS"

#define KeyStr_MoreConnect			"IDS_ST_BODY_MORE_CONNECTIVITY_SETTINGS"
#define KeyStr_MoreSystem			"IDS_ST_BODY_MORE_SYSTEM_SETTINGS"

#define KeyStr_Save                    		 "IDS_ST_BODY_SAVE"

/*//////////////Common Strings/////////////// */
#define KeyStr_Item_Sel 				"Item(s) Selected"
#define UNSUPPORTED_FUNCTION 		"IDS_MSGF_POP_UNSUPPORTED"
#define KeyStr_Select_All				"IDS_ST_BODY_SELECT_ALL"
#define KeyStr_Select_Item				"IDS_ST_HEADER_SELECT_VIBRATION_PATTERNS_ABB"
#define KeyStr_INFO 					"IDS_PB_BODY_INFO"
#define KeyStr_DeviceMange			"IDS_ST_HEADER_DEVICE_MANAGER"
#define KeyStr_Regulatory_Info			"IDS_ST_BODY_REGULATORY_INFORMATION_ABB"
#define KeyStr_Loading				"IDS_ST_POP_LOADING_ING"


/*////////////////////////// */
/*/////// IMAGE //////////// */
/*///////////////////////// */
#define IMG_SETTING	"/usr/share/icons/default/small/org.tizen.setting.png"

#define IMG_Joyn				SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
/* connectivity IMG*/
#define IMG_WiFi				SETTING_LIST_ICON_PATH_CFG"settings_wifi.png"
#define IMG_Bluetooth			SETTING_LIST_ICON_PATH_CFG"settings_bluetooth.png"
#define IMG_FlightMode			SETTING_LIST_ICON_PATH_CFG"settings_flight.png"
#define IMG_MobileAP			SETTING_LIST_ICON_PATH_CFG"settings_tethering.png"
#define IMG_WiFiDirect			SETTING_LIST_ICON_PATH_CFG"settings_wifi_direct.png"
#define IMG_NFC					SETTING_LIST_ICON_PATH_CFG"settings_nfc_and_payment.png"
#define IMG_Network				SETTING_LIST_ICON_PATH_CFG"settings_mobile_networks.png"
#define IMG_More_Connections	SETTING_LIST_ICON_PATH_CFG"settings_more_connections.png"
#define IMG_Location			SETTING_LIST_ICON_PATH_CFG"settings_location.png"

/* Device IMG*/
#define IMG_Sounds				SETTING_LIST_ICON_PATH_CFG"settings_sound_and_notifications.png"
#define IMG_Display				SETTING_LIST_ICON_PATH_CFG"settings_display.png"
#define IMG_Applications		SETTING_LIST_ICON_PATH_CFG"settings_applications.png"

/* Personal IMG */
#define IMG_Wallpaper			SETTING_LIST_ICON_PATH_CFG"settings_wallpapers.png"
#define IMG_LockedScreen		SETTING_LIST_ICON_PATH_CFG"settings_lock_screen_and_security.png"
#define IMG_PrivacySafety		SETTING_LIST_ICON_PATH_CFG"settings_privacy_and_safety.png"
#define IMG_Accessibility		SETTING_LIST_ICON_PATH_CFG"settings_accessibility.png"
#define IMG_Accounts			SETTING_LIST_ICON_PATH_CFG"settings_account.png"
#define IMG_BackupReset			SETTING_LIST_ICON_PATH_CFG"settings_backup_and_reset.png"

/* System IMG */
#define IMG_LanguageInput		SETTING_LIST_ICON_PATH_CFG"settings_language_and_input.png"
#define IMG_Storage				SETTING_LIST_ICON_PATH_CFG"settings_storage.png"
#define IMG_DateTime			SETTING_LIST_ICON_PATH_CFG"settings_date_and_time.png"
#define IMG_AboutDevice			SETTING_LIST_ICON_PATH_CFG"settings_about_device.png"

/* ETC IMG */
#define IMG_Security			SETTING_LIST_ICON_PATH_CFG"settings_security.png"
#define IMG_NetRestrictionMode		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_NetRestrictionOffMode	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*//////////////////////////////////////////////////////////////////////////////// */
#define IMG_BacklightTime		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Font		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_SmartScreen		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_USBconnection		SETTING_LIST_ICON_PATH_CFG"settings_list_developer.png"
#define IMG_SereenMode 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Easy_Mode		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_MutiWindow			SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_BlockingMode		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Firewall				SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_FingerScanner		SETTING_ICON_PATH_CFG"org.tizen.setting.png"


#define IMG_Cloud			SETTING_ICON_PATH_CFG"org.tizen.setting.png"
/*#define IMG_SystemMonitor		SETTING_ICON_PATH_CFG"org.tizen.setting.png" */

#define IMG_Call					SETTING_LIST_ICON_PATH_CFG"settings_call_setting.png"
#define IMG_Messages			SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_Email				SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_Contacts				SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_Calendar				SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_Gallery				SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_VoiceRecorder		SETTING_OTHER_UG_ICON_PATH"org.tizen.setting.png"
#define IMG_ReadyToShare		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_KeisOverWifi			SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_SBeam				SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_DataUsage			SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Web_Apps			"org.tizen.setting.png"
/*
#define IMG_PhoneCalling SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_PhoneCalling_Landscape SETTING_ICON_PATH_CFG"org.tizen.setting.png"
*/

#define IMG_PhoneCalling SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_PhoneCalling_Landscape SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_DirectCalling_Landscape SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_DirectCalling SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*#define IMG_PhoneCalling_Landscape SETTING_ICON_PATH_CFG"org.tizen.setting.png" */
/*#define IMG_VoiceTalk			vconf_get_str(VCONFKEY_BGSET) */
#define IMG_PANNING_CENTER	SETTING_ICON_PATH_CFG "org.tizen.setting.png"
#define IMG_Panning		SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Tilt		SETTING_ICON_PATH_CFG"org.tizen.setting.png" /*vconf_get_str(VCONFKEY_BGSET) */
#define IMG_Palm_MutPause_P     SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Palm_MutPause_L     SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Palm_MutPause_Stop_P     SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Palm_MutPause_Stop_L     SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Palm_Capture_L     SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_Palm_Capture_P     SETTING_ICON_PATH_CFG"org.tizen.setting.png"




/* display UG IMG*/
#define IMG_BRIGTHNESS_LEFT	 SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_BRIGHTNESS_RIGHT	 SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/* screen timeout IMG*/
#define IMG_SCREEN_TIMEOUT_LEFT	 SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*  SOUND IMG*/
#define IMG_SLIDER_BTN_VOLUME01 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_VOLUME02 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_RING_VOLUME \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_NOTI_VOLUME \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_NOTI_VOLUME_MUTE \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_NOTI_VOLUME_VIBRATE \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_SYSTEM_VOLUME \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_SYSTEM_VOLUME_VIBRATE \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

#define IMG_SLIDER_BTN_SYSTEM_VOLUME_MUTE \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"


/*  touch IMG*/
#define IMG_VIBRATION_ICON_01 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_VIBRATION_ICON_02 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_VIBRATION_ICON_03 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*  Sensitivity IMG*/
#define IMG_SENSITIVITY_ICON_01 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_SENSITIVITY_ICON_02 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_SENSITIVITY_PRESSED_ICON_01 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_SENSITIVITY_PRESSED_ICON_02 \
	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*  pdf */
#define PDF_SOURCE_ANNOUNCEMENT \
	"/usr/apps/org.tizen.setting/data/Open_Source_Announcement.pdf"

/* home screen*/
#define IMG_DEFAULT_HOME_SCREEN \
	"org.tizen.setting.png"

#define IMG_Noti_Panel SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_QuickCMD SETTING_ICON_PATH_CFG"org.tizen.setting.png"


/* setting image list*/
#define IMG_SYSTEM_MYFILE_FILE	SYSTEM_ICON_PATH"org.tizen.setting.png"
#define IMG_SYSTEM_MYFILE_MP3	SYSTEM_ICON_PATH"org.tizen.setting.png"
#define IMG_SYSTEM_MYFILE_VOICE	SYSTEM_ICON_PATH"org.tizen.setting.png"
#define IMG_SYSTEM_MYFILE_MP4	SYSTEM_ICON_PATH"org.tizen.setting.png"
#define IMG_SYSTEM_MYFILE_ICON	SYSTEM_ICON_PATH"org.tizen.setting.png"

/* IO profile*/
#define IMG_INOFFICE_HOMESCREEN_PROFILE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_OUTDOOR_HOMESCREEN_PROFILE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_INOFFICE_FONT_PROFILE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_OUTDOOR_FONT_PROFILE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
#define IMG_EMPTY_PROFILE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/* plug in*/
#define IMG_GREEN_SWITCH	SETTING_ICON_PATH_CFG"org.tizen.setting.png"
/*#define IMG_APPS_INFORMATION	SETTING_ICON_PATH_CFG"org.tizen.setting.png" */
#define IMG_NEW_EXCHANGE	"org.tizen.setting.png"
#define IMG_ICON_EMAIL		"org.tizen.setting.png"
#define IMG_ICON_SPLITBER_HOR	"org.tizen.setting.png"

#define IMG_SCREEN_MODE 	SETTING_ICON_PATH_CFG"org.tizen.setting.png"

/*****/

/****Others****/

/*  network*/
extern const char *STR_SETTING_MMS_CONNECTIONS;
extern const char *STR_SETTING_NEW_CONNECTIONS;

/*  phone > langauge*/
extern const char *STR_SETTING_LANG_ENGLISH;
extern const char *STR_SETTING_LANG_GERMAN;
extern const char *STR_SETTING_LANG_DUTCH;
extern const char *STR_SETTING_LANG_SPAINISH;
extern const char *STR_SETTING_LANG_PORTUGUESE;
extern const char *STR_SETTING_LANG_GREEK;
extern const char *STR_SETTING_LANG_ITALIAN;
extern const char *STR_SETTING_LANG_FRENCH;
extern const char *STR_SETTING_LANG_TURKISH;
extern const char *STR_SETTING_LANG_KOREAN;
extern const char *STR_SETTING_LANG_JAPAN;
extern const char *STR_SETTING_LANG_CHINA;
extern const char *STR_SETTING_LANG_CANTONESE;
extern const char *STR_SETTING_LANG_TAIWAN;
extern const char *STR_SETTING_LANG_RUSSIAN;


/* uuid as Setting_Cfg_Node_T primary key */
#define uuid_Connectivity              "1c83e280-3111-42d7-825f-a8f0f2e79577"
#define uuid_Joyn                      "7a837bc3-06c0-7d9f-2ef8-49ebaa4aae2a"
#define uuid_WiFi                      "14cd6b4c-9133-c571-c11f-8cbca16a025a"
#define uuid_DownloadBooster                   "4984d3de-c8c7-11e3-9436-7446a08e0712"
#define uuid_Bluetooth                 "beaf505a-d83c-938d-e9f5-44a5638e75db"
#define uuid_MobileAP                  "1a9b3432-f7a9-0391-134e-03c5a8cab5ba"
#define uuid_FlightMode                        "0f357cea-91bf-0485-2a8b-53569b490827"
#define uuid_DataUsage                 "673eda22-db3e-c12b-b0f6-485867760a11"
#define uuid_Location                  "86029052-c87e-ab5c-2a09-2c1fef36a524"
#define uuid_Network                   "14dacb84-ec2c-d371-1865-a1509d9d95ac"
#define uuid_NFC                       "d5a369bc-f241-9ccc-013b-7aab4bf182c8"
#define uuid_WiFiDirect                        "6f83b633-dbfe-d056-af33-ff624d48d866"
#define uuid_SoundDisplay              "d22ac480-21b3-e81f-a2fd-f232d06b8a0f"
#define uuid_Sounds                    "addd75c8-da64-53ae-e866-019ea8f66f08"
#define uuid_Display                   "4340842d-09d1-2dba-ab2a-283763ec4ec2"
#define uuid_Personal                  "cca58ed0-53f0-d1ca-3a81-2e3171c9a13f"
#define uuid_HomeAndLockscreen         "1708b9fa-b802-9a30-9ae7-ae3574533b3e"
#define uuid_HomeScreen                        "931b0266-c78a-0923-21c0-9ed22b263706"
#define uuid_LockedScreen              "f75bf0dd-744e-3212-c522-a7c25af41c9f"
#define uuid_Menuscreen                        "76f9bb4c-fdd8-a1c2-1a36-be5156b32e28"
#define uuid_Wallpaper                 "345d2710-13cc-875a-b367-a7b0d6b7ef0b"
#define uuid_Accessibility             "6167dc9e-6c41-7970-bd12-c34ba3c6961e"
#define uuid_Call                      "7935d4dc-0ac0-665b-995c-e6d038a34643"
#define uuid_Motions                   "0c5981e6-fe5f-fc79-2e75-1389409fbbdb"
#define uuid_Accounts                  "af754880-549f-9b0f-2b6e-611e0f824135"
#define uuid_Reset                     "a5535088-601b-05bb-3bc6-7b6667f3270a"
#define uuid_DeviceMange               "bc11e5c6-7837-5801-8eac-57504093e285"
#define uuid_LanguageKeyboard          "624e0a2c-10c3-3b46-8665-4f60bf875bbe"
#define uuid_DateTime                  "c79eae8d-8eec-5bf8-9f94-4ecfad09e4ee"
#define uuid_ManageApps                        "c3e7db1d-cb96-b52e-f8ea-6be02ceebe8d"
#define uuid_APP                               "62B00308-89CA-453D-96AC-1C3B6E42BF41"
#define uuid_Storage                   "a5a4121c-0c7b-d6ed-81b6-0f4118aad364"
#define uuid_Security                  "8a684695-8396-cd24-dd7d-c8a3643beca5"
#define uuid_DeveloperOption           "380f3cfb-0f06-cc99-fe58-96f830e0630e"
#define uuid_AboutPhone                        "4d02a807-d0c4-0b0c-8497-5133eebed91a"


#endif				/* end of __SETTING_COMMON_RESOURCE */
