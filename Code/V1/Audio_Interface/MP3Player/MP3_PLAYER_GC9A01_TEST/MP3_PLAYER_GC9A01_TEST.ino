#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <Wire.h>               // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"
#include "SPI.h"
#include "TFT_eSPI.h"

TFT_eSPI tft = TFT_eSPI(); 
// microSD Card Reader connections
#define SD_CS          5
#define SPI_MOSI      23 
#define SPI_MISO      19
#define SPI_SCK       18
 
// I2S Connections
#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26


// I/O connections
#define Vol_Knob
#define Skip_Bttn
#define Back_Bttn
#define play_bttn

 // Create Audio object
Audio audio;
 
void setup() {
    
    // Set microSD Card CS as OUTPUT and set HIGH
    pinMode(SD_CS, OUTPUT);      
    digitalWrite(SD_CS, HIGH); 
    
    // Initialize SPI bus for microSD Card
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    
    // Start Serial Port
    Serial.begin(115200);
    
    //Start I2C channel
    Wire.begin();
    // Start microSD Card
    if(!SD.begin(SD_CS))
    {
      Serial.println("Error accessing microSD card!");
      while(true); 
    }
    
    // Setup I2S 
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    
    // Set Volume
    audio.setVolume(75);
    
    // Open music file
    audio.connecttoFS(SD,"/wavfile.wav");

    tft.init();

    //tn = micros();
    tft.fillScreen(TFT_BLACK);

    // Create a viewport 220 x 300 pixels
    tft.setViewport(10,10,240,300);
    
}


void loop()
{
  audio.loop();
  
  //unsigned long start = micros();
  tft.fillScreen(TFT_BLACK);
  tft.fillScreen(TFT_RED);
  tft.fillScreen(TFT_GREEN);
  tft.fillScreen(TFT_BLUE);
  tft.fillScreen(TFT_BLACK);
  //return micros() - start;
  //delay(1000);
      
}