#include <Arduino.h>
#include <M5Cardputer.h>
#include <lvgl.h>

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 135
lv_display_t* disp;
static lv_color_t buf1[DISPLAY_WIDTH * DISPLAY_HEIGHT / 5 * 2];
bool backlightOn = true;
String data = "> ";

void my_disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  lv_color_t* color_p = (lv_color_t*)px_map;
  uint32_t w = area->x2 - area->x1 + 1;
  uint32_t h = area->y2 - area->y1 + 1;
  M5Cardputer.Display.pushImage(area->x1, area->y1, w, h, (uint16_t*)color_p);
  lv_display_flush_ready(disp);
}

void lvgl_setup() {
  lv_init();
  disp = lv_display_create(DISPLAY_WIDTH, DISPLAY_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);

  static lv_indev_t* indev;
  indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_KEYPAD);

  lv_obj_t* label = lv_label_create(lv_scr_act());
  lv_label_set_text(label, "Hello world");
  lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}
void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for serial to initialize

  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);

  lvgl_setup();
}

void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange()) {
    Serial.println("Key state changed");
    if (M5Cardputer.Keyboard.isPressed()) {
      Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();

      for (auto i : status.word) {
        int value = i - '0';
        Serial.printf("value: %d\n", value);

        Serial.print(i);
        data += i;
      }

      if (status.del) {
        data.remove(data.length() - 1);
      }

      if (status.enter) {
        data.remove(0, 2);
        data = "> ";
      }
    }
  }

  lv_timer_handler();
  delay(5);
  lv_tick_inc(5);
}
