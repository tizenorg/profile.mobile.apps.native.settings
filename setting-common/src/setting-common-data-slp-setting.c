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
#include <setting-common-data-slp-setting.h>
#include <Elementary.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib.h>

#include <glib.h>
#include <json-glib/json-glib.h>

#define SETTING_DATA_DIR_PATH _TZ_SYS_RW_APP"/org.tizen.setting/data/"
#define SETTING_CFG_JSON_FILE_PATH	SETTING_DATA_DIR_PATH"exported.json"
#define EXPORT_FILE			SETTING_DATA_DIR_PATH"setting_export.xml"


typedef enum {
	eBOOL,
	eINT,
	eSTRING,
} vconftype;

typedef union {
	int i;
	bool b;
	char *c;
} vconfval;


typedef struct VconfNode VconfNode;
struct VconfNode {
	const char *public_key;					/** key for importing & exporting */
	const char *public_groupkey;			/** category for importing & exporting */
	/*------------------------------------------------------------------------------------- */
	int key;								/** vconf id definded in Setting app */
	const char *vconf_key;					/** vconf string defined in libslp-setting */
	vconftype type;							/** BOOL, INT, STRING */
	vconfval value;							/** variable by union */
	void (*export)(VconfNode *pnode, void *data);		/** method for exporting current state */
	void (*import)(VconfNode *pnode, void *data);		/** method for importing state from cloud */
};

int get_vconf(VconfNode node, VconfNode *result);
int set_vconf(VconfNode node, VconfNode *result);

static VconfNode *g_sortedarr;

/* export function for each vconf key */
EXPORT_PUBLIC
void export_default(VconfNode *node, void *root)
{

	xmlNodePtr *root_node = (xmlNodePtr *)root;

	if (node) {
		char *val = NULL;
		char *type = NULL;

		char arr[1024];
		if (node->vconf_key == NULL) return; /* NO DOTHING IF null */
		VconfNode result;
		switch (node->type) {
		case eBOOL:
			SETTING_TRACE("EXPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value.b);
			/* call vconf_get */

			get_vconf(*node, &result);

			val = "boolval";
			type = "bool";
			if (result.value.b)
				val = "true";
			else
				val = "false";

			break;
		case eINT:
			SETTING_TRACE("EXPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value.i);
			/* call vconf_get */
			get_vconf(*node, &result);
			type = "int";

			SETTING_TRACE(">>(%d)<<", result.value.i);
			snprintf(arr,1024, "%d", result.value.i);
			/*node->value.i */
			val = arr;
			break;
		case eSTRING:
			SETTING_TRACE("EXPORTING key : %s : %d : %s", node->vconf_key, node->type, node->value.c);
			/* call vconf_get */
			get_vconf(*node, &result);
			val = result.value.c;
			type = "string";
			break;
		}

		xmlNodePtr xmlnode = xmlNewChild(*root_node, NULL, BAD_CAST "config", BAD_CAST val);
		xmlNewProp(xmlnode, BAD_CAST "key", BAD_CAST node->vconf_key);
		xmlNewProp(xmlnode, BAD_CAST "type", BAD_CAST type);
		xmlAddChild(*root_node, xmlnode);
	}
}

/*----------------------------------------------------------------------------------------------- */

/* export function for each vconf key */
EXPORT_PUBLIC
void export_json(VconfNode *node, void *root)
{
	JsonArray *array = (JsonArray *)root;

	if (node) {
		char *val = NULL;
		/*char *type = NULL; */

		char arr[1024];
		if (node->vconf_key == NULL) return; /* NO DOTHING IF null */
		VconfNode result;
		switch (node->type) {
		case eBOOL:
			SETTING_TRACE("EXPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value.b);
			/* call vconf_get */

			get_vconf(*node, &result);

			val = "boolval";
			/*type = "bool"; */
			if (result.value.b)
				val = "true";
			else
				val = "false";

			break;
		case eINT:
			SETTING_TRACE("EXPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value.i);
			/* call vconf_get */
			get_vconf(*node, &result);
			/*type = "int"; */

			SETTING_TRACE(">>(%d)<<", result.value.i);
			snprintf(arr,1024, "%d", result.value.i);
			/*node->value.i */
			val = arr;
			break;
		case eSTRING:
			SETTING_TRACE("EXPORTING key : %s : %d : %s", node->vconf_key, node->type, node->value.c);
			/* call vconf_get */
			get_vconf(*node, &result);
			val = result.value.c;
			/*type = "string"; */
			break;
		}

		JsonNode *item = json_node_new(JSON_NODE_OBJECT);
		JsonObject *obj = json_object_new();

		json_node_take_object(item, obj);
		json_object_set_string_member(obj, "key", node->public_key);
		json_object_set_string_member(obj, "value", val);
		json_array_add_element(array, item);
	}
}

/* _TZ_SYS_DATA/setting/aaa.xml */
/* import function for each vconf key */
EXPORT_PUBLIC
void import_default(VconfNode *node, void *data)
{
	/*xmlNodePtr* root_node =(xmlNodePtr*)data; */

	switch (node->type) {
	case eBOOL:
		SETTING_TRACE("IMPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value);
		break;
	case eINT:
		SETTING_TRACE("IMPORTING key : %s : %d : %d", node->vconf_key, node->type, node->value);
		break;
	case eSTRING:
		SETTING_TRACE("IMPORTING key : %s : %d : %s", node->vconf_key, node->type, node->value);
		break;
	}
}

/** bool vconf table */
static VconfNode g_btable[] = {

	{"power_on_lock", "security", BOOL_SLP_SETTING_POWER_ON_LOCK, VCONFKEY_SETAPPL_STATE_POWER_ON_LOCK_BOOL, eBOOL, {0}, export_json, import_default}, /* node[0] */
	{"simple_password", "security", BOOL_SLP_SETTING_SIMPLE_PASSWORD, VCONFKEY_SETAPPL_STATE_SIMPLE_PASSWORD_BOOL, eBOOL, {0}, export_json, import_default}, /* node[0] */

	{"automatic_time", "datetime", BOOL_SLP_SETTING_AUTOMATIC_TIME_UPDATE, VCONFKEY_SETAPPL_STATE_AUTOMATIC_TIME_UPDATE_BOOL, eBOOL, {0}, export_json, import_default},

	{"filight_mode", "connectivity", BOOL_SLP_SETTING_FLIGHT_MODE, VCONFKEY_TELEPHONY_FLIGHT_MODE, eBOOL, {0}, export_json, import_default},
	{"net_restriction_mode", "connectivity", BOOL_SLP_SETTING_NET_RESTRICTION_MODE, VCONFKEY_SETAPPL_NETWORK_RESTRICT_MODE, eBOOL, {0}, export_json, import_default},
	{"data_roaming", "connectivity", BOOL_SLP_SETTING_DATA_ROAMING, VCONFKEY_SETAPPL_STATE_DATA_ROAMING_BOOL, eBOOL, {0}, export_json, import_default},
	{"nfc_mode", "connectivity", BOOL_SLP_SETTING_NFC_STATUS, VCONFKEY_NFC_STATE, eBOOL, {0}, export_json, import_default},

	{"use_packetdata", "connectivity", BOOL_SLP_SETTING_USE_PACKET_DATA, VCONFKEY_3G_ENABLE, eBOOL, {0}, export_json, import_default},
	{"high_contract", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_HIGH_CONTRAST, VCONFKEY_SETAPPL_ACCESSIBILITY_HIGH_CONTRAST, eBOOL, {0}, export_json, import_default},
	{"screen_zoom", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_SCREEN_ZOOM, VCONFKEY_SETAPPL_ACCESSIBILITY_SCREEN_ZOOM, eBOOL, {0}, export_json, import_default},
	{"assistive_light", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_ASSISTIVE_LIGHT, VCONFKEY_SETAPPL_ACCESSIBILITY_TORCH_LIGHT, eBOOL, {0}, export_json, import_default},
	{"mono_audio", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_MONO_AUDIO, VCONFKEY_SETAPPL_ACCESSIBILITY_MONO_AUDIO, eBOOL, {0}, export_json, import_default},
	{"turn_off_all_sounds", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_TURN_OFF_ALL_SOUNDS, VCONFKEY_SETAPPL_ACCESSIBILITY_TURN_OFF_ALL_SOUNDS, eBOOL, {0}, export_json, import_default},
	{"led_notify", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_LED_NOTIFY, VCONFKEY_SETAPPL_ACCESSIBILITY_LED_NOTIFY, eBOOL, {0}, export_json, import_default},
	{"accept_call", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_ACCEPT_CALL, VCONFKEY_CISSAPPL_ANSWERING_KEY_BOOL, eBOOL, {0}, export_json, import_default},
	{"powerkey_end_calls", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_POWERKEY_END_CALLS, VCONFKEY_CISSAPPL_POWER_KEY_ENDS_CALL_BOOL, eBOOL, {0}, export_json, import_default},
	{"easy_touch_mode", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_EASY_TOUCH_MODE, VCONFKEY_SETAPPL_EASY_TOUCH_MODE_BOOL, eBOOL, {0}, export_json, import_default},
	{"rapid_key_input", "accessibility", BOOL_SLP_SETTING_ACCESSIBILITY_RAPID_KEY_INPUT, VCONFKEY_SETAPPL_ACCESSIBILITY_RAPID_KEY_INPUT, eBOOL, {0}, export_json, import_default},

	/*datausage */
	{"data_usage_limit_bool", "datausage", BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_BOOL, VCONFKEY_SETAPPL_SET_DATA_USAGE_LIMIT_BOOL, eBOOL, {0}, export_json, import_default},
	{"data_usage_limit_roaming_bool", "datausage", BOOL_SLP_SETTING_DATAUSAGE_SET_DATA_USAGE_LIMIT_ROAMING_BOOL, VCONFKEY_SETAPPL_SET_DATA_USAGE_LIMIT_ROAMING_BOOL, eBOOL, {0}, export_json, import_default},

	/* motion */
	{"motion_activation", "motion", BOOL_SLP_SETTING_MOTION_ACTIVATION, VCONFKEY_SETAPPL_MOTION_ACTIVATION, eBOOL, {0}, export_json, import_default},
	{"use_pick_up_call", "motion", BOOL_SLP_SETTING_MOTION_USE_PICK_UP_CALL, VCONFKEY_SETAPPL_USE_PICK_UP_CALL, eBOOL, {0}, export_json, import_default},
	{"use_pick_up", "motion", BOOL_SLP_SETTING_MOTION_USE_PICK_UP, VCONFKEY_SETAPPL_USE_PICK_UP, eBOOL, {0}, export_json, import_default},
	{"use_double_tap", "motion", BOOL_SLP_SETTING_MOTION_USE_DOUBLE_TAP, VCONFKEY_SETAPPL_USE_DOUBLE_TAP, eBOOL, {0}, export_json, import_default},
	{"use_tilt", "motion", BOOL_SLP_SETTING_MOTION_USE_TILT, VCONFKEY_SETAPPL_USE_TILT, eBOOL, {0}, export_json, import_default},
	{"use_panning", "motion", BOOL_SLP_SETTING_MOTION_USE_PANNING, VCONFKEY_SETAPPL_USE_PANNING, eBOOL, {0}, export_json, import_default},
	{"use_panning_browser", "motion", BOOL_SLP_SETTING_MOTION_USE_PANNING_BROWSER, VCONFKEY_SETAPPL_USE_PANNING_BROWSER, eBOOL, {0}, export_json, import_default},
	{"use_shake", "motion", BOOL_SLP_SETTING_MOTION_USE_SHAKE, VCONFKEY_SETAPPL_USE_SHAKE, eBOOL, {0}, export_json, import_default},
	{"use_turn_over", "motion", BOOL_SLP_SETTING_MOTION_USE_TURN_OVER, VCONFKEY_SETAPPL_USE_TURN_OVER, eBOOL, {0}, export_json, import_default},
	{"tab_twist", "motion", BOOL_SLP_SETTING_MOTION_TAP_TWIST, VCONFKEY_SETAPPL_TAP_TWIST, eBOOL, {0}, export_json, import_default},
	{"use_mute_pause", "motion", BOOL_SLP_SETTING_MOTION_USE_MUTE_PAUSE, VCONFKEY_SETAPPL_USE_MUTE_PAUSE, eBOOL, {0}, export_json, import_default},
	/*---- */
	{NULL, "groupname", SETTING_BOOL_SLP_LIST_MAX, NULL, eBOOL, {0}, export_json, import_default},
};


/** int vconf table */
static VconfNode g_itable[] = {
	{"lcd_brightness", "display", INT_SLP_SETTING_LCD_BRIGHTNESS, VCONFKEY_SETAPPL_LCD_BRIGHTNESS, eINT, {0}, export_json, import_default},
	{"lcd_timeout_normal", "display", INT_SLP_SETTING_LCD_TIMEOUT_NORMAL, VCONFKEY_SETAPPL_LCD_TIMEOUT_NORMAL, eINT, {0}, export_json, import_default},
	{"lcd_timeout_normal_backup", "display", INT_SLP_SETTING_LCD_TIMEOUT_NORMAL_BACKUP, VCONFKEY_LCD_TIMEOUT_NORMAL_BACKUP, eINT, {0}, export_json, import_default},

	{"automatic_brightness", "display", INT_SLP_SETTING_AUTOMATIC_BRIGHTNESS, VCONFKEY_SETAPPL_BRIGHTNESS_AUTOMATIC_INT, eINT, {0}, export_json, import_default},

	{"select_network", "network", INT_SLP_SETTING_SELECT_NETWORK, VCONFKEY_SETAPPL_SELECT_NETWORK_INT, eINT, {0}, export_json, import_default},
	{"network_mode", "network", INT_SLP_SETTING_NETWORK_MODE, VCONFKEY_SETAPPL_NETWORK_MODE_INT, eINT, {0}, export_json, import_default},
	{"regionformat_time1224", "datetime", INT_SLP_SETTING_REGIONFORMAT_TIME1224, VCONFKEY_REGIONFORMAT_TIME1224, eINT, {0}, export_json, import_default},
	{"dateformat", "datetime", INT_SLP_SETTING_DATE_FORMAT, VCONFKEY_SETAPPL_DATE_FORMAT_INT, eINT, {0}, export_json, import_default},
	{"weekformat", "datetime", INT_SLP_SETTING_WEEK_FORMAT, VCONFKEY_SETAPPL_WEEKOFDAY_FORMAT_INT, eINT, {0}, export_json, import_default},

	/* CALL_ALERT_VIB */
	{"call_alert_vib", "sound", INT_SLP_SETTING_CALL_ALERT_VIB, VCONFKEY_SETAPPL_CALL_VIBRATION_PATTERN_INT, eINT, {0}, export_json, import_default},

	/*****/
	{"msg_alert_repeat", "sound", INT_SLP_SETTING_MSG_ALERT_REPEAT, VCONFKEY_SETAPPL_NOTI_MSG_ALERT_REP_TYPE_INT, eINT, {0}, export_json, import_default},

	/*****/
	{"language_index", "display", INT_SLP_SETTING_LANG, VCONFKEY_SETAPPL_LANG_INT, eINT, {0}, export_json, import_default},
	{"bluetooth_switch", "connectivity", INT_SLP_SETTING_BT_STATUS, VCONFKEY_BT_STATUS, eINT, {0}, export_json, import_default},
	{"wifi_state", "connectivity", INT_SLP_SETTING_WIFI_STATUS, VCONFKEY_WIFI_STATE, eINT, {0}, export_json, import_default},
	{"mobile_hotspot_mode", "connectivity", INT_SLP_SETTING_MOBILE_AP_STATUS, VCONFKEY_MOBILE_HOTSPOT_MODE, eINT, {0}, export_json, import_default},
	{"roaming_network", "connectivity", INT_SLP_SETTING_ROAMING_NETWORK, VCONFKEY_SETAPPL_ROAMING_NETWORK_INT, eINT, {0}, export_json, import_default},
	{"usb_mode", "connectivity", INT_SLP_SETTING_USB_MODE, VCONFKEY_SETAPPL_USB_MODE_INT, eINT, {0}, export_json, import_default},

	/* memory */
	{"mem_bluetooth", "memory", INT_SLP_SETTING_DEFAULT_MEM_BLUETOOTH, VCONFKEY_SETAPPL_DEFAULT_MEM_BLUETOOTH_INT, eINT, {0}, export_json, import_default},
	{"mem_wifidirect", "memory", INT_SLP_SETTING_DEFAULT_MEM_WIFIDIRECT, VCONFKEY_SETAPPL_DEFAULT_MEM_WIFI_DIRECT_INT, eINT, {0}, export_json, import_default},
	{"mem_installapplications", "memory", INT_SLP_SETTING_DEFAULT_MEM_INSTALLAPPLICATIONS, VCONFKEY_SETAPPL_DEFAULT_MEM_INSTALL_APPLICATIONS_INT, eINT, {0}, export_json, import_default},

	/* other module */
	{"sim_slot", "security", INT_SLP_SETTING_SIM_SLOT, VCONFKEY_TELEPHONY_SIM_SLOT, eINT, {0}, export_json, import_default},
	{"phone_lock_attempt", "security", INT_SLP_SETTING_PHONE_LOCK_ATTEMPTS_LEFT, VCONFKEY_SETAPPL_PHONE_LOCK_ATTEMPTS_LEFT_INT, eINT, {0}, export_json, import_default},
	{"sime_lock_attempt", "security", INT_SLP_SETTING_SIM_LOCK_ATTEMPTS_LEFT, VCONFKEY_SETAPPL_SIM_LOCK_ATTEMPTS_LEFT_INT, eINT, {0}, export_json, import_default},

	/* accessibility setting */
	{"font_size", "accessibility", INT_SLP_SETTING_ACCESSIBILITY_FONT_SIZE, VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_SIZE, eINT, {0}, export_json, import_default},
#if SUPPORT_FONT_STYLE
	{"font_style", "accessibility", INT_SLP_SETTING_ACCESSIBILITY_FONT_STYLE, VCONFKEY_SETAPPL_ACCESSIBILITY_FONT_STYLE, eINT, {0}, export_json, import_default},
#endif
	{"sound_balance", "accessibility", INT_SLP_SETTING_ACCESSIBILITY_SOUND_BALANCE, VCONFKEY_SETAPPL_SOUND_BALANCE_INT, eINT, {0}, export_json, import_default},

	/*powersaving */
	{"bgprocess", "developeroptions", INT_SLP_SETTING_DEVOPTION_BGPROCESS, VCONFKEY_SETAPPL_DEVOPTION_BGPROCESS, eINT, {0}, export_json, import_default},

	/* data usage */
	{"usage_cycle", "datausage", INT_SLP_SETTING_DATAUSAGE_DATA_USAGE_CYCLE_INT, VCONFKEY_SETAPPL_DATA_USAGE_CYCLE_INT, eINT, {0}, export_json, import_default},
	{"each_month", "datausage", INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_INT, VCONFKEY_SETAPPL_DATA_EACH_MONTH_INT, eINT, {0}, export_json, import_default},
	{"each_month_app", "datausage", INT_SLP_SETTING_DATAUSAGE_DATA_EACH_MONTH_APP_INT, VCONFKEY_SETAPPL_DATA_EACH_MONTH_APP_INT, eINT, {0}, export_json, import_default},

	/* accessories */
	{"display/touchkey_light_duration", "display", INT_SLP_SETTING_TOUCHKEY_LIGHT_DURATION_INT, VCONFKEY_SETAPPL_TOUCHKEY_LIGHT_DURATION, eINT, {0}, export_json, import_default},
	{"display/screen_capture_destination", "display", INT_SLP_SETTING_SCREEN_CAPTURE_DESTINATION_INT, VCONFKEY_SETAPPL_SCREEN_CAPTURE_DESTINATION, eINT, {0}, export_json, import_default},
	/*---- */
	{NULL, "groupkey", SETTING_INT_SLP_LIST_MAX, NULL, eINT, {0}, export_json, import_default},

};


/** string vconf table */
static VconfNode g_stable[] = {
	/* password code - special processing */
	{"phone_password", "security",	STR_SLP_LIST_PHONE_PASSWORD, NULL, eSTRING, {0}, export_json, import_default },
	{"phonelock_timestamp", "security",	STR_SLP_SETTING_PHONE_LOCK_TIMESTAMP, VCONFKEY_SETAPPL_PHONE_LOCK_TIMESTAMP_STR, eSTRING, {0}, export_json, import_default },
	{"simlock_timestamp", "security",	STR_SLP_SETTING_SIM_LOCK_TIMESTAMP, VCONFKEY_SETAPPL_SIM_LOCK_TIMESTAMP_STR, eSTRING, {0}, export_json, import_default },
	{"selected_phonenum", "about",	STR_SLP_SETTING_SELECT_NUM, VCONFKEY_SETAPPL_SELECTED_NUM, eSTRING, {0}, export_json, import_default },
	/*---- */
	{NULL, "groupkey",	STR_SLP_LIST_MAX, NULL, eSTRING, {0}, export_json, import_default},
};


/**
 * import
 */
int sync_adapter_restore_set_data(void *data, unsigned long long size)
{

	/*if(size <=  0) */
	/*	SETTING_TRACE("error"); */

	/*SETTING_TRACE("%s: data is %s", __func__, (char *)data); */
	/*SETTING_TRACE("%s: size is %lld", __func__, size); */
	return 0;
}



static int getconfsize()
{
	/*---- */
	int bcount = sizeof(g_btable) / sizeof(g_btable[0]) - 1;
	int icount = sizeof(g_itable) / sizeof(g_itable[0]) - 1;
	int scount = sizeof(g_stable) / sizeof(g_stable[0]) - 1; /* why?? buggy */
	int total = bcount + icount + scount;

	return total;
}

/**
 * leaf node
 *
 * g_sortedarr from caller
 */
void __foreach_attr(JsonObject *object, const gchar *public_key, JsonNode *member_node, gpointer user_data)
{
	char *public_groupname = (char *)user_data;
	char *retstr = (char *)json_object_get_string_member(object, public_key);

	/*SETTING_TRACE(" public_groupname : %s", public_groupname); */

	/* LOOKUP(group, key) ----> vconf_key */
	int total = getconfsize();

	int i;
	static int ncount = 0;
	int run = 1;
	for (i = 0; i < total && run == 1; i++) {
		/*SETTING_TRACE(" >> g_sortedarr[%d].public_groupkey : %s",i, g_sortedarr[i].public_groupkey ); */
		/*SETTING_TRACE(" >> g_sortedarr[%d].public_key : %s",i, g_sortedarr[i].public_key ); */
		/*SETTING_TRACE(" >> public_key : %s",public_key ); */

		if (g_sortedarr[i].public_groupkey &&
			safeStrCmp(g_sortedarr[i].public_groupkey, public_groupname) == 0 &&
			g_sortedarr[i].public_key &&
			safeStrCmp(g_sortedarr[i].public_key, public_key) == 0) {
			ncount += 1;
			SETTING_TRACE(" FOUND ---> group name %s : public key %s --- vconf key (%s) - type (%d) -- strval : [%s] ", g_sortedarr[i].public_groupkey, g_sortedarr[i].public_key, g_sortedarr[i].vconf_key, g_sortedarr[i].type, retstr);

			/* set value from vconf (key/value) */
			/* retstr */
			VconfNode result;
			switch (g_sortedarr[i].type) {
			case eBOOL:
				/* "true"  --> 1  "false" --> 0 */
				if (retstr && safeStrCmp(retstr, "true") == 0) {
					g_sortedarr[i].value.b = 1;
				}	else if (retstr && safeStrCmp(retstr, "false") == 0) {
					g_sortedarr[i].value.b = 0;
				}
				break;

			case eINT: {
					int num = -1;
					/* "111" --> 1111 */
					if (retstr)
						num = atoi(retstr);
					g_sortedarr[i].value.i = num;
				}
				break;

			case eSTRING:
				/* "hello world"  --> "hello world" */
				if (retstr)
					g_sortedarr[i].value.c = retstr;
				else
					g_sortedarr[i].value.c = "";	/* error handler */
				break;

			}

			if (g_sortedarr[i].vconf_key)
				set_vconf(g_sortedarr[i], &result);
			else {
				SETTING_TRACE("vconf key is NULL");
			}

			run = 0; /* stop loop */
		}
	}
	SETTING_TRACE(" n - FOUND %d ", ncount);
}

void __func_cb(JsonObject *object, const gchar *member_name, JsonNode *member_node, gpointer user_data)
{
	/*JsonArray *array = json_node_get_array(member_node); */
	/*SETTING_TRACE(" ----> member name : %s ", member_name); */
	JsonObject *child = json_object_get_object_member(object, member_name);
	json_object_foreach_member(child, __foreach_attr, (gpointer)member_name);
}

int __compareByCategory(const void *arg1, const void *arg2)
{
	char *str1 = (char *)(((VconfNode *)arg1)->public_groupkey);
	char *str2 = (char *)(((VconfNode *)arg2)->public_groupkey);

	if (!str1 || !str2) return -1;

	return safeStrCmp(str1, str2);
}

EXPORT_PUBLIC
void setting_import_json(status_handler_fp fp, void *data)
{
	SETTING_TRACE_BEGIN;
	/*--------------------------------------------------------------------------------- */
	JsonParser *parser;
	JsonNode *root;
	GError *error;

	/*---- */
	int bcount = sizeof(g_btable) / sizeof(g_btable[0]) - 1;
	int icount = sizeof(g_itable) / sizeof(g_itable[0]) - 1;
	int scount = sizeof(g_stable) / sizeof(g_stable[0]) - 1; /* why?? buggy */
	int total = bcount + icount + scount;
	SETTING_TRACE(" >>> total : %d ", total);

	VconfNode *arr3 = (VconfNode *)malloc(sizeof(VconfNode)*total);
	if (!arr3) return;
	memcpy(arr3, g_btable, sizeof(VconfNode)*(bcount));
	memcpy(&arr3[bcount - 1], g_itable, sizeof(g_itable));
	memcpy(&arr3[bcount - 1 + icount - 1], g_stable, sizeof(g_stable));
	qsort(arr3, total, sizeof(VconfNode), __compareByCategory);
	/*---- */

	g_sortedarr = arr3;

	parser = json_parser_new();

	error = NULL;
	json_parser_load_from_file(parser, SETTING_CFG_JSON_FILE_PATH, &error);
	if (error) {
		SETTING_TRACE("Unable to parse `%s': %s", SETTING_CFG_JSON_FILE_PATH, error->message);
		g_error_free(error);
		g_object_unref(parser);
		return;
	}

	root = json_parser_get_root(parser);
	/* manipulate the object tree and then exit */

	JsonObject *obj1 = json_node_get_object(root);
	if (json_object_has_member(obj1, "value")) {
		JsonNode *node = json_object_get_member(obj1, "value");
		JsonObject *object2 = json_node_get_object(node);
		json_object_foreach_member(object2, __func_cb, NULL);	/* with g_sortedarr */
	}
	g_object_unref(parser);

	FREE(arr3);
	g_sortedarr = NULL;
}

/**
 * setting --> cloud : JSON
 *
 * int status_handler(int total, int current,  void* data);
 */
EXPORT_PUBLIC
char *setting_export_json(status_handler_fp fp, void *data)
{
	JsonNode *root = NULL;

	root = json_node_new(JSON_NODE_OBJECT);
	JsonObject *top = json_object_new();
	json_node_take_object(root, top);

	json_object_set_string_member(top, "key", "SETTINGS_359617040746834_8592d887-8b97-406e-9cf9-03aebc045f81");

	int bcount = sizeof(g_btable) / sizeof(g_btable[0]) - 1;
	int icount = sizeof(g_itable) / sizeof(g_itable[0]) - 1;
	int scount = sizeof(g_stable) / sizeof(g_stable[0]) - 1; /* why?? buggy */

	int total = bcount + icount + scount;

	JsonNode *topnode = json_node_new(JSON_NODE_OBJECT);
	JsonObject *topobj = json_object_new();
	json_node_take_object(topnode, topobj);

	json_object_set_member(top, "value", topnode);

	VconfNode *arr3 = (VconfNode *)malloc(sizeof(VconfNode)*total);
	if (!arr3) return NULL;
	memcpy(arr3, g_btable, sizeof(VconfNode)*(bcount));
	memcpy(&arr3[bcount - 1], g_itable, sizeof(g_itable));
	memcpy(&arr3[bcount - 1 + icount - 1], g_stable, sizeof(g_stable));
	qsort(arr3, total, sizeof(VconfNode), __compareByCategory);

	char *public_groupkey = "";

	/*JsonArray* array = NULL; */
	JsonObject *obj = NULL;
	JsonNode *node = NULL;

	int i;
	for (i = 0; i < total; i++) {
		if (arr3[i].public_key) {
			if (public_groupkey && safeStrCmp(public_groupkey, arr3[i].public_groupkey) != 0) {
				public_groupkey = (char *)(arr3[i].public_groupkey);
				/*array = json_array_new(); */
				obj = json_object_new();
				node = json_node_new(JSON_NODE_OBJECT);
				json_node_take_object(node, obj);
				json_object_set_member(topobj, public_groupkey, node);
				/*json_array_add_element(array, node); */
			}

			/* get data from vconf */
			/* get vconf_get */
			char *val = NULL;
			char arr[1024];
			VconfNode result;
			switch (arr3[i].type) {
			case eBOOL:
				get_vconf(arr3[i], &result);
				if (result.value.b)
					val = "true";
				else
					val = "false";
				break;
			case eINT:
				get_vconf(arr3[i], &result);
				snprintf(arr, 1024, "%d", result.value.i);
				val = arr;
				/*if (val == NULL) val = "-100"; */
				break;
			case eSTRING:
				get_vconf(arr3[i], &result);
				val = result.value.c;
				if (val == NULL) val = "";
				break;
			default:
				val = "error";
			}

			json_object_set_string_member(obj, arr3[i].public_key, val);
		}
	}

	/* save data to a file */
	GError *error = NULL;
	JsonGenerator *generator = json_generator_new();
	json_generator_set_root(generator, root/*node*/);

	gsize len;
	char *buf = (char *)json_generator_to_data(generator, &len);

	g_object_set(generator, "pretty", TRUE, NULL);	 /*write file in indent format */
	gboolean ret = json_generator_to_file(generator, SETTING_CFG_JSON_FILE_PATH, &error);
	g_object_unref(generator);

	if (FALSE == ret) {
		SETTING_TRACE_ERROR("Error writing file %s!", SETTING_CFG_JSON_FILE_PATH);
		/*return FALSE; */
	}

	json_node_free(root);
	FREE(arr3);

	return buf;
}

int set_vconf(VconfNode node, VconfNode *result)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = -1 ;
	/*node.key */
	/*node.vconf_key */
	*result = node;
	switch (node.type) {
	case eBOOL:
		/*SETTING_TRACE("begin case eBOOL[%s=\"%d\"]", node.vconf_key, node.value.b); */
		ret = vconf_set_bool(node.vconf_key, node.value.b);
		result->type = eBOOL;
		result->value.b = node.value.b;
		break;
	case eINT:
		/*SETTING_TRACE("begin case eINT[%s=\"%d\"]", node.vconf_key, node.value.i); */
		ret = vconf_set_int(node.vconf_key, node.value.i);
		result->type = eINT;
		result->value.i = node.value.i;
		break;
	case eSTRING:
		if (node.key == STR_SLP_LIST_PHONE_PASSWORD) {
			ret = setting_store_init_password(node.value.c);
		} else {
			/*SETTING_TRACE("begin case eSTRING[%s=\"%s\"]", node.vconf_key, node.value.c); */
			ret = vconf_set_str(node.vconf_key, node.value.c);
		}
		result->type = eSTRING;
		result->value.c = node.value.c;
		break;
	default:
		SETTING_TRACE_ERROR(">>>>>>>>>> node.type is NOT DEFINED");
		break;
	}

	if (0 != ret) {
		SETTING_TRACE_ERROR(">>>>>>>>>> Failed to set vconf[%s]", node.vconf_key);
	}
	/*SETTING_TRACE("KEY:%d VCONF_KEY=%s TYPE=%d VALUE=%d ", result->key, result->vconf_key, result->type, result->value.i); */
	return ret;
}


int get_vconf(VconfNode node, VconfNode *result)
{
	int ret = -1 ;

	char md_result[SHA256_DIGEST_LENGTH];
	memset(md_result, 0x0, SHA256_DIGEST_LENGTH);


	*result = node;

	switch (node.type) {
	case eBOOL:
		ret = vconf_get_bool(node.vconf_key, (int *)(&(node.value.b)));
		result->type = eBOOL;
		result->value.b = node.value.b;
		break;
	case eINT:
		ret = vconf_get_int(node.vconf_key, &(node.value.i));
		result->type = eINT;
		result->value.i = node.value.i;
		break;
	case eSTRING:
		if (node.key == STR_SLP_LIST_PHONE_PASSWORD) {
			node.value.c = (char *)malloc(sizeof(char)*SHA256_DIGEST_LENGTH);
			if (node.value.c) {
				memset(node.value.c, 0x0, SHA256_DIGEST_LENGTH);
			} else {
				SETTING_TRACE_ERROR(" malloc filed : eSTRING, node.value.c ");
				ret = SETTING_RETURN_FAIL;
				goto endtag;
			}

			if (setting_read_password(md_result) == 0) {
				safeCopyStr(node.value.c, md_result, SHA256_DIGEST_LENGTH); /*	un-safe */

				result->type = eSTRING;
				result->value.c = node.value.c;
				ret = SETTING_RETURN_SUCCESS;
			} else {
				/*do nothing */
				ret = SETTING_RETURN_FAIL;
				goto endtag;
			}
		} else {
			node.value.c = vconf_get_str(node.vconf_key);
			SETTING_TRACE("string type ---> %s ", node.value.c);
			result->type = eSTRING;
			result->value.c = node.value.c;
			ret = SETTING_RETURN_SUCCESS;
		}
		break;
	default:
		SETTING_TRACE_ERROR(">>>>>>>>>>>>>>>>>>>>>>> get vconf ERROR : %s ", node.vconf_key);
	}
endtag:
	return ret;
}

EXPORT_PUBLIC
int setting_set_bool_slp_key(setting_bool_slp_list key, int value, int *err)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= SETTING_BOOL_SLP_LIST_MAX) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		return ret;
	}

	VconfNode result;
	g_btable[key].value.b = value;
	ret = set_vconf(g_btable[key], &result);

	SETTING_TRACE("setting_set_bool ::: KEY:%d VCONF_KEY=%s TYPE=%d VALUE=%d ", result.key, result.vconf_key, result.type, result.value.b);
	return ret;
}

EXPORT_PUBLIC
int setting_get_bool_slp_key(setting_bool_slp_list key, int *value, int *err)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= SETTING_BOOL_SLP_LIST_MAX) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		SETTING_TRACE_ERROR(" <<<<<<<< key is invalid >>>>>>>>>>>>> ");
		return ret;
	}
	VconfNode result;
	ret = get_vconf(g_btable[key], &result);
	/**value = g_btable[key].value.b; */
	*value = result.value.b;
	return ret;
}

EXPORT_PUBLIC
int setting_set_int_slp_key(setting_int_slp_list key, int value, int *err)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= SETTING_INT_SLP_LIST_MAX) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		SETTING_TRACE(">>> key is invalid ");
		return ret;
	}

	VconfNode result;
	g_itable[key].value.i = value;
	ret = set_vconf(g_itable[key], &result);
	*err = ret;
	SETTING_TRACE_DEBUG("setting_set_int ::: KEY:%d VCONF_KEY=%s TYPE=%d VALUE=%d ", result.key, result.vconf_key, result.type, result.value.i);
	return ret;
}

/* return -1: fail	cannot use err to check the result, return value instead*/
EXPORT_PUBLIC
int setting_get_int_slp_key(setting_int_slp_list key, int *value, int *err)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= SETTING_INT_SLP_LIST_MAX) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		return ret;
	}

	VconfNode result;
	ret = get_vconf(g_itable[key], &result);
	/**value = g_itable[key].value.i; */
	*value = result.value.i;

	*err = ret;
	return ret;
}

EXPORT_PUBLIC
int setting_get_string_slp_key(setting_str_slp_list key, char *value, int *err)
{
	/*SETTING_TRACE_BEGIN; */
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= STR_SLP_LIST_MAX || NULL == value) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		return ret;
	}

	/** @todo check memory leack when passwork checking */
	VconfNode result;
	ret = get_vconf(g_stable[key], &result);

	if (ret >= 0) {
		safeCopyStr(value, result.value.c, strlen(result.value.c));
		SETTING_TRACE(">>>>>>>> %s <<<<<<<<", value);
		FREE(result.value.c);
	} else {
		SETTING_TRACE("get_vconf is failed");
	}
	*err = ret;
	return ret;
}

EXPORT_PUBLIC
int setting_set_string_slp_key(setting_str_slp_list key, char *value, int *err)
{
	int ret = SETTING_RETURN_FAIL;

	/* error check */
	if (key < 0 || key >= STR_SLP_LIST_MAX) {
		*err = SETTING_GENERAL_ERR_WRONG_PARAMETER;
		return ret;
	}

	VconfNode result;
	g_stable[key].value.c = value;
	ret = set_vconf(g_stable[key], &result);
	g_stable[key].value.c = "";

	SETTING_TRACE("setting_set_str ::: KEY:%d VCONF_KEY=%s TYPE=%d VALUE=%d ", result.key, result.vconf_key, result.type, result.value.c);
	*err = ret;
	return ret;
}

/** @todo don't use i18n string directly. */
static const setting_lang_entry lang_table[] = {
	{	"auto",			"Automatic"},
	{	"ko_KR.UTF-8",	"한국어"},
	{	"en_US.UTF-8",	"English"},
	{	"zh_CN.UTF-8",	"简体中文"},
	{	"zh_HK.UTF-8",	"繁體中文(香港)"},
	{	"zh_TW.UTF-8",	"繁體中文(台灣)"},
	{	"de_DE.UTF-8",	"Deutsch"},
	{	"nl_NL.UTF-8",	"Nederlands"},
	{	"es_ES.UTF-8",	"Español"},
	{	"pt_PT.UTF-8",	"Português"},
	{	"el_GR.UTF-8",	"Eλληνικά"},
	{	"it_IT.UTF-8",	"Italiano"},
	{	"fr_FR.UTF-8",	"Français"},
	{	"tr_TR.UTF-8",	"Türkçe"},
	{	"ja_JP.UTF-8",	"にほんご"},
	{	"ru_RU.UTF-8",	"Россию"},
};

setting_lang_entry *setting_get_language_table()
{
	return (setting_lang_entry *)lang_table;
}


static void _parseLangListXML(char *docname);	/* internal */
static void __tree_walk_langlist(xmlNodePtr cur); /* internal */

static Eina_List *s_langlist;					/* internal */

/* do it once */
int _langlist_load(char *path)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("language list path: %s ", path);

	if (!s_langlist) {
		_parseLangListXML(path);
	}

	return 0;
}

/* singleton */
EXPORT_PUBLIC
Eina_List *setting_get_language_list()
{
	if (NULL == s_langlist) {
		_langlist_load(LANGLIST_FILE_PATH);
	}

	return s_langlist;
}

/* singleton */
EXPORT_PUBLIC
Eina_List *setting_get_language_list2(char *path)
{
	if (NULL == s_langlist) {
		_langlist_load(path);
	}

	return s_langlist;
}


int _langlist_destroy()
{
	SETTING_TRACE_BEGIN;
	Eina_List *li = s_langlist;
	setting_lang_entry *node;
	while (li) {
		node = (setting_lang_entry *) eina_list_data_get(li);
		if (node) {
			/*SETTING_TRACE("destroy nodes : locale : %s title : %s", node->locale, node->title); */
			G_FREE(node->locale);
			G_FREE(node->title);
			G_FREE(node->mcc);
			FREE(node);
		}
		li = eina_list_next(li);
	}
	s_langlist = eina_list_free(s_langlist);		/* returns NULL */

	return 0;
}

EXPORT_PUBLIC
void  setting_get_language_list_destroy()
{
	_langlist_destroy();
}

static void _parseLangListXML(char *docname)
{
	SETTING_TRACE_BEGIN;
	xmlDocPtr doc;
	xmlNodePtr cur;

	doc = xmlParseFile(docname);

	if (doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, "empty document\n");
		xmlFreeDoc(doc);
		return;
	}

	/*SETTING_TRACE("ROOT NODE : %s ", cur->name); */
	/* make sure root node is "settings" */
	if (xmlStrcmp(cur->name, (const xmlChar *) "langlist")) {
		SETTING_TRACE("document of the wrong type, root node != settings");
		xmlFreeDoc(doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	__tree_walk_langlist(cur);

	/* save tree to file */
	if (doc != NULL) {
		/*xmlSaveFormatFile (docname, doc, 1); */
		xmlFreeDoc(doc);
	}
	/*xmlFreeDoc(doc); */
	return;
}

static void __tree_walk_langlist(xmlNodePtr cur)
{
	SETTING_TRACE_BEGIN;
	xmlNode *cur_node = NULL;
	char *id;
	char *string;
	char *mcc = NULL;
	/*int number = 1; */

	for (cur_node = cur; cur_node; cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			/*SETTING_TRACE(" name=%s title=%s \n", xmlGetProp(cur_node, (const xmlChar *)"id"), xmlGetProp(cur_node, (const xmlChar *)"string")); */
			id = (char *)g_strdup((char *)xmlGetProp(cur_node, (const xmlChar *)"id"));
			string = (char *)g_strdup((char *) xmlGetProp(cur_node, (const xmlChar *)"string"));
			/*SETTING_TRACE_DEBUG("lang: %s", xmlGetProp(cur_node, (const xmlChar *)"lang")); */
			mcc = (char *)g_strdup((char *) xmlGetProp(cur_node, (const xmlChar *)"mcc"));
			/*number = atoi((char*) xmlGetProp(cur_node, (const xmlChar *)"no")); */

			setting_lang_entry *pitem = (setting_lang_entry *)calloc(1, sizeof(setting_lang_entry));
			if (pitem) {
				pitem->locale = id;
				pitem->title = string;
				pitem->mcc = mcc;
				/*pitem->number = number++; */
				/*SETTING_TRACE_DEBUG("no=%d", pitem->number); */
				/*SETTING_TRACE_DEBUG(">>>> locale: %s title: %s mcc: %s", pitem->locale, pitem->title, pitem->mcc); */
				s_langlist = eina_list_append(s_langlist, pitem);
			}
		}
	}
}
/*----------------------------------------------------------------------------------------------- */
/*----------------------------------------------------------------------------------------------- */

/*setting_lang_entry* */

/*//////////////////////////////////////////////////////////////////////////////////////// */
int setting_store_init_password(char *in)
{

	SHA256_CTX context;
	char md[SHA256_DIGEST_LENGTH] = {0,};
	/* int i = 0; */
	int ret = SETTING_RETURN_FAIL;

	SHA256_Init(&context);
	SHA256_Update(&context, (char *)in, strlen(in));
	SHA256_Final((unsigned char *)md, &context);

#ifdef DEBUG
	SETTING_TRACE("input: [%s]\n", in);
	SETTING_TRACE("output: [");
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
		SETTING_TRACE("%02x", md[i]);
	SETTING_TRACE("]\n");
#endif

	/*	create a file or store a data */
	/*	store_password */
	FILE *fp = NULL;
	int ret_fw = 0;
	fp = fopen(PWD_FILE, "w+");
	if (fp) {
		ret_fw = fwrite(md, 1, SHA256_DIGEST_LENGTH, fp);
		SETTING_TRACE_DEBUG("fwrite() wrote %d chars.", ret_fw);
		if (ret_fw == SHA256_DIGEST_LENGTH) {
			fflush(fp);
			fsync(fp->_fileno);
			ret = SETTING_RETURN_SUCCESS;
		}
		fclose(fp);
	} else {
		SETTING_TRACE("ERR: pwd file is not exist \n");
	}

	return ret;
}

int setting_read_password(char *md_result)
{
	SETTING_TRACE_BEGIN;
	/* int i; */
	FILE *fp = NULL;
	fp = fopen(PWD_FILE, "r");
	if (fp) {
		int size = fread(md_result, sizeof(char), SHA256_DIGEST_LENGTH, fp);
		if (size != SHA256_DIGEST_LENGTH) {
			SETTING_TRACE_ERROR("fread failed");
		}
		fclose(fp);
	} else {
		SETTING_TRACE("ERR: pwd file is not exist \n");
		return SETTING_RETURN_FAIL;
	}

#ifdef DEBUG
	SETTING_TRACE("output from file: [");
	for (i = 0; i < SHA256_DIGEST_LENGTH; i++)
		SETTING_TRACE("%02x", md_result[i]);
	SETTING_TRACE("]\n");
#endif
	SETTING_TRACE_END;
	return 0;
}

int setting_compare_password(char *md_result, const char *in_later)
{
	SETTING_TRACE_BEGIN;
	SHA256_CTX context_later;
	char md_later[SHA256_DIGEST_LENGTH];
	int ret = SETTING_RETURN_FAIL;

	SHA256_Init(&context_later);
	SHA256_Update(&context_later, (char *)in_later, strlen(in_later));
	SHA256_Final((unsigned char *)md_later, &context_later);

	SETTING_TRACE(" :[ %25s ]", in_later);

	if (0 == memcmp(md_later, md_result, SHA256_DIGEST_LENGTH)) {
		SETTING_TRACE("two passwords are same \n");
		ret = 0;
	} else {
		SETTING_TRACE("two passwords are different \n");
	}
	return ret;
}

EXPORT_PUBLIC
void setting_destory_listen_list(Eina_List **listened_list)
{
	if (listened_list && *listened_list) {
		Vconf_Change_Node *node = NULL;
		Eina_List *li = *listened_list;
		while (li) {
			node = (Vconf_Change_Node *) eina_list_data_get(li);
			if (node) {
				/*SETTING_TRACE("Deregister callback[%p] of %s", node->cb, node->in_key); */
				if (node->in_key && node->cb) {
					(void)vconf_ignore_key_changed(node->in_key, node->cb);
					FREE(node);
				}
			}
			li = eina_list_next(li);
		}
		*listened_list = eina_list_free(*listened_list);
	}
}

EXPORT_PUBLIC
bool setting_add_listen_node(Eina_List **listened_list, const char *vconf, vconf_callback_fn cb, void *data)
{
	Vconf_Change_Node *node = calloc(1, sizeof(Vconf_Change_Node));
	bool ret = TRUE;
	if (node && vconf && cb) {
		node->in_key = vconf;
		node->cb = cb;
		node->cb_data = data;

		if (0 == vconf_notify_key_changed(vconf, cb, data)) {
			/*SETTING_TRACE("Register callback[%p] of %s", node->cb, node->in_key); */
			*listened_list = eina_list_append(*listened_list, node);
		} else {
			SETTING_TRACE_ERROR("Failed to register callback[%p] of %s", node->cb, node->in_key);
			FREE(node);
			ret = FALSE;
		}
	}
	return ret;
}

