SET(PROJECT_SETTING_CONNECTIVITY "setting-connectivity")
SET(SETTING_PKG_CONNECTIVITY "org.tizen.setting-connectivity")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_connectivity REQUIRED
	appcore-common
	appcore-efl
	capi-appfw-app-manager
	capi-appfw-application
	capi-system-runtime-info
	dlog
	elementary
	evas
	json-glib-1.0
	libtzplatform-config
	ui-gadget-1
	vconf
)

FOREACH(flag ${pkgs_connectivity_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_CONNECTIVITY}
	./src/setting-connectivity.c
	./src/setting-connectivity-usb.c
	./src/setting-connectivity-usb-help.c
	)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-connectivity.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-connectivity.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_CONNECTIVITY} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_CONNECTIVITY} ${pkgs_connectivity_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_CONNECTIVITY} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_CONNECTIVITY}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-connectivity.xml DESTINATION /usr/share/packages/)

