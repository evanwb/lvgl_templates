#include <Arduino.h>
#include <BleKeyboard.h>
#include <RotaryEncoder.h>
#include <Setup200_GC9A01.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <lvgl.h>

#include "ui.h"

#define LVGL_TICK_PERIOD 5

#define ENCODER_A_PIN 41
#define ENCODER_B_PIN 40
#define ENCODER_STEP 5

#define TP_SDA_PIN 11
#define TP_SCL_PIN 12

BleKeyboard bleKeyboard("Dial", "Espressif", 100);
TFT_eSPI tft = TFT_eSPI();  // Uses settings from User_Setup.h
RotaryEncoder encoder(ENCODER_A_PIN, ENCODER_B_PIN,
                      RotaryEncoder::LatchMode::TWO03);

static lv_color_t buf1[TFT_WIDTH * TFT_HEIGHT / 5 * 2];

static lv_indev_t* indev_touch;
static lv_indev_t* encoder_indev;
static int pos = 0;
static bool brightnessControl = true;
#define FT3267_ADDR 0x38

static uint32_t my_tick(void) { return millis(); }

bool ft3267_read_touch(uint16_t& x, uint16_t& y, bool& pressed) {
  Wire.beginTransmission(FT3267_ADDR);
  Wire.write(0x02);  // Start at TD_STATUS
  if (Wire.endTransmission(false) != 0) {
    pressed = false;
    return false;  // I2C error
  }

  if (Wire.requestFrom(FT3267_ADDR, 5) < 5) {
    pressed = false;
    return false;  // Not enough data
  }

  uint8_t td_status = Wire.read();  // Number of touch points (0, 1, 2)
  uint8_t xh = Wire.read();         // Touch1 XH
  uint8_t xl = Wire.read();         // Touch1 XL
  uint8_t yh = Wire.read();         // Touch1 YH
  uint8_t yl = Wire.read();         // Touch1 YL

  if (td_status == 0) {
    pressed = false;
    return true;
  }

  x = ((xh & 0x0F) << 8) | xl;
  y = ((yh & 0x0F) << 8) | yl;
  pressed = true;

  Serial.printf("Raw FT3267: td=%d xh=0x%02X xl=0x%02X yh=0x%02X yl=0x%02X\n",
                td_status, xh, xl, yh, yl);
  Serial.printf("Decoded: x=%d y=%d pressed=%d\n", x, y, pressed);

  return true;
}

void my_touchpad_read(lv_indev_t* indev, lv_indev_data_t* data) {
  static uint16_t last_x = 0;
  static uint16_t last_y = 0;
  uint16_t x, y;
  bool pressed;

  if (ft3267_read_touch(x, y, pressed) && pressed) {
    last_x = x;
    last_y = y;
    data->point.x = x;
    data->point.y = y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = LV_INDEV_STATE_RELEASED;
  }

  // if (pressed) {
  //   lv_obj_t *dot = lv_obj_create(lv_scr_act());
  //   lv_obj_set_size(dot, 10, 10);
  //   lv_obj_set_style_bg_color(dot, lv_color_hex(0x00FF00), 0);
  //   lv_obj_align(dot, LV_ALIGN_CENTER, x, y);
  // }
}

static void my_disp_flush(lv_display_t* disp, const lv_area_t* area,
                          uint8_t* color_p) {
  tft.startWrite();
  tft.setAddrWindow(area->x1, area->y1, area->x2 - area->x1 + 1,
                    area->y2 - area->y1 + 1);
  tft.pushColors((uint16_t*)&color_p[0],
                 (area->x2 - area->x1 + 1) * (area->y2 - area->y1 + 1), false);
  tft.endWrite();
  lv_disp_flush_ready(disp);
}

void my_button_event(lv_event_t* e) {
  lv_event_code_t code = lv_event_get_code(e);
  if (code == LV_EVENT_CLICKED) {
    Serial.println("Button was clicked!");
    brightnessControl = !brightnessControl;
    // You can also update the screen, toggle states, etc.
  } else if (code == LV_EVENT_LONG_PRESSED) {
    Serial.println("Button was long pressed!");
  } else if (code == LV_EVENT_DOUBLE_CLICKED) {
    Serial.println("Button was double clicked!");
  } else if (code == LV_EVENT_RELEASED) {
    Serial.println("Button was released!");
  } else if (code == LV_EVENT_PRESSED) {
    Serial.println("Button is pressed!");
  } else if (code == LV_EVENT_PRESSING) {
    Serial.println("Button is being pressed!");
  } else if (code == LV_EVENT_PRESS_LOST) {
    Serial.println("Button press lost!");
  } else {
    // Serial.printf("Unhandled event code: %d\n", code);
  }
}

void my_encoder_read(lv_indev_t* indev, lv_indev_data_t* data) {
  int newPos = encoder.getPosition();
  if (pos != newPos) {
    int dir = (int)(encoder.getDirection());

    // Media control
    if (dir == 1) {
      Serial.printf("Volume Up %d\n", bleKeyboard.write(KEY_MEDIA_VOLUME_UP));

    } else {
      Serial.printf("Volume down %d\n", bleKeyboard.write('d'));
    }

    // // Arc control
    // int arcVal = lv_arc_get_value(ui_Arc1);
    // arcVal += (dir == 1) ? ENCODER_STEP : -1 * ENCODER_STEP;

    // // Clamp arc value (adjust min/max as needed)
    // arcVal = LV_CLAMP(0, arcVal, 100);
    // if (brightnessControl)
    //   analogWrite(TFT_BL,
    //               arcVal * 2.55);  // Adjust backlight based on arc value
    // lv_arc_set_value(ui_Arc1, arcVal);
    // lv_label_set_text_fmt(ui_Label1, "%d%%", arcVal);

    // Debug output
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println(dir);
    if (dir == 1) {
      Serial.println("right");
      Serial.printf("Volume Up %d\n", bleKeyboard.write(KEY_MEDIA_VOLUME_UP));
      bleKeyboard.press(KEY_MEDIA_VOLUME_UP);

    } else if (dir == -1) {
      Serial.println("left");
      Serial.printf("Volume down %d\n",
                    bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN));
      bleKeyboard.press(KEY_MEDIA_VOLUME_DOWN);
    }

    pos = newPos;
    data->enc_diff = dir;                  // Set the encoder direction
    data->state = LV_INDEV_STATE_PRESSED;  // Set state to pressed
  } else {
    data->enc_diff = 0;                     // No change
    data->state = LV_INDEV_STATE_RELEASED;  // Set state to released
  }
}

void setup() {
  Serial.begin(115200);

  bleKeyboard.setBatteryLevel(100);
  Wire.begin(TP_SDA_PIN, TP_SCL_PIN);
  tft.init();
  tft.setRotation(0);  // Try 0-3 or 0-7 depending on your screen
  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 50 * 2.55);  // Turn on backlight

  lv_init();
  lv_tick_set_cb(my_tick);

  lv_display_t* disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  // lv_obj_add_event_cb(ui_Arc1, my_arc_event, LV_EVENT_VALUE_CHANGED, NULL);

  indev_touch =
      lv_indev_create(); /* Create input device connected to Default Display. */
  lv_indev_set_type(
      indev_touch,
      LV_INDEV_TYPE_POINTER); /* Touch pad is a pointer-like device. */
  lv_indev_set_read_cb(indev_touch,
                       my_touchpad_read); /* Set driver function. */

  encoder_indev = lv_indev_create();
  lv_indev_set_type(encoder_indev, LV_INDEV_TYPE_ENCODER);
  lv_indev_set_read_cb(encoder_indev, my_encoder_read);

  lv_obj_set_style_bg_color(lv_scr_act(), lv_color_hex(0x0000FF), 0);
  ui_init();

  // lv_obj_t *button = lv_btn_create(lv_scr_act());
  // lv_obj_set_size(button, 100, 50);
  // lv_obj_align(button, LV_ALIGN_CENTER, 0, 0);
  // lv_obj_add_event_cb(button, on_button_event, LV_EVENT_ALL, NULL);
  // lv_obj_t *label = lv_label_create(button);
  // lv_label_set_text(label, "Click me");
  // lv_obj_center(label);
  bleKeyboard.begin();
  delay(1000);  // Wait for BLE to initialize
}

unsigned long last_touch_check = 0;

void loop() {
  encoder.tick();

  if (millis() - last_touch_check > 1000) {
    bleKeyboard.press(KEY_MEDIA_PLAY_PAUSE);
    last_touch_check = millis();
  }

  // static int lastPos = encoder.getPosition();
  // int newPos = encoder.getPosition();
  // if (newPos != lastPos) {
  //   int dir = (int)encoder.getDirection();
  //   if (dir == 1) {
  //     Serial.println("right");
  //     bleKeyboard.write(KEY_MEDIA_VOLUME_UP);
  //   } else if (dir == -1) {
  //     Serial.println("left");
  //     bleKeyboard.write(KEY_MEDIA_VOLUME_DOWN);
  //   }
  //   lastPos = newPos;
  // }

  lv_task_handler();
  delay(LVGL_TICK_PERIOD);
}
