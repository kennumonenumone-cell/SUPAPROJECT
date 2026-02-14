#include <iostream>
#include "data.h"
#include "backend.h"
#include "ui.h"

using namespace std;

// ประกาศตัวแปรจริง (ต้องมีแค่ที่นี่!)
vector<Day> week(7);
vector<Task> taskList;
int currentDay = 1;
int currentHour = 8;
int currentMinute = 0;

int main() {
    cout << "Program started\n";
    displayToDoList();
    return 0;
}
