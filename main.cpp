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

atomic<bool> run(false);
atomic<int> A{0}, B{0};
Bus *bus = new Bus();
thread *busThread;
thread *lights;
WINDOW *windows[3];
mutex writeMutex;
mutex busMutex, aMutex, bMutex, roadMutex;
condition_variable busCV, aCV, bCV;
atomic<bool> firstLight;
atomic<bool> secondLight;

// screen
void write(int no, int line, string txt)
{
  writeMutex.lock();
  mvwaddstr(windows[no], line, 0, txt.c_str());
  wrefresh(windows[no]);
  writeMutex.unlock();
}

void writeToA(int x)
{
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(0, 2, txt);
}

void writeToBus(int line, string txt)
{
  write(1, line, txt);
}

void writeToB(int x)
{
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(2, 2, txt);
}

// bus
void onTheRoad()
{
  writeToBus(2, "W trasie\n");
  this_thread::sleep_for(chrono::milliseconds(2000));
  bus->onRoad.store(true);
  unique_lock<mutex> lck(busMutex);
  if (firstLight == false)
  {
    string txt = "Stoi na czerwonym swietle\n";
    writeToBus(2, txt);
    while (firstLight.load() != true)
    {
      busCV.wait(lck);
    }
  }
  if (secondLight == false)
  {
    string txt = "Stoi na czerwonym swietle\n";
    writeToBus(2, txt);
    while (secondLight.load() != true)
    {
      busCV.wait(lck);
    }
  }
  lck.unlock();
}

void *go()
{
  while (run)
  {
    // a
    busMutex.lock();
    bus->onRoad.store(false);
    bus->atA.store(true);
    bus->atB.store(false);
    writeToBus(2, "Na przystanku A\n");
    this_thread::sleep_for(chrono::milliseconds(2000));
    aCV.notify_all();
    busMutex.unlock();
    unique_lock<mutex> aLock(aMutex);
    while (A.load() > 0)
    {
      aCV.wait(aLock);
    }
    aLock.unlock();
    // trasa
    onTheRoad();
    // b
    busMutex.lock();
    bus->onRoad.store(false);
    bus->atA.store(false);
    bus->atB.store(true);
    writeToBus(2, "Na przystanku B\n");
    this_thread::sleep_for(chrono::milliseconds(2000));
    bCV.notify_all();
    busMutex.unlock();
    unique_lock<mutex> bLock(bMutex);
    while (B.load() > 0)
    {
      bCV.wait(bLock);
    }
    bLock.unlock();
    // trasa
    onTheRoad();
  }
}

// lights
void *beOn()
{
  while (run)
  {
    if (firstLight.load() == true)
    {
      firstLight.store(false);
      secondLight.store(true);
      writeToBus(8, "czerwone\n");
      writeToBus(10, "zielone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      roadMutex.lock();
      if (bus->onRoad.load() == true)
      {
        busCV.notify_one();
      }
      roadMutex.unlock();
    }
    else
    {
      firstLight.store(true);
      secondLight.store(false);
      writeToBus(8, "zielone\n");
      writeToBus(10, "czerwone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      roadMutex.lock();
      if (bus->onRoad.load() == true)
      {
        busCV.notify_one();
      }
      roadMutex.unlock();
    }
  }
}

// threads
void createThreads()
{
  busThread = new thread(&go);
  lights = new thread(&beOn);
}

void stopThreads()
{
  busThread->join();
  delete busThread;
  lights->join();
  delete lights;
  delete bus;
}

// screen
void init()
{
  int x, y;
  initscr();
  refresh();
  getmaxyx(stdscr, y, x);
  x /= 3;
  windows[0] = newwin(y, x, 0, 0);
  windows[1] = newwin(y, x, 0, x);
  windows[2] = newwin(y, x, 0, 2 * x);
  mvwaddstr(windows[0], 0, 0, "Przystanek A\n");
  wrefresh(windows[0]);
  mvwaddstr(windows[2], 0, 0, "Przystanek B\n");
  wrefresh(windows[2]);
  mvwaddstr(windows[1], 0, 0, "Autobus\n");
  wrefresh(windows[1]);
  char choice = 0;
  createThreads();
  while (run)
  {
    cin >> choice;
    if (choice == 'n')
    {
      run.store(false);
      stopThreads();
    }
  }
  endwin();
}

int main()
{
  srand(time(NULL));
  run.store(true);
  init();
  return 0;
}
