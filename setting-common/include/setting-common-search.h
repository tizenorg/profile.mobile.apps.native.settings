/*
 * setting
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Flora License, Version 1.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	   http://floralicense.org/license/
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _SETTING_SEARCH_H_
#define _SETTING_SEARCH_H_

#include <Elementary.h>
#include <Eina.h>

#include <setting-common-general-func.h>
#include <setting-common-draw-widget.h>

#include <setting-cfg.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include <sqlite3.h>

#define S_FINDER_SETTING_STR "setting_from_sfinder"

#ifndef bool
#define bool int
#define TRUE 1
#define FALSE 0
#endif

#define SEARCH_DB_PATH	_TZ_SYS_DATA"/setting/test.db"
#define SEARCH_DB_QUERY \
	"select * from setting_search indexed by search_index where locale ="\
	"%Q and keystr LIKE '%%%q%%'"
#define SEARCH_DB_QUERY_SELECT_ALL "select * from setting_search indexed by "\
	"search_index where locale = %Q"
#define SEARCH_COUNT_DB_QUERY "select count(*) from setting_search indexed by "\
	"search_index where locale = %Q and keystr LIKE '%%%q%%'"


extern const Elm_Genlist_Item_Class g_search_genlist_ts;
extern const Elm_Genlist_Item_Class g_search_genlist_ts_toggle;
extern const Elm_Genlist_Item_Class g_search_genlist_ts_divided_toggle;
extern const Elm_Genlist_Item_Class itc_no_reult;

/**
* @brief Item declaration of genlist
* retreive data from Database
*/
typedef struct _ListItemObject {
	char *title;		/* tag text */
	char *title2;		/* plain text, */
	char *type;
	char *infostr;
	char *viewname;
	char *fieldname;
	char *icon_path;
	void *data;
	void **pBack; /*for pointer safety */

	Elm_Object_Item *item;
	/* toggle button proc */
	void (*fp_check_btn_cb)(void *data, Evas_Object *obj, void *event_info);
	char *check_value_set;
} ListItemObject;


/**
* @brief add serach index for Database
*
* @param keystr keyword to be searched
* @param infostr additional info
* @param data_type 1:UGname 2:menustring 3:title
* @param viewname to find page
* @param fieldname to find field in the page
*/
extern int setting_search_get_module_item_state(char *keyword, char *ug_name,
		Cfg_Item_State *stat, cfg_func_table *pops);
extern int setting_search_set_module_item_state(char *keyword, char *ug_name,
		Cfg_Item_State stat, cfg_func_table *pops);

extern void setting_add_db_search_index(sqlite3 *db, char *locale,
		char *keystr, char *infostr, int data_type, char *viewname,
		char *fieldname, char *icon_path);
extern int setting_searchdb_open(sqlite3 **ppdb);
extern void setting_searchdb_close(sqlite3 *db);

extern int __setting_init_search_index_app();
extern int __setting_init_search_index_module();

#endif /* _SETTING_SEARCH_H_ */
