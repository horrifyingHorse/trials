#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <raylib.h>
#include <vector>

#define BGCOLOR (Color){23, 23, 23, 255}
#define FGCOLOR (Color){244, 221, 222, 255}

class Bird;

class Obstacle {
public:
  Obstacle(int radius);

  void draw();
  void generateNewObstacle();
  void pauseFrame();
  void incrementSpeed();
  void reset();

  friend bool scoreOrCollision(Bird &, Obstacle &, bool &);

  void debug();

protected:
  typedef struct Blocks {
    Rectangle top, bottom;
  } Blocks;
  std::vector<Blocks> obstacles;

  int min_gap; // radius + 20

  float winWidth = GetScreenWidth();
  int winHeight = GetScreenHeight();

  float width = 140;
  int space = 300;
  float speedX = 3;

  int lastObstacle = 9999;
  bool firstOutOfView = false;
  bool pause = false;
};

#endif // !OBSTACLE_H
