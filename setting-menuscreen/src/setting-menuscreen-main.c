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
#include <setting-menuscreen-main.h>
#include <setting-common-draw-widget.h>

static int setting_menuscreen_main_create(void *cb);
static int setting_menuscreen_main_destroy(void *cb);
static int setting_menuscreen_main_update(void *cb);
static int setting_menuscreen_main_cleanup(void *cb);


// BUSINESS LOGIC  ---
static void _get_menuscreen_info2(/*[OUT]*/int* pkglist_cnt, void* data);
ail_cb_ret_e appinfo_menuscreen_func(const ail_appinfo_h appinfo, void *user_data);
// BUSINESS LOGIC  ---
static void setting_menuscreen_main_click_softkey_back_cb(void *data, Evas_Object *obj, void *event_info);

static void setting_menuscreen_mouse_up_Gendial_list_radio_cb(void *data, Evas_Object *obj, void *event_info);

setting_view setting_view_menuscreen_main = {
	.create = setting_menuscreen_main_create,
	.destroy = setting_menuscreen_main_destroy,
	.update = setting_menuscreen_main_update,
	.cleanup = setting_menuscreen_main_cleanup,
};

static int setting_menuscreen_main_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) cb;
	Evas_Object *scroller = NULL;
	ad->ly_main =
	    setting_create_layout_navi_bar_genlist(ad->win_main_layout,
						   ad->win_get,
						   _("IDS_ST_HEADER_HOME_SCREEN_ABB"), _("IDS_COM_BODY_BACK"),
						   NULL,
						   setting_menuscreen_main_click_softkey_back_cb,
						   NULL, ad, &scroller,
						   &(ad->navi_bar));

	Elm_Object_Item *item = NULL;
	item = elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	int* pkglist_cnt = &ad->pkglist_cnt;

	_get_menuscreen_info2(pkglist_cnt, ad);

	// radio button
	ad->chk_radio = elm_radio_add(scroller);
	elm_radio_state_value_set(ad->chk_radio, -1);

	Setting_GenGroupItem_Data *item_data = NULL;
	Eina_List* elist = NULL;
	ug_menuscreen_info* pnode = NULL;
	int idx = 0;
	char *sel_str = vconf_get_str(VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME);

	EINA_LIST_FOREACH( ad->menuscreen_list, elist, pnode)
	{
		item_data = setting_create_Gendial_field_2radio(scroller,
							&(ad->itc_1text_1icon_2),
							setting_menuscreen_mouse_up_Gendial_list_radio_cb,
							ad->chk_radio,
							SWALLOW_Type_1RADIO,
							ad->chk_radio,
							idx,
							pnode->appname,
							pnode->pkgname,
							 NULL);

		if (item_data) {
			item_data->userdata = ad;
		} else {
			SETTING_TRACE_ERROR("item_data is NULL");
		}
		SETTING_TRACE("pkgname[%s] sel_str[%s]", pnode->pkgname, sel_str);
		if (0 == safeStrCmp(pnode->pkgname, sel_str))
		{
			elm_radio_value_set(ad->chk_radio, idx);
		}

		idx++;
	}
	FREE(sel_str);


	setting_view_menuscreen_main.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_menuscreen_main_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) cb;

	if (ad->ly_main != NULL) {
		evas_object_del(ad->ly_main);
		/* if(ad->back_dialData) FREE(ad->back_dialData); */
		setting_view_menuscreen_main.is_create = 0;
	}

	return SETTING_RETURN_SUCCESS;
}

static int setting_menuscreen_main_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	return SETTING_RETURN_SUCCESS;
}

static int setting_menuscreen_main_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) cb;

	if (ad->ly_main != NULL) {
		/*         evas_object_hide(ad->ly_main); */
	}

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

static void
setting_menuscreen_main_click_softkey_back_cb(void *data, Evas_Object *obj,
					   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL,
			"[Setting > Menuscreen] Data parameter is NULL");

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) data;

	/* Send destroy request */
	ug_destroy_me(ad->ug);
	SETTING_TRACE_END;

}


static void _get_menuscreen_info2(/*[OUT]*/int* pkglist_cnt, void* data)
{
	SETTING_TRACE_BEGIN;
	SettingMenuscreenUG *ugd = (SettingMenuscreenUG *) data;

    ail_filter_h filter;
    ail_error_e ret;
    // int i=0;
     int n=0;

    ret = ail_filter_new(&filter);
    if (ret != AIL_ERROR_OK) {
        return ;
    }

    ret = ail_filter_count_appinfo(filter, &n);
    //printf("count =%d \n", n);
    *pkglist_cnt = n;

    ret = ail_filter_add_str(filter, AIL_PROP_CATEGORIES_STR, "home-screen");
    if (ret != AIL_ERROR_OK) {
	    ail_filter_destroy(filter);
	    return ;
    }

    ail_filter_list_appinfo_foreach(filter, appinfo_menuscreen_func, (void *)ugd);
    ail_filter_destroy(filter);
}

/*
    /opt/home/root/aaa/cprog/ail_test # ./ail_test
    i=0 List Menu
    i=1 BEAT
*/
ail_cb_ret_e appinfo_menuscreen_func(const ail_appinfo_h appinfo, void *user_data)
{
	SETTING_TRACE_BEGIN;
	SettingMenuscreenUG *ugd = (SettingMenuscreenUG *) user_data;

    char *package;
    char *appname;
    static int s_i = 0 ;

    ail_appinfo_get_str(appinfo, AIL_PROP_PACKAGE_STR, &package);
    ail_appinfo_get_str(appinfo, AIL_PROP_NAME_STR, &appname);

    Eina_List** d_menuscreen_list = &ugd->menuscreen_list;

    ug_menuscreen_info* pnode = (ug_menuscreen_info*)malloc(sizeof(ug_menuscreen_info));
    pnode->pkgname = strdup(package);
    pnode->appname = strdup(appname);

    SETTING_TRACE(" >>>>>>>>>> s_i=%d %s\n", s_i++, package);

    *d_menuscreen_list = eina_list_append(*d_menuscreen_list, pnode);

    if (s_i > 30)
        return AIL_CB_RET_CANCEL;

    return AIL_CB_RET_CONTINUE;
}

static void setting_menuscreen_mouse_up_Gendial_list_radio_cb(void *data, Evas_Object *obj, void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL, "Data parameter is NULL");
	setting_retm_if(event_info == NULL, "Invalid argument: event info is NULL");

	Evas_Object *radio = (Evas_Object *) data;
	Elm_Object_Item *item = (Elm_Object_Item *) event_info;
	elm_genlist_item_selected_set(item, 0);

	Setting_GenGroupItem_Data *list_item = (Setting_GenGroupItem_Data *) elm_object_item_data_get(item);
	if (list_item->chk_status == elm_radio_value_get(radio))
	{//select the same item
		return;
	}

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) list_item->userdata;

	SETTING_TRACE("---------------------------------------------");
	SETTING_TRACE("chk_status = %d ", list_item->chk_status);
	SETTING_TRACE("sub_desc= %s", list_item->sub_desc);
	SETTING_TRACE("KeyStr= %s", list_item->keyStr);
	SETTING_TRACE("---------------------------------------------");

	// store list_itemdata
	ad->selected_theme = list_item->sub_desc;

	if ( 0 == strcmp(list_item->sub_desc, SETTING_THEME_LIVE_MAGAZINE))
	{
		setting_view_change(&setting_view_menuscreen_main, &setting_view_menuscreen_password, ad);
		return;
	}
	else if (0 == strcmp(list_item->sub_desc, SETTING_THEME_CLUSTER_HOME))
	{
		setting_view_change(&setting_view_menuscreen_main, &setting_view_menuscreen_password, ad);
		return;
	}
	else {
		elm_radio_value_set(radio, list_item->chk_status);
		vconf_set_str( VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME, ad->selected_theme);
	}
	ug_destroy_me(ad->ug);
}


