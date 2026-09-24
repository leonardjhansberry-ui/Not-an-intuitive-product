#include <TFT_eSPI.h>
TFT_eSPI display;
void setup(){display.init();display.setRotation(3);display.fillScreen(TFT_BLACK);display.drawString("S3 ILI9488 compile probe",8,8);}
void loop(){}
