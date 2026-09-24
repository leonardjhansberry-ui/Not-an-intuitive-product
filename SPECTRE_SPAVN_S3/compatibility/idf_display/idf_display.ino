#include <Arduino.h>
#include <esp_lcd_ili9488.h>
#include <esp_lcd_panel_ops.h>
void setup(){Serial.begin(115200);}
void loop(){
 if(Serial.available() && Serial.read()=='!'){
  esp_lcd_panel_dev_config_t config={};
  config.reset_gpio_num=21;config.bits_per_pixel=18;
  esp_lcd_panel_handle_t panel=nullptr;
  // Null IO deliberately prevents activating a display; compile/link probe only.
  esp_err_t result=esp_lcd_new_panel_ili9488(nullptr,&config,480*20,&panel);
  Serial.println(result);
 }
}
