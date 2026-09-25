# Cy.ID SPECTRE RF Checkpoint — 2026-09-25

## Scope
Consolidated checkpoint for the current SPECTRE + E01 + LM393 build and the Pi-side ESP32U RF receiver investigation.

## Hardware inventory
Exactly two E01-ML01DP5 modules are currently in the Cy.ID system:
1. SPECTRE E01 — ESP32-S3 side.
2. Existing ESP32U E01 — Pi-connected RF observation side.

## SPECTRE wiring

### ILI9488 TFT
- CS: GPIO10
- DC/RS: GPIO14
- RST: GPIO21
- SCK: GPIO12
- MOSI: GPIO11
- Display state: stable after cold boot; navigation works.
- Known limitation: uptime text is static until a page/button redraw. Attempts at periodic dynamic refresh caused blinking/corruption and were reverted.

### Buttons
- PREV: GPIO4
- NEXT: GPIO5
- SELECT: GPIO6
- HOME: GPIO7
- Other side of each button: GND

### LM393
- AO: GPIO1
- VCC: 3.3V
- GND: GND
- DO: disconnected
- GPIO1 ADC test: PASS
- LM393 analog response: PASS
- Typical observed AO values after correction: approximately 41–53

### E01-ML01DP5
- GND: GND
- VCC: 3.3V only
- CE: GPIO17
- CSN: GPIO18
- SCK: GPIO9
- MOSI: GPIO8
- MISO: GPIO13
- IRQ: disconnected
- E01 uses a separate SPI bus from the TFT.

### E01 power junction
- Red breadboard rail: 3.3V
- Blue breadboard rail: GND
- ESP32 3.3V -> red rail
- E01 VCC -> red rail
- Capacitor positive -> red rail
- ESP32 GND -> blue rail
- E01 GND -> blue rail
- Capacitor negative/striped side -> blue rail
- Do not apply 5V to the E01 rail.

## Verified SPECTRE status
- ESP32-S3 firmware build/upload: PASS
- CYID_STATUS: PASS
- PERIPH command: PASS
- Buttons/page navigation: PASS
- LM393 / GPIO1 ADC: PASS
- E01 RF24 initialization: PASS
- Stable E01 register results:
  - STATUS: 0x0E
  - CONFIG: 0x7E
- Controlled RF transmit test:
  - command: RFTEST
  - channel: 76 / 2476 MHz
  - packets requested: 3000
  - RFTEST START: PASS
  - RFTEST DONE: PASS
  - E01 remained RF24 PASS after the burst
- Local known-good commit before RFTEST addition: 3c1590d
  - message: SPECTRE LM393 and E01 RF24 hardware PASS

## Pi-side serial topology
- /dev/ttyACM0: Pico / MicroPython board
- /dev/ttyUSB0: existing ESP32U RF observation system
- /dev/ttyUSB0 is owned by:
  /home/ogun/Cyberdeck/rf-telemetry/live/live_processor.py
- Do not stop, reassign, or flash /dev/ttyUSB0 casually.

## Pi-side RF receiver investigation
- Existing artifacts include:
  - ~/Cyberdeck/rf-telemetry/live/current-sweep.txt
  - ~/Cyberdeck/rf-telemetry/processed/CYID-E01-RPD-BASELINE-01-PASS.txt
  - ~/Cyberdeck/rf-telemetry/logs/rpd-test-01.txt
- current-sweep.txt is stale; last observed update: 2026-09-22 02:13:03.
- Stored channel 76 value was 2476 MHz, 0.0%, 445 samples, but this is old data and cannot judge the 2026-09-25 RFTEST.
- live_processor.py is running but produced no fresh output files.
- A controlled 5-second direct read of /dev/ttyUSB0 while live_processor.py was paused produced no serial data.
- Therefore SPECTRE transmit is verified, but SPECTRE -> Pi-side ESP32U E01 reception remains UNVERIFIED, not failed.
- Do not reflash the Pi-side ESP32U until its existing RF firmware/output path is positively identified.

## Resume point
1. Preserve SPECTRE wiring and firmware state.
2. Identify/recover the Pi-side ESP32U E01 sweep output without reflashing if possible.
3. Confirm fresh channel-76 observations.
4. Repeat one controlled SPECTRE RFTEST burst.
5. Record receiver-side detection result.
6. Only after that, proceed to packet/payload receive/ACK testing.

## Do-not-change checkpoint items
- Do not alter SPECTRE E01 wiring.
- Do not alter the capacitor/power rail junction.
- Do not re-enable periodic full-screen TFT redraw.
- Do not assume a third E01 exists.
- Do not stop or reflash the Pi-side ESP32U until its current role is recovered.
