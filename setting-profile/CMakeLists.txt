INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

SET(PROJECT_SETTING_PROFILE "setting-profile")
SET(SETTING_PKG_PROFILE "org.tizen.setting-profile")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_profile REQUIRED
			iniparser eina ecore ecore-file evas
			elementary appcore-common appcore-efl
			ui-gadget-1 capi-appfw-application capi-appfw-app-manager vconf
			capi-media-player capi-media-sound-manager
			dlog feedback deviced capi-system-device efl-extension
			cairo mm-keysound capi-media-metadata-extractor json-glib-1.0 notification callmgr_client capi-system-system-settings libtzplatform-config)

FOREACH(flag ${pkgs_profile_CFLAGS})
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

ADD_EXECUTABLE(${PROJECT_SETTING_PROFILE}
	./src/setting-profile.c
	./src/setting-profile-common.c
	./src/setting-profile-sound-main.c
	)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-profile.xml.in ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-profile.xml @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-profile.xml DESTINATION /usr/share/packages/)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PROFILE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PROFILE} ${pkgs_profile_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_PROFILE} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/bin)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-theme.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/settings.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/res/edje/)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-genlist.edj DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/res/edje/)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/icons DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/res FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/images DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PROFILE}/res FILES_MATCHING PATTERN "*.png")

# support volume popup
SET(SETTING_VOLUME_POPUP setting_volume_popup)
ADD_EXECUTABLE(${SETTING_VOLUME_POPUP}
	./src/volume_popup.c
	./src/setting-profile-common.c
	./src/setting-profile-sound-main.c
	)

TARGET_LINK_LIBRARIES(${SETTING_VOLUME_POPUP} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_VOLUME_POPUP} ${pkgs_profile_LDFLAGS} "-pie")
INSTALL(TARGETS ${SETTING_VOLUME_POPUP} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_NAME}/bin)
