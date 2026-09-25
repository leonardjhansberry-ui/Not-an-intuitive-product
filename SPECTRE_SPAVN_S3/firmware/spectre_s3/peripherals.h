#pragma once
#include <Arduino.h>
#include "config.h"

#if !SPAVN_PROFILE
#include <SPI.h>
#include <RF24.h>

constexpr int LM393_AO_PIN = 1;
constexpr int E01_CE_PIN = 17;
constexpr int E01_CSN_PIN = 18;
constexpr int E01_SCK_PIN = 9;
constexpr int E01_MOSI_PIN = 8;
constexpr int E01_MISO_PIN = 13;

SPIClass e01SPI(HSPI);
RF24 e01Radio(E01_CE_PIN, E01_CSN_PIN);
bool e01Ready = false;

uint8_t e01ReadStatus() {
  e01SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(E01_CSN_PIN, LOW);
  delayMicroseconds(5);
  uint8_t status = e01SPI.transfer(0xFF);
  digitalWrite(E01_CSN_PIN, HIGH);
  e01SPI.endTransaction();
  return status;
}

uint8_t e01ReadRegister(uint8_t reg) {
  e01SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  digitalWrite(E01_CSN_PIN, LOW);
  delayMicroseconds(5);
  e01SPI.transfer(reg & 0x1F);
  uint8_t value = e01SPI.transfer(0xFF);
  digitalWrite(E01_CSN_PIN, HIGH);
  e01SPI.endTransaction();
  return value;
}

void showPeripherals() {
  const uint16_t raw = analogRead(LM393_AO_PIN);
  const uint8_t status = e01ReadStatus();
  const uint8_t config = e01ReadRegister(0x00);
  Serial.printf("PERIPH,LM393_AO=%u,E01_RF24=%s,E01_STATUS=0x%02X,E01_CONFIG=0x%02X\n",
                raw, e01Ready ? "PASS" : "FAIL", status, config);
}

void beginPeripherals() {
  analogReadResolution(12);
  pinMode(LM393_AO_PIN, INPUT);

  pinMode(E01_CE_PIN, OUTPUT);
  digitalWrite(E01_CE_PIN, LOW);
  pinMode(E01_CSN_PIN, OUTPUT);
  digitalWrite(E01_CSN_PIN, HIGH);

  e01SPI.begin(E01_SCK_PIN, E01_MISO_PIN, E01_MOSI_PIN, E01_CSN_PIN);
  e01Ready = e01Radio.begin(&e01SPI);

  if (e01Ready) {
    e01Radio.setPALevel(RF24_PA_LOW);
    e01Radio.setDataRate(RF24_1MBPS);
    e01Radio.setChannel(76);
    e01Radio.stopListening();
  }

  showPeripherals();
}
#else
void beginPeripherals() {}
void showPeripherals() {
  Serial.println("PERIPH,NOT_AVAILABLE=SPAVN_PROFILE");
}
#endif
