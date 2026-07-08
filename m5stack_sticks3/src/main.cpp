#include <Arduino.h>
#include <lvgl.h>

#include "M5Unified.h"

#define MAIN_BUTTON_PIN 11
#define SIDE_BUTTON_PIN 12

#define I2C_SDA_PIN 47
#define I2C_SCL_PIN 48

bool backlightOn = true;

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 135

lv_display_t* disp;
static lv_color_t buf1[DISPLAY_WIDTH * DISPLAY_HEIGHT / 5 * 2];
static int rotation = 0;

void my_disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  lv_color_t* color_p = (lv_color_t*)px_map;
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;
  M5.Display.pushImage(area->x1, area->y1, w, h, (uint16_t*)color_p);
  lv_display_flush_ready(disp);
}

void lvgl_setup() {
  lv_init();
  disp = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for serial to initialize
  pinMode(MAIN_BUTTON_PIN, INPUT_PULLUP);
  pinMode(SIDE_BUTTON_PIN, INPUT_PULLUP);

  M5.begin();
  M5.Display.setRotation(1);
  M5.Display.setBrightness(128);
  lvgl_setup();

  Serial.printf("LVGL Version: %d.%d.%d\n", lv_version_major(),
                lv_version_minor(), lv_version_patch());
}

void loop() {
  if (digitalRead(MAIN_BUTTON_PIN) == LOW) {
    Serial.println("Main button pressed");
    delay(200);  // Debounce delay
  }
  if (digitalRead(SIDE_BUTTON_PIN) == LOW) {
    Serial.println("Side button pressed");
    delay(200);  // Debounce delay
  }

  lv_timer_handler();
  delay(5);
}