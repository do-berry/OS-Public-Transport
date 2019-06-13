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

#define howMany 10

using namespace std;

Passenger *passengers[howMany];
Bus *bus = new Bus();
thread *busThread, *lights, *passengerThreads[howMany];
WINDOW *windows[3];

mutex writeMutex;
mutex busMutex, stopMutex, roadMutex;
condition_variable busStatusCV, passengerCV, roadCV;

atomic<bool> firstLight;
atomic<bool> secondLight;
atomic<bool> run(false);
atomic<int> A{0}, B{0}, current{0};

// screen
void write(int no, int line, string txt)
{
  writeMutex.lock();
  mvwaddstr(windows[no], line, 0, txt.c_str());
  wrefresh(windows[no]);
  writeMutex.unlock();
}

void writeToA(int line, int x)
{
  if (line == -1)
  {
    write(0, x + 4, "\n");
  }
  else
  {
    string txt = "Watek " + to_string(x) + "\n";
    write(0, x + 4, txt);
  }
}

void writeToBus(int line, string txt)
{
  write(1, line, txt);
}

void writeToB(int line, int x)
{
  if (line == -1)
  {
    write(2, x + 4, "\n");
  }
  else
  {
    string txt = "Watek " + to_string(x) + "\n";
    write(2, x + 4, txt);
  }
}

// bus
void onTheRoad()
{
  writeToBus(1, "W trasie\n");
  this_thread::sleep_for(chrono::milliseconds(2000));
  bus->onRoad.store(true);
  bus->atA.store(false);
  bus->atB.store(false);
  unique_lock<mutex> lck(busMutex);
  if (firstLight == false)
  {
    string txt = "Stoi na czerwonym swietle\n";
    writeToBus(1, txt);
    while (firstLight.load() != true)
    {
      roadCV.wait(lck);
    }
  }
  if (secondLight == false)
  {
    string txt = "Stoi na czerwonym swietle\n";
    writeToBus(1, txt);
    while (secondLight.load() != true)
    {
      roadCV.wait(lck);
    }
  }
  lck.unlock();
}

void onTheAStop()
{
  busMutex.lock();
  bus->onRoad.store(false);
  bus->atA.store(true);
  bus->atB.store(false);
  bus->current.store(0);
  writeToBus(1, "Na przystanku A\n");
  this_thread::sleep_for(chrono::milliseconds(3000));
  passengerCV.notify_all();
  busMutex.unlock();
}

void waitForBusStatus()
{
  unique_lock<mutex> statusMutex(stopMutex);
  while (bus->empty.load() == false)
  {
    busStatusCV.wait(statusMutex); // tu ma byc pusty
  }
  passengerCV.notify_all();
  this_thread::sleep_for(chrono::milliseconds(1000));
  while (bus->full.load() == false)
  {
    busStatusCV.wait(statusMutex); // tu ma byc pelny
  }
  passengerCV.notify_all();
}

void onTheBStop()
{
  busMutex.lock();
  bus->onRoad.store(false);
  bus->atA.store(false);
  bus->atB.store(true);
  bus->current.store(1);
  writeToBus(1, "Na przystanku B\n");
  this_thread::sleep_for(chrono::milliseconds(2000));
  passengerCV.notify_all();
  busMutex.unlock();
}

void go()
{
  while (run)
  {
    // -> a
    onTheAStop();

    // czeka na pusty
    waitForBusStatus();

    // trasa
    onTheRoad();

    // -> b
    onTheBStop();

    // czeka na pusty
    waitForBusStatus();

    // trasa
    onTheRoad();
  }
}

// lights
void beOn()
{
  while (run)
  {
    writeToBus(15, "Pierwszy przejazd:\n");
    writeToBus(17, "Drugi przejazd:\n");
    if (firstLight.load() == true)
    {
      firstLight.store(false);
      secondLight.store(true);
      writeToBus(16, "czerwone\n");
      writeToBus(18, "zielone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      roadMutex.lock();
      if (bus->onRoad.load() == true)
      {
        roadCV.notify_one();
      }
      roadMutex.unlock();
    }
    else
    {
      firstLight.store(true);
      secondLight.store(false);
      writeToBus(16, "zielone\n");
      writeToBus(18, "czerwone\n");
      this_thread::sleep_for(chrono::milliseconds(3000));
      roadMutex.lock();
      if (bus->onRoad.load() == true)
      {
        roadCV.notify_one();
      }
      roadMutex.unlock();
    }
  }
}

void waitForAStop(int id)
{
  unique_lock<mutex> stop(busMutex);
  while (bus->atA.load() == false)
  {
    passengerCV.wait(stop); // 1
  }
  busStatusCV.notify_one();
}

void getOut(int id)
{
  unique_lock<mutex> getOut(busMutex);
  while (bus->empty.load() == false)
  {
    if (passengers[id]->travels.load() == true && passengers[id]->current.load() != bus->current.load())
    {
      passengers[id]->travels.store(false);
      --bus->howManyPassengers;
      passengers[id]->current.store(bus->current.load());
      if (bus->atA.load() == true)
      {
        ++A;
        writeToA(id, id);
      }
      else
      {
        ++B;
        writeToB(id, id);
      }
      writeToBus(id + 4, "\n");
      if (bus->howManyPassengers.load() == 0)
      {
        bus->empty.store(true);
        bus->full.store(false);
      }
    }
    passengerCV.wait(getOut); // 2
  }
  busStatusCV.notify_one();
}

void waitForBStop(int id)
{
  unique_lock<mutex> stop(busMutex);
  while (bus->atB.load() == false)
  {
    passengerCV.wait(stop); // 1
  }
  busStatusCV.notify_one();
}

void getIn(int id)
{
  unique_lock<mutex> getIn(busMutex);
  while (bus->full == false)
  {
    if (bus->howManyPassengers.load() < 5 && passengers[id]->current.load() == bus->current.load())
    {
      if (passengers[id]->travels.load() == false)
      {
        passengers[id]->travels.store(true);
        ++bus->howManyPassengers;
        if (bus->atA.load() == true)
        {
          --A;
          writeToA(-1, id);
        }
        else
        {
          --B;
          writeToB(-1, id);
        }
        writeToBus(id + 4, "Watek " + to_string(id) + "\n");
      }
      if (bus->howManyPassengers == 5)
      {
        bus->full.store(true);
        bus->empty.store(false);
      }
    }
    passengerCV.wait(getIn); // 3
  }
  busStatusCV.notify_one();
}

// passengers
void *travel(int id)
{
  while (run)
  {
    // czekanie na swoj przystanek
    waitForAStop(id);

    // wysiadanie
    getOut(id);

    // zaladowanie
    getIn(id);

    // czekanie na B
    waitForBStop(id);

    // wysiadanie
    getOut(id);

    // wsiadanie
    getIn(id);
  }
}

// threads
void createThreads()
{
  busThread = new thread(&go);
  lights = new thread(&beOn);
  for (int i = 0; i < howMany; i++)
  {
    passengers[i] = new Passenger(i);
    passengerThreads[i] = new thread(&travel, i);
    ++A;
    writeToA(i, i);
  }
}

void stopThreads()
{
  for (int i = 0; i < howMany; i++)
  { 
    passengerThreads[i]->join();
    delete passengerThreads[i];
    delete passengers[i];
  }
  busThread->join();
  lights->join();
  delete busThread;
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
  while (run.load() == true)
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
