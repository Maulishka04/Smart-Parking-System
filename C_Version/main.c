// Smart Parking System - C Version
// Requirements covered:
// - 5 floors, 20 spots per floor
// - Vehicle types: Car, Bike, Truck
// - Dynamic memory for vehicle records
// - Pointers/arrays for allocation
// - File I/O persistence (CSV)
// - CLI operations: entry, exit, search, reports
// - Smart allocation: nearest spot by floor then spot
// - Billing by type and duration; reports: occupancy, revenue, peak entry hour

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h> // _mkdir
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#define FLOORS 5
#define SPOTS_PER_FLOOR 20
#define LICENSE_MAX 32
#define OWNER_MAX 64

typedef enum {
	VEHICLE_BIKE = 0,
	VEHICLE_CAR = 1,
	VEHICLE_TRUCK = 2
} VehicleType;

typedef struct {
	char license[LICENSE_MAX];
	char owner[OWNER_MAX];
	time_t entryTime;
	VehicleType type;
	int floor; // 0..FLOORS-1
	int spot;  // 0..SPOTS_PER_FLOOR-1
} Vehicle;

typedef struct {
	int occupied;     // 0 = free, 1 = occupied
	Vehicle *vehicle; // dynamically allocated when occupied
} ParkingSpot;

static ParkingSpot parkingLot[FLOORS][SPOTS_PER_FLOOR];

// Data directory and files
static const char *DATA_DIR = "data-c";
static const char *PARKING_STATE_FILE = "data-c/parking_state.csv";   // floor,spot,license,owner,type,entryTime
static const char *TRANSACTIONS_FILE = "data-c/transactions.csv";     // license,type,entryTime,exitTime,durationMin,fee

// Forward declarations
void init_parking();
void ensure_data_dir();
int save_parking_state();
int load_parking_state();
int append_transaction(const char *license, VehicleType type, time_t entry, time_t exit, long durationMin, double fee);
int find_vehicle(const char *license, int *outFloor, int *outSpot);
int park_vehicle();
int exit_vehicle();
void search_vehicle();
void reports_menu();
void report_occupancy();
void report_revenue();
void report_peak_entry_hour();
void wait_for_enter();

// Utilities
const char *vehicle_type_str(VehicleType t) {
	switch (t) {
	case VEHICLE_BIKE: return "Bike";
	case VEHICLE_CAR: return "Car";
	case VEHICLE_TRUCK: return "Truck";
	default: return "Unknown";
	}
}

int parse_vehicle_type(int choice, VehicleType *out) {
	switch (choice) {
	case 1: *out = VEHICLE_BIKE; return 1;
	case 2: *out = VEHICLE_CAR; return 1;
	case 3: *out = VEHICLE_TRUCK; return 1;
	default: return 0;
	}
}

// Billing: first hour + additional per hour (rounded up)
double calculate_fee(VehicleType type, long durationMinutes) {
	long hours = (durationMinutes + 59) / 60; // ceil to hours
	if (hours < 1) hours = 1;
	double firstHour = 0.0, addHour = 0.0;
	switch (type) {
		case VEHICLE_BIKE: firstHour = 20.0; addHour = 10.0; break;
		case VEHICLE_CAR: firstHour = 40.0; addHour = 20.0; break;
		case VEHICLE_TRUCK: firstHour = 60.0; addHour = 30.0; break;
		default: firstHour = 40.0; addHour = 20.0; break;
	}
	if (hours == 1) return firstHour;
	return firstHour + (hours - 1) * addHour;
}

void init_parking() {
	for (int f = 0; f < FLOORS; ++f) {
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
			parkingLot[f][s].occupied = 0;
			parkingLot[f][s].vehicle = NULL;
		}
	}
}

void ensure_data_dir() {
#ifdef _WIN32
	_mkdir(DATA_DIR);
#else
	mkdir(DATA_DIR, 0755);
#endif
}

int save_parking_state() {
	FILE *fp = fopen(PARKING_STATE_FILE, "w");
	if (!fp) {
		perror("Failed to open parking_state.csv for writing");
		return 0;
	}
	// header
	fprintf(fp, "floor,spot,license,owner,type,entryTime\n");
	for (int f = 0; f < FLOORS; ++f) {
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
			if (parkingLot[f][s].occupied && parkingLot[f][s].vehicle) {
				Vehicle *v = parkingLot[f][s].vehicle;
				fprintf(fp, "%d,%d,%s,%s,%d,%ld\n", v->floor, v->spot, v->license, v->owner, (int)v->type, (long)v->entryTime);
			}
		}
	}
	fclose(fp);
	return 1;
}

int load_parking_state() {
	FILE *fp = fopen(PARKING_STATE_FILE, "r");
	if (!fp) {
		// No state file yet; treat as empty lot
		return 1;
	}
	char line[512];
	// skip header
	if (!fgets(line, sizeof(line), fp)) {
		fclose(fp);
		return 1;
	}
	while (fgets(line, sizeof(line), fp)) {
		// floor,spot,license,owner,type,entryTime
		int f = 0, s = 0, typeInt = 0; long entry = 0;
		char license[LICENSE_MAX] = {0};
		char owner[OWNER_MAX] = {0};
		// Use strtok and parsing
		char *p = line;
		char *token;
		// floor
		token = strtok(p, ","); if (!token) continue; f = atoi(token);
		// spot
		token = strtok(NULL, ","); if (!token) continue; s = atoi(token);
		// license
		token = strtok(NULL, ","); if (!token) continue; strncpy(license, token, LICENSE_MAX-1);
		// owner
		token = strtok(NULL, ","); if (!token) continue; strncpy(owner, token, OWNER_MAX-1);
		// type
		token = strtok(NULL, ","); if (!token) continue; typeInt = atoi(token);
		// entryTime (may include trailing newline)
		token = strtok(NULL, ",\r\n"); if (!token) continue; entry = atol(token);
		// Trim possible newline from owner/license
		// Remove potential trailing newline/carriage return in strings
		for (size_t i = 0; i < strlen(license); ++i) { if (license[i] == '\n' || license[i] == '\r') license[i] = '\0'; }
		for (size_t i = 0; i < strlen(owner); ++i) { if (owner[i] == '\n' || owner[i] == '\r') owner[i] = '\0'; }

		if (f >= 0 && f < FLOORS && s >= 0 && s < SPOTS_PER_FLOOR) {
			Vehicle *v = (Vehicle*)malloc(sizeof(Vehicle));
			if (!v) continue;
			strncpy(v->license, license, LICENSE_MAX-1); v->license[LICENSE_MAX-1] = '\0';
			strncpy(v->owner, owner, OWNER_MAX-1); v->owner[OWNER_MAX-1] = '\0';
			v->entryTime = (time_t)entry;
			v->type = (VehicleType)typeInt;
			v->floor = f; v->spot = s;
			parkingLot[f][s].occupied = 1;
			parkingLot[f][s].vehicle = v;
		}
	}
	fclose(fp);
	return 1;
}

int append_transaction(const char *license, VehicleType type, time_t entry, time_t exit, long durationMin, double fee) {
	FILE *fp = fopen(TRANSACTIONS_FILE, "a");
	if (!fp) {
		// Try to create with header then append
		fp = fopen(TRANSACTIONS_FILE, "w");
		if (!fp) { perror("Failed to open transactions.csv"); return 0; }
		fprintf(fp, "license,type,entryTime,exitTime,durationMin,fee\n");
		fclose(fp);
		fp = fopen(TRANSACTIONS_FILE, "a");
		if (!fp) { perror("Failed to reopen transactions.csv"); return 0; }
	}
	fprintf(fp, "%s,%d,%ld,%ld,%ld,%.2f\n", license, (int)type, (long)entry, (long)exit, durationMin, fee);
	fclose(fp);
	return 1;
}

int find_vehicle(const char *license, int *outFloor, int *outSpot) {
	for (int f = 0; f < FLOORS; ++f) {
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
			if (parkingLot[f][s].occupied && parkingLot[f][s].vehicle) {
				if (strcmp(parkingLot[f][s].vehicle->license, license) == 0) {
					if (outFloor) *outFloor = f;
					if (outSpot) *outSpot = s;
					return 1;
				}
			}
		}
	}
	return 0;
}

// Smart allocation: scan floors then spots to find nearest to entrance (floor 0, spot 0)
int find_nearest_spot(int *outFloor, int *outSpot) {
	for (int f = 0; f < FLOORS; ++f) {
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
			if (!parkingLot[f][s].occupied) {
				*outFloor = f;
				*outSpot = s;
				return 1;
			}
		}
	}
	return 0;
}

void read_line(char *buffer, size_t size) {
	if (!fgets(buffer, (int)size, stdin)) {
		buffer[0] = '\0';
		return;
	}
	size_t len = strlen(buffer);
	if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) buffer[len-1] = '\0';
	// Remove possible remaining CR if CRLF
	len = strlen(buffer);
	if (len > 0 && (buffer[len-1] == '\r' || buffer[len-1] == '\n')) buffer[len-1] = '\0';
}

int park_vehicle() {
	int typeChoice = 0;
	char license[LICENSE_MAX];
	char owner[OWNER_MAX];

	printf("\n=== Vehicle Entry ===\n");
	printf("Select vehicle type:\n");
	printf("1. Bike\n2. Car\n3. Truck\n> ");
	if (scanf("%d", &typeChoice) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); return 0; }
	while (getchar()!='\n'); // clear line
	VehicleType type;
	if (!parse_vehicle_type(typeChoice, &type)) { printf("Invalid type selection.\n"); return 0; }

	printf("License plate: ");
	read_line(license, sizeof(license));
	if (license[0] == '\0') { printf("License cannot be empty.\n"); return 0; }

	// Ensure license not already parked
	int ef, es;
    if (find_vehicle(license, &ef, &es)) {
        printf("Vehicle with license %s is already parked at Floor %d, Spot %d.\n", license, ef+1, es+1);
        return 0;
    }	printf("Owner contact/name: ");
	read_line(owner, sizeof(owner));
	if (owner[0] == '\0') { printf("Owner cannot be empty.\n"); return 0; }

	int f, s;
	if (!find_nearest_spot(&f, &s)) {
		printf("Parking full. No available spots.\n");
		return 0;
	}

	Vehicle *v = (Vehicle*)malloc(sizeof(Vehicle));
	if (!v) { printf("Memory allocation failed.\n"); return 0; }
	strncpy(v->license, license, LICENSE_MAX-1); v->license[LICENSE_MAX-1] = '\0';
	strncpy(v->owner, owner, OWNER_MAX-1); v->owner[OWNER_MAX-1] = '\0';
	v->type = type;
	v->entryTime = time(NULL);
	v->floor = f; v->spot = s;

    parkingLot[f][s].occupied = 1;
    parkingLot[f][s].vehicle = v;

    if (!save_parking_state()) {
        printf("Warning: failed to persist parking state.\n");
    }

    // Display entry time
    char entryBuf[64];
    struct tm *tmEntry = localtime(&v->entryTime);
    strftime(entryBuf, sizeof(entryBuf), "%Y-%m-%d %H:%M:%S", tmEntry);
    printf("Assigned Floor %d, Spot %d.\n", f+1, s+1);
    printf("Entry time: %s\n", entryBuf);
    return 1;
}int exit_vehicle() {
	char license[LICENSE_MAX];
	printf("\n=== Vehicle Exit ===\n");
	printf("Enter license plate: ");
	read_line(license, sizeof(license));
	if (license[0] == '\0') { printf("License cannot be empty.\n"); return 0; }

	int f = -1, s = -1;
	if (!find_vehicle(license, &f, &s)) {
		printf("Vehicle with license %s not found.\n", license);
		return 0;
	}

	Vehicle *v = parkingLot[f][s].vehicle;
	time_t now = time(NULL);
	long durationMin = (long) difftime(now, v->entryTime) / 60;
	if (durationMin <= 0) durationMin = 1; // minimum chargeable minute
	double fee = calculate_fee(v->type, durationMin);

	// Receipt
	char entryBuf[64], exitBuf[64];
	struct tm *tmEntry = localtime(&v->entryTime);
	struct tm *tmExit = localtime(&now);
	strftime(entryBuf, sizeof(entryBuf), "%Y-%m-%d %H:%M:%S", tmEntry);
	strftime(exitBuf, sizeof(exitBuf), "%Y-%m-%d %H:%M:%S", tmExit);
	printf("\n--- Receipt ---\n");
	printf("License: %s\n", v->license);
	printf("Type: %s\n", vehicle_type_str(v->type));
	printf("Entry: %s\n", entryBuf);
	printf("Exit:  %s\n", exitBuf);
	printf("Duration: %ld min\n", durationMin);
	printf("Fee: %.2f\n", fee);

	// Append transaction
	if (!append_transaction(v->license, v->type, v->entryTime, now, durationMin, fee)) {
		printf("Warning: failed to record transaction.\n");
	}

	// Free spot
	parkingLot[f][s].occupied = 0;
	parkingLot[f][s].vehicle = NULL;
	free(v);

	if (!save_parking_state()) {
		printf("Warning: failed to persist parking state.\n");
	}

	return 1;
}

void search_vehicle() {
	char license[LICENSE_MAX];
	printf("\n=== Search Vehicle ===\n");
	printf("Enter license plate: ");
	read_line(license, sizeof(license));
	if (license[0] == '\0') { printf("License cannot be empty.\n"); return; }

	int f = -1, s = -1;
	if (!find_vehicle(license, &f, &s)) {
		printf("Vehicle with license %s not found.\n", license);
		return;
	}
    Vehicle *v = parkingLot[f][s].vehicle;
    char entryBuf[64];
    struct tm *tmEntry = localtime(&v->entryTime);
    strftime(entryBuf, sizeof(entryBuf), "%Y-%m-%d %H:%M:%S", tmEntry);
    printf("Found: Floor %d, Spot %d, Type: %s, Owner: %s, Entry: %s\n",
           f+1, s+1, vehicle_type_str(v->type), v->owner, entryBuf);
}

void report_occupancy() {
	int totalOcc = 0;
	printf("\n=== Occupancy Report ===\n");
	for (int f = 0; f < FLOORS; ++f) {
		int occ = 0;
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) if (parkingLot[f][s].occupied) ++occ;
		totalOcc += occ;
		double rate = (100.0 * occ) / SPOTS_PER_FLOOR;
		printf("Floor %d: %d/%d (%.1f%%)\n", f+1, occ, SPOTS_PER_FLOOR, rate);
	}
	double overall = (100.0 * totalOcc) / (FLOORS * SPOTS_PER_FLOOR);
	printf("Overall: %d/%d (%.1f%%)\n", totalOcc, FLOORS * SPOTS_PER_FLOOR, overall);
}

void report_revenue() {
	FILE *fp = fopen(TRANSACTIONS_FILE, "r");
	if (!fp) {
		printf("No transactions yet.\n");
		return;
	}
	char line[512];
	// skip header
	if (!fgets(line, sizeof(line), fp)) { fclose(fp); printf("No transactions yet.\n"); return; }
	double total = 0.0;
	double today = 0.0;
	time_t now = time(NULL);
	struct tm tmNow; tmNow = *localtime(&now);
	int y = tmNow.tm_year, m = tmNow.tm_mon, d = tmNow.tm_mday;
	while (fgets(line, sizeof(line), fp)) {
		// license,type,entryTime,exitTime,durationMin,fee
		char *token = NULL; char *p = line;
		// license
		token = strtok(p, ","); if (!token) continue;
		// type
		token = strtok(NULL, ","); if (!token) continue;
		// entry
		token = strtok(NULL, ","); if (!token) continue; long entry = atol(token);
		// exit
		token = strtok(NULL, ","); if (!token) continue; long exitT = atol(token);
		// duration
		token = strtok(NULL, ","); if (!token) continue;
		// fee
		token = strtok(NULL, ",\r\n"); if (!token) continue; double fee = atof(token);
		total += fee;
		time_t exit_time = (time_t)exitT;
		struct tm tme = *localtime(&exit_time);
		if (tme.tm_year == y && tme.tm_mon == m && tme.tm_mday == d) {
			today += fee;
		}
	}
	fclose(fp);
	printf("Revenue (today): %.2f\n", today);
	printf("Revenue (total): %.2f\n", total);
}

void report_peak_entry_hour() {
	FILE *fp = fopen(TRANSACTIONS_FILE, "r");
	int counts[24] = {0};
	if (fp) {
		char line[512];
		if (fgets(line, sizeof(line), fp)) {
			while (fgets(line, sizeof(line), fp)) {
				// license,type,entryTime,exitTime,durationMin,fee
				char *token = NULL; char *p = line;
				// license
				token = strtok(p, ","); if (!token) continue;
				// type
				token = strtok(NULL, ","); if (!token) continue;
				// entry
				token = strtok(NULL, ","); if (!token) continue; long entry = atol(token);
				time_t entry_time = (time_t)entry;
				struct tm tme = *localtime(&entry_time);
				int hour = tme.tm_hour;
				if (hour >= 0 && hour < 24) counts[hour]++;
			}
		}
		fclose(fp);
	}
	// Also include currently parked vehicles' entry hours
	for (int f = 0; f < FLOORS; ++f) {
		for (int s = 0; s < SPOTS_PER_FLOOR; ++s) {
			if (parkingLot[f][s].occupied && parkingLot[f][s].vehicle) {
				time_t entry = parkingLot[f][s].vehicle->entryTime;
				struct tm tme = *localtime(&entry);
				int hour = tme.tm_hour; if (hour >= 0 && hour < 24) counts[hour]++;
			}
		}
	}
	int maxHour = 0, maxCount = counts[0];
	for (int h = 1; h < 24; ++h) {
		if (counts[h] > maxCount) { maxCount = counts[h]; maxHour = h; }
	}
	printf("\n=== Peak Entry Hour ===\n");
	if (maxCount == 0) {
		printf("No data available yet.\n");
	} else {
		printf("Busiest entry hour: %02d:00-%02d:00 with %d entries (historical + current).\n", maxHour, (maxHour+1)%24, maxCount);
	}
}

void reports_menu() {
	int choice = 0;
	while (1) {
		printf("\n=== Reports ===\n");
		printf("1. Occupancy\n");
		printf("2. Revenue\n");
		printf("3. Peak Entry Hour\n");
		printf("4. Back\n> ");
		if (scanf("%d", &choice) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); continue; }
		while (getchar()!='\n');
		if (choice == 1) { report_occupancy(); }
		else if (choice == 2) { report_revenue(); }
		else if (choice == 3) { report_peak_entry_hour(); }
		else if (choice == 4) { break; }
		else { printf("Invalid choice.\n"); }
	}
}

void wait_for_enter() {
	printf("\nPress Enter to continue...");
	fflush(stdout);
	while (1) { int c = getchar(); if (c == '\n' || c == '\r' || c == EOF) break; }
}

int main() {
	ensure_data_dir();
	init_parking();
	load_parking_state();

	int choice = 0;
	while (1) {
		printf("\n==============================\n");
		printf(" Smart Parking System (C)\n");
		printf(" Floors: %d, Spots/Floor: %d\n", FLOORS, SPOTS_PER_FLOOR);
		printf("==============================\n");
		printf("1. Vehicle Entry (Park)\n");
		printf("2. Vehicle Exit\n");
		printf("3. Search Vehicle\n");
		printf("4. Reports\n");
		printf("5. Save & Exit\n> ");

		if (scanf("%d", &choice) != 1) { while (getchar()!='\n'); printf("Invalid input.\n"); continue; }
		while (getchar()!='\n'); // clear trailing

		switch (choice) {
			case 1: park_vehicle(); break;
			case 2: exit_vehicle(); break;
			case 3: search_vehicle(); break;
			case 4: reports_menu(); break;
			case 5:
				if (!save_parking_state()) {
					printf("Warning: failed to save parking state.\n");
				}
				printf("Goodbye!\n");
				return 0;
			default:
				printf("Invalid choice.\n");
		}
	}
}

