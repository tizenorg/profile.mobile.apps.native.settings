SET(SYSPOP_BIN mode-syspopup)
SET(SYSPOP_PKG_NAME org.tizen.mode-syspopup)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs_syspopup REQUIRED
	deviced
	capi-system-device
	feedback
	capi-appfw-application
	capi-appfw-preference
	appcore-efl
	bundle
	dlog
	libxml-2.0
	ecore
	edje
	eina
	elementary
	evas
	syspopup
	aul
	tts
	capi-media-sound-manager
	vconf
	ui-gadget-1
	notification
	alarm-service
	efl-assist
	tapi
	pkgmgr
	pkgmgr-info
)


FOREACH(flag ${pkgs_syspopup_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

#SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC -fPIE")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIC")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -fPIE")

IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
	MESSAGE("add -DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-fpie")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie -Wl,--rpath=/usr/lib")

ADD_EXECUTABLE(${SYSPOP_BIN}
	./src/mode-syspopup.c
	./src/mode-syspopup-alarmmgr.c
	./src/util.c
	)

TARGET_LINK_LIBRARIES(${SYSPOP_BIN} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${SYSPOP_BIN} ${pkgs_syspopup_LDFLAGS})

INSTALL(TARGETS ${SYSPOP_BIN} DESTINATION /usr/apps/${SYSPOP_PKG_NAME}/bin)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.mode-syspopup.xml DESTINATION /usr/share/packages/)
#INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.mode-syspopup.rule DESTINATION /etc/smack/accesses2.d)

# make mode-syspopup.edj
ADD_CUSTOM_TARGET(mode-syspopup.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/data/images
		${CMAKE_CURRENT_SOURCE_DIR}/data/mode-syspopup.edc ${CMAKE_BINARY_DIR}/mode-syspopup.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/mode-syspopup.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} mode-syspopup.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/mode-syspopup.edj DESTINATION /usr/apps/${SYSPOP_PKG_NAME}/res/edje/)

# make popup-custom.edj
ADD_CUSTOM_TARGET(popup-custom.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/data/images
		${CMAKE_CURRENT_SOURCE_DIR}/data/popup-custom.edc ${CMAKE_BINARY_DIR}/popup-custom.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/popup-custom.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} popup-custom.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/popup-custom.edj DESTINATION /usr/apps/${SYSPOP_PKG_NAME}/res/edje/)
# make ultra-popup.edj
ADD_CUSTOM_TARGET(ultra-popup.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/data/images
		${CMAKE_CURRENT_SOURCE_DIR}/data/ultra-popup.edc ${CMAKE_BINARY_DIR}/ultra-popup.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/data/ultra-popup.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} ultra-popup.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ultra-popup.edj DESTINATION /usr/apps/${SYSPOP_PKG_NAME}/res/edje/)

INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data/images DESTINATION /usr/apps/${SYSPOP_PKG_NAME}/res FILES_MATCHING PATTERN "*.png")

