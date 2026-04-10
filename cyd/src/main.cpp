#include <Arduino.h>
#include <CST820.h>
#include <SPI.h>
#include <Setup14_ILI9341_CYD.h>
#include <TFT_Touch.h>
#include <TFT_eSPI.h>
#include <Wire.h>
#include <XPT2046_Touchscreen.h>
#include <lvgl.h>

#define LVGL_TICK_PERIOD 5

#define NEO_PIXEL_PIN 4
#define NEO_PIXEL_COUNT 1

#define TOUCH_CS 33
#define TOUCH_SCLK 25
#define TOUCH_MOSI 39
#define TOUCH_MISO 32
#define TOUCH_IRQ 36

#define LED_RED_PIN 4
#define LED_GREEN_PIN 16
#define LED_BLUE_PIN 17

#define SD_CS 5
#define SD_MOSI 23
#define SD_MISO 19
#define SD_SCLK 18

#define BUTTON_PIN 0

#define VBAT_PIN 34

#define PERIPHERAL_PIN 35

#define UART_RX_PIN 3
#define UART_TX_PIN 1

#define DAC_PIN 26
#define DAC_EN 4

#define I2C_SDA 33
#define I2C_SCL 32
#define TP_RST 25
#define TP_INT 21

TFT_eSPI tft = TFT_eSPI();
CST820 touch(I2C_SDA, I2C_SCL, TP_RST, TP_INT);

static lv_color_t buf1[TFT_WIDTH * TFT_HEIGHT / 10 * 1];

lv_indev_t* indev_touch;

static uint32_t my_tick(void) { return millis(); }

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

uint16_t x, y;

void my_touchpad_read(lv_indev_t* indev, lv_indev_data_t* data) {
  static uint16_t last_x = 0;
  static uint16_t last_y = 0;

  uint8_t gesture;
  if (touch.getTouch(&x, &y, &gesture)) {
    uint16_t rotated_x = TFT_WIDTH - x;
    uint16_t rotated_y = TFT_HEIGHT - y;

    last_x = rotated_x;
    last_y = rotated_y;

    data->point.x = rotated_x;
    data->point.y = rotated_y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->point.x = last_x;
    data->point.y = last_y;
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(TFT_ROTATION);

  touch.begin();

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(TFT_BL, OUTPUT);
  analogWrite(TFT_BL, 50 * 2.55);

  lv_init();
  lv_tick_set_cb(my_tick);

  lv_display_t* disp = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
  lv_display_set_flush_cb(disp, my_disp_flush);
  lv_display_set_buffers(disp, buf1, NULL, sizeof(buf1),
                         LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_disp_set_rotation(disp, LV_DISP_ROTATION_90);

  indev_touch = lv_indev_create();
  lv_indev_set_type(indev_touch, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev_touch, my_touchpad_read);
}

void loop() {
  lv_task_handler();
  delay(LVGL_TICK_PERIOD);
}
