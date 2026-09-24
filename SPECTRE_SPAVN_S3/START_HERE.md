# SPECTRE + SPAVN — compiled S3 build package

Prepared for pico@penguin (Chromebook Linux terminal).
Target: Lonely Binary ESP32-S3 N16R8 with SPI ILI9488 screens.

## What is ready

Two distinct development firmware builds are in binaries/SPECTRE and binaries/SPAVN.
They compile successfully with Espressif Arduino core 3.3.1 and Arduino_GFX 1.6.8.
Their functionality is the original foundation: six pages, four buttons, health sampling,
RAM record queue, USB serial transfer, and CyID receipt/storage acknowledgements.

The selected 1–22 code sources were checked in compatible groups and separate alternatives.
Those checks are not 22 fully integrated working features. See COMPATIBILITY.md for the exact scope.
Additional examples 26–50 are in additional25, with their source and compile results.

## Start with these files

The boards have arrived: begin with FIRST_BOARD.md for a read-only USB connection check.

- docs/HARDWARE.md: proposed NEW S3 wiring; preserve the original SENTRY ESP32U.
- COMPATIBILITY.md: what passed, configuration fixes, and runtime limits.
- OPEN_ON_CHROMEBOOK.md: how to open or rebuild later.
- firmware/spectre_s3/: editable source. SPAVN_PROFILE=0 selects SPECTRE; 1 selects SPAVN.
- cyid/receiver.py: tested serial receiver; no installation on the Pi has been performed.
- logs/: actual compiler and test outputs.
- SHA256SUMS.txt: checksums for firmware, source and dependency snapshots.

## Before eventual flashing

Only use these binaries on the specified ESP32-S3 N16R8. They are not for SENTRY's ESP32-WROOM-32U.
Confirm the SPI screen module and NEW pin assignments first. VCC/backlight requirements depend
on the actual display breakout and are not settled by its ILI9488 controller name.

The firmware expects the UART-labelled USB-C port, 115200 baud, USB CDC disabled, OPI PSRAM.
No board has been flashed here. No physical screen, radio, button or sensor has been tested here.
The screen/buttons are enabled at build time; the listed pin map must match.
The combined compatibility sketches are compile probes only, NOT firmware to deploy.

## Binary files

Each profile includes the application, bootloader, partition table and merged flash image.
The merged image is a fresh-install image at offset 0 and includes the partition layout;
do not flash it over a working device as an ordinary app-only update. Upload instructions
will be chosen when the physical board is identified and ready. Merely opening this package
or running build.py never flashes anything.

Battery, SD and RTC deployment purchases remain deferred. Current RAM records disappear on reset.
Firmware version string remains v0.1.0; this package adds real compilation evidence and library checks.
GitHub publishing was previously rejected. This saved package does not claim a repository commit.
