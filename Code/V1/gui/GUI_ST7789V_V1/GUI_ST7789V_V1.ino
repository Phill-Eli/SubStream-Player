#include <TFT_eSPI.h>
#include "SPI.h"
#include <TJpg_Decoder.h>
#include "play.h"


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

TFT_eSPI tft = TFT_eSPI();

//#define SD_CS   5
#define AA_FONT_LARGE "NotoSansBold36"

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

void sd_init()
{
  
  Serial.println("\n\n Testing TJpg_Decoder library");
  
  //SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
  
  // Initialise SD before TFT
  if (!SD.begin(SD_CS, SPI)) {
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  Serial.println("\r\nInitialisation done.");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  sd_init();
  
  tft.initDMA();
  tft.init();
  tft.setSwapBytes(true);
  tft.fillScreen(TFT_DARKGREY);

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  tft.pushImage(84,250,72,72, play);
  //x = (240 - image width)/2

}

void loop() {
  // put your main code here, to run repeatedly:
 
  String im[2] = {"/tpab2.jpg", "/g52.jpg"};
  uint32_t t = millis();

/*
  for (int i = 0; i < 2; i++)
  {
    // Get the width and height in pixels of the jpeg if you wish
    uint16_t w = 0, h = 0;
    TJpgDec.getSdJpgSize(&w, &h, im[i]);
    Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

    // Draw the image, top left at 0,0
    
    TJpgDec.drawSdJpg(30,20, im[i]);
    //tft.loadFont(AA_FONT_LARGE);
    tft.drawString("To Pimp A Butter Fly", 25, 217);

    // How much time did rendering take
    t = millis() - t;
    Serial.print(t); Serial.println(" ms");

    // Wait before drawing again
    delay(5000);
  }
  */
  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getSdJpgSize(&w, &h, "/tpab2.jpg");
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  // Draw the image, top left at 0,0
  
  TJpgDec.drawSdJpg(30,20, "/tpab2.jpg");
  //x = (240 - image width)/2
  
  //tft.loadFont(AA_FONT_LARGE);
  tft.drawString("To Pimp A Butter Fly", 25, 217);

  // How much time did rendering take
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");
  

  // Wait before drawing again
  delay(5000);
  //*/
}
