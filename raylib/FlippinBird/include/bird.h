#ifndef BIRD_H
#define BIRD_H

#include <raylib.h>

#define BGCOLOR (Color){23, 23, 23, 255}
#define FGCOLOR (Color){244, 221, 222, 255}

class Bird;
class Obstacle;

class Bird {
public:
  Bird(int radius, Color c = {0, 0, 0, 255});
  void draw();
  void flap();
  void gravity();
  void pauseFrame();
  void reset();

  void debug();

  friend bool scoreOrCollision(Bird &, Obstacle &, bool &);

protected:
  typedef struct Circle {
    int x, y, r;
    Color c = {0, 0, 0, 255};
  } Circle;

  Circle birb;

  int velocity;
  float acceleration = 1;
  float t = 0;

  bool pause = false;
};

#endif // BIRD_H
