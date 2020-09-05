#ifndef __VOICE_CONTROL_PANEL_MAIN_H
#define __VOICE_CONTROL_PANEL_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <app.h>
#include <glib.h>
#include <Elementary.h>
#include <dlog.h>
#include <efl_util.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG	"vcp"
//#define _(S)	gettext(S)

#define GPIO_BALL (27) // 20:Impossible
#define GPIO_BALL_2 (17)
#define GPIO_CONVEYER_A (21)
#define GPIO_CONVEYER_B (22)

#define STOP_DURATION (0.5f)

enum {
	PANEL_STATE_INIT = 1,
	PANEL_STATE_PAUSE,
	PANEL_STATE_SERVICE,
	PANEL_STATE_TERMINATE,
};

struct _current_information {
	int ball_machine;
	int ball_machine_2;
	int conveyer_a;
	int conveyer_b;
	int reserve[4];
	Ecore_Timer *timer[4];
};

typedef struct _appdata {
	/* GUI */
	float scale_w;
	float scale_h;

	Evas_Object *win;
	Evas_Object *layout_main;
	Evas_Object *image_mic;
	Evas_Object *image_arrow;
	Evas_Object *content_box;
	Evas_Object *image_setting;
	Evas_Object *image_close;

	Evas_Object *help_win;
	Evas_Object *help_genlist;
	Evas_Object *help_layout;

	Elm_Theme *theme;

	int app_state;

	int act_state;

	int current_depth;
	int current_path[2];

	efl_util_inputgen_h touch;
	efl_util_inputgen_h back_key;

	GList *cmd_list;
} appdata;

#define retv_if(expr, val) do { \
	if (expr) { \
		LOGE("(%s) -> %s() return", #expr, __FUNCTION__); \
		return (val); \
	} \
} while (0)

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_MAIN_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
