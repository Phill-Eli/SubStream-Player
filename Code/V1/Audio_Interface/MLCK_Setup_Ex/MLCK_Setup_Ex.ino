//Thanks a lot for all of this useful samples. I'm using the Arduino IDE. The used I2S2.h library is slightly different, so I have to made some changes to the code. All i2s_* functions are not void, they return a value from the type esp_err_t. Also i2s_config_t need values from special i2s_* _t types.
//Other changes:
//* Enable Masterclock generation before config and start i2s interface.
//* Arduino starts void setup() first, then void loop() as the name said, in a loop. So I rearrange the code to fit to this IDE.
//* Serial output while initializing, for debug

//Used Hardware:
//ESP-32-WROOM-32,  power supply from build in micro USB
//DIGILENT Pmod I2S2,  Vcc 3.3 from ESP-32,  MCLK from ESP-32 GPIO 0

//Code:
//==================================================================================================


// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Modifications to run the code at Arduino IDE
// Joerg Rettkowski 20.03.2021

#include "driver/i2s.h"

//error type
esp_err_t err;

//buffers
int rxbuf[256], txbuf[256];
float l_in[128], r_in[128];
float l_out[128], r_out[128];


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void setup() {

Serial.begin(115200);
delay (500);


  //enable MCLK on GPIO0
  REG_WRITE(PIN_CTRL, 0xFF0); 
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0_CLK_OUT1);

//------------------------------------------------------------
  
  i2s_config_t i2s_config = {

        .mode                 = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX  | I2S_MODE_RX), // i2s_mode_t             work mode
        .sample_rate          = 44100,                                                    // int                    sample rate
        .bits_per_sample      = I2S_BITS_PER_SAMPLE_32BIT,                                // i2s_bits_per_sample_t  bits per sample
        .channel_format       = I2S_CHANNEL_FMT_RIGHT_LEFT,                               // i2s_channel_fmt_t      channel format
        .communication_format = I2S_COMM_FORMAT_I2S_MSB,                                  // i2s_comm_format_t      communication format
        .intr_alloc_flags     = 0,                                                        // int                    Flags used to allocate the interrupt. One or multiple (ORred) ESP_INTR_FLAG_* values. See esp_intr_alloc.h for more info
        .dma_buf_count        = 6,                                                        // int                    DMA Buffer Count
        .dma_buf_len          = 512,                                                      // int                    DMA Buffer Length
        .use_apll             = true,                                                     // bool                   using APLL as main clock, enable it to get accurate clock
        .tx_desc_auto_clear   = true,                                                     // bool                   auto clear tx descriptor if there is underflow condition (helps in avoiding noise in case of data unavailability)
        .fixed_mclk           = true                                                      // int                    using fixed MCLK output. If use_apll = true and fixed_mclk > 0, then the clock output for is fixed and equal to the fixed_mclk value
                                             
    };

    err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    if (err != ESP_OK) {
       Serial.print("Failed installing driver: ");
       Serial.print(err);
       while (true);
     }
//------------------------------------------------------------

    i2s_pin_config_t pin_config = {
        .bck_io_num   = 26,
        .ws_io_num    = 25,
        .data_out_num = 22,
        .data_in_num  = 19                                                       
    };

   err = i2s_set_pin(I2S_NUM_0, &pin_config);
   if (err != ESP_OK) {
      Serial.print("Failed setting pin: ");
      Serial.print(err);
      while (true);
   }

   Serial.println("I2S driver installed.");
  
 

 }
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void loop() {
size_t readsize = 0; 

    //read 256 samples (128 stereo samples)
    err = i2s_read(I2S_NUM_0,&rxbuf[0],256*4, &readsize, 1000);
    
    if (err == ESP_OK && readsize==256*4) {
        
          //extract stereo samples to mono buffers
          int y=0;
          for (int i=0; i<256; i=i+2) {
            l_in[y] = (float) rxbuf[i];
            r_in[y] = (float) rxbuf[i+1];
            y++;
          }
          
          
          //do something with your left + right channel samples here in the buffers l_in/r_in and ouput result to l_out and r_out (e.g. build mono sum and apply -6dB gain (*0.5)
          for (int i=0; i<128; i++) {
            l_out[i] = 0.5f * (l_in[i] + r_in[i]);
            r_out[i] = l_out[i];
          }
          
          
          //merge two l and r buffers into a mixed buffer and write back to HW
          y=0;
          for (int i=0;i<128;i++) {
            txbuf[y] = (int) l_out[i];
            txbuf[y+1] = (int) r_out[i];
            y=y+2;
          }

          //write 256 samples (128 stereo samples)
          i2s_write(I2S_NUM_0, &txbuf[0],256*4, &readsize, 1000);
          
      }   // end of cycle
}         // end of void loop()


static const int i2s_num = 0; // i2s port number

i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_TX,
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S
    .tx_desc_auto_clear = false,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1  // Interrupt level 1, default 0
};

i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);


static const int i2s_num = 0; // i2s port number

i2s_config_t i2s_config = {
    .mode = I2S_MODE_MASTER | I2S_MODE_TX,
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S
    .tx_desc_auto_clear = false,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1  // Interrupt level 1, default 0
};

i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);








