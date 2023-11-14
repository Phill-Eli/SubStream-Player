#include <TFT_eSPI.h>
//#include "SPI.h"
//#include <TJpg_Decoder.h>
#include "play64.h"
#include "pause64.h"

// microSD Card Reader connections
#define SD_CS          5
#define SPI_MOSI      23 
#define SPI_MISO      19
#define SPI_SCK       18

// Include SD
//#define FS_NO_GLOBALS
//#include <FS.h>
//#ifdef ESP32
//  #include "SPIFFS.h" // ESP32 only
//#endif

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite playSprite = TFT_eSprite(&tft);
TFT_eSprite pauseSprite = TFT_eSprite(&tft);
int run;

//#define SD_CS   5
//#define AA_FONT_LARGE "NotoSansBold36"

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

/*
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
*/

//TFT_eSprite background = TFT_eSprite(&tft);
//TFT_eSprite play = TFT_eSprite(&tft);
//TFT_eSprite cover = TFT_eSprite(&tft);

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(17, INPUT_PULLUP);
  
  tft.init();
  tft.fillScreen(TFT_DARKGREY);
  tft.setSwapBytes(true);
  
  //playSprite.createSprite(64,64);
  //pauseSprite.createSprite(64,64);
  /*
  playSprite.pushImage(0,0, 64,64, run); //x,y = 0,0 because the sprite is the same size as the image
  
  
  playSprite.setSwapBytes(true);
  
  playSprite.pushSprite(88,250, TFT_BLACK); //x = (240 - image width)/2, y = desired height
  */

}

void loop() 
{
  // put your main code here, to run repeatedly:
  
  int audioplay = digitalRead(17);
  if(audioplay == 0 & run == 1)
  {
    
    //playSprite.deleteSprite();
    playSprite.deleteSprite();
    run = 0;
    pauseSprite.createSprite(64,64);
    pauseSprite.pushImage(0,0, 64,64, pause64); //x,y = 0,0 because the sprite is the same size as the image
    pauseSprite.setSwapBytes(true);
    pauseSprite.pushSprite(88,250, TFT_BLACK); //x = (240 - image width)/2, y = desired height
    delay(500);
  }
  if(audioplay == 0 & run == 0)
  {
    pauseSprite.deleteSprite();
    //playSprite.createSprite(64,64);
    run = 1;
    playSprite.createSprite(64,64);
    playSprite.pushImage(0,0, 64,64, play64); //x,y = 0,0 because the sprite is the same size as the image
    playSprite.setSwapBytes(true);
    playSprite.pushSprite(88,250, TFT_BLACK); //x = (240 - image width)/2, y = desired height
    delay(500);
    playSprite.
  }


  // Wait before drawing again
  
}
