INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
		elementary
		ecore
		edje
		evas
		appcore-common
		appcore-efl
		ui-gadget-1
		capi-appfw-application
		dlog
		capi-system-device
		deviced
		efl-extension
		efl-assist
		json-glib-1.0)

FOREACH(flag ${pkgs_display_CFLAGS})
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

ADD_LIBRARY(${SETTING_MORECONNECTIONS_UG} SHARED
	./src/setting-moreconnections.c
	./src/setting-moreconnections-main.c
	)

TARGET_LINK_LIBRARIES(${SETTING_MORECONNECTIONS_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_MORECONNECTIONS_UG} ${pkgs_display_LDFLAGS})

INSTALL(TARGETS ${SETTING_MORECONNECTIONS_UG} DESTINATION ${PREFIX}/lib/ug)
