CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(EDJ_PATH "/usr/apps/org.tizen.voice-control-panel/res/edje/")

ADD_CUSTOM_TARGET(voice-control-panel.edj
    COMMAND edje_cc ${CMAKE_CURRENT_SOURCE_DIR}/voice-control-panel.edc voice-control-panel.edj
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/voice-control-panel.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} voice-control-panel.edj)
INSTALL(FILES voice-control-panel.edj DESTINATION ${EDJ_PATH})