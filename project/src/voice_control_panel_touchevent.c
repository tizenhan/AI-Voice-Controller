#include <dlog.h>
#include <efl_util.h>
#include <system_info.h>

#include "voice_control_panel_touchevent.h"
#include "voice_control_panel_main.h"
#include "voice_control_panel_action.h"
#include "voice_control_panel_command.h"
#include "voice_control_panel_view.h"

#define SWIPE_DELAY 10000000
#define MAX_PPS 1000

static int g_move_speed = 0;
static int g_screen_w = 0;
static int g_screen_h = 0;

int vc_panel_touch_set_speed(const int pps)
{
	if (MAX_PPS < pps) {
		g_move_speed = MAX_PPS;
	} else {
		g_move_speed = pps;
	}

	return 0;
}

void vc_panel_touch_back(void* data)
{
	appdata* ad = data;
	efl_util_inputgen_h back_key = ad->back_key;

	efl_util_input_generate_key(back_key, "XF86Back", 1);
	efl_util_input_generate_key(back_key, "XF86Back", 0);
}

void vc_panel_touch_swipe(const int direction, void* data)
{
	appdata* ad = data;
	efl_util_inputgen_h touch = ad->touch;
	int i;
	int ret = 0;
	int delay = 0;
	int center_x, center_y;
	int dest;

	LOGD("=== Touch swipe(%d)", direction);

	system_info_get_platform_int("http://tizen.org/feature/screen.width", &g_screen_w);
	system_info_get_platform_int("http://tizen.org/feature/screen.height", &g_screen_h);

	center_x = g_screen_w / 2;
	center_y = g_screen_h / 2;

	ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_BEGIN, center_x, center_y);
	if (0 != ret) {
		LOGE("[ERROR] event generation failed");
		return;
	}

	delay = SWIPE_DELAY / g_move_speed;

	// scroll down
	if (VC_PANEL_TOUCH_SWIPE_UP == direction) {
		dest = g_screen_h * 0.1;
		for (i = center_y; dest < i; i -= 10) {
			ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_UPDATE, center_x, i);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				return;
			}
			usleep(delay);
		}

		ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_END, center_x, i);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			return;
		}
	// scroll up
	} else if (VC_PANEL_TOUCH_SWIPE_DOWN == direction) {
		dest = g_screen_h * 0.9;
		for (i = center_y; dest > i; i += 10) {
			ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_UPDATE, center_x, i);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				return;
			}
			usleep(delay);
		}

		ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_END, center_x, i);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			return;
		}
	// swipe left
	} else if (VC_PANEL_TOUCH_SWIPE_LEFT == direction) {
		dest = g_screen_w * 0.1;
		for (i = center_x; dest < i; i -= 10) {
			ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_UPDATE, i, center_y);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				return;
			}
			usleep(delay);
		}

		ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_END, i, center_y);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			return;
		}
	// swipe right
	} else if (VC_PANEL_TOUCH_SWIPE_RIGHT == direction) {
		dest = g_screen_w * 0.9;
		for (i = center_x; dest > i; i += 10) {
			ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_UPDATE, i, center_y);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				return;
			}
			usleep(delay);
		}

		ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_END, i, center_y);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			return;
		}
	}

	LOGD("==== end swipe");
}

#if 0
void __vc_panel_action_zoom(const int direction, void* data)
{
	appdata* ad = data;
	efl_util_inputgen_h touch = ad->touch;
	int ret = 0;
	int x;

	LOGD("Zoom(%d)", direction);

	ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_BEGIN, 400, 640);
	if (0 != ret) {
		LOGE("[ERROR] event generation failed");
		efl_util_input_deinitialize_generator(touch);
		return;
	}

	if (1 == direction) {
		ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_BEGIN, 340, 640);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			efl_util_input_deinitialize_generator(touch);
			return;
		}

		for (x = 340; 210 < x; x -= 2) {
			ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_UPDATE, x, 640);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				efl_util_input_deinitialize_generator(touch);
				return;
			}
			usleep(1000);
		}

		ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_END, x, 640);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			efl_util_input_deinitialize_generator(touch);
			return;
		}
	} else if (2 == direction) {
		ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_BEGIN, 210, 640);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			efl_util_input_deinitialize_generator(touch);
			return;
		}

		for (x = 210; 340 > x; x += 2) {
			ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_UPDATE, x, 640);
			if (0 != ret) {
				LOGE("[ERROR] event generation failed");
				efl_util_input_deinitialize_generator(touch);
				return;
			}
			usleep(1000);
		}

		ret = efl_util_input_generate_touch(touch, 1, EFL_UTIL_INPUT_TOUCH_END, x, 640);
		if (0 != ret) {
			LOGE("[ERROR] event generation failed");
			efl_util_input_deinitialize_generator(touch);
			return;
		}
	}

	ret = efl_util_input_generate_touch(touch, 0, EFL_UTIL_INPUT_TOUCH_END, 400, 640);
	if (0 != ret) {
		LOGE("[ERROR] event generation failed");
		efl_util_input_deinitialize_generator(touch);
		return;
	}
}
#endif