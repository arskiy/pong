#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <GL/freeglut.h>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
const int interval = 1000 / 100; // fps

// score
int score_left = 0;
int score_right = 0;

// ball
float ball_pos_x = SCR_WIDTH / 2;
float ball_pos_y = SCR_HEIGHT / 2;
float ball_dir_x = -1.0f;
float ball_dir_y = 0.0f;
const int ball_size = 8;
const int ball_speed = 9;

// rackets in general
const int racket_width = 10;
const int racket_height = 80;
const int racket_speed = 8;

// left racket
float racket_left_x = 10.0f;
float racket_left_y = SCR_HEIGHT / 2;

// right racket
float racket_right_x = SCR_WIDTH - racket_width - 10;
float racket_right_y = SCR_HEIGHT / 2;

void draw(void);
void update(int value);
void drawText(float x, float y, char* text);
void drawRect(float x, float y, float SCR_WIDTH, float SCR_HEIGHT);
void dotted_line();
char int2str(int x);
void keyboard(unsigned char key, int x, int y);
void updateBall();
int predict();
int generate_random(int max_num);

// call a bunch of GL functions
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

  glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
  glutInitWindowPosition(100, 100);

  glutCreateWindow("Pong");

  glutDisplayFunc(draw);
  glutKeyboardFunc(keyboard);

  glutTimerFunc(interval, update, 0);

  // start the whole thing

  glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0.0f, SCR_WIDTH, 0.0f, SCR_HEIGHT, 0.0f, 1.0f);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();

  glColor3f(1.0f, 1.0f, 1.0f);
  glutMainLoop();

  return 0;
}

// render loop
void draw(void) {
  // clear (has to be done at the beginning)
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  char score_str[5];
  
  sprintf(score_str, "%d : %d", score_left, score_right);
  drawText(SCR_WIDTH / 2 - 10, SCR_HEIGHT - 15, score_str); 

  drawRect(racket_left_x, racket_left_y, racket_width, racket_height);
  drawRect(racket_right_x, racket_right_y, racket_width, racket_height);
  drawRect(ball_pos_x - ball_size / 2, ball_pos_y - ball_size / 2, ball_size, ball_size);

  dotted_line();

  // swap buffers (has to be done at the end)
  glutSwapBuffers();
}

// this function will be called every X (fps/interval) seconds
void update(int value) {
  glutTimerFunc(interval, update, 0);

  updateBall();

  // get the Y pos where the ball will be when it reaches the right wall
  int prediction = predict();

  // add a bit of randomness on where it will land on the racket
  if (ball_pos_x == SCR_WIDTH - 20) {
    prediction -= generate_random(40);
  }

  printf("prediction: %d\n", prediction);

  // do nothing if it is close enough to the prediction.
  // (yes ik this an awful way of doing it)
  if ((racket_right_y <= prediction + racket_speed) && (racket_right_y >= prediction - racket_speed)) {}
  // move down towards the prediction
  else if (racket_right_y > prediction) {
    racket_right_y -= racket_speed;
  }
  // move up
  else if (racket_right_y < prediction) {
    racket_right_y += racket_speed;
  }

  // Redisplay frame
  glutPostRedisplay();
}

// self-explanatory.
void drawText(float x, float y, char* text) {
  glRasterPos2f(x, y);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*) text );
}

// self-explanatory.
char int2str(int x) {
  return (char) (x + 48);
}

// self-explanatory.
void drawRect(float x, float y, float width, float height) {
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
}

// this will get 'n' check our input
void keyboard(unsigned char key, int dummy1, int dummy2) {
  switch(key) {
    case 'w':
      if (racket_left_y >= SCR_HEIGHT) {
        break;
      }
      racket_left_y += racket_speed;
      break;
    case 's':
      if (racket_left_y <= 0) {
        break;
      }
      racket_left_y -= racket_speed;
      break;
  }	
}

void updateBall() {
  // fly a bit
  ball_pos_x += ball_dir_x * ball_speed;
  ball_pos_y += ball_dir_y * ball_speed;

  // hit by left racket?
  if (ball_pos_x < racket_left_x + racket_width && 
      ball_pos_x > racket_left_x &&
      ball_pos_y < racket_left_y + racket_height &&
      ball_pos_y > racket_left_y) {
    // set fly direction depending on where it hit the racket
    // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
    float t = ((ball_pos_y - racket_left_y) / racket_height) - 0.5f;
    ball_dir_x = fabs(ball_dir_x); // force it to be positive
    ball_dir_y = t;
  }

  // hit by right racket?
  if (ball_pos_x > racket_right_x && 
      ball_pos_x < racket_right_x + racket_width &&
      ball_pos_y < racket_right_y + racket_height &&
      ball_pos_y > racket_right_y) {
    // set fly direction depending on where it hit the racket
    // (t is 0.5 if hit at top, 0 at center, -0.5 at bottom)
    float t = ((ball_pos_y - racket_right_y) / racket_height) - 0.5f;
    ball_dir_x = -fabs(ball_dir_x); // force it to be negative
    ball_dir_y = t;
  }

  // hit left wall?
  if (ball_pos_x < 0) {
    ++score_right;
    ball_pos_x = SCR_WIDTH / 2;
    ball_pos_y = SCR_HEIGHT / 2;
    ball_dir_x = fabs(ball_dir_x); // force it to be positive
    ball_dir_y = 0;
  }

  // hit right wall?
  if (ball_pos_x > SCR_WIDTH) {
    ++score_left;
    ball_pos_x = SCR_WIDTH / 2;
    ball_pos_y = SCR_HEIGHT / 2;
    ball_dir_x = -fabs(ball_dir_x); // force it to be negative
    ball_dir_y = 0;
  }

  // hit top wall?
  if (ball_pos_y > SCR_HEIGHT) {
    ball_dir_y = -fabs(ball_dir_y); // force it to be negative
  }

  // hit bottom wall?
  if (ball_pos_y < 0) {
    ball_dir_y = fabs(ball_dir_y); // force it to be positive
  }
}

// excuse me for unreadable code here. I was really sleepy when writing this, and I do not plan to come back here
// this will return the Y value where the ball will be
int predict() {
  // if it is not going towards the AI, do nothing
  if (ball_dir_x < 0) {
    return racket_right_y;
  }

  // define variables we can mess with to predict (sim here stands for simulation)
  float ball_dir_y_sim = ball_dir_y, ball_dir_x_sim = ball_dir_x;
  int ball_x_sim = ball_pos_x, ball_y_sim = ball_pos_y;

  // while it is not where we want, loop here
  while (ball_x_sim < SCR_WIDTH) {
    // make the simulation ball move (just like a normal ball, but it is not drawn)
    ball_x_sim += ball_dir_x_sim * ball_speed;
    ball_y_sim += ball_dir_y_sim * ball_speed;

    // maek the ball bounce
    if (ball_y_sim < 0) {
      ball_dir_y = fabs(ball_dir_y_sim); // bounce bottom wall
    } else if (ball_y_sim >= SCR_HEIGHT) {
      ball_dir_y_sim = -fabs(ball_dir_y_sim); // bounce top wall
    }

    // if we get off-limits, tell the AI to come back
    if (ball_y_sim > SCR_HEIGHT) {
      ball_y_sim = SCR_HEIGHT;
    } if (ball_y_sim < 0) {
      ball_y_sim = 0;
    }
  }
  return ball_y_sim - 20;
}

int generate_random(int max_num) {
  struct timeval t;
  gettimeofday(&t, NULL);
  pid_t pid = getpid();
  srand(t.tv_usec * t.tv_sec * pid);

  return rand() % max_num;
}

void dotted_line() {
  glBegin(GL_POINTS);
  for (int i = 0; i < SCR_HEIGHT; i += 4) {
    glVertex2f(SCR_WIDTH / 2    , i);
    glVertex2f(SCR_WIDTH / 2    , i + 1);
    glVertex2f(SCR_WIDTH / 2 + 1, i);
    glVertex2f(SCR_WIDTH / 2 + 1, i + 1);
    glVertex2f(SCR_WIDTH / 2 - 1, i);
    glVertex2f(SCR_WIDTH / 2 - 1, i + 1);
  }
  glEnd();
}
