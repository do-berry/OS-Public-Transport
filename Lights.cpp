#include "Lights.h"
#include <atomic>
#include <thread>
#include "common.h"

Lights::Lights() {
    this->lights = new thread(&Lights::beOn, this);
}

Lights::~Lights() {
    this->lights->join();
    delete lights;
}

void * Lights::beOn() {
  while(run) {
    if (firstLight.load() == true) {
      firstLight.store(false);
      screen->writeToBus(8, "czerwone\n");
      secondLight.store(true);
      screen->writeToBus(10, "zielone\n");
      this_thread::sleep_for(chrono::milliseconds(2500));
    } else {
      firstLight.store(true);
      screen->writeToBus(8, "zielone\n");
      secondLight.store(false);
      screen->writeToBus(10, "czerwone\n");
      this_thread::sleep_for(chrono::milliseconds(2500));
    }
  }
}
