#include <Arduino.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <Preferences.h>
#include "config.h"
#include "queue.h"
#include "display.h"
#include "peripherals.h"

#if !defined(CONFIG_IDF_TARGET_ESP32S3)
#error This firmware targets ESP32-S3 only
#endif

RecordQueue<QUEUE_CAPACITY> pending;
const char* pages[] = {"HOME", "MONITOR", "LAB", "DEVICES", "LOGS", "SETTINGS"};
unsigned page = 0;
char deviceId[20], bootId[33], inputLine[160];
size_t inputLength = 0;
bool discardLine = false, linked = false, sentHead = false;
bool ready = false;
uint32_t sequence = 0, lastSample = 0, lastSend = 0, lastContact = 0;
String lastResult = "No analysis received";

void showStatus() {
  Serial.printf("%s,UPTIME=%llu,VOICE_EVENTS=0,STATUS=OK,VOICE_AVAILABLE=0,PAGE=%s,PENDING=%u,DROPPED=%lu,LINK=%s\n",
    NODE_NAME, (unsigned long long)(esp_timer_get_time()/1000000), pages[page],
    (unsigned)pending.size(), (unsigned long)pending.dropped, linked ? "ONLINE" : "OFFLINE");
  Serial.print("RESULT "); Serial.println(lastResult);
}
void navigate(int delta) { page = (page + 6 + delta) % 6; showStatus(); }
void sampleNow() {
  if (sequence == UINT32_MAX) { Serial.println("ERROR sequence exhausted; reboot required"); return; }
  pending.push({++sequence, (uint64_t)(esp_timer_get_time()/1000), ESP.getFreeHeap(), pending.dropped});
}
void processCommand(const char* line) {
  if (!strcmp(line, "CYID_PING")) {
    linked = true; lastContact = millis(); Serial.printf("%s_PONG\n", NODE_NAME);
  } else if (!strcmp(line, "CYID_STATUS")) showStatus();
  else if (!strcmp(line, "NEXT")) navigate(1);
  else if (!strcmp(line, "PREV")) navigate(-1);
  else if (!strcmp(line, "SAMPLE")) sampleNow();
  else if (!strcmp(line, "SYNC")) { lastSend = millis() - RETRY_MS; }
  else if (!strncmp(line, "ACK ", 4)) {
    const Record* r = pending.front();
    if (r && sentHead) {
      char expected[100];
      snprintf(expected, sizeof(expected), "ACK %s %s %lu", deviceId, bootId, (unsigned long)r->seq);
      if (!strcmp(line, expected)) {
        pending.acknowledge(r->seq); sentHead = false;
        linked = true; lastContact = millis(); lastSend = millis() - RETRY_MS;
      }
    }
  } else if (!strncmp(line, "RESULT ", 7)) {
    lastResult = String(line + 7).substring(0, 100);
  } else if (!strcmp(line, "PERIPH")) showPeripherals();
  else if (!strcmp(line, "RFTEST")) runRfTxTest();
  else if (!strcmp(line, "HELP")) {
    Serial.println("HELP NEXT PREV SAMPLE SYNC CYID_PING CYID_STATUS PERIPH RFTEST");
  }
}
void pollSerial() {
  // Bound work so a noisy serial source cannot monopolize the loop.
  for (unsigned n = 0; n < 256 && Serial.available(); ++n) {
    char c = Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      if (!discardLine) { inputLine[inputLength] = 0; processCommand(inputLine); }
      inputLength = 0; discardLine = false;
    } else if (!discardLine) {
      if (c < 32 || c > 126 || inputLength >= sizeof(inputLine)-1) discardLine = true;
      else inputLine[inputLength++] = c;
    }
  }
}
void sendPending() {
  const Record* r = pending.front();
  if (!linked || !r || (uint32_t)(millis()-lastSend) < RETRY_MS) return;
  char data[420];
  snprintf(data, sizeof(data), "{\"v\":1,\"node\":\"%s\",\"device\":\"%s\",\"boot\":\"%s\",\"seq\":%lu,\"type\":\"status\",\"uptime_ms\":%llu,\"clock\":\"unsynced\",\"heap_bytes\":%lu,\"dropped\":%lu}",
    NODE_NAME, deviceId, bootId, (unsigned long)r->seq, (unsigned long long)r->uptime_ms,
    (unsigned long)r->heap, (unsigned long)r->dropped);
  Serial.println(data); sentHead = true; lastSend = millis();
}
bool initializeBootId() {
  Preferences prefs;
  if (!prefs.begin("spectre", false)) return false;
  uint64_t counter = prefs.getULong64("boot", 0);
  if (counter == UINT64_MAX || prefs.putULong64("boot", counter + 1) != 8) {
    prefs.end(); return false;
  }
  prefs.end();
  snprintf(bootId, sizeof(bootId), "%016llx%08lx%08lx", (unsigned long long)(counter + 1),
           (unsigned long)esp_random(), (unsigned long)esp_random());
  return true;
}
void setup() {
  Serial.begin(SERIAL_BAUD); // UART-labelled port; USB CDC disabled in build profile.
  uint64_t mac = ESP.getEfuseMac();
  snprintf(deviceId, sizeof(deviceId), "%012llx", (unsigned long long)mac);
  // Retained boot counter prevents normal reboot collisions. Flash erasure resets it.
  ready = initializeBootId();
  if (!ready) { Serial.println("ERROR boot identity storage unavailable"); return; }
  beginDisplay();
  beginPeripherals();
#if ENABLE_BUTTONS
  for (int pin : BUTTON_PINS) pinMode(pin, INPUT_PULLUP);
#endif
  Serial.printf("%s_S3_READY v0.1.0\n", NODE_NAME);
  sampleNow(); showStatus();
}
void loop() {
  if (!ready) { delay(10); return; }
  pollSerial();
  pollButtons();
  if (linked && (uint32_t)(millis()-lastContact) > LINK_TIMEOUT_MS) linked = false;
  if ((uint32_t)(millis()-lastSample) >= SAMPLE_MS) { lastSample = millis(); sampleNow(); }
  sendPending();
  static uint32_t drawn = 0;
  static unsigned previousPage = 99;
  if (page != previousPage) {
    drawPage(page, pages[page], linked, pending.size(), pending.dropped, lastResult,
             (uint64_t)(esp_timer_get_time()/1000000));
    drawn = millis(); previousPage = page;
  }
  delay(1);
}

void pollButtons() {
#if ENABLE_BUTTONS
  static bool last[] = {true, true, true, true}, stable[] = {true, true, true, true};
  static uint32_t changed[4] = {};
  for (unsigned i = 0; i < 4; ++i) {
    bool value = digitalRead(BUTTON_PINS[i]);
    if (value != last[i]) { changed[i] = millis(); last[i] = value; }
    if (value != stable[i] && (uint32_t)(millis()-changed[i]) >= 35) {
      stable[i] = value;
      if (!value) {
        if (i == 0) navigate(-1);
        else if (i == 1) navigate(1);
        else if (i == 3) page = 0;
        else if (page == 4) lastSend = millis() - RETRY_MS;
        else if (page == 2) sampleNow();
        else showStatus();
      }
    }
  }
#endif
}
