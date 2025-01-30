#include <raylib.h>
#include <string>

#define BGCOLOR (Color){23, 23, 23, 255}
#define FGCOLOR (Color){244, 221, 222, 255}

class Bird {
public:
  Bird(int radius, Color c = {0, 0, 0, 255}) {
    birb.r = radius;
    birb.x = GetScreenWidth() / 2;
    birb.y = GetScreenHeight() / 2;
    birb.c = c;
  }

  void draw() { DrawCircle(birb.x, birb.y, birb.r, birb.c); }

  void flap() { t = -5; }

  void gravity() {
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

protected:
  typedef struct Circle {
    int x, y, r;
    Color c = {0, 0, 0, 255};
  } Circle;

  Circle birb;

  int velocity;
  float acceleration = 1;
  float t = 0;
};

int main() {
  int h = 500, w = 900;
  InitWindow(w, h, "bilyag");
  SetTargetFPS(60);

  Bird b(25, FGCOLOR);
  int speed = 10;
  bool isPressed = false;
  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_SPACE) && !isPressed) {
      b.flap();
      isPressed = true;
    }
    if (IsKeyUp(KEY_SPACE)) {
      isPressed = false;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawText("Space To Bounce", 100, 350, 20, FGCOLOR);
    b.debug();
    b.draw();
    EndDrawing();

    b.gravity();
  }

  CloseWindow();

  return 0;
}
