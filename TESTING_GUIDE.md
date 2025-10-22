# Testing Billing with Custom Entry Times

## 🎯 New Features Added

### 1. **User-Friendly Display**
- ✅ Floors now display as **1 to 5** (instead of 0-4)
- ✅ Spots now display as **1 to 20** (instead of 0-19)

### 2. **Custom Entry Time for Testing**
When parking a vehicle, you can now set how long ago it entered to test billing calculations!

## 📋 How to Test Billing

### Example 1: Test 2-Hour Parking Fee for Car

```
> 1 (Vehicle Entry)
Type: 2 (Car)
License: TEST001
Owner: Test User
Set custom entry time? (y/n): y
Hours ago vehicle entered: 2
Entry time set to 2 hours ago.
Assigned Floor 1, Spot 1

> 2 (Vehicle Exit)
License: TEST001

Receipt will show:
- Duration: ~120 minutes
- Fee: 60.00 (₹40 first hour + ₹20 second hour)
```

### Example 2: Test 5-Hour Parking for Truck

```
> 1
Type: 3 (Truck)
License: TRUCK01
Owner: Driver Name
Set custom entry time? (y/n): y
Hours ago vehicle entered: 5
Assigned Floor 1, Spot 2

> 2
License: TRUCK01

Receipt will show:
- Duration: ~300 minutes
- Fee: 180.00 (₹60 + 4×₹30)
```

### Example 3: Normal Entry (Current Time)

```
> 1
Type: 1 (Bike)
License: BIKE123
Owner: Owner Name
Set custom entry time? (y/n): n
# Entry time = current time

> 2
License: BIKE123
# Will show minimal fee if exited immediately
```

## 💰 Pricing Reminder

| Vehicle | First Hour | Each Additional Hour |
|---------|-----------|---------------------|
| Bike    | ₹20       | ₹10                 |
| Car     | ₹40       | ₹20                 |
| Truck   | ₹60       | ₹30                 |

**Duration is rounded UP to the next hour**
- 0-60 min = 1 hour
- 61-120 min = 2 hours
- 121-180 min = 3 hours, etc.

## 🧪 Complete Test Scenarios

### Scenario 1: Same-Hour Exit
```
Entry: Set to 0 hours ago (now)
Exit: Immediate
Expected Fee: First hour rate (₹20/₹40/₹60)
```

### Scenario 2: Multi-Hour Parking
```
Car Entry: 3 hours ago
Car Exit: Now
Duration: 3 hours
Fee Calculation: ₹40 + (2 × ₹20) = ₹80
```

### Scenario 3: Partial Hour (Rounds Up)
```
Bike Entry: Set manually to 90 minutes ago
Exit: Now
Duration: 90 min → rounds to 2 hours
Fee: ₹20 + ₹10 = ₹30
```

### Scenario 4: Long-Term Parking
```
Truck Entry: 24 hours ago
Exit: Now
Duration: 24 hours
Fee: ₹60 + (23 × ₹30) = ₹750
```

## 🎮 Quick Test Commands

Run the program and try these:

```powershell
# Start C version
& "SmartParkingSystem/C_Version/parking-c.exe"

# Or C++ version
& "SmartParkingSystem/CPP_Version/parking-cpp.exe"
```

**Test Sequence:**
1. Park 3 vehicles with different types and different entry times (0hrs, 2hrs, 5hrs ago)
2. Exit each one and verify fees
3. View Reports → Revenue to see total earnings
4. Park same vehicles again to test persistence

## 📊 Expected Outputs

### Floor Display (New Format)
```
Assigned Floor 1, Spot 1  ✅ (was Floor 0, Spot 0)
Assigned Floor 1, Spot 2  ✅
Assigned Floor 2, Spot 1  ✅ (when Floor 1 is full)
```

### Search Output
```
Found at Floor 1, Spot 5: ABC123 (Car) owner=John ...
```

### Occupancy Report
```
Floor 1: 5/20 (25.0%)
Floor 2: 3/20 (15.0%)
Floor 3: 0/20 (0.0%)
Floor 4: 0/20 (0.0%)
Floor 5: 0/20 (0.0%)
Overall: 8/100 (8.0%)
```

---

**Both versions now support:**
- ✅ 1-based floor/spot numbering for user display
- ✅ Manual entry time setting for testing
- ✅ Accurate billing calculations
- ✅ All original features intact
