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

#include <setting-powersaving-about.h>
#if SUPPORT_UI0_4
#define CPU_PS_DESC_DETAIL "IDS_ST_BODY_THIS_OPTION_LIMITS_THE_MAXIMUM_CPU_PERFORMANCE_AND_HELPS_TO_CONSERVE_BATTERY_LIFE_IT_WILL_NOT_EFFECT_NORMAL_USAGE_LIKE_BROWSING_AND_VIDEO_PLAYBACK_MSG"
#define CPU_PS_DESC KeyStr_CPU_PS

#define DISPLAY_PS_DESC_DETAIL "IDS_ST_BODY_THIS_OPTION_REDUCES_THE_SCREEN_FRAME_RATE_AND_LOWERS_THE_BRIGHTNESS_IT_HELPS_TO_EXTEND_BATTERY_LIFE_WHEN_THE_SCREEN_IS_TURNED_ON"
#define DISPLAY_PS_DESC KeyStr_DISPLAY_PS

#define BG_COLOR_DESC_DETAIL "IDS_ST_BODY_THIS_OPTION_CHANGES_THE_BACKGROUND_COLOUR_BRIGHTNESS_LEVEL_SO_THAT_IT_CONSUMES_LESS_BATTERY_MSG"
#define BG_COLOR_DESC KeyStr_BG_COLOR

#define VIBRATION_DESC_DETAIL "IDS_ST_BODY_THE_VIBRATE_FUNCTION_USES_ADDITIONAL_BATTERY_POWER"
#define VIBRATION_DESC KeyStr_SCREEN_VIB
#else
#define MAIN_DESC "1. System power saving settings<br>"\
		"Set system functions below<br>"\
		"  1.Dual core max<br>"\
		"  2.Brightness<br>"\
		"  3.UI rendering speed"

#define WIFI_DESC "2. Wi-Fi<br>"\
		"Turn off Wi-Fi, Turning off Wi-Fi can extend battery life, "\
		"However, if you surf the Web frequently, using Wi-Fi can "\
		"save more battery than using the cellular network."

#define BT_DESC "3. Bluetooth<br>"\
		"Turn off Bluetooth. Not using Bluetooth headset or Car home "\
		"mode can extend battery life"

#define GPS_DESC "4. GPS<br>"\
		"Applications using GPS location data reduce battery life. "\
		"Turning off BDU GPS can extend battery life"

#define SYNC_DESC "5. Data sync<br>"\
		"Applications such as Gmail, Calendar, and Twitter are auto"\
		"synced via wireless network at specific intervals. Syncing "\
		"data manually can extend battery life"

#define HOTSPOT_DESC "6. Portable Wi-Fi hotspot<br>"\
		"Turn off Portable Wi-Fi hotspot. Sharing mobile internet"\
		" connection with other devices	can reduce battery life. "\
		"Turn off portable Wi-Fi hotspot to extend battery life."

#define BRIGHT_DESC "7. Brightness<br>"\
			"Lowering the brightness of the screen can extend battery life."\
			"Selecting Automatic brightness will adjust screen brightness"\
			" to ambient automatically"

#define ETC_DESC "8. Etc.<br>"\
		"1.Disabling Use packet data(Settings > Wireless and network >"\
		" Mobile networks) helps extend battery.<br>"\
		"2.Disabling Auto sync (Settings > Account and sync) "\
		"helps extend battery"
#endif
static int setting_powersaving_about_create(void *cb);
static int setting_powersaving_about_destroy(void *cb);
static int setting_powersaving_about_update(void *cb);
static int setting_powersaving_about_cleanup(void *cb);

setting_view setting_view_powersaving_about = {
	.create = setting_powersaving_about_create,
	.destroy = setting_powersaving_about_destroy,
	.update = setting_powersaving_about_update,
	.cleanup = setting_powersaving_about_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_powersaving_about_create(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;
	char temp[MAX_COMMON_BUFFER_LEN] ={0,};

	Evas_Object *genlist;
	setting_push_layout_navi_bar_genlist(ad->win_main_layout,
					     ad->win_get,
					     _(KeyStr_Powersaving_About),
					     _("IDS_COM_BODY_BACK"),
					     NULL,
					     setting_powersaving_about_click_back_cb,
					     NULL, ad, &genlist, ad->navi_bar);
	#if SUPPORT_UI0_4
	Setting_GenGroupItem_Data *item_data = NULL;
	snprintf(temp, MAX_COMMON_BUFFER_LEN, "1. %s<br>%s", _(CPU_PS_DESC), _(CPU_PS_DESC_DETAIL));
	item_data = setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, temp, NULL, NULL);
	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);

	memset(temp, 0x00, MAX_COMMON_BUFFER_LEN);
	snprintf(temp, MAX_COMMON_BUFFER_LEN, "2. %s<br>%s", _(VIBRATION_DESC), _(VIBRATION_DESC_DETAIL));
	item_data = setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, temp, NULL, NULL);
	elm_genlist_item_select_mode_set(item_data->item, ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY);
	#else
	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, MAIN_DESC, NULL, NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, WIFI_DESC, NULL, NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, BT_DESC, NULL, NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, GPS_DESC, NULL, NULL);
	setting_create_Gendial_field_def(genlist, &itc_multiline_text, NULL, ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, SYNC_DESC, NULL, NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, HOTSPOT_DESC, NULL,
					 NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text,
					 NULL,
					 ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, BRIGHT_DESC, NULL,
					 NULL);

	setting_create_Gendial_field_def(genlist, &itc_multiline_text, NULL, ad,
					 SWALLOW_Type_LAYOUT_SPECIALIZTION_X,
					 NULL, NULL, 0, ETC_DESC, NULL, NULL);
	#endif
	setting_view_powersaving_about.is_create = 1;

	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_about_destroy(void *cb)
{
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingPowersavingUG *ad = (SettingPowersavingUG *) cb;

	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_powersaving_about.is_create = 0;

	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_about_update(void *cb)
{
	return SETTING_RETURN_SUCCESS;
}

static int setting_powersaving_about_cleanup(void *cb)
{
	return setting_powersaving_about_destroy(cb);
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
setting_powersaving_about_click_back_cb(void *data, Evas_Object *obj,
					void *event_info)
{
	/* error check */
	setting_retm_if(data == NULL, " Data parameter is NULL");

	SettingPowersavingUG *ad = (SettingPowersavingUG *) data;
	setting_view_change(&setting_view_powersaving_about,
			    &setting_view_powersaving_main, ad);
}
