#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>
#include <mutex>

#define howManyWindows 3

using namespace std;

class Screen {
private:
    WINDOW * windows[howManyWindows];
    mutex writeMutex;
    void write(int no, int line, string txt);
public:
    Screen();
    ~Screen();
    void writeToA(int x);
    void writeToB(int x);
    void writeToBus(int line, string txt);
};

#endif // SCREEN_H