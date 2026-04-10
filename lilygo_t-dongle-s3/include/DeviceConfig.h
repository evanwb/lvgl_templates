// // display_config.h

// #ifndef DEVICE_CONFIG_H
// #define DEVICE_CONFIG_H

// #ifdef ESP32S3GEEK
// #define USER_SETUP_LOADED
// #define TFT_SPI_FREQUENCY 20000000
// #define ST7789_DRIVER
// #define TFT_WIDTH 135
// #define TFT_HEIGHT 240
// #define TFT_BL 7
// #define TFT_RST 9
// #define TFT_DC 8
// #define TFT_MOSI 11
// #define TFT_SCLK 12
// #define TFT_CS 10
// #define TOUCH_CS -1
// #define LOAD_GLCD 1

// #define SD_CS 34
// #define SD_SCK 36
// #define SD_MISO 37
// #define SD_MOSI 35

// #elif defined(CARDPUTER)
// #define USER_SETUP_LOADED
// #define TFT_SPI_FREQUENCY 20000000
// #define ST7789_DRIVER
// #define TFT_WIDTH 135
// #define TFT_HEIGHT 240
// #define TFT_BL 38
// #define TFT_RST 33
// #define TFT_DC 34
// #define TFT_MOSI 35
// #define TFT_SCLK 36
// #define TFT_CS 37
// #define TOUCH_CS -1
// #define LOAD_GLCD 1

// #define SD_CS 12
// #define SD_SCK 40
// #define SD_MISO 39
// #define SD_MOSI 14

// #else
// #error \
//     "No device configuration defined! Please define ESP32S3GEEK or CARDPUTER
//     in build_flags."
// #endif

// #endif  // DEVICE_CONFIG_H