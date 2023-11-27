/*
#include <OneButton.h>
#include <SD.h>
#include <Arduino.h>
#include "buttons.h"

#define upButtonPin 16    // External up button
#define downButtonPin 33  // External down button
#define selectButtonPin 17 // External select button

int currentIndex = 0;
String folders[10]; 

// Setup a new OneButton on pin PIN_INPUT2.
OneButton up1(upButtonPin, true);
// Setup a new OneButton on pin PIN_INPUT2.
OneButton down1(downButtonPin, true);

OneButton select1(selectButtonPin, true);


//fclass::fclass(){}

void fclass::up()
{
  if(currentIndex > 0)
  {
    currentIndex -= 1;
    Serial.print("Current index is: "); Serial.print(currentIndex); Serial.println(". Go up");
  }
}


void fclass::down()
{
  if(currentIndex > 0)
  {
    currentIndex -= 1;
    Serial.print("Current index is: "); Serial.print(currentIndex); Serial.println(". Go down");
  }
  
}

void fclass::select()
{
  Serial.print(folders[currentIndex].c_str()); Serial.println(" has been selected");
}

void fclass::selectBack()
{
  Serial.println("Back to Folders");
}

void fclass::buttonsInit(int cIndex, String folder[10])
{
  currentIndex = cIndex;
  folders = folder;
  up1.attachClick(up);
  down1.attachClick(down);
  select1.attachClick(select);
}

void fclass::buttons()
{
  up1.tick();
  down1.tick();
  select1.tick();
}

fclass buttons = fclass();


*/



