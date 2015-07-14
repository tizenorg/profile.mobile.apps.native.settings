INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_reset REQUIRED evas ecore elementary appcore-common appcore-efl tapi json-glib-1.0 ui-gadget-1 capi-appfw-application mm-sound dlog)

FOREACH(flag ${pkgs_reset_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-fpie")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_RESET_UG} SHARED
	./src/setting-reset.c
	./src/setting-reset-main.c
	./src/setting-reset-settings.c)

TARGET_LINK_LIBRARIES(${SETTING_RESET_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_RESET_UG} ${pkgs_reset_LDFLAGS})

INSTALL(TARGETS ${SETTING_RESET_UG} DESTINATION /usr/ug/lib)
