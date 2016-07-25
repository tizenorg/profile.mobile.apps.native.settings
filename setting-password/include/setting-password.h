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
#ifndef _SETTING_PASSWORD_H_
#define _SETTING_PASSWORD_H_

#include <glib.h>
#include <Elementary.h>
#include <Ecore_IMF.h>
#include <ITapiSim.h>
#include <TelSim.h>
#include <TapiUtility.h>
#include <glib-object.h>
#include <dlog.h>

#include "setting-common-draw-widget.h"
#include "setting-common-view.h"
#include "setting-debug.h"
#include "setting-common-init.h"
#include "setting-password-strings.h"

/*#include <ckmc/ckmc-control.h> */
/*#include <ckmc/ckmc-type.h> */

#define SETTING_PW_NORMAL_PASSWORD_MIN_LENGTH 4
#define SETTING_PW_NORMAL_PASSWORD_MAX_LENGTH 16
#define SETTING_PW_SIM_MIN_LENGTH 4
#define SETTING_PW_SIM_MAX_LENGTH 8
#define SETTING_PW_PIN_MIN_LENGTH 4
#define SETTING_PW_PIN_MAX_LENGTH 8
#define SETTING_PW_PUK_MAX_LENGTH 8
#define SETTING_PW_MAX_TITLE_LENGTH MAX_DISPLAY_NAME_LEN_ON_UI

#define PW_NORMAL_CONTAIN_ALPHANUMERIC 4
#define PW_ERR_DELAY_TIME	30
#define ATTEMPT_INFINITE	999999

typedef enum {
	/* Final type to verify current password */
	SETTING_PW_TYPE_PASSWORD = 1,
	/* This view type is used when click "password" and "change password"
	 * items in screen lock type view */
	SETTING_PW_TYPE_SET_PASSWORD,
	/* Final type to verify current simple password */
	SETTING_PW_TYPE_SIMPLE_PASSWORD,
	/* This view type is used when click "simple password" and "change
	 * simple password" items in screen lock type view */
	SETTING_PW_TYPE_SET_SIMPLE_PASSWORD,
	/* This view type is used when enter screen lock type menu to verify
	 * current password lock type */
	SETTING_PW_TYPE_ENTER_LOCK_TYPE,
	SETTING_PW_TYPE_SIM_LOCK_ON = 6, /* No use : Do not support SIM LOCK */
	/* No use : Do not support SIM LOCK */
	SETTING_PW_TYPE_SIM_LOCK_OFF,
	/* This view type is used when try to turn PIN LOCK on */
	SETTING_PW_TYPE_PIN_LOCK_ON,
	/* This view type is used when try to turn PIN LOCK off */
	SETTING_PW_TYPE_PIN_LOCK_OFF,
	SETTING_PW_TYPE_CHANGE_PIN,
	SETTING_PW_TYPE_CHANGE_PIN2 = 11,
	/* No use : PIN1 must be unblocked through pwlock app */
	SETTING_PW_TYPE_PIN_BLOCKED,
	SETTING_PW_TYPE_PIN2_BLOCKED,
	SETTING_PW_TYPE_MAX,
} setting_pw_type;

typedef enum {
	/* if length of password is wrong, */
	SETTING_PW_ERROR_INVALID_LENGTH = -1,
	/* if password that user enter has no letter, */
	SETTING_PW_ERROR_INCLUDE_NO_LETTER = -2,
	/* if password that user enter does not match with policy, */
	SETTING_PW_ERROR_NO_MATCH_WITH_POLICY = -3,
	/* if password that user enter has no letter, */
	SETTING_PW_ERROR_NO_MATCH_MIN_COMPLEX = -4,
	SETTING_PW_ERROR_EXIST_FORBIDDEN_STRING = -5,
	SETTING_PW_ERROR_NO_MATCH_MAX_REPEAT_COUNT = -6,
	SETTING_PW_ERROR_NO_MATCH_CUR_PASSWORD = -7,
	SETTING_PW_ERROR_SIMPLE_STYLE = -8,
	SETTING_PW_ERROR_REUSED = -9,
	SETTING_PW_ERROR_INCLUDE_DIGIT = -10,
	SETTING_PW_ERROR_NO_MATCH_WITH_PATTERN = -11,
	SETTING_PW_ERROR_NO_DIGIT = -12,
	SETTING_PW_ERROR_DO_NOT_MATCH = -13,
	SETTING_PW_ERROR_UNKNOWN = -50,
} setting_pw_errors;

typedef enum {
	SETTING_PW_STATUS_OK = 0,
	SETTING_PW_STATUS_EXPIRED,
	SETTING_PW_STATUS_MAX_FAILED,
	SETTING_PW_STATUS_EMPTY,
	SETTING_PW_STATUS_DEFAULT,
	SETTING_PW_STATUS_NON_PW_TYPE,
	SETTING_PW_STATUS_MAX,
} setting_pw_status;

enum {
	SETTING_PW_CATEGORY_LOCK = 0x100,
	SETTING_PW_CATEGORY_SIM = 0x200,
	SETTING_PW_CATEGORY_MAX,
};

enum {
	SETTING_PW_SUB_CATEGORY_SIMPLE = 0x01, SETTING_PW_SUB_CATEGORY_MAX,
};

struct _pw_item {
	int pw_type_num;
	char *pw_type_string;
	int category;
	int step;
};

/* For Tapi */
typedef enum {
	SIM_OPERATION_OK,
	SIM_ERROR,
	SIM_REQ_PIN,
	SIM_OPERATION_OFF,
	SIM_LOCK_ON_SUCCESS,
	SIM_LOCK_OFF_SUCCESS,
	SIM_PIN_LOCK_ON_SUCCESS,
	SIM_PIN_LOCK_OFF_SUCCESS,
	SIM_LOCK_ON_FAIL,
	SIM_LOCK_INCORRECT_PASSWORD,
	SIM_INCORRECT_PIN1_CODE,
	SIM_INCORRECT_PIN2_CODE,
	SIM_INCORRECT_PUK1_CODE,
	SIM_INCORRECT_PUK2_CODE,
	SIM_PIN1_CHANGE_SUCCESS,
	SIM_PIN2_CHANGE_SUCCESS,
	SIM_PIN1_CHANGE_FAIL,
	SIM_PIN2_CHANGE_FAIL,
	SIM_PIN1_BLOCKED,
	SIM_PIN2_BLOCKED,
	SIM_PIN1_UNBLOCKED,
	SIM_PIN2_UNBLOCKED,
	SIM_PUK1_BLOCKED,
	SIM_PUK2_BLOCKED,
	SIM_REQUIRED_PUK_CODE,
	SIM_OPERATION_UNAVAILABLE,
	SIM_UNKNOWN_ERROR,
} sim_stat_value;

typedef enum {
	SETTING_PW_NAVIBAR_WITH_NO_BTN,
	SETTING_PW_NAVIBAR_WITH_LEFT_BTN,
	SETTING_PW_NAVIBAR_WITH_RIGHT_BTN,
	SETTING_PW_NAVIBAR_WITH_BOTH_BTN,
} setting_pw_navibar_style_t;

typedef struct _tapi_request_event {
	unsigned int sid;
	int event;
} tapi_request_event;

typedef struct _tapi_receive_info {
	sim_stat_value stat;
	int retry_cnt;
} tapi_receive_info;

typedef struct _tapi_request_tapi_info {
	tapi_request_event *evt;
	int evt_sz;

	void *cb_data;
	void (*cb)(tapi_receive_info *, void *);
} tapi_request_tapi_info;

struct _word_repeat_info {
	char name[SETTING_PW_NORMAL_PASSWORD_MAX_LENGTH + 1];
	unsigned int count;
	struct _word_repeat_info *next;
};

typedef struct _pw_quality {
	int quality;
	int complex;
	int length;
} setting_pw_quality;

/**
 * Setting Password context
 */
typedef struct _SettingPassword {
	MainData md;
	TapiHandle *handle;
	TelSimSecResult_t *verify_puks_result;

	/* add more variables here (move your appdata to here) */
	setting_pw_type view_type;
	char *view_type_string;
	int step;

	Evas_Object *bottom_btn; /* Instead of Toolbar btn */
	Evas_Object *bottom_btn1; /* Instead of Toolbar btn */
	Setting_GenGroupItem_Data *ed_pw1;
	Setting_GenGroupItem_Data *ed_pw2;
	Setting_GenGroupItem_Data *ed_pw3;
	Setting_GenGroupItem_Data *focus_data;
	Evas_Object *setpw_notify;
	Evas_Object *notify;

	/*Elm_Object_Item *err_desc; */
	Setting_GenGroupItem_Data *err_desc;
	Elm_Genlist_Item_Class itc_err_desc;

	Evas_Object *controllbar;
	tapi_request_tapi_info *t_info;
	Elm_Genlist_Item_Class itc_variable_height;
	Elm_Genlist_Item_Class itc_layout;
	Elm_Genlist_Item_Class itc_group_item;
	int disable_item_type;

	unsigned int remain_attempt;
	unsigned int valid_sec;

	setting_pw_quality pw_quality_info;

	char *step1_str;

	/* for simple password */
	Elm_Genlist_Item_Class itc_title;
	Elm_Genlist_Item_Class itc_sp_bg_1icon;
	Elm_Object_Item *sp_item;
	Evas_Object *sp_entry1;
	Evas_Object *sp_entry2;
	Evas_Object *sp_entry3;
	Evas_Object *sp_entry4;
	int sp_focused_num;
	char sp_chars[5];

	int sp_title_index;
	char *cur_pwd;

	/*int is_simple_password_on; */
	int screen_lock_type;
	int is_description;
	int pw_status; /*int is_empty; */
	int rotated;

	bool puk_verified_flag;
	bool update_change_pin1_flag;
	bool incorrect_pin1_flag;
	bool incorrect_pin2_flag;
	bool destroy;
	Ecore_Timer *remove_timer;
	Ecore_Timer *set_history_timer;
	Ecore_Timer *focus_timer;
	/* end */

	Evas_Object *simple_layout;
	Ecore_IMF_Context *imf_context;
	Ecore_Event_Handler *imf_handler;
	Evas_Object *sp_entry[5];
	Ecore_Timer *simple_done_timer;

	/* fingerprint's alternative password */
	int cur_step;

} SettingPassword;

extern setting_view setting_view_password_sim;
extern setting_view setting_view_password_simple;
extern setting_view setting_view_password_main;

void create_popup_notitle_nobtn(void *data, char *str, int destroy);
void display_desc(void *data, char *desc, int destroy);
extern void check_attemps_left(void *data);
int check_password(const char *challenge, unsigned int *remain_attempt,
		unsigned int *valid_sec);
int set_password(const char *cur_pwd, const char *new_pwd, void *data);

#endif
