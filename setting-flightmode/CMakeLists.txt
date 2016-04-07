INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_flightmode REQUIRED elementary ecore edje evas appcore-common appcore-efl ui-gadget-1 dlog aul json-glib-1.0
pkgmgr-info tapi notification libtzplatform-config)

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

ADD_LIBRARY(${SETTING_FLIGHTMODE_UG} SHARED
	./src/setting-flightmode.c
	./src/setting-flightmode-main.c
	)

TARGET_LINK_LIBRARIES(${SETTING_FLIGHTMODE_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_FLIGHTMODE_UG} ${pkgs_flightmode_LDFLAGS})

INSTALL(TARGETS ${SETTING_FLIGHTMODE_UG} DESTINATION ${PREFIX}/lib/ug)
