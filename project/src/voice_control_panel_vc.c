#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <dlog.h>
#include <Elementary.h>

#include <tts.h>
#include <glib.h>
#include <json-glib/json-glib.h>
#include <efl_util.h>

#include <voice_control_command_expand.h>
#include <voice_control_common.h>
#include <voice_control_manager.h>

#include "voice_control_panel_action.h"
#include "voice_control_panel_command.h"
#include "voice_control_panel_main.h"
#include "voice_control_panel_vc.h"
#include "voice_control_panel_view.h"
#include "voice_control_panel_touchevent.h"

static vc_cmd_list_h g_cmd_list_1st;
static vc_cmd_list_h g_cmd_list_2nd[NUM_COMMAND_1ST];
static Ecore_Timer *g_deactive_timer = NULL;

static tts_h g_tts;
static char *g_tts_lang = NULL;
static bool g_dialog_continuous = false;
static bool g_dialog_process = false;
static int g_num_candidate = 0;
static int g_final_status = 0;
static char *g_final_result = NULL;

static vc_cmd_list_h g_candidate_cmd_list;

static JsonParser *g_json_parser;

static bool __current_command_cb(vc_cmd_h vc_command, void* user_data)
{
	int type;
	if (0 != vc_cmd_get_type(vc_command, &type)) {
		LOGE("[ERROR] Fail to get type");
		return false;
	}

	return true;
}

static int __vc_panel_vc_get_current_commands(void *data)
{
	appdata *ad = (appdata *)data;

	LOGD("==== Get current commands ====");
	if (0 < g_list_length(ad->cmd_list)) {
		GList *iter = NULL;
		iter = g_list_first(ad->cmd_list);

		while (NULL != iter) {
			char *cmd = iter->data;
			if (NULL != cmd) {
				free(cmd);
				cmd = NULL;
			}

			ad->cmd_list = g_list_remove_link(ad->cmd_list, iter);
			iter = g_list_first(ad->cmd_list);
		}
	}

	vc_cmd_list_h vc_cmd_list;
	if (0 != vc_mgr_get_current_commands(&vc_cmd_list)) {
		LOGE("[ERROR] Fail to get current commands");

		if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
			LOGE("[WARNING] Fail to cmd list destroy");
		}
		return -1;
	}

	if (0 != vc_cmd_list_foreach_commands(vc_cmd_list, __current_command_cb, ad)) {
		LOGE("[ERROR] Fail to get current commands");

		if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
			LOGE("[WARNING] Fail to cmd list destroy");
		}
		return -1;
	}

	if (0 != vc_cmd_list_destroy(vc_cmd_list, true)) {
		LOGE("[WARNING] Fail to cmd list destroy");
	}

	return 0;
}

static void __vc_panel_vc_destroy_command_list()
{
	LOGD("==== Destroy Command List ====");

	if (0 != vc_cmd_list_destroy(g_cmd_list_1st, true)) {
		LOGE("[WARNING] Fail to destroy list");
	}

	int i;
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		if (0 != vc_cmd_list_destroy(g_cmd_list_2nd[i], true)) {
			LOGE("[WARNING] Fail to destroy list");
		}
	}

	LOGD("====");
	LOGD(" ");
}

static int __vc_panel_vc_create_command_list()
{
	LOGD("==== Create Command List ====");

	/* 1st depth */
	if (0 != vc_cmd_list_create(&g_cmd_list_1st)) {
		LOGE("[ERROR] Fail to cmd list create");
		return -1;
	}

	vc_cmd_h cmd;
	int i;
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		if (0 != vc_cmd_create(&cmd)) {
			LOGE("[ERROR] Fail to cmd create");
			return -1;
		}
		if (0 != vc_cmd_set_command(cmd, _(g_command_1st[i]))) {
			LOGE("[ERROR] Fail to set command");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_set_type(cmd, VC_COMMAND_TYPE_SYSTEM)) {
			LOGE("[ERROR] Fail to set type");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_set_format(cmd, VC_CMD_FORMAT_FIXED)) {
			LOGE("[ERROR] Fail to set format");
			vc_cmd_destroy(cmd);
			return -1;
		}
		if (0 != vc_cmd_list_add(g_cmd_list_1st, cmd)) {
			LOGE("[ERROR] Fail to list add");
			vc_cmd_destroy(cmd);
			return -1;
		}
	}

	/* 2nd depth */
	for (i = 0; i < NUM_COMMAND_1ST; i++) {
		if (0 != vc_cmd_list_create(&g_cmd_list_2nd[i])) {
			LOGE("[ERROR] Fail to list create");
			return -1;
		}

		int j;
		for (j = 0; j < NUM_COMMAND_2ND; j++) {
			if (NULL != g_command_2nd[i][j]) {
				if (0 != vc_cmd_create(&cmd)) {
					LOGE("[ERROR] Fail to cmd create");
					return -1;
				}
				if (0 != vc_cmd_set_command(cmd, _(g_command_2nd[i][j]))) {
					LOGE("[ERROR] Fail to set command");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_set_type(cmd, VC_COMMAND_TYPE_SYSTEM)) {
					LOGE("[ERROR] Fail to set type");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_set_format(cmd, VC_CMD_FORMAT_FIXED)) {
					LOGE("[ERROR] Fail to set format");
					vc_cmd_destroy(cmd);
					return -1;
				}
				if (0 != vc_cmd_list_add(g_cmd_list_2nd[i], cmd)) {
					LOGE("[ERROR] Fail to list add");
					vc_cmd_destroy(cmd);
					return -1;
				}
			}
		}
	}

	return 0;
}

#if 0
static int __vc_panel_find_dispText(const gchar *json, char **displayText)
{
	LOGD("==== Find the display text(%s)", json);

	JsonNode *root = NULL;
	JsonObject *object = NULL;
	GError *error = NULL;
	const gchar *dispText = NULL;

	json_parser_load_from_data(g_json_parser, json, -1, &error);

	if (error) {
		LOGE("[ERROR] Fail to parse the json data");
		g_error_free(error);
		return -1;
	}

	LOGD("Success to parsing");
	root = json_parser_get_root(g_json_parser);
	if (NULL == root) {
		LOGE("[ERROR] Fail to get root node");
		return -1;
	}
	object = json_node_get_object(root);
	if (NULL == object) {
		LOGE("[ERROR] Fail to get json object");
		return -1;
	}

	root = json_object_get_member(object, "Server Response");
	if (NULL == root) {
		LOGE("[ERROR] Fail to get member node");
		return -1;
	}
	object = json_node_get_object(root);
	if (NULL == object) {
		LOGE("[ERROR] Fail to get json object");
		return -1;
	}

	root = json_object_get_member(object, "displayText");
	if (NULL == root) {
		LOGE("[ERROR] Fail to get member node");
		return -1;
	}
	dispText = json_node_get_string(root);
	if (NULL == dispText) {
		LOGE("[ERROR] Fail to get string");
		return -1;
	}

	const char *tempText = dispText;
	for (tempText = dispText; isspace(*tempText); tempText++);

	if (!strcmp("", tempText)) {
		root = json_object_get_member(object, "spokenText");
		if (NULL == root) {
			LOGE("[ERROR] Fail to get root node");
			return -1;
		}

		dispText = json_node_get_string(root);
		if (NULL == dispText) {
			LOGE("[ERROR] Fail to get string");
			return -1;
		}
	}

	LOGD("displayText - %s", dispText);
	*displayText = strdup(dispText);

	if (NULL == *displayText) {
		LOGE("[ERROR] Fail to memory allocation");
		return -1;
	}

	return 0;
}
#endif

static Eina_Bool __start_cb(void *data)
{
	appdata *ad = (appdata *)data;
	ad->act_state = 0;

	vc_service_state_e state;
	if (0 != vc_mgr_get_service_state(&state)) {
		LOGE("[ERROR] Fail to get service state");
		return EINA_FALSE;
	}

	if (VC_SERVICE_STATE_READY != state) {
		LOGD("[WARNING] Wait for service state ready");
		return EINA_TRUE;
	}

	if (0 != vc_mgr_set_recognition_mode(VC_RECOGNITION_MODE_STOP_BY_SILENCE)) {
		LOGE("[ERROR] Fail to set recognition mode");
	}

	if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
		LOGE("[ERROR] Fail to set command list");
	}

	if (0 != __vc_panel_vc_get_current_commands(ad)) {
		LOGE("[ERROR] Fail to get current commands");
	}

	if (0 != vc_mgr_start(false)) {
		LOGE("[ERROR] Fail to start");
	}

	return EINA_FALSE;
}

static void __vc_panel_vc_state_changed_cb(vc_state_e previous, vc_state_e current, void *user_data)
{
	LOGD("==== State is changed ====");
	LOGD("Previous(%d) -> Current(%d)", previous, current);

	appdata *ad = (appdata*)user_data;

	if (VC_STATE_INITIALIZED == previous && VC_STATE_READY == current) {
		vc_panel_vc_start(ad);
	}
}

static void __vc_panel_vc_error_cb(vc_error_e reason, void *user_data)
{
	LOGD("==== Error handling ====");
	char *err_msg = NULL;
	char disp_text[256];

	if (0 != vc_mgr_get_error_message(&err_msg)) {
		LOGE("[ERROR] Fail to get error message");
		vc_panel_view_show_result(user_data, "Unknow error is occurred!!");

		return;
	}

	if (NULL != err_msg) {
		snprintf(disp_text, 256, "Error is occurred!! (%s)", err_msg);
		vc_panel_view_show_result(user_data, disp_text);
		free(err_msg);
		err_msg = NULL;
	}

	LOGD("====");
	LOGD(" ");
}

static void __vc_panel_vc_reset(void *data)
{
	appdata *ad = (appdata *)data;

	ad->current_depth = 1;
	ad->current_path[0] = -1;
	ad->current_path[1] = -1;

	if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
		LOGE("[ERROR] Fail to set command list");
	}
}

static Eina_Bool __vc_panel_vc_finalize(void *data)
{
	LOGD("=== Finalize ===");

	ui_app_exit();
	return EINA_FALSE;
}

static Eina_Bool __vc_panel_vc_deactivate(void *data)
{
	LOGD("=== Deactivate ===");

	tts_state_e state;

	tts_get_state(g_tts, &state);
	if (TTS_STATE_PLAYING == state) {
		LOGD("[WARNING] Processing Dialog");
		return EINA_TRUE;
	}

	if (0 != vc_panel_vc_cancel(data)) {
		LOGE("[ERROR] Fail to deactivate");
		return EINA_FALSE;
	}

	vc_panel_view_show_result(data, _("IDS_RESTART"));

	return EINA_FALSE;
}

static Eina_Bool __vc_panel_vc_restart(void *data)
{
	LOGD("==== Restart ====");

	appdata *ad = (appdata *)data;
	ad->act_state = 0;

	vc_service_state_e state;
	if (0 != vc_mgr_get_service_state(&state)) {
	    LOGE("[ERROR] Fail to get service state");
	    return EINA_FALSE;
	}

	if (VC_SERVICE_STATE_READY != state) {
	    LOGD("[WARNING] Wait for service state ready");
	    return EINA_FALSE;
	}

	/* set current command list */
	if (1 == ad->current_depth) {
		if (0 != vc_mgr_set_command_list(g_cmd_list_1st)) {
			LOGE("[ERROR] Fail to set command list");
		}
	} else if (2 == ad->current_depth) {
		if (0 != vc_mgr_set_command_list(g_cmd_list_2nd[ad->current_path[0]])) {
			LOGE("[ERROR] Fail to set command list");
		}
	}

	vc_panel_view_show(ad);

	if (0 != __vc_panel_vc_get_current_commands(ad)) {
		LOGE("[ERROR] Fail to get current commands");
	}

	if (0 != vc_mgr_start(false)) {
		LOGE("[ERROR] Fail to start");
	}

	LOGD("====");
	LOGD(" ");

	return EINA_FALSE;
}

static void __vc_panel_vc_service_state_changed_cb(vc_service_state_e previous, vc_service_state_e current, void *user_data)
{
	LOGD("==== Service state is changed ====");
	LOGD("Previous(%d) -> Current(%d)", previous, current);

	appdata *ad = (appdata *)user_data;

	if (VC_SERVICE_STATE_READY == previous && VC_SERVICE_STATE_RECORDING == current) {
		LOGD("==== Show by recording ====");
		vc_panel_view_show(ad);
		g_final_status = 0;
	} else if ((VC_SERVICE_STATE_RECORDING == previous || VC_SERVICE_STATE_PROCESSING == previous) && VC_SERVICE_STATE_READY == current) {
		if (PANEL_STATE_SERVICE == ad->app_state) {
			LOGD("==== Process finish ====");
		} else {
			LOGD("==== Hide ====");
			__vc_panel_vc_reset(ad);
		}
		g_final_status = 0;
	} else if (VC_SERVICE_STATE_RECORDING == previous && VC_SERVICE_STATE_PROCESSING == current) {
		LOGD("==== Processing ====");
		vc_panel_view_show_result(ad, _("IDS_PROCESSING"));
		g_final_status = 1;
	}
}

static bool __vc_panel_search_cmd_cb(vc_cmd_h vc_command, void* user_data)
{
	LOGD("==== Search the selected command cb ====");

	vc_cmd_list_h vc_cmd_list;
	intptr_t p_selected_pid = (intptr_t)user_data;
	int selected_pid = (int)p_selected_pid;
	int pid;

	if (0 != vc_cmd_get_pid(vc_command, &pid)) {
		LOGE("[ERROR] fail to get pid");
		return true;
	}

	if (selected_pid == pid) {
		if (0 != vc_cmd_list_create(&vc_cmd_list)) {
			LOGE("[ERROR] Fail to create the command list");
			return false;
		}
		if (0 != vc_cmd_list_add(vc_cmd_list, vc_command)) {
			LOGE("[ERROR] Fail to add the command");

			if (0 != vc_cmd_list_destroy(vc_cmd_list, false)) {
				LOGE("[ERROR] Fail to destroy the command list");
			}

			return false;
		}

		if (0 != vc_mgr_set_selected_results(vc_cmd_list)) {
			LOGE("[ERROR] Fail to select the app");

			if (0 != vc_cmd_list_destroy(vc_cmd_list, false)) {
				LOGE("[ERROR] Fail to destroy the command list");
			}

			return false;
		}

		if (0 != vc_cmd_list_destroy(vc_cmd_list, false)) {
			LOGE("[ERROR] Fail to destroy the command list");
		}

		return false;
	}

	return true;
}

static void __vc_panel_item_selected_cb(void *data, Evas_Object *obj, void *event_info)
{
	LOGD("==== Command list item selected cb ====");

	appdata *ad = (appdata*)data;
	Elm_Object_Item *it = (Elm_Object_Item*)event_info;
	intptr_t pid = (intptr_t)elm_object_item_data_get(it);

	if (0 != vc_cmd_list_foreach_commands(g_candidate_cmd_list, __vc_panel_search_cmd_cb, (void*)pid)) {
		LOGE("[ERROR] Fail to search the command list");
	}

	if (0 != vc_cmd_list_remove_all(g_candidate_cmd_list, true)) {
		LOGE("[ERROR] Fail to destroy the command list");
	}

	vc_panel_view_hide_help(ad);

	LOGD("====");
	LOGD("");
}

static int __vc_panel_copy_command(vc_cmd_h src_cmd, vc_cmd_h des_cmd)
{
	LOGD("==== Start copying command ====");

	int ret;
	int value;
	char *command = NULL;

	ret = vc_cmd_get_domain(src_cmd, &value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to get domain(%d)", ret);
		return -1;
	}
	ret = vc_cmd_set_domain(des_cmd, value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to set domain(%d)", ret);
		return -1;
	}

	ret = vc_cmd_get_pid(src_cmd, &value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to get pid(%d)", ret);
		return -1;
	}
	ret = vc_cmd_set_pid(des_cmd, value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to set pid(%d)", ret);
		return -1;
	}

	ret = vc_cmd_get_type(src_cmd, &value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to get type(%d)", ret);
		return -1;
	}
	ret = vc_cmd_set_type(des_cmd, value);
	if (0 != ret) {
		LOGE("[ERROR] Fail to set type(%d)", ret);
		return -1;
	}

	ret = vc_cmd_get_command(src_cmd, &command);
	if (0 != ret) {
		LOGE("[ERROR] Fail to get command(%d)", ret);
		return -1;
	}
	ret = vc_cmd_set_command(des_cmd, command);
	if (NULL != command) {
		free(command);
		command = NULL;
	}
	if (0 != ret) {
		LOGE("[ERROR] Fail to set command(%d)", ret);
		return -1;
	}

	ret = vc_cmd_get_unfixed_command(src_cmd, &command);
	if (0 != ret) {
		LOGE("[ERROR] Fail to get unfixed command(%d)", ret);
		return -1;
	}
	ret = vc_cmd_set_unfixed_command(des_cmd, command);
	if (NULL != command) {
		free(command);
		command = NULL;
	}
	if (0 != ret) {
		LOGE("[ERROR] Fail to set unfixed command(%d)", ret);
		return -1;
	}

	LOGD("====");
	LOGD(" ");

	return 0;
}

#if 0
static bool __vc_panel_vc_app_chooser_cb(vc_cmd_h vc_command, void* user_data)
{
	LOGD("==== App chooser cb ====");

	vc_cmd_h cmd_backup;
	char *json = NULL;
	int pid;

	if (0 != vc_cmd_create(&cmd_backup)) {
		LOGE("[ERROR] Fail to create command handle");
		return false;
	}

	if (0 != __vc_panel_copy_command(vc_command, cmd_backup)) {
		LOGE("[ERROR] Fail to copy the command");
	}

	if (0 != vc_cmd_get_pid(vc_command, &pid)) {
		LOGE("[ERROR] fail to get pid of the command");

		if (0 != vc_cmd_destroy(cmd_backup)) {
			LOGE("[ERROR] fail to destroy command");
		}

		return false;
	}

	if (0 == vc_cmd_get_nlu_json(vc_command, &json)) {
		LOGD("NLU command");

		free(json);
		json = NULL;

		if (getpid() == pid) {
			if (0 != vc_cmd_destroy(cmd_backup)) {
				LOGE("[ERROR] fail to destroy command");
			}

			return true;
		}
	}

	if (0 != vc_cmd_list_add(g_candidate_cmd_list, cmd_backup)) {
		LOGE("[ERROR] Fail to add command");
		if (0 != vc_cmd_destroy(cmd_backup)) {
			LOGE("[ERROR] fail to destroy command");
		}
		return false;
	}

	vc_panel_add_app(pid, &g_num_candidate, user_data);

	LOGD("g_num_candidate : %d", g_num_candidate);

	// No candidate
	g_num_candidate = 0;

	return true;
}
#endif

static bool __vc_panel_vc_all_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char* result, const char* msg, void *user_data)
{
	LOGD("==== All result cb ====");
	LOGD("Result Text - %s", result);

	if (g_final_status) {
		bool ret;
		ret = vc_panel_action(g_final_result, user_data);
		if (true != ret) {
			vc_panel_vc_finalize(user_data);
		}
	}

#if 0
	appdata *ad = (appdata*)user_data;
	if (VC_RESULT_EVENT_REJECTED == event) {
		LOGD("Rejected event");
		vc_panel_view_show_result(ad, _("IDS_SORRY"));
	} else {
		if (result) {
			LOGD("Result Text - %s", result);
			vc_panel_view_show_result(ad, result);

			vc_panel_view_hide_help(ad);

			if (ad->help_genlist) {
				elm_genlist_clear(ad->help_genlist);
			}

			g_num_candidate = 0;

			if (0 != vc_cmd_list_foreach_commands(vc_cmd_list, __vc_panel_vc_app_chooser_cb, ad)) {
				LOGE("[ERROR] fail to search command list");
				return true;
			}

			LOGD("g_num_candidate : %d", g_num_candidate);

			if (1 < g_num_candidate) {
				if (NULL != g_deactive_timer) {
					ecore_timer_del(g_deactive_timer);
					g_deactive_timer = NULL;
				}
				vc_panel_view_show_app_list(ad);

				LOGD("====");
				LOGD(" ");

				return false;
			}


			if (0 != vc_cmd_list_remove_all(g_candidate_cmd_list, true)) {
				LOGE("[ERROR] Fail to destroy the command list");
			}
		}
	}
#endif

	LOGD("====");
	LOGD(" ");

	return true;
}

static void __vc_panel_vc_pre_result_cb(vc_pre_result_event_e event, const char* pre_result, void *user_data)
{
	LOGD("==== Pre result cb ====");

	if (g_final_result) {
		free(g_final_result);
		g_final_result = NULL;
	}

	g_final_result = strdup(pre_result);
}

static void __vc_panel_vc_result_cb(vc_result_event_e event, vc_cmd_list_h vc_cmd_list, const char* result, void *user_data)
{
	LOGD("==== Result cb ====");
#if 0
	appdata* ad = user_data;
#endif

	if (NULL != result) {
		LOGD("Result Text - %s", result);

		vc_cmd_list_first(vc_cmd_list);
		vc_cmd_h result_cmd;
		vc_cmd_list_get_current(vc_cmd_list, &result_cmd);

#if 0
		char* json = NULL;
		if (0 != vc_cmd_get_nlu_json(result_cmd, &json) || NULL == json) {
			LOGE("[ERROR] Fail to get nlu json");
		} else {
			LOGD("Result Json - %s", json);

			char *displayText = NULL;
			if (0 == __vc_panel_find_dispText(json, &displayText)) {
				vc_panel_view_show_result(user_data, displayText);

				free(displayText);
				displayText = NULL;
			}

			free(json);
			json = NULL;
		}

		if (0 == ad->act_state) {
			bool ret;
			ret = vc_panel_action(result, user_data);
			ad->act_state = 1;
			if (true != ret) {
				vc_panel_vc_finalize(user_data);
			}
		}
#endif
	}
}

static void __vc_panel_vc_speech_detected_cb(void *user_data)
{
	//LOGD("==== Speech detected ====");
}

static void __vc_panel_vc_language_changed_cb(const char* previous, const char* current, void* user_data)
{
	LOGD("Language is changed (%s) to (%s)", previous, current);

	char loc[64] = {'\0', };
	snprintf(loc, 64, "%s.UTF-8", current);

	setlocale(LC_ALL, loc);

	__vc_panel_vc_destroy_command_list();
	if (0 != __vc_panel_vc_create_command_list()) {
		LOGE("[ERROR] Fail to create command list");
	}

	if (NULL != g_tts_lang) {
		free(g_tts_lang);
	}

	g_tts_lang = strdup(current);
}

int vc_panel_vc_play_text(const char *utt_text)
{
	int utt_id = 0;

	//play tts
	if (0 != tts_add_text(g_tts, utt_text, g_tts_lang, TTS_VOICE_TYPE_AUTO, TTS_SPEED_AUTO, &utt_id)) {
		LOGE("[WARNING] Text adding is failed");
	}

	if (0 != tts_play(g_tts)) {
		LOGE("[WARNING] Text playing is failed");
	}

	return 0;
}

static void __vc_panel_vc_mgr_dialog_request_cb(int pid, const char *disp_text, const char *utt_text, bool continuous, void *user_data)
{
	int utt_id;

	LOGD("==== Requested dialog processing start ====");

	if (NULL != g_deactive_timer) {
		ecore_timer_del(g_deactive_timer);
		g_deactive_timer = NULL;
	}

	if (true == g_dialog_process) {
		vc_panel_view_show_result(user_data, "Please, try again after moments");
	} else {
		//display the text
		vc_panel_view_show_result(user_data, disp_text);

		g_dialog_continuous = continuous;
		g_dialog_process = true;

		//play tts
		if (0 != tts_add_text(g_tts, utt_text, g_tts_lang, TTS_VOICE_TYPE_AUTO, TTS_SPEED_AUTO, &utt_id)) {
			LOGE("[WARNING] Text adding is failed");
		}

		if (0 != tts_play(g_tts)) {
			LOGE("[WARNING] Text playing is failed");
		}
	}
}

static void __vc_panel_tts_utterance_completed_cb(tts_h tts, int utt_id, void* user_data)
{
	LOGD("==== Utterance (%d) is completed ====", utt_id);

	if (0 != tts_stop(tts)) {
		LOGE("[ERROR] Fail to stop tts");
	}

	if (true == g_dialog_continuous) {
		vc_panel_vc_activate(user_data);
	} else {
		vc_panel_vc_deactivate(user_data, 0.5);
	}

	g_dialog_process = false;
}

int vc_panel_vc_start(void *data)
{
	LOGD("=== Start the voice control panel ===");

	appdata* ad = data;
	ad->act_state = 0;

	ecore_thread_main_loop_begin();
	ecore_idler_add(__start_cb, data);
	ecore_thread_main_loop_end();

	return 0;
}

int vc_panel_vc_cancel(void *data)
{
	vc_service_state_e service_state;

	if (0 != vc_mgr_get_service_state(&service_state)) {
		LOGE("[ERROR] Fail to get service state");
		return -1;
	}

	if ((VC_SERVICE_STATE_RECORDING == service_state) || (VC_SERVICE_STATE_PROCESSING == service_state)) {
		LOGD("==== service state (%d)", service_state);

		if (0 != vc_mgr_cancel()) {
			LOGE("[ERROR] Fail to cancel");
			return -1;
		}
	}

	return 0;
}

int vc_panel_vc_init(void *data)
{
	LOGD("==== Initialze Voice control ====");
	appdata *ad = (appdata *)data;

	if (0 != vc_mgr_initialize()) {
		LOGE("[ERROR] Fail to initialize");
		return -1;
	}

	if (0 != vc_mgr_set_state_changed_cb(__vc_panel_vc_state_changed_cb, data)) {
		LOGE("[ERROR] Fail to set state changed cb");
		return -1;
	}

	if (0 != vc_mgr_set_service_state_changed_cb(__vc_panel_vc_service_state_changed_cb, data)) {
		LOGE("[ERROR] Fail to set service state changed cb");
		return -1;
	}

	if (0 != vc_mgr_set_all_result_cb(__vc_panel_vc_all_result_cb, data)) {
		LOGE("[ERROR] Fail to set all result cb");
		return -1;
	}

	if (0 != vc_mgr_set_pre_result_cb(__vc_panel_vc_pre_result_cb, data)) {
		LOGE("[ERROR] Fail to set pre result cb");
		return -1;
	}

	if (0 != vc_mgr_set_result_cb(__vc_panel_vc_result_cb, data)) {
		LOGE("[ERROR] Fail to set result cb");
		return -1;
	}

	if (0 != vc_mgr_set_speech_detected_cb(__vc_panel_vc_speech_detected_cb, data)) {
		LOGE("[ERROR] Fail to set speech detected cb");
		return -1;
	}

	if (0 != vc_mgr_set_current_language_changed_cb(__vc_panel_vc_language_changed_cb, data)) {
		LOGE("[ERROR] Fail to set language changed cb");
		return -1;
	}

	if (0 != vc_mgr_set_dialog_request_cb(__vc_panel_vc_mgr_dialog_request_cb, data)) {
		LOGE("[ERROR] Fail to set dialog request cb");
		return -1;
	}

	if (0 != vc_mgr_set_error_cb(__vc_panel_vc_error_cb, data)) {
		LOGE("[ERROR] Fail to set error cb");
		return -1;
	}

	if (0 != vc_mgr_prepare()) {
		LOGE("[ERROR] Fail to prepare");
		return -1;
	}

	if (0 != __vc_panel_vc_create_command_list()) {
		LOGE("[ERROR] Fail to create command list");
		return -1;
	}

	//candidate command list create
	if (0 != vc_cmd_list_create(&g_candidate_cmd_list)) {
		LOGE("[ERROR] Fail to create the command list");
		return -1;
	}

	evas_object_smart_callback_add(ad->help_genlist, "selected", __vc_panel_item_selected_cb, ad);

	//tts handle create & set
	if (0 != tts_create(&g_tts)) {
		LOGE("[ERROR] Fail to create tts handle");
		return -1;
	}

	if (0 != tts_set_utterance_completed_cb(g_tts, __vc_panel_tts_utterance_completed_cb, data)) {
		LOGE("[ERROR] Fail to set utt completed cb");
		tts_destroy(g_tts);

		return -1;
	}

	char *language = NULL;
	int voice_type;

	if (0 != tts_get_default_voice(g_tts, &language, &voice_type) || NULL == language) {
		LOGE("[ERROR] Fail to get default voice");
		tts_destroy(g_tts);

		return -1;
	}

	g_tts_lang = strdup(language);

	free(language);
	language = NULL;

	if (NULL == g_tts_lang) {
		LOGE("[ERROR] Fail to memory allocation for voice information");
		tts_destroy(g_tts);

		return -1;
	}

	if (0 != tts_prepare(g_tts)) {
		LOGE("[ERROR] Fail to tts prepare");
		tts_destroy(g_tts);

		return -1;
	}

	g_json_parser = json_parser_new();

	if (NULL == g_json_parser) {
		LOGE("[ERROR] Fail to memory allocation for json parser");
		tts_destroy(g_tts);

		return -1;
	}

	// initialize virtual touch device
	ad->touch = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_TOUCHSCREEN);

	if (NULL == ad->touch) {
		LOGE("[ERROR] initialize efl util failed");
		tts_destroy(g_tts);
		g_object_unref(g_json_parser);

		return -1;
	}

	ad->back_key = efl_util_input_initialize_generator(EFL_UTIL_INPUT_DEVTYPE_KEYBOARD);

	if (NULL == ad->back_key) {
		LOGE("[ERROR] initialize efl util failed");
		tts_destroy(g_tts);
		g_object_unref(g_json_parser);
		efl_util_input_deinitialize_generator(ad->touch);

		return -1;
	}

	vc_panel_touch_set_speed(800);

	LOGD("====");
	LOGD(" ");

	return 0;
}

int vc_panel_vc_deinit(void *data)
{
	LOGD("==== De-initialize Voice control ====");

	appdata *ad = (appdata *)data;

	if (0 < g_list_length(ad->cmd_list)) {
		GList *iter = NULL;
		iter = g_list_first(ad->cmd_list);

		while (NULL != iter) {
			char *cmd = iter->data;
			if (NULL != cmd) {
				free(cmd);
				cmd = NULL;
			}

			ad->cmd_list = g_list_remove_link(ad->cmd_list, iter);
			iter = g_list_first(ad->cmd_list);
		}
	}

	__vc_panel_vc_destroy_command_list();

	if (0 != vc_mgr_deinitialize()) {
		LOGE("[ERROR] Fail to deinitialize");
		return -1;
	}

	if (NULL != g_tts_lang) {
		free(g_tts_lang);
		g_tts_lang = NULL;
	}

	if (0 != tts_destroy(g_tts)) {
		LOGE("[ERROR] Fail to tts destroy");
		return -1;
	}

	if (0 != vc_cmd_list_destroy(g_candidate_cmd_list, true)) {
		LOGE("[ERROR] Fail to command list destroy");
		return -1;
	}

	g_object_unref(g_json_parser);

	if (0 != efl_util_input_deinitialize_generator(ad->touch)) {
		LOGE("[ERROR] Fail to deinitialize");
		return -1;
	}
	if (0 != efl_util_input_deinitialize_generator(ad->back_key)) {
		LOGE("[ERROR] Fail to deinitialize");
		return -1;
	}

	LOGD("====");
	LOGD(" ");

	return 0;
}

int vc_panel_vc_activate(void *data)
{
	if (NULL != g_deactive_timer) {
		ecore_timer_del(g_deactive_timer);
		g_deactive_timer = NULL;
	}

	ecore_idler_add(__vc_panel_vc_restart, data);

	return 0;
}

int vc_panel_vc_deactivate(void *data, double delay)
{
	if (NULL != g_deactive_timer) {
		ecore_timer_del(g_deactive_timer);
		g_deactive_timer = NULL;
	}

	g_deactive_timer = ecore_timer_add(delay, __vc_panel_vc_deactivate, data);

	return 0;
}

int vc_panel_vc_finalize(void *data)
{
	ecore_idler_add(__vc_panel_vc_finalize, data);

	return 0;
}

/*
vi:ts=4:ai:nowrap:expandtab
*/
