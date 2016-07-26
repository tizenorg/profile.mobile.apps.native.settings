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

#include "setting-common-init.h"


#define Keystr_FDN "IDS_ST_BODY_FIXED_DIALLING_MODE"
#define SEC_FIREWALL_DIR "/home/app/.firewall"
#define PIN_BLOCKED_ENTER_PUK_DESC \
	"IDS_ST_BODY_PIN_BLOCKED_ENTER_PUK_TO_CONTINUE_JPN_DCM"
#define SECURITY_MANUAL_UPDATE_TEXT \
	"IDS_ST_POP_CHECK_FOR_SECURITY_UPDATES_AND_DOWNLOAD_NEW_UPDATES_Q_NOTI"\
	"_MSG"
#define SECURITY_UPDATE_TOGGLE_ON \
	"IDS_ST_POP_ENABLING_AUTO_UPDATE_MEANS_YOUR_DEVICE_WILL_CHECK_FOR_SECU"\
	"RITY_UPDATES_AUTOMATICALLY_NOTI_MSG"
#define SECURITY_UPDATE_TOGGLE_OFF \
	"IDS_ST_POP_DISABLING_AUTO_UPDATE_MEANS_YOU_HAVE_TO_CHECK_FOR_SECURITY"\
	"_UPDATES_MANUALLY_NOTI_MSG"
#define SECURITY_UPDATE_TEXT "IDS_EMAIL_POP_SECURITY_UPDATE_ABB"
#define UPDATE_TEXT "IDS_ST_BODY_CHECK_FOR_UPDATES"
#define AUTO_UPDATE "IDS_ST_OPT_AUTO_UPDATE_ABB"
#define CHECK_UPDATE_AUTO_MSG \
	"IDS_ST_SBODY_AUTOMATICALLY_CHECK_FOR_CHANGES_TO_THE_SECURITY_POLICY_M"\
	"SG"
#define CONNECT_TO_SERVER "IDS_ST_POP_CONNECTING_TO_THE_SERVER_ING"
#define	SECURITY_VIA_WIFI "IDS_ST_MBODY_VIA_WI_FI_ONLY"
#define	SECURITY_VIA_WIFI_HELP_MSG \
	"IDS_ST_SBODY_ONLY_UPDATE_THE_SECURITY_POLICY_AUTOMATICALLY_WHEN_YOUR_"\
	"DEVICE_IS_CONNECTED_TO_A_WI_FI_NETWORK"
#define	SECURITY_SIM_NOT_PRESENT_MSG ""
#define	SECURITY_SIM_SETTINGS "IDS_JAVA_OPT_SIM_SETTINGS"
#define KeyStr_Security_Waiting_Sim \
	"IDS_ST_BODY_WAITING_FOR_RESPONSE_FROM_SIM_CARD_ING"


typedef struct _SettingSecurity SettingSecurity;

struct _security_item {
	int pw_type_num;
	char *pw_type_string;
	int (*passwd_handler)(SettingSecurity *ad, void *data);
};

enum {
	SETTING_SEC_VIEWTYPE_MAIN = 0,
	SETTING_SEC_VIEWTYPE_ENCRYPTION,
	SETTING_SEC_VIEWTYPE_UPDATE,
};

/**
 * Setting Security context
 */
struct _SettingSecurity {
	MainData md;

	TapiHandle *handle;
	TelSimFacilityStatus_t sim_status;
	TelSimFacilityStatus_t pin1_status;
	TelSimFacilityStatus_t pin2_status;/*it indicates fdn status too */
	unsigned int enter_tapi_async_cb_flag;

	int viewtype;

	/* to export lock type */
	setting_view *view_to_load;

	Evas_Object *sim_popup;
	ui_gadget_h ug_mmc_encryption;

	Evas_Object *pop_manual_update;
	Evas_Object *pop_progress;
	Evas_Object *pop_auto_update_on;
	Evas_Object *pop_auto_update_off;

	/*Setting_GenGroupItem_Data *data_sim_lk;*/
	Setting_GenGroupItem_Data *data_pin_lk;
	Setting_GenGroupItem_Data *data_change_pin1;
	Setting_GenGroupItem_Data *data_change_pin2;
	/*Setting_GenGroupItem_Data *data_sim_settings;*/

	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_bg_1icon;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_2text_3_parent;

#if SUPPORT_FDN
	Setting_GenGroupItem_Data *data_fdn;
#endif

	setting_sec_pw_type pw_type;

#if SUPPORT_SECURITY_FIREWALL
	Setting_GenGroupItem_Data *data_security_update;
#endif

	Setting_GenGroupItem_Data *data_security_update_bt;
	Setting_GenGroupItem_Data *data_security_update_chk;
	Setting_GenGroupItem_Data *data_security_wifi_update_chk;
	char *input_pwd;

	Ecore_Timer *update_view_timer;
	Ecore_Timer *tapi_async_cb_check_timer;

	bool pin2_blocked_flag;
};

extern setting_view setting_view_security_main;
#if SUPPORT_ENCRYPTION
extern setting_view setting_view_security_encryption;
extern setting_view setting_view_security_confirm_encryption;
#endif
extern setting_view setting_view_security_sim_settings;
extern setting_view setting_view_security_update;

void setting_security_sim_get_facility_cb(TapiHandle *handle, int result,
		void *data, void *user_data);
void setting_security_pin_get_facility_cb(TapiHandle *handle, int result,
		void *data, void *user_data);

gboolean setting_security_create_password_sg(void *data);

#endif
