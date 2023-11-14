// Example for library:
// https://github.com/Bodmer/TJpg_Decoder

// This example if for an ESP8266 or ESP32, it renders a Jpeg file
// that is stored in a SD card file. The test image is in the sketch
// "data" folder (press Ctrl+K to see it). You must save the image
// to the SD card using you PC.

// Include the jpeg decoder library
#include <TJpg_Decoder.h>

// microSD Card Reader connections
#define SD_CS          5
#define SPI_MOSI      23 
#define SPI_MISO      19
#define SPI_SCK       18

// Include SD
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

//#define SD_CS   5
#define AA_FONT_LARGE "NotoSansBold36"

// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

#define USE_DMA_TO_TFT

// This next function will be called during decoding of the jpeg file to
// render each block to the TFT.  If you use a different TFT library
// you will need to adapt this function to suit.
bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}


void setup()
{
  Serial.begin(115200);
  Serial.println("\n\n Testing TJpg_Decoder library");
  
  //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  
  // Initialise SD before TFT
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  Serial.println("\r\nInitialisation done.");

   #ifdef USE_DMA_TO_TFT
  // DMA - should work with ESP32, STM32F2xx/F4xx/F7xx processors
  // NOTE: >>>>>> DMA IS FOR SPI DISPLAYS ONLY <<<<<<
  tft.initDMA(); // Initialise the DMA engine (tested with STM32F446 and STM32F767)
  #endif


  // Initialise the TFT
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_DARKGREY);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

void loop()
{
  //tft.fillScreen(TFT_RED);

  // Time recorded for test purposes
  uint32_t t = millis();

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getSdJpgSize(&w, &h, "/tpab2.jpg");
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  // Draw the image, top left at 0,0
  
  TJpgDec.drawSdJpg(30,20, "/tpab2.jpg");
  tft.loadFont(AA_FONT_LARGE);
  tft.drawString("To Pimp A Butter Fly", 25, 217, 10);

  // How much time did rendering take
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");

  // Wait before drawing again
  delay(5000);
  
}
