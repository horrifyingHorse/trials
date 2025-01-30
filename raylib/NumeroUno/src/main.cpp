#include <raylib.h>

int main() {
  int h = 500, w = 900;
  InitWindow(w, h, "bilyag");
  SetTargetFPS(60);
  Rectangle r = {100, 100, 30, 20};

  int speed = 10;
  while (!WindowShouldClose()) {
    if ((IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_L)) && r.x < w - 30)
      r.x += speed;
    else if ((IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_H)) && r.x > 0)
      r.x -= speed;
    else if ((IsKeyDown(KEY_UP) || IsKeyDown(KEY_K)) && r.y > 0)
      r.y -= speed;
    else if ((IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_J)) && r.y < h - 30)
      r.y += speed;

    BeginDrawing();
    ClearBackground(SKYBLUE);
    DrawRectangleRec(r, RED);
    DrawCircle(r.x + 10, r.y + r.height / 2, 5, BLACK);
    DrawText("What are these args btw..", 320, 450, 20, BLACK);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
