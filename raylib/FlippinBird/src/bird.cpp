#include "bird.h"
#include <string>

Bird::Bird(int radius, Color c) {
  birb.r = radius;
  birb.x = GetScreenWidth() / 2;
  birb.y = GetScreenHeight() / 2;
  birb.c = c;
}

void Bird::draw() { DrawCircle(birb.x, birb.y, birb.r, birb.c); }

void Bird::flap() {
  if (pause)
    return;
  t = -5;
}

void Bird::gravity() {
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

void Bird::debug() {
  std::string v = "velocity: " + std::to_string(velocity);
  std::string tstr = "t: " + std::to_string(t);
  DrawText(v.c_str(), 100, 100, 20, FGCOLOR);
  DrawText(tstr.c_str(), 100, 130, 20, FGCOLOR);
}

void Bird::pauseFrame() { pause = true; }

void Bird::reset() {
  birb.x = GetScreenWidth() / 2;
  birb.y = GetScreenHeight() / 2;
  t = 0;
  pause = false;
}
