#include "Bus.h"
 
#include <atomic>
#include <thread>
#include <mutex>
#include <iostream>

using namespace std;

Bus::Bus() {
  this->howManyPassengers.store(0);
}

Bus::~Bus() {}

void Bus::onTheRoad(Screen *screen, Lights *lights) {
  screen->writeToBus(2, "W trasie\n");
  unique_lock<mutex> lck(busMutex);
  this_thread::sleep_for(chrono::milliseconds(2000));
  this->onRoad.store(true);
  if (lights->firstLight == false) {
    string txt = "Stoi na czerwonym swietle\n";
    screen->writeToBus(2, txt);
    while (lights->firstLight.load() != true) {
      busCV.wait(lck);
    }
  }
  if (lights->secondLight == false) {
    string txt = "Stoi na czerwonym swietle\n";
    screen->writeToBus(2, txt);
    while (lights->secondLight.load() != true) {
      busCV.wait(lck);
    }
  }
  lck.unlock();
}

void * Bus::go(Screen *screen, Lights *lights, bool *run) {
  while(run) {
  // a
    this->busMutex.lock();
    this->onRoad.store(false);
    this->atA.store(true);
    this->atB.store(false);
    screen->writeToBus(2, "Na przystanku A\n");
    this_thread::sleep_for(chrono::milliseconds(2000));
  //this->aCV.notify_all();
    this->busMutex.unlock();
/*  unique_lock<mutex> aLock(aMutex);
  while (A.load() > 0) {
    aCV.wait(aLock);
  } 
  aLock.unlock();
*/  // trasa
    onTheRoad(screen, lights);
  // b
    this->busMutex.lock();
    this->onRoad.store(false);
    this->atA.store(false);
    this->atB.store(true);
    screen->writeToBus(2, "Na przystanku B\n");
    this_thread::sleep_for(chrono::milliseconds(2000));
//  this->bCV.notify_all();
    this->busMutex.unlock();
/*  unique_lock<mutex> bLock(bMutex);
    while (B.load() > 0) {
      bCV.wait(bLock);
    }
    bLock.unlock();
 */ // trasa
    onTheRoad(screen, lights);
  }
}
