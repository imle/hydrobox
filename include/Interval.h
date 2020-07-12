#ifndef HPCC_INTERVAL_H
#define HPCC_INTERVAL_H

struct Interval {
  const unsigned long PUB_INTERVAL;
  unsigned long last_pub_ms;
};

#endif //HPCC_INTERVAL_H
