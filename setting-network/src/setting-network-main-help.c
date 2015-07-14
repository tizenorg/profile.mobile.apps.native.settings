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
#include <setting-network-main-help.h>

#define SELECT_NETWORK "<b>1. Select network</b><br>"/*1. Select network*/\
	"User can select network operator which are supported in "\
	"current location. Even if you select specific network "\
	"operator in list, you may not use it, because your SIM card"\
	" is not verified with it.<br> -Recommend : Select automatic"

#define NETWORK_MODE "<b>2. Network mode</b><br>"/*2. Network mode*/\
	"Network mode list means possible network bands to be "\
	"attached. According to the condition of current network, "\
	"phone may be attached or not with user selection."\
	"<br> -Recommend : Select automatic"

#ifdef ENABLED_PREFERRED_NETWORKS
#define PERFER_NETWORKS "<b>3. Preferred networks</b><br>"/*3. Preferred networks*/\
	"User defined network list which is stored in SIM card. "\
	"If the phone couldn`t be registered with high priority "\
	"procedure, phone will try to register to network by using "\
	"user defined network list. The first record indicates the "\
	"highest priority and the last record indicates the lowest."

#define DATA_ROAMING  "<b>4. Data roaming setting</b><br>"/*4. Data roaming setting*/\
	"User can select data roaming availability. If user sets to "\
	"'OFF', user can not use data service when phone is registered "\
	"to roaming network (not home network) currently."
#else
#define PERFER_NETWORKS ""
#define DATA_ROAMING  "<b>3. Data roaming setting</b><br>"/*3. Data roaming setting*/\
	"User can select data roaming availability. If user sets to "\
	"'OFF', user can not use data service when phone is registered "\
	"to roaming network (not home network) currently."
#endif

static int setting_network_main_help_create(void *cb);
static int setting_network_main_help_destroy(void *cb);
static int setting_network_main_help_update(void *cb);
static int setting_network_main_help_cleanup(void *cb);

setting_view setting_view_network_main_help = {
	.create = setting_network_main_help_create,
	.destroy = setting_network_main_help_destroy,
	.update = setting_network_main_help_update,
	.cleanup = setting_network_main_help_cleanup,
};

/* ***************************************************
 *
 *basic func
 *
 ***************************************************/

static int setting_network_main_help_create(void *cb)
{
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	SETTING_TRACE_BEGIN;
	SettingNetworkUG *ad = (SettingNetworkUG *) cb;

	Evas_Object *genlist;
	setting_push_layout_navi_bar_genlist(ad->win_main_layout,
	                                     ad->win_get,
	                                     _("IDS_ST_HEADER_HELP"),
	                                     _("IDS_ST_BUTTON_BACK"),
	                                     NULL,
	                                     setting_network_main_help_click_back_cb,
	                                     NULL, ad, &genlist,
	                                     ad->navi_bar);

	ADD_GL_LABLE(genlist, SELECT_NETWORK);
	ADD_GL_LABLE(genlist, NETWORK_MODE);

#ifdef ENABLED_PREFERRED_NETWORKS
	ADD_GL_LABLE(genlist, PERFER_NETWORKS);
#endif
	ADD_GL_LABLE(genlist, DATA_ROAMING);

	setting_view_network_main_help.is_create = 1;
	SETTING_TRACE_END;

	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_help_destroy(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);

	SettingNetworkUG *ad = (SettingNetworkUG *) cb;
	elm_naviframe_item_pop(ad->navi_bar);
	setting_view_network_main_help.is_create = 0;
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_help_update(void *cb)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retv_if(cb == NULL, SETTING_GENERAL_ERR_NULL_DATA_PARAMETER);
	return SETTING_RETURN_SUCCESS;
}

static int setting_network_main_help_cleanup(void *cb)
{
	SETTING_TRACE_BEGIN;
	return setting_network_main_help_destroy(cb);
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
setting_network_main_help_click_back_cb(void *data, Evas_Object *obj,
                                        void *event_info)
{
	SETTING_TRACE_BEGIN;
	/* error check */
	retm_if(data == NULL, "Data parameter is NULL");

	SettingNetworkUG *ad = (SettingNetworkUG *) data;
	setting_view_change(&setting_view_network_main_help,
	                    &setting_view_network_main, ad);
}
