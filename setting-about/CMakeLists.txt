CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
SET(PROJECT_SETTING_ABOUT "setting-about")
SET(SETTING_PKG_ABOUT "org.tizen.setting-about")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_about REQUIRED
	elementary appcore-common appcore-efl
	ui-gadget-1 efl-extension vconf dlog dbus-glib-1
	capi-appfw-application capi-appfw-app-manager evas ecore
	tapi capi-system-info capi-system-device capi-system-runtime-info capi-system-system-settings capi-network-bluetooth
	notification capi-network-wifi json-glib-1.0 capi-telephony libtzplatform-config)

FOREACH(flag ${pkgs_about_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")

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

ADD_EXECUTABLE(${PROJECT_SETTING_ABOUT}
	./src/setting-about.c
	./src/setting-about-main.c
	./src/ui/setting-about-ui.c
	./src/setting-about-controller.c
	./src/logic/setting-about-device.c
)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_ABOUT} ${pkgs_about_LDFLAGS})
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_ABOUT} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON}/ -lsetting-common)

INSTALL(PROGRAMS ${PROJECT_SETTING_ABOUT} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_ABOUT}/bin)
INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/org.tizen.setting-about.xml DESTINATION /usr/share/packages/)
