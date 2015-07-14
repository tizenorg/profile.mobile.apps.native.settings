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
#ifndef __SETTING_SECURITY_MAIN_H__
#define __SETTING_SECURITY_MAIN_H__

#include <setting-security.h>

#define SETTING_SECURITY_SD_CARD_DECYPTION_WITHOUT_SD_CARD_POPUP_HELP	"IDS_ST_POP_NO_SD_CARD_IS_INSERTED_DISABLE_ENCRYPT_SD_CARD_ANYWAY_Q"
#define SETTING_SECURITY_SD_CARD_ENCRYPTION_WITHOUT_SD_CARD_POPUP_HELP	"IDS_ST_POP_NO_SD_CARD_HAS_BEEN_INSERTED_ENCRYPTION_WILL_START_AUTOMATICALLY_WHEN_YOU_INSERT_AN_SD_CARD"
#define SETTING_SECURITY_SD_CARD_ENCRYPTION_POPUP_HELP	"IDS_ST_POP_ENCRYPT_SD_CARD_Q_OPERATION_IS_IRREVERSIBLE_AND_IF_YOU_INTERRUPT_IT_YOU_WILL_LOSE_DATA"
#define SETTING_SECURITY_DEVICE_ENCRYPTION_POPUP_HELP	"IDS_ST_BODY_ENCRYPT_DEVICE_Q_OPERATION_IS_IRREVERSIBLE_AND_IF_YOU_INTERRUPT_IT_YOU_WILL_LOSE_DATA"

void setting_security_main_vconf_change_cb(keynode_t *key, void *data);
int _handle_sim_exception(void *data, int sim_status);
#endif
