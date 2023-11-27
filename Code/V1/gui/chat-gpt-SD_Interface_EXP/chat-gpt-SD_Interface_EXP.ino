#include <SD.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

const int upButtonPin = 16;    // External up button
const int downButtonPin = 33;  // External down button
const int selectButtonPin = 17; // External select button

String currentFolder = "/";
int currentIndex = 0;

void updateScreen() {
  tft.fillScreen(TFT_DARKGREY);

  File root = SD.open(currentFolder);
  File entry = root.openNextFile();
  int currentDisplayIndex = 0;

  // Iterate through the files and folders in the current directory
  while (entry) {
    // Highlight the selected item in TFT_ORANGE
    if (currentDisplayIndex == currentIndex) {
      tft.setTextColor(TFT_ORANGE, TFT_DARKGREY);
    } else {
      tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
    }

    // Display the name of the file or folder on the screen
    String itemName = entry.name();
    tft.drawString(itemName.c_str(), 10, 10 + currentDisplayIndex * 20);
    Serial.println(itemName.c_str());

    entry = root.openNextFile();
    currentDisplayIndex++;
  }

  // Wrap around if there are no items in the current folder
  if (currentDisplayIndex == 0) {
    currentIndex = 0;
  }

  tft.setTextColor(TFT_WHITE, TFT_DARKGREY);
}

void selectItem() {
  File root = SD.open(currentFolder);
  File entry = root.openNextFile();
  int currentDisplayIndex = 0;

  // Iterate through the files and folders in the current directory
  while (entry) {
    // If the selected item is a folder, enter that folder
    if (currentDisplayIndex == currentIndex) {
      if (entry.isDirectory()) {
        currentFolder = entry.name();
        currentIndex = 0;
      }
    }

    entry = root.openNextFile();
    currentDisplayIndex++;
  }

  // Wrap around if there are no items in the current folder
  if (currentDisplayIndex == 0) {
    currentIndex = 0;
  }
}

void setup() {
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

  pinMode(upButtonPin, INPUT_PULLUP);
  pinMode(downButtonPin, INPUT_PULLUP);
  pinMode(selectButtonPin, INPUT_PULLUP);
}

void loop() {
  updateScreen();

  if (digitalRead(upButtonPin) == LOW) {
    currentIndex = max(0, currentIndex - 1);
    delay(200); // Debounce
  }

  if (digitalRead(downButtonPin) == LOW) {
    currentIndex = min(int(SD.totalBytes() - 1), currentIndex + 1);
    delay(200); // Debounce
  }

  if (digitalRead(selectButtonPin) == LOW) {
    selectItem();
    delay(200); // Debounce
  }

  delay(1000);
}
