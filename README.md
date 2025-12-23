# 🏟️ Entry Queue Management System

**Project ID:** `P3`  
**Team ID:** `SeeksForGeeks`

## 👥 Team Members
- **Jiya Patel** – 202301034  
- **Hiya Modi** – 202301011  
- **Yashasvi Jadav** – 202301069  
- **Kavya Parmar** – 202301085  

---

## 🧠 Efficiency & Data Structure Rationale

This project simulates and optimizes crowd entry into a stadium with multiple entry gates. The system is designed to **minimize total entry time** while ensuring:
- Thread safety
- Scalability
- Accurate performance measurement

### 🔄 Key Improvement
Unlike the initial version, the current implementation **automatically assigns spectators to the optimal gate** based on real-time queue lengths, removing reliance on manual user decisions and guaranteeing deterministic, optimal behavior.

---

## ✅ Key Data Structures

### 1. Array of Deques
```cpp
deque<int> Queue[N];  // N = number of gates
```

**Purpose:**
- Each entry gate is represented by a separate deque

**Why Deque?**
- O(1) insertion and deletion at both ends
- Efficient front removal for gate processing
- Dynamic resizing without costly reallocations

### 2. Array of Pairs
```cpp
pair<short int, int> SerialStat[M];  // M = number of spectators
```

**Structure:**
- **First Element (status):**
  - `0` – Not yet arrived
  - `1` – Currently waiting in a queue
  - `2` – Already entered the stadium
- **Second Element** – Assigned gate number

**Advantages:**
- O(1) access using serial number as index
- Eliminates expensive scans across all queues
- Simplifies state transitions (arrival → queued → entered)

### 3. Time Tracking Arrays
```cpp
int arrival_time[M];  // When each spectator arrives
int entry_time[M];    // When each spectator enters
```

**Purpose:**
- Record precise timing data
- Compute performance metrics:
  - Average waiting time
  - Maximum waiting time
  - Total processing time

### 4. Counter Class
```cpp
class Counter {
    int total_entered;
    int vip_count;
    // ...
};
```

**Tracks:**
- Number of spectators who have entered
- Number of VIPs (who bypass queues)
- Determines simulation termination

### 5. Stopwatch Class
```cpp
class Stopwatch {
    using clock = std::chrono::high_resolution_clock;
    // ...
};
```

**Features:**
- Uses `high_resolution_clock` for precision
- Simulates real-world time (1 second = 1 minute)
- Tracks current and total elapsed simulation time

---

## ⏱️ Performance Analysis

### Time Complexity
- **Worst-case:** O(M × N)
- **Practical (when N is small):** O(M)
- **Space Complexity:** O(M + N) ≈ O(M)

### Key Features
- Linear scaling with number of spectators
- Real-time processing
- Thread-safe implementation using fine-grained mutexes
- Optimal gate assignment through auto-allocation

### System Features
- Supports parallel gate processing via background thread
- Collects real performance metrics instead of assumptions
- Clean separation between arrival logic and processing logic

---

## ⚙️ Algorithm Overview

### 1. Initialize Constants
```cpp
const int M = 1000;  // Stadium capacity
const int N = 5;     // Number of entry gates
const int p = 1;     // Time per spectator (simulated minutes)
```

### 2. VIP Identification (`HashFunction`)
- Marks ~10% of spectators as VIPs
- VIPs bypass queues and enter immediately
- Prevents VIPs from skewing queue metrics

### 3. Initialization (`CreateSerialNo`)
- Initializes spectator states
- Records VIPs as already entered
- Prepares arrival and entry time arrays

### 4. Initial Random Assignment (`AssignRandomGate`)
- Randomly assigns M/2 spectators to gates
- Models real-world pre-existing queues
- Avoids VIPs and duplicate assignments

### 5. Load Balancing (`Distribute`)
- Redistributes spectators across gates
- Prevents heavily skewed queues
- Improves throughput and tail latency

### 6. Optimal Gate Selection (`FindBestGate`)
```cpp
int FindBestGate() {
    int min_queue = INT_MAX;
    int best_gate = 0;
    for (int i = 0; i < N; i++) {
        if (Queue[i].size() < min_queue) {
            min_queue = Queue[i].size();
            best_gate = i;
        }
    }
    return best_gate;
}
```
- Selects gate with minimum queue length
- Implements `argmin(queue_size × p)`
- Replaces manual gate selection from earlier versions

### 7. Auto-Dequeue Thread (`AutoDequeue`)
- Runs concurrently with user input
- Every simulated minute:
  - One spectator enters from each gate
  - Updates entry times and progress counters

### 8. Main Execution Flow
1. Accepts serial numbers interactively
2. Validates serial and spectator state
3. Displays:
   - Estimated waiting time
   - Recommended gate(s)
4. Automatically assigns spectator to optimal gate
5. Remaining spectators are auto-assigned after input ends
6. System waits until all non-VIPs have entered

---

## 📊 Output Metrics
After simulation completion, the system reports:
- **Average waiting time**
- **Maximum waiting time**
- **Total simulated entry time**

These metrics validate that the system successfully minimizes total entry time.

---

## 🚀 Summary of Improvements

| Initial Version | Current Version |
|----------------|-----------------|
| Manual queue selection | Automatic optimal assignment |
| Single-threaded | Multi-threaded with background processing |
| Basic metrics | Comprehensive performance tracking |
| Static queue assignment | Dynamic load balancing |
| No VIP handling | VIP support with queue bypass |

---

## 🏁 Getting Started

### Prerequisites
- C++17 or later
- Thread support in standard library

### Building
```bash
g++ -std=c++17 -pthread main.cpp -o entry_system
```

### Running
```bash
./entry_system
```

### Input Format
- Enter spectator serial numbers (1 to M)
- Type 'done' to start auto-assignment of remaining spectators

---
