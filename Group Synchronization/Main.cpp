#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>

using namespace std;

struct DungeonInstance {
	long id = 0;
	bool active = false;
	long partiesServed = 0;
	long totalTimeServed = 0;
	mutex mtx;
	condition_variable cv;
};

bool simulationDone = false;
mutex coutMutex;

void dungeonInstance(DungeonInstance& instance, int minTime, int maxTime) {
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> distrib(minTime, maxTime);

	while (true) {
		unique_lock<mutex> lock(instance.mtx);

		instance.cv.wait(lock, [&]() { return instance.active || simulationDone; });
		if (simulationDone && !instance.active) {
			break; // Exit if simulation is done and instance is not active
		}

		// Simulate dungeon run
		int duration = distrib(gen);
		{
			lock_guard<mutex> printLock(coutMutex);
			cout << "Instance " << instance.id + 1 << " is active. Time: " << duration << "s" << endl;
		}

		/*this_thread::sleep_for(chrono::seconds(duration));*/
		instance.active = false;
		instance.partiesServed++;
		instance.totalTimeServed += duration;
		{
			lock_guard<mutex> printLock(coutMutex);
			cout << "Instance " << instance.id + 1 << " has finished. Now empty." << endl;
		}

		instance.cv.notify_one(); // Notify the main thread
	}
}

bool isValidInteger(const string& value) {
	static const regex integerRegex(R"(^-?\d+$)");
	return regex_match(value, integerRegex);
}

bool readConfig(long& n, long& t, long& h, long& d, long& t1, long& t2) {
	string fileContent, key, value;
	ifstream ConfigFile("config.txt");

	// Check if the file opened successfully
	if (!ConfigFile.is_open()) {
		cerr << "Error: Could not open config.txt" << endl;
		return false;
	}

	bool hasN = false, hasT = false, hasH = false, hasD = false, hasT1 = false, hasT2 = false;

	while (getline(ConfigFile, fileContent)) {
		istringstream iss(fileContent);

		if (!(iss >> key >> value)) {
			cerr << "Warning: Invalid line in config file: " << fileContent << endl;
			continue; // Skip to next line
		}

		try {
			if (!isValidInteger(value)) {
				cerr << "Error: Invalid number format in config file: " << value << endl;
				continue;
			}

			if (key == "n") {
				n = stol(value);
				hasN = true;
			}
			else if (key == "t") {
				t = stol(value);
				hasT = true;
			}
			else if (key == "h") {
				h = stol(value);
				hasH = true;
			}
			else if (key == "d") {
				d = stol(value);
				hasD = true;
			}
			else if (key == "t1") {
				t1 = stol(value);
				hasT1 = true;
			}
			else if (key == "t2") {
				t2 = stol(value);
				hasT2 = true;
			}
			else {
				cerr << "Warning: Unknown config key: " << key << endl;
			}
		}
		catch (const out_of_range&) {
			cerr << "Error: Number out of range in config file: " << value << endl;
			continue; // Skip this line
		}
	}

	ConfigFile.close();

	if (!hasN || !hasT || !hasH || !hasD || !hasT1 || !hasT2) {
		cout << n << endl;
		cerr << "Error: Missing required config values (n, t, h, d, t1, t2)" << endl;
		return false;
	}

	if (n < 1) {
		cerr << "Error: n must be greater than 0." << endl;
		return false;
	}

	if (t < 1) {
		cerr << "Error: t must be greater than 0." << endl;
		return false;
	}

	if (h < 1) {
		cerr << "Error: h must be greater than 0." << endl;
		return false;
	}

	if (d < 3) {
		cerr << "Error: d must be greater than or equal to 0." << endl;
		return false;
	}

	if (t1 > t2) {
		cerr << "Error: t2 must be greater than t1." << endl;
		return false;
	}

	if (t2 > 15) {
		cerr << "Error: t2 must be less than or equal to 15. Setting t2 = 15" << endl;
		t2 = 15;
	}
	return true; // Both values were successfully read and validated
}


int main() {
	long n, t, h, d, t1, t2;

	if (!readConfig(n, t, h, d, t1, t2)) {
		return 1;
	}

	vector<DungeonInstance> instances(n);
	vector<thread> threads;
	for (int i = 0; i < n; i++) {
		instances[i].id = i;
		threads.emplace_back(dungeonInstance, ref(instances[i]), t1, t2);
	}

	while (t >= 1 && h >= 1 && d >= 3) {
		// Find an empty instance
		long instanceID = -1;
		for (int i = 0; i < n; i++) {
			unique_lock<mutex> lock(instances[i].mtx);
			if (!instances[i].active) {
				// Form a party
				instanceID = i;
				instances[i].active = true;
				instances[i].cv.notify_one();
				break;
			}
		}

		if (instanceID == -1) {
			continue;
		}

		// Decrease the number of available players
		t -= 1;
		h -= 1;
		d -= 3;
	}

	// End the simulation
	simulationDone = true;

	for (auto& instance : instances) {
		unique_lock<mutex> lock(instance.mtx);
		instance.cv.notify_one();
	}

	for (auto& th : threads) {
		th.join();
	}

	cout << "\nSummary:\n";
	for (int i = 0; i < n; i++) {
		cout << "Instance " << i + 1 << " served " << instances[i].partiesServed << " parties for a total of " << instances[i].totalTimeServed << "s\n";
	}
	cout << "\nRemaining: \n";
	cout << "Tanks: " << t << endl << "Healers: " << h << endl << "DPS: " << d << endl;

	return 0;
}