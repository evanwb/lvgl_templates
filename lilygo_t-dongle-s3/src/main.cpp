#include <Arduino.h>
#include <TFT_eSPI.h>
#include <lvgl.h>

#define LED_CI 39
#define LED_DI 40

#define TFT_ROTATION 3

TFT_eSPI tft = TFT_eSPI();

static lv_color_t buf1[TFT_WIDTH * TFT_HEIGHT / 5 * 2];
lv_display_t* disp;

void my_disp_flush(lv_display_t* _disp, const lv_area_t* area,
                   uint8_t* color_p) {
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1,
                    area->y2 - area->y1 + 1);
  tft.pushColors((uint16_t*)color_p,
                 (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1), false);
  tft.endWrite();
  lv_disp_flush_ready(_disp);
}

static uint32_t my_tick(void) { return millis(); }

void setup() {
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 50 * 2.55);

  tft.init();
  tft.setRotation(TFT_ROTATION);

  lv_init();
  lv_tick_set_cb(my_tick);

  disp = lv_display_create(TFT_HEIGHT, TFT_WIDTH);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void loop() {
  lv_task_handler();
  delay(5);
}