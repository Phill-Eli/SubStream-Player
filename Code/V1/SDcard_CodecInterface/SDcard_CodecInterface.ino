#include <SD.h>
#include <driver/i2s.h>
#include <SPI.h>
#include <SSD1306.h>

#define sd1      5
#define sd2      4
#define I2S_DOUT 25
#define I2S_BCLK 27
#define I2S_LRC  26
#define I2s_NUM  0

int play = 27;

int s1En = 5;
int s2En = 21;
int codec = 2;

ezButton button1(sd1);
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(mode, INPUT_PULLUP);
  pinMode(play, INPUT_PULLUP);
  pinMode(s1En, OUTPUT);
  pinMode(s2En, OUTPUT);
  pinMode(codec, OUTPUT);

}

void loop()
{
  // put your main code here, to run repeatedly:
  int btt = button1.getCount();
  int valid = initcard(5, 21);

  if(valid == 1)
  {
    
  }


}

void initcard(int card1, int card2)
{
  int crd1 = card1;
  int crd2 = card2;
  int btt = button1.getCount();
  
  if((btt%2) == 0)
  {
    if(!SD.begin(crd1))
    {
      Serial.println("Card 1 Mount Failed");
      return 0;
    }
    else
    {
      Serial.println("Card 1 Mount Successful");
      return 2;
    }
        
  }
  else
  {
    if(!SD.begin(crd2))
    {
      Serial.println("Card 2 Mount Failed");
      return 0;
    }
    else
    {
      Serial.println("Card 2 Mount Successful");
      return 1;
    }
  }
 }