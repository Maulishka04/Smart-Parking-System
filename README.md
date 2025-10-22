## 🚗 Smart Parking System — Simplifying Urban Parking with Intelligent Slot Allocation

[![Language: C/C++](https://img.shields.io/badge/language-C%2FC%2B%2B-blue?logo=c)](https://en.wikipedia.org/wiki/C_(programming_language)) [![License: MIT](https://img.shields.io/badge/license-MIT-green)](./LICENSE) ![Platform: VS Code](https://img.shields.io/badge/platform-VS%20Code-blueviolet) ![Build: Passing](https://img.shields.io/badge/build-passing-brightgreen)

A comprehensive, educational, and production-minded implementation of an intelligent parking management system — built in both **C** and **C++** for comparison, learning, and real-world deployment.

---

## 📑 Table of Contents

- [About the Project](#-about-the-project)
- [System Architecture Overview](#-system-architecture-overview)
- [Features](#-features)
- [Tech Stack](#-tech-stack)
- [Folder Structure](#-folder-structure)
- [Getting Started](#-getting-started)
  - [Prerequisites](#prerequisites)
  - [Compile & Run (PowerShell / VS Code Terminal)](#compile--run-powershell--vs-code-terminal)
- [Example Input / Output](#-example-input--output)
- [VS Code Environment Setup](#-vs-code-environment-setup)
- [Modularity & Data Structures](#-modularity--data-structures)
- [Troubleshooting](#-troubleshooting)
- [Future Enhancements](#-future-enhancements)
- [Contributing](#-contributing)
- [License](#-license)
- [Credits](#-credits)

---

## 🎯 About the Project

The **Smart Parking System** is a robust, lightweight application designed to demonstrate intelligent vehicle detection and automated slot management for parking facilities. Whether you're a student learning data structures, an instructor teaching C/C++ concepts, or a developer prototyping IoT parking solutions, this project offers a complete reference implementation.

### 🎓 **Key Goals:**
- **Efficient Slot Detection & Allocation**: Automatically assigns the nearest available parking spot based on real-time availability.
- **Vehicle Entry/Exit Tracking**: Logs timestamps for every vehicle, enabling accurate duration calculations.
- **Automated Billing & Receipt Generation**: Calculates parking fees based on duration and vehicle type (Bike, Car, Truck).
- **Persistent State Management**: Uses lightweight CSV files (`parking_state.csv`, `transactions.csv`) for data persistence between sessions.
- **Educational Comparative Study**: Implemented in both **C** (procedural) and **C++** (object-oriented) to showcase different programming paradigms.

### 🏗️ **Why Two Implementations?**
- **C Version**: Demonstrates manual memory management, structs, pointers, and file I/O — ideal for embedded systems and performance-critical applications.
- **C++ Version**: Showcases OOP principles (classes, inheritance, polymorphism), STL containers, smart pointers (RAII), and modern C++17 features.

This dual approach allows developers to:
- Compare idioms and design patterns across languages.
- Understand trade-offs between procedural and object-oriented paradigms.
- Learn best practices for memory management, modularity, and code organization.

---

## 🏛️ System Architecture Overview

Below is a simplified ASCII diagram illustrating the core components and control flow:

```
┌─────────────┐     Vehicle     ┌──────────────────┐     Allocate    ┌─────────────────┐
│ Entry Gate  │  ──────────────>│  Slot Manager    │ ───────────────>│  Parking Lot    │
│  (Input)    │   Detection     │  (Allocator)     │   Nearest Slot  │  (100 Slots)    │
└─────────────┘                 └──────────────────┘                 └─────────────────┘
       │                                 │                                    │
       │ License Plate                   │ Timestamp                          │
       │ Vehicle Type                    │ Billing Logic                      │ Persist State
       v                                 v                                    v
┌─────────────┐                 ┌──────────────────┐                 ┌─────────────────┐
│  User Input │                 │  Billing Engine  │                 │   CSV Database  │
│  (CLI/UI)   │                 │  (Duration → ₹)  │                 │  parking_state  │
└─────────────┘                 └──────────────────┘                 │  transactions   │
                                                                      └─────────────────┘
                                         │                                    │
                                         │ Generate Receipt                   │
                                         v                                    v
                                ┌──────────────────┐                 ┌─────────────────┐
                                │  Display/Output  │ <───────────────│  Reports & Logs │
                                │  (Console/UI)    │     Query Data  └─────────────────┘
                                └──────────────────┘
```

### 🔄 **Control Flow (Simplified):**

1. **Vehicle Arrival (Entry)**:
   - User inputs vehicle number, type (Bike/Car/Truck), and owner details.
   - System checks available slots (5 floors × 20 spots = 100 total).
   - Assigns the nearest available slot using a first-fit algorithm.
   - Records entry timestamp and updates `parking_state.csv`.

2. **Vehicle Departure (Exit)**:
   - User enters vehicle license plate.
   - System retrieves entry timestamp from the database.
   - Calculates parking duration (exit time - entry time).
   - Computes billing based on vehicle type and duration.
   - Generates a receipt, frees the slot, and logs the transaction to `transactions.csv`.

3. **Real-Time Queries**:
   - **Search Vehicle**: Locate a vehicle by license plate and display its floor/spot.
   - **View Occupancy**: Show floor-wise and total occupancy statistics.
   - **Revenue Reports**: Display today's earnings and all-time revenue.
   - **Peak Hour Analysis**: Identify the busiest entry times.

### 💾 **Storage Layer:**
- **C Version**: Uses `data-c/` directory with CSV files for lightweight persistence.
- **C++ Version**: Uses `data-cpp/` directory (can be extended to SQLite or JSON).

---

## ✨ Features

### 🅿️ **Core Functionality**

- ✅ **Vehicle Entry & Exit Tracking**: Log every vehicle's arrival and departure with precise timestamps.
- ✅ **Dynamic Slot Allocation**: Automatically assigns the nearest available parking spot (first-fit algorithm).
- ✅ **Real-Time Availability Updates**: Console-based live display of occupied/free slots.
- ✅ **Automated Billing System**: Calculates fees based on:
  - Vehicle type (Bike: ₹20/hr base, Car: ₹40/hr, Truck: ₹60/hr)
  - Parking duration (rounded up to the next hour)
- ✅ **Receipt Generation**: Prints detailed parking receipts with duration, fee, and timestamps.
- ✅ **Search Functionality**: Find vehicles by license plate and display their location.
- ✅ **Admin & User Modes**: 
  - **User Mode**: Park, exit, search vehicles.
  - **Admin Mode**: View reports, clear logs, reset system.
- ✅ **CSV-Based Persistence**: Lightweight data storage for demo/testing (no external database required).
- ✅ **Multi-Vehicle Support**: Handles Bikes, Cars, and Trucks with differential pricing.

### 📊 **Reports & Analytics**

- **Occupancy Report**: Floor-by-floor breakdown of occupied vs. available slots.
- **Revenue Report**: Displays today's earnings and cumulative revenue.
- **Peak Entry Hour Analysis**: Identifies the busiest time for vehicle entries (useful for traffic management).
- **Transaction History**: View all past transactions with timestamps and fees.

### 🔒 **Data Integrity**

- **Duplicate Prevention**: Prevents parking the same vehicle twice without exit.
- **Validation**: Checks for valid license plates, slot numbers, and vehicle types.
- **Persistence**: All data survives program restarts (CSV files are loaded on startup).

---

## 🛠️ Tech Stack

| Component           | Technology                          | Purpose                                      |
|---------------------|-------------------------------------|----------------------------------------------|
| 💻 **Languages**    | C (procedural), C++ (OOP)           | Dual implementations for learning & comparison |
| 🧠 **Data Structures** | Arrays, Structs, Classes, Vectors, Queues | Efficient slot management & vehicle tracking |
| ⚙️ **Compilers**    | GCC (C), G++ (C++)                  | MinGW-w64, MSYS2, or native Linux/macOS toolchains |
| 🧩 **IDE**          | VS Code (recommended)               | Integrated terminal, debugging, extensions    |
| 📂 **Storage**      | CSV Files                           | Lightweight persistence (can upgrade to SQLite) |
| 🔧 **Build Tools**  | Manual compilation / VS Code Tasks  | Simple `gcc`/`g++` commands or automated tasks |
| 🧪 **Testing**      | Manual CLI testing                  | Test cases in `USER_MANUAL.md`              |

### 🎨 **Optional Tools:**
- **Git**: Version control and collaboration.
- **Make / CMake**: For larger projects (optional here).
- **SQLite**: For production-grade persistence (future enhancement).

---

## 📂 Folder Structure

```
SmartParkingSystem/
│
├── C_Version/
│   ├── main.c                  # C implementation entry point (procedural)
│   └── parking-c.exe           # Compiled executable (generated, not tracked)
│
├── CPP_Version/
│   ├── main.cpp                # C++ implementation entry point (OOP)
│   └── parking-cpp.exe         # Compiled executable (generated, not tracked)
│
├── data-c/
│   ├── parking_state.csv       # Runtime parking state (C version)
│   └── transactions.csv        # Logged transactions (C version)
│
├── data-cpp/                   # (Optional) C++ version data directory
│   ├── parking_state.csv
│   └── transactions.csv
│
├── DESIGN.md                   # System design, flowcharts, and architecture
├── PERFORMANCE.md              # Time/space complexity analysis
├── USER_MANUAL.md              # Detailed build/run instructions & test cases
├── DELIVERABLES.md             # Project deliverables and milestones
├── TESTING_GUIDE.md            # Comprehensive testing strategies
├── ENTRY_TIME_INFO.md          # Entry time tracking implementation notes
└── README.md                   # This file (you are here!)
```

### 📝 **File Descriptions:**

- **`C_Version/main.c`**: Procedural C implementation with structs, pointers, and manual memory management.
- **`CPP_Version/main.cpp`**: Object-oriented C++ implementation with classes, inheritance, STL containers, and smart pointers.
- **`data-c/`**: Directory for C version's CSV files (auto-generated on first run).
- **`DESIGN.md`**: High-level design decisions, Mermaid flowcharts, and data models.
- **`PERFORMANCE.md`**: Big-O complexity analysis for key operations (search, insert, delete).
- **`USER_MANUAL.md`**: Step-by-step usage guide with test scenarios.

---

## 🚀 Getting Started

Follow these steps to compile and run the Smart Parking System on your local machine.

### Prerequisites

Before you begin, ensure you have the following installed:

#### 🖥️ **Windows:**
- **MinGW-w64** or **MSYS2** with GCC/G++ toolchain.
- Add `<mingw>/bin` to your system PATH.
- Verify installation:
  ```powershell
  gcc --version
  g++ --version
  ```

#### 🐧 **Linux:**
- Install build essentials:
  ```bash
  sudo apt-get install build-essential
  ```

#### 🍎 **macOS:**
- Install Xcode Command Line Tools:
  ```bash
  xcode-select --install
  ```

#### 📝 **IDE (Optional but Recommended):**
- **VS Code** with the following extensions:
  - "C/C++" by Microsoft
  - "Code Runner" (optional, for quick runs)

### Compile & Run (PowerShell / VS Code Terminal)

Open the integrated terminal in VS Code (`` Ctrl+` ``) or a standalone PowerShell window. Navigate to the workspace root directory.

#### 🔵 **C Version**

```powershell
# Compile the C version
gcc "C_Version/main.c" -o "C_Version/parking-c.exe"

# Run the compiled executable
& "C_Version/parking-c.exe"
```

#### 🟣 **C++ Version**

```powershell
# Compile the C++ version with C++17 standard
g++ -std=c++17 "CPP_Version/main.cpp" -o "CPP_Version/parking-cpp.exe"

# Run the compiled executable
& "CPP_Version/parking-cpp.exe"
```

### 🔁 **Alternative: Using Relative Paths**

If you're already inside the `C_Version` or `CPP_Version` directory:

```powershell
# For C
gcc main.c -o parking-c.exe
./parking-c.exe

# For C++
g++ -std=c++17 main.cpp -o parking-cpp.exe
./parking-cpp.exe
```

### ⚡ **Quick Run with Code Runner Extension**

If you have the **Code Runner** extension installed:
1. Open `main.c` or `main.cpp` in VS Code.
2. Press `Ctrl+Alt+N` or click the ▶️ button in the top-right corner.
3. The code will compile and run automatically in the output panel.

---

## 📸 Example Input / Output

Below are realistic test scenarios demonstrating the system's functionality.

### 🚙 **C Version Example**

**Scenario: Park a Car and Exit After 2 Hours**

```
========================================
   SMART PARKING SYSTEM (C Version)
========================================

Main Menu:
1. Park Vehicle
2. Exit Vehicle
3. Search Vehicle
4. View Occupancy
5. Reports
6. Admin Panel
0. Quit

Enter your choice: 1

--- Park Vehicle ---
Enter Vehicle Number: UP32 AB 1234
Enter Owner Name: Rajesh Kumar
Select Vehicle Type:
  1. Bike
  2. Car
  3. Truck
Enter type (1-3): 2

Available Slots: [Floor 0: Spots 1-20 available]
Nearest Slot: Floor 0, Spot 5

✅ Vehicle Parked Successfully!
   Vehicle: UP32 AB 1234
   Owner: Rajesh Kumar
   Type: Car
   Location: Floor 0, Spot 5
   Entry Time: 2025-10-22 10:15:30

Press Enter to continue...

[User exits and re-enters the vehicle after 2 hours]

Enter your choice: 2

--- Exit Vehicle ---
Enter Vehicle Number: UP32 AB 1234

Found Vehicle:
   Location: Floor 0, Spot 5
   Entry Time: 2025-10-22 10:15:30
   Exit Time: 2025-10-22 12:18:45
   Duration: 2 hours 3 minutes

Calculating Bill...
   Base Rate (Car): ₹40 (first hour)
   Additional Hours: 1 × ₹20 = ₹20
   Total Bill: ₹60

Receipt:
┌─────────────────────────────────────┐
│       PARKING RECEIPT               │
├─────────────────────────────────────┤
│ Vehicle: UP32 AB 1234               │
│ Type: Car                           │
│ Entry: 2025-10-22 10:15:30          │
│ Exit: 2025-10-22 12:18:45           │
│ Duration: 2 hours 3 minutes         │
│ Total Fee: ₹60                      │
└─────────────────────────────────────┘

Transaction logged to data-c/transactions.csv

Press Enter to continue...
```

---

### 🏍️ **C++ Version Example**

**Scenario: Park a Bike and Exit After 45 Minutes**

```
========================================
  SMART PARKING SYSTEM (C++ Version)
========================================
[INFO] Initializing... Loaded 0 vehicles from data-cpp/parking_state.csv

Main Menu:
1. Park Vehicle
2. Exit Vehicle
3. Search Vehicle
4. View Occupancy Report
5. Revenue Report
6. Peak Hour Analysis
0. Exit System

Enter command: 1

--- Park Vehicle ---
Vehicle Number: KA01 XY 9999
Owner Name: Priya Sharma
Vehicle Type (1=Bike, 2=Car, 3=Truck): 1

[INFO] Allocating nearest slot...
✅ Slot Assigned: Floor 0, Spot 2

Parking Details:
   Vehicle: KA01 XY 9999
   Owner: Priya Sharma
   Type: Bike
   Location: Floor 0, Spot 2
   Entry Time: 2025-10-22 14:30:00

[INFO] State saved to data-cpp/parking_state.csv

Press Enter to return to menu...

[User exits after 45 minutes]

Enter command: 2

--- Exit Vehicle ---
Vehicle Number: KA01 XY 9999

[INFO] Searching for vehicle...
Found at: Floor 0, Spot 2

Exit Time: 2025-10-22 15:15:00
Duration: 45 minutes (rounded to 1 hour)
Amount Due: ₹20 (Bike: ₹20 for first hour)

Receipt:
╔═════════════════════════════════════╗
║       PARKING RECEIPT               ║
╠═════════════════════════════════════╣
║ Vehicle: KA01 XY 9999               ║
║ Type: Bike                          ║
║ Entry: 2025-10-22 14:30:00          ║
║ Exit: 2025-10-22 15:15:00           ║
║ Duration: 45 minutes                ║
║ Total Fee: ₹20                      ║
╚═════════════════════════════════════╝

[INFO] Slot 2 freed.
[INFO] Transaction logged to data-cpp/transactions.csv

Press Enter to return to menu...
```

---

### 📊 **Reports Example**

```
Enter command: 5

--- Revenue Report ---
Today's Revenue: ₹280
Total Lifetime Revenue: ₹4,560

Transaction Summary:
   Total Transactions: 38
   Bikes: 12 (₹520)
   Cars: 20 (₹2,800)
   Trucks: 6 (₹1,240)

Press Enter to continue...
```

---

## 🖥️ VS Code Environment Setup

Set up a professional development environment in VS Code for seamless coding, debugging, and testing.

### 🔧 **Step 1: Install Required Extensions**

1. Open VS Code.
2. Go to Extensions (`Ctrl+Shift+X`).
3. Install:
   - **C/C++** (by Microsoft) — IntelliSense, debugging, code navigation.
   - **Code Runner** (optional) — Quick run button for single files.

### 🔧 **Step 2: Configure Build Tasks**

Create a `.vscode/tasks.json` file in your workspace root for automated builds:

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "build-c",
      "type": "shell",
      "command": "gcc",
      "args": [
        "${workspaceFolder}/C_Version/main.c",
        "-o",
        "${workspaceFolder}/C_Version/parking-c.exe"
      ],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": ["$gcc"],
      "detail": "Compile C version"
    },
    {
      "label": "build-cpp",
      "type": "shell",
      "command": "g++",
      "args": [
        "-std=c++17",
        "${workspaceFolder}/CPP_Version/main.cpp",
        "-o",
        "${workspaceFolder}/CPP_Version/parking-cpp.exe"
      ],
      "group": "build",
      "problemMatcher": ["$gcc"],
      "detail": "Compile C++ version"
    },
    {
      "label": "run-c",
      "type": "shell",
      "command": "${workspaceFolder}/C_Version/parking-c.exe",
      "dependsOn": "build-c",
      "detail": "Build and run C version"
    },
    {
      "label": "run-cpp",
      "type": "shell",
      "command": "${workspaceFolder}/CPP_Version/parking-cpp.exe",
      "dependsOn": "build-cpp",
      "detail": "Build and run C++ version"
    }
  ]
}
```

**Usage:**
- Press `Ctrl+Shift+B` to open the build menu.
- Select `build-c` or `build-cpp` to compile.
- Run tasks from the Command Palette (`Ctrl+Shift+P` → "Tasks: Run Task").

---

### 🐞 **Step 3: Configure Debugging (Optional)**

Create `.vscode/launch.json` for debugging:

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "Debug C Version",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/C_Version/parking-c.exe",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}/C_Version",
      "environment": [],
      "externalConsole": true,
      "MIMode": "gdb",
      "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "build-c"
    },
    {
      "name": "Debug C++ Version",
      "type": "cppdbg",
      "request": "launch",
      "program": "${workspaceFolder}/CPP_Version/parking-cpp.exe",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${workspaceFolder}/CPP_Version",
      "environment": [],
      "externalConsole": true,
      "MIMode": "gdb",
      "miDebuggerPath": "C:/msys64/mingw64/bin/gdb.exe",
      "setupCommands": [
        {
          "description": "Enable pretty-printing for gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "build-cpp"
    }
  ]
}
```

**Note:** Update `miDebuggerPath` to match your GDB installation path.

**Usage:**
- Press `F5` to start debugging.
- Set breakpoints by clicking left of line numbers.

---

### ⚙️ **Step 4: Workspace Settings (Optional)**

Create `.vscode/settings.json` for consistent formatting and IntelliSense:

```json
{
  "C_Cpp.default.cppStandard": "c++17",
  "C_Cpp.default.cStandard": "c11",
  "C_Cpp.default.compilerPath": "C:/msys64/mingw64/bin/g++.exe",
  "files.trimTrailingWhitespace": true,
  "files.insertFinalNewline": true,
  "editor.formatOnSave": true,
  "editor.tabSize": 4,
  "editor.insertSpaces": true
}
```

---

## 🧩 Modularity & Data Structures

### 🔵 **C Version Architecture**

**Design Philosophy:** Procedural, modular functions with manual memory management.

**Key Data Structures:**

```c
// Parking Spot Structure
typedef struct {
    int floor;
    int spot_number;
    bool is_occupied;
    char license_plate[20];
    char owner_name[50];
    int vehicle_type; // 1=Bike, 2=Car, 3=Truck
    time_t entry_time;
} ParkingSpot;

// Transaction Record
typedef struct {
    char license_plate[20];
    int vehicle_type;
    time_t entry_time;
    time_t exit_time;
    int duration_minutes;
    float fee;
} Transaction;
```

**Core Functions:**

- `void initializeParkingLot()` — Set up 100 spots (5 floors × 20 spots).
- `int findNearestSlot()` — First-fit algorithm to find available slot.
- `void parkVehicle(char* license, char* owner, int type)` — Park a vehicle.
- `void exitVehicle(char* license)` — Process exit and billing.
- `float calculateBill(int type, int minutes)` — Compute parking fee.
- `void saveState()` — Write `parking_state.csv`.
- `void loadState()` — Read `parking_state.csv` on startup.
- `void logTransaction(Transaction* t)` — Append to `transactions.csv`.

**Memory Management:**
- Uses `malloc()`/`free()` for dynamic allocations.
- Manual pointer arithmetic for array traversal.

---

### 🟣 **C++ Version Architecture**

**Design Philosophy:** Object-oriented, RAII, STL containers, polymorphism.

**Key Classes:**

```cpp
// Base Vehicle Class (Abstract)
class Vehicle {
protected:
    string licensePlate;
    string ownerName;
    time_t entryTime;
public:
    virtual float calculateFee(int minutes) = 0; // Pure virtual
    virtual string getType() = 0;
    virtual ~Vehicle() = default;
};

// Derived Classes
class Bike : public Vehicle {
public:
    float calculateFee(int minutes) override {
        int hours = (minutes + 59) / 60; // Round up
        return (hours == 1) ? 20 : 20 + (hours - 1) * 10;
    }
    string getType() override { return "Bike"; }
};

class Car : public Vehicle { /* Similar */ };
class Truck : public Vehicle { /* Similar */ };

// Parking Lot Manager
class ParkingLot {
private:
    array<array<unique_ptr<Vehicle>, 20>, 5> spots; // Smart pointers!
    vector<Transaction> transactions;
public:
    int findNearestSlot();
    void parkVehicle(unique_ptr<Vehicle> vehicle);
    void exitVehicle(const string& license);
    void saveState();
    void loadState();
};
```

**Why This Design?**

- **Inheritance & Polymorphism**: Different vehicle types share common interface but have unique billing logic.
- **Smart Pointers (`unique_ptr`)**: Automatic memory management, no manual `delete`.
- **STL Containers**: `std::vector` for transactions, `std::array` for fixed-size parking grid.
- **RAII**: Resources (file handles, memory) are automatically released when objects go out of scope.

**Advantages Over C:**
- Cleaner code, less boilerplate.
- Type safety and compile-time checks.
- Exception handling for error management.
- Easier to extend (add new vehicle types).

---

### 📚 **Why These Data Structures?**

| Structure         | Usage                          | Rationale                                   |
|-------------------|--------------------------------|---------------------------------------------|
| **Arrays**        | Parking grid (5×20)            | Constant-time access, fixed size            |
| **Structs/Classes** | Vehicle, Spot, Transaction   | Encapsulate related data                    |
| **Vectors (C++)** | Dynamic transaction list       | Flexible size, safe resizing                |
| **Queue (future)**| Waitlist for full parking     | FIFO order for queued vehicles              |
| **Hash Map (future)** | Fast license plate lookup | O(1) search instead of O(n)                 |

---

## 🛠️ Troubleshooting

Common issues and their solutions:

### ❌ **Issue 1: `'gcc' is not recognized as an internal or external command`**

**Cause:** GCC/G++ is not installed or not in PATH.

**Solution:**
1. **Windows**: Install MinGW-w64 or MSYS2.
   - Download: [MSYS2](https://www.msys2.org/)
   - After installation, run:
     ```bash
     pacman -S mingw-w64-x86_64-gcc
     ```
   - Add `C:\msys64\mingw64\bin` to your system PATH.
   - Restart VS Code/Terminal.

2. **Linux**:
   ```bash
   sudo apt-get install build-essential
   ```

3. **macOS**:
   ```bash
   xcode-select --install
   ```

**Verify Installation:**
```powershell
gcc --version
g++ --version
```

---

### ❌ **Issue 2: Permission Denied When Running `.exe`**

**Cause:** Windows security or file permissions.

**Solutions:**
- Run PowerShell/VS Code as Administrator.
- Check antivirus settings (some AVs block unsigned executables).
- Compile to a different directory (e.g., `Desktop`).

---

### ❌ **Issue 3: Missing `.exe` After Compilation**

**Cause:** Compilation failed silently or output path is incorrect.

**Solution:**
1. Check for compiler errors:
   ```powershell
   gcc "C_Version/main.c" -o "C_Version/parking-c.exe"
   ```
   Look for syntax errors in the output.

2. Verify the file was created:
   ```powershell
   ls "C_Version/"
   ```

3. If errors persist, check:
   - File paths (spaces in paths should be quoted).
   - Missing libraries or headers.

---

### ❌ **Issue 4: CSV Files Not Found / Empty State**

**Cause:** Program can't locate or create `data-c/parking_state.csv`.

**Solutions:**
- Ensure the `data-c/` directory exists:
  ```powershell
  mkdir -p data-c
  ```
- Run the program from the workspace root (not from inside `C_Version/`).
- Check file permissions (program needs write access).

---

### ❌ **Issue 5: Incorrect Bill Calculation**

**Cause:** Time zone issues or rounding errors.

**Solutions:**
- Ensure your system clock is set correctly.
- Check time zone settings (use UTC for consistency).
- Review the billing logic in `calculateBill()` function.

---

### ❌ **Issue 6: Linker Errors in C++ (`undefined reference to...`)**

**Cause:** Missing library or incorrect compilation flags.

**Solution:**
- Ensure you're using `g++` (not `gcc`) for C++ files.
- Add required flags:
  ```powershell
  g++ -std=c++17 "CPP_Version/main.cpp" -o "CPP_Version/parking-cpp.exe"
  ```

---

### 🆘 **Still Stuck?**

If you encounter a specific error not listed here:
1. **Check the Error Message**: Copy the full error output.
2. **Google the Error**: Most GCC errors have well-documented solutions.
3. **Open an Issue**: 
   - Describe your OS, compiler version (`gcc --version`), exact commands used, and full error output.
   - Include a minimal reproducible example.

---

## 🚀 Future Enhancements

Planned features and improvements:

### 🌐 **Cloud Integration**
- **Firebase/Supabase**: Replace CSV with real-time cloud database.
- **Multi-User Access**: Support multiple concurrent users/admins.
- **Remote Monitoring**: View parking status from anywhere.

### 📱 **Mobile & Web Interface**
- **React/Flutter Frontend**: Modern UI for users and admins.
- **QR Code Tickets**: Generate QR codes for vehicle entry/exit.
- **Payment Gateway**: Integrate Stripe/Razorpay for online payments.

### 🔌 **IoT & Hardware Integration**
- **Sensor Integration**: Connect RFID/ultrasonic sensors for automatic detection.
- **Raspberry Pi Deployment**: Run the system on embedded hardware.
- **License Plate Recognition**: Use OpenCV/Tesseract OCR for automatic plate reading.

### 🤖 **AI & Machine Learning**
- **Predictive Analytics**: Forecast peak hours and occupancy trends.
- **Dynamic Pricing**: Adjust rates based on demand (surge pricing).
- **Recommendation System**: Suggest optimal parking times to users.

### 🔒 **Security & Scalability**
- **User Authentication**: Add login system for users and admins.
- **Role-Based Access Control**: Different permissions for staff, admins, and users.
- **Encryption**: Secure sensitive data (payment info, personal details).
- **Load Balancing**: Handle thousands of concurrent requests.

### ♻️ **Testing & Quality Assurance**
- **Unit Tests**: Use Catch2 (C++) or CUnit (C) for automated testing.
- **CI/CD Pipeline**: GitHub Actions for automated builds and tests.
- **Performance Benchmarking**: Measure and optimize critical operations.

---

## 🤝 Contributing

We welcome contributions from developers of all skill levels!

### 📝 **How to Contribute**

1. **Fork the Repository**
   - Click the "Fork" button on GitHub.

2. **Clone Your Fork**
   ```bash
   git clone https://github.com/<your-username>/SmartParkingSystem.git
   cd SmartParkingSystem
   ```

3. **Create a Feature Branch**
   ```bash
   git checkout -b feature/add-search-optimization
   ```

4. **Make Your Changes**
   - Write clean, well-documented code.
   - Follow existing naming conventions.
   - Add comments for complex logic.

5. **Test Your Changes**
   - Compile and run both C and C++ versions.
   - Test edge cases (full parking, invalid inputs, etc.).

6. **Commit with a Clear Message**
   ```bash
   git add .
   git commit -m "feat: optimize search using hash map"
   ```

7. **Push to Your Fork**
   ```bash
   git push origin feature/add-search-optimization
   ```

8. **Open a Pull Request**
   - Go to the original repository on GitHub.
   - Click "New Pull Request" and describe your changes.

---

### 🎨 **Coding Guidelines**

#### **C Code:**
- Use `snake_case` for functions and variables.
- Example: `int find_nearest_slot()`
- Document functions with comments:
  ```c
  /**
   * Finds the nearest available parking slot.
   * @return Slot index, or -1 if none available.
   */
  int find_nearest_slot() { /* ... */ }
  ```

#### **C++ Code:**
- Use `camelCase` for methods and variables, `PascalCase` for classes.
- Example: `class ParkingLot { void parkVehicle(); }`
- Use const-correctness:
  ```cpp
  void displayReceipt(const Transaction& transaction) const;
  ```

#### **General:**
- Limit lines to 80-100 characters.
- Use consistent indentation (4 spaces, no tabs).
- Avoid magic numbers (use named constants).
- Update `DESIGN.md` and `USER_MANUAL.md` for behavioral changes.

---

### 🐛 **Reporting Bugs**

Found a bug? Open an issue with:
- **Title**: Brief description (e.g., "Crash on exit with invalid license plate")
- **Steps to Reproduce**: Exact inputs and commands.
- **Expected Behavior**: What should happen.
- **Actual Behavior**: What actually happens.
- **Environment**: OS, compiler version, VS Code version.

---

### 💡 **Suggesting Features**

Have an idea? Open an issue with the **[Feature Request]** tag and describe:
- **Problem**: What need does this address?
- **Solution**: How should it work?
- **Alternatives**: Other approaches considered.

---

## 📄 License

This project is licensed under the **MIT License**.

### MIT License Summary:
- ✅ **Use** the code for personal or commercial projects.
- ✅ **Modify** and distribute as you wish.
- ✅ **Include** the original license in distributions.
- ❌ **No warranty** — provided "as is".

For full license text, see the `LICENSE` file.

---

## 🌟 Credits

> **Developed by [Maulishka Srivastava](https://github.com/maulishka) 🌸 and Team**  
> _"Code. Innovate. Simplify Parking."_

### 🙏 **Special Thanks:**
- **Contributors**: All developers who submitted PRs and reported issues.
- **Open Source Tools**: GCC/G++, MinGW-w64, VS Code, GitHub.
- **Community**: Stack Overflow, Reddit r/C_Programming, and C++ forums.
- **Testers**: Friends, family, and beta testers who provided feedback.

### 📚 **Resources & Inspiration:**
- "The C Programming Language" by Kernighan & Ritchie
- "Effective Modern C++" by Scott Meyers
- [GeeksforGeeks](https://www.geeksforgeeks.org/) for data structures tutorials
- [cppreference.com](https://en.cppreference.com/) for C++ standard library docs

---

## 📬 Contact & Support

Have questions or need help?

- 📧 **Email**: [your-email@example.com](mailto:your-email@example.com)
- 💬 **GitHub Discussions**: [Start a discussion](https://github.com/<your-repo>/discussions)
- 🐛 **Report Issues**: [Open an issue](https://github.com/<your-repo>/issues)
- ⭐ **Star the Repo**: If you found this useful, give us a star!

---

## 🎉 Quick Recap

✨ **What You Can Do Now:**

1. ✅ Clone/download the repository.
2. ✅ Compile both C and C++ versions.
3. ✅ Test parking, exit, search, and reports.
4. ✅ Compare procedural vs. OOP implementations.
5. ✅ Extend with your own features (IoT, web UI, etc.).
6. ✅ Contribute back to the community!

---

### 🚀 **Next Steps (Optional):**

Would you like me to:
1. **Add `.vscode/launch.json` and `tasks.json`** tailored to your workspace path?
2. **Create a simple web dashboard** (HTML/JS) that reads `transactions.csv`?
3. **Convert CSV persistence to SQLite** for safer concurrent access?
4. **Write unit tests** using Catch2 (C++) or CUnit (C)?
5. **Set up GitHub Actions** for automated CI/CD?

Let me know how I can assist further! 🚗💨
