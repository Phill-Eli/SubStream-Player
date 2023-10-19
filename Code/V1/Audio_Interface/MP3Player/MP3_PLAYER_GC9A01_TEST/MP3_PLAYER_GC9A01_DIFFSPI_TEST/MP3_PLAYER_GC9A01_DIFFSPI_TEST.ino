#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SPI.h"
#include "TFT_eSPI.h"


// microSD Card Reader connections
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

#define HSPI_MISO   12
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     15

// I2S Connections
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26

#define USE_DMA_TO_TFT

SPIClass * vspi = NULL;
SPIClass * hspi = NULL;
// I/O connections
//#define vol_plus 4
//#define skip_bttn 2
//#define vol_minus 15
//#define play_bttn 13

// Initialize the OLED display using Arduino Wire:
//SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);

TaskHandle_t Task1;
TaskHandle_t Task2;

// Create Audio object
Audio audio;

int counter = 1;
int volume = 0;
void setup() {

  //pinMode(vol_plus, INPUT_PULLUP);
  //pinMode(skip_bttn, INPUT_PULLUP);
  //pinMode(play_bttn, INPUT_PULLUP);
  //pinMode(vol_minus, INPUT_PULLUP);

  // Set microSD Card CS as OUTPUT and set HIGH
  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  pinMode(HSPI_SS, OUTPUT);
  digitalWrite(HSPI_SS, HIGH);

  vspi = new SPIClass(VSPI);
  hspi = new SPIClass(HSPI);

  // Initialize SPI bus for microSD Card
  //vspi->begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  //hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI);

  vspi->begin();
  hspi->begin();
  // Start Serial Port
  Serial.begin(115200);

  //Start I2C channel
  //Wire.begin();
  // Start microSD Card
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("Error accessing microSD card!");
    while (true)
      ;
  }
  
  }
  // Setup I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  //audio.setBufsize(int rambuf_sz, int psrambuf_sz)
  // Set Volume
  audio.setVolume(20);

  // Open music file
  audio.connecttoFS(SD, "/wavfile.wav");
  
  //init tft dispaly
  tft.init();

  //tn = micros();
  tft.fillScreen(TFT_BLACK);

  // Create a viewport 220 x 300 pixels
  tft.setViewport(10,10,220,300);
  tft.setRotation(1);
  img.createSprite(220,135);

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
    Task1code, /* Task function. */
    "Task1",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task1,    /* Task handle to keep track of created task */
    0);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task2",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &Task2,    /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */

}


void Task1code(void* pvParameters) {
  Serial.print("Task1 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    audio.loop();
  }
}

int x = 0;

//Task2code: blinks an LED every 700 ms
void Task2code(void* pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    img.fillCircle(x,36, 30, TFT_BLACK);
    x+=1;
    if(x>240)
    x=0;
    img.fillCircle(x,36,30, TFT_ORANGE);
    img.fillRect(18,70,100,100,TFT_BLACK);
    img.drawString(String(x),20,74,7); 

    img.pushSprite(0,0);
    delay(1000);
  
  }
}

void loop() {
}