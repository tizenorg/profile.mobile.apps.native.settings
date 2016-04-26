INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
SET(PROJECT_SETTING_FLIGHTMODE "setting-flightmode")
SET(SETTING_PKG_FLIGHTMODE "org.tizen.setting-flightmode")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_flightmode REQUIRED
	callmgr_client
	deviced
	elementary
	ecore
	edje
	evas
	dlog
	dbus-glib-1
	appcore-common
	appcore-efl
	efl-extension
	ui-gadget-1
	dlog
	aul
	json-glib-1.0
	vconf
	dbus-1
	pkgmgr-info
	tapi
	notification
	libtzplatform-config)

FOREACH(flag ${pkgs_flightmode_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_FLIGHTMODE}
	./src/setting-flightmode.c
	./src/setting-flightmode-main.c
	)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_FLIGHTMODE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_FLIGHTMODE} ${pkgs_flightmode_LDFLAGS})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-flightmode.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-flightmode.xml @ONLY)

INSTALL(PROGRAMS ${PROJECT_SETTING_FLIGHTMODE} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_FLIGHTMODE}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-flightmode.xml DESTINATION /usr/share/packages/)

