#include <algorithm>
#include "algorithm.h"
#include "backend.h"
#include "data.h"

void processTaskPriority() {
    for (auto& t : taskList) {
        int free = getNetFreeMinutes(t.deadlineDay, t.deadlineHour, t.deadlineMinute);
        t.stressIndex = (free <= 0) ? 999.0 : (double)t.remainingMinutes / free;
    }

    sort(taskList.begin(), taskList.end(), [](const Task& a, const Task& b) {
        return a.stressIndex > b.stressIndex;
    });
}
