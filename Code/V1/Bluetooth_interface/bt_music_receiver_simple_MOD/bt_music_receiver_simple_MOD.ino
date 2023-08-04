/*
  Streaming data from Bluetooth to internal DAC of ESP32
  Using Library: https://github.com/pschatzmann/ESP32-A2DP

  Copyright (C) 2020 Phil Schatzmann
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// Connect one side of speaker to GND, the other to GPIO25
// Can use existing speakers / headphones --> https://i.pinimg.com/originals/4e/72/72/4e72729dba13164ad78586395fe11873.png

// ==> Example to use built in DAC of ESP32

#include "BluetoothA2DPSink.h"

//        I2S
          #define I2S_DOUT      25          // i2S Data out oin
          #define I2S_BCLK      27          // Bit clock
          #define I2S_LRC       26          // Left/Right clock, also known as Frame clock or word select
          #define I2S_NUM       0           // i2s port number

BluetoothA2DPSink a2dp_sink;

// VolumeChanged doesnt work
int volume = 50;

// I/O connections
#define vol_plus 4
#define skip_bttn 14
#define vol_minus 15
#define play_bttn 13

void volumeChanged(int newVolume) {
    Serial.println(newVolume);
    volume = newVolume;
}
    
// for esp_a2d_connection_state_t see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_a2dp.html#_CPPv426esp_a2d_connection_state_t
void connection_state_changed(esp_a2d_connection_state_t state, void* ptr) {
    Serial.println(a2dp_sink.to_str(state));
}

// for esp_a2d_audio_state_t see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_a2dp.html#_CPPv421esp_a2d_audio_state_t
void audio_state_changed(esp_a2d_audio_state_t state, void* ptr) {
    Serial.println(a2dp_sink.to_str(state));
}

void setup() {
    Serial.begin(115200);
    
    static const i2s_pin_config_t pin_config = 
      {
          .bck_io_num = I2S_BCLK,                           // The bit clock connectiom, goes to pin 27 of ESP32
          .ws_io_num = I2S_LRC,                             // Word select, also known as word select or left right clock
          .data_out_num = I2S_DOUT,                         // Data out from the ESP32, connect to DIN on 38357A
          .data_in_num = I2S_PIN_NO_CHANGE                  // we are not interested in I2S data into the ESP32
      };

    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
        .sample_rate = 44100, // corrected by info from bluetooth
        .bits_per_sample = (i2s_bits_per_sample_t)16, /* the DAC module will only take the 8bits from MSB */
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = (i2s_comm_format_t)I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = 0, // default interrupt priority
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };
    
    pinMode(vol_plus, INPUT_PULLUP);
    pinMode(skip_bttn, INPUT_PULLUP);
    pinMode(play_bttn, INPUT_PULLUP);
    pinMode(vol_minus, INPUT_PULLUP);

    a2dp_sink.set_i2s_config(i2s_config);
    a2dp_sink.set_on_volumechange(volumeChanged);
    a2dp_sink.set_on_connection_state_changed(connection_state_changed);
    a2dp_sink.set_on_audio_state_changed(audio_state_changed);
    a2dp_sink.start("Phill's DIY Speaker");
}

void audioControl()
{
  int volUp = digitalRead(vol_plus);
  int volDown = digitalRead(vol_minus);
  int play = digitalRead(play_bttn);
  int skip = digitalRead(skip_bttn);

    
  if (volUp == 0)
  {
    volume += 10;
    a2dp_sink.set_volume(volume);
    delay(50);
  }
  if (volDown == 0)
  {
      volume -= 10;
      a2dp_sink.set_volume(volume);
      delay(50);
  }
  if (play == 0)
  {
    a2dp_sink.pause();
  } 
  if (skip == 0)
  {
    a2dp_sink.play();
  }
}

void loop() {
  audioControl();
  
}