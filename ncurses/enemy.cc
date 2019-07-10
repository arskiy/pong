#pragma once
#include "pad.cc"
#include <ncurses.h>

class Enemy : public Pad {
  public:
    int ball_speed;

    int width;
    int height;

    // 1   2 
    //  \|/
    //  /|\
    // 3   4

    int generate_random(int max_num) {
      struct timeval t;
      gettimeofday(&t, NULL);
      pid_t pid = getpid();
      srand(t.tv_usec * t.tv_sec * pid);

      return rand() % max_num;
    }

    int predict(int ball_y, int ball_x, int dir) {
HAHA:
      switch (dir) {
        case 1:
        case 3:
          break;
        case 2:
          while (ball_y > 0) {
            if (ball_x >= width) {
              return ball_y;	
            }
            ball_y--;
            ball_x++;
          }
          dir = 4;
          goto HAHA;
          break;
        case 4:
          while (ball_y < height) {
            if (ball_x >= width) {
              return ball_y;	
            }
            ball_y++;
            ball_x++;
          }
          dir = 2;
          goto HAHA;
      }
      return generate_random(height);
    }


    Enemy(int speed, int hth, int wth) 
      : Pad(hth / 2, wth / 2),
      ball_speed(speed), 
      width(wth), 
      height(hth) {
        x = width - 1;	
      }
};
