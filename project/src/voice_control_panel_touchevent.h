#ifndef __VOICE_CONTROL_PANEL_TOUCHEVENT_H
#define __VOICE_CONTROL_PANEL_TOUCHEVENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define VC_PANEL_TOUCH_SWIPE_UP 1
#define VC_PANEL_TOUCH_SWIPE_DOWN 2
#define VC_PANEL_TOUCH_SWIPE_LEFT 3
#define VC_PANEL_TOUCH_SWIPE_RIGHT 4

int vc_panel_touch_set_speed(const int pps);
void vc_panel_touch_swipe(const int direction, void* data);
void vc_panel_touch_back(void* data);

#ifdef __cplusplus
}
#endif

#endif /* __VOICE_CONTROL_PANEL_TOUCHEVENT_H */

/*
vi:ts=4:ai:nowrap:expandtab
*/
