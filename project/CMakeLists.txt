CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(voice-control-panel C)

SET(HEADERS)
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(EXEC_PREFIX "\${prefix}")
SET(INCLUDEDIR "\${prefix}/include")

SET(PACKAGE ${PROJECT_NAME})
SET(PKGNAME "org.tizen.${PACKAGE}")
SET(PKGDIR "${PREFIX}")
SET(BINDIR "${PKGDIR}/bin")
SET(RESDIR "${PKGDIR}/res")
SET(DATADIR "${PREFIX}/data")
SET(LOCALEDIR "${RESDIR}/locale")
SET(IMAGEDIR "${RESDIR}/images")

SET(SRCS
    src/voice_control_panel_action.c
    src/voice_control_panel_command.c
    src/voice_control_panel_main.c
    src/voice_control_panel_view.c
    src/voice_control_panel_vc.c
    src/voice_control_panel_touchevent.c
    src/resource.c
    src/resource/resource_led.c
    src/resource/resource_relay.c
)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
    SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED 
	appcore-efl dbus-1 elementary ecore-imf eina ecore ecore-input evas edje glib-2.0 json-glib-1.0 capi-ui-efl-util capi-system-info
	capi-media-sound-manager capi-base-common voice-control-manager capi-appfw-app-manager tts voice-control-setting aul capi-appfw-application capi-system-peripheral-io
)

FOREACH(flag ${pkgs_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
    SET(EXTRA_CXXFLAGS "${EXTRA_CXXFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Wall -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CXXFLAGS} -Wall -fPIE")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g -Wall -fPIE")
SET(CMAKE_C_FLAGS_RELEASE "-O2 -Wall -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE "-O2 -g -fPIE")

ADD_DEFINITIONS("-DPREFIX=\"${PREFIX}\"")
ADD_DEFINITIONS("-DPACKAGE=\"${PACKAGE}\"")
ADD_DEFINITIONS("-DPACKAGE_NAME=\"${PKGNAME}\"")
ADD_DEFINITIONS("-DRESDIR=\"${RESDIR}\"")
ADD_DEFINITIONS("-DDATADIR=\"${DATADIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BINDIR})

INSTALL(FILES ${CMAKE_SOURCE_DIR}/org.tizen.voice-control-panel.xml DESTINATION ${MANIFESTDIR})

ADD_SUBDIRECTORY(res)
