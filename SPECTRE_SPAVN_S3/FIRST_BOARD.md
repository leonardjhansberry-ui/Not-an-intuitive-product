# First board connection — Chromebook

The boards have arrived. Start with one new ESP32-S3 N16R8 only; leave the working SENTRY ESP32U alone.

1. Leave the ILI9488 screen, buttons and other peripherals disconnected.
2. Connect a data-capable USB cable to the new board's **UART** USB-C port.
3. Allow ChromeOS to connect/share the USB serial device with Linux when offered.
4. In the Chromebook Linux terminal, run the read-only check below and copy its output into this conversation. No pictures are required.

CHROMEBOOK TERMINAL 1 — Linux shell (bash), pico@penguin

```bash
python3 - <<'PY'
import glob
ports = sorted(set(glob.glob('/dev/ttyUSB*') + glob.glob('/dev/ttyACM*')))
print('SERIAL PORTS:', ', '.join(ports) if ports else 'NONE')
PY
```

This check lists port names; chip and flash identity must be read before writing firmware. If no port appears, check USB sharing and the cable. The compiled main firmware expects ESP32-S3, 16 MB flash, OPI PSRAM, UART at 115200 baud. First boot should report `SPECTRE_S3_READY v0.1.0` or `SPAVN_S3_READY v0.1.0`. Screen operation will be tested separately after confirming the actual breakout interface, supply requirements and wiring.
