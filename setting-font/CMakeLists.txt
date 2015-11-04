INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_font REQUIRED ecore ecore-file evas elementary appcore-common appcore-efl ui-gadget-1 capi-appfw-application
deviced fontconfig libxml-2.0 capi-system-system-settings json-glib-1.0 eventsystem)

FOREACH(flag ${pkgs_font_CFLAGS})
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

ADD_LIBRARY(${SETTING_FONT_UG} SHARED
	./src/setting-font.c
	./src/setting-font-main.c
	./src/setting-font-font-size.c
	)

TARGET_LINK_LIBRARIES(${SETTING_FONT_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_FONT_UG} ${pkgs_font_LDFLAGS})

INSTALL(TARGETS ${SETTING_FONT_UG} DESTINATION ${PREFIX}/lib/ug)
