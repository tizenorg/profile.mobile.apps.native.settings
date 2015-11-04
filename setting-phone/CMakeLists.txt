INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_phone REQUIRED evas eina ecore elementary appcore-common iniparser vconf
appcore-efl deviced tapi ui-gadget-1 capi-appfw-application appsvc libxml-2.0 badge
json-glib-1.0 ail eventsystem capi-system-system-settings)

FOREACH(flag ${pkgs_phone_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-D_FILE_OFFSET_BITS=64")
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-shared")

#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

ADD_LIBRARY(${SETTING_PHONE_UG} SHARED
	./src/setting-phone.c
	./src/setting-phone-language-region.c
	./src/setting-phone-license-main.c
	./src/setting-phone-display-language.c
	./src/setting-phone-region-format.c
	)

TARGET_LINK_LIBRARIES(${SETTING_PHONE_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}
-lsetting-common -licui18n -licudata -licuuc)
TARGET_LINK_LIBRARIES(${SETTING_PHONE_UG} ${pkgs_phone_LDFLAGS})

INSTALL(TARGETS ${SETTING_PHONE_UG} DESTINATION ${PREFIX}/lib/ug)
