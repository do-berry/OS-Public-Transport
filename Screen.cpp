#include "Screen.h"
#include <ncurses.h>
#include <mutex>

Screen::Screen() {
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
}

Screen::~Screen() {
  endwin();
}

void Screen::write(int no, int line, string txt) {        // nr okna, nr linii, tekst
  writeMutex.lock();
  mvwaddstr(windows[no], line, 0, txt.c_str());
  wrefresh(windows[no]);
  writeMutex.unlock();
}

void Screen::writeToA(int x) {
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(0, 2, txt);
}

void Screen::writeToBus(int line, string txt) {
  write(1, line, txt);
}

void Screen::writeToB(int x) {
  string txt = "Na przystanku: " + to_string(x) + "\n";
  write(2, 2, txt);
}