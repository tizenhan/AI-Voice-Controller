#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <voice_control_setting.h>
#include <Ecore.h>
#include <sound_manager.h>

#include "voice_control_panel_main.h"
#include "voice_control_panel_view.h"
#include "voice_control_panel_vc.h"
#include "resource.h"

struct _current_information current_information = {
	.ball_machine = 0,
	.ball_machine_2 = 0,
	.conveyer_a = 0,
	.conveyer_b = 0
};

static void __vc_enabled_changed_cb(bool enabled, void* user_data)
{
	LOGD("Voice control enabled changed to (%d)", enabled);

	if (false == enabled) {
		ui_app_exit();
	}
}

#if 1
Eina_Bool _test_gpio(void *data)
{
	static int cmd = 0;

	LOGD("Test CMD : %d", cmd);

	if (resource_write_led(GPIO_BALL, cmd)) {
		LOGE("Cannot open FRONT");
	}

	if (resource_write_led(GPIO_BALL_2, cmd)) {
		LOGE("Cannot open BACK");
	}

	if (resource_write_led(GPIO_CONVEYER_A, cmd)) {
		LOGE("Cannot open FRONT");
	}

	if (resource_write_led(GPIO_CONVEYER_B, cmd)) {
		LOGE("Cannot open BACK");
	}

	if (cmd) cmd = 0;
	else cmd = 1;

	return ECORE_CALLBACK_RENEW;
}
#endif

static bool app_create(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;

#if 1
	if (resource_write_led(GPIO_BALL, 0)) {
		LOGE("Cannot open FRONT");
	}

	if (resource_write_led(GPIO_BALL_2, 0)) {
		LOGE("Cannot open BACK");
	}

	if (resource_write_led(GPIO_CONVEYER_A, 0)) {
		LOGE("Cannot open A");
	}

	if (resource_write_led(GPIO_CONVEYER_B, 0)) {
		LOGE("Cannot open B");
	}
#endif

	/* Check voice control enabled */
	if (0 != vc_setting_initialize()) {
		LOGE("Fail to init");
	}

	bool enabled;
	if (0 != vc_setting_get_enabled(&enabled)) {
		LOGE("Fail to get enabled");
		vc_setting_deinitialize();
		return false;
	}

	LOGD("Enabled [%d]", enabled);
#if 0
	if (false == enabled) {
		LOGE("Voice control is disabled");
		vc_setting_deinitialize();
		return false;
	}
#endif

	if (0 != vc_setting_set_enabled_changed_cb(__vc_enabled_changed_cb, NULL)) {
		LOGE("Fail to set enabled cb");
	}

	char* lang = NULL;
	if (0 != vc_setting_get_language(&lang)) {
		LOGE("Fail to get language");
		return false;
	}

	char loc[64] = {'\0',};
	snprintf(loc, 64, "%s.UTF-8", lang);

	setlocale(LC_ALL, loc);

	bindtextdomain("org.tizen.voice-control-panel", "/usr/apps/org.tizen.voice-control-panel/res/locale");
	textdomain("org.tizen.voice-control-panel");

	if (NULL != lang) {
	    free(lang);
	    lang = NULL;
	}

	int max_vol = 0;
	sound_manager_get_max_volume(SOUND_TYPE_VOICE, &max_vol);
	sound_manager_set_volume(SOUND_TYPE_VOICE, max_vol);

	/* Create View */
	if (0 != vc_panel_view_create(ad)) {
		LOGE("[ERROR] Fail to create view");
		return -1;
	}

	/* Initialize Voice Control */
	if (0 != vc_panel_vc_init(ad)) {
		LOGE("[ERROR] Fail to vc init");
		return -1;
	}

	return true;
}

static void app_control(app_control_h app_control, void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	LOGD("state - %d", ad->app_state);

	if (0 != ad->app_state) {
		ui_app_exit();
		return;
	}

	if (ad->win) {
		elm_win_activate(ad->win);
	}
	ad->app_state = PANEL_STATE_INIT;

#if 0 // Test
	Ecore_Timer *timer = ecore_timer_add(5.0f, _test_gpio, NULL);
	if (!timer) LOGE("Cannot add a timer");
#endif
}

static void app_pause(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	ad->app_state = PANEL_STATE_PAUSE;

	vc_panel_vc_deactivate(data, 0.5);
}

static void app_resume(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	if (ad->app_state == PANEL_STATE_PAUSE) {
		if (ad->win) {
			elm_win_activate(ad->win);
		}
	}

	ad->app_state = PANEL_STATE_SERVICE;
}

static void app_terminate(void *data)
{
	LOGD("");

	appdata *ad = (appdata *)data;
	ad->app_state = PANEL_STATE_TERMINATE;

	resource_close_all();

	vc_panel_vc_cancel(ad);

	vc_panel_view_destroy(ad);

	vc_setting_deinitialize();

	if (0 != vc_panel_vc_deinit(data)) {
		LOGE("[ERROR] Fail to vc deinit");
	}
}

static void ui_app_lang_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_orient_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_region_changed(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_low_battery(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

static void ui_app_low_memory(app_event_info_h event_info, void *user_data)
{
	LOGD("");
}

int main(int argc, char *argv[])
{
	appdata ad = {0,};
	int ret = 0;

	ui_app_lifecycle_callback_s event_callback = {0,};
	app_event_handler_h handlers[5] = {NULL, };

	event_callback.create = app_create;
	event_callback.terminate = app_terminate;
	event_callback.pause = app_pause;
	event_callback.resume = app_resume;
	event_callback.app_control = app_control;

	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_BATTERY], APP_EVENT_LOW_BATTERY, ui_app_low_battery, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LOW_MEMORY], APP_EVENT_LOW_MEMORY, ui_app_low_memory, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_DEVICE_ORIENTATION_CHANGED], APP_EVENT_DEVICE_ORIENTATION_CHANGED, ui_app_orient_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_LANGUAGE_CHANGED], APP_EVENT_LANGUAGE_CHANGED, ui_app_lang_changed, &ad);
	ui_app_add_event_handler(&handlers[APP_EVENT_REGION_FORMAT_CHANGED], APP_EVENT_REGION_FORMAT_CHANGED, ui_app_region_changed, &ad);

	ret = ui_app_main(argc, argv, &event_callback, &ad);
	if (ret != APP_ERROR_NONE) {
		LOGW("ui_app_main failed, Err=%d\n", ret);
	}

	return ret;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
