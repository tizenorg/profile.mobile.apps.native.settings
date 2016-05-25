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
/**
 * @defgroup setting-debug
 * setting debug utility
 */

#ifndef _SETTING_DEBUG_H_
#define _SETTING_DEBUG_H_
#include <stdio.h>
#include <glib.h>

/*#define DEBUG_CODE */
#define SETTING_USING_PLATFORM_DBG
#ifdef SETTING_USING_PLATFORM_DBG
#include <dlog.h>
#ifdef LOG_TAG
#undef LOG_TAG
#endif


#define LOG_TAG "SETTING"
#endif

/*#define LAUNCHING_DEBUG_LOG */


#define USE_TIMER_UPDATE_TIME_IN_TIME_VIEW

#define SECURITY_SERVER			1

/* launching */
#ifdef LAUNCHING_DEBUG_LOG
#define LAUNCH_SETTING_IN(fmt, arg...)	LOG(LOG_DEBUG, "LAUNCH", \
		"[setting:Application:%s:IN]" fmt, __FUNCTION__, ##arg)
#define LAUNCH_SETTING_OUT(fmt, arg...) LOG(LOG_DEBUG, "LAUNCH", \
		"[setting:Application:%s:OUT]" fmt, __FUNCTION__, ##arg)
#else
#define LAUNCH_SETTING_IN(fmt, arg...)
#define LAUNCH_SETTING_OUT(fmt, arg...)
#endif

#if !defined(LOCALEDIR)
#define LOCALEDIR	_TZ_SYS_RO_APP"/org.tizen.setting/res/locale"
#endif

#if !defined(EDJDIR)
#define EDJDIR	_TZ_SYS_RO_APP"/org.tizen.setting/res/edje"
#endif

#define SUPPORT_BOTTOM_BTNS 1
#define SUPPORT_DRI 1

/*macros to control program flow*/

/* Genlist Update is in progress. */

#define FUNCTION_SYSTEM_SETTING 1
#define SETTING_SEARCH 1

/*##menu options##*/
#define SUPPORT_FONT			1
#define SUPPORT_WALLPAPER		0
#define SUPPORT_SEPARATE_BRIGHTNESS	0

#define SUPPORT_ACCESSIBILITY		1

#define SUPPORT_STORAGE			1
#define SUPPORT_WIFI_DIRECT		1
#define SUPPORT_DATA_USAGE		1
#define SUPPORT_TETHERING		0

/*#endif*/

#define SUPPORT_APP_ROATION		1

/**< 1 : turn the low batter handlding off */
#define	 LOW_BATTERY_DO_NOTHING		1


#define SUPPORT_SIMLOCK 0
#define SUPPORT_FDN 0
#define SUPPORT_ENCRYPTION 0
#define SUPPORT_SD_ENCRYPTION 0
#define SUPPORT_SCREEN_SECURITY 1

#define SUPPORT_SECURITY_FIREWALL 1
#define SUPPORT_PRIVACY 1

#define SETTING_ENABLE_TRACE

#ifdef SETTING_ENABLE_TRACE

#ifdef SETTING_USING_PLATFORM_DBG
#define SETTING_TRACE_SECURE_DEBUG(fmt, arg...) \
	do {\
		SECURE_LOGD("\033[0;32mDEBUG: " fmt "\033[0m\n", ##arg);\
	}while(0)

#define SETTING_TRACE_DEBUG(fmt, arg...) \
	do {\
		SECURE_LOGD("\033[0;32mDEBUG: " fmt "\033[0m\n", ##arg);\
	}while(0)

#define SETTING_TRACE(fmt, arg...) \
	do {\
		SECURE_LOGI("\033[0;36m" fmt "\033[0m\n", ##arg);\
	}while(0)

#define SETTING_TRACE_WARNING(fmt, arg...) \
	do {\
		SECURE_LOGI("\033[0;33mWARRING: " fmt "\033[0m\n", ##arg);\
	}while(0)

#define SETTING_TRACE_ERROR(fmt, arg...) \
	do {\
		SECURE_LOGE("\033[0;31mERROR: " fmt "\033[0m\n", ##arg);\
	}while(0)

#define SETTING_TRACE_BEGIN do {\
		SECURE_LOGD("\033[0;35mENTER FUNCTION: %s. \033[0m\n", \
			__FUNCTION__);\
	}while(0)

#define SETTING_TRACE_END  do {\
		SECURE_LOGD("\033[0;35mEXIT FUNCTION: %s. \033[0m\n", \
			__FUNCTION__);\
	}while(0)

#else

#define SETTING_TRACE(fmt, arg...) \
	do {\
		printf("\n[SETTING]\033[0;36m" fmt "\033[0m\t%s:%d\n", \
			##arg, (char*)(strrchr(__FILE__, '/')+1), __LINE__);\
	}while(0)

#define SETTING_TRACE_SECURE_DEBUG(fmt, arg...) \
	do {\
		printf("\n[SETTING]\033[0;32mDEBUG: " fmt "\033[0m\t%s:%d\n", \
			##arg, (char*)(strrchr(__FILE__, '/')+1), __LINE__);\
	}while(0)


#define SETTING_TRACE_DEBUG(fmt, arg...) \
	do {\
		printf("\n[SETTING]\033[0;32mDEBUG: " fmt "\033[0m\t%s:%d\n", \
			##arg, (char*)(strrchr(__FILE__, '/')+1), __LINE__);\
	}while(0)

#define SETTING_TRACE_WARNING(fmt, arg...) \
	do {\
		{\
			printf("[SETTING]\033[0;33mWARRING: " fmt \
			"\033[0m\t%s:%d\n", ##arg, \
			(char*)(strrchr(__FILE__, '/')+1), __LINE__);}\
	}while(0)

#define SETTING_TRACE_ERROR(fmt, arg...) \
	do {\
		{fprintf(stderr, "[SETTING]\033[0;31mERROR: " fmt \
		"\033[0m\t%s:%d\n", ##arg, (char*)(strrchr(__FILE__, '/')+1), \
		__LINE__);}\
	}while(0)

#define SETTING_TRACE_BEGIN do {\
		{\
			printf("\n[SETTING]\033[0;35mENTER FUNCTION: %s. \033[0m\t%s:%d\n", \
				__FUNCTION__, \
				(char*)(strrchr(__FILE__, '/')+1), __LINE__);\
		}\
	}while(0)

#define SETTING_TRACE_END  do {\
		{\
			printf("\n[SETTING]\033[0;35mEXIT FUNCTION: %s. \033[0m\t%s:%d\n", \
				__FUNCTION__, \
				(char*)(strrchr(__FILE__, '/')+1), __LINE__);\
		}\
	}while(0)
#endif

#else

#define SETTING_TRACE(fmt, arg...) do {} while(0)
#define SETTING_TRACE_SECURE_DEBUG(fmt, arg...) do {} while(0)
#define SETTING_TRACE_DEBUG(fmt, arg...) do {} while(0)
#define SETTING_TRACE_WARNING(fmt, arg...) do {} while(0)
#define SETTING_TRACE_ERROR(fmt, arg...) do {} while(0)
#define SETTING_TRACE_BEGIN do {} while(0)
#define SETTING_TRACE_END do {} while(0)

#endif

#define setting_retvm_if(expr, val, fmt, arg...) do { \
		if(expr) { \
			SETTING_TRACE_ERROR(fmt, ##arg); \
			return (val); \
		} \
	} while (0)

#define setting_retm_if(expr, fmt, arg...) do { \
		if(expr) { \
			SETTING_TRACE_ERROR(fmt, ##arg); \
			return; \
		} \
	} while (0)

#ifndef retm_if
#define retm_if setting_retm_if
#endif
#ifndef retvm_if
#define retvm_if setting_retvm_if
#endif

#ifndef retv_if
#define retv_if(expr, val) do { \
		if(expr) { \
			SETTING_TRACE_ERROR("(%s)", #expr); \
			return (val); \
		} \
	} while (0)
#endif

#ifndef ret_if
#define ret_if(expr) do { \
		if(expr) { \
			SETTING_TRACE_ERROR("(%s)", #expr); \
			return ; \
		} \
	} while (0)
#endif

#ifndef warn_if
#define warn_if(expr, fmt, arg...) do { \
		if (expr) { \
			SETTING_TRACE_ERROR(fmt, ##arg); \
		} \
	} while (0)
#endif


#define __FREE(del, arg) do { \
		if(arg) { \
			/*cast any argument to (void*) to avoid build warring*/\
			del((void *)(arg)); \
			arg = NULL; \
		} \
	} while (0)
#define FREE(arg) __FREE(free, arg)
#define G_FREE(arg) __FREE(g_free, arg)


/*//// */
/*a trick:To manager back pointer. eg, */
/*For using genlist, whenever Setting side and Genlist side both keep
 * pointers to a same block memory(here is genlis item data via calloc), must
 * use __BACK_POINTER_SET to bind the Setting side point into the Genlist side
 * pointer */
#define __BACK_POINTER_SET(pData) do { \
		if (pData)\
		{\
			pData->pBack = (void **)(&(pData));\
		}\
	} while (0)

#define __BACK_POINTER_UNSET(pData) do { \
		/*release Setting side poniter handlers*/\
		if (pData && pData->pBack)\
		{\
			*(pData->pBack) = NULL;\
		}\
	} while (0)

#define UNUSED __attribute__((unused))

#endif /* _SETTING_DEBUG_H_ */
