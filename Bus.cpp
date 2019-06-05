#include "Bus.h"
#include <atomic>
#include <iostream>

using namespace std;

Bus::Bus() {
  for (int i = 0; i < 5; i++) {
    this->passengers[i] = -1;
  }
  this->howManyPassengers = -1;
  //isFull.store(false);
  this->actual = 0;                     // start z 0
}

Bus::~Bus() {}
/*
bool Bus::sitInTheBus(int id) {
  if (!this->isFull) {
    busMutex.lock();
    cout << "lock\n";
    this->howManyPassengers++;
    passengers[this->howManyPassengers] = id;
    if (this->howManyPassengers == 4) {
      //this->isFull = true;
    }
    busMutex.unlock();
    return true;
  }
  return false;
}
*/
