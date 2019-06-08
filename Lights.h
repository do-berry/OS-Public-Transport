#ifndef LIGHTS_H
#define LIGHTS_H

#include "Screen.h"
#include "Bus.h"
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

class Lights {
private:
    mutex roadMutex;
public:
    Lights();
    ~Lights();
    atomic<bool> firstLight;          // pierwsze swiatla na trasie
    atomic<bool> secondLight;         // drugie swiatla na trasie
    void * beOn(Screen *screen, Bus *bus, bool *run);
};

#endif // LIGHTS_H