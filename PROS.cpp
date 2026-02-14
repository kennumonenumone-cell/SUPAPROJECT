#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include "variable.h"

using namespace std;

// --- ส่วนกำหนดสี (Shared UI Resources) ---
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLDRED "\033[1m\033[31m"

// --- โครงสร้างข้อมูลพื้นฐาน (Data Core) ---
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
    Day() {
        for(int i=0; i<1440; i++) timeSlots[i] = "Free";
    }
};

vector<Day> week(7);
vector<Task> taskList;
int currentDay = 1;
int currentHour = 8;
int currentMinute = 0;

// ==========================================================
// ส่วนที่ 1: [BACKEND DEVELOPER] - ระบบประมวลผลฐานเวลา
// รับผิดชอบ: การจัดการนาทีในตาราง และการคำนวณหา "เวลาว่างที่เหลือ"
// ==========================================================

int toMinutes(int h, int m) {
    return (h * 60) + m;
}

// ฟังก์ชันสำหรับจองเวลาในตาราง (Mark slot ว่าไม่ว่าง)
void fillSchedule(int dIdx, string name, int sH, int sM, int eH, int eM) {
    int start = toMinutes(sH, sM);
    int end = toMinutes(eH, eM);
   
    if (end < start) { // กรณีจองข้ามคืน
        for(int m = start; m < 1440; m++) week[dIdx].timeSlots[m] = name;
        for(int m = 0; m <= end; m++) week[dIdx].timeSlots[m] = name;
    } else {
        for(int m = start; m <= end; m++) week[dIdx].timeSlots[m] = name;
    }
}

// ฟังก์ชันคำนวณนาทีว่างที่เหลืออยู่จริง (Net Free Minutes)
int getNetFreeMinutes(int dDay, int dHour, int dMinute) {
    int freeCount = 0;
    int startMinToday = toMinutes(currentHour, currentMinute);
    int targetEndMin = toMinutes(dHour, dMinute);

    for (int d = currentDay - 1; d < dDay; d++) {
        int mStart = (d == currentDay - 1) ? startMinToday : 0;
        int mEnd = (d == dDay - 1) ? targetEndMin : 1439;

        for (int m = mStart; m <= mEnd; m++) {
            if (m >= 0 && m < 1440 && week[d].timeSlots[m] == "Free") {
                freeCount++;
            }
        }
    }
    return freeCount;
}

// ==========================================================
// ส่วนที่ 2: [ALGORITHM ENGINEER] - ระบบคิดลอจิกความเครียด
// รับผิดชอบ: การประมวลผล Stress Index และการจัดลำดับงาน (Priority)
// ==========================================================

void processTaskPriority() {
    for (auto& t : taskList) {
        // ขอข้อมูลเวลาว่างจาก Backend มาประมวลผล
        int free = getNetFreeMinutes(t.deadlineDay, t.deadlineHour, t.deadlineMinute);
        // สูตร: งานที่เหลืออยู่ / เวลาว่างที่ใช้ได้จริง
        t.stressIndex = (free <= 0) ? 999.0 : (double)t.remainingMinutes / free;
    }

    // เรียงลำดับงานตามความวิกฤต (Stress มากไปหาน้อย)
    sort(taskList.begin(), taskList.end(), [](const Task& a, const Task& b) {
        return a.stressIndex > b.stressIndex;
    });
}

// ==========================================================
// ส่วนที่ 3: [UI VISUALIZER] - ระบบแสดงผลข้อมูล Dashboard
// รับผิดชอบ: การวาดตารางสรุปผล, ใส่สีสัน และวาดตารางเวลา ASCII
// ==========================================================

void printScheduleTable() {
    cout << "\n--- ตารางเวลาภาพรวม (Overview) ---\n";
    cout << "Time  | D1   | D2   | D3   | D4   | D5   | D6   | D7   |\n";
    cout << "----------------------------------------------------------\n";
    for (int h = 0; h < 24; h++) {
        printf("%02d:00 | ", h);
        for (int d = 0; d < 7; d++) {
            string display = "-";
            for(int m = h*60; m < (h+1)*60; m++) {
                if(week[d].timeSlots[m] != "Free") {
                    display = week[d].timeSlots[m].substr(0, 4);
                    break;
                }
            }
            cout << left << setw(5) << display << "| ";
        }
        cout << endl;
    }
}

void displayToDoList() {
    processTaskPriority(); // เรียกใช้ระบบคิดจากส่วนที่ 2

    cout << "\n" << string(75, '=') << endl;
    cout << "  NOW: Day " << currentDay << " | Time " << setfill('0') << setw(2) << currentHour << ":" << setw(2) << currentMinute << setfill(' ') << endl;
    cout << string(75, '=') << endl;
    cout << left << setw(15) << "ชื่องาน" << setw(12) << "Deadline" << setw(15) << "เวลาว่างที่เหลือ" << setw(15) << "เวลาที่ต้องใช้" << "สถานะ" << endl;
    cout << string(75, '-') << endl;

    for (const auto& t : taskList) {
        double freeH = getNetFreeMinutes(t.deadlineDay, t.deadlineHour, t.deadlineMinute) / 60.0;
        double needH = t.remainingMinutes / 60.0;
       
        // กำหนดสีตามระดับความเครียด (Stress Level)
        string status = (t.stressIndex > 1.0) ? BOLDRED + string("CRITICAL") : (t.stressIndex > 0.7) ? RED + string("Urgent") : (t.stressIndex > 0.4) ? YELLOW + string("Warning") : GREEN + string("Chill");
       
        printf("%-15s D%d %02d:%02d    %-12.1f ชม.    %-12.1f ชม.   ", t.name.c_str(), t.deadlineDay, t.deadlineHour, t.deadlineMinute, freeH, needH);
        cout << status << RESET << endl;
    }
}

// ==========================================================
// ส่วนที่ 4: [SYSTEM CONTROLLER] - ระบบควบคุมการทำงาน (Main Control)
// รับผิดชอบ: การรับ Input, ลูปเมนูหลัก และการตัดสินใจในระดับโปรแกรม
// ==========================================================

int main() {
    // Phase 1: รับข้อมูลการจองเวลาเริ่มต้น
    cout << "=== ตั้งค่ากิจกรรมที่ทำเป็นรายวันหรือทุกวัน (ex.นอน,เรียน) ===\n";
    int rCount; cout << "จำนวนกิจกรรม: "; cin >> rCount;
    for(int i=0; i<rCount; i++) {
        string name; int sH, sM, eH, eM, mode;
        cout << "\nชื่อกิจกรรม: "; cin >> name;
        cout << "เริ่ม(ชม นาที) จบ(ชม นาที): "; cin >> sH >> sM >> eH >> eM;
        cout << "ลงตารางแบบไหน? (1: ทุกวัน, 2: ระบุวัน): "; cin >> mode;
        if(mode == 1) {
            for(int d=0; d<7; d++) fillSchedule(d, name, sH, sM, eH, eM);
        } else {
            cout << "ระบุเลขวัน (1-7) พิมพ์ 0 เพื่อจบ: ";
            int dTarget;
            while(cin >> dTarget && dTarget != 0) {
                if(dTarget >= 1 && dTarget <= 7) fillSchedule(dTarget-1, name, sH, sM, eH, eM);
            }
        }
    }

    // Phase 2: เพิ่มงานที่ต้องทำ
    cout << "\n=== เพิ่มงาน (Tasks) ===\n";
    int tCount; cout << "จำนวนงาน: "; cin >> tCount;
    for(int i=0; i<tCount; i++) {
        Task t; double hours;
        cout << "ชื่อ ชม.ที่ใช้ วันส่ง เวลาส่ง(ชม นาที): ";
        cin >> t.name >> hours >> t.deadlineDay >> t.deadlineHour >> t.deadlineMinute;
        t.remainingMinutes = (int)(hours * 60);
        taskList.push_back(t);
    }

    // Phase 3: Runtime Loop (วนลูปหน้าเมนู)
    while (true) {
        displayToDoList();
        cout << "\n[ MENU ]\n1. อัปเดตเวลาปัจจุบัน\n2. เพิ่มงานใหม่\n3. เพิ่มธุระพิเศษ\n4. แจ้งงานเสร็จ\n5. ดูตารางภาพรวม\n0. ออก\nเลือก: ";
        int choice; cin >> choice;

        if (choice == 1) {
            cout << "ระบุวัน(1-7) และเวลา(ชม นาที) ปัจจุบัน: "; cin >> currentDay >> currentHour >> currentMinute;
        }
        else if (choice == 2) {
            Task t; double h; cin >> t.name >> h >> t.deadlineDay >> t.deadlineHour >> t.deadlineMinute;
            t.remainingMinutes = h * 60; taskList.push_back(t);
        }
        else if (choice == 3) {
            string n; int d, sH, sM, eH, eM; cin >> d >> n >> sH >> sM >> eH >> eM;
            if(d >= 1 && d <= 7) fillSchedule(d-1, n, sH, sM, eH, eM);
        }
        else if (choice == 4) {
            string n; cout << "ชื่องานที่เสร็จ: "; cin >> n;
            for(int i=0; i<taskList.size(); i++) if(taskList[i].name == n) { taskList.erase(taskList.begin()+i); break; }
        }
        else if (choice == 5) {
            printScheduleTable();
            cout << "\nกด Enter เพื่อกลับเมนูหลัก..."; cin.ignore(1000, '\n'); cin.get();
        }
        else if (choice == 0) break;
    }
    return 0;
}