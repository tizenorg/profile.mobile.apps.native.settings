%bcond_with x
%bcond_with wayland


Name:       org.tizen.setting
Summary:    Setting application
Version:    0.2.0
Release:    99
Group:      misc
License:    Apache-1.0
Source0:    %{name}-%{version}.tar.gz
Source1:    packaging/firewall.service

%if "%{?tizen_profile_name}" == "tv"
ExcludeArch: %{arm} %ix86 x86_64
%endif
%if "%{?tizen_profile_name}" == "wearable"
ExcludeArch: %{arm} %ix86 x86_64
%endif

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
BuildRequires:  pkgconfig(ail)
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
BuildRequires:  pkgconfig(accounts-svc)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(mm-player)
BuildRequires:  pkgconfig(capi-network-connection)
BuildRequires:  pkgconfig(security-server)
BuildRequires:  pkgconfig(feedback)
BuildRequires:  pkgconfig(appsvc)
BuildRequires:  pkgconfig(libxml-2.0)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(cairo)
BuildRequires:  pkgconfig(capi-system-runtime-info)
BuildRequires:  pkgconfig(capi-media-player)
BuildRequires:  pkgconfig(capi-media-sound-manager)
BuildRequires:  pkgconfig(capi-media-recorder)
BuildRequires:  pkgconfig(capi-content-media-content)
BuildRequires:  pkgconfig(capi-system-device)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-appfw-preference)
BuildRequires:  pkgconfig(capi-appfw-package-manager)
BuildRequires:  pkgconfig(capi-system-sensor)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(capi-system-system-settings)
BuildRequires:  pkgconfig(capi-network-wifi)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(capi-media-metadata-extractor)
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
BuildRequires:  pkgconfig(efl-assist)
BuildRequires:  pkgconfig(sqlite3)
BuildRequires:  pkgconfig(capi-media-camera)
BuildRequires:  pkgconfig(notification)
BuildRequires:  pkgconfig(syspopup)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:	pkgconfig(app2sd)
BuildRequires:	pkgconfig(capi-media-image-util)
BuildRequires:  pkgconfig(capi-telephony)
BuildRequires:  pkgconfig(eventsystem)
BuildRequires:  pkgconfig(bundle)
BuildRequires:  libcap-devel
BuildRequires:  cmake
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  efl-assist-devel
BuildRequires:  capi-security-privilege-manager-devel
BuildRequires:  hash-signer
BuildRequires: pkgconfig(libtzplatform-config)
#BuildRequires:  system-resource-devel-meta
#BuildRequires: model-build-features

#Requires: libeas-common
Requires: capi-security-privilege-manager
Requires(post): attr
#Requires(post): /sbin/ldconfig
#Requires(postun): /sbin/ldconfig

# ls: cannot access /opt/usr/share/settings: No such file or directory

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

%prep
%setup -q

%build

%define PREFIX    "/usr/apps/org.tizen.setting"
%define RESDIR    "/usr/apps/org.tizen.setting/res"
%define DATADIR    "/usr/apps/org.tizen.setting/data"
%define PREF_DATADIR "/opt/usr/apps/org.tizen.setting/data"
%define IMAGEDIR    "/usr/apps/org.tizen.setting/res/images"
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
	cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DARCH=arm \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE
%else
-Dwith_x=TRUE
%endif
%else
	cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DARCH=x86 \
%if %{with wayland} && !%{with x}
-Dwith_wayland=TRUE
%else
-Dwith_x=TRUE
%endif
%endif
%if 0%{?tizen_build_binary_release_type_eng}
		-DBINARY_RELEASE_TYPE_ENG=YES \
%endif

# build all
make

%install
rm -rf %{buildroot}
%make_install

%define tizen_sign 1
%define tizen_sign_base /usr/apps/org.tizen.setting
%define tizen_sign_level platform
%define tizen_author_sign 1
%define tizen_dist_sign 1

mkdir -p %{buildroot}%{PREF_DATADIR}
mkdir -p %{buildroot}/opt/usr/apps/org.tizen.mode-syspopup/data
mkdir -p %{buildroot}%{_libdir}/systemd/system/multi-user.target.wants
mkdir -p %{buildroot}%{_libdir}/systemd/system/default.target.wants

%clean

%post
#create directory /opt/usr/apps/org.tizen.setting.data
if [ ! -d /opt/usr/apps/org.tizen.setting ]
then
    mkdir -p /opt/usr/apps/org.tizen.setting
    mkdir -p /opt/usr/apps/org.tizen.setting/data
    mkdir -p %{PREF_DATADIR}
fi
chmod a+w /opt/usr/apps/org.tizen.setting/data


update-mime-database /usr/share/mime

# Set vconf values with -g/-u options
GOPTION="-g 6514"

#resetSound
%ifarch %{arm}
	DEFAULT_CALL_TONE="/opt/usr/share/settings/Ringtones/ringtone_sdk.mp3"
%else
	DEFAULT_CALL_TONE="/opt/usr/share/settings/Ringtones/ringtone_sdk.mp3"
%endif

%ifarch %{arm}
	DEFAULT_NOTI_TONE="/opt/usr/share/settings/Alerts/General notification_sdk.wav"
%else
	DEFAULT_NOTI_TONE="/opt/usr/share/settings/Alerts/General notification_sdk.wav"
%endif

#resetSound
%ifarch %{arm}
	DEFAULT_HOME="/opt/usr/share/settings/Wallpapers/Home_default.jpg"
	DEFAULT_LOCK="/opt/usr/share/settings/Wallpapers/Default.jpg"
%else
	# for emulator
	DEFAULT_HOME="/opt/usr/share/settings/Wallpapers/Home_default.jpg"
	DEFAULT_LOCK="/opt/usr/share/settings/Wallpapers/Default.jpg"
%endif

	rm -f /opt/etc/localtime
	ln -s /usr/share/zoneinfo/Asia/Seoul /opt/etc/localtime
	rm -f /etc/localtime
	ln -s /opt/etc/localtime /etc/localtime

#resetSecurity
	rm -rf /opt/usr/data/setting/set_info

# for support shared menu icons
ln -s /usr/apps/org.tizen.setting/res/icons /usr/apps/org.tizen.setting/shared/res/icons

/usr/apps/org.tizen.setting/bin/setting_conf_util timezone_init
#------------------------------------------
sync

#if [ -d /opt/usr/share/settings ]
#then
#	rm -rf /opt/usr/share/settings
#	mkdir -p /opt/usr/share/settings
#fi

#ln -s /opt/usr/share/settings /opt/share/settings


mkdir -p /usr/ug/bin/
# help directory
mkdir -p /usr/apps/org.tizen.setting/shared
mkdir -p /usr/apps/org.tizen.setting/shared/res
mkdir -p /usr/apps/org.tizen.mode-syspopup/res/edje
mkdir -p /usr/apps/org.tizen.mode-syspopup/bin/

#%post ref
#Requires(post): org.tizen.setting


%post ref
rm -rf /usr/share/packages/org.tizen.setting.xml
mv /usr/share/packages/org.tizen.setting.xml.ref /usr/share/packages/org.tizen.setting.xml

%posttrans

#%files -n settings
%files -n org.tizen.setting
%manifest org.tizen.setting.manifest


/usr/apps/org.tizen.setting/author-signature.xml
/usr/apps/org.tizen.setting/signature1.xml

# Smack default --------------------------------------------------
/etc/smack/accesses.d/org.tizen.setting.efl

# Firewall -------------------------------------------------------
/etc/opt/upgrade/210.org.tizen.setting.patch.sh

%defattr(-,root,root,-)
#/usr/apps/org.tizen.mode-syspopup/bin/mode-syspopup
#/usr/apps/org.tizen.mode-syspopup/res/images/*
#/usr/apps/org.tizen.mode-syspopup/res/edje/mode-syspopup.edj
#/usr/apps/org.tizen.mode-syspopup/res/edje/popup-custom.edj
#/usr/apps/org.tizen.mode-syspopup/res/edje/ultra-popup.edj
/usr/apps/org.tizen.mode-syspopup/res/locale/*

#%attr(-,app,app) %dir /opt/usr/apps/org.tizen.mode-syspopup/data

%attr(-,app,app) %dir %{PREF_DATADIR}
/usr/apps/org.tizen.setting/bin/setting
/usr/apps/org.tizen.setting/data/*
/usr/apps/org.tizen.setting/bin/setting_conf_util
/usr/apps/org.tizen.setting/bin/setting_turnoff_light
/usr/apps/org.tizen.setting/bin/setting_volume_popup

%attr(0755,root,root) /usr/apps/org.tizen.setting/bin/setting_help_ringtone

/usr/lib/lib*.so.*

/usr/share/packages/org.tizen.setting.xml

/usr/share/icons/default/small/org.tizen.setting.png
/opt/usr/share/settings/*
/usr/apps/org.tizen.setting/res/*
/usr/share/mime/packages/*

/usr/apps/org.tizen.setting/shared/res/*
%attr(-,app,app) %dir /usr/apps/org.tizen.setting/shared

/opt/usr/apps/org.tizen.setting/data/test.db

# new
/usr/apps/org.tizen.setting/lib/ug/*

%files devel
#---------------------------------------------
# moved from org.tizen.setting
#---------------------------------------------
#/usr/apps/org.tizen.setting/lib/lib*.so
/usr/lib/lib*.so
/usr/apps/org.tizen.setting/lib/ug/*

#%{_libdir}/pkgconfig/*.pc
/usr/lib/pkgconfig/setting-common-internal.pc
/usr/include/setting-cfg.h
/usr/include/setting-common-data-edj-define.h
/usr/include/setting-common-data-error.h
/usr/include/setting-common-data-type.h
/usr/include/setting-common-draw-widget.h
/usr/include/setting-common-general-func.h
/usr/include/setting-common-resource.h
/usr/include/setting-common-search.h

%files ref
/usr/share/mime/packages/mime.setting.xml
/usr/share/packages/org.tizen.setting.xml.ref
