INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_locktype REQUIRED ecore evas elementary dlog efl-extension
ui-gadget-1 capi-appfw-application appcore-common ail
deviced pkgmgr-info
)

FOREACH(flag ${pkgs_locktype_CFLAGS})
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

ADD_LIBRARY(${SETTING_LOCKTYPE_UG} SHARED
	./src/setting-locktype.c
	./src/setting-locktype-main.c
	)

TARGET_LINK_LIBRARIES(${SETTING_LOCKTYPE_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_LOCKTYPE_UG} ${pkgs_locktype_LDFLAGS})

INSTALL(TARGETS ${SETTING_LOCKTYPE_UG} DESTINATION ${PREFIX}/lib/ug)
