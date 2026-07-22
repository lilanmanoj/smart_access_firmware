/**
 * @file ui_home.cpp
 * Home screen: status bar + scanner banner + 4 action buttons.
 *
 * Screen is used in landscape, i.e. 320 x 240.
 */

#include "ui_home.h"
#include <stdio.h>

/* ---------------- palette ---------------- */
#define COL_BG          lv_color_hex(0x101418)
#define COL_STATUS_BAR  lv_color_hex(0x1565C0)  /* blue  */
#define COL_GREEN       lv_color_hex(0x2E7D32)  /* green */
#define COL_GREEN_LIGHT lv_color_hex(0x4CAF50)
#define COL_AMBER       lv_color_hex(0xFFB300)
#define COL_RED         lv_color_hex(0xE53935)
#define COL_BTN         lv_color_hex(0x263238)
#define COL_BTN_SEL     lv_color_hex(0x1565C0)

/* ---------------- widget handles ---------------- */
static lv_obj_t *lbl_enrolls;
static lv_obj_t *lbl_net;
static lv_obj_t *panel_scanner;
static lv_obj_t *lbl_scanner;
static lv_obj_t *btns[4];

static const char *BTN_TEXT[4] = {"Enroll", "OTP\nAccess", "Request\nAccess", "Settings"};

/* -------------------------------------------------------------------------
 * A fingerprint glyph built from concentric arcs — LVGL's symbol font has no
 * fingerprint, and arcs scale cleanly without shipping a bitmap.
 * ---------------------------------------------------------------------- */
static void fp_ridge(lv_obj_t *parent, int16_t size, uint16_t start, uint16_t end,
                     uint8_t thickness, lv_color_t color)
{
    lv_obj_t *arc = lv_arc_create(parent);
    lv_obj_set_size(arc, size, size);
    lv_obj_center(arc);
    lv_obj_remove_style(arc, NULL, LV_PART_KNOB);          /* no drag knob   */
    lv_obj_clear_flag(arc, LV_OBJ_FLAG_CLICKABLE);         /* decorative     */
    lv_obj_set_style_arc_opa(arc, LV_OPA_TRANSP, LV_PART_MAIN);  /* hide track */
    lv_obj_set_style_arc_color(arc, color, LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(arc, thickness, LV_PART_INDICATOR);
    lv_obj_set_style_arc_rounded(arc, true, LV_PART_INDICATOR);
    lv_arc_set_rotation(arc, 0);
    lv_arc_set_bg_angles(arc, start, end);
    lv_arc_set_angles(arc, start, end);
}

static lv_obj_t *fingerprint_icon_create(lv_obj_t *parent, int16_t size)
{
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, size, size);
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_pad_all(cont, 0, 0);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);

    /* 0 deg = 3 o'clock, angles run clockwise, so 180..360 is the top half. */
    const lv_color_t c = lv_color_white();
    fp_ridge(cont, size,          200, 340, 4, c);
    fp_ridge(cont, size * 3 / 4,  190, 350, 4, c);
    fp_ridge(cont, size / 2,      200, 340, 4, c);
    fp_ridge(cont, size / 4,      170,  10, 4, c);   /* the core, wraps past 0 */
    return cont;
}

/* ------------------------------- status bar ------------------------------ */
static void status_bar_create(lv_obj_t *parent)
{
    lv_obj_t *bar = lv_obj_create(parent);
    lv_obj_set_size(bar, LV_PCT(100), 26);
    lv_obj_align(bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(bar, COL_STATUS_BAR, 0);
    lv_obj_set_style_bg_opa(bar, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(bar, 0, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_pad_hor(bar, 8, 0);
    lv_obj_set_style_pad_ver(bar, 0, 0);
    lv_obj_clear_flag(bar, LV_OBJ_FLAG_SCROLLABLE);

    lbl_enrolls = lv_label_create(bar);
    lv_obj_set_style_text_color(lbl_enrolls, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_enrolls, &lv_font_montserrat_14, 0);
    lv_obj_align(lbl_enrolls, LV_ALIGN_LEFT_MID, 0, 0);
    lv_label_set_text(lbl_enrolls, "Enrolled: 1/162");

    lbl_net = lv_label_create(bar);
    /* White, not green: green reads poorly against the blue status bar. */
    lv_obj_set_style_text_color(lbl_net, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_net, &lv_font_montserrat_16, 0);
    lv_obj_align(lbl_net, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_label_set_text(lbl_net, LV_SYMBOL_WIFI);
}

/* ----------------------------- scanner banner ---------------------------- */
static void scanner_panel_create(lv_obj_t *parent)
{
    panel_scanner = lv_obj_create(parent);
    lv_obj_set_size(panel_scanner, 304, 112);
    lv_obj_align(panel_scanner, LV_ALIGN_TOP_MID, 0, 34);
    lv_obj_set_style_bg_color(panel_scanner, COL_GREEN, 0);
    lv_obj_set_style_bg_opa(panel_scanner, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(panel_scanner, 10, 0);
    lv_obj_set_style_border_width(panel_scanner, 0, 0);
    lv_obj_set_style_pad_all(panel_scanner, 10, 0);
    lv_obj_clear_flag(panel_scanner, LV_OBJ_FLAG_SCROLLABLE);

    /* Landscape: icon on the left, text on the right. */
    lv_obj_set_flex_flow(panel_scanner, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(panel_scanner, LV_FLEX_ALIGN_CENTER,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(panel_scanner, 14, 0);

    fingerprint_icon_create(panel_scanner, 80);

    lbl_scanner = lv_label_create(panel_scanner);
    lv_obj_set_style_text_color(lbl_scanner, lv_color_white(), 0);
    lv_obj_set_style_text_font(lbl_scanner, &lv_font_montserrat_18, 0);
    lv_obj_set_style_text_align(lbl_scanner, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(lbl_scanner, "Scanner Ready\nPlace your finger");
}

/* ------------------------------- button row ------------------------------ */
static void button_row_create(lv_obj_t *parent)
{
    lv_obj_t *row = lv_obj_create(parent);
    lv_obj_set_size(row, 320, 76);
    lv_obj_align(row, LV_ALIGN_BOTTOM_MID, 0, -4);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_hor(row, 8, 0);
    lv_obj_set_style_pad_column(row, 6, 0);
    lv_obj_clear_flag(row, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    for (int i = 0; i < 4; i++) {
        lv_obj_t *btn = lv_btn_create(row);
        /* row 320 - 16 pad - 3*6 gaps = 286 -> 71 each */
        lv_obj_set_size(btn, 71, 66);
        lv_obj_set_style_bg_color(btn, COL_BTN, 0);
        lv_obj_set_style_bg_color(btn, COL_BTN_SEL, LV_STATE_PRESSED);
        lv_obj_set_style_radius(btn, 8, 0);
        lv_obj_set_style_border_width(btn, 1, 0);
        lv_obj_set_style_border_color(btn, lv_color_hex(0x37474F), 0);
        lv_obj_set_style_pad_all(btn, 2, 0);

        lv_obj_t *num = lv_label_create(btn);
        lv_label_set_text_fmt(num, "%d", i + 1);
        lv_obj_set_style_text_color(num, COL_GREEN_LIGHT, 0);
        lv_obj_set_style_text_font(num, &lv_font_montserrat_18, 0);
        lv_obj_align(num, LV_ALIGN_TOP_MID, 0, 2);

        lv_obj_t *txt = lv_label_create(btn);
        lv_label_set_text(txt, BTN_TEXT[i]);
        lv_obj_set_style_text_color(txt, lv_color_white(), 0);
        lv_obj_set_style_text_font(txt, &lv_font_montserrat_12, 0);
        lv_obj_set_style_text_align(txt, LV_TEXT_ALIGN_CENTER, 0);
        /* 67px of usable width; wrap rather than clip if a label grows. */
        lv_obj_set_width(txt, 67);
        lv_label_set_long_mode(txt, LV_LABEL_LONG_WRAP);
        lv_obj_align(txt, LV_ALIGN_BOTTOM_MID, 0, -2);

        btns[i] = btn;
    }
}

/* --------------------------------- API ----------------------------------- */
void ui_home_create(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, COL_BG, 0);
    lv_obj_set_style_pad_all(scr, 0, 0);
    lv_obj_clear_flag(scr, LV_OBJ_FLAG_SCROLLABLE);

    status_bar_create(scr);
    scanner_panel_create(scr);
    button_row_create(scr);
}

void ui_home_set_enrolls(uint16_t used, uint16_t capacity)
{
    lv_label_set_text_fmt(lbl_enrolls, "Enrolled: %u/%u", used, capacity);
}

void ui_home_set_net_state(ui_net_state_t state)
{
    /* Online is white for legibility on blue; the fault states keep their
       colours, which still read clearly against the bar. */
    lv_color_t c = COL_RED;
    if (state == UI_STATUS_ONLINE)          c = lv_color_white();
    else if (state == UI_STATUS_CONNECTING) c = COL_AMBER;
    lv_obj_set_style_text_color(lbl_net, c, 0);
}

void ui_home_set_scanner_text(const char *text, lv_color_t bg)
{
    lv_label_set_text(lbl_scanner, text);
    lv_obj_set_style_bg_color(panel_scanner, bg, 0);
}

void ui_home_select_button(int index)
{
    for (int i = 0; i < 4; i++) {
        lv_obj_set_style_bg_color(btns[i], (i == index) ? COL_BTN_SEL : COL_BTN, 0);
    }
}
