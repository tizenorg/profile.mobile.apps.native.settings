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
#ifndef __SETTING_RESET_H__
#define __SETTING_RESET_H__

#include <stdio.h>
#include <Elementary.h>

#include <glib-object.h>

#include <Ecore_IMF.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>

#define RESET_RESULT_STR "IDS_ST_POP_RESETTING_ING"
#define RESET_SUCESS_STR "IDS_COM_POP_SUCCESS"
#define LOAD_LIB_FAILED_STR "Exception"//"Load LIB failed"
#define UNDEFINE_STR "Undefined" //"Resetting undefined"

#define RESET_FAILED_STR "IDS_COM_POP_FAILED"//"Reset failed"
#define VCONF_RESET_FAILED_STR "IDS_COM_POP_FAILED"//"vconf set failed"

#define ADN_STR " and "
#define CLOSEING_STR " had been reseted just now, Setting will be closed"

//#define RESET_ALL "Reset all"
#define RESET_ALL "IDS_ST_BODY_RESET_ALL"

//#define RESET_ALL_DESC "IDS_ST_BODY_ALL_SETTINGS_WILL_REVERT_TO_DEFAULT_VALUES"
#define RESET_ALL_DESC "IDS_ST_BODY_AFTER_RESETTING_PHONE_WILL_RESTART_AUTOMATICALLY"

#define RESET_SETTINGS_STR	"IDS_ST_BODY_BACKUP_AND_RESET"	//"Reset settings"
#define SIM_CARD_PROFILE_SETTINGS_STR "IDS_ST_MBODY_SIM_CARD_PROFILE_SETTINGS"	//"SIM card profile sttings"
#define FACTORY_RESET_STR	"IDS_ST_MBODY_FACTORY_RESET"	//"Factory reset"

#define SIMPLY_SUPPORT_ALL_RESET 0
#define SUPPORT_RESET_DETAILS 1
#define SUPPORT_SIM_CARD_PROFILE 0
#define SUPPORT_FACTORY_RESET 1
#define SUPPORT_RESET_OLD 0

typedef struct _SettingResetUG SettingResetUG;

/**
 * Setting Reset UG context
 * all UG function has void* as an agument. this is casted back to SettingResetUG
 * and the functions access app context.
 */
struct _SettingResetUG {
	ui_gadget_h ug;
	ui_gadget_h ug_loading;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;	/*  window_layout */
	Evas_Object *win_get;	/*  win_main */
	Evas_Object *ly_main;
	Evas_Object *notify;
	Evas_Object *old_notify;
	Evas_Object *ask_popup;
	Evas_Object *pop_progress;
	Evas_Object *view_layout;
	Evas_Object *navi_bar;
	Evas_Object *controllbar;
	Evas_Object *reset_list;
	Evas_Object *main_scroller;

	Evas_Object *pop_factory_reset;

	Setting_Done_List_Data list_data;
	Ecore_Animator *animator;
	Ecore_Idler *reset_all_idler;
	int gl_index;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1icon;
#if SUPPORT_RESET_DETAILS
	Elm_Genlist_Item_Class itc_dlg_1text;
	Elm_Genlist_Item_Class itc_seperator;
	Evas_Object *ly_reset;
	Evas_Object *pop;
#endif
	Evas_Object *pop_btn;
};

extern setting_view setting_view_reset_main;
extern setting_view setting_view_reset_settings;

int setting_reset_flight_mode();
int setting_reset_rotation_lock(void);
void setting_reset_result_popup_resp_cb(void *data, Evas_Object *obj, void *event_info);
void setting_reset_result_popup_update(void *data);
void setting_reset_special_process(void *data);

#endif				/* __SETTING_RESET_H__ */
