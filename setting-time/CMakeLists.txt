INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_time REQUIRED evas elementary appcore-common appcore-efl ecore tapi ui-gadget-1
capi-appfw-application deviced icu-i18n icu-io icu-le icu-lx icu-uc json-glib-1.0 alarm-service eventsystem capi-system-system-settings)

FOREACH(flag ${pkgs_time_CFLAGS})
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
ADD_DEFINITIONS("-shared")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_TIME_UG} SHARED
	./src/setting-time.c
	./src/setting-time-main.c
	./src/setting-time-engine.c
	)

TARGET_LINK_LIBRARIES(${SETTING_TIME_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_TIME_UG} ${pkgs_time_LDFLAGS})

INSTALL(TARGETS ${SETTING_TIME_UG} DESTINATION ${PREFIX}/lib/ug)
