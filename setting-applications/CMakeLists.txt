INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

SET(PROJECT_SETTING_APPLICATIONS "setting-applications")
SET(SETTING_PKG_APPLICATIONS "org.tizen.setting-applications")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_display REQUIRED elementary ecore edje evas appcore-common appcore-efl ui-gadget-1 capi-appfw-application dlog capi-system-device efl-extension
deviced json-glib-1.0 capi-appfw-package-manager pkgmgr-info libtzplatform-config)

FOREACH(flag ${pkgs_display_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_APPLICATIONS}
	./src/setting-applications.c
	./src/setting-applications-main.c
	./src/setting-applications-defaultapp.c
	)

TARGET_LINK_LIBRARIES(${SETTING_APPLICATIONS_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_APPLICATIONS_UG} ${pkgs_display_LDFLAGS})

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-applications.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-applications.xml @ONLY)

INSTALL(PROGRAMS ${PROJECT_SETTING_APPLICATIONS} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPLICATIONS}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-applications.xml DESTINATION /usr/share/packages/)

