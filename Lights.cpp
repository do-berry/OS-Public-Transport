#include "Lights.h"
#include "Bus.h"
#include "Screen.h"
#include <atomic>
#include <thread>
 

Lights::Lights() {}

Lights::~Lights() {}

void * Lights::beOn(Screen *screen, Bus *bus, bool *run) {
  while(run) {
    if (firstLight.load() == true) {
      firstLight.store(false);
      screen->writeToBus(8, "czerwone\n");
      secondLight.store(true);
      screen->writeToBus(10, "zielone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      if (bus->onRoad.load() == true) {
        bus->busCV.notify_one();
      }
    } else {
      firstLight.store(true);
      screen->writeToBus(8, "zielone\n");
      secondLight.store(false);
      screen->writeToBus(10, "czerwone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      if (bus->onRoad.load() == true) {
        bus->busCV.notify_one();
      }
    }
  }
}
