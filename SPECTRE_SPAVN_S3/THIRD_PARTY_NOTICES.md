# Third-party sources

Dependency versions and archive URLs/checksums are recorded in dependencies.lock.json. Their respective licenses apply.

- Arduino ESP32 core 3.3.1: https://github.com/espressif/arduino-esp32/tree/3.3.1 ; license in licenses/Arduino_ESP32.md. Original source archive included in source_archives, including upstream notices.
- Arduino_GFX 1.6.8: https://github.com/moononournation/Arduino_GFX ; license in licenses/Arduino_GFX.txt, original source archive included.
- esp_lcd_ili9488: https://github.com/atanisoft/esp_lcd_ili9488 ; driver source, Arduino adapter and upstream LICENSE under vendor/esp_lcd_ili9488. Driver reports version 1.1.1; snapshot retrieved from master.

Additional examples come from the core release identified by each catalog URL. Compatibility probes use separately installed dependencies; their binaries are not included. Bundled main firmware uses the ESP32 core and Arduino_GFX. Preserve upstream notices when redistributing source.
