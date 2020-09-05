#ifndef __VOICE_CONTROL_PANEL_VIEW_H
#define __VOICE_CONTROL_PANEL_VIEW_H

#ifdef __cplusplus
extern "C" {
#endif

int vc_panel_view_create(void *data);
void vc_panel_view_destroy(void *data);
int vc_panel_view_show(void *data);
int vc_panel_view_hide(void *data);
int vc_panel_add_app(int pid, int *count, void *data);
int vc_panel_view_show_app_list(void *data);
int vc_panel_view_show_help(void *data);
int vc_panel_view_hide_help(void *data);
int vc_panel_view_show_result(void *data, const char *result);

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_VIEW_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
