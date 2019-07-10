#pragma once
#include <ncurses.h>

class Pad {
  protected:
    const int speed = 1;
  public:
    int x = 0;
    int y = 0;
    const int size = 4;

    double relposY = 0;

    void move(int dir, int height) {

      if (dir == KEY_UP && y > 0 + 1) {
        y--;
      }
      else if (dir == KEY_DOWN && y < height - size) {
        y++;
      }
    }

    void draw() {
      // print the player, in bold
      attron(A_BOLD);
      for (int i = 0; i < size; i++) {
        mvprintw(y + i, x, "|");
      }
      attroff(A_BOLD);
      refresh();
    }

    Pad(int ya, int xa) {
      y = ya;
      x = xa;
    }
};
