INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

SET(PROJECT_SETTING_TIME "setting-time")
SET(SETTING_PKG_TIME "org.tizen.setting-time")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_time REQUIRED evas elementary efl-extension appcore-common appcore-efl ecore tapi ui-gadget-1 vconf sqlite3
capi-appfw-application deviced icu-i18n icu-io icu-le icu-lx icu-uc json-glib-1.0 alarm-service capi-system-system-settings libtzplatform-config)

FOREACH(flag ${pkgs_time_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_EXECUTABLE(${PROJECT_SETTING_TIME}
	./src/setting-time.c
	./src/setting-time-main.c
	./src/setting-time-engine.c
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-time.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-time.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_TIME} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_TIME} ${pkgs_time_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_TIME} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_TIME}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-time.xml DESTINATION /usr/share/packages/)

#INSTALL(FILES ${CMAKE_BINARY_DIR}/font-slider.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_ABOUT}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-theme.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_TIME}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/settings.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_TIME}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-genlist.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_TIME}/res/edje/)
