INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_network REQUIRED elementary evas ecore ecore-imf eina edje
appcore-common appcore-efl
ui-gadget-1 capi-appfw-application
capi-network-connection
tapi dlog efl-extension
capi-system-system-settings
notification json-glib-1.0 callmgr_client libtzplatform-config)

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

ADD_LIBRARY(${SETTING_NETWORK_UG} SHARED
	./src/setting-network.c
	./src/setting-network-main.c
	./src/setting-network-select-network.c
	./src/setting-network-con.c
	./src/setting-network-con-list.c
	./src/setting-network-profile-delete.c
	./src/setting-network-connection-create.c
	)

TARGET_LINK_LIBRARIES(${SETTING_NETWORK_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_NETWORK_UG} ${pkgs_network_LDFLAGS})

INSTALL(TARGETS ${SETTING_NETWORK_UG} DESTINATION ${PREFIX}/lib/ug)
