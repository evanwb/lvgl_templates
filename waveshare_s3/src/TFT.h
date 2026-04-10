#define USER_SETUP_ID 18

#define ST7789_DRIVER

#define TFT_WIDTH 320
#define TFT_HEIGHT 172
#define TFT_BL 46    // Backlight pin
#define TFT_DC 45    // Data/Command pin
#define TFT_CS 21    // Chip Select pin
#define TFT_RST 40   // Reset pin
#define TFT_SCK 38   // SPI Clock pin
#define TFT_MOSI 39  // SPI MOSI pin

#define TFT_ROTATION 0
#define TFT_IPS false
#define TFT_COL_OFFSET1 34
#define TFT_ROW_OFFSET1 0
#define TFT_COL_OFFSET2 34
#define TFT_ROW_OFFSET2 0

// #define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820
// bytes in FLASH #define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs
// ~3534 bytes in FLASH, 96 characters #define LOAD_FONT4  // Font 4. Medium 26
// pixel high font, needs ~5848 bytes in FLASH, 96 characters #define LOAD_FONT6
// // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters
// 1234567890:-.apm #define LOAD_FONT7  // Font 7. 7 segment 48 pixel font,
// needs ~2438 bytes in FLASH, only characters 1234567890:-. #define LOAD_FONT8
// // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters
// 1234567890:-. #define LOAD_GFXFF  // FreeFonts. Include access to the 48
// Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// #define SMOOTH_FONT

// #define SPI_FREQUENCY  20000000
#define SPI_FREQUENCY 27000000
// #define SPI_FREQUENCY  40000000
// #define SPI_FREQUENCY  80000000
