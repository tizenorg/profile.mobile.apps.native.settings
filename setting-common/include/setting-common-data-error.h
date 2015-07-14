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

#ifndef __SETTING_COMMON_DATA_ERROR_H__
#define __SETTING_COMMON_DATA_ERROR_H__

#define SETTING_RETURN_SUCCESS 1
#define SETTING_RETURN_FAIL -1

/* general error (start 2)*/
typedef enum {
    SETTING_GENERAL_ERR_WRONG_PARAMETER = 2,
    SETTING_GENERAL_ERR_NOT_FIND_VALUE_SWITCH,
    SETTING_GENERAL_ERR_NULL_DATA_PARAMETER,
    SETTING_GENERAL_ERR_SGCONTROLLER_NULL
} setting_general_error;

/* draw error (start 100)*/
typedef enum {
    SETTING_DRAW_ERR_FAIL_LOAD_EDJ = 100,
    SETTING_DRAW_ERR_FAIL_TITLEBAR,
    SETTING_DRAW_ERR_FAIL_SCROLLER,
    SETTING_DRAW_ERR_FAIL_CLIENTBOX,
    SETTING_DRAW_ERR_FAIL_SOFTKEY,
    SETTING_DRAW_ERR_FAIL_LIST,

    SETTING_ENTRY_RETURN_NULL_STR_ERR,
    SETTING_ENTRY_NOT_MATCH_CURRENT_PW,
    SETTING_ENTRY_NOT_MATCH_NEW_CONF_PW,
    SETTING_ENTRY_REQUIRED_4_TO_8_DIGIT_PW,
    SETTING_ENTRY_REQUIRED_CORRECT_DIGIT_PW,
} setting_draw_error;

/* time error (start 200)*/

/* ps & view error (start 300)*/
typedef enum {
    SETTING_PS_FORK_ERR = 300,
    SETTING_PS_EXECV_ERR
} setting_ps_view_error;

/* file IO error (start 400)*/
typedef enum {
    SETTING_IO_NOT_OPEN_FILE = 400,
    SETTING_IO_IOCTL_ERR
} setting_IO_error;

/* network error (start 500)*/
typedef enum {
    SETTING_TAPI_SIM_TYPE_ERR = 500,
    SETTING_TAPI_INIT_ECORE_ERR,
    SETTING_TAPI_DEINIT_ECORE_ERR,
    SETTING_TAPI_REG_EVENT_ECORE_ERR,
    SETTING_TAPI_DEREG_EVENT_ECORE_ERR,
    SETTING_TAPI_UNKNOWN_SIM_TYPE,
    SETTING_TAPI_SIM_GET_PIN1_ERR,
    SETTING_TAPI_SIM_GET_PIN2_ERR,
    SETTING_TAPI_SIM_PIN1_STATUS_ERR,
    SETTING_DNET_RETURN_NULL_ACCESS_NAME,
    SETTING_DNET_RETURN_NULL_USER_NAME,
    SETTING_DNET_RETURN_NULL_PASSWORD,
    SETTING_DNET_RETURN_NULL_PROXY_ADDR,
    SETTING_DNET_RETURN_NULL_PROXY_PORT,
    SETTING_DNET_RETURN_NULL_HOMEURL,
    SETTING_DNET_RETURN_ERR,
    SETTING_DNET_RETURN_INVALID_PROXY_ADDR
} setting_net_error;

/* vconf error (start 600)*/
typedef enum {
    SETTING_VCONF_ERR_RETURN_STR_NULL = 600,
    SETTING_VCONF_ERR_RETURN_INVALID_INT
} setting_vconf_error;

/* multi media error (start 700)*/
typedef enum {
    SETTING_MMPLAYER_STOP_ERR = 700,
    SETTING_MMPLAYER_UNREALIZE_ERR,
    SETTING_MMPLAYER_DESTROY_ERR,
    SETTING_MMPLAYER_CREATE_ERR,
    SETTING_MMPLAYER_SETATTRS_ERR,
    SETTING_MMPLAYER_REALIZE_ERR,
    SETTING_MMPLAYER_START_ERR,
    SETTING_MMPLAYER_SET_VOLUME_ERR,
    SETTING_MMPLAYER_NOT_FOUND_SOUND_FILE
} setting_mm_error;

#endif				/* __SETTING_COMMON_DATA_ERROR_H__ */
