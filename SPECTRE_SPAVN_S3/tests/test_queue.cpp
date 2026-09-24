#include "../firmware/spectre_s3/queue.h"
#include <cassert>
int main() {
  RecordQueue<2> q;
  assert(!q.acknowledge(1));
  assert(q.push({1, 0, 100, 0}));
  assert(q.push({2, 5, 90, 0}));
  assert(!q.push({3, 10, 80, 0}));
  assert(q.dropped == 1 && q.front()->seq == 1);
  assert(!q.acknowledge(2) && q.size() == 2);
  assert(q.acknowledge(1));
  assert(!q.acknowledge(1));
  assert(q.push({4, 15, 70, 1}));
  assert(q.front()->seq == 2);
  assert(q.acknowledge(2) && q.front()->seq == 4);
  assert(q.acknowledge(4) && q.front() == nullptr);
}
