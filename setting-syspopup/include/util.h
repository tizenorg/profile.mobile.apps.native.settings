/*
 * Copyright (c) 2013-2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 * You shall not disclose such Confidential Information and shall
 * use it only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 * SAMSUNG make no representations or warranties about the suitability
 * of the software, either express or implied, including but not
 * limited to the implied warranties of merchantability, fitness for
 * a particular purpose, or non-infringement.
 * SAMSUNG shall not be liable for any damages suffered by licensee as
 * a result of using, modifying or distributing this software or its derivatives.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <debug.h>

#define DEFAULT_COMPLETE_CONFIG_FLAG_DIR "/opt/usr/apps/org.tizen.mode-syspopup/res/config/"

int export_xml(char *path);
int remove_xml(char *path);

#endif
