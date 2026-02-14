#ifndef PLANNER_H
#define PLANNER_H

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// ======================
// สี UI
// ======================
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLDRED "\033[1m\033[31m"

// ======================
// โครงสร้างข้อมูล
// ======================

struct Task {
    string name;
    int remainingMinutes;
    int deadlineDay;
    int deadlineHour;
    int deadlineMinute;
    double stressIndex;
};

struct Day {
    string timeSlots[1440];
    Day();
};

// ======================
// ตัวแปรที่ใช้ร่วมกัน
// ======================

extern vector<Day> week;
extern vector<Task> taskList;

extern int currentDay;
extern int currentHour;
extern int currentMinute;

// ======================
// Backend Functions
// ======================

int toMinutes(int h, int m);
void fillSchedule(int dIdx, string name, int sH, int sM, int eH, int eM);
int getNetFreeMinutes(int dDay, int dHour, int dMinute);

// ======================
// Algorithm Functions
// ======================

void processTaskPriority();

// ======================
// UI Functions
// ======================

void printScheduleTable();
void displayToDoList();

#endif

// what
