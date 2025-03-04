/**
 * @file stream-memory_wav-serial.ino
 * @author Phil Schatzmann
 * @brief decode WAV strea
 * @version 0.1
 * @date 2021-01-24
 * 
 * @copyright Copyright (c) 2021
 
 */
#include "AudioTools.h"
#include "knghtsng.h"

using namespace audio_tools;  

// MemoryStream -> AudioOutputStream -> WAVDecoder -> CsvStream
MemoryStream wav(knghtsng_wav, knghtsng_wav_len);
CsvStream<int16_t> out(Serial);  // ASCII stream 
EncodedAudioStream enc(out, new WAVDecoder());
StreamCopy copier(enc, wav);    // copy in to out

void setup(){
  Serial.begin(115200);
  AudioLogger::instance().begin(Serial, AudioLogger::Debug);  

  // update number of channels from wav file
  in.setNotifyAudioChange(out);
  
  out.begin();
  in.begin()
}

void loop(){
  if (in) {
    copier.copy();
  } else {
    auto info = in.decoder().audioInfo();
    LOGI("The audio rate from the wav file is %d", info.sample_rate);
    LOGI("The channels from the wav file is %d", info.channels);
    stop();
  }
}