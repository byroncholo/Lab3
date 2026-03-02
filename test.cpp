/*
General Plan for the Empirical Analysis of Algorithm Time Efficiency
(Towers of Hanoi: Recursive vs Iterative)

1) Purpose:
   Empirically compare runtime of recursive and iterative Hanoi and verify
   consistency with mathematical analysis: moves = 2^n - 1, time = Θ(2^n).

2) Efficiency metric M + unit:
   - Primary: runtime (microseconds)
   - Secondary: operation count (number of moves), unit = "moves"

3) Input characteristics:
   Input size = number of disks n, tested over a range [nMin, nMax].

4) Program implementation:
   - Implement recursive and iterative algorithms in "count-only" mode (no printing)
     to avoid I/O dominating the timing.
   - Run multiple trials per n and compute mean time (and min time for reference).
   - Record results in CSV (Excel-compatible).

5) Generate a sample of inputs:
   n = nMin..nMax (uniform sample).

6) Run and record:
   For each n record:
   expectedMoves, recursiveMoves, recursiveMeanTime_us, recursiveMinTime_us,
   iterativeMoves, iterativeMeanTime_us, iterativeMinTime_us

7) Analyze:
   Plot in Excel:
   - time vs n
   - time vs expectedMoves
   - (optional) ratio time(n)/time(n-1)

Name: Byron Cholo Zambonino
Date: 3/1/2026
*/

#include <iostream>
#include <stack>
#include <tuple>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;
using namespace std::chrono;

// ----------------------------
// Utilities
// ----------------------------
static inline uint64_t expected_moves(int n) {
    // 2^n - 1, safe for n <= 63
    if (n >= 63) return UINT64_MAX;
    return (1ULL << n) - 1ULL;
}

static inline uint64_t now_us() {
    return (uint64_t)duration_cast<microseconds>(
               high_resolution_clock::now().time_since_epoch())
        .count();
}

// ----------------------------
// Recursive Hanoi (count-only)
// moves = number of "move operations"
// ----------------------------
void hanoi_recursive_count(int n, uint64_t &moves) {
    if (n == 0) return;
    hanoi_recursive_count(n - 1, moves);
    ++moves; // conceptual "move disk"
    hanoi_recursive_count(n - 1, moves);
}

// ----------------------------
// Iterative Hanoi (count-only)
// Simulates recursion using explicit stack frames
//
// Frame: (k, s, t, a, stage)
// stage=0: expand first call
// stage=1: do middle move and expand second call
// ----------------------------
void hanoi_iterative_count(int n, char source, char target, char auxiliary, uint64_t &moves) {
    stack<tuple<int, char, char, char, int>> st;
    st.push({n, source, target, auxiliary, 0});

    while (!st.empty()) {
        auto [k, s, t, a, stage] = st.top();
        st.pop();

        if (k == 0) continue;

        if (k == 1) {
            ++moves; // one move
            continue;
        }

        if (stage == 0) {
            // Simulate:
            // hanoi(k-1, s, a, t)
            // move k: s -> t
            // hanoi(k-1, a, t, s)
            //
            // Push in reverse order (LIFO):
            st.push({k,   s, t, a, 1});   // after first part, do middle+second
            st.push({k-1, s, a, t, 0});   // first part
        } else {
            ++moves;                      // middle move
            st.push({k-1, a, t, s, 0});    // second part
        }
    }
}

// ----------------------------
// Benchmark helpers
// ----------------------------
struct Stats {
    uint64_t moves = 0;      // sanity check
    double mean_us = 0.0;    // average runtime in microseconds
    double min_us = 0.0;     // minimum runtime in microseconds (useful reference)
};

Stats benchmark_recursive(int n, int trials) {
    vector<double> times;
    times.reserve(trials);

    uint64_t lastMoves = 0;

    for (int i = 0; i < trials; ++i) {
        uint64_t moves = 0;

        auto t0 = high_resolution_clock::now();
        hanoi_recursive_count(n, moves);
        auto t1 = high_resolution_clock::now();

        double us = (double)duration_cast<microseconds>(t1 - t0).count();
        times.push_back(us);
        lastMoves = moves;
    }

    double sum = 0.0;
    for (double x : times) sum += x;

    Stats s;
    s.moves = lastMoves;
    s.mean_us = sum / (double)times.size();
    s.min_us = *min_element(times.begin(), times.end());
    return s;
}

Stats benchmark_iterative(int n, int trials, char source='A', char target='C', char auxiliary='B') {
    vector<double> times;
    times.reserve(trials);

    uint64_t lastMoves = 0;

    for (int i = 0; i < trials; ++i) {
        uint64_t moves = 0;

        auto t0 = high_resolution_clock::now();
        hanoi_iterative_count(n, source, target, auxiliary, moves);
        auto t1 = high_resolution_clock::now();

        double us = (double)duration_cast<microseconds>(t1 - t0).count();
        times.push_back(us);
        lastMoves = moves;
    }

    double sum = 0.0;
    for (double x : times) sum += x;

    Stats s;
    s.moves = lastMoves;
    s.mean_us = sum / (double)times.size();
    s.min_us = *min_element(times.begin(), times.end());
    return s;
}

// ----------------------------
// Main experiment driver
// ----------------------------
int main(int argc, char** argv) {
    // Defaults (edit as you want)
    int nMin   = 1;
    int nMax   = 23;   // try 25..35 depending on your machine
    int trials = 20;   // increase to reduce noise (e.g., 30, 50)
    string outCsv = "hanoi_results.csv";

    // Optional CLI usage:
    // ./hanoi_empirical 1 30 20 results.csv
    if (argc >= 2) nMin = stoi(argv[1]);
    if (argc >= 3) nMax = stoi(argv[2]);
    if (argc >= 4) trials = stoi(argv[3]);
    if (argc >= 5) outCsv = argv[4];

    if (nMin < 1) nMin = 1;
    if (nMax < nMin) {
        cerr << "Error: nMax must be >= nMin\n";
        return 1;
    }
    if (trials < 1) trials = 1;

    cout << "Empirical Analysis: Towers of Hanoi (count-only)\n";
    cout << "n range: [" << nMin << ", " << nMax << "], trials per n: " << trials << "\n";
    cout << "Output CSV: " << outCsv << "\n\n";

    ofstream fout(outCsv);
    if (!fout) {
        cerr << "Error: cannot open output file: " << outCsv << "\n";
        return 1;
    }

    // CSV header (Excel-friendly)
    fout << "n,expected_moves,rec_moves,rec_mean_us,rec_min_us,it_moves,it_mean_us,it_min_us\n";

    // Print a small console table too
    cout << left
         << setw(3)  << "n" << " | "
         << setw(14) << "expected" << " | "
         << setw(10) << "recMean" << " | "
         << setw(10) << "itMean"  << " | "
         << "checks\n";
    cout << string(60, '-') << "\n";

    for (int n = nMin; n <= nMax; ++n) {
        uint64_t exp = expected_moves(n);

        // Benchmark both
        Stats rec = benchmark_recursive(n, trials);
        Stats it  = benchmark_iterative(n, trials);

        // Consistency checks
        bool okRec = (rec.moves == exp);
        bool okIt  = (it.moves  == exp);

        // Write CSV row
        fout << n << ","
             << exp << ","
             << rec.moves << ","
             << fixed << setprecision(3) << rec.mean_us << ","
             << fixed << setprecision(3) << rec.min_us  << ","
             << it.moves  << ","
             << fixed << setprecision(3) << it.mean_us  << ","
             << fixed << setprecision(3) << it.min_us   << "\n";

        // Console summary
        cout << setw(3)  << n << " | "
             << setw(14) << exp << " | "
             << setw(10) << fixed << setprecision(3) << rec.mean_us << " | "
             << setw(10) << fixed << setprecision(3) << it.mean_us  << " | "
             << (okRec ? "recOK " : "recFAIL ")
             << (okIt  ? "itOK"   : "itFAIL")
             << "\n";

        if (!okRec || !okIt) {
            cerr << "\n[ERROR] Move-count mismatch at n=" << n << "\n"
                 << "Expected: " << exp
                 << " | recMoves: " << rec.moves
                 << " | itMoves: " << it.moves << "\n";
            cerr << "Your implementations must produce exactly 2^n - 1 moves.\n";
            return 1;
        }
    }

    fout.close();

    cout << "\nDone. Open the CSV in Excel and plot:\n"
         << "- rec_mean_us and it_mean_us vs n\n"
         << "- rec_mean_us and it_mean_us vs expected_moves\n"
         << "Optionally compute ratios time(n)/time(n-1) in Excel.\n";

    cout << "\nTip: If times for small n are too close to 0, increase trials.\n";
    return 0;
}