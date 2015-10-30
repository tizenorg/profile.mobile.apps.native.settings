CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_about REQUIRED
	elementary appcore-common appcore-efl
	ui-gadget-1 efl-extension
	capi-appfw-application capi-appfw-app-manager evas ecore
	tapi capi-system-info capi-system-system-settings capi-network-bluetooth
	notification capi-network-wifi json-glib-1.0 capi-telephony)

FOREACH(flag ${pkgs_about_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_ABOUT_UG} SHARED
	src/setting-about.c
	src/setting-about-main.c
)

TARGET_LINK_LIBRARIES(${SETTING_ABOUT_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}/ -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_ABOUT_UG} ${pkgs_about_LDFLAGS})

INSTALL(TARGETS ${SETTING_ABOUT_UG} DESTINATION ${PREFIX}/lib/ug)
