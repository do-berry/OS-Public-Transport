#ifndef BUS_H
#define BUS_H
#include <mutex>
#include <thread>
#include <atomic>
#include "Screen.h"
#include "Lights.h"
#include <condition_variable>

using namespace std;

class Bus {
private:
  void onTheRoad(Screen *screen, Lights *lights);
public:
  void * go(Screen *screen, Lights *lights, bool *run);
  mutex busMutex, aMutex, bMutex, roadMutex;
  condition_variable busCV, aCV, bCV;
  atomic<bool> atA, atB, onRoad;
  atomic<int> howManyPassengers;
  Bus();
  ~Bus();
};

#endif // BUS_H
