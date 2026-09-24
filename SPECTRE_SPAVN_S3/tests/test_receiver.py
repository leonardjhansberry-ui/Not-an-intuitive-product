import json
import tempfile
import unittest
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / 'cyid'))
from receiver import open_database, accept_record


class ReceiverTests(unittest.TestCase):
    def setUp(self):
        self.directory = tempfile.TemporaryDirectory()
        self.path = Path(self.directory.name) / 'records.sqlite3'
        self.db = open_database(self.path)
        self.record = dict(v=1, device='001122aabbcc', boot='0'*31+'1', seq=1,
                           type='status', uptime_ms=5000, clock='unsynced', heap_bytes=100000, dropped=0)

    def tearDown(self):
        self.db.close()
        self.directory.cleanup()

    def test_retry_is_deduplicated_and_persists(self):
        first = accept_record(self.db, json.dumps(self.record))
        self.assertEqual(first, accept_record(self.db, json.dumps(self.record)))
        self.assertTrue(first.startswith('ACK 001122aabbcc '))
        self.db.close()
        self.db = open_database(self.path)
        self.assertEqual(self.db.execute('SELECT count(*) FROM records').fetchone()[0], 1)

    def test_conflicting_replay_rejected(self):
        accept_record(self.db, json.dumps(self.record))
        self.record['heap_bytes'] = 1
        with self.assertRaises(ValueError): accept_record(self.db, json.dumps(self.record))

    def test_reboot_sequence_is_separate(self):
        accept_record(self.db, json.dumps(self.record))
        self.record['boot'] = 'a'*32
        accept_record(self.db, json.dumps(self.record))
        self.assertEqual(self.db.execute('SELECT count(*) FROM records').fetchone()[0], 2)

    def test_invalid_data_not_stored(self):
        for line in ['[]', '{', '{}', 'x'*1025, json.dumps({**self.record, 'seq': True})]:
            with self.assertRaises(ValueError): accept_record(self.db, line)
        self.assertEqual(self.db.execute('SELECT count(*) FROM records').fetchone()[0], 0)

    def test_storage_failure_does_not_ack(self):
        self.db.execute('PRAGMA query_only=ON')
        import sqlite3
        with self.assertRaises(sqlite3.OperationalError): accept_record(self.db, json.dumps(self.record))

if __name__ == '__main__': unittest.main()
