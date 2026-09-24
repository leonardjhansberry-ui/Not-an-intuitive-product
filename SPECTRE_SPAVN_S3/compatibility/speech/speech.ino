#include <Arduino.h>
#include <ESP_I2S.h>
#include <ESP_SR.h>
I2SClass audio;
const sr_cmd_t commands[]={{0,"Go dark","Gb DnRK"}};
void event(sr_event_t,int,int){}
void setup(){Serial.begin(115200);ESP_SR.onEvent(event);}
void loop(){
 if(Serial.available() && Serial.read()=='!') {
  audio.setPins(17,47,-1,16);
  audio.begin(I2S_MODE_STD,16000,I2S_DATA_BIT_WIDTH_16BIT,I2S_SLOT_MODE_STEREO);
  ESP_SR.begin(audio,commands,1,SR_CHANNELS_STEREO,SR_MODE_WAKEWORD,"MM");
 }
}
