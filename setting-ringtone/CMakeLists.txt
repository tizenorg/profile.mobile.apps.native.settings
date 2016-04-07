CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_ringtone REQUIRED
elementary appcore-common appcore-efl ui-gadget-1 capi-appfw-application evas capi-media-metadata-extractor efl-extension json-glib-1.0
ecore edje glib-2.0 capi-media-player capi-media-sound-manager libtzplatform-config)

FOREACH(flag ${pkgs_ringtone_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(CMAKE_LDFLAGS "-Wl,-z,nodelete")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_RINGTONE_UG} SHARED
	src/setting-ringtone.c
	src/setting-ringtone-main.c
	src/setting-ringtone-util.c
)

TARGET_LINK_LIBRARIES(${SETTING_RINGTONE_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}/ -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_RINGTONE_UG} ${CMAKE_LDFLAGS} ${pkgs_ringtone_LDFLAGS})

INSTALL(TARGETS ${SETTING_RINGTONE_UG} DESTINATION ${PREFIX}/lib/ug)
