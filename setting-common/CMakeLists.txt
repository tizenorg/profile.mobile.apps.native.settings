#set variable
# LIB_SETTING_COMMON - setting-common

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${LIB_SETTING_COMMON})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_common_data REQUIRED appcore-efl elementary vconf openssl evas
ecore ecore-imf json-glib-1.0 edje glib-2.0 libxml-2.0 ui-gadget-1 alarm-service cairo
capi-appfw-application capi-system-info capi-system-system-settings pkgmgr-info efl-extension libtzplatform-config
capi-appfw-app-common notification feedback capi-media-player capi-media-sound-manager dbus-1 icu-i18n icu-io icu-le icu-lx icu-uc libtzplatform-config)


FOREACH(flag ${pkgs_common_data_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl, --rpath=/usr/lib")

#make lib-${LIB_SETTING_COMMON}.so files
ADD_LIBRARY(${LIB_SETTING_COMMON} SHARED
	./src/setting-common-data-slp-setting.c
	./src/setting-common-resource.c
	./src/setting-common-view.c
	./src/setting-common-draw-widget.c
	./src/setting-common-draw-searchbar.c
	./src/setting-common-draw-popup.c
	./src/setting-common-draw-naviframe.c
	./src/setting-common-draw-genlist.c
	./src/setting-common-general-func.c
	./src/setting-common-sound.c
	./src/setting-common-alarmmgr.c
	./src/setting-cfg.c
)

TARGET_LINK_LIBRARIES(${LIB_SETTING_COMMON} ${pkgs_common_data_LDFLAGS})
SET_TARGET_PROPERTIES(${LIB_SETTING_COMMON} PROPERTIES VERSION 0.1.0 SOVERSION 1)
INSTALL(TARGETS ${LIB_SETTING_COMMON} DESTINATION ${LIBDIR})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-common-data-type.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-common-draw-widget.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-common-general-func.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-common-resource.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-cfg.h DESTINATION ${INCLUDEDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/setting-common-data-error.h DESTINATION ${INCLUDEDIR})

