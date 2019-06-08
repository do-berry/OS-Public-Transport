#include <atomic>
#include "Screen.h"

using namespace std;

#define howMany 10

atomic<bool> run (false);                 // dzialanie programu
atomic<int> A {0}, B{0};                  // ile watkow na przystanku A/B
Screen * screen = new Screen();           // ncurses
