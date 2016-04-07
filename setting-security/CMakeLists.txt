INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_security REQUIRED ecore evas elementary tapi dlog
ui-gadget-1 capi-appfw-application capi-appfw-app-manager appcore-common ail
deviced capi-network-connection json-glib-1.0 efl-extension libtzplatform-config)

FOREACH(flag ${pkgs_security_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")
FOREACH(flag ${pkgs_security_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_SECURITY_UG} SHARED
	./src/setting-security.c
	./src/setting-security-main.c
	./src/setting-security-sim-settings.c
	./src/setting-security-update.c
	)

TARGET_LINK_LIBRARIES(${SETTING_SECURITY_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_SECURITY_UG} ${pkgs_security_LDFLAGS})

INSTALL(TARGETS ${SETTING_SECURITY_UG} DESTINATION ${PREFIX}/lib/ug)
