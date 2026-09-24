#pragma once
#include "config.h"
#if ENABLE_DISPLAY
#include <Arduino_GFX_Library.h>
Arduino_ESP32SPI screenBus(TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, GFX_NOT_DEFINED);
Arduino_ILI9488_18bit screen(&screenBus, TFT_RST, 3, false);
bool displayReady = false;
void beginDisplay() { displayReady = screen.begin(16000000); }
void drawPage(unsigned page, const char* title, bool link, unsigned count, uint32_t dropped,
              const String& result, uint64_t uptime) {
  if (!displayReady) return;
  // Fixed 480x320 landscape layout, text kept within 38 characters at size 2.
  screen.fillScreen(0x0000);
  screen.setTextSize(2); screen.setTextColor(0x07FF); screen.setCursor(12, 12);
  screen.printf("%s / %s", NODE_NAME, title);
  screen.drawFastHLine(12, 38, 456, 0x39E7);
  screen.setTextColor(0xFFFF); screen.setCursor(12, 54);
  switch (page) {
    case 0:
      screen.printf("Standalone interface\n\n  CyID: %s\n\n  Uptime: %llu s\n\n  Pending: %u", link ? "ONLINE" : "OFFLINE",
                    (unsigned long long)uptime, count); break;
    case 1:
      screen.printf("Free heap: %lu bytes\n\n  Device health: collecting\n\n  Sensors: not configured\n\n  RF monitoring: pending", (unsigned long)ESP.getFreeHeap()); break;
    case 2:
      screen.print("SELECT: record health sample\n\n  Test transmitter: pending\n\n  Sensor tools: pending"); break;
    case 3:
      screen.printf("CyID serial: %s\n\n  Peer discovery: pending\n\n  Wireless transport: pending", link ? "ONLINE" : "OFFLINE"); break;
    case 4:
      screen.printf("Pending: %u / %u\n\n  Dropped: %lu\n\n  Storage: RAM only\n\n  SELECT: retry transfer", count, QUEUE_CAPACITY, (unsigned long)dropped);
      screen.setCursor(12, 220); screen.print(result.substring(0, 37)); break;
    case 5:
      screen.print("ESP32-S3 N16R8\n\n  ILI9488 / SPI / rotation 3\n\n  Serial: 115200\n\n  Clock: unsynchronized"); break;
  }
  screen.setTextColor(0x07FF); screen.setCursor(12, 292);
  screen.print("PREV   NEXT   SELECT   HOME");
}
#else
void beginDisplay() {}
void drawPage(unsigned, const char*, bool, unsigned, uint32_t, const String&, uint64_t) {}
#endif
