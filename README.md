# 🏰 **Dungeon Simulation**  

This project simulates a dungeon instance management system using multithreading in **C++**. Players are assigned to dungeon instances where they complete runs of random durations. Each instance operates independently using threads.  

---

## 📦 **Project Structure**  

- **DungeonSimulation.sln** — Visual Studio Solution file.  
- **DungeonSimulation.vcxproj** — Visual Studio project file.  
- **main.cpp** — Contains the core logic for the simulation.  
- **config.txt** — Configuration file to customize the simulation parameters.  
- **README.md** — This file.  

---

## ⚙️ **Prerequisites**  

- Visual Studio (Recommended 2022 or later)  
- C++17 or later  
- Windows OS  

---

## 🚀 **How to Build and Run**  

### **Option 1: Using Visual Studio**  

1. **Open Project**:  
    - Double-click `DungeonSimulation.sln` to open it in Visual Studio.  

2. **Configure**:  
    - Ensure the `config.txt` file is in the project directory.  
    - Modify parameters as needed (explained below).  

3. **Build**:  
    - Go to `Build` → `Build Solution` or press `Ctrl + Shift + B`.  

4. **Run**:  
    - Press `F5` to run with debugging, or  
    - Go to `Debug` → `Start Without Debugging` to run without debugging.  

---

### **Option 2: Using the Console (CMD/PowerShell)**  

1. **Open Command Prompt**:  
    - Press `Win + R`, type `cmd`, and press `Enter`.  

2. **Navigate to Project Folder**:  
    ```bash
    cd path\to\your\project
    ```

3. **Compile the Program**:  
    Ensure you have a compiler like `g++` installed (part of MinGW or installed via Visual Studio). Then run:  
    ```bash
    g++ main.cpp -o DungeonSimulation -std=c++17 -lpthread
    ```

4. **Run the Program**:  
    ```bash
    DungeonSimulation.exe
    ```

5. **Ensure Config File**:  
    - Ensure `config.txt` is in the same directory as the executable.  

---

## 🛠️ **Configuration**  

The simulation uses `config.txt` for input values. The file should be formatted like this:  

```plaintext
n 3      # Number of dungeon instances
t 10     # Number of tanks
h 10     # Number of healers
d 30     # Number of DPS
t1 1     # Minimum dungeon run time (in seconds)
t2 15    # Maximum dungeon run time (in seconds)
```

## 🧑‍💻 **How It Works**  

- The simulation reads values from `config.txt`.  
- Each dungeon instance runs on its own thread using `std::thread`.  
- Mutexes (`std::mutex`) and condition variables (`std::condition_variable`) are used to synchronize the instances.  
- Players are assigned to free dungeon instances.  
- When a dungeon is completed, it becomes available for the next party.  
- The results are displayed once the simulation finishes.  

---

## 📊 **Output Example**  

```plaintext
Instance 1 is active. Time: 12s
Instance 2 is active. Time: 11s
Instance 3 is active. Time: 3s
Instance 3 has finished. Now empty.
Instance 3 is active. Time: 11s
Instance 2 has finished. Now empty.
Instance 2 is active. Time: 12s
Instance 1 has finished. Now empty.
Instance 3 has finished. Now empty.
Instance 2 has finished. Now empty.

Summary:
Instance 1 served 1 parties for a total of 12s
Instance 2 served 2 parties for a total of 23s
Instance 3 served 2 parties for a total of 14s

Remaining:
Tanks: 5
Healers: 0
DPS: 15
```

