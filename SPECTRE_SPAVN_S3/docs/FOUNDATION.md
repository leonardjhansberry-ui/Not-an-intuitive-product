# SPECTRE S3 foundation — 2026-09-24

Target: Lonely Binary ESP32-S3 N16R8, 16 MB flash and 8 MB octal PSRAM.
Status: both development firmware profiles compiled successfully; not hardware validated or deployment ready. See ../COMPATIBILITY.md.

## Implemented in this increment

- Independent periodic device-health sampling; no CyID connection required to start.
- Bounded RAM queue of 120 status records; about 10 minutes at the default five-second interval.
- Full queue preserves older unacknowledged records, rejects new records, counts losses.
- USB-UART serial transport at 115200 with bounded input parsing and retries.
- Persistent boot counter and record sequence for duplicate detection; erase of NVS resets the counter.
- CyID SQLite storage, committed before acknowledging, with duplicate and conflicting replay checks.
- Basic returned result text (stored / overflow reported). This is not advanced analysis.
- Six-page ILI9488 interface, four debounced buttons, and serial navigation.
- Separate SPECTRE/SPAVN compile profiles; SPAVN transmitter functions remain pending.

## Explicitly pending

Hardware validation of the proposed S3 wiring and ILI9488 rendering, sensor drivers, voice decoding,
Wi-Fi transport, ESP-NOW component messaging, authenticated wireless commands, RF monitoring,
SD persistence, RTC integration, power monitoring and enclosure integration. No fake sensor readings.
The current firmware does not activate Wi-Fi. Display/buttons default to enabled using docs/HARDWARE.md; disable both for a bare-board build.
The old WROOM pin map is not a valid S3 wiring plan.

Battery, SD and RTC purchases remain deferred. RAM records are lost on power loss/reset.
CyID is optional during collection, required for this increment's durable record storage.
Collection timestamps are uptime only; CyID adds receipt UTC without claiming it is collection UTC.

## Source and build

Arduino sketch: `firmware/spectre_s3/spectre_s3.ino`. Core compile target in CI: Espressif 3.3.1; GFX Library for Arduino 1.6.8.
Board: ESP32S3 Dev Module; Flash Size: 16MB; PSRAM: OPI; USB CDC On Boot: Disabled.
Use the UART-labelled USB-C port for this profile. No upload or erase action is performed by CI.
The CI default partition scheme need not use the full 16MB; there is no flash filesystem in this version.

The local Xtensa builds passed. The included CI configuration has not run on GitHub.
Repository writes were rejected with HTTP 403. A configured workflow is not a passed build;
inspect its result. Hardware acceptance is still required after compilation.

## Serial protocol

Newline-delimited ASCII. `CYID_PING` returns `SPECTRE_PONG` and enables transmission for ten seconds;
the receiver refreshes it every three seconds. No ping means local collection only.
`CYID_STATUS` returns human-readable status. `HELP`, `NEXT`, `PREV`, `SAMPLE`, `SYNC` are local controls.
Each JSON status record contains v, device, boot, seq, type, uptime_ms, clock, heap_bytes, dropped.
`ACK <device> <boot> <seq>` removes only the matching already-sent queue head.
`RESULT <text>` retains up to 100 characters of returned analysis text.
This serial protocol assumes a trusted physically connected host; it is not a wireless security protocol.
Firmware status retains the earlier prefix but voice is unavailable, explicitly marked VOICE_AVAILABLE=0.

## CyID receiver preparation

`cyid/receiver.py` requires Python 3 and `pyserial==3.5`.
It takes `--port` (an explicitly identified serial path) and optional `--db`.
Default database: `~/Cyberdeck/spectre-s3/records.sqlite3`.
It does not replace or modify the original `cyid-spectre.service` or receiver.
Only one receiver or serial monitor may own the board port. Port names are not board identities.
No installation or service activation has been performed on the user's Pi.

## Hardware acceptance, when the board is ready

1. Boot and navigate with CyID absent; confirm sampling continues.
2. Connect receiver; confirm data commit, ACK, and queue drain.
3. Disconnect/reconnect and replay a record; verify only one stored copy.
4. Fill the queue offline; verify loss count rather than silent overwrite.
5. Verify correct display/sensor pin mapping separately before enabling peripherals.
6. Add SD/RTC/power later and test power-loss recovery before portable deployment.

References:
- https://learn.lonelybinary.com/pinouts/esp32-s3
- https://learn.lonelybinary.com/boards/esp32-s3/powering-the-board
- https://docs.espressif.com/projects/arduino-esp32/en/latest/
