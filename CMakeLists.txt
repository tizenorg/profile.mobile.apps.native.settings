CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(setting)

SET(SETTING_PKG_NAME org.tizen.setting)

SET(PREFIX "${TZ_SYS_RO_APP}/org.tizen.setting")
SET(BINDIR "${PREFIX}/bin")
SET(RESDIR "${PREFIX}/res")
SET(DATADIR "${PREFIX}/data")
SET(CONFICDIR "${PREFIX}/def_config")
SET(LOCALEDIR "${RESDIR}/locale")
SET(ICONDIR "${TZ_SYS_RO_ICONS}/default/small")
SET(EDJDIR "${RESDIR}/edje")
SET(IMAGEDIR "${RESDIR}/images")
SET(CMAKE_SKIP_BUILD_RPATH true)

#set variable
SET(SETTING_PROFILE setting-profile)
SET(SETTING_CONNECTIVITY setting-connectivity)
SET(SETTING_SECURITY setting-security)
SET(SETTING_LOCKTYPE setting-locktype)
SET(SETTING_DISPLAY setting-display)
SET(SETTING_APPLICATIONS setting-applications)
SET(SETTING_MORECONNECTIONS setting-moreconnections)
SET(SETTING_TIME setting-time)
SET(SETTING_PHONE setting-phone)
SET(SETTING_PASSWORD setting-password)
SET(SETTING_COMMON setting-common)
SET(SETTING_NETWORK setting-network)
SET(SETTING_ABOUT setting-about)
SET(SETTING_FONT setting-font)
SET(SETTING_STORAGE setting-storage)
SET(SETTING_FILEVIEW setting-fileview)
SET(SETTING_RINGTONE setting-ringtone)
SET(SETTING_APPMGR setting-appmgr)
SET(SETTING_FLIGHTMODE setting-flightmode)
#SET(SETTING_ACCESSIBILITY setting-accessibility)

SET(SETTING_PROFILE_UG setting-profile-efl)
SET(SETTING_DEVELOPEROPTION_UG setting-developeroption-efl)
SET(SETTING_SECURITY_UG setting-security-efl)
SET(SETTING_LOCKTYPE_UG setting-locktype-efl)
SET(SETTING_DISPLAY_UG setting-display-efl)
SET(SETTING_APPLICATIONS_UG setting-applications-efl)
SET(SETTING_MORECONNECTIONS_UG setting-moreconnections-efl)
SET(SETTING_TIME_UG setting-time-efl)
SET(SETTING_PHONE_UG setting-phone-efl)
SET(SETTING_NETWORK_UG setting-network-efl)
SET(SETTING_ABOUT_UG setting-about-efl)
SET(SETTING_FILEVIEW_UG setting-fileview-efl)
SET(SETTING_RINGTONE_UG setting-ringtone-efl)

SET(SETTING_PASSWORD_UG setting-password-efl)
SET(SETTING_FONT_UG setting-font-efl)
SET(SETTING_STORAGE_UG setting-storage-efl)
SET(SETTING_MANAGE_APPS_UG setting-manage-applications-efl)
SET(SETTING_FLIGHTMODE_UG setting-flightmode-efl)
#SET(SETTING_ACCESSIBILITY_UG setting-accessibility-efl)


SET(LIB_SETTING_COMMON setting-common)


#SET(GC_SECTIONS_FLAGS "-fdata-sections -ffunction-sections -Wl,--gc-sections -Werror-implicit-function-declaration -Wparentheses -feliminate-unused-debug-types -pipe -Wall -Wp, -D_FORTIFY_SOURCE=2 -fexceptions -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security -fmessage-length=0 -fdiagnostics-color=never -Wno-deprecated-declarations -Werror-implicit-function-declaration")
SET(GC_SECTIONS_FLAGS "-fdata-sections -ffunction-sections -Wl,--gc-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GC_SECTIONS_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GC_SECTIONS_FLAGS}")

ADD_DEFINITIONS("-D_TZ_SYS_RO_APP=\"${TZ_SYS_RO_APP}\"")
ADD_DEFINITIONS("-D_TZ_SYS_DATA=\"${TZ_SYS_DATA}\"")
ADD_DEFINITIONS("-D_TZ_SYS_ETC=\"${TZ_SYS_ETC}\"")
ADD_DEFINITIONS("-D_TZ_SYS_SHARE=\"${TZ_SYS_SHARE}\"")
ADD_DEFINITIONS("-D_TZ_SYS_RW_APP=\"${TZ_SYS_RW_APP}\"")
ADD_DEFINITIONS("-D_TZ_SYS_RO_UG=\"${TZ_SYS_RO_UG}\"")
ADD_DEFINITIONS("-D_TZ_SYS_RO_ICONS=\"${TZ_SYS_RO_ICONS}\"")
ADD_DEFINITIONS("-D_TZ_SYS_MEDIA=\"${TZ_SYS_MEDIA}\"")
ADD_DEFINITIONS("-D_TZ_SYS_DB=\"${TZ_SYS_DB}\"")

#Create po header file
SET(CREATE_PO_HEADER "./scripts/create_po_header.sh")
EXECUTE_PROCESS(COMMAND  ${CREATE_PO_HEADER}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE CREATE_PO_HEADER_OUTPUT
        RESULT_VARIABLE CREATE_PO_HEADER_RESULT)
MESSAGE("Create PO Header result [${CREATE_PO_HEADER_RESULT}] : ${CREATE_PO_HEADER_OUTPUT}")

#setting main view build start
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/${SETTING_COMMON}/include)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_COMMON})
ADD_SUBDIRECTORY(resource/po)



CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/org.tizen.setting.xml.in ${CMAKE_SOURCE_DIR}/org.tizen.setting.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/org.tizen.setting.xml.ref.in ${CMAKE_SOURCE_DIR}/org.tizen.setting.xml.ref @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/org.tizen.setting-about.xml.in ${CMAKE_SOURCE_DIR}/org.tizen.setting-about.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/langlist_all.xml.in ${CMAKE_SOURCE_DIR}/resource/langlist_all.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/ug-setting-shortcut-efl.xml.in ${CMAKE_SOURCE_DIR}/resource/ug-setting-shortcut-efl.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/langlist.india.xml.in ${CMAKE_SOURCE_DIR}/resource/langlist.india.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/langlist.xml.in ${CMAKE_SOURCE_DIR}/resource/langlist.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/changeable/org.tizen.setting.xml.in ${CMAKE_SOURCE_DIR}/resource/changeable/org.tizen.setting.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/resource/changeable/org.tizen.setting_font-lite.xml.in ${CMAKE_SOURCE_DIR}/resource/changeable/org.tizen.setting_font-lite.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/mime.setting.xml.in ${CMAKE_SOURCE_DIR}/mime.setting.xml @ONLY)
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/src/setting-common-internal.pc.in ${CMAKE_SOURCE_DIR}/src/setting-common-internal.pc @ONLY)


# setting bin
# setting plugin
ADD_SUBDIRECTORY(src)

# all UGs for Setting itself
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_STORAGE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_PHONE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_CONNECTIVITY})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_DISPLAY})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_NETWORK})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_APPLICATIONS})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_MORECONNECTIONS})
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_PROFILE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_SECURITY})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_LOCKTYPE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_TIME})
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_ABOUT})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_PASSWORD})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_FONT})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_FILEVIEW})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_RINGTONE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_APPMGR})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_FLIGHTMODE})
#ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/${SETTING_ACCESSIBILITY})

