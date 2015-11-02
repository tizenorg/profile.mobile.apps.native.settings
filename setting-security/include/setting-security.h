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
#ifndef __SETTING_SECURITY_H__
#define __SETTING_SECURITY_H__

#include <glib.h>
#include <stdio.h>
#include <dirent.h>

#include <Elementary.h>
#include <Ecore_IMF.h>

#include <ITapiSim.h>
#include <TelSim.h>
#include <TapiUtility.h>
#include <ITapiModem.h>

#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-debug.h>
#include <dlog.h>
#include <net_connection.h>

#include <app_manager.h>
#include <dd-deviced.h>


/*///////////////////face and voice unlock */
#define SET_AS_LOCK_STR "IDS_ST_SK_SET_LITE"/*"Set as lock" */
#define CONTINUE_STR "Continue"
#define SET_UP_STR "Set it up"
#define ADAPT_VOICE_STR "Adapt Voice"

#define About_Motion_Unlock_Str "IDS_ST_HEADER_MOTION_UNLOCK"
#define About_Face_Unlock_Str "About Face Unlock"
#define Setup_Face_Unlock_Str	"Set up Face Unlock"
#define Voice_Record_BtnStr "Voice record icon"
#define Put_Face_Str "<align=middle>Put your face here</align>"
#define Show_Face_Str "Show your face"

#define Motion_Unlock_Guildtext	"Unlock device using motion<br><br>"\
	"Tap and hold the screen then tilt the device "\
	"towards you to unlock it. If the screen does not unlock, "\
	"try tiling the device more. When learn about tilt or motion "\
	"lock are set,motion activation will be automatically enabled "\
	"in Settings"

#define Motion_Unlock_Guildtext1 "IDS_ST_HEADER_UNLOCK_DEVICE_USING_MOTION_ABB"
#define Motion_Unlock_Guildtext2 "IDS_ST_BODY_TAP_AND_HOLD_THE_SCREEN_THEN_TILT_THE_DEVICE_TOWARDS_YOU_TO_UNLOCK_IT_IF_THE_SCREEN_DOES_NOT_UNLOCK_TRY_TILTING_THE_DEVICE_MORE"
#define Motion_Unlock_Guildtext3 "IDS_ST_BODY_WHEN_LEARN_ABOUT_TILT_OR_MOTION_UNLOCK_ARE_SET_MOTION_ACTIVATION_WILL_BE_AUTOMATICALLY_ENABLED_IN_SETTINGS"

#define Face_Unlock_Warring	"Warning<br><br>"\
	"Do not use face unlock while driving, it may distract you from "\
	"the road and cause an accident"

#define Face_Unlock_Warring2	"Look at your phone to Unlock it<br><br>"\
	"Keep these things in mind<br>"\
	".Face Unlock is less secure than PIN or password.<br><br>"\
	".Someone who looks similar to you could unlock your phone.<br><br>"\
	".The data used to identify your face is kept private on the phone."

#define Face_Unlock_Warring3	"For best results:<br><br>"\
	".Find an indoor spot that is not too bright or too dim.<br><br>"\
	".Hold the phone at eye level."

#define Voice_Unlock_Guildtext "Tap button below then say voice command you want to use for Unlock 4 times all at once"
#define Voice_Unlock_Guildtext2 "Use 3-5 syllable words and maintain constant intonation.<br><br>"\
	"All background music and recording will stop when you unlock the device using Face and voice<br>"
#define Voice_Recognized_text "Recognised. Now you can use your own voice command for Unlock.<br><br>"\
	"For assuring higher security. You can adapt voice command by tapping 'Adapt voice' button."

#define Adapt_voice_Str "Adapt Voice"

#define Finish_Setup_Face "You've finished!<br><br>"\
	"To unlock your phone, just look at it.<br><br>"\
	"Tip : improve face matching by having your face capture in different "\
	"conditions."

#define Speak_Now_Str 					"Speak now"
#define Keystr_FDN 						"IDS_ST_BODY_FIXED_DIALLING_MODE"

#define SEC_FIREWALL_DIR "/home/app/.firewall"

#define PIN_BLOCKED_ENTER_PUK_DESC "IDS_ST_BODY_PIN_BLOCKED_ENTER_PUK_TO_CONTINUE_JPN_DCM"
#define SECURITY_MANUAL_UPDATE_TEXT "IDS_ST_POP_CHECK_FOR_SECURITY_UPDATES_AND_DOWNLOAD_NEW_UPDATES_Q_NOTI_MSG"
#define SECURITY_UPDATE_TOGGLE_ON "IDS_ST_POP_ENABLING_AUTO_UPDATE_MEANS_YOUR_DEVICE_WILL_CHECK_FOR_SECURITY_UPDATES_AUTOMATICALLY_NOTI_MSG"
#define SECURITY_UPDATE_TOGGLE_OFF "IDS_ST_POP_DISABLING_AUTO_UPDATE_MEANS_YOU_HAVE_TO_CHECK_FOR_SECURITY_UPDATES_MANUALLY_NOTI_MSG"
#define SECURITY_UPDATE_TEXT "IDS_EMAIL_POP_SECURITY_UPDATE_ABB"
#define UPDATE_TEXT "IDS_ST_BODY_CHECK_FOR_UPDATES"
#define CHECK_UPDATES "IDS_ST_BODY_CHECK_FOR_UPDATES_ABB"
#define AUTO_UPDATE "IDS_ST_OPT_AUTO_UPDATE_ABB"
#define CHECK_UPDATE_AUTO "IDS_ST_BODY_CHECK_FOR_UPDATES_AUTOMATICALLY_ABB"
#define CHECK_UPDATE_AUTO_MSG "IDS_ST_SBODY_AUTOMATICALLY_CHECK_FOR_CHANGES_TO_THE_SECURITY_POLICY_MSG"
#define CONNECT_TO_SERVER "IDS_ST_POP_CONNECTING_TO_THE_SERVER_ING"
#define	SECURITY_VIA_WIFI "IDS_ST_MBODY_VIA_WI_FI_ONLY"
#define	SECURITY_VIA_WIFI_HELP_MSG "IDS_ST_SBODY_ONLY_UPDATE_THE_SECURITY_POLICY_AUTOMATICALLY_WHEN_YOUR_DEVICE_IS_CONNECTED_TO_A_WI_FI_NETWORK"
#define	SECURITY_SIM_NOT_PRESENT_MSG ""
#define	SECURITY_SIM_SETTINGS "IDS_JAVA_OPT_SIM_SETTINGS"


#define SETTING_SECURITY_FIREWALL_BLOCKLIST_NAME_LEN_MAX		256

#if SUPPORT_SECURITY_FIREWALL
typedef struct _BlockList_Info BlockList_Info;
struct _BlockList_Info {
	char name[SETTING_SECURITY_FIREWALL_BLOCKLIST_NAME_LEN_MAX + 1];
};

#endif

#if SUPPORT_ENCRYPTION
#define Keystr_DeviceEncryption 			"IDS_ASEML_BODY_DEVICE_ENCRYPTION"
#define keystr_DeviceDecryption			"Device decryption"
#define DEVICE_ENCRYPTION_DESC		"IDS_ST_BODY_PASSWORD_REQUIRED_TO_DECRYPT_DEVICE_EACH_TIME_YOU_TURN_IT_ON"
#define DEVICE_DECRYPTION_DESC		"IDS_ST_BODY_DEVICE_ENCRYPTED"

#define Keystr_ConfirmEncryption 			"IDS_ST_HEADER_CONFIRM_ENCRYPTION"
#define Keystr_ConfirmDecryption			"IDS_ST_BODY_CONFIRM_DECRYPTION"
#define DEVICE_ENCRYPTION_GUIDE_01	"IDS_ST_BODY_DEVICE_ENCRYPTION_HELP_MSG"
#define DEVICE_ENCRYPTION_GUIDE_03	"IDS_ST_BODY_SET_AN_UNLOCK_PASSWORD_OF_AT_LEAST_6_CHARACTERS_CONTAINING_AT_LEAST_1_NUMBER"
#define DEVICE_ENCRYPTION_GUIDE_04	"IDS_ST_BODY_PLUG_IN_CHARGER_AND_TRY_AGAIN"
#define DEVICE_ENCRYPTION_GUIDE_05	"IDS_ST_BODY_CHARGE_BATTERY_TO_ABOVE_80P_AND_TRY_AGAIN"
#define DEVICE_DECRYPTION_GUIDE_01	"IDS_ST_BODY_YOU_CAN_DECRYPT_ACCOUNTS_SETTINGS_DOWNLOADED_APPLICATIONS_AND_THEIR_DATA_MEDIA_AND_OTHER_FILES_DECRYPTION_WARNING_MSG"
#define CONFIRM_ENCRYPTION_GUIDE_01	"IDS_ST_BODY_ENCRYPT_DEVICE_Q_WARNING_MSG"
#define CONFIRM_ENCRYPTION_GUIDE_02	"IDS_ST_BODY_FAST_ENCRYPTION"
#define CONFIRM_ENCRYPTION_GUIDE_03	"IDS_ST_BODY_IF_YOU_SELECT_THIS_OPTION_ONLY_USED_MEMORY_SPACE_WILL_BE_ENCRYPTED"
#define CONFIRM_DECRYPTION_GUIDE_01	"IDS_ST_BODY_DECRYPT_DEVICE_Q_WARNING_MSG"
#define Keystr_Encrypting				"IDS_ST_BODY_ENCRYPTING_ING"
#define Keystr_Decrypting				"IDS_ST_BODY_DECRYPTING_ING"
#define ENCRYPTING_GUIDE_01			"IDS_ST_BODY_PREPARING_THIS_COULD_TAKE_OVER_10_MINUTES_ING"
#define ENCRYPTING_GUIDE_02			"IDS_ST_BODY_WAIT_WHILE_DEVICE_IS_ENCRYPTED_PDP_COMPLETE"
#define DECRYPTING_GUIDE_02			"IDS_ST_BODY_WAIT_WHILE_DEVICE_IS_DECRYPTED_PDP_COMPLETE"
#define Keystr_EncryptDevice				"IDS_ST_HEADER_ENCRYPT_DEVICE"
#define Keystr_DecryptDevice				"IDS_ST_BODY_DECRYPT_DEVICE"
#endif

#define USELESS_SIM_CARD 				"Not useful SIM card"
#define KeyStr_Security_Waiting_Sim 		"IDS_ST_BODY_WAITING_FOR_RESPONSE_FROM_SIM_CARD_ING"
#define Keystr_ScreenLockType			"IDS_ST_BODY_SCREEN_LOCK_TYPE"
#define Keystr_LockScreenOptions			"IDS_ST_BODY_LOCK_SCREEN_OPTIONS"
#define Keystr_Swipe						"IDS_ST_MBODY_SWIPE"
#define Keystr_FaceAndVoice				"IDS_ST_BODY_FACE_AND_VOICE"

#if SUPPORT_SECURITY_FIREWALL
#define Keystr_Firewall				"IDS_ST_BODY_FIREWALL"
#define Keystr_BlockIP				"IDS_ST_BODY_BLOCK_IP"
#define Keystr_BlockedIPList 			"IDS_ST_BODY_BLOCKED_IP_LIST"
#define Keystr_BlockedIPDesc			"IDS_ST_BODY_USER_CAN_BLOCK_A_SPECIFIC_IP_ADDRESS_MSG"
#define Keystr_BlockURL				"IDS_ST_BODY_BLOCK_URL"
#define Keystr_BlockedURLList		"IDS_ST_BODY_BLOCKED_URL_LIST"
#define Keystr_BlockedURLDesc		"IDS_ST_BODY_USER_CAN_BLOCK_A_SPECIFIC_URL_MSG"
#define Keystr_BlockPort				"IDS_ST_BODY_BLOCK_PORT"
#define Keystr_BlockedPortList		"IDS_ST_BODY_BLOCKED_PORT_LIST"
#define Keystr_BlockedPortDesc		"IDS_ST_BODY_USER_CAN_BLOCK_A_SPECIFIC_PORT_MSG"
#define Keystr_BlockedNetworkType	"IDS_ST_BODY_NETWORK_TYPE"
#define Keystr_BlockedProtocolType	"IDS_ST_BODY_PROTOCOL_TYPE"

#define Keystr_AddIP					"IDS_ST_HEADER_ADD_IP"
#define Keystr_AddURL				"IDS_ST_HEADER_ADD_URL"
#define Keystr_AddPort				"IDS_ST_HEADER_ADD_PORT"
#define Keystr_IP					"IDS_WIFI_BODY_IP"
#define Keystr_URL					"IDS_ST_BODY_URL"
#define Keystr_Port					"IDS_ST_HEADER_PORT"

#define KeyStr_IP_removed			"IDS_ST_POP_IP_ADDRESS_REMOVED"
#define KeyStr_Port_removed			"IDS_ST_POP_PORT_REMOVED"
#define KeyStr_Ports_removed		"IDS_ST_POP_PORTS_REMOVED"
#define KeyStr_URL_removed			"IDS_ST_POP_URL_REMOVED"
#define KeyStr_URLs_removed		"IDS_ST_POP_URLS_REMOVED"
#endif
#define Keystr_Security_Engine      "IDS_ST_BODY_SECURITY_ENGINE"
#define Keystr_Security_Engine_Default "IDS_ST_BODY_ANTIVIRUS_SOFTWARE_ABB"
#define Keystr_Security_Engine_Type_None "IDS_ST_BODY_NONE"
#define Keystr_Security_Engine_Desc      "IDS_ST_BODY_PROTECT_YOUR_DEVICE_FROM_HARMFUL_VIRUSES_AND_MALWARE"
#define Keystr_Firewall_dimm_desc "IDS_IM_POP_THIS_FEATURE_IS_NOT_AVAILABLE_WHILE_FLIGHT_MODE_IS_ON"

typedef struct _SettingSecurityUG SettingSecurityUG;

struct _security_item {
	int pw_type_num;
	char *pw_type_string;
	int (*passwd_handler)(SettingSecurityUG *ad, void *data);
};

#if SUPPORT_SECURITY_FIREWALL
typedef enum {
    SETTING_SEC_FIREWALL_BLOCK_IP,
    SETTING_SEC_FIREWALL_BLOCK_URL,
    SETTING_SEC_FIREWALL_BLOCK_PORT,
    SETTING_SEC_FIREWALL_BLOCK_3G,
    SETTING_SEC_FIREWALL_BLOCK_WIFI,
    SETTING_SEC_FIREWALL_BLOCK_TCP,
    SETTING_SEC_FIREWALL_BLOCK_UDP,
    SETTING_SEC_FIREWALL_BLOCK_MAX
} setting_sec_firewall_blcok_type;

typedef enum {
    SETTING_SEC_FIREWALL_NETWORK_BLOCK_ALLOW_ALL = 0,
    SETTING_SEC_FIREWALL_NETWORK_BLOCK_WIFI,
    SETTING_SEC_FIREWALL_NETWORK_BLOCK_3G,
    SETTING_SEC_FIREWALL_NETWORK_BLOCK_ALL,
    SETTING_SEC_FIREWALL_NETWORK_BLOCK_MAX
} setting_sec_firewall_network_block_type;

typedef enum {
    SETTING_SEC_FIREWALL_PROTOCOL_BLOCK_ALLOW_ALL  = 0,
    SETTING_SEC_FIREWALL_PROTOCOL_BLOCK_TCP,
    SETTING_SEC_FIREWALL_PROTOCOL_BLOCK_UDP,
    SETTING_SEC_FIREWALL_PROTOCOL_BLOCK_ALL,
    SETTING_SEC_FIREWALL_PROTOCOL_BLOCK_MAX
} setting_sec_firewall_protocol_block_type;

typedef struct _bs firewall_db;
typedef struct _bsl firewall_db_list;

struct _bs {
	int _magic;
	int id;
	/*char * ip; */
	char ip[256];
	/*char url[256]; */
	/*char port[256]; */
};

struct _bsl {
	struct _bs firewall_db;
	struct _bsl *prev;
	struct _bsl *next;
};

typedef struct _commandList {
	/*char ** arg_list ; */
	char *command;
	struct _commandList *next;
} COMMAND;

COMMAND *g_ipHead, *g_ipTail;
COMMAND *g_urlHead, *g_urlTail;
COMMAND *g_portHead, *g_portTail;


#endif
#define MAX_VOICE_TIME 4
#define MAX_SEC_PROFILE_NUM 100


enum {
    SETTING_SEC_VIEWTYPE_MAIN = 0,
    SETTING_SEC_VIEWTYPE_ENCRYPTION,
    SETTING_SEC_VIEWTYPE_LOCKTYPE,
    SETTING_SEC_VIEWTYPE_UPDATE,
    SETTING_SEC_VIEWTYPE_MAX
};

/**
 * Setting Security UG context
 * all UG function has void* as an agument. this is casted back to SettingSecurityUG
 * and the functions access app context.
 */
struct _SettingSecurityUG {
	ui_gadget_h ug;
	ui_gadget_h ug_loading;
	TapiHandle *handle;
	TelSimFacilityStatus_t sim_status;
	TelSimFacilityStatus_t pin1_status;
	TelSimFacilityStatus_t pin2_status;/*it indicates fdn status too */
	unsigned int enter_tapi_async_cb_flag;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *notify;
	Evas_Object *video_ly;
	Evas_Object *video_ly_show_face_btn;
	Evas_Object *video_ly_content_face;
	Ecore_Timer *update_timer;
	Evas_Object *image[MAX_VOICE_TIME];
	int processed_img_num;
	Evas_Object *record_genlist;
	Elm_Object_Item *screen_lock_main_item;
	char *selected_lock_type;

	int viewtype;

	/*to export lock type UG */
	setting_view *view_to_load;
	Evas_Object *save_popup;

	Evas_Object *ly_guild;
	Evas_Object *ly_main;
	Evas_Object *navi_bar;
	Evas_Object *sim_popup;
	ui_gadget_h ug_passwd;
	ui_gadget_h ug_mmc_encryption;
	ui_gadget_h ug_mt;
	ui_gadget_h ug_firewall;
	ui_gadget_h ug_locktype;

	Evas_Object *genlist;
	Evas_Object *sec_genlist;
	Evas_Object *pop_manual_update;
	Evas_Object *pop_progress;
	Evas_Object *pop_auto_update_on;
	Evas_Object *pop_auto_update_off;

#if SUPPORT_SECURITY_FIREWALL
	Elm_Object_Item 	*navi_it;
	Evas_Object *view_layout;
	Evas_Object *scroller;
	Evas_Object *content;
	Evas_Object *focus_timer;
	int list_cnt;
	char *interface_name;
	connection_h connection;
	connection_profile_h profile;
#endif
	Setting_GenGroupItem_Data *data_phone_lk;
	Setting_GenGroupItem_Data *data_mb;
	Setting_GenGroupItem_Data *data_sim_lk;
	Setting_GenGroupItem_Data *data_pin_lk;
	Setting_GenGroupItem_Data *data_parental_md;
	/*Setting_GenGroupItem_Data *data_simple_pw; */
	Setting_GenGroupItem_Data *data_change_pin1;
	Setting_GenGroupItem_Data *data_change_pin2;
	Setting_GenGroupItem_Data *data_sim_settings;
	/* for simple password */

	Setting_GenGroupItem_Data *data_cng_pw;
#if SUPPORT_ENCRYPTION
	Setting_GenGroupItem_Data *data_device_enc;
	Setting_GenGroupItem_Data *data_mmc_enc;
	Setting_GenGroupItem_Data *data_enc_btn;
	Setting_GenGroupItem_Data *data_confirm_enc_btn;
	int cur_enc_mode; /* 0: unencrypted, 1: encrypted */
	Ecore_Event_Handler *event_handler;
	Evas_Object *enc_progressbar;
	Evas_Object *enc_layout;
	Evas_Object *enc_genlist;
	Evas_Object *sub_layout;
	Evas_Object *scroller_lable;
	Evas_Object *enc_scroller;
	Evas_Object *popup_sd_card_encryption;
	Evas_Object *popup_device_encryption;
#endif
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_bg_1icon;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1icon;

	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_2text_3_parent;


#if SUPPORT_FDN
	Setting_GenGroupItem_Data *data_fdn;
	Evas_Object *chk_fdn;
#endif

	setting_sec_pw_type pw_type;
#if SUPPORT_SECURITY_FIREWALL
	Elm_Object_Item *firewall_separator;
	setting_sec_firewall_blcok_type firewall_block_type;
#endif

	Evas_Object *sl_scroller;
	ui_gadget_h ug_lockscreen;

	Setting_GenGroupItem_Data *data_set_screenlock_btn;

#if SUPPORT_SECURITY_FIREWALL
	firewall_db *fdb;
	Evas_Object *blocked_list;
	Setting_Done_List_Data data_block_list;
	Setting_GenGroupItem_Data *data_security_firewall;
	Setting_GenGroupItem_Data *data_firewall_disable_desc;
	Setting_GenGroupItem_Data *data_security_engine;
	Setting_GenGroupItem_Data *data_security_update;
	Setting_GenGroupItem_Data *data_block_ip;
	Setting_GenGroupItem_Data *data_blocked_ip_list;
	Setting_GenGroupItem_Data *data_blocked_ip_des;
	Setting_GenGroupItem_Data *data_block_url;
	Setting_GenGroupItem_Data *data_blocked_url_list;
	Setting_GenGroupItem_Data *data_blocked_url_des;
	Setting_GenGroupItem_Data *data_block_port;
	Setting_GenGroupItem_Data *data_blocked_port_list;
	Setting_GenGroupItem_Data *data_blocked_port_des;
	Setting_GenGroupItem_Data *data_block_network_type;
	Setting_GenGroupItem_Data *data_block_protocol_type;
	Setting_GenGroupItem_Data *data_security_keypad;
	Setting_GenGroupItem_Data *ip_list;
	Setting_GenGroupItem_Data *url_list;
	Setting_GenGroupItem_Data *port_list;
#endif

	Setting_GenGroupItem_Data *data_security_update_bt;
	Setting_GenGroupItem_Data *data_security_update_chk;
	Setting_GenGroupItem_Data *data_security_wifi_update_chk;
	char *caller;
	char *input_pwd;

	int sel_item; /**< It's used to check PIN1, PIN2 _BLOCKED. 0:click pin lock, 1:change pin1 code, 2:fdn, 3: change pin2 code */
	Ecore_Timer *update_view_timer;
	Ecore_Timer *get_security_list_timer;

	Ecore_Timer *remove_sim_popup_timer;
	Ecore_Timer *tapi_async_cb_check_timer;
	/*for Security engine */
	GList *secrity_engine_list;

	bool ug_removed;
	bool pin2_blocked_flag;
	bool samsung_account_is_running;

	struct mmc_contents mmc_content; /* Handle exception of SD card encryption */
};

extern setting_view setting_view_security_main;
#if SUPPORT_ENCRYPTION
extern setting_view setting_view_security_encryption;
extern setting_view setting_view_security_confirm_encryption;
#endif
/*extern setting_view setting_view_security_enc_processing; */
/*extern setting_view setting_view_security_locktype; */
extern setting_view setting_view_security_sim_settings;

#if SUPPORT_SECURITY_FIREWALL
/*extern setting_view setting_view_security_firewall; */
/*extern setting_view setting_view_security_blocked_list; */
/*extern setting_view setting_view_security_firewall_keypad; */
/*extern setting_view setting_view_security_delete_block_list; */
#endif

extern setting_view setting_view_security_update;

SettingSecurityUG *g_ad;	/* for tapi callback */

/*char *get_screen_lock_type_str(void * priv); */
void setting_security_result_password_ug_cb(ui_gadget_h ug,
                                            app_control_h service, void *priv);
void setting_security_sim_get_facility_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_security_pin_get_facility_cb(TapiHandle *handle, int result, void *data, void *user_data);
void setting_security_pin2_get_lock_info_cb(TapiHandle *handle, int result, void *data, void *user_data);

gboolean setting_security_create_password_sg(void *data);

void setting_security_destroy_password_ug_cb(ui_gadget_h ug,
                                             void *priv);
void setting_security_layout_passwd_ug_cb(ui_gadget_h ug,
                                          enum ug_mode mode, void *priv);

gboolean setting_security_create_lockscreen_options_sg(void *data);
gboolean setting_security_create_mmc_encryption_sg(void *data);
gboolean setting_security_create_firewall_sg(void *data);

#endif

#if SUPPORT_SECURITY_FIREWALL
int setting_security_system_command(const char *command, const char *commandex);
\
int setting_security_firewall_reset();
#endif
