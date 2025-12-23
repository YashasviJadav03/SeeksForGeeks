#include <iostream>
#include <chrono>
#include <thread>
#include <utility>
#include <deque>
#include <vector>
#include <mutex>
#include <atomic>
#include <random>
#include <algorithm>
#include <limits>

using namespace std;
using namespace std::chrono;

const int M = 10; // Total people
const int N = 2;  // Gates
const int p = 1;  // Logical minutes per person (simulated)

// ================= COUNTER =================
class Counter
{
    atomic<int> Done;

public:
    atomic<int> count_VIP;

    Counter() : Done(0), count_VIP(0) {}

    void increase() { Done.fetch_add(1, memory_order_relaxed); }

    bool isEmpty()
    {
        return Done.load() >= (M - count_VIP.load());
    }
};

// ================= STOPWATCH =================
class Stopwatch
{
    time_point<high_resolution_clock> start;

public:
    Stopwatch() : start(high_resolution_clock::now()) {}

    long elapsedSeconds()
    {
        return duration_cast<seconds>(high_resolution_clock::now() - start).count();
    }

    long currentMinutes()
    {
        return elapsedSeconds(); // 1 second = 1 simulated minute
    }
};

// ================= VIP CHECK =================
int HashFunction(int s)
{
    int divisor = max(10, M); // Rare VIPs (~10%)
    return !(s % divisor);
}

// ================= INIT =================
void CreateSerialNo(pair<short, int> (&SerialStat)[M],
                    Counter &counter,
                    long arrival_time[M],
                    long entry_time[M])
{

    for (int i = 0; i < M; i++)
    {
        SerialStat[i] = {0, -1};
        arrival_time[i] = -1;
        entry_time[i] = -1;

        if (HashFunction(i))
        {
            counter.count_VIP++;
            SerialStat[i].first = 2; // VIP enters immediately
            arrival_time[i] = 0;
            entry_time[i] = 0;
        }
    }
}

// ================= RANDOM INITIAL ASSIGNMENT =================
void AssignRandomGate(pair<short, int> (&SerialStat)[M],
                      deque<int> (&Queue)[N],
                      mutex gate_mutex[N])
{

    mt19937 rng(42);
    uniform_int_distribution<int> gate_dist(0, N - 1);
    uniform_int_distribution<int> serial_dist(0, M - 1);

    int assigned = 0;
    while (assigned < M / 2)
    {
        int s = serial_dist(rng);
        if (SerialStat[s].first == 0)
        {
            int g = gate_dist(rng);
            lock_guard<mutex> lock(gate_mutex[g]);
            Queue[g].push_back(s);
            SerialStat[s] = {1, g};
            assigned++;
        }
    }
}

// ================= LOAD BALANCING =================
void Distribute(deque<int> (&Queue)[N], mutex gate_mutex[N])
{
    long limit = M / N;
    vector<int> buffer;

    for (int i = 0; i < N; i++)
    {
        lock_guard<mutex> lock(gate_mutex[i]);
        while ((long)Queue[i].size() > limit)
        {
            buffer.push_back(Queue[i].front());
            Queue[i].pop_front();
        }
    }

    for (int i = 0; i < N && !buffer.empty(); i++)
    {
        lock_guard<mutex> lock(gate_mutex[i]);
        while ((long)Queue[i].size() < limit && !buffer.empty())
        {
            Queue[i].push_back(buffer.back());
            buffer.pop_back();
        }
    }
}

// ================= FIND BEST GATE =================
int FindBestGate(deque<int> (&Queue)[N], mutex gate_mutex[N])
{
    int best = 0;
    size_t min_size = numeric_limits<size_t>::max();

    for (int i = 0; i < N; i++)
    {
        lock_guard<mutex> lock(gate_mutex[i]);
        if (Queue[i].size() < min_size)
        {
            min_size = Queue[i].size();
            best = i;
        }
    }
    return best;
}

// ================= AUTO DEQUEUE =================
void AutoDequeue(deque<int> (&Queue)[N],
                 pair<short, int> (&SerialStat)[M],
                 Counter &counter,
                 mutex gate_mutex[N],
                 atomic<bool> &stop_flag,
                 long entry_time[M],
                 Stopwatch &timer)
{

    while (!stop_flag && !counter.isEmpty())
    {
        this_thread::sleep_for(seconds(1)); // responsive simulation

        for (int i = 0; i < N; i++)
        {
            lock_guard<mutex> lock(gate_mutex[i]);
            if (!Queue[i].empty())
            {
                int s = Queue[i].front();
                Queue[i].pop_front();
                SerialStat[s].first = 2;
                entry_time[s] = timer.currentMinutes();
                counter.increase();
            }
        }
    }
}

// ================= MAIN =================
int main()
{
    pair<short, int> SerialStat[M];
    deque<int> Queue[N];
    mutex gate_mutex[N];
    atomic<bool> stop_flag(false);
    long arrival_time[M], entry_time[M];

    Counter counter;
    Stopwatch timer;

    CreateSerialNo(SerialStat, counter, arrival_time, entry_time);
    AssignRandomGate(SerialStat, Queue, gate_mutex);
    Distribute(Queue, gate_mutex);

    cout << "===== ENTRY QUEUE MANAGEMENT SYSTEM =====\n";
    cout << "Simulation started (1 second = 1 minute)\n";
    cout << "Enter serials between 1000000 and " << (1000000 + M - 1) << "\n";
    cout << "Press Ctrl+Z (Windows) to stop manual input\n\n"
         << flush;

    thread worker(AutoDequeue,
                  ref(Queue),
                  ref(SerialStat),
                  ref(counter),
                  gate_mutex,
                  ref(stop_flag),
                  entry_time,
                  ref(timer));
    // ================= MANUAL INPUT LOOP =================
    while (true)
    {
        int sr;
        cout << "\n----------------------------------\n";
        cout << "Welcome to the Entry Queue Management System!\n";
        cout << "Please enter your 7-digit serial number: " << flush;

        if (!(cin >> sr))
            break;

        sr -= 1000000;

        if (sr < 0 || sr >= M)
        {
            cout << "Invalid serial number. Please try again.\n";
            continue;
        }

        // Case 1: Already entered
        if (SerialStat[sr].first == 2)
        {
            cout << "You have already entered the stadium. Re-entry not allowed.\n";
            continue;
        }

        // Case 2: Already in queue
        if (SerialStat[sr].first == 1)
        {
            cout << "You are already in a queue at Gate "
                 << (SerialStat[sr].second + 1) << ".\n";
            cout << "Queue switching is currently disabled in auto-mode.\n";
            continue;
        }

        // Case 3: VIP
        if (HashFunction(sr))
        {
            cout << "VIP detected! You may enter immediately.\n";
            arrival_time[sr] = timer.currentMinutes();
            entry_time[sr] = arrival_time[sr];
            SerialStat[sr].first = 2;
            continue;
        }

        // Case 4: Normal arrival
        arrival_time[sr] = timer.currentMinutes();

        // Snapshot queue sizes for suggestion
        vector<size_t> snapshot(N);
        for (int i = 0; i < N; i++)
        {
            lock_guard<mutex> lock(gate_mutex[i]);
            snapshot[i] = Queue[i].size();
        }

        size_t min_q = *min_element(snapshot.begin(), snapshot.end());

        cout << "Estimated waiting time: "
             << max<size_t>(1, min_q) * p << " minutes\n";

        cout << "Recommended gate(s): ";
        for (int i = 0; i < N; i++)
            if (snapshot[i] == min_q)
                cout << (i + 1) << " ";
        cout << "\n";

        int g = FindBestGate(Queue, gate_mutex);

        SerialStat[sr] = {1, g};
        {
            lock_guard<mutex> lock(gate_mutex[g]);
            Queue[g].push_back(sr);
        }

        cout << "You have been assigned to Gate " << (g + 1) << ".\n";
    }

    // ================= AUTO-ASSIGN REMAINING ARRIVALS =================
    for (int i = 0; i < M; i++)
    {
        if (SerialStat[i].first == 0)
        {
            arrival_time[i] = timer.currentMinutes();
            int g = FindBestGate(Queue, gate_mutex);
            SerialStat[i] = {1, g};
            lock_guard<mutex> lock(gate_mutex[g]);
            Queue[g].push_back(i);
        }
    }

    // ================= WAIT FOR ALL TO ENTER =================
    while (!counter.isEmpty())
    {
        this_thread::sleep_for(milliseconds(100));
    }

    stop_flag = true;
    worker.join();

    // ================= METRICS =================
    long total_wait = 0, max_wait = 0, count = 0;
    for (int i = 0; i < M; i++)
    {
        if (arrival_time[i] >= 0 && entry_time[i] >= 0)
        {
            long w = entry_time[i] - arrival_time[i];
            total_wait += w;
            max_wait = max(max_wait, w);
            count++;
        }
    }

    cout << "\n===== METRICS =====\n";
    cout << "Average wait: " << (count ? total_wait / count : 0) << " minutes\n";
    cout << "Max wait: " << max_wait << " minutes\n";
    cout << "Total time: " << timer.elapsedSeconds() << " seconds\n";
    cout << "===================\n";
}
