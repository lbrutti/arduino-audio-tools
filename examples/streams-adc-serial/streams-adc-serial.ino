/**
 * @file streams-adc-serial.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/streams-adc-serial/README.md 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
 
#include "AudioTools.h"

using namespace audio_tools;  

uint8_t channels = 2;
AnalogAudioStream microphone;                 // analog microphone
CsvStream<int16_t> printer(Serial, channels); // ASCII output stream 
StreamCopy copier(printer, microphone);       // copies microphone into printer
ConverterAutoCenter<int16_t> center;          // make sure the avg of the signal is 0

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);

  AnalogConfig cfg = microphone.defaultConfig(RX_MODE);
  microphone.begin(cfg);
}


// Arduino loop - repeated processing 
void loop() {
  copier.copy(center);
}