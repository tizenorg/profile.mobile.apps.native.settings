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
#include <setting-more-menu.h>

static int setting_more_menu_create(void *cb);
static int setting_more_menu_destroy(void *cb);
static int setting_more_menu_update(void *cb);
static int setting_more_menu_cleanup(void *cb);

setting_view setting_view_more_menu = {
	.create = setting_more_menu_create,
	.destroy = setting_more_menu_destroy,
	.update = setting_more_menu_update,
	.cleanup = setting_more_menu_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/
const static Elm_Genlist_Item_Class itc_sep = {
	.item_style = "dialogue/separator",
	.func.text_get = NULL,
	.func.content_get = NULL,
	.func.state_get = NULL,
	.func.del = NULL,
};

static int setting_more_menu_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_main_appdata *ad = (setting_main_appdata *) cb;

	Evas_Object *scroller = NULL;
	char *title;
	menu_list_load_fn2 load_fn = NULL;
	if (SETTING_VIEW_CONNECTIVE_MORE == ad->view_load)
	{
		title = _(KeyStr_MoreConnect);
		load_fn = __load_connectivity_menu_list;
	}
	else if (SETTING_VIEW_SYSTEM_MORE == ad->view_load)
	{
		title = _(KeyStr_MoreSystem);
		load_fn = __load_system_menu_list;
	}
	else
	{
		SETTING_TRACE_ERROR("Invalid view");
		return SETTING_RETURN_FAIL;
	}
	setting_push_layout_navi_bar_genlist(ad->win_main, ad->win_main,
				     title, _("IDS_COM_BODY_BACK"),
				     NULL,
				     setting_more_menu_click_softkey_cancel_cb,
				     NULL, ad, &scroller, ad->navibar_main);

	evas_object_smart_callback_add(scroller, "realized", __gl_realized, ad);

	Elm_Object_Item *item = elm_genlist_item_append(scroller, &(itc_sep),
				       NULL, NULL,
				       ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	load_fn(ad, Cfg_Item_Pos_Level1, scroller);

	item = elm_genlist_item_append(scroller, &itc_bottom_seperator, NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	ad->genlist_load = scroller;
	setting_view_more_menu.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_more_menu_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	setting_main_appdata *ad = (setting_main_appdata *) cb;
	if (0 == setting_view_more_menu.is_create)
	{
		//already not exsit
		return SETTING_RETURN_SUCCESS;
	}

	evas_object_smart_callback_del(ad->genlist_load, "realized", __gl_realized);

	elm_naviframe_item_pop(ad->navibar_main);
	ad->view_load = SETTING_VIEW_MAIN;
	ad->genlist_load = ad->main_genlist;
	setting_view_more_menu.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_more_menu_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;

}

static int setting_more_menu_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	setting_more_menu_destroy(cb);

	return SETTING_RETURN_SUCCESS;
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/

/* ***************************************************
 *
 *call back func
 *
 ***************************************************/

static void
setting_more_menu_click_softkey_cancel_cb(void *data,
							Evas_Object *obj,
							void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");
	setting_main_appdata *ad = (setting_main_appdata *) data;

	setting_view_change(&setting_view_more_menu,
			    &setting_view_main, ad);
}

