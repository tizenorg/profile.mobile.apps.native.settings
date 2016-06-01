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
#ifndef _SETTING_PASSWORD_STRINGS_H_
#define _SETTING_PASSWORD_STRINGS_H_

#define PASSWORD_MUST_DIGITS \
	"Password must consist of at least %d digit(s)."
#define PASSWORD_MUST_ALPHANUMERIC \
	"Password must consist of at least %d alphanumeric character(s)."
#define PASSWORD_MUST_NOT_MATCH_PREV \
	"Password must not match previous passwords."
#define ENTER_ANOTHER_PASSWORD	"IDS_ASEML_POP_ENTER_ANOTHER_PASSWORD"
#define PASSWORD_EXPIRED	"IDS_ASEML_POP_PASSWORD_EXPIRED"
#define PASSWORD_EXPIRED_CHANGE_PASSWORD \
	"Current password is expired. Change password."

#define MAX_NUM_REACHED	"Maxinum number of password entry attempts reached."

#define PASSWORD_EMPTY		_("IDS_ST_BODY_PASSWORD_EMPTY")
#define DCM_PIN_ATTEMPT_DESC2	"IDS_ST_BODY_PD_ATTEMPTS_REMAINING_JPN_DCM"
#define ATTEMPT_REMAINING_DESC	"%d attempt remaining"	/* on processing NTR */
/* "Incorrect PIN. %d attempts left" */
#define PIN_ERR_DESC		_("IDS_ST_POP_INCORRECT_PIN_PD_ATTEMPTS_LEFT")
#define PUK1_BLOCKED		"IDS_ST_BODY_PUK1_BLOCKED"

/* "%d attempt left" */
#define ATTEMPT_DESC		_("IDS_ST_POP_PD_ATTEMPT_LEFT")
/* "%d attempts left" */
#define ATTEMPTS_DESC		_("IDS_ST_POP_PD_ATTEMPTS_LEFT")
/* "Incorrect PUK1. %d attempts left" */
#define PUK1_ERR_DESC		_("IDS_ST_POP_INCORRECT_PUK1_PD_ATTEMPTS_LEFT")
/* "Incorrect PUK2. %d attempts left" */
#define PUK2_ERR_DESC		_("IDS_ST_POP_INCORRECT_PUK2_PD_ATTEMPTS_LEFT")
/* "Incorrect password" */
#define PW_ERR_DESC		_("IDS_ST_POP_INCORRECT_PASSWORD")
/* "Incorrect password" */
#define PW_ERR_REUSED		_("IDS_ST_BODY_UNABLE_TO_USE_CURRENT_PASSWORD")
/* "Please enter password again after %d seconds" */
#define PW_ERR_DELAY_DESC _("IDS_ST_BODY_ENTER_PASSWORD_AGAIN_AFTER_PD_SECONDS")
/* "Normal password : Password must contain at least %d alphanumeric
 * characters" */
#define PW_NORMAL_DESC \
	_("IDS_ST_POP_PASSWORD_MUST_CONTAIN_AT_LEAST_PD_ALPHANUMERIC_"\
			"CHARACTERS")
/* mdm and normal shared password : Passwords must consist of at least 4
 * alphanumeric characters also including at least 1 alphabetical character */
#define PW_NORMAL_AND_CONTAIN_ALPHANUMER_DESC _("IDS_ST_BODY_PASSWORDS_MUST_CO"\
		"NSIST_OF_AT_LEAST_4_ALPHANUMERIC_CHARACTERS_ALSO_INCLUDING_AT"\
		"_LEAST_1_ALPHABETICAL_CHARACTER")

#define PW_SHORT_GUIDE_CURRENT	"IDS_ST_BODY_CURRENT"
#define PW_SHORT_GUIDE_NEW	"IDS_ST_BODY_NEW"
#define PW_SHORT_GUIDE_CONFIRM	"IDS_ST_BODY_CONFIRM"

#define PW_ST_ENTER_PW		""
#define PW_ST_NEW_PW		"IDS_ST_BODY_ENTER_NEW_PASSWORD"
#define PW_ST_ENTER_PW_AGAIN	"IDS_ST_BODY_ENTER_PASSWORD_AGAIN"
#define PW_ST_ENTER_CUR_PW	"IDS_ST_BODY_ENTER_CURRENT_PASSWORD"

#define PW_ST_INCORRECT_PW_TRY_AGAIN "IDS_ST_BODY_INCORRECT_PASSWORD_TRY_AGAIN"
#define PW_ST_DO_NOT_CONTAIN_SIMPLE_SEQUENCE_HELP \
	"IDS_ST_BODY_THE_PASSWORD_MUST_NOT_CONTAIN_A_SIMPLE_SEQUENCE_HEG_1111_"\
	"OR_1234"
#define PW_ST_PW_DO_NOT_MATCH "IDS_ST_POP_PASSWORDS_DO_NOT_MATCH"
#define PW_ST_INCORRECT_PW_5TIMES_HELP \
	"Incorrect password entered 5 times<br>Try again in %d seconds"
#define PW_ST_PW_MAX_LENGTH_HELP \
	"IDS_ST_BODY_PASSWORD_MUST_CONTAIN_NO_MORE_THAN_PD_CHARACTERS_ABB"
#define PW_ST_PW_MIN_LENGTH_HELP \
	"IDS_ST_POP_PASSWORD_MUST_CONTAIN_AT_LEAST_PD_ALPHANUMERIC_CHARACTERS"
#define PW_ST_PW_MIN_LENGTH_ALPHABETIC_HELP \
	"IDS_ST_BODY_PASSWORD_MUST_CONTAIN_AT_LEAST_PD_CHARACTERS_ABB"


/* Forgot password for personal mode */
#define PW_ST_RESET_PERSONAL_MODE_HELP \
	"IDS_ST_BODY_RESET_PERSONAL_MODE_SECURITY_SETTINGS_AND_DELETE_PERSONAL"\
	"_CONTENT"
#define PW_ST_RESET_PERSONAL_MODE	"IDS_ST_BODY_RESET_PERSONAL_MODE"
#define PW_ST_FORGOT_PASSWORD		"IDS_ST_BODY_FORGOT_PASSWORD_SWA"
#define PW_ST_RESET_PERSONAL_MODE_POPUP_Q \
	"IDS_ST_POP_YOUR_PERSONAL_MODE_SECURITY_SETTINGS_WILL_BE_RESET_AND_YOU"\
	"R_PERSONAL_CONTENT_WILL_BE_DELETED_RESET_PERSONAL_MODE_Q"
#define PW_ST_RESET			"IDS_ST_BUTTON_RESET"
#define PW_ST_CANCEL			"IDS_ST_BUTTON_CANCEL_ABB"


#endif
