

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_powersaving REQUIRED evas ecore edje elementary appcore-common appcore-efl devman ui-gadget-1 capi-appfw-application dlog)

FOREACH(flag ${pkgs_powersaving_CFLAGS})
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

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl, --rpath=/usr/lib")

ADD_LIBRARY(${SETTING_POWERSAVING_UG} SHARED
	./src/setting-powersaving.c
	./src/setting-powersaving-main.c
	./src/setting-powersaving-about.c
	./src/setting-powersaving-customed.c
	./src/setting-powersaving-brightness.c
)

TARGET_LINK_LIBRARIES(${SETTING_POWERSAVING_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_POWERSAVING_UG} ${pkgs_powersaving_LDFLAGS})
INSTALL(TARGETS ${SETTING_POWERSAVING_UG} DESTINATION /usr/ug/lib)

