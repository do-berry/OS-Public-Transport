#ifndef PASSENGER_H
#define PASSENGER_H
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

class Passenger {
public:
  bool isAlreadyArrived;
  bool goes;
  bool inTheBus;
  bool busStop;             // true: A; false: B
  mutex mtx;
  condition_variable cv;
  Passenger();
  ~Passenger();
};

#endif // PASSENGER_H
