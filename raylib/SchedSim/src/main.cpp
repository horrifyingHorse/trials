#include <raylib.h>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
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

std::string formatTo2(double d) {
  std::ostringstream ss;
  ss << std::fixed << std::setprecision(2) << d;
  return ss.str();
}

void ProcsTab() {
  int balancey = 0;
  DrawText("SchedSim", 100, 80, 60, WHITE);
  DrawTableBoxes(90, 300, procs.size() + 1, 5, 180, 40);

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

void AlgoTabs(int selection) {
  DrawText("sjf", 10, 40, 20, (selection == 0) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("srtf", 110, 40, 20, (selection == 1) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("rr", 210, 40, 20, (selection == 2) ? CPUGREEN : FGCOLOR_TAB);
  DrawText("vrr", 310, 40, 20, (selection == 3) ? CPUGREEN : FGCOLOR_TAB);
}

class GanttChart {
 public:
  GanttChart() {}
  GanttChart(Processes&& procs, size_t ticks) { setProcs(procs, ticks); }
  void setProcs(Processes& procs, size_t ticks) {
    this->procs = procs;
    maxTicks = ticks;
    units = w / maxTicks;
  }
  void draw() {
    size_t chartY = this->chartY;
    DrawLine(chartX, chartY, chartX + w, chartY, FGCOLOR);
    DrawLine(chartX, chartY, chartX, chartY + 40 + procs.size() * 60, FGCOLOR);
    // DrawRectangleLines(chartX, chartY, w, h, FGCOLOR);
    for (size_t i = 0; i <= units; i++) {
      DrawText(std::to_string(i * maxTicks / units).c_str(),
               chartX + maxTicks * i, chartY - 25, 20, FGCOLOR);
    }
    chartY += 40;
    for (auto& proc : procs) {
      double fsize = 20, fadjust = 1;
      while (proc.procName.size() * (double)((fsize / fadjust) / 2) +
                 nameStart >=
             chartX) {
        fadjust *= 1.2;
        nameStart = std::max(nameStart - 3, 0);
      }
      DrawText(proc.procName.c_str(), nameStart, chartY - (fsize / fadjust) / 2,
               fsize / fadjust, FGCOLOR);
      DrawLine(chartX, chartY, chartX + w, chartY, FGCOLOR);
      Vector2 mousePos = GetMousePosition();
      for (auto& burst : proc.burstCPU) {
        int x = chartX + burst.first * units;
        int y = chartY - 20;
        int len = (burst.second - burst.first) * units;
        int h = 20;
        Rectangle r = {(float)x, (float)y, (float)len, (float)h};
        Color color = CPUGREEN;
        if (CheckCollisionPointRec(mousePos, r)) {
          DrawText(std::string(std::to_string(burst.first) + "-" +
                               std::to_string(burst.second))
                       .c_str(),
                   x, y - 25, 20, FGCOLOR);
          color = SKYBLUE;
        }
        DrawRectangleRec(r, color);
      }
      for (auto& burst : proc.burstIO) {
        int x = chartX + burst.first * units;
        int y = chartY;
        int len = (burst.second - burst.first) * units;
        int h = 20;
        Rectangle r = {(float)x, (float)y, (float)len, (float)h};
        Color color = IORED;
        if (CheckCollisionPointRec(mousePos, r)) {
          DrawText(std::string(std::to_string(burst.first) + "-" +
                               std::to_string(burst.second))
                       .c_str(),
                   x, y - 25, 20, FGCOLOR);
          color = SKYBLUE;
        }
        DrawRectangleRec(r, color);
      }
      chartY += 60;
    }
  }

  size_t draw_performace() {
    DrawText("Performance Report:", 5, 80, 20, IORED);
    int adjustment = 0;
    DrawTableBoxes(90, chartY - 10, procs.size() + 1, 6, 180, 40);
    DrawText("Process Name", 100, chartY, 20, FGCOLOR);
    DrawText("Arrival Time", 280, chartY, 20, FGCOLOR);
    DrawText("Start Time", 460, chartY, 20, FGCOLOR);
    DrawText("Comp Time", 640, chartY, 20, FGCOLOR);
    DrawText("Turnaround Time", 820, chartY, 20, FGCOLOR);
    DrawText("Waiting Time", 1030, chartY, 20, FGCOLOR);
    // aT, sT, cT, TAT, WT, Name
    for (auto& proc : procs) {
      DrawText(proc.procName.c_str(), 100, 190 + adjustment, 20, FGCOLOR);
      DrawText(std::to_string(proc.arrivalTime).c_str(), 280, 190 + adjustment,
               20, FGCOLOR);
      DrawText(std::to_string(proc.startTime).c_str(), 460, 190 + adjustment,
               20, FGCOLOR);
      DrawText(std::to_string(proc.completionTime).c_str(), 640,
               190 + adjustment, 20, FGCOLOR);
      DrawText(std::to_string(proc.turnAroundTime()).c_str(), 820,
               190 + adjustment, 20, FGCOLOR);
      DrawText(std::to_string(proc.waitingTime()).c_str(), 1030,
               190 + adjustment, 20, FGCOLOR);
      adjustment += 40;
    }
    return 190 + adjustment + 20;
  }

 private:
  Processes procs = {};
  size_t maxTicks = 1;

  int w = 1100, h = 600;
  int chartX = 100;
  int chartY = 150;
  int nameStart = 20;
  size_t units = 1100;
};

int main(int argc, char* argv[]) {
  int h = 800, w = 1300;
  InitWindow(w, h, "SchedSim");
  SetTargetFPS(60);

  Schedulers schedulers = {Scheduler::SJF, Scheduler::SRTF, Scheduler::RR,
                           Scheduler::VRR};
  std::vector<GanttChart> charts = {};
  std::vector<PerformanceReport> preports = {};
  procs = GetProcs();
  for (const auto& scheduler : schedulers) {
    d.init(procs, {.sched = scheduler, .q = 5});
    d.start();
    charts.push_back(GanttChart(d.getCompletedProcs(), d.getTicks()));
    preports.push_back(d.getPerfReport());
  }
  int chartSelection = 0;
  bool isPressedL = false, isPressedH = false;
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
    } else if (IsKeyDown(KEY_THREE)) {
      activeTab = {.x = 275, .y = 5, .length = ((20 / 2) * 14) + 25};
      activeTabNo = 3;
    }

    BeginDrawing();
    ClearBackground(BGCOLOR);
    DrawRectangle(0, 0, w, 25, BGCOLOR);
    DrawRectangle(activeTab.x, activeTab.y, activeTab.length, 25,
                  BGCOLOR_ACTIVE);
    DrawText("1: Procs", 10, 10, 20, FGCOLOR_TAB);
    DrawText("2: Gantt Chart", 110, 10, 20, FGCOLOR_TAB);
    DrawText("3: Performance", 110 + (14 * 20 / 2) + 30, 10, 20, FGCOLOR_TAB);
    if (activeTabNo != 1) {
      AlgoTabs(chartSelection);
      if (IsKeyDown(KEY_L) && !isPressedL) {
        chartSelection = (chartSelection + 1) % 4;
        isPressedL = true;
      }
      if (IsKeyUp(KEY_L) && isPressedL) {
        isPressedL = false;
      }
      if (IsKeyDown(KEY_H) && !isPressedH) {
        chartSelection = chartSelection - 1;
        if (chartSelection < 0) {
          chartSelection = 3;
        }
        isPressedH = true;
      }
      if (IsKeyUp(KEY_H) && isPressedH) {
        isPressedH = false;
      }
    }
    if (activeTabNo == 1) {
      ProcsTab();
    } else if (activeTabNo == 2) {
      charts[chartSelection].draw();
    } else if (activeTabNo == 3) {
      size_t preportY = charts[chartSelection].draw_performace();
      PerformanceReport preport = preports[chartSelection];
      int i = 0;
      DrawText("Total Time Taken: ", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(
          std::string(std::to_string(preport.totalTicks) + " ticks").c_str(),
          400, preportY + 30 * i++, 20, FGCOLOR);
      DrawText("Avg Waiting Time: ", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.avgWaitingTime).c_str(), 400,
               preportY + 30 * i++, 20, FGCOLOR);
      DrawText("Avg Turnaround Time:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.avgTurnAroundTime).c_str(), 400,
               preportY + 30 * i++, 20, FGCOLOR);
      DrawText("CPU Usage:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.cpuUsage).c_str(), 400, preportY + 30 * i++,
               20, FGCOLOR);
      DrawText("Throughput:", 100, preportY + 30 * i, 20, FGCOLOR);
      DrawText(formatTo2(preport.throughput).c_str(), 400, preportY + 30 * i++,
               20, FGCOLOR);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
