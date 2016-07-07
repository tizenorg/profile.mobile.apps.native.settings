SET(PROJECT_SETTING_ACCESSIBILITY "setting-accessibility")
SET(SETTING_PKG_ACCESSIBILITY "org.tizen.setting-accessibility")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

FILE(GLOB ACC_SRCS ./src/*.c)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_acc REQUIRED
	elementary
	ui-gadget-1
	capi-appfw-application
        evas
	dlog
	edje
	glib-2.0
	appcore-efl
	eina
	appsvc
	capi-appfw-app-manager
        vconf
	app2sd
	json-glib-1.0
	efl-extension
	libtzplatform-config
)

FOREACH(flag ${pkgs_acc_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
        ADD_DEFINITIONS("-DTARGET")
        MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

ADD_EXECUTABLE(${PROJECT_SETTING_ACCESSIBILITY} ${ACC_SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_ACCESSIBILITY} ${LIB_SETTING_COMMON})
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_ACCESSIBILITY} ${pkgs_appmgr_LDFLAGS})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-accessibility.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-accessibility.xml @ONLY)

INSTALL(PROGRAMS ${PROJECT_SETTING_ACCESSIBILITY} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_ACCESSIBILITY}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-accessibility.xml DESTINATION /usr/share/packages/)

