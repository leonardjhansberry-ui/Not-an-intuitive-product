// Compile/link probe, NOT the operational SPECTRE application.
// APIs are reachable for link verification. Do not upload with hardware attached.
#include <Arduino.h>
#include <Arduino_GFX_Library.h>
#include <lvgl.h>
#include <OneButton.h>
#include <WiFi.h>
#include <esp_now.h>
#ifdef USE_ASYNC_WEB
#include <ESPAsyncWebServer.h>
#else
#include <WebServer.h>
#endif
#include <PubSubClient.h>
#include <NimBLEDevice.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <SD.h>
#include <ArduinoJson.h>
#include <RTClib.h>
#include <DHT.h>
#include <Adafruit_INA219.h>
#include <RF24.h>
#include <IRremote.hpp>
Arduino_ESP32SPI bus(14,10,12,11,-1);
Arduino_ILI9488_18bit display(&bus,21,3,false);
OneButton button(4,true,true);
WiFiClient network;
PubSubClient mqtt(network);
#ifdef USE_ASYNC_WEB
AsyncWebServer web(80);
#else
WebServer web(80);
#endif
RTC_PCF8523 rtc;
DHT dht(15,DHT22);
Adafruit_INA219 powerSensor;
RF24 radio(16,17);
void exerciseAPIs() {
 display.begin(16000000); display.fillScreen(0);
 lv_init(); lv_obj_t* label=lv_label_create(lv_screen_active()); lv_label_set_text(label,"SPECTRE probe");
 button.tick(); WiFi.mode(WIFI_STA); WiFi.scanNetworks(true);
 esp_now_init();
#ifdef USE_ASYNC_WEB
 web.on("/",HTTP_GET,[](AsyncWebServerRequest* r){r->send(200,"text/plain","probe");});web.begin();
#else
 web.on("/",[](){web.send(200,"text/plain","probe");}); web.begin();web.handleClient();
#endif
 mqtt.setServer("127.0.0.1",1883);mqtt.loop();
 NimBLEDevice::init("SPECTRE-PROBE");NimBLEDevice::createServer();
 ArduinoOTA.setHostname("spectre-probe");ArduinoOTA.handle();
 LittleFS.begin(false);File f=LittleFS.open("/probe.txt","r");f.close();
 SD.begin(18);JsonDocument doc;doc["type"]="probe";serializeJson(doc,Serial);
 rtc.begin(); rtc.now();dht.begin();dht.readTemperature();powerSensor.begin();powerSensor.getBusVoltage_V();
 radio.begin();radio.isChipConnected();IrReceiver.begin(38);IrReceiver.decode();
}
void setup(){Serial.begin(115200);Serial.println("COMPILE PROBE ONLY; not an integrated application");}
void loop(){if(Serial.available() && Serial.read()=='!')exerciseAPIs();}
