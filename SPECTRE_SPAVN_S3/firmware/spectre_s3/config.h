#pragma once
// Proposed NEW S3 wiring only; never use with the old WROOM wiring.
#ifndef SPAVN_PROFILE
#define SPAVN_PROFILE 0
#endif
#ifndef ENABLE_DISPLAY
#define ENABLE_DISPLAY 1
#endif
#ifndef ENABLE_BUTTONS
#define ENABLE_BUTTONS 1
#endif
constexpr const char* NODE_NAME = SPAVN_PROFILE ? "SPAVN" : "SPECTRE";
constexpr int TFT_CS = 10, TFT_DC = 14, TFT_RST = 21;
constexpr int TFT_SCK = 12, TFT_MOSI = 11;
constexpr int BUTTON_PINS[] = {4, 5, 6, 7}; // PREV NEXT SELECT HOME; each button to GND
constexpr unsigned SAMPLE_MS = 5000;
constexpr unsigned RETRY_MS = 1500;
constexpr unsigned LINK_TIMEOUT_MS = 10000;
constexpr unsigned QUEUE_CAPACITY = 120;
constexpr unsigned SERIAL_BAUD = 115200;
