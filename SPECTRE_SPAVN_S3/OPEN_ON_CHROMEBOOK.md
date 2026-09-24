# Open this saved package later

Download the ZIP from this conversation on the Chromebook. Extract it in the Files app,
then copy the extracted SPECTRE_SPAVN_S3 folder into Linux files for pico@penguin.

Read START_HERE.md first. Source and compiled foundation firmware are included.
No board needs to be connected to inspect the package or repeat a build.

CHROMEBOOK TERMINAL 1 — Linux shell (bash), pico@penguin

After placing the extracted folder directly in Linux files, a future rebuild is:

```bash
cd ~/SPECTRE_SPAVN_S3 && python3 build.py --install --target all
```

This requires arduino-cli already installed, internet access, and several GB of free disk space.
It builds locally and writes short PASS/FAIL summaries plus full logs. It never flashes or erases a board.
The bundled binaries do not require recompilation merely to retrieve them.

Upload instructions will be matched to the identified physical S3 and confirmed screen wiring when ready.
The original ESP32U/SENTRY is not a target for these binaries.
