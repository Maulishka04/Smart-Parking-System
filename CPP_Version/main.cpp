// Smart Parking System - C++ Version
// Requirements covered:
// - 5 floors, 20 spots per floor
// - Vehicle base class; Car, Bike, Truck derived
// - Constructors/destructors, operator overloading (<<), exceptions
// - STL containers: vector/map
// - File I/O persistence (CSV)
// - CLI operations: entry, exit, search, reports
// - Smart allocation: nearest spot by floor then spot
// - Billing by type and duration; reports: occupancy, revenue, peak entry hour

#include <algorithm>
#include <array>
#include <ctime>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif
using namespace std;

static const int FLOORS = 5;
static const int SPOTS_PER_FLOOR = 20;
static const char* DATA_DIR_CPP = "data-cpp";
static const char* PARKING_STATE_CPP = "data-cpp/parking_state.csv";
static const char* TRANSACTIONS_CPP = "data-cpp/transactions.csv";

enum class VehicleType { Bike=0, Car=1, Truck=2 };

inline string to_string(VehicleType t) {
    switch (t) {
        case VehicleType::Bike: return "Bike";
        case VehicleType::Car: return "Car";
        case VehicleType::Truck: return "Truck";
        default: return "Unknown";
    }
}

class Vehicle {
protected:
    string license;
    string owner;
    time_t entryTime{};
    VehicleType type;
    int floor{-1}, spot{-1};
public:
    Vehicle(string lic, string own, VehicleType t)
        : license(std::move(lic)), owner(std::move(own)), type(t) {
        entryTime = time(nullptr);
    }
    virtual ~Vehicle() = default;
    VehicleType getType() const { return type; }
    const string& getLicense() const { return license; }
    const string& getOwner() const { return owner; }
    time_t getEntryTime() const { return entryTime; }
    int getFloor() const { return floor; }
    int getSpot() const { return spot; }
    void setPosition(int f, int s) { floor = f; spot = s; }
    void setEntryTime(time_t t) { entryTime = t; }
    virtual double rateFirstHour() const = 0;
    virtual double rateAddHour() const = 0;
    double calcFee(long durationMinutes) const {
        long hours = (durationMinutes + 59) / 60; if (hours < 1) hours = 1;
        double fh = rateFirstHour(); double ah = rateAddHour();
        if (hours == 1) return fh;
        return fh + (hours - 1) * ah;
    }
    friend ostream& operator<<(ostream& os, const Vehicle& v) {
        char buf[64];
        tm tmEntry = *localtime(&v.entryTime);
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmEntry);
        os << v.license << " (" << to_string(v.type) << ") owner=" << v.owner
           << " at F" << v.floor << "-S" << v.spot << " entry=" << buf;
        return os;
    }
};

class Bike : public Vehicle {
public:
    using Vehicle::Vehicle;
    double rateFirstHour() const override { return 20.0; }
    double rateAddHour() const override { return 10.0; }
};
class Car : public Vehicle {
public:
    using Vehicle::Vehicle;
    double rateFirstHour() const override { return 40.0; }
    double rateAddHour() const override { return 20.0; }
};
class Truck : public Vehicle {
public:
    using Vehicle::Vehicle;
    double rateFirstHour() const override { return 60.0; }
    double rateAddHour() const override { return 30.0; }
};

struct ParkingSpot {
    bool occupied{false};
    unique_ptr<Vehicle> vehicle{};
};

static array<array<ParkingSpot, SPOTS_PER_FLOOR>, FLOORS> lot;

static void ensure_dir() {
#ifdef _WIN32
    _mkdir(DATA_DIR_CPP);
#else
    mkdir(DATA_DIR_CPP, 0755);
#endif
}

static bool save_state() {
    ofstream ofs(PARKING_STATE_CPP);
    if (!ofs) return false;
    ofs << "floor,spot,license,owner,type,entryTime\n";
    for (int f = 0; f < FLOORS; ++f) {
        for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
            const auto& ps = lot[f][s];
            if (ps.occupied && ps.vehicle) {
                ofs << f << ',' << s << ','
                    << ps.vehicle->getLicense() << ','
                    << ps.vehicle->getOwner() << ','
                    << static_cast<int>(ps.vehicle->getType()) << ','
                    << static_cast<long long>(ps.vehicle->getEntryTime())
                    << "\n";
            }
        }
    }
    return true;
}

static VehicleType intToType(int x) {
    switch (x) { case 0: return VehicleType::Bike; case 1: return VehicleType::Car; default: return VehicleType::Truck; }
}

static bool load_state() {
    ifstream ifs(PARKING_STATE_CPP);
    if (!ifs) return true;
    string line; getline(ifs, line); // header
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string fld;
        vector<string> cols;
        while (getline(ss, fld, ',')) cols.push_back(fld);
        if (cols.size() < 6) continue;
        int f = stoi(cols[0]);
        int s = stoi(cols[1]);
        string lic = cols[2];
        string own = cols[3];
        int t = stoi(cols[4]);
        long long entry = stoll(cols[5]);
        unique_ptr<Vehicle> v;
        switch (intToType(t)) {
            case VehicleType::Bike: v = make_unique<Bike>(lic, own, VehicleType::Bike); break;
            case VehicleType::Car: v = make_unique<Car>(lic, own, VehicleType::Car); break;
            case VehicleType::Truck: v = make_unique<Truck>(lic, own, VehicleType::Truck); break;
        }
    // override entry time and position using setters
    v->setEntryTime((time_t)entry);
    v->setPosition(f, s);
        lot[f][s].occupied = true;
        lot[f][s].vehicle = std::move(v);
    }
    return true;
}

static bool append_txn(const string& lic, VehicleType t, time_t entry, time_t exitT, long durationMin, double fee) {
    // Ensure file exists with header
    ifstream chk(TRANSACTIONS_CPP);
    bool exists = chk.good();
    chk.close();
    ofstream ofs(TRANSACTIONS_CPP, ios::app);
    if (!ofs) return false;
    if (!exists) ofs << "license,type,entryTime,exitTime,durationMin,fee\n";
    ofs << lic << ',' << static_cast<int>(t) << ','
        << static_cast<long long>(entry) << ','
        << static_cast<long long>(exitT) << ','
        << durationMin << ',' << fixed << setprecision(2) << fee << "\n";
    return true;
}

static pair<int,int> find_nearest_spot() {
    for (int f = 0; f < FLOORS; ++f) {
        for (int s = 0; s < SPOTS_PER_FLOOR; ++s) if (!lot[f][s].occupied) return {f,s};
    }
    return {-1,-1};
}

static pair<bool,pair<int,int>> find_vehicle(const string& lic) {
    for (int f = 0; f < FLOORS; ++f) for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
        if (lot[f][s].occupied && lot[f][s].vehicle && lot[f][s].vehicle->getLicense() == lic) return {true,{f,s}};
    }
    return {false,{-1,-1}};
}

static void trim(string& s) {
    while (!s.empty() && (s.back()=='\r' || s.back()=='\n')) s.pop_back();
}

static int ask_int(const string& prompt) {
    cout << prompt; string line; getline(cin, line); trim(line); try { return stoi(line); } catch (...) { throw runtime_error("Invalid number"); }
}

static string ask_str(const string& prompt) {
    cout << prompt; string line; getline(cin, line); trim(line); if (line.empty()) throw runtime_error("Empty input"); return line;
}

static unique_ptr<Vehicle> make_vehicle(int type, const string& lic, const string& own) {
    VehicleType t;
    if (type==1) t = VehicleType::Bike; else if (type==2) t = VehicleType::Car; else if (type==3) t = VehicleType::Truck; else throw invalid_argument("Invalid vehicle type");
    switch (t) {
        case VehicleType::Bike: return make_unique<Bike>(lic, own, t);
        case VehicleType::Car: return make_unique<Car>(lic, own, t);
        case VehicleType::Truck: return make_unique<Truck>(lic, own, t);
    }
    throw invalid_argument("Invalid vehicle type");
}

static void menu_entry() {
    try {
        cout << "\n=== Vehicle Entry ===\n";
        cout << "1. Bike\n2. Car\n3. Truck\n";
        int type = ask_int("> ");
        string lic = ask_str("License plate: ");
        if (find_vehicle(lic).first) throw runtime_error("Vehicle already parked");
        string own = ask_str("Owner contact/name: ");
        auto pos = find_nearest_spot(); if (pos.first < 0) throw runtime_error("Parking full");
        auto v = make_vehicle(type, lic, own);
        v->setPosition(pos.first, pos.second);
        
        // Display entry time
        time_t entry = v->getEntryTime();
        char entryBuf[64];
        tm te = *localtime(&entry);
        strftime(entryBuf, sizeof(entryBuf), "%Y-%m-%d %H:%M:%S", &te);
        
        lot[pos.first][pos.second].occupied = true;
        lot[pos.first][pos.second].vehicle = std::move(v);
        if (!save_state()) cerr << "Warning: failed to persist state\n";
        cout << "Assigned Floor " << (pos.first+1) << ", Spot " << (pos.second+1) << "\n";
        cout << "Entry time: " << entryBuf << "\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

static void menu_exit() {
    try {
        cout << "\n=== Vehicle Exit ===\n";
        string lic = ask_str("Enter license plate: ");
        auto pos = find_vehicle(lic); if (!pos.first) throw runtime_error("Not found");
        int f = pos.second.first, s = pos.second.second;
        auto& ps = lot[f][s];
        auto& v = ps.vehicle;
        time_t now = time(nullptr);
        long durationMin = max(1L, (long)difftime(now, v->getEntryTime()) / 60);
        double fee = v->calcFee(durationMin);
        time_t entryT = v->getEntryTime();
        char eb[64], xb[64]; tm te = *localtime(&entryT); tm tx = *localtime(&now);
        strftime(eb, sizeof(eb), "%Y-%m-%d %H:%M:%S", &te);
        strftime(xb, sizeof(xb), "%Y-%m-%d %H:%M:%S", &tx);
        cout << "--- Receipt ---\n";
        cout << *v << "\n";
        cout << "Exit=" << xb << ", Duration=" << durationMin << " min, Fee=" << fixed << setprecision(2) << fee << "\n";
        if (!append_txn(v->getLicense(), v->getType(), v->getEntryTime(), now, durationMin, fee)) cerr << "Warning: failed to record transaction\n";
        ps.occupied = false; ps.vehicle.reset();
        if (!save_state()) cerr << "Warning: failed to persist state\n";
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
}

static void menu_search() {
    try {
        cout << "\n=== Search Vehicle ===\n";
        string lic = ask_str("Enter license plate: ");
        auto pos = find_vehicle(lic);
        if (!pos.first) { cout << "Not found\n"; return; }
        int f = pos.second.first, s = pos.second.second;
        auto& v = lot[f][s].vehicle;
        cout << "Found at Floor " << (f+1) << ", Spot " << (s+1) << ": " << *v << "\n";
    } catch (const exception& e) { cout << "Error: " << e.what() << "\n"; }
}

static void report_occupancy() {
    cout << "\n=== Occupancy Report ===\n";
    int totalOcc = 0;
    for (int f=0; f<FLOORS; ++f) {
        int occ=0; for (int s=0; s<SPOTS_PER_FLOOR; ++s) if (lot[f][s].occupied) ++occ;
        totalOcc += occ; double rate = 100.0*occ/SPOTS_PER_FLOOR; cout << "Floor " << (f+1) << ": " << occ << "/" << SPOTS_PER_FLOOR << " (" << fixed << setprecision(1) << rate << "%)\n";
    }
    cout << "Overall: " << totalOcc << "/" << FLOORS*SPOTS_PER_FLOOR << " (" << fixed << setprecision(1) << 100.0*totalOcc/(FLOORS*SPOTS_PER_FLOOR) << "%)\n";
}

static void report_revenue() {
    ifstream ifs(TRANSACTIONS_CPP);
    if (!ifs) { cout << "No transactions yet.\n"; return; }
    string line; if (!getline(ifs, line)) { cout << "No transactions yet.\n"; return; }
    double total=0.0, today=0.0;
    time_t now = time(nullptr); tm tn = *localtime(&now);
    while (getline(ifs, line)) {
        if (line.empty()) continue;
        stringstream ss(line); string col; vector<string> cols; while (getline(ss,col,',')) cols.push_back(col);
        if (cols.size()<6) continue;
    long long exitTll = stoll(cols[3]); double fee = stod(cols[5]); total += fee; time_t exitT = (time_t)exitTll; tm tx = *localtime(&exitT);
        if (tx.tm_year==tn.tm_year && tx.tm_mon==tn.tm_mon && tx.tm_mday==tn.tm_mday) today += fee;
    }
    cout << fixed << setprecision(2) << "Revenue (today): " << today << "\nRevenue (total): " << total << "\n";
}

static void report_peak_entry_hour() {
    array<int,24> counts{}; counts.fill(0);
    ifstream ifs(TRANSACTIONS_CPP);
    string line; if (ifs && getline(ifs, line)) {
        while (getline(ifs, line)) {
            if (line.empty()) continue; stringstream ss(line); string col; vector<string> cols; while (getline(ss,col,',')) cols.push_back(col); if (cols.size()<3) continue; long long entryll = stoll(cols[2]); time_t entry = (time_t)entryll; tm te = *localtime(&entry); int h = te.tm_hour; if (h>=0 && h<24) counts[h]++;
        }
    }
    for (int f=0; f<FLOORS; ++f) for (int s=0; s<SPOTS_PER_FLOOR; ++s) if (lot[f][s].occupied && lot[f][s].vehicle) { time_t et = lot[f][s].vehicle->getEntryTime(); tm te = *localtime(&et); int h = te.tm_hour; if (h>=0 && h<24) counts[h]++; }
    int maxHour = 0, maxCount = counts[0]; for (int h=1; h<24; ++h) if (counts[h]>maxCount) { maxCount=counts[h]; maxHour=h; }
    cout << "\n=== Peak Entry Hour ===\n"; if (maxCount==0) cout << "No data available yet.\n"; else cout << "Busiest entry hour: " << setw(2) << setfill('0') << maxHour << ":00-" << setw(2) << (maxHour+1)%24 << ":00 with " << setfill(' ') << maxCount << " entries\n";
}

static void reports_menu() {
    while (true) {
        cout << "\n=== Reports ===\n1. Occupancy\n2. Revenue\n3. Peak Entry Hour\n4. Back\n> ";
        string line; getline(cin, line); trim(line); if (line.empty()) continue; int ch = stoi(line);
        if (ch==1) report_occupancy(); else if (ch==2) report_revenue(); else if (ch==3) report_peak_entry_hour(); else if (ch==4) break; else cout << "Invalid choice\n";
    }
}

int main() {
    ios::sync_with_stdio(false); cin.tie(nullptr);
    ensure_dir();
    load_state();
    while (true) {
        cout << "\n==============================\n Smart Parking System (C++)\n Floors: " << FLOORS << ", Spots/Floor: " << SPOTS_PER_FLOOR << "\n==============================\n";
        cout << "1. Vehicle Entry (Park)\n2. Vehicle Exit\n3. Search Vehicle\n4. Reports\n5. Save & Exit\n> ";
        string line; getline(cin, line); trim(line); if (line.empty()) continue; int choice = 0; try { choice = stoi(line); } catch (...) { cout << "Invalid input\n"; continue; }
        try {
            if (choice==1) menu_entry();
            else if (choice==2) menu_exit();
            else if (choice==3) menu_search();
            else if (choice==4) reports_menu();
            else if (choice==5) { if (!save_state()) cerr << "Warning: failed to save state\n"; cout << "Goodbye!\n"; break; }
            else cout << "Invalid choice\n";
        } catch (const exception& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
