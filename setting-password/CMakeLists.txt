SET(PROJECT_SETTING_PASSWORD "setting-password")
SET(SETTING_PKG_PASSWORD "org.tizen.setting-password")

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)


INCLUDE(FindPkgConfig)

pkg_check_modules(pkgs_password REQUIRED evas ecore edje elementary appcore-common
appcore-efl deviced ecore-imf
ui-gadget-1 capi-appfw-application tapi dlog  auth-fw
notification json-glib-1.0
libtzplatform-config)

FOREACH(flag ${pkgs_password_CFLAGS})
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

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl, --rpath=/usr/lib")

ADD_EXECUTABLE(${PROJECT_SETTING_PASSWORD}
	./src/setting-password.c
	./src/setting-password-sim.c
	./src/setting-password-main.c
	./src/setting-password-simple.c
)

TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PASSWORD} -L${CMAKE_BINARY_DIR}/${SETTING_COMMON} -lsetting-common)
TARGET_LINK_LIBRARIES(${PROJECT_SETTING_PASSWORD} ${pkgs_password_LDFLAGS})

INSTALL(PROGRAMS ${PROJECT_SETTING_PASSWORD} DESTINATION ${TZ_SYS_RO_APP}/${SETTING_PKG_PASSWORD}/bin)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/org.tizen.setting-password.xml DESTINATION /usr/share/packages/)

