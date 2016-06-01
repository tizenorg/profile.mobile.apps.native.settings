INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_display REQUIRED elementary ecore edje evas
				appcore-common appcore-efl ui-gadget-1 capi-appfw-application dlog
				capi-system-device efl-extension capi-system-system-settings
				deviced json-glib-1.0 libtzplatform-config dpm)

FOREACH(flag ${pkgs_display_CFLAGS})
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

ADD_LIBRARY(${SETTING_DISPLAY_UG} SHARED
	./src/setting-display.c
	./src/setting-display-main.c
	./src/setting-display-backlight.c
	./src/setting-display-brightness.c
	)

TARGET_LINK_LIBRARIES(${SETTING_DISPLAY_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_DISPLAY_UG} ${pkgs_display_LDFLAGS})

INSTALL(TARGETS ${SETTING_DISPLAY_UG} DESTINATION ${PREFIX}/lib/ug)
