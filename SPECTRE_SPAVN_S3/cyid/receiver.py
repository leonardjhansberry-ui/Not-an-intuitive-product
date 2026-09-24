"""SPECTRE v1 serial ingest. ACK is emitted only after SQLite commits."""
import argparse
import datetime as dt
import json
import re
import sqlite3
import time
from pathlib import Path


def open_database(path):
    db = sqlite3.connect(path)
    db.execute('PRAGMA journal_mode=WAL')
    db.execute('PRAGMA synchronous=FULL')
    db.execute('''CREATE TABLE IF NOT EXISTS records (
        device TEXT, boot TEXT, seq INTEGER, received_utc TEXT NOT NULL,
        payload TEXT NOT NULL, PRIMARY KEY(device, boot, seq))''')
    db.commit()
    return db


def accept_record(db, line):
    if len(line) > 1024:
        raise ValueError('Oversized record')
    record = json.loads(line)
    if not isinstance(record, dict):
        raise ValueError('Expected object')
    if record.get('v') != 1 or record.get('type') != 'status' or record.get('clock') != 'unsynced':
        raise ValueError('Unsupported schema')
    for field, pattern in [('device', r'[0-9a-f]{12}'), ('boot', r'[0-9a-f]{32}')]:
        if not isinstance(record.get(field), str) or not re.fullmatch(pattern, record[field]):
            raise ValueError('Invalid identity')
    for field, minimum, maximum in [('seq', 1, 2**32-1), ('uptime_ms', 0, 2**63-1),
                                     ('heap_bytes', 0, 2**32-1), ('dropped', 0, 2**32-1)]:
        if type(record.get(field)) is not int or not minimum <= record[field] <= maximum:
            raise ValueError('Invalid numeric field')
    payload = json.dumps(record, sort_keys=True, separators=(',', ':'))
    key = (record['device'], record['boot'], record['seq'])
    existing = db.execute('SELECT payload FROM records WHERE device=? AND boot=? AND seq=?', key).fetchone()
    if existing and existing[0] != payload:
        raise ValueError('Identity collision with different content')
    with db:
        db.execute('INSERT OR IGNORE INTO records VALUES (?,?,?,?,?)',
                   (*key, dt.datetime.now(dt.timezone.utc).isoformat(), payload))
    # Simple status analysis only; no RF or sensor inference.
    result = 'Queue overflow reported' if record['dropped'] else 'Status stored'
    return f'ACK {key[0]} {key[1]} {key[2]}\nRESULT {result}\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', required=True, help='Explicit SPECTRE port, preferably /dev/serial/by-id/...')
    parser.add_argument('--db', default=str(Path.home() / 'Cyberdeck/spectre-s3/records.sqlite3'))
    args = parser.parse_args()
    import serial
    Path(args.db).expanduser().parent.mkdir(parents=True, exist_ok=True)
    db = open_database(str(Path(args.db).expanduser()))
    try:
        while True:
            try:
                with serial.Serial(args.port, 115200, timeout=0.1, write_timeout=1) as port:
                    next_ping = 0
                    buffer = bytearray()
                    discard = False
                    while True:
                        if time.monotonic() >= next_ping:
                            port.write(b'CYID_PING\n')
                            next_ping = time.monotonic() + 3
                        for byte in port.read(256):
                            if byte == 10:
                                if not discard and buffer.startswith(b'{'):
                                    try:
                                        response = accept_record(db, buffer.decode('ascii'))
                                        port.write(response.encode('ascii'))
                                    except (ValueError, UnicodeError) as exc:
                                        print(f'Record rejected: {exc}', flush=True)
                                buffer.clear(); discard = False
                            elif not discard:
                                if len(buffer) >= 1024:
                                    discard = True
                                else:
                                    buffer.append(byte)
            except (serial.SerialException, OSError) as exc:
                print(f'Serial disconnected: {exc}; retrying', flush=True)
                time.sleep(2)
    except KeyboardInterrupt:
        pass
    finally:
        db.close()


if __name__ == '__main__':
    main()
