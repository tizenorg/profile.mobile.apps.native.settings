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


#include <setting-common-search.h>
#include <unistd.h>
#include <eina_list.h>
#include <errno.h>
#include <glib.h>

#include <setting-debug.h>
#include <setting-cfg.h>
#include <setting-common-general-func.h>
#include <setting-common-draw-widget.h>

#include <dlfcn.h>

#define BUF_SIZE 512

/* search genlist */
static char *__search_label_get(void *data, Evas_Object *obj, const char *part);
static char *___add_tag_for_search(char *target, char *searcher);

static Evas_Object *__search_icon_get(void *data, Evas_Object *obj, const char *part);
static void __search_item_del(void *data, Evas_Object *obj);
static Evas_Object *__no_result_icon_get(void *data, Evas_Object *obj, const char *part);
/**
 * [UI] search bar
 * plain list item style
 */

EXPORT_PUBLIC
const Elm_Genlist_Item_Class itc_no_reult = {
	.item_style = "1icon/with_no_line",
	.func.text_get = NULL,
	.func.content_get = __no_result_icon_get,
	.func.state_get = NULL,
	.func.del = __search_item_del,
};

#define DEFINE_ITC2(style, name) \
	EXPORT_PUBLIC const Elm_Genlist_Item_Class name = {\
	                                                   .item_style = style,\
	                                                   .func.text_get = __search_label_get,\
	                                                   .func.content_get = __search_icon_get,\
	                                                   .func.state_get = NULL,\
	                                                   .func.del = __search_item_del,\
	                                                  };

DEFINE_ITC2("dialogue/1text.1icon.3.tb", g_search_genlist_ts);
DEFINE_ITC2("dialogue/newset.1text.2icon.4.tb", g_search_genlist_ts_toggle);
DEFINE_ITC2("dialogue/newset.1text.2icon.4.divider.tb", g_search_genlist_ts_divided_toggle);

static
char *___add_tag_for_search(char *target, char *searcher)
{
	const char *p = NULL;
	const char *pre = NULL;
	char buf[BUF_SIZE] = {0,};
	char str_tag_added[BUF_SIZE] = {0,};

	pre = target;
	p = (const char *)strcasestr(pre, searcher);
	if (p != NULL) {
		/* append characters before matched string */
		if (p != pre) {
			buf[0] = '\0';
			strncat(buf, pre, p - pre);
			sncat(str_tag_added, BUF_SIZE, buf);
		}
		/* highlight str */
		sncat(str_tag_added, BUF_SIZE, "<match>");
		buf[0] = '\0';
		strncat(buf, p, strlen(searcher));
		sncat(str_tag_added, BUF_SIZE, buf);
		sncat(str_tag_added, BUF_SIZE, "</match>");
		/* set pointer after matched string */
		pre = p + strlen(searcher);
	}
	sncat(str_tag_added, BUF_SIZE, pre);
	return strdup(str_tag_added);
}





/**
 * Callback function used for get label as member in genlist item
 *
 * @param[in]  data   data used for this function
 * @param[in]  obj    object which caused this function
 * @param[in]  part   part name of the label member in genlist item
 * @return     pointer to content of label
 */
static
char *__search_label_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	/*SETTING_TRACE("part : %s", part); */
	ListItemObject *item = (ListItemObject *)data;

	char buf[BUF_SIZE] = "";

	char *tag_str = NULL;

	retv_if(NULL == part, NULL);

	if (!strcmp(part, "elm.text.1")) {
		snprintf(buf, BUF_SIZE, "%s", "elm.text.1");        /* print out test message */
	} else if (!strcmp(part, "elm.text.2")) {
		snprintf(buf, BUF_SIZE, "%s", "elm.text.2");        /* print out test message */
	} else if (!strcmp(part, "elm.text.3")) {
		snprintf(buf, BUF_SIZE, "%s", "elm.text.3");        /* print out test message */
	} else if (!strcmp(part, "elm.text")) {
#if 0
		/*snprintf(buf, BUF_SIZE, "[%s] - %s", item->type, item->title);   // print out "[type] - title" */
		snprintf(buf, BUF_SIZE, "%s", item->title);   /* print out "[type] - title" */
		return strdup(buf);
#else
		/*tag_str = ___add_tag_for_search(item->title, g_ad->search_text); */
		tag_str = ___add_tag_for_search(item->title, "");
		snprintf(buf, BUF_SIZE, "%s", tag_str);   /* print out "[type] - title" */
		FREE(tag_str);
		return strdup(buf);
#endif
	}
	FREE(tag_str);
	return strdup(buf);
}

/*--------------------------------------------------- */
/* wifi -> go to wifi after calling the set_state */
/* mock function */
/* do nothing */
/*--------------------------------------------------- */
static void ___toggle_chg_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;

}

static Evas_Object *__no_result_icon_get(void *data, Evas_Object *obj, const char *part)
{
	SETTING_TRACE_BEGIN;
	setting_retvm_if(!data || !obj, NULL, "!data || !obj");

	if (!safeStrCmp(part, "elm.icon")) {
		Evas_Object *box = elm_box_add(obj);

		int rot = elm_win_rotation_get(obj);
		SETTING_TRACE_DEBUG("....change_ang:%d", rot);
		if (APP_DEVICE_ORIENTATION_0 == rot
		    || APP_DEVICE_ORIENTATION_180 == rot) {
			/*b.separator.. */
			Evas_Object *separator = setting_create_blank_rect_customize(box, 1, 270);
			elm_box_pack_end(box, separator);
		}

		Evas_Object *sub_layout = elm_layout_add(obj);
		elm_layout_theme_set(sub_layout, "layout", "nocontents", "search");
		evas_object_size_hint_weight_set(sub_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
		evas_object_size_hint_align_set(sub_layout, EVAS_HINT_FILL, EVAS_HINT_FILL);

		elm_object_part_text_set(sub_layout, "elm.text", _("IDS_ST_BODY_NO_RESULTS_FOUND"));
		setting_resize_object(sub_layout, 500, 500);
		evas_object_show(sub_layout);
		elm_box_pack_end(box, sub_layout);
		return box;
	}

	return NULL;
}

/**
 * Get icon of given part in genlist item
 *
 * @param[in]  data   Data used in this function
 * @param[in]  obj    Evas_Object who emit this event
 * @param[in]  part   Name of the part in genlist item
 *
 * @return     NULL if meet error
 *             Pointer to new icon
 */
static
Evas_Object *__search_icon_get(void *data, Evas_Object *obj, const char *part)
{
	/*SETTING_TRACE_BEGIN; */
	retv_if(NULL == part, NULL);
	ListItemObject *item = (ListItemObject *)data;

	SETTING_TRACE(" genlist :: part : (%s) ", part);

	retv_if(!item || !obj || !item->title, NULL);
	if (!safeStrCmp(part, "elm.icon") || !safeStrCmp(part, "elm.icon.1")) {
		Evas_Object *icon = elm_icon_add(obj);

		char *icon_path = NULL;
		icon_path = IMG_SETTING;
		if (item->icon_path) {
			if (safeStrCmp(item->icon_path, "") == 0)
				icon_path = IMG_SETTING;
			else
				icon_path = item->icon_path;
		}
		/*SET//TING_TRACE("item->title:%s", item->title); */
		/*SETTING_TRACE("item->title2:%s", item->title2);	// plain text */
		/*SETTING_TRACE("item->iconpath:%s", icon_path);	// plain text */
		elm_image_file_set(icon, icon_path, NULL);
		evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

		Evas_Object *ly_outer = elm_layout_add(obj);
		elm_layout_file_set(ly_outer, SETTING_THEME_EDJ_NAME, "nocontents");
		evas_object_show(ly_outer);

		evas_object_size_hint_min_set(icon, ELM_SCALE_SIZE(63), ELM_SCALE_SIZE(63));
		evas_object_size_hint_max_set(icon, 63, 63);
		elm_object_part_content_set(ly_outer, "contents", icon);

		/*return icon; */
		return ly_outer;

	} else if (!safeStrCmp(part, "elm.icon.2")) {
		Evas_Object *toggle = elm_check_add(obj);

		if (item->fp_check_btn_cb)
			evas_object_smart_callback_add(toggle, "changed", item->fp_check_btn_cb, item);
		else
			evas_object_smart_callback_add(toggle, "changed", ___toggle_chg_cb, item);

		/* get state------------------------------------- */
		/* if it is the 1'st depth item, get the pnode */
		/* display UG need to get the pointer */

		Setting_Cfg_Node_T *pnode = get_cfg_node_by_keystr(item->title2);

		Cfg_Item_State state = Cfg_Item_Error;
		if (pnode && pnode->tfunc && pnode->tfunc->get_item_state) {
			pnode->tfunc->get_item_state(&state, NULL);
		} else if (!pnode && item->viewname) {
			/* if get_cfg_node_by_keystr returns NULL, search for UG's keyword */
			int ret =  setting_search_get_module_item_state(item->title2, item->viewname, &state, NULL);
			SETTING_TRACE("---------------------------------------");
			SETTING_TRACE(" >>>  ret : %d, state : %d <<< ", ret, state);
			SETTING_TRACE("---------------------------------------");
		}

		if (state == Cfg_Item_On)
			elm_check_state_set(toggle, EINA_TRUE);
		else
			elm_check_state_set(toggle, EINA_FALSE);
		/* get state------------------------------------- */

		evas_object_propagate_events_set(toggle, 0);
		elm_object_style_set(toggle, "on&off");

		evas_object_pass_events_set(toggle, 1);

		evas_object_show(toggle);
		return toggle;
	}

	return NULL;
}

/**
 * [UI] search bar
 * used for genlist item style - callback function destroying an item of list
 */
static
void __search_item_del(void *data, Evas_Object *obj)
{
	/*SETTING_TRACE_BEGIN; */
	ListItemObject *item = (ListItemObject *)data;
	ret_if(!item);
	FREE(item->title);
	FREE(item->type);
	FREE(item->infostr);
	FREE(item->viewname);
	FREE(item->fieldname);
	FREE(item->icon_path);
	__BACK_POINTER_UNSET(item); /*for pointer safety */
	FREE(item);
}



static void sql_stmt(sqlite3 *db, const char *stmt)
{
	char *errmsg;
	int   ret;

	ret = sqlite3_exec(db, stmt, 0, 0, &errmsg);
	if (ret != SQLITE_OK) {
		SETTING_TRACE("Error in statement: %s [%s].", stmt, errmsg);
	} else if (ret == SQLITE_OK) {
		SETTING_TRACE("OK running : (%s) --- [%s].", stmt, errmsg);
	}
}

EXPORT_PUBLIC
int setting_searchdb_open(sqlite3 **ppdb)
{
	SETTING_TRACE_BEGIN;
	/*char *zErrMsg = NULL; */
	int rc;
	/*char query[512]; */
	rc = sqlite3_open(SEARCH_DB_PATH, ppdb);
	if (rc) {
		SETTING_TRACE_ERROR("Can't open database: %s", sqlite3_errmsg(*ppdb));
		sqlite3_close(*ppdb);
		return (-1);
	} else {
		SETTING_TRACE("open database - OK - %d ", *ppdb);
	}

	return (0);
}

EXPORT_PUBLIC
void setting_searchdb_close(sqlite3 *db)
{
	SETTING_TRACE_BEGIN;
	sqlite3_close(db);
}

EXPORT_PUBLIC
void setting_add_db_search_clean(sqlite3 *db)
{
	SETTING_TRACE_BEGIN;
	sql_stmt(db, "BEGIN");
	char *query = "DELETE FROM setting_search";

	sql_stmt(db, query);
	sql_stmt(db, "COMMIT");
}


/**
* @brief add index to DB
*
* @param db
* @param locale	locale name (ko_KR, en_US ...)
* @param keystr string name (ex. Wi-Fi)
* @param infostr UG path, memta info, URL
* @param data_type 0, 1, 2, 3, 4 ... type
* @param viewname ""
* @param fieldname ""
* @param icon_path ""
*/
EXPORT_PUBLIC
void setting_add_db_search_index(sqlite3 *db, char *locale, char *keystr, char *infostr, int data_type, char *viewname, char *fieldname, char *icon_path)
{
	SETTING_TRACE_BEGIN;
	sql_stmt(db, "BEGIN");
	/*char query[1024]; */

	/*snprintf(query, sizeof(query), "INSERT INTO setting_search VALUES (%Q, %Q, %Q, %d, NULL, %Q, %Q, %Q)", */
	/*			locale, fieldname, viewname, data_type,	keystr,	infostr, icon_path); */
	char *query = sqlite3_mprintf("INSERT INTO setting_search VALUES (%Q, %Q, %Q, %d, NULL, %Q, %Q, %Q)",
	                              locale, fieldname, viewname, data_type, keystr, infostr, icon_path);
	sql_stmt(db, query);
	sqlite3_free(query);
	query = NULL;
	sql_stmt(db, "COMMIT");
}

#if 1
EXPORT_PUBLIC
int setting_invoke_search_db_function(char *ug_name, app_control_h service, void *ext, char **ppapplocale)
{
	SETTING_TRACE("Enter %s with ug_name:%s", __FUNCTION__, ug_name);
	int (*search_plugin)(app_control_h pair, void **ptr, char **applocale);
	int ret = OPERATE_LIB_SUCESS;

	/*1.first do exist-checking in /opt/ug/lib */
	char ug_file[PATH_MAX + 1];
	snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH, ug_name);
	struct stat st;
	if (stat(ug_file, &st) != 0) {

		/*2.if it does not exit in /opt/ug/lib, then do exist-checking in /usr/ug/lib */
		memset(ug_file, 0x00, PATH_MAX + 1);
		snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH_USR, ug_name);
		if (stat(ug_file, &st) != 0) {
			/*both not exist,skip it */
			SETTING_TRACE_ERROR(" libug-%s.so is *NOT* present, so skip it..\n", ug_name);
			return OPERATE_LIB_SUCESS;
		}
	}
	/*------------------------------------------------------------------------------------- */
	void *handle = dlopen(ug_file, RTLD_LAZY);
	if (!handle) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", dlerror());
		return LOAD_LIB_FAILED;
	}

	char *error = NULL;
	search_plugin = dlsym(handle, "setting_plugin_search_init");
	if ((error = dlerror()) != NULL) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", error);
		dlclose(handle);
		return UNDEFINED_LIB_SYMBOL;
	}

	if (search_plugin) {
		ret = (*search_plugin)(service, ext, ppapplocale);    /*  CALL */
		SETTING_TRACE(" after calling PLUGIN WITH -APP LOCALE ID = %s", *ppapplocale);
	}
	if (ret < 0) ret += UNDEFINED_LIB_SYMBOL;/*from -2 on */

	dlclose(handle);
	return ret;
}
#endif


EXPORT_PUBLIC
int setting_search_set_module_item_state(char *keyword, char *ug_name, Cfg_Item_State item_state, cfg_func_table *pops)
{
	SETTING_TRACE("Enter %s with ug_name:%s", __FUNCTION__, ug_name);
	/* int setting_plugin_search_query_ops(char* str_id, void** tfunc_obj) */
	int (*query_ops)(char *str_id, void **tfunc_obj);
	int ret = OPERATE_LIB_SUCESS;

	/*1.first do exist-checking in /opt/ug/lib */
	char ug_file[PATH_MAX + 1];
	snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH, ug_name);
	struct stat st;
	if (stat(ug_file, &st) != 0) {

		/*2.if it does not exit in /opt/ug/lib, then do exist-checking in /usr/ug/lib */
		memset(ug_file, 0x00, PATH_MAX + 1);
		snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH_USR, ug_name);
		if (stat(ug_file, &st) != 0) {
			/*both not exist,skip it */
			SETTING_TRACE_ERROR(" libug-%s.so is *NOT* present, so skip it..\n", ug_name);
			return OPERATE_LIB_SUCESS;
		}
	}
	/*------------------------------------------------------------------------------------- */
	void *handle = dlopen(ug_file, RTLD_LAZY);
	if (!handle) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", dlerror());
		return LOAD_LIB_FAILED;
	}

	char *error = NULL;
	query_ops = dlsym(handle, "setting_plugin_search_query_ops");
	if ((error = dlerror()) != NULL) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", error);
		dlclose(handle);
		return UNDEFINED_LIB_SYMBOL;
	}

	if (query_ops && keyword) {
		/* OPS RETURNS NULL, IT'S PROBLEM */
		cfg_func_table *ops = NULL;
		/*ret = (*query_ops) ("MOCK_STR_ID", &ops);   */
		ret = (*query_ops)(keyword, (void **)&ops);   /*  CALL */
		SETTING_TRACE(" ops = %x ", ops);
		/*------------------------------------------------------------- */
		/* ACTUAL PROCESSING */
		/*------------------------------------------------------------- */
		if ((ops) && (ops)->set_item_state) {
			(ops)->set_item_state(item_state, NULL, NULL);
		}
		SETTING_TRACE(" ---> ppops.set_item_state(stat, NULL); -- after, sstat : %d", item_state);
		/*------------------------------------------------------------- */
	}
	if (ret < 0) ret += UNDEFINED_LIB_SYMBOL;/*from -2 on */

	dlclose(handle);
	return ret;

}

#if 1
EXPORT_PUBLIC
int setting_search_get_module_item_state(char *keyword, char *ug_name, Cfg_Item_State *pstat, cfg_func_table *pops)
{
	SETTING_TRACE("Enter %s with ug_name:%s", __FUNCTION__, ug_name);
	/* int setting_plugin_search_query_ops(char* str_id, void** tfunc_obj) */
	int (*query_ops)(char *str_id, void **tfunc_obj);
	int ret = OPERATE_LIB_SUCESS;

	/*1.first do exist-checking in /opt/ug/lib */
	char ug_file[PATH_MAX + 1];
	snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH, ug_name);
	struct stat st;
	if (stat(ug_file, &st) != 0) {

		/*2.if it does not exit in /opt/ug/lib, then do exist-checking in /usr/ug/lib */
		memset(ug_file, 0x00, PATH_MAX + 1);
		snprintf(ug_file, PATH_MAX, "%s/libug-%s.so", SETTING_UG_PATH_USR, ug_name);
		if (stat(ug_file, &st) != 0) {
			/*both not exist,skip it */
			SETTING_TRACE_ERROR(" libug-%s.so is *NOT* present, so skip it..\n", ug_name);
			return OPERATE_LIB_SUCESS;
		}
	}
	/*------------------------------------------------------------------------------------- */
	void *handle = dlopen(ug_file, RTLD_LAZY);
	if (!handle) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", dlerror());
		return LOAD_LIB_FAILED;
	}

	char *error = NULL;
	query_ops = dlsym(handle, "setting_plugin_search_query_ops");
	if ((error = dlerror()) != NULL) {
		SETTING_TRACE_ERROR(" >>>>>>>>>>> %s", error);
		dlclose(handle);
		return UNDEFINED_LIB_SYMBOL;
	}

	if (query_ops && keyword) {
		/* OPS RETURNS NULL, IT'S PROBLEM */
		cfg_func_table *ops = NULL;
		// ret = (*query_ops) ("MOCK_STR_ID", &ops);   /*  CALL * /
		ret = (*query_ops)(keyword, (void **)&ops);   /*  CALL */
		SETTING_TRACE(" ops = %x ", ops);
		/*------------------------------------------------------------- */
		/* ACTUAL PROCESSING */
		/*------------------------------------------------------------- */
		if ((ops) && (ops)->get_item_state)
			(ops)->get_item_state(pstat, NULL);

		SETTING_TRACE(" ---> ppops.get_item_state(pstat, NULL); -- after, pstat : %d", *pstat);
		/*------------------------------------------------------------- */
	}
	if (ret < 0) ret += UNDEFINED_LIB_SYMBOL;/*from -2 on */

	dlclose(handle);
	return ret;
}
#endif


EXPORT_PUBLIC
int __setting_init_search_index_module()
{

	/* test, adding sql query */
	sqlite3 *db = NULL;
	setting_searchdb_open(&db);
	SETTING_TRACE("search db = %d", db);

	char *backup_langset =  vconf_get_str(VCONFKEY_LANGSET);
	char *ug_name_arr[] = {
		"setting-display-efl",					/* finish */
		"setting-profile-efl",					/* finish */
		"setting-call-efl",						/* NO CODE !!! -- put  node here for testing */
		"setting-cloud-efl",					/*finish */
		"quickcommand-setting-efl",				/* finish */
		"setting-mobileap-efl",					/* finish */
		"lockscreen-options",					/* finish */
		"email-setting-efl",					/* NO CODE !!! */
		"emergency-msg-setting-efl",			/* NO CODE !!! */
		"msg-setting-efl",						/* NO CODE !!! */
		"setting-allshare-cast-efl",			/* NO CODE !!! */
		"setting-allshare-efl",					/* NO CODE !!! */
		"setting-blockingmode-efl",				/* NO CODE !!! */
		/*"ug-bluetooth-efl", */
		"setting-dock-efl",						/* NO CODE !!! */
		"setting-drivingmode-efl",				/* NO CODE !!! */
		"setting-gallery-efl",					/* NO CODE !!! */
		"setting-homescreen-efl",				/* NO CODE !!! */
		"setting-ledindicator-efl",				/* NO CODE !!! */
		"setting-location-efl",					/* NO CODE !!! */
		"setting-manage-applications-efl",		/* NO CODE !!! */
		"setting-manage-certificates-efl",		/* NO CODE !!! */
		"setting-myaccount-efl",				/* NO CODE !!! */
		"setting-phone-efl",					/* NO CODE !!! */
		"setting-readytoshare-efl",				/* NO CODE !!! */
		"setting-synchronise-efl",				/* NO CODE !!! */
		"wifi-efl-UG",							/* NO CODE !!! */
		"setting-wifidirect-efl",				/* NO CODE !!! */
		"worldclock-efl"						/* NO CODE !!! */
		"setting-nfc-efl",						/* NO CODE !!! */
		"setting-font-efl",						/* NO CODE !!! */
		"quickpanel-setting-efl",				/* NO CODE !!! */
		"setting-easymode-efl",					/* NO CODE !!! */
		"setting-accessibility-efl",			/* NO CODE !!! */
		"setting-personalpage-efl",				/* NO CODE !!! */
		"setting-voice-efl",					/* NO CODE !!! */
		"setting-motion-efl",					/* NO CODE !!! */
		"setting-palmmotion-efl",				/* NO CODE !!! */
		"setting-time-efl",						/* NO CODE !!! */
		"setting-accessory-efl",				/* NO CODE !!! */
		"setting-powersaving-efl",				/* NO CODE !!! */
		"setting-storage-efl",					/* NO CODE !!! */
		"setting-security-efl",					/* NO CODE !!! */
		"setting-privacy-efl",					/* NO CODE !!! */
		"isfsetting-efl",					/* NO CODE !!! */
		"setting-developeroption-efl",			/* NO CODE !!! */
		"setting-about-efl",						/* NO CODE !!! */
		"setting-network-efl",				/*finish */
		"setting-datausage-efl",
		"setting-fingerscanner-efl",
		"setting-vpn-efl",
	};

	int i;
	for (i = 0; i < sizeof(ug_name_arr) / sizeof(ug_name_arr[0]); i++) {
		/*SETTING_TRACE("---------> UG NAME : %s", ug_name_arr[i]); */
		int ret = -1;
		Eina_List *plist = NULL;
		Setting_Cfg_Node_T *n = calloc(1, sizeof(Setting_Cfg_Node_T));
		if (n) {
			n->key_name = strdup("Sentinal");
			plist = eina_list_append(plist, n);
		}

		char *domainname = NULL;
		ret =  setting_invoke_search_db_function(ug_name_arr[i], NULL, &plist, &domainname);
		if (ret != OPERATE_LIB_SUCESS) {
			SETTING_TRACE_ERROR(" no plugin -- code : %s : %d ", ug_name_arr[i], ret);
			continue;
		} else {
			/*SETTING_TRACE(" return : %d", ret); */
			SETTING_TRACE(" plugin -- code : %s O.K : %d ", ug_name_arr[i], ret);
		}

		if (!domainname) {
			SETTING_TRACE_ERROR("domainname == NULL, it's error by plugin, not go next");
			Eina_List *l = NULL;
			Setting_Cfg_Node_T *node = NULL;
			/*int count = 0; */
			EINA_LIST_FOREACH(plist, l, node) {
				FREE(node->key_name);
				FREE(node->ug_args);
				FREE(node->icon_path);
				FREE(node->data);
				FREE(node);
			}
			eina_list_free(plist);
			plist = NULL;
			FREE(domainname);
			continue;
		}
		char *p = strchr(domainname, ':');

		char *domain_appname = NULL;
#if 1
		if (p) {
			char *tempp;
			domain_appname = strtok_r(domainname, ":",&tempp);
			
			SETTING_TRACE("p1:appname : %s\n", domain_appname);
			SETTING_TRACE("p2:localedir : %s\n", p + 1);
			/* set */
			setting_set_i18n(domain_appname, p + 1);
		} else {
			domain_appname = domainname;
			/* default locale */
			setting_set_i18n(domain_appname, "/usr/ug/res/locale");
		}
#endif

		/* test */
		Eina_List *langlist = setting_get_language_list2(LANGLIST_ALL_FILE_PATH);

		Eina_List *elist = NULL;
		setting_lang_entry *pnode = NULL;
		char *locale = NULL;
		EINA_LIST_FOREACH(langlist, elist, pnode) {
			locale = pnode->locale;

			char str[256];
			snprintf(str, 256, "%s.UTF-8", locale);
			(void)setlocale(LC_MESSAGES, str);

			/*-------------------------------------------------------------- */
			SETTING_TRACE(" (locale : %s) --------- BEFORE EINA LOOP : %x ", locale, plist);
			/* loop and print */
			Eina_List *l = NULL;
			Setting_Cfg_Node_T *node = NULL;
			int count = 0;
			EINA_LIST_FOREACH(plist, l, node) {
				if (count != 0) {
#if 0
					SETTING_TRACE(" key_name : %s", node->key_name);
					SETTING_TRACE(" ug_args : %s", node->ug_args);
					SETTING_TRACE(" icon_path : %s", node->icon_path);

					SETTING_TRACE(" pos : %d", node->pos);
					SETTING_TRACE(" reset_type : %d", node->reset_type);
					SETTING_TRACE(" click_times : %d", node->click_times);
					SETTING_TRACE(" item_type : %d", node->item_type);
					SETTING_TRACE(" data : %x", node->data);
					SETTING_TRACE(" -----------------------------");
#else
					SETTING_TRACE(" add DB :: key_name : %s", node->key_name);


					char *subindex = (node->sub_index) ? node->sub_index : "";
					char *icon_path = (node->icon_path) ? node->icon_path : "";

					setting_add_db_search_index(db, locale, __(node->key_name, domain_appname), node->ug_args, node->item_type, ug_name_arr[i], __(subindex, domain_appname), icon_path);
#endif
				}
				count++;
			}
			/*SETTING_TRACE(" --------- AFTER EINA LOOP "); */

		} /* locale - LOOP */

		/*free the list */
		Eina_List *l = NULL;
		Setting_Cfg_Node_T *node = NULL;

		EINA_LIST_FOREACH(plist, l, node) {
			FREE(node->key_name);
			FREE(node->ug_args);
			FREE(node->icon_path);
			FREE(node->data);
			FREE(node->sub_index);
			FREE(node);
		}
		eina_list_free(plist);
		plist = NULL;
		FREE(domainname);
		/*-------------------------------------------------------------- */
	}	/* end of for loop */


	/* roll back */
	(void)setlocale(LC_MESSAGES, backup_langset);
	setting_searchdb_close(db);

	return 0;
}

EXPORT_PUBLIC
int __setting_init_search_index_app()
{
	/* test, adding sql query */
	sqlite3 *db = NULL;
	setting_searchdb_open(&db);
	SETTING_TRACE("search db = %d", db);

	/* clean all entries */
	setting_add_db_search_clean(db);

	/* add index for testing */
	Setting_Cfg_Node_T *pitem = get_cfg_node_first();	/* head */
	int size = get_cfg_array_size();
	SETTING_TRACE(" size of array : %d ", size);

	char *locale = NULL;
	char *backup_langset =  vconf_get_str(VCONFKEY_LANGSET);

	/* get langlist // load language table from XML file */
	Eina_List *elist = NULL;
	setting_lang_entry *pnode = NULL;
	Eina_List *langlist = setting_get_language_list2(LANGLIST_ALL_FILE_PATH);

	EINA_LIST_FOREACH(langlist, elist, pnode) {
		/*SETTING_TRACE(" locale = %s --- language = %s --- mcc = %s ", pnode->locale, pnode->title, pnode->mcc); */
		locale = pnode->locale;

		char str[256];
		snprintf(str, 256, "%s.UTF-8", locale);
		(void)setlocale(LC_MESSAGES, str);

		int i;
		for (i = 0; i < size; i++) {
			/*SETTING_TRACE(" index : %d ", i); */
			char *subindex = (pitem[i].sub_index) ? _(pitem[i].sub_index) : "";
			char *icon_path = (pitem[i].icon_path) ? pitem[i].icon_path : "";
			if (pitem[i].item_type == Cfg_Item_Title_Node) {

				setting_add_db_search_index(db, locale, _(pitem[i].key_name), pitem[i].ug_args, pitem[i].item_type/*title in Genlist*/, "", subindex, icon_path);

			} else if (pitem[i].item_type == Cfg_Item_Ug_Node
			           || pitem[i].item_type == Cfg_Item_Ug_Node_Toggle
			           || pitem[i].item_type == Cfg_Item_AppLauncher_Node
			           || pitem[i].item_type == Cfg_Item_AppLauncher_Node_Toggle
			          ) {

				if (pitem[i].key_name && _(pitem[i].key_name)) {
					setting_add_db_search_index(db, locale, _(pitem[i].key_name), pitem[i].ug_args, pitem[i].item_type/*UG name*/, "", subindex, icon_path);
				}
			} else if (pitem[i].item_type == Cfg_Item_App_Node) {

				if (pitem[i].key_name && _(pitem[i].key_name)) {
					setting_add_db_search_index(db, locale, _(pitem[i].key_name), pitem[i].ug_args, pitem[i].item_type/*App Launching*/, "", subindex, icon_path);
				}
			} else if (pitem[i].item_type == Cfg_Item_Ui_Node
			           || pitem[i].item_type == Cfg_Item_Ui_Node_Toggle
			          ) {

				setting_add_db_search_index(db, locale, _(pitem[i].key_name), pitem[i].ug_args, pitem[i].item_type/*UI type*/, "", subindex, icon_path);
			}
		}

	}
	/* roll back */
	(void)setlocale(LC_MESSAGES, backup_langset);
	setting_searchdb_close(db);
	return 0;
}
