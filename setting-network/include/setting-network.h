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
#ifndef __SETTING_NETWORK_H__
#define __SETTING_NETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <Elementary.h>
#include <Ecore_IMF.h>
#include <string.h>

#include <glib-object.h>
#include <net_connection.h>

#include <efl_extension.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-debug.h>

#include <TapiUtility.h>
#include <ITapiNetwork.h>
#include <ITapiSim.h>

#include <Ecore_IMF.h>

#include <bundle_internal.h>
#include <system_settings.h>

#define UI_NETWORK_MODE

/*#include <tethering.h> */

#define TIME_TO_TURN_OFF_3G 2

/*for new metwork mode */
/*"GSM/WCDMA (auto connect)" */
#define Keystr_NetBrand_Auto "IDS_CST_OPT_GSM_WCDMA_HAUTO_CONNECT"
/*"GSM/WCDMA (auto connect)" */
#define Keystr_NetBrand_Auto_WCDMA_GSM "IDS_CST_OPT_GSM_WCDMA_HAUTO_CONNECT"

#define Keystr_NetBrand_WCDMA "IDS_ST_MBODY_WCDMA_ONLY"
#define Keystr_NetBrand_LTE_WCDMA "LTE/3G"

#define Keystr_NetBrand_GSM "IDS_ST_MBODY_GSM_ONLY"
/*************************************************** // */
#define KeyStr_UserID "IDS_ST_BODY_USER_ID"

#define SUPPORT_CON_RESET
/*#define NET_CON_RESET_DBUS_PARA  "--system --print-reply --dest=com.tcore.ps
 * / com.tcore.ps.master.ResetProfile" */
#define NET_CON_RESET_DBUS_PARA \
	"--system --print-reply --dest=com.tcore.ps / com.tcore.ps.master.Rese"\
	"tProfile int32:0"

#define Data_packet_Sub_desc \
	"IDS_ST_BODY_ENABLE_DATA_ACCESS_OVER_MOBILE_NETWORK"
#define Data_packet_Popup_desc \
	"IDS_ST_POP_MOBILE_DATA_ENABLED_CONNECTING_TO_MOBILE_NETWORKS_MAY_INCU"\
	"R_EXTRA_CHARGES_DEPENDING_ON_YOUR_DATA_PLAN_CONTINUE_Q"
#define SETTING_NETWORK_DATA_ROMING_DESC \
	"IDS_COM_BODY_CONNECT_TO_DATA_SERVICES_WHEN_ROAMING"
#define SETTING_NETWORK_DATA_ROMING_OFF_DESC \
	"IDS_ST_BODY_WHEN_DATA_ROAMING_IS_DISABLED_APPLICATIONS_THAT_USE_THE_D"\
	"ATA_CONNECTION_MAY_NOT_WORK"

#define SETTING_NETWORK_USE_PACKET_DATA_OFF_DESC \
	"IDS_ST_BODY_USING_MOBILE_DATA_MAY_RESULT_IN_ADDITIONAL_CHARGES_CONTIN"\
	"UE_Q"
#define SETTING_NETWORK_USE_PACKET_DATA_ON_DESC "IDS_ST_POP_ROAMING_WILL_INCUR"\
	"_EXTRA_CHARGES_CONTINUE_Q"

/* Unable to search networks during data service activated. Do you want to
 * disconnect the data service and search available network? */
#define SETTING_NETWORK_SEARCH_3G_ON_DESC \
	"IDS_ST_POP_UNABLE_TO_SEARCH_NETWORKS_DURING_DATA_SERVICE_ACTIVATED_DO"\
	"_YOU_WANT_TO_DISCONNECT_THE_DATA_SERVICE_AND_SEARCH_AVAILABLE_NETWORK"\
	"_Q"
#define SETTING_NETWORK_CHANGE_3G_ON_DESC \
	"IDS_ST_POP_UNABLE_TO_CHANGE_NETWORKS_WHILE_DATA_SERVICE_IN_USE_STOP_U"\
	"SING_CURRENT_DATA_SERVICE_AND_SELECT_ANOTHER_AVAILABLE_NETWORK_Q"

#define SETTING_NETWORK_SEARCHING_NETWORK "IDS_ST_POP_SEARCHING_NETWORK_ING"
#define KeyStr_Network_Mobile_Data_Has_Been_Disabled_Msg \
	"IDS_ST_POP_MOBILE_DATA_HAS_BEEN_DISABLED_MSG"
#define KeyStr_Network_Turn_Off_Mobile_Data "IDS_ST_HEADER_TURN_OFF_MOBILE_DATA"
#define KeyStr_Network_Turn_On_Mobile_Data "IDS_ST_HEADER_TURN_ON_MOBILE_DATA"

#define CONNECTION_DSC "IDS_ST_BODY_CONNECTIONS"
#define KeyStr_Reset_Def "IDS_ST_BODY_RESET_TO_DEFAULT"
#define KeyStr_Edit "IDS_ST_BODY_EDIT"
#define KeyStr_Saved "IDS_ST_BODY_SAVED"
#define keyStr_Searched "IDS_SSEARCH_POP_SEARCHED"
#define keyStr_Failed_Searched "IDS_ST_POP_NETWORK_SEARCH_FAILED"
#define keyStr_Failed_Select_Network \
	"IDS_COM_BODY_YOUR_SIM_CARD_DOES_NOT_ALLOW_A_CONNECTION_TO_THIS_NETWORK"
#define keyStr_Failed_Get_PreferNetwork "Get preferred network failed"
#define keyStr_Forbidden_Network _("IDS_ST_BODY_FORBIDDEN_NETWORK")
#define keyStr_OK _("IDS_ST_BUTTON_OK_ABB")
#define keyStr_CANCEL	_("IDS_ST_BUTTON_CANCEL_ABB2")

#define KeyStr_No_Profile _("IDS_ST_BODY_EMPTY")
#define keyStr_No_Other_Network "IDS_ST_BODY_THERE_ARE_NO_NETWORKS_NEARBY"

/*IDS_ST_BODY_ACCESS_NAME */
#define Insert_Profile_Name_desc "Please input Profile name !"
/*IDS_ST_BODY_ACCESS_NAME */
#define Insert_Access_Name_desc "Please input Access name !"
#define Insert_User_Name_Desc "Please input User ID !" /*IDS_ST_BODY_USERNAME */
#define Invok_API_Failed_Desc "Failed to invoke API"
#define SETTING_NETWORK_NOT_ALLOWED_WITH_TETHERING \
	"Unable to change APN when tethering is enabled"
#define SETTING_NETWORK_DOT "IDS_WIFI_BODY_DOT"
#define SETTING_NETWORK_CONTINUE "IDS_COM_OPT_CONTINUE_Q"

/*in the feture */
#define NotifyStr_PDP_OFF "PDP will be disabled to find network nearby"
#define Keystr_UNKNOWN_NETWORK _("IDS_ST_OPT_UNTITLED_NETWORK")

/*#define WHITELIST_DIR_PATH	_TZ_SYS_DATA"/setting/lteon_netlist.xml" */
#define WHITELIST_DIR_PATH	"/opt/system/csc/usr/network/lteon_netlist.xml"

#define SETTING_ADD_PROFILE_INDEX -1
#define Invoking_TAPI_Timeout  8
#define COUNTRY_CODE_LEN	3
#define NETWORK_CODE_LEN	3
#define PRE_NETWORK_NAME_MAX 256
#define SIM_CARDS_MAX 2

/* the popupresponse action ID */
#define SETTING_NETWORK_POP_RESPONSE_ACTION_CANCEL	10
#define SETTING_NETWORK_SUBS_ID_NET_LEN 7
typedef enum {
	SETTING_NETWORK_PREFERRED_NETWORK_NONE,
	SETTING_NETWORK_PREFERRED_NETWORK_NEW,
	SETTING_NETWORK_PREFERRED_NETWORK_EDIT,
	SETTING_NETWORK_PREFERRED_NETWORK_LIST,
	SETTING_NETWORK_PREFERRED_NETWORK_DELETE,

} setting_network_preferred_network_operation_type_t;

typedef struct _SettingNetworkUG SettingNetworkUG;

struct gl_network_data {
	int index;
	char name[PRE_NETWORK_NAME_MAX];
};

typedef enum {
	ERROR_STATE = 0,
	MAIN_ONLY = 1,
	CONNECTIONS_MAIN = 2,
	CREATE_CONNECTIONS_MAIN = 3,
	CREATE_CONLIST_CONNECTIONS_MAIN = 4
} setting_network_view_type_t;

#define MAX_PROFILE_NUM 100
/**
 * Setting Network UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingNetworkUG and the functions access app context.
 */
struct _SettingNetworkUG {
	ui_gadget_h ug;
	TapiHandle *handle[SIM_CARDS_MAX];
	connection_h connection[SIM_CARDS_MAX];
	connection_profile_h sel_profile_h;
	setting_view *profile_topper_view;
	char spn_names[SIM_CARDS_MAX][PRE_NETWORK_NAME_MAX];

	/*xmlDocPtr whitelist_doc; */
	/*xmlNodePtr whitelist_root_node; */
	void *whitelist_doc;
	void *whitelist_root_node;

#if SUPPORT_TETHERING
	tethering_h th_main;
	tethering_h th_connections;
	tethering_h th_conlists;
	tethering_h th_concreate;
#endif
	int profile_service_type;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *navi_bar;

	Evas_Object *network_mode_popup[SIM_CARDS_MAX];

	Evas_Object *popup_concreate;
	Evas_Object *popup_conlist;
	Evas_Object *popup_conreset;
	Evas_Object *popup_conreset_complete;
	Evas_Object *popup;
	Evas_Object *popup_chk;

	Evas_Object *popup_auth_type;
	Evas_Object *popup_pdn_type;
	Evas_Object *popup_data_on;
	Evas_Object *popup_data_off;

	/*connection, */
	Elm_Object_Item *navi_it;
	Elm_Object_Item *navi_it_con_list;
	Elm_Object_Item *navi_it_profile_del_list;
	Evas_Object *bottom_btn;

	/*connection-create */
	Evas_Object *con_create_gl; /* connection create */

	/*Evas_Object *conformant; */
	Evas_Object *back_btn;
	Evas_Object *l_button;
	Evas_Object *r_button;
	/*Evas_Object *m_button; */

	bool has_form_changed;
	Evas_Object *genlist;
	Evas_Object *genlist_sel_provider;
	Evas_Object *connections_gl; /* genlist in connections view */
	Evas_Object *con_list_gl; /* genlist in connections view */
	int cur_profile_num;

	Evas_Object *network_ug_pop[SIM_CARDS_MAX];
	Evas_Object *network_select_registering_pop;
	Evas_Object *controllbar;

	char *view_type_string;

	Evas_Object *ly_main;
	unsigned int subs_id_net[SETTING_NETWORK_SUBS_ID_NET_LEN];

	Evas_Point point_down;

	Setting_GenGroupItem_Data *data_auto_network_item;
	Setting_GenGroupItem_Data *data_search_network_item;

	bool b_searching_network;
	bool b_set_auto_network;
	bool b_set_manul_network;
	bool b_trigged_by_searching;
	bool b_trigged_by_modechanging;
	bool b_restore_3g_status;
	bool b_restore_roaming_status;

	Setting_GenGroupItem_Data *data_sel_net;
#ifdef UI_NETWORK_MODE
	Setting_GenGroupItem_Data *data_net_mode[SIM_CARDS_MAX];
#endif
	Setting_GenGroupItem_Data *data_connection;
	Setting_GenGroupItem_Data *data_mobile_data;
	Setting_GenGroupItem_Data *data_roaming;
	Setting_GenGroupItem_Data *data_srv_type;
	Setting_GenGroupItem_Data *internet_conn;
	Setting_GenGroupItem_Data *mms_conn;
	Setting_GenGroupItem_Data *selected_profile;

	Setting_GenGroupItem_Data *lte_wcdma_gsm[SIM_CARDS_MAX];
	Setting_GenGroupItem_Data *wcdma_gsm[SIM_CARDS_MAX];
	Setting_GenGroupItem_Data *wcdma_only[SIM_CARDS_MAX];
	Setting_GenGroupItem_Data *gsm_only[SIM_CARDS_MAX];

	Evas_Object *chk_sel;
	Evas_Object *chk_mode;
	Evas_Object *pop_gsm;
	Evas_Object *chk_3gcon;

	int sel_net; /**< like vconf key:VCONFKEY_SETAPPL_SELECT_NETWORK_INT */
	int sel_act; /**< like vconf key:VCONFKEY_SETAPPL_SELECT_OLD_NT_ACT */
	char *sel_network;
#ifdef UI_NETWORK_MODE
	int net_mode[SIM_CARDS_MAX]; /* like vconf key */
#endif

	TelNetworkPlmnList_t plmn_info;

	int is_show_user;
	char *con_name;
	char *access_name;

	Evas_Object *chk_type;
	Evas_Object *chk_type_pdn;
	Evas_Object *chk_type_roam_pdn;

	char *ed_profile_name_desc;
	char *ed_acs_name_desc;
	char *ed_auth_type_desc;
	char *ed_user_name_desc;
	char *ed_pwd_desc;
	char *ed_pxy_addr_desc;
	char *ed_pxy_port_desc;
	char *ed_pdn_type_desc;
	char *ed_roam_pdn_type_desc;
	char *ed_hm_url_desc;
	int chkType;
	int chkType_pdn;
	int chkType_roam_pdn;
	int srvType;
	bool is_editable;
	Elm_Object_Item *item_above_user_name;
	Elm_Object_Item *item_above_proxy_add;

	Setting_GenGroupItem_Data *data_user_name;
	Setting_GenGroupItem_Data *data_pwd;
	Setting_GenGroupItem_Data *data_acs_name;
	Setting_GenGroupItem_Data *data_profile_name;
	Setting_GenGroupItem_Data *data_pxy_addr;
	Setting_GenGroupItem_Data *data_pxy_port;
	Setting_GenGroupItem_Data *data_pdn_type;
	Setting_GenGroupItem_Data *data_roam_pdn_type;
	Setting_GenGroupItem_Data *data_hm_url;
	Setting_GenGroupItem_Data *data_auth_type;
	Setting_GenGroupItem_Data *data_type_nor;
	Setting_GenGroupItem_Data *data_type_sec;
	Setting_GenGroupItem_Data *data_type_none;

	Evas_Object *scl_edit;

	Evas_Object *gl_profile_del;
	Eina_List *profile_list;
	Eina_List *profile_del_list;

	/* Preferred network network list for genlist */
	Evas_Object *gl_network;
	Elm_Genlist_Item_Class itc;

	Evas_Object *selInfoPop;
	Evas_Object *selInfoPop_layout;
	Evas_Object *rdg;

	/* Delete */
	Setting_GenGroupItem_Data *data_delete_all;
	/**********************************************/
	setting_view *view_to_load;
	Ecore_Timer *timer;

	char *apn_internet;
	char *apn_MMS;
	Ecore_Idler *add_apn_idler;
	Ecore_Idler *add_apn_timer;

	int is_update;

	int viewtype;

	/*dual sim */
	int sim_type;
};

extern setting_view setting_view_network_main;
extern setting_view setting_view_network_select_provider;
extern setting_view setting_view_network_con;
extern setting_view setting_view_network_con_list;
extern setting_view setting_view_network_connection_create;
extern setting_view setting_view_network_profile_delete;

const char *setting_network_get_act_str(TelNetworkSystemType_t type);
void setting_network_update_sel_network(void *data);
bool setting_network_equal_profile(connection_profile_h pf1,
		connection_profile_h pf2);
void setting_network_reget_profile_list(void *cb);

void setting_tapi_cancel_manual_search_cb(TapiHandle *handle, int result,
		void *data, void *user_data);
void setting_tapi_search_network_cb(TapiHandle *handle, int result, void *data,
		void *user_data);

void setting_tapi_get_serving_network_cb(TapiHandle *handle, int result,
		void *data, void *user_data);
void setting_tapi_get_plmn_mode_cb(TapiHandle *handle, int result, void *data,
		void *user_data);
void setting_tapi_set_plmn_mode_cb(TapiHandle *handle, int result, void *data,
		void *user_data);

void setting_tapi_get_band_cb(TapiHandle *handle, int result, void *data,
		void *user_data);
void setting_tapi_set_band_cb(TapiHandle *handle, int result, void *data,
		void *user_data);

void setting_network_select_network_chk_changed(void *data, Evas_Object *obj,
		void *event_info);
void setting_network_Gendial_select_plmn_cb(void *data, Evas_Object *obj,
		void *event_info);
void setting_network_popup_delete(void *data, int sim_ix);
int is_lte_on_feature(void *data);
char *__get_profile_name(int conType, void *data);

void setting_network_searching_network(Setting_GenGroupItem_Data *list_item);

void setting_network_set_state_mobile_data(ButtonState state);
int setting_network_get_state_mobile_data();

void setting_network_set_state_data_roaming(ButtonState state);
int setting_network_get_state_data_roaming(int *value);
int get_sim_ix_from_tapi_handle(TapiHandle *handle, SettingNetworkUG *ad);

#endif				/* __SETTING_NETWORK_H__ */
