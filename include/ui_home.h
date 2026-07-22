/**
 * @file ui_home.h
 * Home screen of the smart access panel (320x240 landscape).
 */

#ifndef UI_HOME_H
#define UI_HOME_H

#include <lvgl.h>

typedef enum {
    UI_STATUS_OFFLINE = 0,
    UI_STATUS_CONNECTING,
    UI_STATUS_ONLINE,
} ui_net_state_t;

/** Build the home screen and load it. Call once, after lv_init(). */
void ui_home_create(void);

/** Update the "Enrolled: x/y" counter in the status bar. */
void ui_home_set_enrolls(uint16_t used, uint16_t capacity);

/** Recolour the network icon: white online, amber connecting, red offline. */
void ui_home_set_net_state(ui_net_state_t state);

/** Replace the scanner banner text and its background colour. */
void ui_home_set_scanner_text(const char *text, lv_color_t bg);

/** Highlight one of the four buttons (0..3), or -1 to clear the highlight. */
void ui_home_select_button(int index);

#endif /* UI_HOME_H */
