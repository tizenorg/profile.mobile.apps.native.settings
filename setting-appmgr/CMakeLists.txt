INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
INCLUDE_DIRECTORIES(/usr/include/privilegemgr)
SET(PROJECT_SETTING_APPMGR "setting-appmgr")
SET(SETTING_PKG_APPMGR "org.tizen.setting-appmgr")

FILE(GLOB APPMGR_SRCS src/*.c)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_appmgr REQUIRED
		elementary
		ui-gadget-1
		capi-appfw-application
		evas
		dlog
		edje
		glib-2.0
		appcore-efl
		eina
		pkgmgr
		appsvc
		capi-appfw-app-manager
		vconf
		pkgmgr-info
		efl-extension
		app2sd
		json-glib-1.0
		privilege-info
		capi-appfw-package-manager
		libtzplatform-config)

FOREACH(flag ${pkgs_appmgr_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")

#ADD_LIBRARY(${SETTING_MANAGE_APPS_UG} SHARED ${APPMGR_SRCS})
ADD_EXECUTABLE(${PROJECT_SETTING_APPMGR} ${APPMGR_SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_APPMGR} ${LIB_SETTING_COMMON})
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_APPMGR} ${pkgs_appmgr_LDFLAGS})
#TARGET_LINK_LIBRARIES(${PROJECT_SETTING_DISPLAY} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
#TARGET_LINK_LIBRARIES(${PROJECT_SETTING_APPMGR} security-privilege-manager)

#INSTALL(TARGETS ${PROJECT_SETTING_APPMGR} DESTINATION ${PREFIX}/lib/ug)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-appmgr.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-appmgr.xml @ONLY)
INSTALL(PROGRAMS ${PROJECT_SETTING_APPMGR} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-appmgr.xml DESTINATION /usr/share/packages/)

# install edj files
INSTALL(FILES ${CMAKE_BINARY_DIR}/font-slider.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-theme.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/settings.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-genlist.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res/edje/)

# images, icons etc
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/icons DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/images DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_APPMGR}/res FILES_MATCHING PATTERN "*.png")



