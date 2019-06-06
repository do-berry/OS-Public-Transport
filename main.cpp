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

using namespace std;

#define howMany 10

/*
  actual, stopped : B
  ready : P
*/

Passenger * passengers [howMany];
thread * threads [howMany + 2];           // pasazerowie + autobus + swiatla
Bus * bus;
static atomic<bool> ready (false);
static atomic<bool> stopped (false);
static atomic<bool> arrived(false);
mutex busMutex, sitMutex, decMutex, writeMutex, threadMutex, outMutex;
static atomic<bool> actual (true);
WINDOW * windows[howMany];
static atomic<bool> run (false);
int inTheBus[5] = {-1, -1, -1, -1, -1};
bool fullBus = false;
atomic<bool> firstLight (false);
atomic<bool> secondLight (true);
atomic<int> howManyInTheBus {0};
condition_variable busCV, threadCV, cv;
int aBusStop = 0;                 // ile watkow na przystanku A
int bBusStop = 0;                 // ile watkow na przystanku B

void write(int no, int line, string txt) {        // nr okna, nr linii, tekst
  writeMutex.lock();
  mvwaddstr(windows[no], line, 0, txt.c_str());
  wrefresh(windows[no]);
  writeMutex.unlock();
}

void writeToA(int x) {
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(0, 2, txt);
}

void writeToBus(int line, string txt) {
  write(1, line, txt);
}

void writeToB(int x) {
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(2, 2, txt);
}

void go(bool value) {
  actual.store(value);
  // ready.store(true);
  stopped.store(false);
  // arrived.store(false);
}

void stop(bool value) {
  actual.store(value);
  // ready.store(false);
  stopped.store(true);
//  arrived.store(true);
}

void * lights() {
  while(run) {
    if (firstLight.load() == true) {
      firstLight.store(false);
      writeToBus(8, "czerwone\n");
      secondLight.store(true);
      writeToBus(10, "zielone\n");
      if (stopped.load() == false) {
        busCV.notify_one();
      }
      this_thread::sleep_for(chrono::milliseconds(2500));
    } else {
      firstLight.store(true);
      writeToBus(8, "zielone\n");
      secondLight.store(false);
      writeToBus(10, "czerwone\n");
      if (stopped.load() == false) {
        busCV.notify_one();
      }
      this_thread::sleep_for(chrono::milliseconds(2500));
    }
  }
}

void * sitInTheBus(int id) {/*
  unique_lock<mutex> lck(threadMutex);
  while(run) {
    // jesli jest na naszym przystanku
    // jesli (goes) -> false
    // : wysiadka
    if (actual.load() == passengers[id]->busStop) {
      if (passengers[id]->goes) {
        --howManyInTheBus;
        passengers[id]->goes = false;
        if (actual.load() == true) {
          ++bBusStop;
          writeToB(bBusStop);
        } else {
          ++aBusStop;
          writeToA(aBusStop);
        }
        string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
        writeToBus(4, txt);
      }
      // probuje wsiasc
      // dopoki autobus stoi na przystanku
      while (stopped.load() == true) {
        // sprawdza czy w autobusie jest miejsce
        // jesli jest to wsiada
        // jesli nie to zmieniamy ready -> true
        sitMutex.lock();
        if (howManyInTheBus < 5) {
          ++howManyInTheBus;
          if (actual.load() == true) {
            --aBusStop;
            passengers[id]->busStop = false;
          } else {
            --bBusStop;
            passengers[id]->busStop = true;
          }
          passengers[id]->goes = true;
        }
        sitMutex.unlock();
        if (actual.load() == true) {
          if (howManyInTheBus == 5 || aBusStop == 0) {
            ready.store(true);
            busCV.notify_one();
          }
        } else {
          if (howManyInTheBus == 5 || bBusStop == 0) {
            ready.store(true);
            busCV.notify_one();
          }
        }
        }
      } else {
        while (ready.load() == true) {
          threadCV.wait(lck);
        }
      }
    }*/
}

void * goOrStop() {
  while(run) {
    unique_lock<mutex> lck(busMutex);

    if (actual) {
      if (!ready && stopped) {
        string txt = "Przystanek A\n";
        writeToBus(2, txt);
        threadCV.notify_all();
        while (/*aBusStop != 0 || */ready.load() == false) {
          busCV.wait(lck);
        }
        go(false);
      } else {
        string txt = "W trasie\n";
        writeToBus(2, txt);
        this_thread::sleep_for(chrono::milliseconds(1000));
        if (firstLight == false) {
          txt = "Stoi na czerwonym swietle\n";
          writeToBus(2, txt);
          while (firstLight.load() != true) {
            busCV.wait(lck);
          }
        }
        if (secondLight == false) {
          txt = "Stoi na czerwonym swietle\n";
          writeToBus(2, txt);
          while (secondLight.load() != true) {
            busCV.wait(lck);
          }
        }
        stop(true);
      }
    } else {
      if (!ready && stopped) {
        string txt = "Przystanek B\n";
        writeToBus(2, txt);
        threadCV.notify_all();
        //
        while (/*bBusStop != 0 || */ready.load() == false) {
          busCV.wait(lck);
        }
        go(true);
      } else {
        string txt = "W trasie\n";
        writeToBus(2, txt);
        this_thread::sleep_for(chrono::milliseconds(1000));
        if (firstLight == false) {
          txt = "Stoi na czerwonym swietle\n";
          writeToBus(2, txt);
          while (firstLight.load() != true) {
            busCV.wait(lck);
          }
        }
        if (secondLight == false) {
          txt = "Stoi na czerwonym swietle\n";
          writeToBus(2, txt);
          while (secondLight.load() != true) {
            busCV.wait(lck);
          }
        }
        stop(false);
      }
    }
  }
}

void init() {
  howManyInTheBus.store(0);
  actual.store(true);
  ready.store(false);
  stopped.store(false);
  //arrived.store(false);
}

void createThreads() {
  for (int i = 0; i < howMany; i++) {
    passengers[i] = new Passenger();
    if (passengers[i]->busStop) {
      aBusStop++;
      writeToA(aBusStop);
    } else {
      bBusStop++;
      writeToB(bBusStop);
    }
    threads[i] = new thread(sitInTheBus, i);
  }
}

void createBusThread() {
  bus = new Bus();
  threads[howMany] = new thread(goOrStop);
}

void createLights() {
  threads[howMany + 1] = new thread(lights);
}

void stopThreads() {
  for (int i = 0; i < howMany; i++) {
    threads[i]->join();
    delete threads[i];
    delete passengers[i];
  }
  delete threads[howMany];
  delete bus;
}

void start() {
  int x, y;
  initscr();
  refresh();
  getmaxyx(stdscr, y, x);
  x /= 3;
  windows[0] = newwin(y, x, 0, 0);
  windows[1] = newwin(y, x, 0, x);
  windows[2] = newwin(y, x, 0, 2*x);
  mvwaddstr(windows[0], 0, 0, "Przystanek A\n");
  wrefresh(windows[0]);
  mvwaddstr(windows[2], 0, 0, "Przystanek B\n");
  wrefresh(windows[2]);
  mvwaddstr(windows[1], 0, 0, "Autobus\n");
  wrefresh(windows[1]);
  char choice = 0;
  init();
  createLights();
  createBusThread();
  createThreads();
  while (run) {
    cin >> choice;
    if (choice == 'n') {
      run.store(false);
      stopThreads();
    }
  }
  endwin();
}

int main() {
  srand(time(NULL));
  run.store(true);
  start();
  return 0;
}
