#include <SD.h>
#include <TFT_eSPI.h>
//#include "buttons.h"
#include <OneButton.h>
#include <TJpg_Decoder.h>

#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif

TFT_eSPI tft = TFT_eSPI();

#define upButtonPin 16   // External up button
#define downButtonPin 33  // External down button
#define selectButtonPin 17 // External select button

// Setup a new OneButton on pin PIN_INPUT2.
OneButton up1(upButtonPin, true);
// Setup a new OneButton on pin PIN_INPUT2.
OneButton down1(downButtonPin, true);

OneButton select1(selectButtonPin, true);

String currentFolder = "/";
String audio;
String image;

String folders[10] = {};
String album[20] = {};
int currentIndex = 0;

bool isAlbums = false;
bool isTracks = false;
bool isSettings = false;
bool isPlaying = false;

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

void updateScreen() 
{
  tft.fillScreen(TFT_DARKGREY);

  File root = SD.open(currentFolder);
  File entry = root.openNextFile();
  int currentDisplayIndex = 0;


  // Iterate through the files and folders in the current directory
  while (entry) 
  {
    // Display the name of the file or folder on the screen
    folders[currentDisplayIndex] = entry.name();
    
    if (currentDisplayIndex == currentIndex) 
    {
      tft.setTextColor(TFT_ORANGE, TFT_DARKGREY);
    } 
    else 
    {
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    }

    tft.drawString(folders[currentDisplayIndex].c_str(), 10, 10 + currentDisplayIndex * 20);
    Serial.println(folders[currentDisplayIndex]);
    entry = root.openNextFile();
    currentDisplayIndex++;
  }

  /*// Wrap around if there are no items in the current folder
  if (currentDisplayIndex == 0) {
    currentIndex = 0;
  }
  */

  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  delay(500);
}

void up()
{
  if(currentIndex > 0)
  {
    currentIndex -= 1;
    updateScreen();
    Serial.print("Current index is: "); Serial.print(currentIndex); Serial.println(". Go up");
  }
}


void down()
{
  if(currentIndex < 10)
  {
    currentIndex += 1;
    updateScreen();
    Serial.print("Current index is: "); Serial.print(currentIndex); Serial.println(". Go down");
  }
  
}

void select()
{
  Serial.print(folders[currentIndex].c_str()); Serial.println(" has been selected");
  openAlbum();
}

void selectBack()
{
  Serial.println("Back to Albums");
  updateScreen();
}

void interface()
{
  
  if (digitalRead(upButtonPin) == LOW) 
  {
    if(currentIndex > 0)
    {
      currentIndex -= 1;
      updateScreen();
      Serial.print("Current index is: "); Serial.println(currentIndex);
    }
    delay(20); // Debounce
  }

  if (digitalRead(downButtonPin) == LOW) 
  {
    if(currentIndex < 10)
    {
      currentIndex += 1;
      updateScreen();
      Serial.print("Current index is: "); Serial.println(currentIndex);
    }
    delay(20); // Debounce
  }

  if (digitalRead(selectButtonPin) == LOW) 
  {
    //selectItem();
    delay(200); // Debounce
    //updateScreen()
    Serial.print(folders[currentIndex].c_str()); Serial.println(" has been selected");
  }
  
}

void fileType(String audio)
{
  String audioName = audio;
  String jpg = "jpg";
  String mp3 = "mp3";
  String wav = "wav";

  // Check if mainString contains subString
  if (audioName.indexOf(jpg) != -1) 
  {
    Serial.println("File is Jpeg");
  } 
  else if (audioName.indexOf(mp3) != -1)
  {
    Serial.println("File is mp3");
  }
  else if (audioName.indexOf(wav) != -1)
  {
    Serial.println("File is wav");
  }
  else
  {
    Serial.println("File is not of mentioned type");
  }
}

void openAlbum()
{
  tft.fillScreen(TFT_DARKGREY);
  
  String albumDir = "/";
  albumDir.concat(folders[currentIndex].c_str());

  File root = SD.open(albumDir);
  File entry = root.openNextFile();
  currentIndex = 0;
  int currentDisplayIndex = 0;


  // Iterate through the files and folders in the current directory
  while (entry) 
  {
    // Display the name of the file or folder on the screen
    album[currentDisplayIndex] = entry.name();
    
    if (currentDisplayIndex == currentIndex) 
    {
      tft.setTextColor(TFT_ORANGE, TFT_DARKGREY);
    } 
    else 
    {
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    }

    tft.drawString(album[currentDisplayIndex].c_str(), 10, 10 + currentDisplayIndex * 20);
    Serial.println(album[currentDisplayIndex]);
    entry = root.openNextFile();
    currentDisplayIndex++;
  }
}

void display()
{
  uint32_t t = millis();

  // Get the width and height in pixels of the jpeg if you wish
  uint16_t w = 0, h = 0;
  TJpgDec.getSdJpgSize(&w, &h, "/tpab2.jpg");
  Serial.print("Width = "); Serial.print(w); Serial.print(", height = "); Serial.println(h);

  // Draw the image, top left at 0,0
  
  TJpgDec.drawSdJpg(30,20, "/tpab2.jpg");
  //tft.loadFont(AA_FONT_LARGE);
  tft.drawString("To Pimp A Butter Fly", 25, 217, 10);

  // How much time did rendering take
  t = millis() - t;
  Serial.print(t); Serial.println(" ms");

}

void setup() 
{
  Serial.begin(115200);
  Serial.println("\n\nSD Card File Explorer");

  if (!SD.begin(5)) {  // SD_CS pin is 5
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  Serial.println("\r\nSD Card Initialized.");

  tft.begin();
  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
  tft.fillScreen(TFT_DARKGREY);
  tft.setSwapBytes(true);
  
  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);
  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);

  //pinMode(upButtonPin, INPUT_PULLUP);
  //pinMode(downButtonPin, INPUT_PULLUP);
  //pinMode(selectButtonPin, INPUT_PULLUP);
 
  //buttons.buttonsInit(currentIndex, folders[currentIndex]);
  up1.attachClick(up);
  down1.attachClick(down);
  select1.attachClick(select);
  select1.attachDoubleClick(selectBack);
}


void loop() 
{
  
  up1.tick();
  down1.tick();
  select1.tick();
  //delay(10);
  

}