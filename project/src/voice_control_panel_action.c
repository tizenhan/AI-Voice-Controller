#include <dbus/dbus.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>

#include <app.h>
#include <aul.h>
#include <dlog.h>
#include <vconf.h>

#include "voice_control_panel_main.h"
#include "voice_control_panel_action.h"
#include "voice_control_panel_command.h"
#include "voice_control_panel_view.h"
#include "voice_control_panel_touchevent.h"
#include "voice_control_panel_vc.h"

extern struct _current_information current_information;

#if 0 /* NOT USED - if needed, reactivate*/
static void __vc_panel_action_send_key_event(void *data, Ecore_Thread *thread)
{
	LOGD("==== Send Key Event ====");

	int keynum = (int)data;
	LOGD("Key - %d", keynum);

	int fd;
	fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
	if (0 > fd) {
		LOGE("[ERROR] Fail to open dev");
		return;
	}

	int ret;
	struct uinput_user_dev uidev;
	memset(&uidev, 0, sizeof(uidev));
	snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "vc-keyevent");
	uidev.id.bustype = BUS_USB;
	uidev.id.vendor = 1;
	uidev.id.product = 1;
	uidev.id.version = 1;

	ret = write(fd, &uidev, sizeof(uidev));
	if (sizeof(uidev) != ret) {
		LOGE("[ERROR] Fail to write info");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
	if (0 != ret) {
		LOGE("[ERROR] Fail to ioctl");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
	if (0 != ret) {
		LOGE("[ERROR] Fail to ioctl");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_SET_KEYBIT, keynum);
	if (0 != ret) {
		LOGE("[ERROR] Fail to register key");
		close(fd);
		return;
	}

	ret = ioctl(fd, UI_DEV_CREATE);
	if (0 != ret) {
		LOGE("[ERROR] Fail to create");
		close(fd);
		return;
	}

	usleep(1000000);

	struct input_event ev;
	memset(&ev, 0, sizeof(ev));
	ev.type = EV_KEY;
	ev.code = keynum;
	ev.value = 1;

	ret = write(fd, &ev, sizeof(ev));
	if (0 > ret) {
		LOGE("[ERROR] Fail to send key event");
		close(fd);
		return;
	}

	memset(&ev, 0, sizeof(ev));
	ev.type = EV_SYN;
	ev.code = 0;
	ev.value = 0;

	ret = write(fd, &ev, sizeof(ev));
	if (0 > ret) {
		LOGE("[ERROR] Fail to send sync event");
		close(fd);
		return;
	}

	usleep(1000000);

	ret = ioctl(fd, UI_DEV_DESTROY);
	if (0 != ret) {
		LOGE("[ERROR] Fail to destroy");
		close(fd);
		return;
	}

	LOGD("====");
	LOGD(" ");

	close(fd);
	return;
}

static void __vc_panel_action_launch_app(const char* app_id)
{
	LOGD("==== Launch app(%s) ====", app_id);
	app_control_h app_control;
	app_control_create(&app_control);
	app_control_set_app_id(app_control, app_id);
	app_control_send_launch_request(app_control, NULL, NULL);
	app_control_destroy(app_control);
	LOGD("====");
	LOGD("");
}
#endif

void __launch_poweroff_popup()
{
	LOGD("--");
	DBusError err;
	dbus_error_init(&err);

	DBusConnection *conn = NULL;
	conn = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err)) {
		dbus_error_free(&err);
	}

	DBusMessage* msg;
	msg = dbus_message_new_method_call(
		"org.tizen.system.popup",
		"/Org/Tizen/System/Popup/Powerkey",
		"org.tizen.system.popup.Powerkey",
		"PopupLaunch");

	const char *tmp1 = "_SYSPOPUP_CONTENT_";
	const char *tmp2 = "powerkey";
	dbus_message_append_args(msg,
		DBUS_TYPE_STRING, &tmp1,
		DBUS_TYPE_STRING, &tmp2,
		DBUS_TYPE_INVALID);

	dbus_connection_send(conn, msg, NULL);
	dbus_message_unref(msg);

	LOGD("++");

	return;
}

bool vc_panel_action(const char* result, void *data)
{
	if (NULL == result)
		return false;

	LOGD("==== Action - %s ====", result);

	appdata *ad = (appdata *)data;

	if (1 == ad->current_depth) {
		do {
			//if (0 == strcmp(result, "공")) {
			if ((0 == strstr(result, "공") - result)
				|| (0 == strstr(result, "콩") - result)
				|| (0 == strstr(result, "홍") - result)
				|| (0 == strstr(result, "곰") - result)
				|| (0 == strstr(result, "꽁") - result)
				|| (0 == strstr(result, "송") - result)) {
				LOGD("Enter : 공");
				vc_panel_vc_play_text("스트라이크");
				vcp_cmd_set(BALL_MACHINE, 1);
				return true;
			} else if ((0 == strstr(result, "시작") - result) 
					|| (0 == strstr(result, "싹") - result)) {
				LOGD("Enter : 시작");

				if (strstr(result, "지마")) {
					LOGD("Enter : 시작 -> 하지마");
					vc_panel_vc_play_text("시작하지 않겠습니다.");
					return true;
				} else { // 일반
					LOGD("Enter : 시작 -> 해");
					//vc_panel_vc_play_text("시작합니다.");
					vcp_cmd_set(CONVEYER_A, 1);
					return true;
				}
			} else if ((0 == strstr(result, "정지") - result)
					|| (0 == strstr(result, "정시") - result)
					|| (0 == strstr(result, "정자") - result)
					|| (0 == strstr(result, "종자") - result)
					|| (0 == strstr(result, "종q	지") - result)
					|| (0 == strstr(result, "중지") - result)
					|| (0 == strstr(result, "경자") - result)) {
				LOGD("Enter : 정지");

				if (strstr(result, "지마")) {
					LOGD("Enter : 시작 -> 하지마");
					vc_panel_vc_play_text("정지하지 않겠습니다.");
					return true;
				} else { // 일반
					LOGD("Enter : 시작 -> 해");
					//vc_panel_vc_play_text("정지합니다.");
					vcp_cmd_set(CONVEYER_A, 0);
					return true;
				}
			} else {
				vc_panel_vc_play_text("다시 말씀해주세요.");
			}
		} while (0);

		int i;
		for (i = 0; i < NUM_COMMAND_1ST; i++) {
			if (NULL == g_command_1st[i])
				continue;

			if (!strcasecmp(result, _(g_command_1st[i]))) {
				ad->current_path[0] = i;
				ad->current_depth = 2;
			}
		}
	}

	LOGD("====");
	LOGD(" ");

	return true;
}