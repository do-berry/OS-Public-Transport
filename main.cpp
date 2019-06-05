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

Passenger * passengers [howMany];
thread * threads [howMany + 1];           // pasazerowie + autobus
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

void go() {
  ready.store(true);
  stopped.store(false);
  arrived.store(false);
}

void stop() {
  ready.store(false);
  stopped.store(true);
  arrived.store(true);
}

void * sitInTheBus(int id) {
  while(run) {/*
    unique_lock<mutex> lck(threadMutex);

    // wysiad
    if (arrived.load() == true) {
      if (passengers[id]->goes) {
        outMutex.lock();
        //
        passengers[id]->busStop = actual.load();
        passengers[id]->goes = false;
        //
        if (actual.load() == false) {
          --howManyInTheBus;
          bBusStop++;
          string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
          writeToBus(4, txt);
          writeToB(bBusStop);
        } else {
          --howManyInTheBus;
          string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
          writeToBus(4, txt);
          aBusStop++;
          writeToA(aBusStop);
        }
        outMutex.unlock();
      }
    }
    
    // wsiadanie
    if (actual.load() == passengers[id]->busStop) {
      if (actual.load() == true) {
        if (ready.load() == false && stopped.load() == true) {
          while(actual.load() != false || passengers[id]->goes != true) {
            sitMutex.lock();
            if (howManyInTheBus.load() < 5) {
              ++howManyInTheBus;
              aBusStop--;
              passengers[id]->goes = true;
              passengers[id]->busStop = false;
              //
              string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
              writeToBus(4, txt);
              writeToA(aBusStop);
              //
              if (howManyInTheBus.load() == 5 || aBusStop == 0) {
                actual.store(false);  // -> B
                ready.store(true); 
                stopped.store(false);
              }              
            }
            sitMutex.unlock();
          }
            busCV.notify_one();
            while(ready.load() == true && stopped.load() == false) {
            threadCV.wait(lck);
          }
        } else {
          while(!ready && stopped) {
            threadCV.wait(lck);
          }
        }
      } else {
        if (ready.load() == false && stopped.load() == true) {
          while(stopped.load() == true) {
            sitMutex.lock();
            if (howManyInTheBus.load() < 5) {
              howManyInTheBus++;
              bBusStop--;
              passengers[id]->goes = true;
              passengers[id]->busStop = true;
              //
              string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
              writeToBus(4, txt);
              writeToB(bBusStop);
              //
              if (howManyInTheBus == 5 || bBusStop == 0) {
                actual.store(true);  // -> A
                ready.store(true);
                stopped.store(false);
              }              
            }
            sitMutex.unlock();
          }
          busCV.notify_one();
          while (ready.load() == true && stopped.load() == false) {
            threadCV.wait(lck);
          }
        } else {
          while (!ready && stopped) {
            threadCV.wait(lck);
          }
        }
      }
  } else {
    while(arrived.load() == false) {
      threadCV.wait(lck);
    }
  }*/
  }
}

void * goOrStop() {
  while(run) {
    unique_lock<mutex> lck(busMutex);

    if (actual) {
      if (!ready && stopped) {
        string txt = "Przystanek A\n";
        writeToBus(2, txt);
        threadCV.notify_all();
        while (aBusStop != 0 || !ready) {
          busCV.wait(lck);
        }
        actual.store(false);
        go();
      } else {
        string txt = "W trasie\n";
        writeToBus(2, txt);
        this_thread::sleep_for(chrono::milliseconds(2000));
        stop();
      }
    } else {
      if (!ready && stopped) {
        string txt = "Przystanek B\n";
        writeToBus(2, txt);
        threadCV.notify_all();
        //
        while (bBusStop != 0 || ready.load() == false) {
          busCV.wait(lck);
        }
        actual.store(true);
        go();
      } else {
        string txt = "W trasie\n";
        writeToBus(2, txt);
        this_thread::sleep_for(chrono::milliseconds(2000));
        stop();
      }
    }
  }
}

void init() {
  howManyInTheBus.store(0);
  actual.store(true);
  ready.store(true);
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
  //stop();
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
