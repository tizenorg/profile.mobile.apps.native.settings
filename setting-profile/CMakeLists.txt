INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_profile REQUIRED
			iniparser eina ecore ecore-file evas
			elementary appcore-common appcore-efl
			ui-gadget-1 capi-appfw-application capi-appfw-app-manager vconf
			capi-media-player capi-media-sound-manager
			dlog feedback deviced capi-system-device efl-extension
			cairo mm-keysound capi-media-metadata-extractor json-glib-1.0 eventsystem notification callmgr_client capi-system-system-settings)

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

ADD_LIBRARY(${SETTING_PROFILE_UG} SHARED
	./src/setting-profile.c
	./src/setting-profile-common.c
	./src/setting-profile-sound-main.c
	)

TARGET_LINK_LIBRARIES(${SETTING_PROFILE_UG} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_PROFILE_UG} ${pkgs_profile_LDFLAGS})

INSTALL(TARGETS ${SETTING_PROFILE_UG} DESTINATION ${PREFIX}/lib/ug)

# support volume popup
SET(SETTING_VOLUME_POPUP setting_volume_popup)
ADD_EXECUTABLE(${SETTING_VOLUME_POPUP}
	./src/volume_popup.c
	./src/setting-profile-common.c
	./src/setting-profile-sound-main.c
	)
TARGET_LINK_LIBRARIES(${SETTING_VOLUME_POPUP} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_VOLUME_POPUP} ${pkgs_profile_LDFLAGS} "-pie")
INSTALL(TARGETS ${SETTING_VOLUME_POPUP} DESTINATION /usr/apps/${SETTING_PKG_NAME}/bin)
# end

