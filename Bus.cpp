#include "Bus.h"
 
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>

using namespace std;

Bus::Bus() {
  this->howManyPassengers.store(0);
  this->atA.store(true);
  this->atB.store(false);
  this->onRoad.store(false);
}

Bus::~Bus() {}
