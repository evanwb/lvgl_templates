
#include <M5GFX.h>
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <lvgl.h>

#include "lv_conf.h"
#include "pins_config.h"

const char* ssid = "XXX";
const char* password = "XXX";

M5GFX display;

static lv_disp_drv_t disp_drv;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t* buf1;
static lv_disp_t* disp;   // store registered display
static int rotation = 0;  // 0=normal, 1=90deg, 2=180deg, 3=270deg

void my_disp_flush(lv_disp_drv_t* drv, const lv_area_t* area,
                   lv_color_t* color_p) {
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  display.pushImageDMA(area->x1, area->y1, w, h, (uint16_t*)color_p);
  lv_disp_flush_ready(drv);
}

void my_rounder(lv_disp_drv_t* drv, lv_area_t* area) {
  if (area->x1 % 2 != 0) area->x1 += 1;
  if (area->y1 % 2 != 0) area->y1 += 1;

  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);
  if (w % 2 != 0) area->x2 -= 1;
  if (h % 2 != 0) area->y2 -= 1;
}

void touchpad_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data) {
  lgfx::touch_point_t tp[3];
  uint8_t touchpad = display.getTouchRaw(tp, 3);
  if (touchpad > 0) {
    data->state = LV_INDEV_STATE_PR;
    data->point.x = tp[0].x;
    data->point.y = tp[0].y;
  } else {
    data->state = LV_INDEV_STATE_REL;
  }
}

void setup(void) {
  Serial.begin(115200);

  display.init();

  lv_init();

  buf1 = (lv_color_t*)heap_caps_malloc(sizeof(lv_color_t) * LVGL_LCD_BUF_SIZE,
                                       MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

  lv_disp_draw_buf_init(&draw_buf, buf1, NULL, LVGL_LCD_BUF_SIZE);

  lv_disp_drv_init(&disp_drv);
  disp_drv.hor_res = EXAMPLE_LCD_H_RES;
  disp_drv.ver_res = EXAMPLE_LCD_V_RES;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.rounder_cb = my_rounder;
  disp_drv.sw_rotate = 1;
  disp_drv.draw_buf = &draw_buf;
  disp = lv_disp_drv_register(&disp_drv);

  // Touch input
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init(&indev_drv);
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = touchpad_read_cb;
  lv_indev_drv_register(&indev_drv);

  lv_disp_set_rotation(disp, LV_DISP_ROT_90);

  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

  // WiFi.setPins(SDIO2_CLK, SDIO2_CMD, SDIO2_D0, SDIO2_D1, SDIO2_D2, SDIO2_D3,
  //              SDIO2_RST);
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
}

void loop(void) {
  lv_timer_handler();
  delay(5);
}
