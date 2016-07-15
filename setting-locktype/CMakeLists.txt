SET(PROJECT_SETTING_LOCKTYPE "setting-locktype")
SET(SETTING_PKG_LOCKTYPE "org.tizen.setting-locktype")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_locktype REQUIRED
	appcore-common
	auth-fw
	capi-appfw-application
	deviced
	dlog
	dpm
	ecore
	efl-extension
	elementary
	evas
	json-glib-1.0
	libtzplatform-config
	pkgmgr-info
	ui-gadget-1
	vconf
)

FOREACH(flag ${pkgs_locktype_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_LOCKTYPE}
	./src/setting-locktype.c
	./src/setting-locktype-main.c
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-locktype.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-locktype.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_LOCKTYPE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_LOCKTYPE} ${pkgs_locktype_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_LOCKTYPE} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_LOCKTYPE}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-locktype.xml DESTINATION /usr/share/packages/)

