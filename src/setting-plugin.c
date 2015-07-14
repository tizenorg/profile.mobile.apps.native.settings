/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <unistd.h>
#include <eina_list.h>
#include <glib.h>


#include <setting-debug.h>
#include <setting-common-general-func.h>
#include <setting-common-draw-widget.h>

#include <setting-plugin.h>


extern setting_main_appdata *g_main_ad;

static Setting_GenGroupItem_Data *g_list_item;	/*TEST*/

/*
 * UI draw handler table : _g_draw_list
 *  - contains Object_Drawer typed object.
 */
Eina_List *_g_drawer_list = NULL;

#if 0
/**
 * @return Evas_Object * obj
 */
static void* navigationbar_func(void *data, xmlNode *xmlObj);
#endif

/*
 * @return void
 */
static void* group_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* link_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* slider_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* label_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* checkbox_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* editbox_func(void *data, xmlNode *xmlObj);

/*
 * @return Setting_GenGroupItem_Data* ptr
 */
static void* expandlist_func(void *data, xmlNode *xmlObj);

/**
 * @return nothing
 */
static void* expanditem_func(void *data, xmlNode *xmlObj);

/**
 * do nothing
 */
static void* settings_func(void *data, xmlNode *xmlObj);


static void* launch_func(void *data, xmlNode *xmlObj);

/**
 * do nothing
 */
static void* setting_func(void *data, xmlNode *xmlObj);

static int __node_walker(PluginNode* context, xmlNode* cur);

static int __node_finder(PluginNode* context, xmlNode* cur, char* id_str, char* value, bool* is_end);


static void __drawer_add(const char *type, drawer_fp draw)
{
	Object_Drawer *node = calloc(1, sizeof(Object_Drawer));
	if (node && type && draw)
	{
		node->type = type;
		node->draw = draw;

		_g_drawer_list = eina_list_append(_g_drawer_list, node);
	}
	// FREE(node);
}


static drawer_fp __drawer_find(char* type)
{
	SETTING_TRACE_BEGIN;
	SETTING_TRACE("node type:%s", type);
	Eina_List *check_list = _g_drawer_list;
	Object_Drawer *list_item = NULL;

	while (check_list) {
		list_item = (Object_Drawer *) eina_list_data_get(check_list);
		if (NULL == list_item)
			continue;

		if (0 == safeStrCmp(list_item->type, type))
		{
			//SETTING_TRACE("list_item->type:%s", list_item->type);
			break;
		}
		//if not matched,to check next node.
		check_list = eina_list_next(check_list);
		list_item = NULL;
	}
	//SETTING_TRACE("list_item:%p", list_item);
	return list_item ? list_item->draw : NULL;
}
void setting_drawer_list_init()
{
	SETTING_TRACE_BEGIN;
#if 0
	/* <navigationbar> */__drawer_add("navigationbar", navigationbar_func);
#endif
	/* <bool>          */__drawer_add("bool", checkbox_func);
	/* <string>        */__drawer_add("string", editbox_func);
	/* <group>         */__drawer_add("group", group_func);
	/* <integer>       */__drawer_add("integer", slider_func);
	/* <label>         */__drawer_add("label", label_func);
	/* <link>          */__drawer_add("link", link_func);
	/* <launch>        */__drawer_add("launch", launch_func);
	/* <extendlist>    */__drawer_add("expandlist", expandlist_func);
	/* <extenditem>    */__drawer_add("expanditem", expanditem_func);
	/* <settings>      */__drawer_add("settings", settings_func);
	/* <setting>       */__drawer_add("setting", setting_func);
}

void setting_drawer_list_fini()
{
	if (_g_drawer_list)
	{
		Object_Drawer *node = NULL;
		Eina_List *li = _g_drawer_list;
		while (li) {
			node = (Object_Drawer *) eina_list_data_get(li);
			if (node)
			{
				//SETTING_TRACE("Deregister %s", node->type);
				FREE(node);
			}
			li = eina_list_next(li);
		}
		_g_drawer_list = eina_list_free(_g_drawer_list);
		_g_drawer_list = NULL;
	}
}

/////////////////////////
/////////////////////////
/////////////////////////

#define MAX_CONTENT_LEN 512
#define MAX_LOCAL_BUFSIZE 128
#define DBUS_PATH "/setting/dbus_handler"
#define DBUS_SIGNAL_INTERFACE "org.tizen.setting.signal"

#define APPID_LENGTH 10
#define APPID_POS_FROM_PATH 10

static char* s_pkg_name;

#if 0
static char* substring(const char* str, size_t begin, size_t len)
{
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
    return 0;

  return strndup(str + begin, len);
}
#endif

/**
 * package ID
 *
 * ncbyusjryr.AppSetting --> package ID is ncbyusjryr
 * "Update_" "ncbyusjryr"
 */
static char* get_app_string(char* path)
{
	SETTING_TRACE_BEGIN;
	char* temp = substring(path, APPID_POS_FROM_PATH, APPID_LENGTH/*string size*/);
	SETTING_TRACE("package ID : >>> %s ",temp );
	return temp;
}

//------------------------------------------------------
// for client - bus
static DBusConnection *bus;
//------------------------------------------------------
static DBusHandlerResult __signal_filter(DBusConnection* conn, DBusMessage* message, void* user_data)
{
    int my_pid = getpid();
    int sender_pid = 0;
    char* key = NULL;
    char* value = NULL;

    DBusError error;
    dbus_error_init(&error);

	setting_main_appdata *ad = user_data;
	char* plugin_path = ad->plugin_path;
	char* pkg_name = get_app_string(plugin_path);

	if (pkg_name == NULL)
	{
		SETTING_TRACE("pkg_name is NULL - it's abnormal operation");
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}

	char str_buf[MAX_COMMON_BUFFER_LEN];
	snprintf(str_buf, MAX_COMMON_BUFFER_LEN, "Update_%s", pkg_name);
	s_pkg_name = strdup(str_buf);

	if (pkg_name)
	{
		free(pkg_name);
		pkg_name = NULL;
	}

	if (s_pkg_name == NULL)
	{
		SETTING_TRACE("s_pkg_name is NULL - it's abnormal operation");
		return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
	}
	SETTING_TRACE("s_pkg_name : %s ", s_pkg_name);
    if (dbus_message_is_signal(message, DBUS_SIGNAL_INTERFACE, s_pkg_name))
    {
        if (dbus_message_get_args(message, &error,
                DBUS_TYPE_UINT32, &sender_pid,
                DBUS_TYPE_STRING, &key,
                DBUS_TYPE_STRING, &value,
                DBUS_TYPE_INVALID) == FALSE)
        {
            SETTING_TRACE_ERROR("Fail to get data : %s", error.message);
            dbus_error_free(&error);
            return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
        }
    }

    if (sender_pid != 0 && my_pid != sender_pid)
    {
        SETTING_TRACE("received key : %s , value : %s", key, value);
		//-------------------------------------------------------------
		// received key : checkbox1|N/A , value : INT|1
		//-------------------------------------------------------------
		//char* key = "checkbox1|N/A";
		char* ptr = NULL;

		if (key) {
			ptr = strchr(key, '|');
		}

		xmlDocPtr doc = NULL;
		if (ptr && key)
		{
			//parsing for : checkbox1|N/A  -> checkbox1
			char* key_name = substring(key, 0, strlen(key)-strlen(ptr));
			char* val_name = strchr(value, '|');
			val_name++;

			// access xml file
			doc = xmlParseFile(plugin_path);
			if (doc == NULL)
			{
				SETTING_TRACE("unable to parse file : %s", plugin_path);
				FREE(key_name);
				return DBUS_HANDLER_RESULT_NOT_YET_HANDLED; 	/* xml parsing error */
			}
			// generate xml tree
			xmlNode *root = xmlDocGetRootElement(doc);
			// find a node
			bool is_end = false;

			// TODO : message queing
			// get key/value from d-bus and parse xml and search xml data with key
			static int count = 0;
			SETTING_TRACE(" BEGIN ---------------------------- dbus--> before __node_finder : %d", count);
			__node_finder((PluginNode*)ad->plugin_node, root, key_name ,val_name, &is_end);
			SETTING_TRACE(" END  ----------------------------- dbus--> after __node_finder : %d", count);
			count += 1;
		}
		// update the node
		GError *error = NULL;

#if 0
		if(doc != NULL)
		{
			xmlSaveFormatFile(ad->plugin_path, doc, 1);
			// TODO: make sure this is right
			xmlFreeDoc(doc);
			doc = NULL;
			SETTING_TRACE("__cfg_file_write successful");
		}
#endif

		// update UI
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}

static int __send_msg(char* key, char* value)
{
	DBusMessage* message;

	int sender_pid = getpid();

	if (bus == NULL)
		return -1;

	// ex) in gdb --> $15 = 0x43b6eb78 "Update_(null)" -> error codintion
	if (s_pkg_name == NULL)
	{
		SETTING_TRACE("s_pkg_name is NULL - it's abnormal operation");
		return -1;
	}

	message = dbus_message_new_signal(DBUS_PATH, DBUS_SIGNAL_INTERFACE, s_pkg_name);

	SETTING_TRACE("Sending message[%s:%s] via dbus", key ,value);
	if (dbus_message_append_args(message,
				DBUS_TYPE_UINT32, &sender_pid,
				DBUS_TYPE_STRING, &key,
				DBUS_TYPE_STRING, &value,
				DBUS_TYPE_INVALID) == FALSE)
	{
		SETTING_TRACE("Fail to load data error");
		return -1;
	}

	if (dbus_connection_send(bus, message, NULL) == FALSE)
	{
		SETTING_TRACE("Fail to send message");
		return -1;
	}

	dbus_connection_flush(bus);
	dbus_message_unref(message);

	SETTING_TRACE("[CLIENT] send data signal done");

    return 0;
}


static void __send_int_msg(xmlNode* xmlObj, int val)
{
	SETTING_TRACE_BEGIN;
	const char *id = (char*)xmlGetProp(xmlObj, "id");
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	char key[MAX_CONTENT_LEN] = {0,};
	snprintf(key, sizeof(key), "%s|%s", id, title);

	char value[MAX_CONTENT_LEN] = {0,};
	snprintf(value, sizeof(value), "INT|%d", val);
	__send_msg(key, value);
	SETTING_TRACE_END;
}


static void __send_string_msg(xmlNode* xmlObj, char *string)
{
	const char *id = (char*)xmlGetProp(xmlObj, "id");
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	char key[MAX_CONTENT_LEN] = {0,};
	snprintf(key, sizeof(key), "%s|%s", id, title);

	char value[MAX_CONTENT_LEN] = {0,};
	snprintf(value, sizeof(value), "STRING|%s", string);
	__send_msg(key, value);
}


int setting_dbus_handler_init(void* user_data)
{
	SETTING_TRACE_BEGIN;
	if (bus)
	{
		SETTING_TRACE("already get a bus, need release first.");
		setting_dbus_handler_fini();
//		return -1;
	}
	DBusError error;
	memset(&error, 0, sizeof(DBusError));
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0,};
	dbus_error_init(&error);
	bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	if (!bus)
	{
		SETTING_TRACE("Fail to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}

	dbus_connection_setup_with_g_main(bus, NULL);
	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);

	dbus_bus_add_match(bus, rule, &error);
	if (dbus_error_is_set(&error))
	{
		SETTING_TRACE("Fail to rule set; %s", error.message);
		dbus_bus_remove_match(bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(bus);
		bus = NULL;
		return -1;
	}

	if (dbus_connection_add_filter(bus, __signal_filter, user_data, NULL) == FALSE)
	{
		dbus_bus_remove_match(bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(bus);
		bus = NULL;
		return -1;
	}

	SETTING_TRACE("app signal initialized");
	return 0;
}

int setting_dbus_handler_fini(void)
{
	SETTING_TRACE_BEGIN;
	//do safty checking first.
	setting_retvm_if(!bus, 0, "!bus");
	DBusError error;
	memset(&error, 0, sizeof(DBusError));
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0, };

	dbus_error_init(&error);

	// why??
	bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);

	//dbus_connection_remove_filter(bus, __signal_filter, NULL);


	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);
	dbus_bus_remove_match(bus, rule, &error);

	if (dbus_error_is_set(&error))
	{
		SETTING_TRACE("Fail to rule unset: %s", error.message);
		dbus_error_free(&error);
		//return -1;
	}

	dbus_connection_close(bus);
	bus = NULL;
	SETTING_TRACE("app signal finalized");
	return 0;
}

/////////////////////////////
/////////////////////////////
/////////////////////////////

Elm_Genlist_Item_Class itc_layout;

// write -> update
static int __cfg_file_write(Draw_Data *pd)
{
	SETTING_TRACE_BEGIN;

	GError *error = NULL;
	if(pd->doc != NULL)
	{
		xmlSaveFormatFile(pd->cfg_file, pd->doc, 1);
		//xmlFreeDoc(pd->doc);
		//pd->doc = NULL;
		SETTING_TRACE("__cfg_file_write successful");
	}

	SETTING_TRACE_END;
	return TRUE;
}


static void ___click_softkey_back_cb(void *data, Evas_Object *obj,
					  void *event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(!data);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;
	if (pd->ly_main) {
		evas_object_del(pd->ly_main);
		pd->ly_main = NULL;
	}

	setting_plugin_destroy(node);
	if (g_main_ad->navibar_main)
	{
		elm_naviframe_item_pop(g_main_ad->navibar_main);
	}
	pd->scroller = NULL;
	pd->navi_bar = NULL;
	pd->cfg_file = NULL;




	pd->root = NULL;
}


static void* group_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	// original code is non-recursive
#if 0
	const char *title = (char*)json_object_get_string_member(jsonObj, "title");
	(void)setting_create_Gendial_field_titleItem(pd->scroller,
						     &(itc_group_item),
						     title, NULL);

	// non recursive method
	if (json_object_has_member(jsonObj, "elements"))
	{
		JsonNode* elements_node = json_object_get_member(jsonObj, "elements");
		int i;
		JsonObject* tempobj;
		char* type;
		for (i=0; i < json_array_get_length(json_node_get_array(elements_node)); i++)
		{
			tempobj  = json_array_get_object_element(json_node_get_array(elements_node), i);
			type = (char*)json_object_get_string_member(tempobj, "type");
			drawer_fp  fp = __drawer_find(type);
			if (fp) fp(pd, tempobj); // draw it
		}
	}
#else
	// new code is recursive
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	SETTING_TRACE (" >>> GROUP NAME : %s \n", title);
	(void)setting_create_Gendial_field_titleItem(pd->scroller, &(itc_group_item), title, NULL);
#endif

	return NULL;
};


static void* __link_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, NULL);
	retvm_if(event_info == NULL, NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	xmlNode* xmlObj = data;
	retv_if(!xmlObj, NULL);
	const char *link_file = (char*)xmlGetProp(xmlObj, "value");

	if(!link_file)
	{
		SETTING_TRACE_ERROR("Invalidate liked file");
		return NULL;
	}
	char file[1024] = {0,};
	snprintf(file, sizeof(file), "%s/%s", PLUGIN_CFG_DIR, link_file);
	SETTING_TRACE("file:%s", file);

	PluginNode* plugin_node = setting_plugin_create(file);
	setting_plugin_load(plugin_node, (const char *)file);

	//setting_plugin_load(NULL, file);

	return NULL;
}

static void* __launch_list_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retv_if(data == NULL, NULL);
	retvm_if(event_info == NULL, NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	xmlNode* xmlObj = data;
	retv_if(!xmlObj, NULL);
	const char *key_str = (char*)xmlGetProp(xmlObj, "id");
	const char *title_str = (char*)xmlGetProp(xmlObj, "title");
	const char *appid_str = (char*)xmlGetProp(xmlObj, "appid");
	const char *operation_str = (char*)xmlGetProp(xmlObj, "operation");


	service_h svc = NULL;
	service_create(&svc);
	service_set_app_id(svc, appid_str);  							// xml property – appid
	service_set_operation(svc, operation_str); 						// property : operation
	service_send_launch_request(svc, NULL, NULL);
	service_destroy(svc);

	return NULL;
}

static void* label_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);
	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	const char *title = (char*)xmlGetProp(xmlObj, "title");

	Setting_GenGroupItem_Data *obj =
		setting_create_Gendial_field_def(pd->scroller, &(itc_1text),
						 NULL,
						 xmlObj, SWALLOW_Type_INVALID, NULL, NULL,
						 0, title, NULL, NULL);

	return obj;
}

static void* link_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);
	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	const char *key_str = (char*)xmlGetProp(xmlObj, "id");
	Setting_GenGroupItem_Data * obj =
		setting_create_Gendial_field_def(pd->scroller, &(itc_1text),
						 __link_list_cb,
						 xmlObj, SWALLOW_Type_INVALID, NULL, NULL,
						 0, key_str, NULL, NULL);

	return (void*)obj;
};

static void* launch_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);
	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	const char *title_str = (char*)xmlGetProp(xmlObj, "title");

	Setting_GenGroupItem_Data * obj =
		setting_create_Gendial_field_def(pd->scroller, &(itc_1text),
						 __launch_list_cb,
						 xmlObj, SWALLOW_Type_INVALID, NULL, NULL,
						 0, title_str, NULL, NULL);

	return (void*)obj;
};

static void __slider_stop_cb(void *data, Evas_Object *obj,
				    void *event_info)
{
	ret_if(data == NULL || obj == NULL);
	double val = elm_slider_value_get(obj);
	SETTING_TRACE("val = %f", val);
	Setting_GenGroupItem_Data *list_item = data;
	ret_if(list_item->userdata == NULL);

	xmlNode* xmlObj = list_item->userdata;
	ret_if(!xmlObj);

	//apply the vconf changes after stopping moving slider..
	list_item->chk_status = (int)(val + 0.5);
	elm_slider_value_set(obj, list_item->chk_status);

	SETTING_TRACE(" slider after ---> val = %d", (int) list_item->chk_status);

	// int -> double -> xmlChar*
	xmlAttrPtr newattr;
	char buf[10];
	sprintf(buf, "%d", (int) list_item->chk_status);
	newattr = xmlSetProp(xmlObj, "value", buf);

	__send_int_msg(xmlObj, list_item->chk_status);
	__cfg_file_write((Draw_Data *)list_item->belongs_to);
}


static void* slider_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);
	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	// type casting
	const char *title = (char*)xmlGetProp(xmlObj, "title");

	SETTING_TRACE (" >>> [slider input] min=%s max=%s value=%s ",(char*)xmlGetProp(xmlObj, "min"), (char*)xmlGetProp(xmlObj, "max"), (char*)xmlGetProp(xmlObj, "value"));

	int value = atoi((char*)xmlGetProp(xmlObj, "value"));
	int min = atoi((char*)xmlGetProp(xmlObj, "min"));
	int max = atoi((char*)xmlGetProp(xmlObj, "max"));

	SETTING_TRACE ("[slider input] min=%d max=%d value=%d ", min, max, value);

	setting_create_Gendial_itc("dialogue/1text.1icon.5", &(itc_layout));
	Setting_GenGroupItem_Data *list_item =
	    setting_create_Gendial_field_def(pd->scroller, &(itc_layout), NULL,
					     NULL,
					     SWALLOW_Type_LAYOUT_SLIDER,
					     IMG_SENSITIVITY_ICON_01,
					     IMG_SENSITIVITY_ICON_02, value,
					     title, NULL, NULL);
	if (list_item) {
		list_item->win_main = NULL;
		list_item->evas = NULL;
		list_item->isIndicatorVisible = true;
		list_item->slider_min = min;
		list_item->slider_max = max;
		list_item->userdata = xmlObj;
		list_item->stop_change_cb = __slider_stop_cb;
		list_item->belongs_to = (int)pd;
	}

	g_list_item = list_item;

	return (void*)list_item;
};

#if 0
/*
  elm_object_item_data_set(item_to_update->item, item_to_update);
  elm_genlist_item_update(item_to_update->item);
*/
static void* navigationbar_func(void *data, xmlNode *xmlObj)
{
#if 1
	SETTING_TRACE_BEGIN;
	ret_if(!data || !xmlObj);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	//----------------------------------------------------------------
	// [DATA] title, btn[0], btn[1]
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	char *btn[2] = {0, };

	// find child nodes named 'elements'
	if (xmlObj->children) {
		xmlNode* cur = xmlObj->children;
		int i =0;
		while (cur != NULL)
		{
			if (!xmlStrcmp(cur->name, (const xmlChar*)"button")) {
				btn[i] = xmlGetProp(cur, "title");
				SETTING_TRACE("------>>> node type : Element, name=%s id=%s / btn[%d] = %s ",
								cur->name,xmlGetProp(cur, "id"),
								i,
								btn[i]);
				i++;
			}
			cur = cur->next;
		}
	}
	//----------------------------------------------------------------
	// [UI] with DATA
	pd->ly_main = setting_create_layout_navi_bar_genlist(pd->win_get,
						   pd->win_get,
						   _(title),
						   _(btn[1]), _(btn[0]),
						   ___click_softkey_back_cb,
						   ___click_softkey_back_cb, data, &pd->scroller,
						   &(pd->navi_bar));

#endif
	return NULL;
};
#endif

static void __check_mouse_up_cb(void *data, Evas_Object *obj,
					     void *event_info)
{
	/* error check */
	SETTING_TRACE_BEGIN;
	setting_retm_if(data == NULL, "Data parameter is NULL");

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));

	int old_status = elm_check_state_get(list_item->eo_check);
	list_item->chk_status = !old_status;
	elm_check_state_set(list_item->eo_check, list_item->chk_status);

	xmlNode *xmlObj = data;
	xmlAttrPtr newattr;
	newattr = xmlSetProp(xmlObj, "state", xmlXPathCastNumberToString(list_item->chk_status));

	__send_int_msg(xmlObj, list_item->chk_status);
	__cfg_file_write((Draw_Data *)list_item->belongs_to);
}


static void __chk_btn_cb(void *data, Evas_Object *obj,
					     void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) data;

	xmlNode* xmlObj = list_item->userdata;
	ret_if(!xmlObj);
	list_item->chk_status = elm_check_state_get(obj);	/*  for genlist update status */

	xmlAttrPtr newattr;
	if (list_item->chk_status == 1) {
		newattr = xmlSetProp(xmlObj, "value", "true");
	} else if (list_item->chk_status == 0) {
		newattr = xmlSetProp(xmlObj, "value", "false");
	} else {
		newattr = xmlSetProp(xmlObj, "value", "false");
	}

	const char *id = (char*)xmlGetProp(xmlObj, "id");
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	//SETTING_TRACE(" >>>> id:%s , title:%s", id, title);
	__send_int_msg(xmlObj, list_item->chk_status);
	__cfg_file_write((Draw_Data *)list_item->belongs_to);
	return;
}


static void* checkbox_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	// [DATA] title, value
	const char *title = (char*)xmlGetProp(xmlObj, "title");

	// true / false
	char* value = (char*)xmlGetProp(xmlObj, "value");

	int ival = -1;

	if ( 0 == safeStrCmp(value, "true")) {
		ival = 1;
	} else if ( 0 == safeStrCmp(value, "false"))  {
		ival = 0;
	} else {
		ival = 0; 	// default : false (0)
	}

	// title, value, xmlObj
	Setting_GenGroupItem_Data *list_item =
		setting_create_Gendial_field_def(pd->scroller,
					  &(itc_1text_1icon),
					  __check_mouse_up_cb,
					  xmlObj,
					  SWALLOW_Type_1TOGGLE,
					  NULL, NULL,
					  ival,
					  title,
					  NULL,
					  __chk_btn_cb);
	if(list_item) {
		list_item->userdata = xmlObj;
		list_item->belongs_to = (int) pd;
		SETTING_TRACE("pd:%p,list_item->belongs_to:%d", pd, list_item->belongs_to);
	}

	return list_item;
}


static void __entry_unfocus_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data || !obj, "Data parameter is NULL");

	setting_hide_input_pannel_cb(obj);
	const char *entry_str = elm_entry_entry_get(obj);
	char *entry_str_utf8 = NULL;
	entry_str_utf8 = elm_entry_markup_to_utf8(entry_str);

	Setting_GenGroupItem_Data *list_item = data;

	xmlNode* xmlObj = list_item->userdata;
	if(!xmlObj) {
		FREE(entry_str_utf8);
		return;
	}

	xmlAttrPtr newattr;
	const char *title = (char*)xmlSetProp(xmlObj, "value",entry_str_utf8);

	__send_string_msg(xmlObj, entry_str_utf8);
	__cfg_file_write((Draw_Data *)list_item->belongs_to);

	FREE(entry_str_utf8);
	SETTING_TRACE_END;
}


static void __editbox_list_cb(void *data, Evas_Object *obj,
					    void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */

	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);
	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);

	SETTING_TRACE("clicking item[%s]", _(list_item->keyStr));
	if (!elm_object_focus_get(list_item->eo_check)) {
		elm_object_focus_set(list_item->eo_check, EINA_TRUE);
	}

	Ecore_IMF_Context *imf_context = (Ecore_IMF_Context *)elm_entry_imf_context_get(list_item->eo_check);
	setting_retm_if(imf_context == NULL, "imf_context is NULL");
	ecore_imf_context_input_panel_show(imf_context);
}


static void __editbox_changed_cb(void *data, Evas_Object *obj,
				       void *event_info)
{
	SETTING_TRACE_BEGIN;
	retm_if(!data || !obj, "Data parameter is NULL");
	retm_if(!elm_object_focus_get(obj), "Entry is not focused");

	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;

	const char *entry_str = elm_entry_entry_get(obj);
	int entry_len = safeStrLen(entry_str);
	SETTING_TRACE("entry_str:[%s], lenght:%d", entry_str, entry_len);

	G_FREE(list_item->sub_desc);//release first
	list_item->sub_desc = (char *)g_strdup(entry_str);
}


/**
 *  editbox
 *
 * @see also __editbox_changed_cb
 * @see also __entry_unfocus_cb
 */
static void* editbox_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	const char *title = (char*)xmlGetProp(xmlObj, "title");
	const char *key_str= (char*)xmlGetProp(xmlObj, "value");

	const char *minlength= (char*)xmlGetProp(xmlObj, "minlength");
	const char *maxlength= (char*)xmlGetProp(xmlObj, "maxlength");

	int max_len = -1;
	if (maxlength)
	{
		max_len = atoi(maxlength);
		SETTING_TRACE(" >> MAXLENGTH FILTER IS AVAILABLE !!!! maxlength = %d", max_len);
	}

	Setting_GenGroupItem_Data *list_item = NULL;

#if 1
	if (max_len == -1)
	{
		// without maxlength filter
		list_item = setting_create_Gendial_field_def(pd->scroller, &(itc_1icon),
								 __editbox_list_cb,
							 pd, SWALLOW_Type_LAYOUT_ENTRY,
							 NULL, NULL, 0, title, key_str,
							 __editbox_changed_cb);

	} else {
		// add max length filter
		list_item = setting_create_Gendial_field_entry_fo(
									pd->scroller,
									&(itc_1icon),
									__editbox_list_cb,
									pd,
									SWALLOW_Type_LAYOUT_ENTRY,
									NULL, NULL, 0, title, key_str,
									__editbox_changed_cb,
									__entry_unfocus_cb,
									ELM_INPUT_PANEL_LAYOUT_NORMAL,
									false,
									false,
									max_len,
									0,
									NULL, NULL);

	}
#endif
	if (list_item) {
		list_item->userdata = xmlObj;
		list_item->stop_change_cb = __entry_unfocus_cb;
		list_item->belongs_to = (int)pd;
	}

	return list_item;
};


static void __expanditem_func_sel_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(event_info == NULL, "Invalid argument: event info is NULL");
	Elm_Object_Item *subitem = (Elm_Object_Item *) event_info;
	Elm_Object_Item *parentItem = elm_genlist_item_parent_get(subitem);
	elm_genlist_item_selected_set(subitem, 0);
	Setting_GenGroupItem_Data *data_subItem = elm_object_item_data_get(subitem);
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	ret_if(NULL == data_subItem || NULL == data_parentItem);

	elm_radio_value_set(data_subItem->rgd, data_subItem->chk_status);

	data_parentItem->sub_desc = (char *)g_strdup(_(data_subItem->keyStr));
	elm_object_item_data_set(data_parentItem->item, data_parentItem);
	elm_genlist_item_update(data_parentItem->item);

	xmlNode* xmlObj = data_parentItem->userdata;
	ret_if(!xmlObj);

	xmlAttrPtr newattr;
	//newattr = xmlSetProp(xmlObj, "string", data_parentItem->sub_desc);
	newattr = xmlSetProp(xmlObj, "value", data_parentItem->sub_desc);

	__send_string_msg(xmlObj, data_parentItem->sub_desc);
	__cfg_file_write((Draw_Data *)data_parentItem->belongs_to);
}


static void __expanditem_func_exp_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(NULL == data || NULL == event_info);
	SETTING_TRACE_BEGIN;

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	Elm_Object_Item *parentItem = event_info;	/* parent item */
	Setting_GenGroupItem_Data *data_parentItem = elm_object_item_data_get(parentItem);	/* parent data */
	Evas_Object *scroller = elm_object_item_widget_get(parentItem);


	xmlNode *xmlObj = data_parentItem->userdata;
	char *value = (char*)xmlGetProp(xmlObj, "value");
	SETTING_TRACE(">>> value = %s", value);
	Evas_Object *rgd = NULL;

	if (xmlObj->children && !data_parentItem->rgd) {//to protect from entering repeatly
		xmlNode* cur = xmlObj->children;

		rgd = elm_radio_add(scroller);
		elm_radio_value_set(rgd, -1);

		char *type;
		char *subitem_title = NULL;
		int subitem_index = 0;
		int sel_idx = -1;

#if 1
		// print out debug message
		if (data_parentItem->childs)
		{
			int howmany = 0;
			Eina_List *li = data_parentItem->childs;
			while(li)
			{
				Setting_GenGroupItem_Data* node = eina_list_data_get(li);
				howmany += 1;
				SETTING_TRACE(" <<< node->keyStr : %s >>> ", node->keyStr);

				// go next
				li = eina_list_next(li);
			}
			SETTING_TRACE(" <<<how many : %d>>> ", howmany);
		}
#endif
		if (data_parentItem->childs)
		{
			eina_list_free(data_parentItem->childs);
			data_parentItem->childs = NULL;
		}

		while (cur != NULL) {
			if (!xmlStrcmp(cur->name, (const xmlChar*)"expanditem")) {
				type = (char*)xmlGetProp(cur, "type");
				if (0 == safeStrCmp(type, "radio")) {
					subitem_title = (char*)xmlGetProp(cur, "title");
					Setting_GenGroupItem_Data *list_item =
											setting_create_Gendial_exp_sub_field(scroller,
													&(itc_1icon_1text_sub),
													__expanditem_func_sel_cb, NULL, parentItem,
													SWALLOW_Type_1RADIO, rgd,
													subitem_index,
													subitem_title, NULL);


					if (0 == safeStrCmp(value, subitem_title)) {
						sel_idx = subitem_index;
						SETTING_TRACE("%d is selected in Radio Group", sel_idx);
					}
					SETTING_TRACE(" eina list add >>> value = %s, subitem_title = %s ", value, subitem_title);
					data_parentItem->childs = eina_list_append(data_parentItem->childs, list_item);
					subitem_index++;

				} else {
					SETTING_TRACE("invalid type[:%s]", type);
				}
			}
			cur = cur->next;
		}

		// value set
		elm_radio_value_set(rgd, sel_idx);
		data_parentItem->rgd = rgd;//protecting condition
	}
}


static void __expanditem_func_smart_cb(void *data, Evas_Object *obj, void *event_info)
{
	ret_if(data == NULL || event_info == NULL);
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	Setting_GenGroupItem_Data *data_item = elm_object_item_data_get(item);
	char *cb_type = data;

	if (0 == safeStrCmp(cb_type, "contracted")) {
		data_item->rgd = NULL;
		elm_genlist_item_subitems_clear(item);
	}
}


static void* settings_func(void *data, xmlNode *xmlObj)
{
	return NULL;
}


static Evas_Object *setting_create_win_layout2(Evas_Object *win_layout, Evas_Object *win_obj)
{

    Evas_Object *layout = NULL;

    /*  Base Layout */
    layout = elm_layout_add(win_obj);
    setting_retvm_if(layout == NULL, FALSE, "layout == NULL");

    elm_layout_theme_set(layout, "layout", "application", "default");
    evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(layout, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_win_resize_object_add(win_obj, layout);

    Evas_Object *bg = setting_create_bg(layout, win_obj, "group_list");
    elm_object_part_content_set(layout, "elm.swallow.bg", bg);
    evas_object_show(layout);
    return layout;
}


Evas_Object *setting_create_layout_navi_bar2(Evas_Object *win_layout, Evas_Object *win_obj, char *title_str, char *lbutton_str, char *rbutton_str, char *mbutton_str, setting_call_back_func lbutton_click_cb, setting_call_back_func rbutton_click_cb, setting_call_back_func mbutton_click_cb, void *cb_data, Evas_Object *eo_view,	/*any container obj constructed on any evas obj */
					    Evas_Object **navi_bar,
					    Evas_Object **titleobj)
{

	Evas_Object *layout = setting_create_win_layout2(win_layout, win_obj);
	*navi_bar = setting_create_navi_bar(layout);

	setting_create_navi_bar_buttons(title_str,
					lbutton_str, rbutton_str, mbutton_str,
					lbutton_click_cb, rbutton_click_cb,
					mbutton_click_cb, cb_data, eo_view /*content */ ,
					*navi_bar, NULL);
	return layout;
}

static void __plugin_genlist_unrealized_cb(void* data,Evas_Object* obj, void* event_info)
{
	SETTING_TRACE_BEGIN;
	ret_if(data == NULL);

	Elm_Object_Item *item = (Elm_Object_Item*)event_info;
	Setting_GenGroupItem_Data *node = (Setting_GenGroupItem_Data *)elm_object_item_data_get(item);
	ret_if(node== NULL);

	SETTING_TRACE("keyStr : %s ", node->keyStr);
	//item_to_update->childs)

	if (node->childs)
	{
		eina_list_free(node->childs);
		node->childs = NULL;
	}
}

// <setting>
static void* setting_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);
#if 1
	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	// [DATA] title, btn[0], btn[1]
	const char *title = (char*)xmlGetProp(xmlObj, "title");
	char *btn[2] = {/* 0 */"OK", /* 1 */"NO"};

	SETTING_TRACE("before setting_create_layout_navi_bar_genlist");

	// [UI] with DATA
	pd->scroller = elm_genlist_add(g_main_ad->win_main);
	retvm_if(pd->scroller == NULL, NULL,
		 "Cannot set scroller object  as contento of layout");
	elm_object_style_set(pd->scroller, "dialogue");
	elm_genlist_clear(pd->scroller);	/* first to clear list */

	evas_object_smart_callback_add(pd->scroller, "unrealized", __plugin_genlist_unrealized_cb, node);


	SETTING_TRACE("_(title):%s", _(title));

	setting_push_layout_navi_bar(
					   _(title),
					   _("IDS_COM_BODY_BACK"), NULL, NULL,
					   ___click_softkey_back_cb,
					   NULL, NULL,
					   data, pd->scroller,
					   g_main_ad->navibar_main, NULL);
	SETTING_TRACE("after setting_create_layout_navi_bar_genlist");
#endif
	return NULL;
}


static void* expanditem_func(void *data, xmlNode *xmlObj)
{
	// DO NOTHING - expandlist draws this area
	return NULL;
}

static void* expandlist_func(void *data, xmlNode *xmlObj)
{
	SETTING_TRACE_BEGIN;
	retv_if(!data || !xmlObj, NULL);

	PluginNode* node = (PluginNode*)data;
	Draw_Data *pd = node->pd;

	const char *key_str = (char*)xmlGetProp(xmlObj, "title");
	const char *value = (char*)xmlGetProp(xmlObj, "value");	// string -> value

	setting_enable_expandable_genlist(pd->scroller, data, __expanditem_func_exp_cb, __expanditem_func_smart_cb);
	Setting_GenGroupItem_Data *list_item =
	    setting_create_Gendial_exp_parent_field(pd->scroller,
						    &(itc_2text_3_parent),
						    NULL, NULL,
						    SWALLOW_Type_INVALID,
						    key_str,
						    value);
	if (list_item) {
		list_item->userdata = xmlObj;
		list_item->belongs_to = (int)pd;
		list_item->childs = NULL; 		/* init */
	}

	return list_item;
}



static int __node_walker(PluginNode* context, xmlNode* cur)
{
	//SETTING_TRACE_BEGIN;
	Draw_Data *pd = context->pd;

	retv_if(!pd, -1);
	retv_if(!context->ui_list, -1);

	xmlNode *cur_node = NULL;
	for (cur_node = cur; cur_node;cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {
			// SETTING_TRACE("node type : %s id= %s", cur_node->name,xmlGetProp(cur_node, "id"));
			drawer_fp  fp = __drawer_find(cur_node->name);
			if (fp)
			{
				// type check
				void* vret = fp(context, cur_node); // draw it

				if (vret)
				{
					Setting_GenGroupItem_Data* genlist_node = (Setting_GenGroupItem_Data* )vret;
					//SETTING_TRACE("add node to Eina List name : %s, id : ", cur_node->name, xmlGetProp(cur_node, "id"));
					// add it to the hash table create before.
					// id, object
					// add list
					char* key_name = xmlGetProp(cur_node, "id");
					eina_hash_add(context->ui_list, strdup(key_name),(void*)genlist_node);
				}
			}else {
				SETTING_TRACE(" >>>> fp is NULL ");
			}
		}
		__node_walker(context, cur_node->children); 	/* RECURSIVE */
	}
	return 0;
}

/**
 * @param id_str [in] "id"
 * @param value [in] value to be udpated
 * @see __expanditem_func_sel_cb
 */
static int __node_finder(PluginNode* context, xmlNode* cur, char* id_str, char* value, bool* is_end)
{
	SETTING_TRACE_BEGIN;
	xmlNode *cur_node = NULL;

	if (! context)
	{
		SETTING_TRACE("context is NULL - it's error CONDITION ");
		return -1;
	}

	retv_if(!context->ui_list, -1);
	if (*is_end == true) return 0;

	for (cur_node = cur; cur_node;cur_node = cur_node->next) {
		if (cur_node->type == XML_ELEMENT_NODE) {

			char* id_name = (char*)xmlGetProp(cur_node, "id");
			if ( id_name && 0 == strcmp(id_str, id_name))
			{
				SETTING_TRACE("FOUND >>>> %s", id_name);
				// cur_node	- update xml code
				xmlAttrPtr newattr = xmlSetProp(cur_node, "value", value);

				//-----------------------------------------------------------
				// UI UPDATE
				// case : slider
				if ( 0 == strcmp (cur_node->name, "integer"))
				{
					SETTING_TRACE(">>>>> UPDATE SLIDER CONTROL %x --- %s ",context->ui_list, id_name);
					Setting_GenGroupItem_Data* item_to_update = (Setting_GenGroupItem_Data*)eina_hash_find(context->ui_list, id_name);
					if 	(item_to_update)
					{
						item_to_update->chk_status = atoi(value);
						SETTING_TRACE(">>>   o-------------0 SLIDER VALUE = %d ", item_to_update->chk_status);

						elm_object_item_data_set(item_to_update->item, item_to_update);
						elm_genlist_item_update(item_to_update->item);
					} else {
						SETTING_TRACE("item_to_update is NULL");
					}
				}
				// case : toggle
				if ( 0 == strcmp (cur_node->name, "bool"))
				{
					SETTING_TRACE(">>>>> UPDATE TOGGLE CONTROL pluginpath:%s, ----  %x --- %s ", context->plugin_path, context->ui_list, id_name);
					Setting_GenGroupItem_Data* item_to_update = NULL;
					item_to_update = (Setting_GenGroupItem_Data*)eina_hash_find(context->ui_list, id_name);
					if 	(item_to_update)
					{
						item_to_update->chk_status = atoi(value);
						SETTING_TRACE(">>>   o-------------0 TOGGLE VALUE = %d ", item_to_update->chk_status);

						elm_object_item_data_set(item_to_update->item, item_to_update);
						elm_genlist_item_update(item_to_update->item);
					} else {
						SETTING_TRACE("item_to_update is NULL");
					}
				}
				// case : edit control
				if ( 0 == strcmp (cur_node->name, "string"))
				{
					SETTING_TRACE(">>>>> UPDATE EDIT CONTROL CONTROL %x --- %s ",context->ui_list, id_name);
					Setting_GenGroupItem_Data* item_to_update = (Setting_GenGroupItem_Data*)eina_hash_find(context->ui_list, id_name);
					if 	(item_to_update)
					{
						char* old_string = item_to_update->sub_desc;
						item_to_update->sub_desc = strdup(value);
						SETTING_TRACE(">>>   o-------------0 STRING VALUE = %s ", value);

						// free old string

						elm_object_item_data_set(item_to_update->item, item_to_update);
						elm_genlist_item_update(item_to_update->item);

					} else {
						SETTING_TRACE("item_to_update is NULL");
					}
				}
				// case : expand list
				// parent
				//    child1
				//    child2
				//    child3
				//    child4
				//-----------------------------------------------------------
				if ( 0 == strcmp (cur_node->name, "expandlist"))
				{
					#if 1
                    SETTING_TRACE(">>>>> UPDATE EXPAND LIST CONTROL %x --- %s ",context->ui_list, id_name);
                    Setting_GenGroupItem_Data* item_to_update = NULL;
					item_to_update = (Setting_GenGroupItem_Data*)eina_hash_find(context->ui_list, id_name);
                    if  (item_to_update)
                    {
                        char* old_string = item_to_update->sub_desc;
                        item_to_update->sub_desc = strdup(value);
                        SETTING_TRACE(">>>   o-------------0 EXPAND LIST VALUE = %s ", value);

                        // free old string
                        // string update
                        elm_object_item_data_set(item_to_update->item, item_to_update);
                        elm_genlist_item_update(item_to_update->item);

                        // TODO: need to update child elements
                        // item_to_update->childs ---> expanded list

						// debug log
						// there should have 4 sub items.
						//-----------------------------------------------------
						// has it already freed by someone?
						//-----------------------------------------------------
						if (item_to_update->childs)
						{
							int howmany2 =0;
							Eina_List *li = item_to_update->childs;
							while(li)
							{
								Setting_GenGroupItem_Data* node = eina_list_data_get(li);
								howmany2 += 1;
								li = eina_list_next(li);
							}
							SETTING_TRACE(" expandlist -- how many items are there? : %d ", howmany2);
						}

                        if (item_to_update->childs)
                        {
                            Eina_List *li = item_to_update->childs;
                            int radio_index = 0;
                            while(li)
                            {
                                Setting_GenGroupItem_Data* node = eina_list_data_get(li);

                                if (node->item && node->keyStr)
                                {
                                    // do something more
                                    // SETTING_TRACE(">>> RADIO LIST STRING VALUE = %s ", node->keyStr);
                                    // set position of radio button
                                    if (strcmp(node->keyStr, value) == 0) // << CRAAH !!!!
                                    {
                                        elm_radio_value_set(node->rgd, radio_index);
                                    }
                                } else {
                                     SETTING_TRACE(">>> node has unproper values - Setting_GenGroupItem_Data");
                                }
                                elm_object_item_data_set(item_to_update->item, item_to_update);
                                elm_genlist_item_update(item_to_update->item);

                                // go next
                                li = eina_list_next(li);
                                radio_index++;
                            }

                        }
                    } else {
                        SETTING_TRACE("item_to_update is NULL");
                    }


					#endif
				}
				*is_end = true;
			}
		}
		__node_finder(context, cur_node->children, id_str, value, is_end); 	/* RECURSIVE */
	}
	return 0;
}

static unsigned int _plugin_string_key_length(const char*key)
{
	if (!key)
		return 0;

	return (int)strlen(key) + 1;
}

static int _plugin_string_key_cmp(const char* key1, int key1_length,
									const char* key2, int key2_length)
{
	return strcmp(key1, key2);
}

static void _plugin_entry_free_cb(void* data)
{
    Setting_GenGroupItem_Data* node = (Setting_GenGroupItem_Data*) data;
	if (node->childs)
	{
		eina_list_free(node->childs);
		node->childs = NULL;
	}
}

PluginNode* setting_plugin_create()
{
	PluginNode *node = calloc(1, sizeof(PluginNode));
	setting_retvm_if(!node, -1, "Create PluginNode obj failed");
	Draw_Data *pd = calloc(1, sizeof(Draw_Data));


	if (!pd) {
		FREE(node);
	}

	setting_retvm_if(!pd, -1, "Create Draw_Data obj failed");
	//eina_init();

	node->pd = pd;
	node->ui_list = eina_hash_new(EINA_KEY_LENGTH(_plugin_string_key_length),
								  EINA_KEY_CMP(_plugin_string_key_cmp),
								  EINA_KEY_HASH(eina_hash_superfast),
								  _plugin_entry_free_cb,
								  5); // 2^5

	return node;
}

void setting_plugin_destroy(PluginNode* node)
{
	SETTING_TRACE_BEGIN;
	if (node) {
		SETTING_TRACE("node is NOT NULL")
		if (node->pd) {
			SETTING_TRACE("node->pd is NOT NULL")
			if(node->pd->doc != NULL) {
				xmlSaveFormatFile(node->plugin_path, node->pd->doc, 1);
				xmlFreeDoc(node->pd->doc);
				node->pd->doc = NULL;
				SETTING_TRACE("__cfg_file_write successful");
			}

			free(node->pd);
			node->pd = NULL;
		}

		if (node->ui_list) {
			eina_hash_free(node->ui_list);
			node->ui_list = NULL;
		}
		free(node);
		node = NULL;
//		PLUGIN_FINI;
	}
}


static Eina_Bool _plugin_foreach_cb(const Eina_Hash *hash, const void*key, void* data, void* fdata)
{
    Setting_GenGroupItem_Data* node = (Setting_GenGroupItem_Data*) data;
	SETTING_TRACE("%s --- %s ", (char*)key, node->keyStr);
	return EINA_TRUE;
}


void setting_plugin_debug(PluginNode* context)
{
//	SETTING_TRACE("HASH TABLE -------------------------------------");
	if (context->ui_list) {
		eina_hash_foreach(context->ui_list,_plugin_foreach_cb, NULL);
	}
//	SETTING_TRACE("HASH TABLE -------------------------------------");

}

void setting_plugin_update(PluginNode* context)
{
    Eina_List *li = context->ui_list;

    while (li) {
        Setting_GenGroupItem_Data* node = (Setting_GenGroupItem_Data*) eina_list_data_get(li);
        if (node) {
            // SETTING_TRACE(" ---> keyStr : %s , swallow type : %d  ", node->keyStr, node->swallow_type);
			Setting_GenGroupItem_Data* item_to_update = NULL;
			item_to_update = node;
			elm_object_item_data_set(item_to_update->item, item_to_update);
			elm_genlist_item_update(item_to_update->item);
        }
        li = eina_list_next(li);
	}
}


/**
 * set modifiable
 */
bool setting_plugin_load(PluginNode* context, const char *cfg_file)
{
	SETTING_TRACE("cfg_file:%s", cfg_file)
	if (isEmptyStr(cfg_file) || 0 != access(cfg_file, R_OK|W_OK|F_OK ))
	{
		SETTING_TRACE_ERROR(" error occured : access \n");
		return FALSE;
	}
	context->pd->cfg_file = cfg_file;

	//GError *error = NULL;

	context->pd->doc = xmlParseFile(cfg_file);
	context->pd->root = xmlDocGetRootElement(context->pd->doc);

	// signal filter change
	//setting_dbus_handler_init(g_main_ad);
	void* user_data = (void*)g_main_ad;

	DBusError error;
	dbus_error_init(&error);
	char rule[MAX_LOCAL_BUFSIZE + 1] = {0,};
	bus = dbus_bus_get_private(DBUS_BUS_SYSTEM, &error);
	if (!bus)
	{
		SETTING_TRACE("Fail to connect to the D-BUS daemon: %s", error.message);
		dbus_error_free(&error);
		return -1;
	}

	// remove filter
	// dbus_connection_remove_filter(bus, __signal_filter, NULL);

	// get rule
	snprintf(rule, MAX_LOCAL_BUFSIZE, "path='%s',type='signal',interface='%s'", DBUS_PATH, DBUS_SIGNAL_INTERFACE);
	dbus_bus_add_match(bus, rule, &error);
	if (dbus_error_is_set(&error))
	{
		SETTING_TRACE("Fail to rule set; %s", error.message);
		dbus_bus_remove_match(bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(bus);
		bus = NULL;
		return -1;
	}

	// re-add filter
	if (dbus_connection_add_filter(bus, __signal_filter, user_data, NULL) == FALSE)
	{
		dbus_bus_remove_match(bus, rule, &error);
		dbus_error_free(&error);
		dbus_connection_close(bus);
		bus = NULL;
		return -1;
	}

	// TODO: error handler here
	__node_walker(context, context->pd->root);

	// debug message
	setting_plugin_debug(context);
	return true;
}


