CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

SET(PROJECT_SETTING_RINGTONE "setting-ringtone")
SET(SETTING_PKG_RINGTONE "org.tizen.setting-ringtone")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_ringtone REQUIRED
elementary appcore-common appcore-efl ui-gadget-1 capi-appfw-application evas capi-media-metadata-extractor dlog efl-extension json-glib-1.0
ecore edje glib-2.0 capi-media-sound-manager libtzplatform-config capi-media-player vconf)

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

ADD_EXECUTABLE(${PROJECT_SETTING_RINGTONE}
	src/setting-ringtone.c
	src/setting-ringtone-main.c
	src/setting-ringtone-util.c
)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-ringtone.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-ringtone.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_RINGTONE} ${pkgs_ringtone_LDFLAGS})
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_RINGTONE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)

INSTALL(PROGRAMS ${PROJECT_SETTING_RINGTONE} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_RINGTONE}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-ringtone.xml DESTINATION /usr/share/packages/)
