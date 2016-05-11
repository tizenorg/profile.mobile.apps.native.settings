INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
INCLUDE_DIRECTORIES(/usr/include/privilegemgr)

FILE(GLOB APPMGR_SRCS src/*.c)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_appmgr REQUIRED
		elementary
		ui-gadget-1
		capi-appfw-application
		evas
		dlog
		edje
		glib-2.0
		appcore-efl
		eina
		pkgmgr
		appsvc
		capi-appfw-app-manager
		vconf
		pkgmgr-info
		efl-extension
		app2sd
		json-glib-1.0
		privilege-info
		capi-appfw-package-manager
		libtzplatform-config)

FOREACH(flag ${pkgs_appmgr_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

ADD_LIBRARY(${SETTING_MANAGE_APPS_UG} SHARED ${APPMGR_SRCS})
TARGET_LINK_LIBRARIES(${SETTING_MANAGE_APPS_UG} ${LIB_SETTING_COMMON})
TARGET_LINK_LIBRARIES(${SETTING_MANAGE_APPS_UG} ${pkgs_appmgr_LDFLAGS})
#TARGET_LINK_LIBRARIES(${SETTING_MANAGE_APPS_UG} security-privilege-manager)

INSTALL(TARGETS ${SETTING_MANAGE_APPS_UG} DESTINATION ${PREFIX}/lib/ug)
