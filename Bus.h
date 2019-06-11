#ifndef BUS_H
#define BUS_H
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>

using namespace std;

class Bus {
public:
  atomic<bool> atA, atB, onRoad, empty, full;
  atomic<int> howManyPassengers, current;
  Bus();
  ~Bus();
};

#endif // BUS_H
