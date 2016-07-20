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

#include <setting-cfg.h>
#include <setting-debug.h>
#include <stdio.h>
#include <Elementary.h>
#include <setting-common-general-func.h>
#include <setting-common-data-type.h>
#include <setting-common-data-slp-setting.h>
#include <unistd.h>
#include <vconf.h>

JsonParser *parser;
JsonNode *root; /* category_list */
static char *cfg_dir_path = NULL;
static char *cfg_file_path = NULL;

EXPORT_PUBLIC
char *setting_cfg_get_dir_path()
{
	if (!cfg_dir_path)
		cfg_dir_path = app_get_data_path();
	return cfg_dir_path;
}

EXPORT_PUBLIC
char *setting_cfg_get_path()
{
	if (!cfg_file_path) {
		char *path = setting_cfg_get_dir_path();
		retv_if(!path, NULL);
		char string[PATH_MAX];
		snprintf(string, PATH_MAX - 1, "%s%s", path, "setting.cfg");
		cfg_file_path = strdup(string);
	}
	return cfg_file_path;
}

int setting_cfg_file_write(JsonNode *node);

#define __create_an_item(item_name, icon_path, ug_args, shortcut_appid, \
		defaultPos, item_type,	is_resetable, ug_exist, uuid, \
		click_times, last_clicked) do {\
	if (ug_exist) { \
		if (!is_ug_installed_by_ug_args(ug_args)) break;\
	} \
	menu_item = json_node_new(JSON_NODE_OBJECT); \
	object = json_object_new(); \
	json_node_take_object(menu_item, object); \
	json_object_set_string_member(object, "key_name", item_name); \
	json_object_set_string_member(object, "icon_path", icon_path); \
	json_object_set_string_member(object, "ug_args", ug_args); \
	json_object_set_string_member(object, "shortcut_appid", \
			shortcut_appid); \
	json_object_set_int_member(object, "pos", defaultPos); \
	json_object_set_int_member(object, "click_times", click_times);\
	json_object_set_int_member(object, "item_type", item_type); \
	json_object_set_int_member(object, "is_resetable", \
			is_resetable); \
	json_object_set_int_member(object, "last_clicked", \
			last_clicked); \
	json_object_set_string_member(object, "uuid", uuid); \
	json_array_add_element(menu, menu_item); \
} while (0);\

#define __create_a_menu(menu_name) {\
	category = json_node_new(JSON_NODE_OBJECT);\
	object = json_object_new();\
	json_node_take_object(category, object);\
	json_object_set_string_member(object, "name", menu_name);\
	menu = json_array_new();\
	json_object_set_array_member(object, "menu", menu);\
	json_array_add_element(json_node_get_array(category_list), \
			category);\
}

int wifi_toggle_get_state(Cfg_Item_State *stat, void *data);
int flightmode_toggle_get_state(Cfg_Item_State *stat, void *data);
int bluetooth_toggle_get_state(Cfg_Item_State *stat, void *data);
int drivingmode_toggle_get_state(Cfg_Item_State *stat, void *data);
int network_restriction_mode_toggle_get_state(Cfg_Item_State *stat, void *data);
int personalmode_toggle_get_state(Cfg_Item_State *stat, void *data);
int handfree_toggle_get_state(Cfg_Item_State *stat, void *data);
int nfc_toggle_get_state(Cfg_Item_State *stat, void *data);

/*////////////////////////////////////////////////////////////////////////// */
/* wifi - DONE */
EXPORT_PUBLIC
cfg_func_table wifi_tfunc = {
		.get_item_state = wifi_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* flight mode - DONE */
EXPORT_PUBLIC
cfg_func_table flightmode_tfunc = {
		.get_item_state = flightmode_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* bluetooth - ***** FAILED ***** */
EXPORT_PUBLIC
cfg_func_table bluetooth_tfunc = {
		.get_item_state = bluetooth_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* driving mode - Done */
EXPORT_PUBLIC
cfg_func_table drivingmode_tfunc = {
		.get_item_state = drivingmode_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* network restrictrion mode - Done */
EXPORT_PUBLIC
cfg_func_table network_restruction_mode_tfunc = {
		.get_item_state = network_restriction_mode_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* personal mode ?? - partially done */
EXPORT_PUBLIC
cfg_func_table personalmode_tfunc = {
		.get_item_state = personalmode_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

/* NFC - ??? */
EXPORT_PUBLIC
cfg_func_table nfc_tfunc = {
		.get_item_state = nfc_toggle_get_state,
		.set_item_state = NULL,
		.set_item_update_ui = NULL, };

EXPORT_PUBLIC
void setting_cfg_print(void)
{
	JsonNode *node = root;

	JsonGenerator *generator = json_generator_new();
	g_object_set(generator, "pretty", TRUE, NULL);
	json_generator_set_root(generator, node);
	gchar *data = json_generator_to_data(generator, NULL);
	SETTING_TRACE("%s", (char *)data);

	g_free(data);
	g_object_unref(generator);
}

/* create config file from scratch */
static Setting_Cfg_Node_T s_cfg_node_array[] = {
	{	KeyStr_Connections, NULL, "move://Tab2.top",
		Cfg_Item_Pos_Level0, 0,
		0, Cfg_Item_Title_Node, NULL, NULL, NULL,
		uuid_Connectivity, 0, NULL},
	{	KeyStr_WiFi, IMG_WiFi, "wifi-efl-ug", Cfg_Item_Pos_Level0,
		Cfg_Item_Resetable, 0, Cfg_Item_AppLauncher_Node, NULL,
		KeyStr_Connections, &wifi_tfunc, uuid_WiFi, 0,
		"wifi-efl-ug"},
	{	KeyStr_Bluetooth, IMG_Bluetooth, "ug-bluetooth-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable,
		0, Cfg_Item_AppLauncher_Node, NULL, KeyStr_Connections,
		&bluetooth_tfunc, uuid_Bluetooth, 0,
		"ug-bluetooth-efl"},
#ifdef TIZEN_BUILD_TARGET
	{	KeyStr_FlightMode, IMG_FlightMode, "setting-flightmode-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_unResetable,
		0, Cfg_Item_Ui_Node_Toggle, NULL, KeyStr_Connections,
		&flightmode_tfunc, uuid_FlightMode, 0, NULL},
#endif

#ifdef TIZEN_BUILD_EMULATOR
	{	KeyStr_NFC, IMG_NFC, "ug-nfc-efl|type:nfc", Cfg_Item_Pos_Level0,
		Cfg_Item_unResetable,
		0, Cfg_Item_Ug_Node_Toggle, NULL, KeyStr_Connections,
		&nfc_tfunc, uuid_NFC, 0, "ug-nfc-efl"},
#endif

#ifdef TIZEN_BUILD_TARGET
	{	KeyStr_MobileAP, IMG_MobileAP, "ug-setting-mobileap-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable,
		0, Cfg_Item_AppLauncher_Node, NULL, KeyStr_Connections,
		NULL, uuid_MobileAP, 0, NULL},
#endif
#ifdef TIZEN_BUILD_TARGET
	{	KeyStr_MobileNetworks, IMG_Network, "setting-network-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_Ug_Node, NULL, KeyStr_Connections, NULL,
		uuid_Network, 0, NULL},
#endif
	/*More connections */
	{	KeyStr_MoreConnectionSettings, IMG_More_Connections,
		"setting-moreconnections-efl", Cfg_Item_Pos_Level0,
		Cfg_Item_Resetable, 0, Cfg_Item_Ug_Node, NULL,
		KeyStr_Connections, NULL, uuid_Network, 0, NULL},

	/* Group:Device */
	{	KeyStr_Device, NULL, "move://Tab3.top",
		Cfg_Item_Pos_Level0, 0, 0,
		Cfg_Item_Title_Node, NULL, NULL, NULL,
		uuid_SoundDisplay, 0, NULL},
	{	KeyStr_Sounds, IMG_Sounds, "setting-profile-efl",
		Cfg_Item_Pos_Level0,
		Cfg_Item_Resetable, 0, Cfg_Item_Ug_Node, NULL,
		KeyStr_Device, NULL, uuid_SoundDisplay, 0, NULL},
	{	KeyStr_Display, IMG_Display,
		"org.tizen.setting-display",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Device, NULL,
		uuid_Display, 0, "org.tizen.setting-display"},
	{	KeyStr_ApplicationsItem, IMG_Applications,
		"setting-applications-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_Ug_Node, NULL, KeyStr_Device, NULL,
		uuid_Display, 0, "org.tizen.setting.display"},

	/* Group: Personal */
	{	KeyStr_Personal, NULL, "move://Tab3.top",
		Cfg_Item_Pos_Level0, 0, 0,
		Cfg_Item_Title_Node, NULL, NULL, NULL, uuid_Personal,
		0, NULL},
	{	KeyStr_Wallpaper, IMG_Wallpaper,
		"org.tizen.wallpaper-ui-service",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Personal, NULL,
		uuid_HomeAndLockscreen, 0,
		"org.tizen.setting.wallpaper-ui-service"},
	{	KeyStr_LockScreen, IMG_LockedScreen, "setting-security-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Personal, NULL,
		uuid_LockedScreen, 0, "ug-lockscreen-options-efl"},
#if 0
	/*Accessbility - not supported since 2015.8.12 */
	{	KeyStr_Accessibility, IMG_Accessibility,
		"setting-accessibility-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_unResetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Personal, NULL,
		uuid_Accounts, 0, "setting-accessibility-efl"},
#endif
	{	KeyStr_Accounts, IMG_Accounts,
		"setting-myaccount-efl|mode:account_list",
		Cfg_Item_Pos_Level0, Cfg_Item_unResetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Personal, NULL,
		uuid_Accounts, 0, "setting-myaccount-efl"},
	{	KeyStr_Privacy, IMG_Privacy,
		"org.tizen.privacy-setting",
		Cfg_Item_Pos_Level0, Cfg_Item_unResetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_Personal, NULL,
		uuid_Accounts, 0, "org.tizen.privacy-setting"},




	/* Group: System */
	{	KeyStr_System, NULL, "move://Tab4.top",
		Cfg_Item_Pos_Level0, 0, 0,
		Cfg_Item_Title_Node, NULL, NULL, NULL,
		uuid_DeviceMange, 0, NULL},
	{	KeyStr_LanguageInput, IMG_LanguageInput,
		"setting-phone-efl|viewtype:language",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_System, NULL,
		uuid_LanguageKeyboard, 0, "org.tizen.setting.language"},
#if SUPPORT_STORAGE
	{	KeyStr_Storage, IMG_Storage,
		"org.tizen.setting-storage",
		Cfg_Item_Pos_Level0, Cfg_Item_unResetable, 0,
		Cfg_Item_AppLauncher_Node, NULL, KeyStr_System, NULL,
		uuid_Storage, 0, "org.tizen.setting-storage"},
#endif
	{	KeyStr_DateTime, IMG_DateTime, "setting-time-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_Ug_Node, NULL, KeyStr_System, NULL,
		uuid_DateTime, 0, NULL},
	/*{KeyStr_DeveloperOption, IMG_USBconnection,
	 * "setting-developeroption-efl|viewtype:usb", Cfg_Item_Pos_Level0,
	 * Cfg_Item_Resetable, 0, Cfg_Item_Ug_Node, NULL, KeyStr_DeviceMange,
	 * NULL, uuid_DeveloperOption, 0,
	 * "org.tizen.setting.developeroptions"}, */
	{	KeyStr_AboutDevice, IMG_AboutDevice, "setting-about-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_Ug_Node, NULL, KeyStr_System, NULL,
		uuid_AboutPhone, 0, NULL},

#if 0
	{	KeyStr_Security, IMG_Security, "setting-security-efl",
		Cfg_Item_Pos_Level0, Cfg_Item_Resetable, 0,
		Cfg_Item_Ug_Node, NULL, KeyStr_DeviceMange, NULL,
		uuid_Security, 0, NULL},
	/* Downloaded App */
	{	KeyStr_DownloadedAPPs, NULL, "move://Tab6.top",
		Cfg_Item_Pos_Level0,
		0, 0, Cfg_Item_Title_Node, NULL, NULL, NULL, uuid_APP,
		0, NULL},
#endif
};

EXPORT_PUBLIC Setting_Cfg_Node_T *get_cfg_node_by_keystr(const char *keystr)
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
#if 0
		SETTING_TRACE("keystr: %s", keystr);
		SETTING_TRACE("s_cfg_node_array[i].key_name: %s",
				s_cfg_node_array[i].key_name);
		SETTING_TRACE("---------> _(keystr): %s", _(keystr));
		SETTING_TRACE("---------> _(s_cfg_node_array[i].key_name): %s",
				_(s_cfg_node_array[i].key_name));
#endif
		if (0 == safeStrCmp(_(keystr),
				_(s_cfg_node_array[i].key_name))) {
#if 0
			SETTING_TRACE("MATCH !!!!!!!!!!!!!!");
#endif
			return &(s_cfg_node_array[i]);
		}
	}
	return NULL;
}

EXPORT_PUBLIC int get_index_by_keystr(const char *keystr)
{
	int size = get_cfg_array_size();
	/*SETTING_TRACE(" size of array : %d ", size); */

	int i;
	/*SETTING_TRACE("keystr:%s", keystr); */
	for (i = 0; i < size; i++) {
		/*SETTING_TRACE("s_cfg_node_array[%d].key_name:%s", i,
		 * s_cfg_node_array[i].key_name); */
		if (0 == safeStrCmp(keystr, s_cfg_node_array[i].key_name))
			return i;
	}
	/*not found */
	return -1;
}

EXPORT_PUBLIC char *get_iconpath_by_keystr(const char *keystr)
{
	int size = get_cfg_array_size();
	int i;
	/*SETTING_TRACE("keystr:%s", keystr); */

	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(keystr, _(s_cfg_node_array[i].key_name))) {
			if (s_cfg_node_array[i].icon_path)
				return s_cfg_node_array[i].icon_path;
			else
				break;
		}
	}
	return IMG_SETTING;
}

/* plistitem->title */
EXPORT_PUBLIC void *get_node_pointer(char *name)
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(name, _(s_cfg_node_array[i].key_name)))
			return s_cfg_node_array[i].data;
	}
	return NULL;
}

EXPORT_PUBLIC int set_node_pointer(char *name, void *ptr)
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(name, s_cfg_node_array[i].key_name)) {
			/*return i; */
			s_cfg_node_array[i].data = ptr;
			return 0;
		}
	}

	return -1;
}

EXPORT_PUBLIC int set_cfg_node_tfunc(char *name, void *ptr)
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(_(name), _(s_cfg_node_array[i].key_name))) {
			/*return i; */
			s_cfg_node_array[i].tfunc = (cfg_func_table *)ptr;
			return 0;
		}
	}

	return -1;
}

EXPORT_PUBLIC int set_cfg_node_tfunc_set_ptr(char *name,
		int (*set_item_state)(Cfg_Item_State stat, void *item,
				void *data))
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(_(name), _(s_cfg_node_array[i].key_name))) {
			/*return i; */
			s_cfg_node_array[i].tfunc->set_item_state =
					set_item_state;
			return 0;
		}
	}

	return -1;
}

EXPORT_PUBLIC
Setting_Cfg_Node_T *get_cfg_node_first()
{
	return &s_cfg_node_array[0];
}

EXPORT_PUBLIC
int get_cfg_array_size()
{
	int arr_size = sizeof(s_cfg_node_array);
	int node_size = sizeof(s_cfg_node_array[0]);

	return arr_size / node_size;
}

/**
 * @brief

 Cfg_Item_Title_Node
 Cfg_Item_App_Node
 Cfg_Item_Ui_Node
 Cfg_Item_Ug_Node
 *
 * @param check_ug
 *
 * @return
 */
EXPORT_PUBLIC
int setting_cfg_create(bool check_ug_exist)
{
	SETTING_TRACE_BEGIN;
	JsonNode *category_list, *category, *menu_item;
	JsonArray *menu = NULL;
	JsonObject *object = NULL;

	/* Category list */
	category_list = json_node_new(JSON_NODE_ARRAY);
	json_node_take_array(category_list, json_array_new());

	Setting_Cfg_Node_T *pitem = get_cfg_node_first(); /* head */
	int size = get_cfg_array_size();
	SETTING_TRACE(" size of array : %d ", size);

	int i;
	for (i = 0; i < size; i++) {
		/* Check Emul and block Wifi & Bluetooth menu */
		if (isEmulBin() && (!safeStrCmp(pitem[i].key_name,
				KeyStr_Bluetooth) || !safeStrCmp(
						pitem[i].key_name,
						KeyStr_WiFi))) {
			SETTING_TRACE("Skip %s", pitem[i].key_name);
			continue;
		}

		SETTING_TRACE(" index : %d ", i);
		Cfg_Item_Type item_type = pitem[i].item_type % 10;
		if (item_type == Cfg_Item_Title_Node) {
			SETTING_TRACE(" add title : %s ", pitem[i].key_name);

			/* __create_a_menu HEADER */
			__create_a_menu(pitem[i].key_name);

		} else if (item_type == Cfg_Item_App_Node
				|| item_type == Cfg_Item_Ui_Node
				|| item_type == Cfg_Item_AppLauncher_Node) {

			__create_an_item(pitem[i].key_name, pitem[i].icon_path,
					pitem[i].ug_args,
					pitem[i].shortcut_appid, pitem[i].pos,
					pitem[i].item_type, pitem[i].reset_type,
					false, pitem[i].uuid,
					pitem[i].click_times,
					pitem[i].last_clicked);
			/*SETTING_TRACE(" add menu - name : %s (%d) - APP ",
			 * pitem[i].key_name,pitem[i].item_type); */

		} else if (item_type == Cfg_Item_Ug_Node) {
			/* app-control */
			__create_an_item(pitem[i].key_name, pitem[i].icon_path,
					pitem[i].ug_args,
					pitem[i].shortcut_appid, pitem[i].pos,
					pitem[i].item_type, pitem[i].reset_type,
					check_ug_exist, pitem[i].uuid,
					pitem[i].click_times,
					pitem[i].last_clicked);
			/*SETTING_TRACE(" add menu - name : %s (%d) - UG ",
			 * pitem[i].key_name,pitem[i].item_type); */
		}
	}
	/* write to file */
	int ret = setting_cfg_file_write(category_list);
	json_node_free(category_list);
	return ret;
}

/* load file */
EXPORT_PUBLIC
int setting_cfg_file_read(void)
{
	struct stat sb;
	memset(&sb, 0, sizeof(struct stat));
	int r = stat(setting_cfg_get_path(), &sb);
	if (0 != r)
		SETTING_TRACE("ERROR, r:%d", r);

	SETTING_TRACE("sb.st_size:%d", sb.st_size);

	parser = json_parser_new(); /* to be freed on exit */
	/* file -> parser */
	GError *error = NULL;
	/*gboolean ret = json_parser_load_from_file( parser,
	 * setting_cfg_get_path(), &error ); */
	if (!sb.st_size
			|| FALSE == json_parser_load_from_file(parser,
					setting_cfg_get_path(),
					&error)) {
		if (error)
			SETTING_TRACE_ERROR("error->message:%s",
					(char *)(error->message));
		/*return FALSE; */

		/* The file is existing and can be accessed normally, but it was
		 * loaded failed as a json script, it means setting.cfg is
		 * damaged(not the normal formatting ), we need to remove
		 * the file and recreate in next running time*/
		SETTING_TRACE_ERROR(
				"The file[%s] is existing and can be accessed "
				"normally, but it was 0 size or loaded failed "
				"as a json script, it means setting.cfg is "
				"damaged (the formatting ia abnormal), we need "
				"to remove the file and recreate in next "
				"running time!",
				setting_cfg_get_path());

		SETTING_TRACE("Trying to removing the damaged file.");
		if (remove(setting_cfg_get_path()) != 0) {
			SETTING_TRACE_ERROR("Error to remove the damaged file");
			return FALSE;
		}

		if (FALSE == setting_cfg_create(true)) {
			SETTING_TRACE_ERROR(
					"Error to create a new config file");
			return FALSE;
		}
		/*if the failed is caused by reading the file recursively
		 * invoking */
		return setting_cfg_file_read();
	}

	/* parser -> root */
	root = json_parser_get_root(parser); /* to be freed on exit*/
	return TRUE;
}

/**
 * @brief dump JSON to file
 */
EXPORT_PUBLIC
int setting_cfg_file_write(JsonNode *node)
{
	GError *error = NULL;
	JsonGenerator *generator = json_generator_new();
	json_generator_set_root(generator, node);
	/*write file in indent format */
	g_object_set(generator, "pretty", TRUE, NULL);
	gboolean ret = json_generator_to_file(generator, setting_cfg_get_path(),
			&error);
	g_object_unref(generator);

	/* ***BEGIN***	DAC black screen SAMSUNG 2010/8/9 add
	 *add read permission
	 *chown -R inhouse:inhouse /home/inhouse/setting.cfg
	 */
	if (FALSE == ret) {
		SETTING_TRACE_ERROR("Error writing file %s!",
				setting_cfg_get_path());
		return FALSE;
	}
	return TRUE;
}

/*	 public functions  */
EXPORT_PUBLIC
int setting_cfg_init(void)
{
	retv_if(!setting_cfg_get_path(), Cfg_Error_Type_OutOfMemory);

	if (!access(setting_cfg_get_path(), R_OK | W_OK | F_OK)) {
		/* succeed to access */
		if (!setting_cfg_file_read()) { /* return FALSE */
			if (remove(setting_cfg_get_path()))
				return Cfg_Error_Type_RemoveCfg_Failed;

			return Cfg_Error_Type_ReadCfg_Failed;
		}

		return Cfg_Error_Type_Sucess;
	} else { /* fail to access */
		switch (errno) {
		/* file non-existing case */
		case ENOENT:
			SETTING_TRACE_ERROR("non-existing [%s]",
					setting_cfg_get_path());
			if (!ecore_file_is_dir(setting_cfg_get_dir_path())) {
				SETTING_TRACE_ERROR("non-existing [%s]",
						setting_cfg_get_dir_path());
				Eina_Bool flag = ecore_file_mkdir(
						setting_cfg_get_dir_path());
				if (flag == EINA_FALSE) {
					SETTING_TRACE_ERROR(
							">failed to create dir");
				} else {
					SETTING_TRACE_ERROR(
							">OK to create dir");
				}
			}

			if (!setting_cfg_create(true)) { /* return FALSE */
				SETTING_TRACE_ERROR(
						"Error to create a new config file");
				return Cfg_Error_Type_CreateCfg_Failed;
			}

			if (!setting_cfg_file_read()) { /* return FALSE */
				SETTING_TRACE_ERROR(
						"Error to read config file");
				if (remove(setting_cfg_get_path()))
					return Cfg_Error_Type_RemoveCfg_Failed;

				return Cfg_Error_Type_ReadCfg_Failed;
			}

			return Cfg_Error_Type_Sucess;
			/* other cases */
		case EACCES:
		case EROFS:
		default:
			return Cfg_Error_Type_DirPermissionDenied;
		}
	}
}

EXPORT_PUBLIC
void setting_cfg_exit(void)
{
	if (cfg_dir_path) {
		free(cfg_dir_path);
		cfg_dir_path = NULL;
	}
	if (cfg_file_path) {
		free(cfg_file_path);
		cfg_file_path = NULL;
	}
	if (parser) {
		g_object_unref(parser);
		parser = NULL;
	}
}

EXPORT_PUBLIC
int setting_cfg_file_update(void)
{
	free(cfg_dir_path);
	free(cfg_file_path);
	cfg_dir_path = NULL;
	cfg_file_path = NULL;
	if (access(setting_cfg_get_path(), W_OK | F_OK) != 0)
		return FALSE;

	return setting_cfg_file_write(root);
}

static void _update_nodes_table_by_uuid(char *uuid, int click_times,
		int last_clicked)
{
	int size = get_cfg_array_size();

	int i;
	for (i = 0; i < size; i++) {
		if (0 == safeStrCmp(uuid, (s_cfg_node_array[i].uuid))) {
			s_cfg_node_array[i].click_times = click_times;
			s_cfg_node_array[i].last_clicked = last_clicked;
			return;
		}
	}
}

EXPORT_PUBLIC
int setting_cfg_migrate(void)
{
	/*1. read old cfg file */
	struct stat sb;
	memset(&sb, 0, sizeof(struct stat));
	int r = stat(setting_cfg_get_path(), &sb);
	if (0 != r)
		SETTING_TRACE("ERROR, r:%d", r);

	SETTING_TRACE("sb.st_size:%d", sb.st_size);

	parser = json_parser_new();
	GError *error = NULL;
	if (!sb.st_size
			|| FALSE == json_parser_load_from_file(parser,
					setting_cfg_get_path(),
					&error)) {
		if (error)
			SETTING_TRACE_ERROR("error->message:%s",
					(char *)(error->message));

		/*if read old cfg file unseccessfully, remove it */
		SETTING_TRACE("Read old cfg fail, Trying to remove it");
		if (remove(setting_cfg_get_path()) != 0)
			SETTING_TRACE_ERROR("Error to remove the damaged file");

		/*re-create cfg file */
		SETTING_TRACE("Trying to re-create cfg file.");
		if (FALSE == setting_cfg_create(true))
			SETTING_TRACE_ERROR(
					"Error to create a new config file");

		g_object_unref(parser);
		parser = NULL;
		return FALSE;
	}
	root = json_parser_get_root(parser);

	/* 2. update click_times & last_clicked in s_cfg_node_array with old
	 * cfg */
	int i, j, click_times, last_clicked;
	JsonObject *category_obj;
	JsonArray *menu;
	JsonObject *menu_item;
	char *uuid;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			category_obj = json_array_get_object_element(
					json_node_get_array(root), i);
			menu = json_object_get_array_member(category_obj,
					"menu");
			menu_item = json_array_get_object_element(menu, j);
			uuid = (char *)json_object_get_string_member(menu_item,
					"uuid");
			click_times = json_object_get_int_member(menu_item,
					"click_times");
			last_clicked = json_object_get_int_member(menu_item,
					"last_clicked");
			if (click_times != 0 || last_clicked != 0) {
				_update_nodes_table_by_uuid(uuid, click_times,
						last_clicked);
			}
		}
	}

	/*3. create new cfg file */
	if (!setting_cfg_create(true)) {
		SETTING_TRACE_ERROR("fail to create new cfg file");
		setting_cfg_exit();
		return FALSE;
	}

	setting_cfg_exit();
	return TRUE;
}

EXPORT_PUBLIC
int setting_cfg_get_category_length(void)
{
	return json_array_get_length(json_node_get_array(root));
}

EXPORT_PUBLIC
char *setting_cfg_get_category_name(int category_index)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	return (char *)json_object_get_string_member(category_obj, "name");
}

EXPORT_PUBLIC
int setting_cfg_get_menu_length(int category_index)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	return json_array_get_length(menu);
}

EXPORT_PUBLIC
char *setting_cfg_get_keyname_idx(int category_index, int menu_index)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	return (char *)json_object_get_string_member(menu_item, "key_name");
}

EXPORT_PUBLIC
int setting_cfg_get_itemtype_idx(int category_index, int menu_index)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	return json_object_get_int_member(menu_item, "item_type");
}

EXPORT_PUBLIC
char *setting_cfg_get_string_field_idx(int category_index, int menu_index,
		char *field_name)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);

	char *data = (char *)json_object_get_string_member(menu_item,
			field_name);
	if (!safeStrCmp(data, "(null)")) {
		/* pass NULL to ug_args when create setting.cfg. */
		return NULL;
	}
	return data;
}

EXPORT_PUBLIC
int setting_cfg_get_int_field_idx(int category_index, int menu_index,
		char *field_name)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	return json_object_get_int_member(menu_item, field_name);
}

EXPORT_PUBLIC
char *setting_cfg_get_icon_path_idx(int category_index, int menu_index)
{
	return setting_cfg_get_string_field_idx(category_index, menu_index,
			"icon_path");
}

EXPORT_PUBLIC
char *setting_cfg_get_shortcut_appid_idx(int category_index, int menu_index)
{
	return setting_cfg_get_string_field_idx(category_index, menu_index,
			"shortcut_appid");
}

EXPORT_PUBLIC
char *setting_cfg_get_icon_path(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_icon_path_idx(i, j);
		}
	}
	return NULL;
}

EXPORT_PUBLIC
char *setting_cfg_get_ug_args_idx(int category_index, int menu_index)
{
	return setting_cfg_get_string_field_idx(category_index, menu_index,
			"ug_args");
}

EXPORT_PUBLIC
char *setting_cfg_get_ug_args(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_ug_args_idx(i, j);
		}
	}
	return NULL;
}

EXPORT_PUBLIC
char *setting_cfg_get_ugpath_idx(int category_index, int menu_index)
{
	return setting_cfg_get_string_field_idx(category_index, menu_index,
			"ug_path");
}

EXPORT_PUBLIC
char *setting_cfg_get_ugpath(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_ugpath_idx(i, j);
		}
	}
	return NULL;
}

EXPORT_PUBLIC
int setting_cfg_get_pos_idx(int category_index, int menu_index)
{
	return setting_cfg_get_int_field_idx(category_index, menu_index, "pos");
}

EXPORT_PUBLIC
int setting_cfg_get_pos(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_pos_idx(i, j);
		}
	}
	return 0;
}

EXPORT_PUBLIC
void setting_cfg_set_pos_idx(int category_index, int menu_index, int pos)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	json_object_remove_member(menu_item, "pos");
	json_object_set_int_member(menu_item, "pos", pos);
}

EXPORT_PUBLIC
void setting_cfg_set_pos(char *keyname, int pos)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0) {
				setting_cfg_set_pos_idx(i, j, pos);
				return;
			}
		}
	}

}

EXPORT_PUBLIC
int setting_cfg_get_last_clicked_idx(int category_index, int menu_index)
{
	return setting_cfg_get_int_field_idx(category_index, menu_index,
			"last_clicked");
}

EXPORT_PUBLIC
int setting_cfg_get_last_clicked(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_last_clicked_idx(i, j);
		}
	}
	return 0;
}

EXPORT_PUBLIC
void setting_cfg_set_last_clicked_idx(int category_index, int menu_index,
		int last_clicked)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	json_object_remove_member(menu_item, "last_clicked");
	json_object_set_int_member(menu_item, "last_clicked", last_clicked);
}

EXPORT_PUBLIC
void setting_cfg_set_last_clicked(char *keyname, int last_clicked)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0) {
				setting_cfg_set_last_clicked_idx(i, j,
						last_clicked);
				return;
			}
		}
	}

}

EXPORT_PUBLIC
int setting_cfg_get_click_times_idx(int category_index, int menu_index)
{
	return setting_cfg_get_int_field_idx(category_index, menu_index,
			"click_times");
}

EXPORT_PUBLIC
int setting_cfg_get_click_times(char *keyname)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0)
				return setting_cfg_get_click_times_idx(i, j);
		}
	}
	return 0;
}

EXPORT_PUBLIC
void setting_cfg_set_click_times_idx(int category_index, int menu_index,
		int click_times)
{
	JsonObject *category_obj = json_array_get_object_element(
			json_node_get_array(root), category_index);
	JsonArray *menu = json_object_get_array_member(category_obj, "menu");
	JsonObject *menu_item = json_array_get_object_element(menu, menu_index);
	json_object_remove_member(menu_item, "click_times");
	json_object_set_int_member(menu_item, "click_times", click_times);
}

EXPORT_PUBLIC
void setting_cfg_set_click_times(char *keyname, int click_times)
{
	int i, j;
	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
			if (safeStrCmp(setting_cfg_get_keyname_idx(i, j),
					keyname) == 0) {
				setting_cfg_set_click_times_idx(i, j,
						click_times);
				time_t ctime = time(NULL);
				setting_cfg_set_last_clicked_idx(i, j,
						(int)ctime);
				return;
			}
		}
	}

}

EXPORT_PUBLIC
int setting_cfg_get_resetable_flag_idx(int category_index, int menu_index)
{
	return setting_cfg_get_int_field_idx(category_index, menu_index,
			"is_resetable");
}

EXPORT_PUBLIC
void setting_cfg_add_downloaded_app(char *keyname, char *icon_path,
		char *ug_args, int pos)
{
	int i;
	JsonNode *menu_item;
	JsonObject *object;
	JsonArray *menu;

	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		if (safeStrCmp(setting_cfg_get_category_name(i),
				"Downloaded App") == 0) {
			menu_item = json_node_new(JSON_NODE_OBJECT);
			object = json_object_new();
			json_node_take_object(menu_item, object);
			json_object_set_string_member(object, "key_name",
					keyname);
			json_object_set_string_member(object, "icon_path",
					icon_path);
			json_object_set_string_member(object, "ug_args",
					ug_args);
			json_object_set_int_member(object, "pos", pos);

			object = json_array_get_object_element(
					json_node_get_array(root), i);
			menu = json_object_get_array_member(object, "menu");
			json_array_add_element(menu, menu_item);
		}
	}
}

void setting_cfg_remove_downloaded_app(char *keyname)
{
	int i, j;
	JsonObject *object;
	JsonArray *menu;

	for (i = 0; i < setting_cfg_get_category_length(); i++) {
		if (safeStrCmp(setting_cfg_get_category_name(i),
				"Downloaded App") == 0) {
			for (j = 0; j < setting_cfg_get_menu_length(i); j++) {
				if (safeStrCmp(setting_cfg_get_keyname_idx(
						i, j), keyname) == 0) {
					object = json_array_get_object_element(
							json_node_get_array(
									root),
							i);
					menu = json_object_get_array_member(
							object, "menu");
					json_array_remove_element(menu, j);
					return;
				}
			}
		}
	}
}

EXPORT_PUBLIC
bool is_ug_installed_by_ug_args(void *data)
{
	retv_if(!data, TRUE);/*if passing NULL,drawing it */
	char *ug_args = data;
	char *path = get_ug_path_from_ug_args(ug_args);
	int is_installed = ug_is_installed(path);

	FREE(path);
	return is_installed;
}

/*other relative function*/
EXPORT_PUBLIC
char *get_ug_path_from_ug_args(void *data)
{
	/*SETTING_TRACE_BEGIN;*/
	char *p = (char *)data;
	if (NULL == p || '\0' == p[0])
		return NULL;

	char *q = strchr(p, '|');
	char *path = NULL;

	if (q) { /* copy out the content before '|'; eg, ab|cd */
		path = (char *)calloc(1, q - p + 1);
		setting_retvm_if(!path, NULL, "calloc failed");
		safeCopyStr(path, p, q - p);
		/* redundant handle */
		path[q - p] = '\0';
	} else { /* copy out all the content. eg.abb */

		path = (char *)calloc(1, safeStrLen(p) + 1);
		setting_retvm_if(!path, NULL, "calloc failed");
		safeCopyStr(path, p, safeStrLen(p));
	}
	/*SETTING_TRACE("get the ug['%s']", path);*/
	return path;
}

EXPORT_PUBLIC
app_control_h get_bundle_from_ug_args(void *data)
{
	/*SETTING_TRACE_BEGIN;*/
	char *p = (char *)data;

	if (NULL == p || '\0' == p[0])
		return NULL;

	char *m = NULL;
	char *q = strchr(p, '|');
	if (q) {
		/* (key, value) pairs exit. eg: ug_args =
		"sevenemail-setting-efl|caller:setting; cmd:main option" */
		/* alloc data */
		app_control_h svc;
		if (app_control_create(&svc))
			return NULL;
		int str_len = safeStrLen(p) + 1;
		char *v_key = (char *)calloc(1, str_len);
		if (!v_key) {
			app_control_destroy(svc);
			return NULL;
		}

		char *v_value = (char *)calloc(1, str_len);
		if (!v_value) {
			app_control_destroy(svc);
			FREE(v_key);
			return NULL;
		}

		p = ++q;
		while (p) {
			q = strchr(p, ';');
			if (q) { /* not the last field */
				m = strchr(p, ':'); /* ziduannei */
				if (m) {
					safeCopyStr(v_key, p, m - p);

					++m;
					safeCopyStr(v_value, m, q - m);
					SETTING_TRACE("To add ('%s', '%s') to bundle data",
							v_key, v_value);
					app_control_add_extra_data(svc, v_key,
							v_value);
				} else {
					SETTING_TRACE_ERROR(
							"invalid key-value format!!\n");
					break; /* end the whole while */
				}
			} else { /* the last field */

				m = strchr(p, ':'); /* ziduannei */
				if (m) {
					safeCopyStr(v_key, p, m - p);
					++m;
					safeCopyStr(v_value, m, safeStrLen(m));

					app_control_add_extra_data(svc, v_key,
							v_value);
				} else {
					SETTING_TRACE_ERROR(
							"invalid key-value format!!\n");
				}
				break; /* end the whole while */
			}

			memset(v_key, '\0', str_len);
			memset(v_value, '\0', str_len);
			p = ++q;
		}

		FREE(v_key);
		FREE(v_value);
		return svc;
	} else { /* eg: ug_args = "setting-browser-efl" */

		/*SETTING_TRACE("(key, value) pairs not exit");*/
		return NULL;
	}

}

EXPORT_PUBLIC void setting_cfg_dump_basic_info()
{
#if 1
	bool fileExit = (0 ==
			access(setting_cfg_get_path(), R_OK | W_OK | F_OK));
	SETTING_TRACE("%s's exist:%d", setting_cfg_get_path(), fileExit);

	struct stat sb;
	memset(&sb, 0, sizeof(struct stat));
	int r = stat(setting_cfg_get_path(), &sb);
	if (0 != r)
		SETTING_TRACE("ERROR, r:%d", r);

	SETTING_TRACE("sb.st_size:%d", sb.st_size);
#endif
}

/**
 * @brief helper function for search
 *
 * @param key_name
 * @param url
 * @param icon_path
 * @param item_type
 * @param data
 *
 * @return
 */
EXPORT_PUBLIC Setting_Cfg_Node_T *setting_plugin_search_item_add(char *key_name,
		char *url, char *icon_path, int item_type, void *data)
{
	Setting_Cfg_Node_T *node = calloc(1, sizeof(Setting_Cfg_Node_T));
	if (node) {
		if (key_name)
			node->key_name = strdup(key_name);
		if (url)
			node->ug_args = strdup(url);
		if (icon_path)
			node->icon_path = strdup(icon_path);
		node->pos = 0;
		node->reset_type = 0;
		node->click_times = 0;
		node->item_type = item_type;
		node->data = data;
		node->sub_index = NULL;
		node->tfunc = NULL;
	}
	return node;
}

EXPORT_PUBLIC Setting_Cfg_Node_T *setting_plugin_search_item_subindex_add(
		char *key_name, char *url, char *icon_path, int item_type,
		void *data, char *sub_index)
{
	Setting_Cfg_Node_T *node = calloc(1, sizeof(Setting_Cfg_Node_T));
	if (node) {
		if (key_name)
			node->key_name = strdup(key_name);
		if (url)
			node->ug_args = strdup(url);
		if (icon_path)
			node->icon_path = strdup(icon_path);
		node->pos = 0;
		node->reset_type = 0;
		node->click_times = 0;
		node->item_type = item_type;
		node->data = data;
		if (sub_index)
			node->sub_index = strdup(sub_index);
		node->tfunc = NULL;
	}
	return node;
}

EXPORT_PUBLIC Cfg_Item_Type setting_search_type(char *type)
{
	Cfg_Item_Type ret_code = Cfg_Item_Node_Error;
	if (type) {
		if (!safeStrCmp(type, SEARCH_TYPE_ERROR))
			ret_code = Cfg_Item_Node_Error;
		else if (!safeStrCmp(type, SEARCH_TYPE_UG_NODE))
			ret_code = Cfg_Item_Ug_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_UI_NODE))
			ret_code = Cfg_Item_Ui_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_TITLE_NODE))
			ret_code = Cfg_Item_Title_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_APP_NODE))
			ret_code = Cfg_Item_App_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_VIEW_NODE))
			ret_code = Cfg_Item_View_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_APPLAUNCHER_NODE))
			ret_code = Cfg_Item_AppLauncher_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_APPLAUNCHER_VIEW_NODE))
			ret_code = Cfg_Item_AppLauncher_View_Node;
		else if (!safeStrCmp(type, SEARCH_TYPE_UG_NODE_TOGGLE))
			ret_code = Cfg_Item_Ug_Node_Toggle;
		else if (!safeStrCmp(type, SEARCH_TYPE_UI_NODE_TOGGLE))
			ret_code = Cfg_Item_Ui_Node_Toggle;
		else if (!safeStrCmp(type, SEARCH_TYPE_APPLAUNCHER_NODE_TOGGLE))
			ret_code = Cfg_Item_AppLauncher_Node_Toggle;
		else if (!safeStrCmp(type, SEARCH_TYPE_VIEW_NODE_TOGGLE))
			ret_code = Cfg_Item_View_Node_Toggle;
	}

	return ret_code;
}

/*////////////////////////////////////////////////////////////////////////// */
/*
 * @return 0 on success, -1 on error
 */
EXPORT_PUBLIC
int wifi_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err;
	int value = -1;
	setting_get_int_slp_key(INT_SLP_SETTING_WIFI_STATUS, &value, &err);

	if (value == VCONFKEY_WIFI_OFF)
		*stat = Cfg_Item_Off;
	else
		*stat = Cfg_Item_On;

	return err;
}

EXPORT_PUBLIC
int flightmode_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err;
	int value = -1;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_FLIGHT_MODE, &value, &err);

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return err;
}

EXPORT_PUBLIC
int bluetooth_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err;
	int value = -1;
	setting_get_int_slp_key(INT_SLP_SETTING_BT_STATUS, &value, &err);

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return err;
}

EXPORT_PUBLIC
int drivingmode_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err = 0;
	*stat = Cfg_Item_Off;

	int value;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_DM_DRIVING_MODE, &value);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
		*stat = Cfg_Item_Error;
		return -1;
	}

	*stat = (value) ? Cfg_Item_On : Cfg_Item_Off;
	return err;
}

EXPORT_PUBLIC
int network_restriction_mode_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err;
	int value = -1;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_NET_RESTRICTION_MODE, &value,
			&err);

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return err;
}

/*////////////////////////////////////////////////////////////////////////// */
/* personal mode */

EXPORT_PUBLIC
int personalmode_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err = 0;
	*stat = Cfg_Item_Off;

	int value;
	int ret = vconf_get_bool(VCONFKEY_SETAPPL_PERSONAL_MODE_STATUS_BOOL,
			&value);
	if (ret != 0) {
		SETTING_TRACE_ERROR("get vconf failed");
		*stat = Cfg_Item_Error;
		return -1;
	}

	*stat = (value) ? Cfg_Item_On : Cfg_Item_Off;
	return err;
}

/*////////////////////////////////////////////////////////////////////////// */
/* NFC */

EXPORT_PUBLIC
int nfc_toggle_get_state(Cfg_Item_State *stat, void *data)
{
	SETTING_TRACE_BEGIN;
	int err;
	int value = -1;
	setting_get_bool_slp_key(BOOL_SLP_SETTING_NFC_STATUS, &value, &err);

	if (value == 1)
		*stat = Cfg_Item_On;
	else
		*stat = Cfg_Item_Off;

	return err;
}
