# Smart Parking System - User Manual

This manual explains how to build, run, and use the C and C++ versions of the Smart Parking System on Windows (PowerShell).

## Build and Run

### C Version
1. Open PowerShell in the project root.
2. Build using GCC (from MinGW or similar) and run:
```powershell
# Build
gcc "SmartParkingSystem/C_Version/main.c" -o "SmartParkingSystem/C_Version/parking-c.exe"
# Run
& "SmartParkingSystem/C_Version/parking-c.exe"
```

### C++ Version
```powershell
# Build (use a compiler that supports C++17)
g++ -std=c++17 "SmartParkingSystem/CPP_Version/main.cpp" -o "SmartParkingSystem/CPP_Version/parking-cpp.exe"
# Run
& "SmartParkingSystem/CPP_Version/parking-cpp.exe"
```

Note: Ensure `gcc`/`g++` are installed and on PATH (e.g., via MinGW-w64 or MSYS2). If using Visual Studio, create a Console Application and add the source files accordingly.

## Using the Application
- Menu options:
  1. Vehicle Entry (Park): choose type (Bike/Car/Truck), enter license and owner. The system assigns the nearest spot.
  2. Vehicle Exit: enter license; system calculates duration and fee, frees the spot, and records a transaction.
  3. Search Vehicle: find where a license is parked and view details.
  4. Reports: occupancy, revenue (today/total), and peak entry hour.
  5. Save & Exit: writes current state to disk and exits.

## Data Files
- C version:
  - `SmartParkingSystem/C_Version/data-c/parking_state.csv`
  - `SmartParkingSystem/C_Version/data-c/transactions.csv`
- C++ version:
  - `SmartParkingSystem/CPP_Version/data-cpp/parking_state.csv`
  - `SmartParkingSystem/CPP_Version/data-cpp/transactions.csv`

These are created automatically on first run.

## Sample Test Cases

1. Park vehicles until first floor fills
   - Enter 20 vehicles; confirm spots 0..19 on floor 0 fill in order.
2. Prevent duplicate parking
   - Try to park the same license twice; expect an error.
3. Exit billing
   - Park a Car; wait 61 minutes (or temporarily edit the entry time in the CSV to simulate) and exit; fee should be 60.00 (40 + 20).
4. Reports
   - After several entries/exits, open Reports to verify occupancy and revenue numbers.
5. Persistence
   - Park a few vehicles, exit program, restart; verify vehicles are restored to their spots.

## Troubleshooting
- If CSV files are locked or cannot be created, ensure you have write permissions to the project folder.
- If builds fail due to missing headers, install a complete toolchain (MinGW-w64, MSYS2, or Visual Studio Build Tools).

