SET(PROJECT_SETTING_MORECONNECTIONS "setting-moreconnections")
SET(SETTING_PKG_MORECONNECTIONS "org.tizen.setting-moreconnections")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_moreconnections REQUIRED
		appcore-common
		appcore-efl
		capi-appfw-application
		#capi-location-manager
		capi-system-device
		deviced
		dlog
		ecore
		edje
		efl-extension
		elementary
		evas
		json-glib-1.0
		libtzplatform-config
		ui-gadget-1
		vconf
)

FOREACH(flag ${pkgs_moreconnections_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_MORECONNECTIONS}
	./src/setting-moreconnections.c
	./src/setting-moreconnections-main.c
)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-moreconnections.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-moreconnections.xml @ONLY)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_MORECONNECTIONS} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_MORECONNECTIONS} ${pkgs_moreconnections_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_MORECONNECTIONS} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_MORECONNECTIONS}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-moreconnections.xml DESTINATION /usr/share/packages/)

