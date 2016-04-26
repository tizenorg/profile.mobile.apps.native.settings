%bcond_with x
%bcond_with wayland


Name:       org.tizen.setting
Summary:    Setting application
Version:    1.0.1
Release:    99
Group:      misc
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
Source1:    packaging/firewall.service

%if "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif
%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  pkgconfig(appcore-common)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(bluetooth-api)
BuildRequires:  pkgconfig(mm-sound)
BuildRequires:  pkgconfig(mm-keysound)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(ui-gadget-1)
BuildRequires:  pkgconfig(json-glib-1.0)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(icu-i18n)
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(gthread-2.0)
BuildRequires:  pkgconfig(dbus-glib-1)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(icu-io)
BuildRequires:  pkgconfig(icu-le)
BuildRequires:  pkgconfig(icu-lx)
BuildRequires:  pkgconfig(icu-uc)
#BuildRequires:  pkgconfig(ewebkit2)
BuildRequires:  pkgconfig(chromium-efl)
BuildRequires:  pkgconfig(accounts-svc)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(mm-player)
BuildRequires:  pkgconfig(capi-network-connection)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(cairo)
BuildRequires:  pkgconfig(capi-system-runtime-info)
BuildRequires:  pkgconfig(capi-media-player)
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(capi-content-media-content)
BuildRequires:  pkgconfig(capi-system-device)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-appfw-preference)
BuildRequires:  pkgconfig(capi-system-sensor)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-runtime-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(capi-media-metadata-extractor)
BuildRequires:  pkgconfig(capi-appfw-app-common)
BuildRequires:  pkgconfig(shortcut)
BuildRequires:  pkgconfig(ecore-evas)
BuildRequires:  pkgconfig(pkgmgr)
BuildRequires:  pkgconfig(storage)
BuildRequires:  pkgconfig(pkgmgr-info)
BuildRequires:  pkgconfig(iniparser)
BuildRequires:  pkgconfig(tts)
BuildRequires:  pkgconfig(alarm-service)
BuildRequires:  pkgconfig(contacts-service2)
BuildRequires:  pkgconfig(deviced)
BuildRequires:  pkgconfig(badge)
BuildRequires:  pkgconfig(efl-extension)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(notification)
BuildRequires:	pkgconfig(app2sd)
BuildRequires:	pkgconfig(capi-media-image-util)
BuildRequires:  pkgconfig(capi-appfw-package-manager)
BuildRequires:  pkgconfig(capi-telephony)
BuildRequires:  pkgconfig(eventsystem)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(callmgr_client)
BuildRequires:  pkgconfig(privilege-info)
#BuildRequires:	pkgconfig(capi-location-manager)
BuildRequires:  pkgconfig(auth-fw)
BuildRequires:  libcap-devel
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
#BuildRequires:  security-privilege-manager-devel
BuildRequires:  hash-signer
#BuildRequires:  system-resource-devel-meta
#Requires: libeas-common
Requires: security-privilege-manager
Requires(post): attr

%description
Setting application
 BEAT UI, Setting application.

%package ref
Summary: replacement for org.tizen.setting.xml
Group: Application/Core Applications
Requires(post): org.tizen.setting

%description ref
replace for org.tizen.setting.xml
 need to check

%package devel
Summary: A setting common library (Development)
Group: Application Framework/Settings

#Requires: %{name} = %{version}-%{release}
#Requires: pkgconfig(setting-common-internal)
%description devel

%package about
Summary: The setting about module
Group: Application

%description about
Application displaying device information

%prep
mkdir -p %{buildroot}%{TZ_SYS_SHARE}/settings
%setup -q

%build

%define PREFIX    %{_prefix}/apps/org.tizen.setting
%define OPTPREFIX %{TZ_SYS_RW_APP}/org.tizen.setting
%define RESDIR    %{PREFIX}/res
#%define OPTSHAREREFIX  %{TZ_SYS_RO_APP}/org.tizen.setting/shared/res/settings

%define CONFIGDIR    %{PREFIX}/def_config
%define IMAGEDIR    %{RESDIR}/images
CFLAGS+=" -fPIC -fvisibility=hidden ";export CFLAGS
CXXFLAGS+=" -fPIC -fvisibility=hidden ";export CFLAGS

LDFLAGS+="-Wl,--rpath=%{PREFIX}/lib -Wl,--hash-style=both -Wl,--as-needed";export LDFLAGS

%if 0%{?sec_build_binary_debug_enable}
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"
%endif

%if 0%{?tizen_build_binary_release_type_eng}
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
%endif

%if "%{tizen_target_name}" == "Z130H"
export CFLAGS+="$CFLAGS -D_Z1"
%endif

%ifarch %{arm}
export CFLAGS="$CFLAGS -DTIZEN_BUILD_TARGET"
%else
export CFLAGS="$CFLAGS -DTIZEN_BUILD_EMULATOR"
%endif

%ifarch %{arm}
	cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DARCH=arm -DLIBDIR=%{_libdir} -DINCLUDEDIR=%{_includedir} \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE \
%else
-Dwith_x=TRUE \
%endif
%else
	cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DARCH=x86 -DLIBDIR=%{_libdir} -DINCLUDEDIR=%{_includedir} \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE \
%else
-Dwith_x=TRUE \
%endif
%endif
%if 0%{?tizen_build_binary_release_type_eng}
		-DBINARY_RELEASE_TYPE_ENG=YES \
%endif
	-DTZ_SYS_DATA=%{TZ_SYS_DATA} \
	-DTZ_SYS_ETC=%{TZ_SYS_ETC} \
	-DTZ_SYS_SHARE=%{TZ_SYS_SHARE} \
	-DTZ_SYS_RO_APP=%{TZ_SYS_RO_APP} \
	-DTZ_SYS_RW_APP=%{TZ_SYS_RW_APP} \
	-DTZ_SYS_RO_UG=%{TZ_SYS_RO_UG} \
	-DTZ_SYS_RO_ICONS=%{TZ_SYS_RO_ICONS} \
	-DTZ_SYS_MEDIA=%{TZ_SYS_MEDIA} \
	-DTZ_SYS_DB=%{TZ_SYS_DB}

# build all
make

%install
rm -rf %{buildroot}
%make_install

%define tizen_sign 1
%define tizen_sign_base %{PREFIX}
%define tizen_sign_level platform
%define tizen_author_sign 1
%define tizen_dist_sign 1

mkdir -p %{buildroot}%{_prefix}/apps/org.tizen.mode-syspopup/data
mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants
mkdir -p %{buildroot}%{_libdir}/systemd/system/default.target.wants

%clean

%post
#if [ ! -d %{OPTPREFIX} ]
#then
#	echo "CREATE %{OPTPREFIX} DIR"
#    mkdir -p %{OPTPREFIX}
#	echo "CREATE %{OPTPREFIX}/data DIR"
#    mkdir -p %{OPTPREFIX}/data
#fi
#chmod a+w %{OPTPREFIX}/data



update-mime-database %{_prefix}/share/mime

# Set vconf values with -g/-u options
GOPTION="-g 6514"

#resetSound
#DEFAULT_CALL_TONE="%{OPTSHAREREFIX}/Ringtones/ringtone_sdk.mp3"
#DEFAULT_NOTI_TONE="%{OPTSHAREREFIX}/Alerts/General notification_sdk.wav"

	DEFAULT_CALL_TONE="%{TZ_SYS_SHARE}/settings/Ringtones/ringtone_sdk.mp3"
	DEFAULT_NOTI_TONE="%{TZ_SYS_SHARE}/Alerts/General notification_sdk.wav"



#resetImages
#	DEFAULT_HOME="%{OPTSHAREREFIX}/Wallpapers/Home_default.jpg"
#	DEFAULT_LOCK="%{OPTSHAREREFIX}/Wallpapers/Default.jpg"
	DEFAULT_HOME="%{TZ_SYS_SHARE}/Wallpapers/Home_default.jpg"
	DEFAULT_LOCK="%{TZ_SYS_SHARE}/Wallpapers/Default.jpg"

	#rm -f %{_sysconfdir}/localtime
	#ln -s %{_datadir}/zoneinfo/Asia/Seoul %{_sysconfdir}/localtime

#if [ -e /etc/localtime ]
#then
#	rm -f /etc/localtime
#	ln -s /usr/share/zoneinfo/Asia/Seoul /etc/localtime
#fi

#resetSecurity
	rm -rf {_prefix}/data/setting/set_info

# for support shared menu icons
#ln -s %{RESDIR}/icons %{PREFIX}/shared/res/icons

#APP_PATH_INFO=`%{PREFIX}/bin/setting_conf_util get_data_path`
#echo "-------------------------------------------------------------> "$APP_PATH_INFO
sync

mkdir -p %{_prefix}/ug/bin/
# help directory
mkdir -p %{PREFIX}/shared
mkdir -p %{PREFIX}/shared/res
mkdir -p %{_prefix}/apps/org.tizen.mode-syspopup/res/edje
mkdir -p %{_prefix}/apps/org.tizen.mode-syspopup/bin/

%post ref
rm -rf %{_datadir}/packages/org.tizen.setting.xml
mv %{_datadir}/packages/org.tizen.setting.xml.ref %{_datadir}/packages/org.tizen.setting.xml

%post about
mkdir -p %{_prefix}/apps/org.tizen.setting-about/shared
mkdir -p %{_prefix}/apps/org.tizen.settung-about/shared/res

%posttrans

%files -n org.tizen.setting
%manifest org.tizen.setting.manifest

%{PREFIX}/author-signature.xml
%{PREFIX}/signature1.xml

# Firewall -------------------------------------------------------
%{_sysconfdir}/opt/upgrade/210.org.tizen.setting.patch.sh

%defattr(-,root,root,-)
#%{_prefix}/apps/org.tizen.mode-syspopup/res/locale/*

%{PREFIX}/bin/setting
%{CONFIGDIR}/*
%{PREFIX}/bin/setting_conf_util
#%{PREFIX}/bin/setting_turnoff_light
%{PREFIX}/bin/setting_volume_popup

%attr(0755,root,root) %{PREFIX}/bin/setting_help_ringtone

%{_libdir}/lib*.so.*
%{_datadir}/packages/org.tizen.setting.xml
%{_datadir}/icons/default/small/org.tizen.setting.png

#%{_datadir}/settings/*

%{PREFIX}/res/*
%{_datadir}/mime/packages/*

%{TZ_SYS_SHARE}/settings/*
#%{OPTSHAREREFIX}/*

%{PREFIX}/shared/res/*
%attr(-,app,app) %dir %{PREFIX}/shared

#[  124s] error: File not found by glob: /usr/src/packages/BUILDROOT/org.tizen.setting-0.2.0-99.aarch64/usr/share/settings/*

# new
%{PREFIX}/lib/ug/*

%files devel
#---------------------------------------------
# moved from org.tizen.setting
#---------------------------------------------
%{_libdir}/lib*.so
%{PREFIX}/lib/ug/*

%{_libdir}/pkgconfig/setting-common-internal.pc
%{_includedir}/setting-cfg.h
%{_includedir}/setting-common-data-edj-define.h
%{_includedir}/setting-common-data-error.h
%{_includedir}/setting-common-data-type.h
%{_includedir}/setting-common-draw-widget.h
%{_includedir}/setting-common-general-func.h
%{_includedir}/setting-common-resource.h
%{_includedir}/setting-common-search.h

%files ref
%{_datadir}/mime/packages/mime.setting.xml
%{_datadir}/packages/org.tizen.setting.xml.ref

%files about
%manifest setting-about/org.tizen.setting-about.manifest
%{_prefix}/apps/org.tizen.setting-about/bin/setting-about
%{_datadir}/packages/org.tizen.setting-about.xml

%{_prefix}/apps/org.tizen.setting-about/res/*
#%{_prefix}/apps/org.tizen.setting-about/shared/res/*
#%attr(-,app,app) %dir %{_prefix}/apps/org.tizen.setting-about/shared
