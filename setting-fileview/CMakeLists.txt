SET(PROJECT_SETTING_FILEVIEW "setting-fileview")
SET(SETTING_PKG_FILEVIEW "org.tizen.setting-fileview")

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_fileview REQUIRED
	appcore-common
	appcore-efl
	capi-appfw-application
	chromium-efl
	dlog
	ecore
	elementary
	evas
	json-glib-1.0
	libtzplatform-config
	ui-gadget-1
)

FOREACH(flag ${pkgs_fileview_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_FILEVIEW}
	src/setting-fileview.c
	src/setting-fileview-main.c
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-fileview.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-fileview.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_FILEVIEW} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}/ -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_FILEVIEW} ${CMAKE_LDFLAGS} ${pkgs_fileview_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_FILEVIEW} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_FILEVIEW}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-fileview.xml DESTINATION /usr/share/packages/)

