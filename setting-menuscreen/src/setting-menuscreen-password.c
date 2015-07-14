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
#include <setting-menuscreen-password.h>

#define SETTING_PW_UG_NORMAL_PASSWORD_MIN_LENGTH 4
#define SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH 16
#define SETTING_PW_UG_PRIVACY_PASSWORD_MIN_LENGTH 4
#define SETTING_PW_UG_PRIVACY_PASSWORD_MAX_LENGTH 16



static int setting_menuscreen_password_create(void *cb);
static int setting_menuscreen_password_destroy(void *cb);
static int setting_menuscreen_password_update(void *cb);
static int setting_menuscreen_password_cleanup(void *cb);


setting_view setting_view_menuscreen_password = {
	.create = setting_menuscreen_password_create,
	.destroy = setting_menuscreen_password_destroy,
	.update = setting_menuscreen_password_update,
	.cleanup = setting_menuscreen_password_cleanup,
};
static Eina_Bool __focus_on_idler(void *data)
{
 	SETTING_TRACE_BEGIN;
 	/* error check */
 	retv_if(data == NULL, FALSE);

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) data;
	retv_if(ad->data_pwd == NULL, EINA_FALSE);
 	if (ad->data_pwd->isFocusFlag || (ad->data_pwd->eo_check != NULL))
	{
		//no matter how many edifileds, it only need focus on 1st editfiled
		elm_object_focus_set(ad->data_pwd->eo_check, EINA_TRUE);
 	}

	SETTING_TRACE_END;
	return ECORE_CALLBACK_CANCEL;
	//return EINA_FALSE;
}

static int setting_menuscreen_password_create(void *cb)
{
	SETTING_TRACE_BEGIN;
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) cb;
	Evas_Object *scroller = elm_genlist_add(ad->win_main_layout);
	elm_object_style_set(scroller, "dialogue");
	if(scroller == NULL)
		return SETTING_RETURN_FAIL;

	elm_genlist_clear(scroller);	/* first to clear list */
	elm_genlist_homogeneous_set(scroller, EINA_TRUE);

	ad->navi_it = setting_create_navi_bar_top_buttons(_("IDS_COM_BODY_AUTH_PASSWORD"),
					   _("IDS_COM_SK_CANCEL"),
					   _("IDS_COM_SK_DONE"), NULL,
					   setting_menuscreen_password_click_softkey_back_cb,
					   setting_menuscreen_password_click_softkey_done_cb,
					   NULL, ad, scroller, ad->navi_bar);
	retv_if(!ad->navi_it, SETTING_RETURN_FAIL);
	Evas_Object *done_btn = elm_object_item_part_content_get(ad->navi_it, "title_right_btn");
	setting_disable_evas_object(done_btn);
	setting_dim_evas_object(done_btn, TRUE);

	Elm_Object_Item *item = NULL;;
	item =elm_genlist_item_append(scroller, &(ad->itc_seperator), NULL, NULL,
				    ELM_GENLIST_ITEM_NONE, NULL, NULL);
	elm_genlist_item_select_mode_set(item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	//---------------------------------------------------------------------
	// draw password UG code
	//---------------------------------------------------------------------
	ad->data_pwd=
		setting_create_Gendial_field_entry(scroller,
						   & (itc_1icon),
						   NULL, ad,
						   SWALLOW_Type_LAYOUT_ENTRY,
						   NULL, NULL, 0,
						   "IDS_COM_BODY_PASSWORD",
						   NULL,
						   setting_menuscreen_password_entry_change_cb,
						   ELM_INPUT_PANEL_LAYOUT_NUMBERONLY,
						   TRUE, TRUE,
						   SETTING_PW_UG_NORMAL_PASSWORD_MAX_LENGTH,
						   0, NULL, NULL);
	elm_object_focus_set(ad->data_pwd->eo_check, EINA_TRUE);
	ecore_idler_add(__focus_on_idler, ad);

	setting_view_menuscreen_password.is_create = 1;
	return SETTING_RETURN_SUCCESS;
}

static int setting_menuscreen_password_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) cb;
	elm_naviframe_item_pop(ad->navi_bar);

	setting_view_menuscreen_password.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}


static int setting_menuscreen_password_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_menuscreen_password_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	return setting_menuscreen_password_destroy(cb);
}

/* ***************************************************
 *
 *general func
 *
 ***************************************************/
static void
setting_menuscreen_password_click_softkey_back_cb(void *data, Evas_Object *obj,
					   void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	setting_retm_if(data == NULL,
			"[Setting > Menuscreen] Data parameter is NULL");

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) data;

	/* Send destroy request */
	setting_view_change(&setting_view_menuscreen_password, &setting_view_menuscreen_main, ad);
	SETTING_TRACE_END;

}

static void
setting_menuscreen_password_click_softkey_done_cb(void *data, Evas_Object *obj,
					    void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");

	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) data;
	const char *entry_str = ad->data_pwd->sub_desc;
	if (0 == safeStrCmp(entry_str, SETTING_PW_STR))
	{
		// choose the value from radio button state.
		vconf_set_str( VCONFKEY_SETAPPL_SELECTED_PACKAGE_NAME, ad->selected_theme);
		ug_destroy_me(ad->ug);
	}
	else
	{
		setting_create_popup_without_btn(ad, ad->win_get, NULL, _("IDS_COM_POP_INCORRECT_PASSWORD"),NULL, 2, FALSE, FALSE);
		ad->data_pwd->sub_desc = NULL;
		elm_entry_entry_set(obj, ad->data_pwd->sub_desc);
		elm_entry_cursor_end_set(obj);
		elm_object_focus_set(ad->data_pwd->eo_check, EINA_TRUE);
		return;
	}
}

static void setting_menuscreen_password_entry_change_cb (void *data, Evas_Object *obj, void *event_info)
{
	retm_if(data == NULL, "[Setting > Password] Data parameter is NULL");
	Setting_GenGroupItem_Data *list_item =
	    (Setting_GenGroupItem_Data *) data;
	SettingMenuscreenUG *ad = (SettingMenuscreenUG *) list_item->userdata;
	const char *entry_str = elm_entry_entry_get(obj);
	/* SETTING_TRACE("entry_str:%s", entry_str); */
	list_item->sub_desc = (char *)g_strdup(entry_str);

	/* Evas_Object *eo_entry = NULL; */
	bool isFoundEmptyEntry = FALSE;

	do {
		if (ad->data_pwd) {
			entry_str = ad->data_pwd->sub_desc;
			if (NULL == entry_str || '\0' == entry_str[0]) {
				isFoundEmptyEntry = TRUE;
				break;
			}
		}

	} while (0);

	bool disableFlag = isFoundEmptyEntry;

	Evas_Object *done_btn = elm_object_item_part_content_get(ad->navi_it, "title_right_btn");
	if (disableFlag) {
		if (!evas_object_pass_events_get(done_btn)) {//it had not yet been disabled
			setting_disable_evas_object(done_btn);
			setting_dim_evas_object(done_btn, TRUE);
		}//else already disabled
	} else {
		setting_enable_evas_object(done_btn);
		setting_undo_dim_evas_object(done_btn, TRUE);
	}
}

