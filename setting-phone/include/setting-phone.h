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
#ifndef __SETTING_PHONE_H__
#define __SETTING_PHONE_H__

#include <stdio.h>
#include <Elementary.h>
#include <glib-object.h>
#include <aul.h>
#ifdef Status
#undef Status
#endif
#include <tapi_common.h>
#include <ITapiSim.h>
#include <TapiUtility.h>
#include <unicode/ucal.h>
#include <unicode/uloc.h>
#include <unicode/udat.h>
#include <unicode/ustring.h>
#include <unicode/udatpg.h>
#include <unicode/utmscale.h>
#include <unicode/ucol.h>

#include "setting-common-draw-widget.h"
#include "setting-common-view.h"
#include "setting-common-init.h"

/* Choose default programe*/
#define Display_APP_Full_Name 1
#define MAX_APP_NAME 1024
#define IMAGES_MIME_TYPE "image/png"	/* temporary implement */
#define SOUNDS_MIME_TYPE "audio/mpeg"	/* temporary implement */
#define VIDEOS_MIME_TYPE "video/3gpp"	/* temporary implement */
#define OTHERS_MIME_TYPE "application/pdf"	/* temporary implement */
#define TBL_NAME "menu"
#define MIME_FIELD_NAME "mimetype"
#define NAME_BUF_LEN (128)

#define REGION_ITEM_MAX 300

#define SETTING_PHONE_REGION_RECT_SIZE_W ((int)720 * WIDGET_SCALE_FACTOR)
#define SETTING_PHONE_REGION_RECT_SIZE_H ((int)1280 * WIDGET_SCALE_FACTOR)

#define ASK_DEREG_PHONE \
	"<linesize=44>Your phone is<br>already registered.<br>"\
	" Are you sure you<br>wish to deregister?"
#define REGISTRATION_GET_FAILED _("IDS_ST_POP_REGISTRATION_CODE_NOT_FOUND")
#define REGISTRATION_INDICATOT "IDS_ST_BODY_YOUR_DEVICE_REGISTRATION_CODE_C_PS"
#define REGISTRATION_AT "IDS_ST_BODY_REGISTER_AT"
#define DEREGISTRATION_INDICATOT \
	"<br><br>Your device<br>deregistration code:<br><br>"
#define EREGISTRATION_AT "<br><br><br>Deregister at"
#define TRANSFER_INFO \
	"IDS_ST_BODY_EXPLANATION_FOR_TRANSACTION_TRACKING_MENU_VODA"

#define REGION_FORMAT_LOWER "IDS_ST_BODY_REGION"

typedef struct _SettingPhone SettingPhone;

/**
 * Setting Phone UG context
 * all UG function has void* as an agument. this is casted back to
 * SettingPhone and the functions access app context.
 */
struct _SettingPhone {
	MainData md;

	ui_gadget_h ug_loading;

	Evas_Object *gl_lang_region;
	Evas_Object *gl_lang;
	Evas_Object *gl_region;

	Evas_Object *ly_region;

	Evas_Object *popup;
	Evas_Object *popup_clear_license;
	void (*popup_cb)(void *data, Evas_Object *obj, void *event_info);

	int intial_shaking_value;

	Evas_Object *ly_language;
	Evas_Object *ly_ticker;
	Evas_Object *ly_license;
	Evas_Object *nocontents;
	Evas_Object *ly_sub_region;

	TapiHandle *handle;

	/* [UI] notification details */
	Setting_GenGroupItem_Data *data_details_notification;
	Setting_GenGroupItem_Data *data_details_sound;
	Setting_GenGroupItem_Data *data_details_display_content;
	Setting_GenGroupItem_Data *data_details_badge;

	char *sel_string;

	/* [UI] notification */
	Setting_GenGroupItem_Data *data_n_msg;
	Setting_GenGroupItem_Data *data_n_email;
	Setting_GenGroupItem_Data *data_n_im;
	Setting_GenGroupItem_Data *data_n_fb;
	Setting_GenGroupItem_Data *data_n_twt;

	/* [UI] license */
	Evas_Object *chk_lang;
	int prev_lang; /** prev is used for special lang. */
	Setting_GenGroupItem_Data *data_tracking;
	Setting_GenGroupItem_Data *data_expiry;
	Setting_GenGroupItem_Data *divx_vod;
	Setting_GenGroupItem_Data *data_roaming_nw;

	/* divx.... */
	Setting_GenGroupItem_Data *data_reg;
	Setting_GenGroupItem_Data *data_dereg;
	Evas_Object *pop_dereg;
	int mode; /* 0: reg, 1: dereg */

	Evas_Object *chk_tracking;
	Evas_Object *chk_expiry;

	/* Roaming network */
	Setting_GenGroupItem_Data *auto_download;
	Setting_GenGroupItem_Data *manual;
	Setting_GenGroupItem_Data *always_reject;

	/*bundle *bundle_data; */
	app_control_h bundle_data;
	setting_view *view_to_load;
	TelSimImsiInfo_t imsi;
	const char *sim_lang;
	Elm_Genlist_Item_Class itc_1text;
	Elm_Genlist_Item_Class itc_1text_tb;
	Elm_Genlist_Item_Class itc_1text_1icon_2;
	Elm_Genlist_Item_Class itc_1text_1icon;
	Elm_Genlist_Item_Class itc_2text_3_parent;
	Elm_Genlist_Item_Class itc_1icon_1text_sub;
	Elm_Genlist_Item_Class itc_1icon_2text;
	Elm_Genlist_Item_Class itc_1icon_with_no_line;

	/* [view] Language & Region */
	Setting_GenGroupItem_Data *data_display_language;
	Setting_GenGroupItem_Data *data_region_fmt;
	Setting_GenGroupItem_Data *data_region_fmt_example;
	Setting_GenGroupItem_Data *data_keyboard;
	Setting_GenGroupItem_Data *data_voice_recog;
	Setting_GenGroupItem_Data *data_text_speech;
	Setting_GenGroupItem_Data *data_readout;
	Setting_GenGroupItem_Data *data_title_keyboard;
	Setting_GenGroupItem_Data *data_title_speech;

	Evas_Object *chk_region;
	int prev_region;
	char *region_keyStr[REGION_ITEM_MAX];
	char *region_desc[REGION_ITEM_MAX];
	int region_index[REGION_ITEM_MAX];
	int region_num;
	int gl_region_index;
	int gl_region_cur_index;
	int selected_region_idx;
	char *region_vconf_str;
	Elm_Genlist_Item_Class itc_2text_2;
	Elm_Genlist_Item_Class itc_bg_1icon;
	UDateTimePatternGenerator *pattern_generator;
	char *prev_locale;
	Elm_Object_Item *selected_item;
	Eina_List *app_list;
	Evas_Object *pop_progress;
	Evas_Object *search_bar;
	char search_text[MAX_SEARCH_STR_LEN + 1];
	Ecore_Idler *search_idler;
	Ecore_Animator *animator;
	Ecore_Timer *set_timer;
	Ecore_Idler *set_idler;

	Elm_Object_Item *lang_region_navi_it;

	Ecore_Timer *lang_flush_timer;
	Eina_Bool lang_change;

	Ecore_Timer *searchlist_update_timer;
	Ecore_Timer *popup_timer;

	Ecore_Timer *event_freeze_timer;

	Evas_Object *pop_progress_lang;

	char *selected_locale;
	char *region_search_id;

	int current_rotation;
};

/* extern setting_view setting_view_phone_main; */
extern setting_view setting_view_phone_language_region;
#ifdef ENABLE_TICKER_NOTI
extern setting_view setting_view_phone_ticker_notification;
extern setting_view setting_view_phone_ticker_notification_details;
#endif
extern setting_view setting_view_phone_license_main;
extern setting_view setting_view_phone_display_language;
extern setting_view setting_view_phone_region_format;

extern char *setting_phone_lang_get_region_str(const char *region);
extern char *setting_phone_lang_get_by_pattern(const char *locale,
		const char *skeleton, void *data);
extern char *setting_phone_lang_get_example_desc(const char *region,
		void *data);
/**
 * Change language string to UTF8 format, the invoker need to free the return
 * string
 *
 * @param unichars The source string will be translated.
 *
 * @return UTF8 format string.
 */
extern char *setting_phone_lang_str_to_utf8(const UChar *unichars);

/* event system */
int set_language_helper(const char *lang);
int set_regionformat_helper(const char *region);

#endif				/* __SETTING_PHONE_H__ */
