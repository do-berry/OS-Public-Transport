#ifndef BUS_H
#define BUS_H
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

class Bus {
public:
  atomic<int> howManyPassengers;
  Bus();
  ~Bus();
};

#endif // BUS_H
