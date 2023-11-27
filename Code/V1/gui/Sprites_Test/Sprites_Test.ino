#include <TFT_eSPI.h>
#include "play64.h"
#include "pause64.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

int buttonPin = 17;
bool isPlaying = false;

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  tft.init();
  tft.fillScreen(TFT_DARKGREY);
  tft.setSwapBytes(true);

  sprite.createSprite(64, 64);
}

void drawPlay() {
  tft.fillScreen(TFT_DARKGREY);
  sprite.pushImage(0, 0, 64, 64, play64);
  sprite.pushSprite(88, 250, TFT_BLACK);
  Serial.println("State: Play");
}

void drawPause() {
  tft.fillScreen(TFT_DARKGREY);
  sprite.pushImage(0, 0, 64, 64, pause64);
  sprite.pushSprite(88, 250, TFT_BLACK);
  Serial.println("State: Pause");
}

void loop() {
  int buttonState = digitalRead(buttonPin);

  if (buttonState == LOW) {
    // Button is pressed
    delay(50);  // Debouncing
    if (isPlaying) {
      drawPlay();
    } else {
      drawPause();
    }
    isPlaying = !isPlaying;  // Toggle state
    while (digitalRead(buttonPin) == LOW) {
      delay(10);  // Wait for button release
    }
  }
}
