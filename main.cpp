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
 
using namespace std;

atomic<bool> run (false);
atomic<int> A{0}, B{0};
Screen * screen = new Screen();
//Passenger * passengers [howMany];         // pasazerowie
Bus * bus = new Bus();                                // autobus
thread * threads [2];               // pasazerowie + autobus
Lights * lights = new Lights();                          // swiatla

/*
void * travel(int id) {
  // todo
}

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
*/
void createBus() {
  threads[0] = new thread(&bus->go, screen, lights, run);
  threads[1] = new thread(&lights->beOn, screen, bus, run);
}

void stop() {
  /*for (int i = 0; i < howMany + 1; i++) {
    threads[i]->join();
    delete threads[i];
    if (i < howMany) {
      delete passengers[i];
    }
  }*/
  run.store(false);
  threads[0]->join();
  threads[1]->join();
  delete bus;
  delete lights;
  delete screen;
}

int main() {
  srand(time(NULL));
  char choice = 0;
  run.store(true);
  createBus();
  //createPassengers();
  while (run) {
    cin >> choice;
    if (choice == 'n') {
      stop();
    }
  }
  return 0;
}
