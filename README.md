# 🏟️ ENTRY QUEUE MANAGEMENT SYSTEM

**PROJECT ID:** P3  

## 👥 TEAM MEMBERS
- Jiya Patel – 202301034  
- Hiya Modi – 202301011  
- Yashasvi Jadav – 202301069  
- Kavya Parmar – 202301085  

**TEAM ID:** SeeksForGeeks  

---

## 🧠 Efficiency & Data Structure Rationale

This project simulates and optimizes crowd entry into a stadium with multiple entry gates.  
The system is designed to **minimize total entry time** while ensuring **thread safety, scalability, and accurate performance measurement**.

Unlike the initial version, the current implementation **automatically assigns spectators to the optimal gate** based on real-time queue lengths. This removes reliance on manual user decisions and guarantees deterministic, optimal behavior.

---

## ✅ Key Data Structures Used

### **Array of Deques (`deque<int> Queue[N]`)
**
Each entry gate is represented by a separate deque.

**Why deque?**
- O(1) insertion and deletion at both ends — ideal for queue operations  
- Efficient front removal for gate processing  
- Dynamic resizing without costly reallocations  

---

### **Array of Pairs (`pair<short int, int> SerialStat[M]`)
**
Stores the state and gate assignment of each spectator.

- **first (status):**
  - `0` – not yet arrived  
  - `1` – currently waiting in a queue  
  - `2` – already entered the stadium  
- **second** – assigned gate number  

**Why this structure?**
- Enables O(1) access using serial number as index  
- Eliminates expensive scans across all queues  
- Simplifies state transitions (arrival → queued → entered)  

---

### **Arrival & Entry Time Arrays**
```cpp
arrival_time[M]
entry_time[M]
```
Purpose:

Record arrival and entry times

Compute average and maximum waiting time

Provide measurable proof of time minimization

Counter Class
Tracks:

Number of spectators who have entered

Number of VIPs (who bypass queues)

Used to determine correct termination of the simulation.

Stopwatch Class
Uses high_resolution_clock

Simulates real-world time (1 second = 1 minute)

Provides:

Current simulated time

Total elapsed simulation time

⏱️ Time & Space Complexity
Time Complexity
Initialization & random assignment: O(M)

Queue balancing: O(M)

Auto-assignment decision: O(N) per arrival

Auto-dequeue processing: O(M) over time

Space Complexity
O(M + N)

M spectators tracked

N independent gate queues

🎯 Why This Design Is Efficient
Guarantees optimal gate assignment (auto-allocation)

Prevents suboptimal human decisions

Fully thread-safe using fine-grained mutexes

Supports parallel gate processing via background thread

Collects real performance metrics instead of assumptions

Clean separation between arrival logic and processing logic

⚙️ Algorithm Overview
1. Initialize Constants
M – Stadium capacity

N – Number of entry gates

p – Time per spectator (simulated)

2. VIP Identification (HashFunction)
Marks a small fraction (~10%) of spectators as VIPs

VIPs bypass queues and enter immediately

Prevents VIPs from skewing queue metrics

3. Initialization (CreateSerialNo)
Initializes spectator states

Records VIPs as already entered

Prepares arrival and entry time arrays

4. Initial Random Assignment (AssignRandomGate)
Randomly assigns M/2 spectators to gates

Models real-world pre-existing queues

Avoids VIPs and duplicate assignments

5. Load Balancing (Distribute)
Redistributes spectators across gates

Prevents heavily skewed queues

Improves throughput and tail latency

6. Optimal Gate Selection (FindBestGate)
Selects gate with minimum queue length

Implements argmin(queue_size × p)

Replaces manual gate selection from earlier versions

7. Auto-Dequeue Thread (AutoDequeue)
Runs concurrently with user input

Every simulated minute:

One spectator enters from each gate

Updates entry times and progress counters

8. Main Execution Flow
Accepts serial numbers interactively

Validates serial and spectator state

Displays:

Estimated waiting time

Recommended gate(s)

Automatically assigns spectator to optimal gate

Remaining spectators are auto-assigned after input ends

System waits until all non-VIPs have entered

📊 Output Metrics
After simulation completion, the system reports:

Average waiting time

Maximum waiting time

Total simulated entry time

These metrics validate that the system successfully minimizes total entry time.

✅ Summary of Improvements Over Initial Version
Manual queue selection → Automatic optimal assignment

No performance proof → Measured wait-time metrics

Potential early termination → Correct completion guarantee

Simulation-only → Optimizer-backed system
