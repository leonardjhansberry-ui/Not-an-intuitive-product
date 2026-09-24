#pragma once
#include <stdint.h>
#include <stddef.h>
struct Record { uint32_t seq; uint64_t uptime_ms; uint32_t heap; uint32_t dropped; };
template<size_t N> class RecordQueue {
  Record records[N]; size_t head = 0, count = 0;
public:
  uint32_t dropped = 0;
  bool push(Record r) {
    if (count == N) { ++dropped; return false; }
    records[(head + count) % N] = r; ++count; return true;
  }
  const Record* front() const { return count ? &records[head] : nullptr; }
  bool acknowledge(uint32_t seq) {
    if (!count || records[head].seq != seq) return false;
    head = (head + 1) % N; --count; return true;
  }
  size_t size() const { return count; }
};
