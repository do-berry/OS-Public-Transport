#include "Passenger.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;

Passenger::Passenger() {
  this->isAlreadyArrived = false;
  this->busStop = rand() % 2;
  this->goes = false;
}

Passenger::~Passenger() {}
