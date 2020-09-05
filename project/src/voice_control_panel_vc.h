#ifndef __VOICE_CONTROL_PANEL_VC_H
#define __VOICE_CONTROL_PANEL_VC_H

#ifdef __cplusplus
extern "C" {
#endif

int vc_panel_vc_start(void *data);
int vc_panel_vc_cancel(void *data);
int vc_panel_vc_init(void *data);
int vc_panel_vc_deinit(void *data);
int vc_panel_vc_deactivate(void *data, double delay);
int vc_panel_vc_activate(void *data);
int vc_panel_vc_finalize(void *data);
int vc_panel_vc_play_text(const char *utt_text);

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_VC_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
