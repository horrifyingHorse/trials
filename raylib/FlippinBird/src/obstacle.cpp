#include "obstacle.h"
#include "bird.h"
#include <random>

Obstacle::Obstacle(int radius) : min_gap(radius + 90) {}

void Obstacle::draw() {
  if (lastObstacle >= space + width) {
    generateNewObstacle();
    lastObstacle = 0;
  }

  for (auto &obstacle : obstacles) {
    Color obsColor = (Color){10, 10, 10, 255};
    DrawRectangleRec(obstacle.top, obsColor);
    DrawRectangleRec(obstacle.bottom, obsColor);
    if (!pause) {
      obstacle.top.x -= speedX;
      obstacle.bottom.x -= speedX;
      if (obstacle.top.x + width < 0)
        firstOutOfView = true;
    }
  }

  if (firstOutOfView) {
    obstacles.erase(obstacles.begin());
    firstOutOfView = false;
  }

  if (!pause)
    lastObstacle += speedX;
}

void Obstacle::generateNewObstacle() {
  Blocks b;
  float height;
  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<> topG(0, winHeight - min_gap);
  height = topG(gen);
  b.top = (Rectangle){winWidth, 0, width, height};

  std::uniform_int_distribution<> bottomG(height + min_gap, winHeight);
  height = bottomG(gen);
  b.bottom = (Rectangle){winWidth, height, width, winHeight - height};

  obstacles.push_back(b);
}

void Obstacle::debug() {
  std::string txt =
      "Obstacles Vector size: " + std::to_string(obstacles.size());
  std::string spd = "speedX: " + std::to_string(speedX);
  DrawText(txt.c_str(), 200, 200, 20, FGCOLOR);
  DrawText(spd.c_str(), 200, 224, 20, FGCOLOR);
}

void Obstacle::pauseFrame() { pause = true; }

void Obstacle::incrementSpeed() { speedX += 0.3; }

void Obstacle::reset() {
  obstacles.clear();
  firstOutOfView = false;
  lastObstacle = 9999;
  pause = false;
}

bool scoreOrCollision(Bird &b, Obstacle &o, bool &passing) {
  if (b.birb.y + b.birb.r >= GetScreenHeight() || b.birb.y - b.birb.r <= 0)
    return true;

  bool modifiedPassing = false;
  for (const auto &obstacle : o.obstacles) {
    if (CheckCollisionCircleRec((Vector2){(float)b.birb.x, (float)b.birb.y},
                                b.birb.r, obstacle.top) ||
        CheckCollisionCircleRec((Vector2){(float)b.birb.x, (float)b.birb.y},
                                b.birb.r, obstacle.bottom))
      return true;

    if (!modifiedPassing && b.birb.x >= obstacle.top.x &&
        b.birb.x <= obstacle.top.x + o.width) {
      modifiedPassing = true;
    }
  }

  passing = modifiedPassing;

  return false;
};
