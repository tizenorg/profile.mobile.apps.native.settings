INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
SET(PROJECT_SETTING_DISPLAY "setting-display")
SET(SETTING_PKG_DISPLAY "org.tizen.setting-display")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_display REQUIRED elementary ecore edje evas eventsystem
				appcore-common appcore-efl ui-gadget-1 capi-appfw-application dlog dbus-glib-1
				capi-system-device efl-extension capi-system-system-settings vconf dbus-1
				deviced json-glib-1.0 libtzplatform-config)

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

ADD_EXECUTABLE(${PROJECT_SETTING_DISPLAY}
	./src/setting-display.c
	./src/setting-display-main.c
	./src/setting-display-backlight.c
	./src/setting-display-brightness.c
	)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_DISPLAY} ${pkgs_display_LDFLAGS})
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_DISPLAY} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)


CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-display.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-display.xml @ONLY)
INSTALL(PROGRAMS ${PROJECT_SETTING_DISPLAY} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-display.xml DESTINATION /usr/share/packages/)

# install edj files
INSTALL(FILES ${CMAKE_BINARY_DIR}/font-slider.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-theme.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/settings.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-newux.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-genlist.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-new-genlist.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-proc.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-slider.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-eff.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ug-setting-manage-applications-efl.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-custompopup.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res/edje/)

# images, icons etc
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/icons DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/images DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_DISPLAY}/res FILES_MATCHING PATTERN "*.png")

