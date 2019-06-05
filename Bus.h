#ifndef BUS_H
#define BUS_H
#include <mutex>
#include <thread>
#include <atomic>

using namespace std;

class Bus {
public:
  Bus();
  ~Bus();
  int howManyPassengers;
  int passengers [5];
  static atomic<bool> isFull;
  mutex busMutex;
  int actual;                 // 0 : A, 1 : B, 2 : w drodze
  //bool sitInTheBus(int id);
};

#endif // BUS_H
