#ifndef DATA_H
#define DATA_H

#include <string>
#include <vector>

using namespace std;

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
};

extern vector<Day> week;
extern vector<Task> taskList;

extern int currentDay;
extern int currentHour;
extern int currentMinute;

#endif
