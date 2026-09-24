#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
AsyncWebServer server(80);
AsyncWebSocket socket("/ws");
void setup(){
 Serial.begin(115200);
 server.on("/",HTTP_GET,[](AsyncWebServerRequest* request){request->send(200,"text/plain","compile probe");});
 server.addHandler(&socket);
 server.begin();
}
void loop(){socket.cleanupClients();}
