#include <climits>
#include <cstdlib>
#include <raylib.h>
#include <string>

#include "bird.h"
#include "obstacle.h"

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
  bool speedIncremented = true;
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
      speedIncremented = false;
    }

    if (score != 0 && !(score % 10) && !speedIncremented) {
      view.incrementSpeed();
      speedIncremented = true;
    }

    b.gravity();
  }

  CloseWindow();

  return 0;
}
