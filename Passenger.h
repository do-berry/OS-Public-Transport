#ifndef PASSENGER_H
#define PASSENGER_H
#include <mutex>
#include <condition_variable>
#include <atomic>

using namespace std;

class Passenger {
public:
  atomic<bool> travels;
  atomic<int> current;
  Passenger();
  Passenger(int id);
  ~Passenger();
};

#endif // PASSENGER_H
