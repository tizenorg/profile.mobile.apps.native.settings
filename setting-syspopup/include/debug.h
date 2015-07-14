/*
 * mode-syspopup, debug
 *
 * Copyright 2012  Samsung Electronics Co., Ltd
 *
 * Licensed under the Flora License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.tizenopensource.org/license
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <dlog.h>

#if !defined(PACKAGE)
#define PACKAGE "mode-syspopup"
#endif

#if !defined(LOCALEDIR)
#define LOCALEDIR "/usr/apps/org.tizen.mode-syspopup/res/locale"
#endif

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#ifndef retv_if
#define retv_if(expr, val) do { \
		if(expr) { \
			return (val); \
		} \
	} while (0)
#endif
#ifndef ret_if
#define ret_if(expr) do { \
		if(expr) { \
			return ; \
		} \
	} while (0)
#endif

#define LOG_TAG "MODE_SYSPOPUP"

#define ENABLE_LOG

#ifdef ENABLE_LOG
#define _DBG(fmt, arg...) LOGD(fmt, ##arg)
#define _INFO(fmt, arg...) LOGI(fmt, ##arg)
#define _ERR(fmt, arg...) LOGE(fmt, ##arg)
#else
#define _DBG(fmt, arg...)
#define _INFO(fmt, arg...)
#define _ERR(fmt, arg...)
#endif

#define goto_if(expr, val) do { \
		if(expr) { \
			_ERR("(%s) -> goto", #expr); \
			goto val; \
		} \
	} while (0)

#ifdef _
#undef _
#endif

#endif /*_DEBUG_H_ */