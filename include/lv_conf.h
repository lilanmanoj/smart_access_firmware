/**
 * @file lv_conf.h
 * Minimal LVGL v8.3 configuration for the smart access panel.
 *
 * Anything not listed here falls back to the LVGL default defined in
 * lv_conf_internal.h, so this file only carries the deltas we care about.
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR / DISPLAY
 *====================*/
#define LV_COLOR_DEPTH     16
#define LV_COLOR_16_SWAP   1        /* TFT_eSPI pushes big-endian 16 bit pixels */

/*====================
   MEMORY
 *====================*/
#define LV_MEM_CUSTOM      0
#define LV_MEM_SIZE        (48U * 1024U)

/*====================
   TICK SOURCE
 *====================*/
#define LV_TICK_CUSTOM                     1
#define LV_TICK_CUSTOM_INCLUDE             "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR       (millis())

/*====================
   FEATURES
 *====================*/
#define LV_DRAW_COMPLEX    1        /* shadows, rounded masks, arcs */
#define LV_USE_PERF_MONITOR 0
#define LV_USE_MEM_MONITOR  0
#define LV_USE_LOG          0

/*====================
   FONTS
 *====================*/
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_DEFAULT         &lv_font_montserrat_14

/*====================
   THEME
 *====================*/
#define LV_USE_THEME_DEFAULT    1
#define LV_THEME_DEFAULT_DARK   0
#define LV_THEME_DEFAULT_GROW   0

#endif /*LV_CONF_H*/
