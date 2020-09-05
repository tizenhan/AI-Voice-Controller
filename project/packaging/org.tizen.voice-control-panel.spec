Name:       org.tizen.voice-control-panel
Summary:    Voice control panel(manager) application
Version:    0.1.1
Release:    1
Group:      Graphics & UI Framework/Voice Framework
License:    Apache-2.0
Source0:    %{name}-%{version}.tar.gz
BuildRequires:  cmake
BuildRequires:  gettext-tools
BuildRequires:  edje-tools
BuildRequires:  gettext-tools
BuildRequires:  pkgconfig(glib-2.0)
BuildRequires:  pkgconfig(json-glib-1.0)
BuildRequires:  pkgconfig(appcore-efl)
BuildRequires:  pkgconfig(elementary)
BuildRequires:  pkgconfig(ecore-imf)
BuildRequires:  pkgconfig(eina)
BuildRequires:  pkgconfig(evas)
BuildRequires:  pkgconfig(edje)
BuildRequires:  pkgconfig(ecore)
BuildRequires:  pkgconfig(ecore-file)
BuildRequires:  pkgconfig(ecore-input)
BuildRequires:  pkgconfig(dlog)
BuildRequires:  pkgconfig(aul)
BuildRequires:  pkgconfig(capi-appfw-app-manager)
BuildRequires:  pkgconfig(capi-appfw-application)
BuildRequires:  pkgconfig(capi-base-common)
BuildRequires:  pkgconfig(voice-control-manager)
BuildRequires:  pkgconfig(voice-control-setting)
BuildRequires:  pkgconfig(tapi)
BuildRequires:  pkgconfig(capi-location-manager)
BuildRequires:  pkgconfig(capi-network-bluetooth)
BuildRequires:  pkgconfig(capi-ui-efl-util)
BuildRequires:  pkgconfig(capi-system-info)
BuildRequires:  pkgconfig(vconf)
BuildRequires:  pkgconfig(tts)
BuildRequires:  pkgconfig(syspopup-caller)
BuildRequires:  pkgconfig(dbus-1)
BuildRequires:  pkgconfig(capi-system-peripheral-io)
BuildRequires:  pkgconfig(libtzplatform-config)
BuildRequires:  pkgconfig(capi-media-sound-manager)

%description
Voice control panel(manager) application

%prep
%setup -q -n %{name}-%{version}

%build
export CFLAGS="$CFLAGS -DTIZEN_ENGINEER_MODE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_ENGINEER_MODE"
export FFLAGS="$FFLAGS -DTIZEN_ENGINEER_MODE"
export CFLAGS="$CFLAGS -DTIZEN_DEBUG_ENABLE"
export CXXFLAGS="$CXXFLAGS -DTIZEN_DEBUG_ENABLE"
export FFLAGS="$FFLAGS -DTIZEN_DEBUG_ENABLE"

%define PREFIX	%{TZ_SYS_RO_APP}/%{name}
%define MANIFESTDIR %{TZ_SYS_RO_PACKAGES}

cmake . -DCMAKE_INSTALL_PREFIX=%{PREFIX} -DMANIFESTDIR=%{MANIFESTDIR}
make %{?jobs:-j%jobs}

%install
rm -rf %{buildroot}

%make_install

%files
%license LICENSE
%manifest org.tizen.voice-control-panel.manifest
%defattr(-,root,root,-)
%{PREFIX}/bin/*
%{PREFIX}/res/edje/*
%{PREFIX}/res/images/*
%{PREFIX}/res/locale/*
%{MANIFESTDIR}/org.tizen.voice-control-panel.xml
