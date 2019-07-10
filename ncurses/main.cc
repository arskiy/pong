#include <ncurses.h>
#include <ctime>
#include <sys/ioctl.h>
#include <csignal>
#include <cmath>

#include "pad.cc"
#include "ball.cc"
#include "enemy.cc"
// REMOVE BORDERS AND MAKE GAME FASTER
// BECAUSE IT IS BETER LIKE THAT

// this whole codes uses (y, x) instead of the mathematical standard (x, y).

// select height of border, speed and shit like that
int msleep(unsigned long milisec);
void draw_points();
void winResize(int dummyvar);

bool winsize_change = false;

int player_points = 0, opponent_points = 0, height, width;


int main() {
  // run ncurses functions
  WINDOW *w = initscr();
  raw();				/* Line buffering disabled	*/
  keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
  noecho();			/* Don't echo() while we do getch */
  curs_set(FALSE); // no cursor
  nodelay(w, TRUE);

RESET:
  getmaxyx(stdscr, height, width);

  // draw_border();

  Pad player(height / 2, 1);
  // player.draw();

  Ball ball(height, width);
  // ball.draw();

  Enemy enemy(1, height / 2, width);

  refresh();
  signal(SIGWINCH, winResize);


  // wait for input and quit on 'q' press
  int ch;
  // while  ((ch = getch()) != (int) 'q') {
  while (true) {
    // clear the screen after each loop
    if(winsize_change) {
      msleep(50);
      // since getmaxyx and the calculations after it
      // are costly, we only do it if 'winsize_change' is true.
      getmaxyx(stdscr, height, width);

      ball.height = height;
      ball.width  = width;

      ball.x      = round(width  * ball.relposX);
      ball.y      = round(height * ball.relposY);
      player.y    = round(height * player.relposY);
      enemy.y     = round(height * enemy.relposY);

      winsize_change = false;
    }
    getmaxyx(stdscr, height, width);
    msleep(50);
    clear();

    draw_points();
    // move up, right, down or left depending on the input
    ch = getch();
    if (ch == (int) 'q') {
      break;
    }

    // AI won a point
    if   (ball.x       == 0       && 
        !(player.y     == ball.y || 
          player.y - 1 == ball.y || 
          player.y + 1 == ball.y || 
          player.y + 2 == ball.y || 
          player.y + 3 == ball.y ||
          player.y + 4 == ball.y
         )){

      opponent_points++;
      goto RESET;
    }

    // player won a point
    if   (ball.x      == width  && 
        !(enemy.y     == ball.y || 
          enemy.y + 1 == ball.y || 
          enemy.y - 1 == ball.y || 
          enemy.y + 2 == ball.y || 
          enemy.y + 3 == ball.y)){

      player_points++;
      goto RESET;
    }

    player.move(ch, height);
    player.draw();

    int pred = enemy.predict(ball.y, ball.x, ball.dir);
    if (pred > enemy.y) {
      enemy.y++;
    }
    else if (pred < enemy.y) {
      enemy.y--;
    }

    enemy.draw();
    ball.move();
    ball.draw();

    ball.relposX   = (double) ball.x / width;
    ball.relposY   = (double) ball.y / height;
    player.relposY = (double) player.y / height;
    enemy.relposY  = (double) enemy.y / height;
    enemy.x        = width - 1;
    enemy.width    = width;

    refresh();
  }
  // finish program
  endwin();
  return 0;
}

//---------------------------------------------------------

int msleep(unsigned long milisec) {
  struct timespec req = {0};
  time_t sec = (int) (milisec / 1000);
  milisec = milisec - (sec * 1000);
  req.tv_sec = sec;
  req.tv_nsec=milisec * 1000000L;

  while(nanosleep(&req, &req) == -1)
    continue;   

  return 1;   
}

//---------------------------------------------------------

void draw_points() {
  mvprintw(1, 3, "%d", player_points);
  mvprintw(1, width - 6, "%d", opponent_points);
}
//---------------------------------------------------------

void winResize(int a) {
  // Set flag to indicate window size changed
  winsize_change = true;

  // Update terminal data so that getmaxyx uses the
  // right data
  struct winsize size;
  if(ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
    resize_term(size.ws_row, size.ws_col);
  }
}

//---------------------------------------------------------

/*void draw_border() {
  mvprintw(0, 0, "________________________________________________\n");

  for (int i = 1; i <= height; i++) {
  mvprintw(i, 0, "|                                              |\n");
  }

  mvprintw(height, 0, "\\______________________________________________/");
  }*/
