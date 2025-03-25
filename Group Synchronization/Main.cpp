#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <random>
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;

mutex mtx;
condition_variable cv;
bool simulationDone = false;

class DungeonInstance {
public:
	long id;
    bool active = false;
    long partiesServed = 0;
    long totalTimeServed = 0;
};

void dungeonInstance(DungeonInstance& instance, int minTime, int maxTime) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(minTime, maxTime);

    while (true) {
        {
            unique_lock<mutex> lock(mtx);

            cv.wait(lock, [&]() { return  instance.active || simulationDone; });
            if (simulationDone && !instance.active) {
                break; // Exit if simulation is done and instance is not active
            }

            // Simulate dungeon run
            int duration = distrib(gen);
            cout << "Instance " << instance.id + 1 << " is active. Time: " << duration << "s\n";
			instance.active = false;
            instance.partiesServed++;
            instance.totalTimeServed += duration;
            cout << "Instance " << instance.id + 1 << " has finished. Now empty.\n";

            cv.notify_all(); // Notify waiting threads
        }
    }
}

int main()
{
    /*long n, t, h, d, t1, t2;*/
    int n = 10, t = 3904, h = 3124, d = 51501, t1 = 5, t2 = 15;

    if (t2 > 15) {
        cout << "Maximum clear time cannot exceed 15 seconds." << endl;
        return 1;
    }
    
    vector<DungeonInstance> instances(n);
    vector<thread> threads;
    for (int i = 0; i < n; i++) {
		instances[i].id = i;
        threads.emplace_back(dungeonInstance, ref(instances[i]), t1, t2);
    }


    while (t >= 1 && h >= 1 && d >= 3) {
        unique_lock<mutex> lock(mtx);

        // Find empty instance
        long instanceID = -1;
		for (int i = 0; i < n; i++) {
			if (!instances[i].active) {
				instanceID = i;
                break;
			}
		}

		if (instanceID == -1) {
            cv.wait(lock);
			continue;
		}

        // Form party
        t -= 1;
		h -= 1;
		d -= 3;
		instances[instanceID].active = true;
		
		cv.notify_all();
    }
	
    {
        unique_lock<mutex> lock(mtx);
        simulationDone = true;
        cv.notify_all(); // Notify all threads to stop
    }
    
    // Cleanup
    for (auto& th : threads) {
        th.join();
    }


    cout << "\nSummary:\n";
	for (int i = 0; i < n; i++) {
		cout << "Instance " << i + 1<< " served " << instances[i].partiesServed << " parties for a total of " << instances[i].totalTimeServed << "s\n";
	}

	return 0;

}

//bool readConfig() {
//	string fileContent, key, value;
//	ifstream ConfigFile("config.txt");
//
//	// Check if the file opened successfully
//	if (!ConfigFile.is_open()) {
//		cerr << "Error: Could not open config.txt" << endl;
//		return false;
//	}
//
//	bool hasThreads = false, hasMax = false; // Track if both values are set
//
//	while (getline(ConfigFile, fileContent)) {
//		istringstream iss(fileContent);
//
//		if (!(iss >> key >> value)) {
//			cerr << "Warning: Invalid line in config file: " << fileContent << endl;
//			continue; // Skip to next line
//		}
//
//		try {
//			if (key == "x") {
//				numThreads = stoi(value);
//				hasThreads = true;
//			}
//			else if (key == "y") {
//				max = stoi(value);
//				hasMax = true;
//			}
//			else {
//				cerr << "Warning: Unknown config key: " << key << endl;
//			}
//		}
//		catch (const invalid_argument&) {
//			cerr << "Error: Invalid number format in config file: " << value << endl;
//			continue; // Skip this line
//		}
//		catch (const out_of_range&) {
//			cerr << "Error: Number out of range in config file: " << value << endl;
//			continue; // Skip this line
//		}
//	}
//
//	ConfigFile.close();
//
//	// Ensure both x (numThreads) and y (max) are set
//	if (!hasThreads || !hasMax) {
//		cerr << "Error: Missing required config values (x and y)" << endl;
//		return false;
//	}
//
//	// Validate the parsed values
//	if (numThreads <= 0) {
//		cerr << "Error: Number of threads must be greater than zero. Setting to 1." << endl;
//		numThreads = 1;
//	}
//	if (max <= 0) {
//		cerr << "Error: Max value must be greater than zero. Setting to 100." << endl;
//		max = 100;
//	}
//	if (numThreads > max) {
//		cerr << "Error: Number of threads cannot be greater than max. Setting numThreads = max." << endl;
//		numThreads = max;
//	}
//
//	return true; // Both values were successfully read and validated
//}