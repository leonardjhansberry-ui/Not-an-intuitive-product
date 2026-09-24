# Compatibility report

Build target: ESP32-S3 N16R8, Arduino ESP32 3.3.1, OPI PSRAM, 16MB flash.
Tests are real Xtensa compile/link checks. They do not establish hardware operation,
radio coexistence, battery runtime, runtime heap sufficiency, or complete feature integration.

## Firmware

| Build | Result | App bytes | Static RAM bytes |
| --- | --- | ---: | ---: |
| SPECTRE foundation | PASS | 427071 | 26064 |
| SPAVN foundation | PASS | 427071 | 26064 |

Different SHA256 hashes verify these are distinct profile binaries, despite identical sizes.
These two builds contain the foundation functions documented in START_HERE, not the full feature selection.

## Selected codes 1–22

| # | Source | Compile/link evidence |
| --- | --- | --- |
| 1 | Arduino_GFX | Both firmware builds and common probe PASS |
| 2 | TFT_eSPI | Separate ILI9488/S3 probe PASS |
| 3 | LVGL 9.3.0 | Common probe PASS; defaults via LV_CONF_SKIP |
| 4 | esp_lcd_ili9488 1.1.1 | Separate ESP-IDF-driver probe PASS through an Arduino packaging adapter |
| 5 | OneButton | Common probe PASS |
| 6 | ESP-NOW | Common probe uses underlying ESP-NOW API; peer exchange not tested |
| 7 | Wi-Fi scanning | Common probe PASS |
| 8 | Basic WebServer | Common probe PASS |
| 9 | ESPAsyncWebServer / Async TCP | Separate async web probe PASS |
| 10 | PubSubClient MQTT | Common probe PASS; broker not connected |
| 11 | NimBLE-Arduino | Common probe PASS; no BLE device paired |
| 12 | ArduinoOTA | Common probe PASS; no OTA update performed |
| 13 | USB HID keyboard | Separate native USB OTG probe PASS; sends no keystrokes |
| 14 | LittleFS | Common probe PASS; flash filesystem not mounted on hardware |
| 15 | SD | Common probe PASS; no physical card |
| 16 | ArduinoJson | Common probe PASS |
| 17 | RTClib PCF8523 | Common probe PASS; no RTC connected |
| 18 | DHT | Common probe PASS; exact sensor type still needs confirmation |
| 19 | INA219 | Common probe PASS; optional hardware |
| 20 | RF24 | Common probe PASS; E01 wiring/power still need validation |
| 21 | IRremote | Common probe PASS; IR input/output hardware not verified |
| 22 | ESP-SR | Separate Arduino ESP_SR wrapper probe PASS; speech partition and suitable I2S audio needed |

The common probe links 17 selected items in one sketch (1,3,5,6,7,8,10,11,12,14–21).
It calls representative APIs, not each project's complete original demonstration.
Alternative display drivers are not wired to the same panel simultaneously. The async web and
speech/USB probes were not combined with the common probe. No claim is made that all 22 coexist at runtime.

## Size and configuration findings

- Common probe: 1,508,303 app bytes; 125,496 static RAM bytes. It exceeds the default
  1,310,720-byte app partition; huge_app (3MB app) was used. That probe partition does not support OTA.
  A production combined build needs a custom 16MB layout with TWO adequately sized OTA slots and storage.
- Async web probe: 989,115 app bytes; 43,816 static RAM bytes.
- TFT_eSPI probe: 336,019 app bytes; 21,392 static RAM bytes.
- ESP-IDF ILI9488 probe: 311,571 app bytes; 20,732 static RAM bytes.
- Speech probe: 802,063 app bytes; 33,580 static RAM bytes. Speech models and microphone wiring
  still require deployment validation; ESP_SR is not a driver for the existing CI1302 UART module.
- LVGL initially failed because LV_CONF_SKIP had not reached assembly compilation.
  Adding it consistently to compiler.c, compiler.cpp AND compiler.S flags fixed the build.
- The ILI9488 ESP-IDF probe initially used a three-argument constructor; this version requires
  an explicit conversion-buffer size. The test was corrected and recompiled successfully.
- A toolchain linker GNU-stack warning occurs in the common build; compilation/linking succeeds.

Runtime design still needs SPI chip-select coordination, a final peripheral GPIO plan,
Wi-Fi/ESP-NOW channel coordination, authenticated network controls and bounded task scheduling.
The compile-only probe's illustrative peripheral pins are NOT a wiring plan.

## Host-side verification

Five CyID receiver tests passed: deduplication/persistence, conflicting replay rejection,
reboot separation, invalid record rejection, and no ACK when storage fails.
The native C++ queue test passed: overflow handling, ordered acknowledgements, retry and wraparound.
