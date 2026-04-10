#include <Arduino_GFX_Library.h>
#include <RotaryEncoder.h>
#include <demos/lv_demos.h>

#include "TFT.h"
#include "esp_lcd_touch_axs5106l.h"
#include "ui.h"

#define ROTATION 3

#define Touch_I2C_SDA 42
#define Touch_I2C_SCL 41
#define Touch_RST 47
#define Touch_INT 48
#define GFX_BL 46

#define ANIMATION_DURATION 75

#define ENCODER_BUTTON_PIN 41
#define ENCODER_A_PIN 2
#define ENCODER_B_PIN 1
#define ENCODER_STEP 5

#define BUTTON_PIN 0

Arduino_DataBus* bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI);

Arduino_GFX* gfx = new Arduino_ST7789(
    bus, TFT_RST, TFT_ROTATION, TFT_IPS, TFT_HEIGHT, TFT_WIDTH, TFT_COL_OFFSET1,
    TFT_ROW_OFFSET1, TFT_COL_OFFSET2, TFT_ROW_OFFSET2);

void lcd_reg_init(void) {
  static const uint8_t init_operations[] = {
      BEGIN_WRITE,
      WRITE_COMMAND_8,
      0x11,  // 2: Out of sleep mode, no args, w/delay
      END_WRITE,
      DELAY,
      120,

      BEGIN_WRITE,
      WRITE_C8_D16,
      0xDF,
      0x98,
      0x53,
      WRITE_C8_D8,
      0xB2,
      0x23,

      WRITE_COMMAND_8,
      0xB7,
      WRITE_BYTES,
      4,
      0x00,
      0x47,
      0x00,
      0x6F,

      WRITE_COMMAND_8,
      0xBB,
      WRITE_BYTES,
      6,
      0x1C,
      0x1A,
      0x55,
      0x73,
      0x63,
      0xF0,

      WRITE_C8_D16,
      0xC0,
      0x44,
      0xA4,
      WRITE_C8_D8,
      0xC1,
      0x16,

      WRITE_COMMAND_8,
      0xC3,
      WRITE_BYTES,
      8,
      0x7D,
      0x07,
      0x14,
      0x06,
      0xCF,
      0x71,
      0x72,
      0x77,

      WRITE_COMMAND_8,
      0xC4,
      WRITE_BYTES,
      12,
      0x00,
      0x00,
      0xA0,
      0x79,
      0x0B,
      0x0A,
      0x16,
      0x79,
      0x0B,
      0x0A,
      0x16,
      0x82,

      WRITE_COMMAND_8,
      0xC8,
      WRITE_BYTES,
      32,
      0x3F,
      0x32,
      0x29,
      0x29,
      0x27,
      0x2B,
      0x27,
      0x28,
      0x28,
      0x26,
      0x25,
      0x17,
      0x12,
      0x0D,
      0x04,
      0x00,
      0x3F,
      0x32,
      0x29,
      0x29,
      0x27,
      0x2B,
      0x27,
      0x28,
      0x28,
      0x26,
      0x25,
      0x17,
      0x12,
      0x0D,
      0x04,
      0x00,

      WRITE_COMMAND_8,
      0xD0,
      WRITE_BYTES,
      5,
      0x04,
      0x06,
      0x6B,
      0x0F,
      0x00,

      WRITE_C8_D16,
      0xD7,
      0x00,
      0x30,
      WRITE_C8_D8,
      0xE6,
      0x14,
      WRITE_C8_D8,
      0xDE,
      0x01,

      WRITE_COMMAND_8,
      0xB7,
      WRITE_BYTES,
      5,
      0x03,
      0x13,
      0xEF,
      0x35,
      0x35,

      WRITE_COMMAND_8,
      0xC1,
      WRITE_BYTES,
      3,
      0x14,
      0x15,
      0xC0,

      WRITE_C8_D16,
      0xC2,
      0x06,
      0x3A,
      WRITE_C8_D16,
      0xC4,
      0x72,
      0x12,
      WRITE_C8_D8,
      0xBE,
      0x00,
      WRITE_C8_D8,
      0xDE,
      0x02,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x00,
      0x02,
      0x00,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x01,
      0x02,
      0x00,

      WRITE_C8_D8,
      0xDE,
      0x00,
      WRITE_C8_D8,
      0x35,
      0x00,
      WRITE_C8_D8,
      0x3A,
      0x05,

      WRITE_COMMAND_8,
      0x2A,
      WRITE_BYTES,
      4,
      0x00,
      0x22,
      0x00,
      0xCD,

      WRITE_COMMAND_8,
      0x2B,
      WRITE_BYTES,
      4,
      0x00,
      0x00,
      0x01,
      0x3F,

      WRITE_C8_D8,
      0xDE,
      0x02,

      WRITE_COMMAND_8,
      0xE5,
      WRITE_BYTES,
      3,
      0x00,
      0x02,
      0x00,

      WRITE_C8_D8,
      0xDE,
      0x00,
      WRITE_C8_D8,
      0x36,
      0x00,
      WRITE_COMMAND_8,
      0x21,
      END_WRITE,

      DELAY,
      10,

      BEGIN_WRITE,
      WRITE_COMMAND_8,
      0x29,  // 5: Main screen turn on, no args, w/delay
      END_WRITE};
  bus->batchOperation(init_operations, sizeof(init_operations));
}

uint32_t screenWidth;
uint32_t screenHeight;
uint32_t bufSize;
lv_color_t* disp_draw_buf;

static lv_display_t* disp;

void my_disp_flush(lv_display_t* disp, const lv_area_t* area, uint8_t* px_map) {
  // Cast the pixel map to lv_color_t*
  lv_color_t* color_p = (lv_color_t*)px_map;

#ifndef DIRECT_RENDER_MODE
  uint32_t w = (area->x2 - area->x1 + 1);
  uint32_t h = (area->y2 - area->y1 + 1);

#if (LV_COLOR_16_SWAP != 0)
  gfx->draw16bitBeRGBBitmap(area->x1, area->y1, (uint16_t*)color_p, w, h);
#else
  // LVGL V9: The color data is now directly pointed to by `color_p`.
  gfx->draw16bitRGBBitmap(area->x1, area->y1, (uint16_t*)color_p, w, h);
#endif
#endif  // #ifndef DIRECT_RENDER_MODE

  // LVGL V9: `lv_disp_flush_ready` now takes a `lv_display_t*`.
  lv_display_flush_ready(disp);
}

void touchpad_read_cb(lv_indev_t* indev, lv_indev_data_t* data) {
  touch_data_t touch_data;
  uint8_t touchpad_cnt = 0;

  /* Read touch controller data */
  bsp_touch_read();
  /* Get coordinates */
  bool touchpad_pressed = bsp_touch_get_coordinates(&touch_data);

  if (touchpad_pressed) {
    data->point.x = touch_data.coords[0].x;
    data->point.y = touch_data.coords[0].y;
    data->state = LV_INDEV_STATE_PRESSED;
  } else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

void setup() {
  Serial.begin(115200);

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
  }
  lcd_reg_init();
  gfx->setRotation(ROTATION);
  gfx->fillScreen(RGB565_BLACK);
  analogWrite(GFX_BL, 255);

  Wire.begin(Touch_I2C_SDA, Touch_I2C_SCL);
  bsp_touch_init(&Wire, Touch_RST, Touch_INT, gfx->getRotation(), gfx->width(),
                 gfx->height());
  lv_init();

  screenWidth = gfx->width();
  screenHeight = gfx->height();

  bufSize = screenWidth * 40;

  // The original code allocated `bufSize * 2` bytes, which is enough for
  // `bufSize` pixels. This is correct for a single buffer.
  disp_draw_buf = (lv_color_t*)heap_caps_malloc(
      bufSize * sizeof(lv_color_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  if (!disp_draw_buf) {
    disp_draw_buf = (lv_color_t*)heap_caps_malloc(bufSize * sizeof(lv_color_t),
                                                  MALLOC_CAP_8BIT);
  }

  if (!disp_draw_buf) {
    Serial.println("LVGL disp_draw_buf allocate failed!");
  } else {
    /* LVGL V9: Display driver initialization is simplified */
    disp = lv_display_create(screenWidth, screenHeight);
    lv_display_set_flush_cb(disp, my_disp_flush);

    /* LVGL V9: Set the render mode and buffer */

    lv_display_set_buffers(disp, disp_draw_buf, NULL,
                           bufSize * sizeof(lv_color_t),
                           LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* LVGL V9: Input device driver initialization is simplified */
    static lv_indev_t* indev;
    indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchpad_read_cb);

    lv_obj_t* label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello world");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
  }
}

void loop() {
  lv_timer_handler(); /* let the GUI do its work */
  delay(5);
  lv_tick_inc(5);
}
