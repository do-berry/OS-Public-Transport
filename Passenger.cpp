#include "Passenger.h"
 
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

Passenger::Passenger() {}

Passenger::Passenger(int id) {
  this->isAlreadyArrived = false;
  this->current.store(0);
  this->travels.store(false);

}

Passenger::~Passenger() {}
