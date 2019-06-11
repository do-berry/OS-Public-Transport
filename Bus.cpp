#include "Bus.h"
 
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>

using namespace std;

Bus::Bus() {
  this->howManyPassengers.store(0);
  this->atA.store(true);
  this->current.store(0);
  this->atB.store(false);
  this->onRoad.store(false);
  this->empty.store(true);
  this->full.store(false);
}

Bus::~Bus() {}
