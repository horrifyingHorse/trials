#include <climits>
#include <cstdlib>
#include <random>
#include <raylib.h>
#include <string>
#include <vector>

#define BGCOLOR (Color){23, 23, 23, 255}
#define FGCOLOR (Color){244, 221, 222, 255}

class Bird;
class Obstacle;

class Bird {
public:
  Bird(int radius, Color c = {0, 0, 0, 255}) {
    birb.r = radius;
    birb.x = GetScreenWidth() / 2;
    birb.y = GetScreenHeight() / 2;
    birb.c = c;
  }

  void draw() { DrawCircle(birb.x, birb.y, birb.r, birb.c); }

  void flap() {
    if (pause)
      return;
    t = -5;
  }

  void gravity() {
    if (pause)
      return;
    if (birb.y >= GetScreenHeight() - birb.r && t >= 0) {
      birb.y = GetScreenHeight() - birb.r;
      velocity = 0;
      t = 0;
      return;
    }
    velocity = acceleration * t;
    t += 0.2;
    birb.y += velocity;
  }

  void debug() {
    std::string v = "velocity: " + std::to_string(velocity);
    std::string tstr = "t: " + std::to_string(t);
    DrawText(v.c_str(), 100, 100, 20, FGCOLOR);
    DrawText(tstr.c_str(), 100, 130, 20, FGCOLOR);
  }

  void pauseFrame() { pause = true; }

  void reset() {
    birb.x = GetScreenWidth() / 2;
    birb.y = GetScreenHeight() / 2;
    t = 0;
    pause = false;
  }

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

class Obstacle {
public:
  Obstacle(int radius) : min_gap(radius + 90) {}

  void draw() {
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

  void generateNewObstacle() {
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

  void debug() {
    std::string txt =
        "Obstacles Vector size: " + std::to_string(obstacles.size());
    DrawText(txt.c_str(), 200, 200, 20, FGCOLOR);
  }

  void pauseFrame() { pause = true; }

  void reset() {
    obstacles.clear();
    firstOutOfView = false;
    lastObstacle = 9999;
    pause = false;
  }

  friend bool scoreOrCollision(Bird &, Obstacle &, bool &);

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
  int speedX = 3;

  int lastObstacle = 9999;
  bool firstOutOfView = false;
  bool pause = false;
};

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

int main() {
  int h = 500, w = 900;
  InitWindow(w, h, "bilyag");
  SetTargetFPS(60);

  const int RADIUS = 15;
  bool ggwp = false;
  Bird b(RADIUS, FGCOLOR);
  Obstacle view(RADIUS);
  bool isPressed = false;
  unsigned long long score = 0;
  bool passing = false;
  while (!WindowShouldClose()) {
    if (ggwp && IsKeyDown(KEY_ENTER)) {
      b.reset();
      view.reset();
      score = 0;
      passing = false;
      ggwp = false;
    }
    if (IsKeyDown(KEY_SPACE) && !isPressed) {
      b.flap();
      isPressed = true;
    }

    if (IsKeyUp(KEY_SPACE) && isPressed) {
      isPressed = false;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawText("Space To Bounce", 100, 350, 20, FGCOLOR);
    view.draw();
    b.draw();

    if (ggwp) {
      DrawText("Press Enter to Restart", 100, 320, 20, FGCOLOR);
    }
    DrawText(std::to_string(score).c_str(), 100, 80, 60, WHITE);
    EndDrawing();

    bool wasPassing = passing;
    ggwp = scoreOrCollision(b, view, passing);
    if (ggwp) {
      b.pauseFrame();
      view.pauseFrame();
    }
    if (wasPassing && !passing) {
      score++;
    }

    b.gravity();
  }

  CloseWindow();

  return 0;
}
