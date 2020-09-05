## 팀명 및 팀원
* 팀명: SINO TECH
* 팀원 1: 한민수(기획/설계/회로/소프트웨어개발/디버깅/발표)
* 팀원 2: 한은총(기획/하드웨어조립/디버깅)

## 프로젝트 제목
* 스마트 공장용 음성제어 시스템 & 야구 연습장용 음성제어 시스템

## 프로젝트 배경 혹은 목적
* 언택시대에 꼭 필요한 음성 인식 제어 시스템을 타이젠 기반으로 구현하고자 합니다.
* 현재 우리는 코로나 바이러스 등으로 사물과 사람의 직접적인 접촉이 꺼려지는 뉴노멀 시대에 살고 있습니다.
* 일상 생활에서 마주할 수 있는 여러 접촉 상황을 찾아서 KCCC 방역 거리두기 지침에 부합하는 시스템을 만들고자 합니다.
* 시나리오 1 : 공장에서 다수의 근로자가 사용하는 스위치를 누르지 않고 음성만으로 동작을 제어할 수 있습니다.
* 시나리오 2 : 야구 연습장 혹은 골프 연습장에서 음성제어 만으로 야구공 혹은 골프공을 배급 받을 수 있습니다.

## 타이젠 오픈소스에 컨트리뷰션한 내역
* 과거에 컨트리뷰션한 코드를 사용하였습니다.
* src/resource/resource_relay.c
* inc/resource/resource_relay.h
* inc/resource/resource_relay_internal.h

## 파일 리스트
* 팀원이 소스 파일을 직접 만든 경우, 해당 파일을 적어주세요.
* 오픈소스(타이젠 등)로부터 가져왔지만, 팀원이 내용을 수정하거나 덧붙인 경우, 해당 파일을 적어주세요.
* 오픈소스(타이젠 등)를 그대로 가져다가 사용한 파일은 적지 말아주세요.
* 헤더와 소스만 적어주세요.
* 디자인 등의 리소스는 적지 말아주세요.
* CMakeList.txt
* packaging/org.tizen.voice-control-panel.spec
* res/po/en_US.po
* res/po/ko_KR.po
* src/voice_control_panel_action.c
* src/voice_control_panel_command.c
* src/voice_control_panel_command_h
* src/voice_control_panel_main.c
* src/voice_control_panel_main.h
* src/voice_control_panel_view.c
* src/resource/resource_relay.c
* inc/resource/resource_relay.h
* inc/resource/resource_relay_internal.h

## 코드 기여자
* CMakeList.txt : capi-media-sound-manager capi-system-peripheral-io 한민수
* packaging/org.tizen.voice-control-panel.spec capi-media-sound-manager capi-system-peripheral-io 한민수
* res/po/en_US.po 음성제어문 한민수
* res/po/ko_KR.po 음성제어문 한민수
* src/voice_control_panel_action.c vc_panel_action 한민수
* src/voice_control_panel_command.c _stop_ball_machine, vcp_cmd_set, _ball_timer_cb, _conveyer_a_timer_cb, vcp_cmd_reserve, vcp_cmd_cancel 한민수
* src/voice_control_panel_command_h enum 한민수
* src/voice_control_panel_main.c current_information, _test_gpio, app_create 한민수
* src/voice_control_panel_main.h _current_information 한민수
* src/voice_control_panel_view.c 한민수
* src/resource/resource_relay.c 한민수
* inc/resource/resource_relay.h 한민수
* inc/resource/resource_relay_internal.h 한민수

## 보드
* RPI4: 음성인식 및 센서제어, https://github.com/tizenhan/AI-Voice-Controller

## 구현사항(가산점)
* Peripheral GPIO를 사용하여 Relay를 제어하였습니다.
