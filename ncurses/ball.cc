#include <ncurses.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <random>

class Ball {
  public:
    int x;
    int y;

    double relposX = 0;
    double relposY = 0;

    int height;
    int width;

    // 1   2 
    //  \|/
    //  /|\
    // 3   4
    int dir;

    void draw() {
      // char to represent ball will be *
      mvprintw(y, x, "*");
      refresh();
    }

    int generate_random(int max_num) {
      struct timeval t;
      gettimeofday(&t, NULL);
      pid_t pid = getpid();
      srand(t.tv_usec * t.tv_sec * pid);

      return rand() % max_num;
    }


    void move() {
      // printw("y = %d, h = %d", y, height);
CHECK_DIR:
      switch (dir) {
        case 1:
          // temporary. we need to check scores before this
          if (x == 0) {
            dir = 2;
            goto CHECK_DIR;
          }

          if (y == 0) {
            dir = 3;
            goto CHECK_DIR;
          }
          y--;
          x--;
          break;
        case 2:
          if (x == width) {
            dir = 1;
            goto CHECK_DIR;
          }

          if (y == 0) {
            dir = 4;
            goto CHECK_DIR;
          }
          y--;
          x++;
          break;
        case 3:
          if (x == 0) {
            dir = 4;
            goto CHECK_DIR;
          }

          if (y == height) {
            dir = 1;
            goto CHECK_DIR;
          }
          y++;
          x--;
          break;
        case 4:
          if (x == width) {
            dir = 3;
            goto CHECK_DIR;
          }

          if (y == height) {
            dir = 2;
            goto CHECK_DIR;
          }
          y++;
          x++;
          break;
      }
    }

    Ball(int height, int width) : 
      x(width / 2), 
      y(height / 2), 
      height(height), 
      width(width) 
  {
    dir = generate_random(4);
    while (dir == 0) {
      dir = generate_random(4);
    }
  }
};
