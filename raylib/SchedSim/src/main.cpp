#include <raylib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../include/Scheduler.h"

#define BGCOLOR (Color){23, 23, 23, 255}
#define BGCOLOR_ACTIVE (Color){40, 44, 52, 255}
#define FGCOLOR (Color){244, 221, 222, 255}
#define FGCOLOR_TAB (Color){184, 187, 241, 255}
#define CPUGREEN (Color){129, 178, 154, 255}
#define IORED (Color){224, 122, 95, 255}

Processes procs;
Device d;

Processes GetProcs() {
#define GetIndex(exec)                           \
  if (indx != 0) {                               \
    sv.remove_prefix(indx + 1);                  \
  }                                              \
  indx = sv.find_first_of(";");                  \
  if (indx == std::string::npos) {               \
    std::cout << "Invalid Format in procs.proc"; \
    exit(1);                                     \
  }                                              \
  exec

  std::fstream procFile("procs.proc");
  if (!procFile) {
    std::cout << "Unable to open procs.proc";
    exit(1);
  }
  int indx = 0;
  Processes procs;
  std::string line;
  std::string_view sv;
  std::string procName;
  size_t arrivalTime = SIZE_MAX;
  size_t burstTimeCPU = SIZE_MAX;
  size_t burstTimeIO = SIZE_MAX;
  size_t burstTimeRate = SIZE_MAX;
  size_t burstRemainCPU = SIZE_MAX;

  while (std::getline(procFile, line)) {
    indx = 0;
    sv = line;
    GetIndex(procName = sv.substr(0, indx));
    GetIndex(arrivalTime = std::stoull((std::string)sv.substr(0, indx)));
    GetIndex(burstTimeCPU = std::stoull((std::string)sv.substr(0, indx)));
    GetIndex(burstTimeIO = std::stoull((std::string)sv.substr(0, indx)));
    sv.remove_prefix(indx + 1);
    burstTimeRate = std::stoull((std::string)sv);
    procs.push_back(Process(procName, arrivalTime, burstTimeCPU, burstTimeIO,
                            burstTimeRate));
  }
  return std::move(procs);
}

void DrawTableBoxes(int x,
                    int y,
                    int rows,
                    int cols,
                    int cellWidth,
                    int cellHeight) {
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      DrawRectangleLines(x + j * cellWidth, y + i * cellHeight, cellWidth,
                         cellHeight, BGCOLOR_ACTIVE);
    }
  }
}

void ProcsTab() {
  int balancey = 0;
  DrawText("SchedSim", 100, 80, 60, WHITE);
  DrawTableBoxes(90, 300, 5, 5, 180, 40);

  DrawText("Process Name", 100, 310, 20, FGCOLOR);
  DrawText("Arrival Time", 280, 310, 20, FGCOLOR);
  DrawText("CPU Burst Time", 460, 310, 20, FGCOLOR);
  DrawText("IO Burst Time", 640, 310, 20, FGCOLOR);
  DrawText("IO Burst Rate", 820, 310, 20, FGCOLOR);
  for (const auto& proc : procs) {
    DrawText(proc.procName.c_str(), 100, 350 + balancey, 20, FGCOLOR);
    DrawText(std::to_string(proc.arrivalTime).c_str(), 280, 350 + balancey, 20,
             FGCOLOR);
    DrawText(std::to_string(proc.burstTimeCPU).c_str(), 460, 350 + balancey, 20,
             FGCOLOR);
    DrawText(std::to_string(proc.burstTimeIO).c_str(), 640, 350 + balancey, 20,
             FGCOLOR);
    DrawText(std::to_string(proc.burstTimeRate).c_str(), 820, 350 + balancey,
             20, FGCOLOR);
    balancey += 40;
  }
}

void GanttTab() {
  int w = 1100, h = 600;
  DrawRectangleLines(100, 100, w, h, FGCOLOR);
  // DrawLine(150, 120, 1000, 120, FGCOLOR);
  size_t max = d.getTicks();
  size_t units = w / max;
  for (size_t i = 0; i <= units; i++) {
    DrawText(std::to_string(i * max / units).c_str(), 100 + max * i, 75, 20,
             FGCOLOR);
  }
  int procY = 140;
  int nameStart = 20;
  Processes procs = d.getCompletedProcs();
  for (auto& proc : procs) {
    double fsize = 20, fadjust = 1;
    while (proc.procName.size() * (double)((fsize / fadjust) / 2) + nameStart >=
           100) {
      fadjust *= 1.2;
      nameStart = std::max(nameStart - 3, 0);
    }
    DrawText(proc.procName.c_str(), nameStart, procY - (fsize / fadjust) / 2,
             fsize / fadjust, FGCOLOR);
    DrawLine(100, procY, 100 + w, procY, FGCOLOR);
    for (auto& burst : proc.burstCPU) {
      DrawRectangle(100 + burst.first * units, procY - 20,
                    (burst.second - burst.first) * units, 20, CPUGREEN);
    }
    for (auto& burst : proc.burstIO) {
      DrawRectangle(100 + burst.first * units, procY,
                    (burst.second - burst.first) * units, 20, IORED);
    }
    procY += 60;
  }
}

void getTab(const int& activeTabNo) {
  if (activeTabNo == 1) {
    ProcsTab();
  } else if (activeTabNo == 2) {
    GanttTab();
  }
}

int main(int argc, char* argv[]) {
  int h = 800, w = 1300;
  InitWindow(w, h, "SchedSim");
  SetTargetFPS(60);

  Schedulers schedulers;
  std::vector<std::string> args(argv + 1, argv + argc);
  for (const auto& arg : args) {
    if (arg == "sjf") {
      schedulers.push_back(Scheduler::SJF);
    } else if (arg == "srtf") {
      schedulers.push_back(Scheduler::SRTF);
    } else if (arg == "rr") {
      schedulers.push_back(Scheduler::RR);
    } else if (arg == "vrr") {
      schedulers.push_back(Scheduler::VRR);
    } else {
      std::cout << "Invalid Argument: " << arg;
      return 1;
    }
  }
  if (schedulers.size() == 0) {
    schedulers.push_back(Scheduler::SJF);
  }

  procs = GetProcs();
  for (const auto& scheduler : schedulers) {
    d.init(procs, {.sched = scheduler, .q = 5});
    d.start();
    d.debug();
  }
  std::cout << "Ended\n";
  bool isPressed = false;
  struct {
    int x, y, length;
  } activeTab = {.x = 5, .y = 5, .length = 90};
  int activeTabNo = 1;
  while (!WindowShouldClose()) {
    if (IsKeyDown(KEY_Q)) {
      exit(0);
    }
    if (IsKeyDown(KEY_TWO)) {
      activeTab = {.x = 105, .y = 5, .length = ((20 / 2) * 14) + 20};
      activeTabNo = 2;
    } else if (IsKeyDown(KEY_ONE)) {
      activeTab = {.x = 5, .y = 5, .length = ((20 / 2) * 8) + 10};
      activeTabNo = 1;
    }
    if (IsKeyDown(KEY_SPACE) && !isPressed) {
      isPressed = true;
    }

    if (IsKeyUp(KEY_SPACE) && isPressed) {
      isPressed = false;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawRectangle(0, 0, w, 25, BGCOLOR);
    DrawRectangle(activeTab.x, activeTab.y, activeTab.length, 25,
                  BGCOLOR_ACTIVE);
    DrawText("1: Procs", 10, 10, 20, FGCOLOR_TAB);
    DrawText("2: Gantt Chart", 110, 10, 20, FGCOLOR_TAB);
    getTab(activeTabNo);

    if (0) {
      DrawText("Press Enter to Restart", 100, 320, 20, FGCOLOR);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
