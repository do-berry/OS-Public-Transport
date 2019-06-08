#include <iostream>
#include <mutex>
#include <ctime>
#include <condition_variable>
#include <thread>
#include <string>
#include <chrono>
#include <ncurses.h>
#include <atomic>
#include "Passenger.h"
#include "Bus.h"
#include "Screen.h"
#include "Lights.h"
#include "common.h"

using namespace std;

Passenger * passengers [howMany];         // pasazerowie
Bus * bus;                                // autobus
thread * threads [howMany + 1];           // pasazerowie + autobus
Lights * lights;                          // swiatla

void * travel(int id) {
  // todo
}

void * go() {
  // todo
}

/* 
    tworzenie, usuwanie watkow & main
*/

void createPassengers() {
  for (int i = 0; i < howMany; i++) {
    passengers[i] = new Passenger();
    if (passengers[i]->busStop) {
      ++A;
      screen->writeToA(A);
    } else {
      ++B;
      screen->writeToB(B);
    }
    threads[i] = new thread(travel, i);
  }
}

void createBus() {
  bus = new Bus();
  threads[howMany] = new thread(go);
}

void createLights() {
  lights = new Lights();
}

void stopThreads() {
  for (int i = 0; i < howMany + 1; i++) {
    threads[i]->join();
    delete threads[i];
    if (i < howMany) {
      delete passengers[i];
    }
  }
  delete bus;
  delete lights;
}

int main() {
  srand(time(NULL));
  char choice = 0;
  run.store(true);
  createLights();
  createBus();
  createPassengers();
  while (run) {
    cin >> choice;
    if (choice == 'n') {
      run.store(false);
      stopThreads();
      delete screen;
    }
  }
  return 0;
}
