INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_memory REQUIRED elementary appcore-common appcore-efl ui-gadget-1 capi-appfw-application tapi devman sysman)

FOREACH(flag ${pkgs_memory_CFLAGS})
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

ADD_LIBRARY(${SETTING_MEMORY_UG} SHARED
	./src/setting-memory.c
	./src/setting-memory-main.c
	./src/setting-memory-common.c
	./src/setting-memory-status.c
	./src/setting-memory-default-memory.c
	)

TARGET_LINK_LIBRARIES(${SETTING_MEMORY_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_MEMORY_UG} ${pkgs_memory_LDFLAGS})


INSTALL(TARGETS ${SETTING_MEMORY_UG} DESTINATION /usr/ug/lib)

