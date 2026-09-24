# Hardware allocation and proposed new wiring

User update, 2026-09-24:
- SPECTRE: new Lonely Binary ESP32-S3 N16R8 + ILI9488.
- SPAVN: recreate on another new ESP32-S3 N16R8 + ILI9488.
- SENTRY: first original ESP32U; user reports this is the only original board still working.
- Third S3 board: unassigned. No changes to SENTRY's firmware or wiring.

This firmware assumes SPI ILI9488 modules (480x320 in landscape). Parallel/RGB modules are not supported by this profile.
Driver: Arduino_GFX Arduino_ILI9488_18bit, 16 MHz initial SPI speed, rotation 3.
The table is the proposed NEW GPIO assignment for each S3, not a description of existing wiring.

| Connection | ESP32-S3 GPIO |
| --- | --- |
| TFT CS | 10 |
| TFT DC | 14 |
| TFT RST | 21 |
| TFT SCK | 12 |
| TFT MOSI / SDI | 11 |
| TFT SDO / MISO | Leave disconnected for this write-only display driver |
| PREV button | 4 |
| NEXT button | 5 |
| SELECT button | 6 |
| HOME button | 7 |

Each button connects its GPIO to GND on press; internal pull-ups and 35ms debounce are enabled.
Touch is deferred. TFT VCC/backlight wiring depends on the actual module's regulator and LED resistor;
the controller name alone does not establish its power requirements. Confirm the module before powering it.
This table intentionally avoids USB GPIO19/20, console GPIO43/44, PSRAM GPIO35/36/37,
boot-strapping pins and onboard RGB GPIO48. GPIO8/9 are reserved here for later I2C RTC/sensors.
No old WROOM pin assignments are carried over.

Set SPAVN_PROFILE=1 for the SPAVN identity. It currently shares the interface and health telemetry;
the dedicated transmitter/test suite is still to be implemented. Device identity also includes its unique chip MAC.
Set ENABLE_DISPLAY=0 and ENABLE_BUTTONS=0 for a bare-board build before external wiring exists.

Reference: https://learn.lonelybinary.com/pinouts/esp32-s3
