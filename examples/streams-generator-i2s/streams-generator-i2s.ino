/**
 * @file streams-generator-i2s.ino
 * @author Phil Schatzmann
 * @brief see https://github.com/pschatzmann/arduino-audio-tools/blob/main/examples/streams-generator-i2s/README.md 
 * @author Phil Schatzmann
 * @copyright GPLv3
 */
 
#include "AudioTools.h"
#include "AudioA2DP.h"

using namespace audio_tools;  

typedef int16_t sound_t;                                   // sound will be represented as int16_t (with 2 bytes)
uint16_t sample_rate=44100;
uint8_t channels = 2;                                      // The stream will have 2 channels 
SineWaveGenerator<sound_t> sineWave(32000);                // subclass of SoundGenerator with max amplitude of 32000
GeneratedSoundStream<sound_t> sound(sineWave);   // Stream generated from sine wave
I2SStream out; 
StreamCopy copier(out, sound);                             // copies sound into i2s

// Arduino Setup
void setup(void) {  
  // Open Serial 
  Serial.begin(115200);

  // start the bluetooth
  Serial.println("starting I2S...");
  I2SConfig config = out.defaultConfig(TX_MODE);
  config.sample_rate = sample_rate; 
  config.channels = channels;
  config.bits_per_sample = 16;
  out.begin(config);

  // Setup sine wave
  sineWave.begin(channels, sample_rate, N_B4);
}

// Arduino loop - copy sound to out 
void loop() {
  copier.copy();
}
