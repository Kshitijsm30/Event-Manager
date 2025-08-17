#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <bits/stdc++.h>
using namespace std;

struct Event {
    int id;
    string name;
    string date;      
    string time; 
    string type;
    string location;

    Event() : id(0), name(""), date(""), time(""), type(""), location("") {}
    Event(int _id, const string& _name, const string& _date, const string& _time,
          const string& _type, const string& _location = "")
        : id(_id), name(_name), date(_date), time(_time), type(_type), location(_location) {}
};


vector<Event> events;
int nextId = 1;
const string STORAGE_FILE = "events.txt";


string getTodayDate() {
    time_t t = time(nullptr);
    tm *now = localtime(&t);
    char buf[11];
    strftime(buf, sizeof(buf), "%d-%m-%Y", now);
    return string(buf);
}

bool isValidDate(const string &date) {
    if (date.size() != 10 || date[2] != '-' || date[5] != '-') return false;

    int d, m, y;
    try {
        d = stoi(date.substr(0,2));
        m = stoi(date.substr(3,2));
        y = stoi(date.substr(6,4));
    } catch (...) {
        return false;
    }

    if (m < 1 || m > 12) return false;

    int daysInMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) daysInMonth[1] = 29;

    if (d < 1 || d > daysInMonth[m-1]) return false;

    return true;
}


void loadEvents() {
    events.clear();
    ifstream fin(STORAGE_FILE);
    if (!fin.is_open()) return;

    string line;
    while (getline(fin, line)) {
        if (line.empty()) continue;
        vector<string> parts;
        string cur;
        stringstream ss(line);
        while (getline(ss, cur, '|')) parts.push_back(cur);
        if (parts.size() >= 5) {
            int id = stoi(parts[0]);
            string name = parts[1];
            string date = parts[2];
            string time = parts[3];
            string type = parts[4];
            string location = (parts.size() > 5 ? parts[5] : "");
            events.emplace_back(id, name, date, time, type, location);
            nextId = max(nextId, id + 1);
        }
    }
    fin.close();
}

void saveEvents() {
    ofstream fout(STORAGE_FILE, ios::trunc);
    for (const auto &e : events) {
        auto esc = [](string s) {
            for (char &c : s) if (c == '|') c = ';';
            return s;
        };
        fout << e.id << "|" << esc(e.name) << "|" << e.date << "|" << e.time << "|" << esc(e.type)
             << "|" << esc(e.location) << "\n";
    }
    fout.close();
}


bool hasConflict(const string &date, const string &time, int excludeId = -1) {
    for (const auto &e : events) {
        if (e.id == excludeId) continue;
        if (e.date == date && e.time == time) return true;
    }
    return false;
}

string toLower(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return tolower(c); });
    return s;
}


void addEventInteractive() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string name, date, time, type, location;
    cout << "Event name: "; getline(cin, name);

   bool valid = false;
do {
    cout << "Date (DD-MM-YYYY): ";
    getline(cin, date);
    valid = isValidDate(date);
    if (!valid) cout << "❌ Invalid date! Please enter again.\n";
} while (!valid);


    cout << "Time (HH:MM): "; getline(cin, time);

    if (hasConflict(date, time)) {
        cout << "⚠ Conflict detected: another event exists at this date & time.\n";
        cout << "Do you want to continue and add anyway? (y/n): ";
        char c; cin >> c; cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (c != 'y') {
            cout << "Add cancelled.\n";
            return;
        }
    }

    cout << "Type: "; getline(cin, type);
    cout << "Location (optional): "; getline(cin, location);

    Event e(nextId++, name, date, time, type, location);
    events.push_back(e);
    saveEvents();
    cout << "✅ Event added with ID " << e.id << ".\n";
}

void viewAllEvents() {
    if (events.empty()) {
        cout << "No events scheduled.\n";
        return;
    }
    vector<Event> sorted = events;
    sort(sorted.begin(), sorted.end(), [](const Event &a, const Event &b){
        if (a.date == b.date) return a.time < b.time;
        return a.date < b.date;
    });

    cout << "\n📌 All Events:\n";
    cout << string(60, '=') << "\n";
    for (const auto &e : sorted) {
        cout << "ID: " << e.id << " | " << e.name << " | " << e.date << " " << e.time
             << " | " << e.type;
        if (!e.location.empty()) cout << " | " << e.location;
        cout << "\n";
    }
}

void searchEventsInteractive() {
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    string kw;
    cout << "Search keyword (name/type): "; getline(cin, kw);
    kw = toLower(kw);
    bool found = false;
    cout << "\n🔎 Search Results:\n";
    cout << string(40, '=') << "\n";
    for (const auto &e : events) {
        if (toLower(e.name).find(kw) != string::npos || toLower(e.type).find(kw) != string::npos) {
            cout << "ID: " << e.id << " | " << e.name << " | " << e.date << " " << e.time
                 << " | " << e.type << "\n";
            found = true;
        }
    }
    if (!found) cout << "No events found for '" << kw << "'.\n";
}

void editEventInteractive() {
    int id; cout << "Enter event ID to edit: "; cin >> id;
    auto it = find_if(events.begin(), events.end(), [id](const Event &e){ return e.id == id; });
    if (it == events.end()) {
        cout << "Event not found.\n";
        return;
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Leave blank to keep current value.\n";

    string input;
    cout << "Name (" << it->name << "): "; getline(cin, input);
    if (!input.empty()) it->name = input;

    cout << "Date (" << it->date << "): "; getline(cin, input);
    if (!input.empty()) {
        if (!isValidDate(input)) {
            cout << "❌ Invalid date entered! Keeping old date.\n";
        } else {
            it->date = input;
        }
    }

    cout << "Time (" << it->time << "): "; getline(cin, input);
    if (!input.empty()) {
        if (hasConflict(it->date, input, it->id)) {
            cout << "⚠ Conflict detected with another event at that time. Time not updated.\n";
        } else {
            it->time = input;
        }
    }

    cout << "Type (" << it->type << "): "; getline(cin, input);
    if (!input.empty()) it->type = input;

    cout << "Location (" << it->location << "): "; getline(cin, input);
    if (!input.empty()) it->location = input;

    saveEvents();
    cout << "✅ Event updated.\n";
}

void deleteEventInteractive() {
    int id; cout << "Enter event ID to delete: "; cin >> id;
    auto it = remove_if(events.begin(), events.end(), [id](const Event &e){ return e.id == id; });
    if (it != events.end()) {
        events.erase(it, events.end());
        saveEvents();
        cout << "🗑 Event deleted.\n";
    } else {
        cout << "Event not found.\n";
    }
}

void summaryByType() {
    if (events.empty()) {
        cout << "No events.\n";
        return;
    }
    unordered_map<string,int> cnt;
    for (const auto &e : events) cnt[e.type]++;

    cout << "\n📊 Events by Type:\n";
    for (const auto &p : cnt) cout << p.first << " : " << p.second << "\n";
}

void exportReport() {
    ofstream fout("report.txt", ios::trunc);
    if (!fout.is_open()) {
        cout << "Failed to create report file.\n";
        return;
    }
    fout << "Event Report\n";
    fout << "===========\n";
    vector<Event> sorted = events;
    sort(sorted.begin(), sorted.end(), [](const Event &a, const Event &b){
        if (a.date == b.date) return a.time < b.time;
        return a.date < b.date;
    });
    for (const auto &e : sorted) {
        fout << "ID: " << e.id << " | " << e.name << " | " << e.date << " " << e.time
             << " | " << e.type;
        if (!e.location.empty()) fout << " | " << e.location;
        fout << "\n";
    }
    fout.close();
    cout << "📄 Report saved to report.txt\n";
}

void dayView(const string &date) {
    if (!isValidDate(date)) {
        cout << "❌ Invalid date entered.\n";
        return;
    }
    vector<Event> dayEvents;
    for (const auto &e : events) if (e.date == date) dayEvents.push_back(e);

    if (dayEvents.empty()) {
        cout << "No events for " << date << ".\n";
        return;
    }

    sort(dayEvents.begin(), dayEvents.end(), [](const Event &a, const Event &b){
        return a.time < b.time;
    });

    cout << "\n📅 Events on " << date << ":\n";
    cout << string(40,'=') << "\n";
    for (const auto &e : dayEvents) {
        cout << "ID: " << e.id << " | " << e.name << " | " << e.time << " | " << e.type;
        if (!e.location.empty()) cout << " | " << e.location;
        cout << "\n";
    }
}

void todaysEvents() {
    string today = getTodayDate();
    cout << "Today's date: " << today << "\n";
    dayView(today);
}

void sendRemindersCLI() {
    string date; cout << "Enter date to send reminders for (DD-MM-YYYY): "; cin >> date;
    if (!isValidDate(date)) {
        cout << "❌ Invalid date.\n";
        return;
    }
    vector<Event> dayEvents;
    for (const auto &e : events) if (e.date == date) dayEvents.push_back(e);
    if (dayEvents.empty()) {
        cout << "No events on " << date << ". Nothing to remind.\n";
        return;
    }
    cout << "\nSimulating reminders for events on " << date << ":\n";
    for (const auto &e : dayEvents) {
        cout << "- Reminder: '" << e.name << "' at " << e.time << " (" << e.type << ")\n";
    }
    cout << "✅ Reminders simulated (printed to console).\n";
}

// ---------- Menus ----------
void adminMenu() {
    while (true) {
        cout << "\n===== ADMIN MENU =====\n";
        cout << "1. Add Event\n2. View All Events\n3. Search Events\n4. Edit Event\n5. Delete Event\n";
        cout << "6. Summary by Type\n7. Export Report\n8. Day View (specific date)\n9. Today's Events\n";
        cout << "10. Send Reminders (CLI)\n11. Logout\nChoice: ";
        int ch; cin >> ch;
        switch (ch) {
            case 1: addEventInteractive(); break;
            case 2: viewAllEvents(); break;
            case 3: searchEventsInteractive(); break;
            case 4: editEventInteractive(); break;
            case 5: deleteEventInteractive(); break;
            case 6: summaryByType(); break;
            case 7: exportReport(); break;
            case 8: {
                string d; cout << "Enter date (DD-MM-YYYY): "; cin >> d; dayView(d); break;
            }
            case 9: todaysEvents(); break;
            case 10: sendRemindersCLI(); break;
            case 11: cout << "Logging out of admin.\n"; saveEvents(); return;
            default: cout << "Invalid choice.\n";
        }
    }
}

void guestMenu() {
    while (true) {
        cout << "\n===== GUEST MENU =====\n";
        cout << "1. View All Events\n2. Search Events\n3. Day View (specific date)\n4. Today's Events\n5. Exit to role selection\nChoice: ";
        int ch; cin >> ch;
        switch (ch) {
            case 1: viewAllEvents(); break;
            case 2: searchEventsInteractive(); break;
            case 3: {
                string d; cout << "Enter date (DD-MM-YYYY): "; cin >> d; dayView(d); break;
            }
            case 4: todaysEvents(); break;
            case 5: return;
            default: cout << "Invalid choice.\n";
        }
    }
}

// ---------- Entry ----------
int main() {
    loadEvents();

    cout << "Welcome to Smart Event Manager\n";
    while (true) {
        cout << "\nSelect role:\n1. Admin\n2. Guest\n3. Exit\nChoice: ";
        int role; cin >> role;
        if (role == 1) {
            string pass;
            cout << "Enter admin password: "; cin >> pass;
            if (pass == "admin123") {
                cout << "✅ Admin login successful.\n";
                adminMenu();
            } else {
                cout << "❌ Wrong password. Returning to role selection.\n";
            }
        } else if (role == 2) {
            cout << "Guest mode activated.\n";
            guestMenu();
        } else if (role == 3) {
            cout << "Exiting... Saving data.\n";
            saveEvents();
            break;
        } else {
            cout << "Invalid option.\n";
        }
    }

    return 0;
}