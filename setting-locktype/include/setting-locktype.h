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
#ifndef __SETTING_LOCKTYPE_H__
#define __SETTING_LOCKTYPE_H__

#include <glib.h>
#include <stdio.h>
#include <dirent.h>
#include <Elementary.h>
#include <Ecore_IMF.h>
#include <glib-object.h>

#include <setting-common-draw-widget.h>
#include <setting-common-view.h>
#include <setting-debug.h>
#include <dlog.h>
#include <app_manager.h>

/*#include <ckmc/ckmc-control.h> */
/*#include <ckmc/ckmc-type.h> */


/*///////////////////face and voice unlock */
#define SET_AS_LOCK_STR "IDS_ST_SK_SET_LITE"/*"Set as lock" */
#define CONTINUE_STR "Continue"
#define SET_UP_STR "Set it up"
#define ADAPT_VOICE_STR "Adapt Voice"

#define About_Motion_Unlock_Str "IDS_ST_HEADER_MOTION_UNLOCK"
#define About_Face_Unlock_Str "About Face Unlock"
#define Setup_Face_Unlock_Str	"Set up Face Unlock"
#define Voice_Record_BtnStr "Voice record icon"
#define Put_Face_Str "<align=middle>Put your face here</align>"
#define Show_Face_Str "Show your face"

#define Motion_Unlock_Guildtext	"Unlock device using motion<br><br>"\
	"Tap and hold the screen then tilt the device "\
	"towards you to unlock it. If the screen does not unlock, "\
	"try tiling the device more. When learn about tilt or motion "\
	"lock are set,motion activation will be automatically enabled "\
	"in Settings"

#define Motion_Unlock_Guildtext1 "IDS_ST_HEADER_UNLOCK_DEVICE_USING_MOTION_ABB"
#define Motion_Unlock_Guildtext2 "IDS_ST_BODY_TAP_AND_HOLD_THE_SCREEN_THEN_TILT_THE_DEVICE_TOWARDS_YOU_TO_UNLOCK_IT_IF_THE_SCREEN_DOES_NOT_UNLOCK_TRY_TILTING_THE_DEVICE_MORE"
#define Motion_Unlock_Guildtext3 "IDS_ST_BODY_WHEN_LEARN_ABOUT_TILT_OR_MOTION_UNLOCK_ARE_SET_MOTION_ACTIVATION_WILL_BE_AUTOMATICALLY_ENABLED_IN_SETTINGS"

#define Face_Unlock_Warring	"Warning<br><br>"\
	"Do not use face unlock while driving, it may distract you from "\
	"the road and cause an accident"

#define Face_Unlock_Warring2	"Look at your phone to Unlock it<br><br>"\
	"Keep these things in mind<br>"\
	".Face Unlock is less secure than PIN or password.<br><br>"\
	".Someone who looks similar to you could unlock your phone.<br><br>"\
	".The data used to identify your face is kept private on the phone."

#define Face_Unlock_Warring3	"For best results:<br><br>"\
	".Find an indoor spot that is not too bright or too dim.<br><br>"\
	".Hold the phone at eye level."

#define Voice_Unlock_Guildtext "Tap button below then say voice command you want to use for Unlock 4 times all at once"
#define Voice_Unlock_Guildtext2 "Use 3-5 syllable words and maintain constant intonation.<br><br>"\
	"All background music and recording will stop when you unlock the device using Face and voice<br>"
#define Voice_Recognized_text "Recognised. Now you can use your own voice command for Unlock.<br><br>"\
	"For assuring higher security. You can adapt voice command by tapping 'Adapt voice' button."

#define Adapt_voice_Str "Adapt Voice"

#define Finish_Setup_Face "You've finished!<br><br>"\
	"To unlock your phone, just look at it.<br><br>"\
	"Tip : improve face matching by having your face capture in different "\
	"conditions."

#define Speak_Now_Str					"Speak now"

#define Keystr_ScreenLockType			"IDS_ST_BODY_SCREEN_LOCK_TYPE"
#define Keystr_LockScreenOptions			"IDS_ST_BODY_LOCK_SCREEN_OPTIONS"
#define Keystr_Swipe						"IDS_ST_MBODY_SWIPE"
#define Keystr_FaceAndVoice				"IDS_ST_BODY_FACE_AND_VOICE"
#define Keystr_FingerPrint						"Fingerprint"


typedef struct _SettingLocktypeUG SettingLocktypeUG;

struct _locktype_item {
	int pw_type_num;
	char *pw_type_string;
	int (*passwd_handler)(SettingLocktypeUG *ad, void *data);
};

#define MAX_VOICE_TIME 4
#define MAX_SEC_PROFILE_NUM 100

enum {
	SETTING_LOCKTYPE_PW_PASSWORD,
	SETTING_LOCKTYPE_PW_SIMPLE_PASSWD,
	SETTING_LOCKTYPE_PW_CHANGE_PASSWD,
	SETTING_LOCKTYPE_PW_CHANGE_SIMPLE_PASSWD,
	SETTING_LOCKTYPE_PW_MAX
};

enum {
	SETTING_LOCKTYPE_VIEWTYPE_MAIN,
	SETTING_LOCKTYPE_VIEWTYPE_ENCRYPTION,
	SETTING_LOCKTYPE_VIEWTYPE_RECOVERY, /* In case that recovery password using tizen account, lockscreen can call locktype */
};

/**
 * Setting Security UG context
 * all UG function has void* as an agument. this is casted back to SettingSecurityUG
 * and the functions access app context.
 */
struct _SettingLocktypeUG {
	ui_gadget_h ug;

	/* add more variables here (move your appdata to here) */
	Evas *evas;
	Evas_Object *win_main_layout;
	Evas_Object *win_get;
	Evas_Object *notify;
	Evas_Object *video_ly;
	Evas_Object *video_ly_show_face_btn;
	Evas_Object *video_ly_content_face;
	Ecore_Timer *update_timer;
	Evas_Object *image[MAX_VOICE_TIME];
	int processed_img_num;
	Evas_Object *record_genlist;
	Elm_Object_Item *screen_lock_main_item;
	char *selected_lock_type;

	Evas_Object *ly_guild;
	Evas_Object *ly_main;
	Evas_Object *navi_bar;
	ui_gadget_h ug_passwd;
	ui_gadget_h ug_loading;
	Evas_Object *genlist;
	Evas_Object *save_popup;

	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_bg_1icon;
	Elm_Genlist_Item_Class itc_1text_1icon;
	/*Elm_Genlist_Item_Class itc_seperator;*/
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_1icon;
	Elm_Genlist_Item_Class itc_group_item;
	Elm_Genlist_Item_Class itc_2text_3_parent;

	int pw_type;

	Evas_Object *lock_type_rd;
	Setting_GenGroupItem_Data *data_sl_simple_pw;
	Setting_GenGroupItem_Data *data_sl_pw;

	int old_type;
	Setting_GenGroupItem_Data *data_locktype_none;
	Setting_GenGroupItem_Data *data_locktype_swipe;
	Setting_GenGroupItem_Data *data_locktype_fingerprint;
	Setting_GenGroupItem_Data *data_locktype_motion;
	Setting_GenGroupItem_Data *data_locktype_face;
	Setting_GenGroupItem_Data *data_locktype_simple;
	Setting_GenGroupItem_Data *data_locktype_password;
	Setting_GenGroupItem_Data *data_locktype_3rd[128];

	Ecore_Timer *update_locktype_timer;
	bool cur_enc_mode;
	bool ug_is_destroying;
	int viewtype;
	char *caller;
	char *input_pwd;
};

extern setting_view setting_view_locktype_main;


void setting_locktype_result_password_ug_cb(ui_gadget_h ug,
											app_control_h service, void *priv);
gboolean setting_locktype_create_password_sg(void *data);
void setting_locktype_destroy_password_ug_cb(ui_gadget_h ug,
											 void *priv);
void setting_locktype_layout_passwd_ug_cb(ui_gadget_h ug,
										  enum ug_mode mode, void *priv);

#endif
