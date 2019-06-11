#ifndef PASSENGER_H
#define PASSENGER_H
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

class Passenger {
public:
  bool isAlreadyArrived;
  atomic<bool> travels;
  bool busStop;             // 0: A, 1: B
  atomic<int> current;
  mutex mtx;
  condition_variable cv;
  Passenger();
  Passenger(int id);
  ~Passenger();
};

#endif // PASSENGER_H
