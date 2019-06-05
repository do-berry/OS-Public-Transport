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
static atomic<bool> stopped (true);
mutex busMutex, sitMutex, decMutex, writeMutex;
bool actual = true;
WINDOW * windows[howMany];
static atomic<bool> run (false);
int inTheBus[5] = {-1, -1, -1, -1, -1};
bool fullBus = false;
int howManyInTheBus = 0;
condition_variable busCV, threadCV, cv;
int aBusStop = 0;                 // ile watkow na przystanku A
int bBusStop = 0;                 // ile watkow na przystanku B

void decrement(bool busStop) {
  decMutex.lock();
  if (busStop) {
    aBusStop--;
  } else {
    bBusStop--;
  }
  decMutex.unlock();
}

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

void * sitInTheBus(int id, bool busStop) {
  while(run) {
  unique_lock<mutex> lck(passengers[id]->mtx);
  sitMutex.lock();
  if (actual == busStop && !ready && stopped) {
    if (howManyInTheBus < 5) {
      inTheBus[howManyInTheBus] = id;
      howManyInTheBus++;
      string txt = "W autobusie: " + to_string(howManyInTheBus) + "\n";
      writeToBus(4, txt);
      decrement(busStop);
      if (actual) {
        writeToA(aBusStop);
        if (howManyInTheBus == 5 || aBusStop == 0) {
          ready.store(true);
        }
      } else {
        writeToB(bBusStop);
        if (howManyInTheBus == 5 || bBusStop == 0) {
          ready.store(true);
        }
      }
      busCV.notify_one();
    }
    busCV.notify_one();
  } else {
    while (!ready) {
      threadCV.wait(lck);
    }
  }
  sitMutex.unlock();
}
}

void changeStop() {
  if (actual) {
    actual = false;
  } else {
    actual = true;
  }
}

void * goOrStop() {
  while(run) {
    unique_lock<mutex> lck(busMutex);
  // dopoki przystanek nie jest pusty
  // lub autobus nie jest zapelniony
    if (actual && stopped) {
      threadCV.notify_all();
      string txt = "Przystanek A\n";
      writeToBus(2, txt);
      while (aBusStop != 0 || !ready) {
        busCV.wait(lck);
      }
      stopped.store(false);
      ready.store(false);
    } else if (!actual && stopped) {
      threadCV.notify_all();
      string txt = "Przystanek B\n";
      writeToBus(2, txt);
      while (bBusStop != 0 || !ready) {
        busCV.wait(lck);
    }
      stopped.store(false);
      ready.store(false);
    } else {
    // w trasie do gdzies tam
      changeStop();
      string txt = "W trasie\n";
      writeToBus(2, txt);
      this_thread::sleep_for(chrono::milliseconds(2000));
    }
  }
}

void * doSomething(int id) {
  // jesli autobus jest na tym przystanku, to probuj wsiasc
  // w przeciwnym razie czekaj az przyjedzie
  while(run) {
      sitInTheBus(id, passengers[id]->busStop);
  }
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
    threads[i] = new thread(doSomething, i);
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
  createThreads();
  createBusThread();
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
