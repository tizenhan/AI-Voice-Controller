#include <stdio.h>
#include <Ecore.h>

#include "voice_control_panel_command.h"
#include "voice_control_panel_main.h"
#include "voice_control_panel_vc.h"
#include "resource.h"

extern struct _current_information current_information;

char* g_command_1st[NUM_COMMAND_1ST] = {
	"IDS_BALL_ON", "IDS_BALL_OFF",
	"IDS_CONVEYER_ON", "IDS_CONVEYER_OFF",
};
char *g_hint_1st[NUM_COMMAND_1ST] = {
	"IDS_BALL_ON", "IDS_BALL_OFF",
	"IDS_CONVEYER_ON", "IDS_CONVEYER_OFF",
 };

char* g_command_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND] = {
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

char* g_hint_2nd[NUM_COMMAND_1ST][NUM_COMMAND_2ND] = {
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
	{NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
};

Eina_Bool _stop_ball_machine(void *data)
{
	LOGD("공 모터를 정지합니다");
	if (resource_write_relay(GPIO_BALL, 0)) {
		LOGE("Cannot set FRONT");
	} else
		LOGD("Stop the ball machine");

	current_information.ball_machine = 0;

	return ECORE_CALLBACK_CANCEL;
}

int vcp_cmd_set(int type, int cmd)
{
	switch (type) {
		case BALL_MACHINE:
			LOGD("공 루틴 시작");
			if (current_information.ball_machine == 1) {
				if (cmd) vc_panel_vc_play_text("공을 배급 중입니다.");
				break;
			}
			current_information.ball_machine = 1;

			if (resource_write_relay(GPIO_BALL, 1)) {
				LOGE("Cannot set FRONT as [%d]", cmd);
			}

			if (current_information.timer[0]) {
				ecore_timer_del(current_information.timer[0]);
				current_information.timer[0] = NULL;
			}

			current_information.timer[0] = ecore_timer_add(STOP_DURATION, _stop_ball_machine, NULL);
			if (!current_information.timer[0])
				LOGE("Cannot add a timer");
			break;
		case CONVEYER_A:
			if (current_information.conveyer_a == cmd) {
				if (cmd) vc_panel_vc_play_text("컨베이어는 이미 동작 중입니다.");
				else vc_panel_vc_play_text("컨베이어는 이미 멈춰있습니다.");
				break;
			}
			current_information.conveyer_a = cmd;

			if (resource_write_relay(GPIO_CONVEYER_A, cmd)) {
				LOGE("Cannot set SWITCH A as [%d]", cmd);
			}

			if (cmd == 1) {
				vc_panel_vc_play_text("컨베이어를 시작하겠습니다.");
			} else {
				vc_panel_vc_play_text("컨베이어를 중지하겠습니다.");
			}
			break;
		default:
			LOGE("No case here");
			break;
	}
	return 0;
}

Eina_Bool _ball_timer_cb(void *data)
{
	int cmd = (int) data;
	LOGD("공 알람 수행[%d]", cmd);
	vcp_cmd_set(BALL_MACHINE, cmd);
	current_information.timer[0] = NULL;
	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool _ball2_timer_cb(void *data)
{
	int cmd = (int) data;
	LOGD("공 알람 수행[%d]", cmd);
	vcp_cmd_set(BALL_MACHINE_2, cmd);
	current_information.timer[1] = NULL;
	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool _conveyer_a_timer_cb(void *data)
{
	int cmd = (int) data;
	LOGD("컨베이버 알람 수행[%d]", cmd);
	vcp_cmd_set(CONVEYER_A, cmd);
	current_information.timer[2] = NULL;
	return ECORE_CALLBACK_CANCEL;
}

Eina_Bool _conveyer_b_timer_cb(void *data)
{
	int cmd = (int) data;
	LOGD("컨베이버 알람 수행[%d]", cmd);
	vcp_cmd_set(CONVEYER_B, cmd);
	current_information.timer[3] = NULL;
	return ECORE_CALLBACK_CANCEL;
}

int vcp_cmd_reserve(int type, int cmd, double seconds)
{
	int is_reserved = 0;
	char tts_str[1024] = {0, };

	switch (type) {
		case BALL_MACHINE:
			if (current_information.timer[0]) {
				ecore_timer_del(current_information.timer[0]);
				is_reserved = 1;
			}
			current_information.timer[0] = ecore_timer_add(seconds, _ball_timer_cb, (void *) cmd);
			if (!current_information.timer[0]) LOGE("Timer error");

			if (cmd == 1) {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			} else {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람이 설정되었습니다.");
			}
			break;
		case BALL_MACHINE_2:
			if (current_information.timer[1]) {
				ecore_timer_del(current_information.timer[1]);
				is_reserved = 1;
			}
			current_information.timer[1] = ecore_timer_add(seconds, _ball2_timer_cb, (void *) cmd);
			if (!current_information.timer[1]) LOGE("Timer error");

			if (cmd == 1) {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			} else {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			}
			break;
		case CONVEYER_A:
			if (current_information.timer[2]) {
				ecore_timer_del(current_information.timer[2]);
				is_reserved = 1;
			}
			current_information.timer[2] = ecore_timer_add(seconds, _conveyer_a_timer_cb, (void *) cmd);
			if (!current_information.timer[2]) LOGE("Timer error");

			if (cmd == 1) {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			} else {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			}
			break;
		case CONVEYER_B:
			if (current_information.timer[3]) {
				ecore_timer_del(current_information.timer[3]);
				is_reserved = 1;
			}
			current_information.timer[3] = ecore_timer_add(seconds, _conveyer_b_timer_cb, (void *) cmd);
			if (!current_information.timer[3]) LOGE("Timer error");

			if (cmd == 1) {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			} else {
				if (is_reserved) vc_panel_vc_play_text("기존 알람을 해제하고 신규 알람을 설정하였습니다.");
				else vc_panel_vc_play_text("알람을 설정하였습니다.");
			}
			break;
		default:
			LOGE("No case here");
			break;
	}
	return 0;
}

int vcp_cmd_cancel(int type)
{
	switch (type) {
		case BALL_MACHINE:
			if (current_information.timer[0]) {
				ecore_timer_del(current_information.timer[0]);
				current_information.timer[0] = NULL;
			} else {
				vc_panel_vc_play_text("설정된 알람이 없습니다.");
				break;
			}

			vc_panel_vc_play_text("알람이 해제되었습니다.");
			break;
		case BALL_MACHINE_2:
			if (current_information.timer[1]) {
				ecore_timer_del(current_information.timer[1]);
				current_information.timer[1] = NULL;
			} else {
				vc_panel_vc_play_text("설정된 알람이 없습니다.");
				break;
			}

			vc_panel_vc_play_text("알람이 해제되었습니다.");
			break;
		case CONVEYER_A:
			if (current_information.timer[2]) {
				ecore_timer_del(current_information.timer[2]);
				current_information.timer[2] = NULL;
			} else {
				vc_panel_vc_play_text("설정된 알람이 없습니다.");
				break;
			}

			vc_panel_vc_play_text("알람이 해제되었습니다.");
			break;
		case CONVEYER_B:
			if (current_information.timer[3]) {
				ecore_timer_del(current_information.timer[3]);
				current_information.timer[3] = NULL;
			} else {
				vc_panel_vc_play_text("설정된 알람이 없습니다.");
				break;
			}

			vc_panel_vc_play_text("알람이 해제되었습니다.");
			break;
		default:
			LOGE("No case here");
			break;
	}
	return 0;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
