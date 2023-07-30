/*
  ESP32 SD I2S Music Player
  esp32-i2s-sd-player.ino
  Plays MP3 file from microSD card
  Uses MAX98357 I2S Amplifier Module
  Uses ESP32-audioI2S Library - https://github.com/schreibfaul1/ESP32-audioI2S
  * 
  DroneBot Workshop 2022
  https://dronebotworkshop.com
*/

// Include required libraries
#include "Arduino.h"
#include "Audio.h"
#include "SD.h"
#include "FS.h"
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306Wire.h"

// microSD Card Reader connections
#define SD_CS 5
#define SPI_MOSI 23
#define SPI_MISO 19
#define SPI_SCK 18

// I2S Connections
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC 26


// I/O connections
#define vol_plus 4
#define skip_bttn 2
#define vol_minus 15
#define play_bttn 13

// Initialize the OLED display using Arduino Wire:
SSD1306Wire display(0x3c, SDA, SCL);  // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h

TaskHandle_t Task1;
TaskHandle_t Task2;

// Create Audio object
Audio audio;

int counter = 1;
int volume = 0;
void setup() {

  pinMode(vol_plus, INPUT_PULLUP);
  pinMode(skip_bttn, INPUT_PULLUP);
  pinMode(play_bttn, INPUT_PULLUP);
  pinMode(vol_minus, INPUT_PULLUP);

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
  if (!SD.begin(SD_CS)) {
    Serial.println("Error accessing microSD card!");
    while (true)
      ;
  }

  // Setup I2S
  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);

  // Set Volume
  audio.setVolume(25);

  // Open music file
  audio.connecttoFS(SD, "/wavfile.wav");

  audioControl();

  display.init();

  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

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

  xTaskCreatePinnedToCore(
    Task2code, /* Task function. */
    "Task3",   /* name of task. */
    10000,     /* Stack size of task */
    NULL,      /* parameter of the task */
    0,         /* priority of the task */
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

//Task2code: blinks an LED every 700 ms
void Task2code(void* pvParameters) {
  Serial.print("Task2 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    display.clear();
    
    drawFontFace();
    drawProgressBar();
    display.display();
  }
}

void Task3code(void* pvParameters) {
  Serial.print("Task3 running on core ");
  Serial.println(xPortGetCoreID());

  for (;;) {
    audioControl();
  }
}


void drawFontFace() {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Track is Running");
}

void drawProgressBar() {
  int progress = (counter / 5) % 100;
  // draw the progress bar
  display.drawProgressBar(0, 45, 120, 10, progress);

  // draw the percentage as String
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 28, String(progress) + "%");
  counter++;
}

void audioControl()
{
  int volUp = digitalRead(vol_plus);
    int volDown = digitalRead(vol_minus);
    int play = digitalRead(play_bttn);
    int skip = digitalRead(skip_bttn);
    
    audio.setVolumeSteps(15);
    if (volUp == 0)
    {
        volume += 15;
        audio.setVolume(volume);
    }
    if (volDown == 0)
    {
        volume -= 15;
        audio.setVolume(volume);
    }
    if (play == 0)
    {
        audio.pauseResume();
    } 
    if (skip == 0)
    {
        audio.pauseResume();
    }

}

void loop() {

}