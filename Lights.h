#ifndef LIGHTS_H
#define LIGHTS_H

#include <thread>
#include <atomic>

using namespace std;

class Lights {
public:
    Lights();
    ~Lights();
    atomic<bool> firstLight;          // pierwsze swiatla na trasie
    atomic<bool> secondLight;         // drugie swiatla na trasie
    thread * lights;
    void * beOn();
};

#endif // LIGHTS_H