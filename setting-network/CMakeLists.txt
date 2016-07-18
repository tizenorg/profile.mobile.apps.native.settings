SET(PROJECT_SETTING_NETWORK "setting-network")
SET(SETTING_PKG_NETWORK "org.tizen.setting-network")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_network REQUIRED
	appcore-common
	appcore-efl
	callmgr_client
	capi-appfw-application
	capi-network-connection
	capi-system-system-settings
	dlog
	ecore
	ecore-imf
	edje
	efl-extension
	eina
	elementary
	evas
	json-glib-1.0
	libtzplatform-config
	notification
	tapi
	ui-gadget-1
	vconf
)

FOREACH(flag ${pkgs_network_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_NETWORK}
	./src/setting-network.c
	./src/setting-network-main.c
	./src/setting-network-select-network.c
	./src/setting-network-con.c
	./src/setting-network-con-list.c
	./src/setting-network-profile-delete.c
	./src/setting-network-connection-create.c
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-network.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-network.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_NETWORK} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_NETWORK} ${pkgs_network_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_NETWORK} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_NETWORK}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-network.xml DESTINATION /usr/share/packages/)

