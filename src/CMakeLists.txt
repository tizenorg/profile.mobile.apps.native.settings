INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_main REQUIRED
					  glib-2.0 gthread-2.0 elementary
					  appcore-common appcore-efl
					  tapi
					  dlog json-glib-1.0
					  ui-gadget-1
					  capi-appfw-application aul
					  capi-appfw-preference
					  ecore-imf eina capi-system-sensor
					  ecore ecore-file ecore-input
					  deviced icu-i18n icu-io icu-le icu-lx icu-uc
					  appsvc libxml-2.0
					  capi-system-system-settings
					  capi-network-wifi
					  capi-network-bluetooth
					  dbus-glib-1
					  capi-media-player
					  capi-media-sound-manager
					  feedback
					  capi-system-device
					  cairo
					  alarm-service
					  notification
					  capi-appfw-app-manager
					  pkgmgr-info
					  sqlite3
					  accounts-svc
					  capi-media-image-util
					  efl-extension
					  callmgr_client
					  tizen-config-fw
				  )

FOREACH(flag ${pkgs_main_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -Werror-implicit-function-declaration")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")
SET(BUILD_ARCH "${ARCH}")

IF (BINARY_RELEASE_TYPE_ENG)
	MESSAGE("eng binary--------------------------------")
	ADD_DEFINITIONS("-DBINARY_RELEASE_TYPE_ENG")
ENDIF (BINARY_RELEASE_TYPE_ENG)

IF("${BUILD_ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("********************** add -DTARGET")
ENDIF("${BUILD_ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DICONDIR=\"${ICONDIR}\"")
ADD_DEFINITIONS("-DEDJEDIR=\"${EDJEDIR}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-DSETTING_PLUGIN")
ADD_DEFINITIONS("-fpie")

ADD_DEFINITIONS("-DXP_UNIX")


ADD_EXECUTABLE(${PROJECT_NAME}
	./setting.c
	./setting-handler.c
	./setting-helper.c
	./setting-main.c
	)

# conf_tool
SET(SETTING_CONFUTIL setting_conf_util)
ADD_EXECUTABLE(${SETTING_CONFUTIL}
	./conf_util/setting_confutil.c
	)
TARGET_LINK_LIBRARIES(${SETTING_CONFUTIL} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_CONFUTIL} ${pkgs_main_LDFLAGS} "-pie")
INSTALL(TARGETS ${SETTING_CONFUTIL} DESTINATION /usr/apps/${SETTING_PKG_NAME}/bin)

####FOR TEST>>>>########
TARGET_LINK_LIBRARIES(${PROJECT_NAME} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)


# dmalloc
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_main_LDFLAGS} "-pie")

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION /usr/apps/${SETTING_PKG_NAME}/bin)

# support help ringtone
SET(SETTING_HELP_RINGTONE setting_help_ringtone)
ADD_EXECUTABLE(${SETTING_HELP_RINGTONE}
                 ./setting-support-shortcut/support_help_ringtone.c
         )
TARGET_LINK_LIBRARIES(${SETTING_HELP_RINGTONE} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SETTING_HELP_RINGTONE} ${pkgs_main_LDFLAGS} "-pie")
INSTALL(TARGETS ${SETTING_HELP_RINGTONE} DESTINATION /usr/apps/${SETTING_PKG_NAME}/bin)
# end


# make font-slider.edj
ADD_CUSTOM_TARGET(font-slider.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/font-slider.edc ${CMAKE_BINARY_DIR}/font-slider.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/font-slider.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} font-slider.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/font-slider.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-theme.edj
ADD_CUSTOM_TARGET(setting-theme.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/setting-theme.edc ${CMAKE_BINARY_DIR}/setting-theme.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-theme.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-theme.edj)

INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-theme.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make settings.edj
ADD_CUSTOM_TARGET(settings.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/settings.edc ${CMAKE_BINARY_DIR}/settings.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/settings.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} settings.edj)

INSTALL(FILES ${CMAKE_BINARY_DIR}/settings.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-newux.edj
ADD_CUSTOM_TARGET(setting-newux.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons
		${CMAKE_SOURCE_DIR}/resource/setting-newux.edc ${CMAKE_BINARY_DIR}/setting-newux.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-newux.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-newux.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-newux.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-genlist.edj
ADD_CUSTOM_TARGET(setting-genlist.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/setting-genlist.edc ${CMAKE_BINARY_DIR}/setting-genlist.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-genlist.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-genlist.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-genlist.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

ADD_CUSTOM_TARGET(setting-new-genlist.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/setting-new-genlist.edc
		${CMAKE_BINARY_DIR}/setting-new-genlist.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-new-genlist.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-new-genlist.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-new-genlist.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-proc.edj
ADD_CUSTOM_TARGET(setting-proc.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons
		${CMAKE_SOURCE_DIR}/resource/setting-proc.edc ${CMAKE_BINARY_DIR}/setting-proc.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-proc.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-proc.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-proc.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-slider.edj
ADD_CUSTOM_TARGET(setting-slider.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -sd ${CMAKE_SOURCE_DIR}/resource/sounds
		${CMAKE_SOURCE_DIR}/resource/setting-slider.edc
		${CMAKE_BINARY_DIR}/setting-slider.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-slider.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-slider.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-slider.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-eff.edj
ADD_CUSTOM_TARGET(setting-eff.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -id ${CMAKE_SOURCE_DIR}/resource/icons
		${CMAKE_SOURCE_DIR}/resource/setting-proc.edc ${CMAKE_BINARY_DIR}/setting-eff.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-proc.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-eff.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-eff.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make ug-setting-manage-applications-efl.edj
ADD_CUSTOM_TARGET(ug-setting-manage-applications-efl.edj
		COMMAND edje_cc
		${CMAKE_SOURCE_DIR}/resource/ug-setting-manage-applications-efl.edc ${CMAKE_BINARY_DIR}/ug-setting-manage-applications-efl.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/ug-setting-manage-applications-efl.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} ug-setting-manage-applications-efl.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ug-setting-manage-applications-efl.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

# make setting-custompopup.edj
ADD_CUSTOM_TARGET(setting-custompopup.edj
		COMMAND edje_cc -id ${CMAKE_SOURCE_DIR}/resource/images -sd ${CMAKE_SOURCE_DIR}/resource/sounds -id ${CMAKE_SOURCE_DIR}/resource/icons
		${CMAKE_SOURCE_DIR}/resource/setting-custompopup.edc
		${CMAKE_BINARY_DIR}/setting-custompopup.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/resource/setting-custompopup.edc
)

ADD_DEPENDENCIES(${PROJECT_NAME} setting-custompopup.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/setting-custompopup.edj DESTINATION /usr/apps/${SETTING_PKG_NAME}/res/edje/)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/resource/langlist.xml DESTINATION /usr/apps/${SETTING_PKG_NAME}/def_config)
#------------------------------------------------------------------------------------------

# install desktop file & icon
SET(PREFIX ${CMAKE_INSTALL_PREFIX})

INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.setting.xml DESTINATION /usr/share/packages/)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.setting.xml.ref DESTINATION /usr/share/packages/)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/${SETTING_PKG_NAME}.png DESTINATION /usr/share/icons/default/small)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/icons DESTINATION /usr/apps/${SETTING_PKG_NAME}/res FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/images DESTINATION /usr/apps/${SETTING_PKG_NAME}/res FILES_MATCHING PATTERN "*.png")


# install file for help app
#SET(HELPDATADIR_PH "/usr/apps/org.tizen.setting/shared/res")
#INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/help DESTINATION ${HELPDATADIR_PH} PATTERN "*")

# install media-data ; should be at /usr/share
IF("${BUILD_ARCH}" STREQUAL "arm")
	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/media-data-sdk/settings DESTINATION /usr/apps/org.tizen.setting/shared/res PATTERN "*")
ELSE("${BUILD_ARCH}" STREQUAL "arm")
	INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/resource/media-data-sdk/settings DESTINATION /usr/apps/org.tizen.setting/shared/res PATTERN "*")
ENDIF("${BUILD_ARCH}" STREQUAL "arm")

# install changeable UI code
INSTALL(FILES ${CMAKE_SOURCE_DIR}/resource/changeable/org.tizen.setting.xml DESTINATION /usr/apps/org.tizen.setting/shared/res/tables)

INSTALL(FILES ${CMAKE_SOURCE_DIR}/mime.setting.xml DESTINATION /usr/share/mime/packages)


#------------------------------------------------------------------
#add setting-sfinder-plugin
#ADD_SUBDIRECTORY(setting-sfinder-plugin)
#------------------------------------------------------------------

#-----------------------------------------------------------------------------------
CONFIGURE_FILE(
	setting-common-internal.pc
    ${CMAKE_CURRENT_SOURCE_DIR}/setting-common-internal.pc
    @ONLY
)
INSTALL(FILES ${CMAKE_SOURCE_DIR}/src/setting-common-internal.pc DESTINATION ${LIBDIR}/pkgconfig)
#-----------------------------------------------------------------------------------

# install firewall script for FOTA(210.org.tizen.setting.patch.sh)
INSTALL(PROGRAMS ${CMAKE_SOURCE_DIR}/210.org.tizen.setting.patch.sh DESTINATION /etc/opt/upgrade/)

