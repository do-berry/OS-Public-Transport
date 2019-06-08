#include "Bus.h"
#include <atomic>
#include <iostream>

using namespace std;

Bus::Bus() {
  howManyPassengers.store(0);
}

Bus::~Bus() {}
