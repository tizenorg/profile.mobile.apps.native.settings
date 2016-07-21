SET(PROJECT_SETTING_PHONE "setting-phone")
SET(SETTING_PKG_PHONE "org.tizen.setting-phone")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_phone REQUIRED
	appcore-common
	appcore-efl
	appsvc
	badge
	capi-appfw-application
	capi-system-system-settings
	deviced
	ecore
	eina
	elementary
	evas
	iniparser
	json-glib-1.0
	libtzplatform-config
	libxml-2.0
	pkgmgr-info
	tapi
	ui-gadget-1
	vconf
)

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

ADD_EXECUTABLE(${PROJECT_SETTING_PHONE}
	./src/setting-phone.c
	./src/setting-phone-language-region.c
	./src/setting-phone-license-main.c
	./src/setting-phone-display-language.c
	./src/setting-phone-region-format.c
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-phone.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-phone.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PHONE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}
	-lsetting-common -licui18n -licudata -licuuc)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PHONE} ${pkgs_phone_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_PHONE} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PHONE}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-phone.xml DESTINATION /usr/share/packages/)

