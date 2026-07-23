/**
 * Smart Access Panel — display bring-up.
 *
 * Board   : ESP32-S3-DevKitC-1
 * Display : GMT024-08-SPI8P, 2.4" 240x320 IPS TFT, ILI9341, 8-pin SPI (no touch)
 * GUI     : LVGL 8.3 over TFT_eSPI, used in landscape (320x240)
 *
 * Wiring (also mirrored in platformio.ini build_flags):
 *   VCC -> 3V3        GND -> GND
 *   SCL -> GPIO12     SDA -> GPIO11     (MISO/SDO -> GPIO13, optional)
 *   CS  -> GPIO10     DC  -> GPIO14     RST -> GPIO21
 *   BL  -> GPIO47     (tie to 3V3 instead if you don't need dimming)
 */

#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

#include "ui_home.h"

/* Landscape. Works correctly now that the panel is driven as an ST7789;
   under the wrong (ILI9341) driver rotations 1 and 3 were unusable. */
static const uint16_t SCREEN_W = 320;
static const uint16_t SCREEN_H = 240;

static TFT_eSPI tft = TFT_eSPI();

/* Ten lines of framebuffer is the usual sweet spot for a 320px wide panel. */
static const uint32_t DRAW_BUF_LINES = 40;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf1[SCREEN_W * DRAW_BUF_LINES];

static void disp_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *px)
{
    uint32_t w = area->x2 - area->x1 + 1;
    uint32_t h = area->y2 - area->y1 + 1;

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    /* LV_COLOR_16_SWAP is already 1 in lv_conf.h, so the bytes arrive in the
       order the panel wants — asking pushColors to swap again inverts them. */
    tft.pushColors((uint16_t *)px, w * h, false);
    tft.endWrite();

    lv_disp_flush_ready(drv);
}

static void display_init(void)
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

    tft.begin();
    tft.setRotation(1);              /* landscape; use 3 to flip 180 deg */
    tft.fillScreen(TFT_BLACK);

    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf1, NULL, SCREEN_W * DRAW_BUF_LINES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res  = SCREEN_W;
    disp_drv.ver_res  = SCREEN_H;
    disp_drv.flush_cb = disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_t *d = lv_disp_drv_register(&disp_drv);

    Serial.printf("[lvgl] display %d x %d\n",
                  lv_disp_get_hor_res(d), lv_disp_get_ver_res(d));
    Serial.flush();
}

void setup()
{
    Serial.begin(115200);
    delay(200);
    Serial.println("\n[boot] Smart Access Panel");

    display_init();
    ui_home_create();

    /* Seed the widgets with the current state. */
    ui_home_set_enrolls(1, 162);
    ui_home_set_net_state(UI_STATUS_ONLINE);

    Serial.println("[boot] UI ready");
}

void loop()
{
    lv_timer_handler();
    delay(5);
}
