/**
 * @file streams-url_raw-serial.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/streams-url_raw-serial/README.md 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
 
#include "WiFi.h"
#include "AudioTools.h"

using namespace audio_tools;  

URLStream music;  // Music Stream
int channels = 2; // The stream has 2 channels 
CsvStream<int16_t> printer(Serial, channels);  // ASCII stream 
StreamCopy copier(printer, music);    // copies music into printer

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);
  
  // connect to WIFI
  WiFi.begin("network-name", "password");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500); 
  }

  // open music stream - it contains 2 channels of int16_t data
  music.begin("https://pschatzmann.github.io/arduino-audio-tools/resources/audio.raw");
}


// Arduino loop - repeated processing 
void loop() {
  copier.copy();
}