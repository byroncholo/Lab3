#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <string>
#include <limits>

// LCG core loop: r <- (a*r + b) mod m, repeated n times.
static inline uint64_t lcg_run(uint64_t n, uint64_t m, uint64_t seed, uint64_t a, uint64_t b) {
    uint64_t r = seed % m;
    for (uint64_t i = 0; i < n; ++i) {
        __uint128_t tmp = static_cast<__uint128_t>(a) * r + b; // avoid overflow
        r = static_cast<uint64_t>(tmp % m);
    }
    return r;
}

// Measure runtime (ms) of one run of lcg_run(n,...)
static inline double time_one_run_ms(uint64_t n, uint64_t m, uint64_t seed, uint64_t a, uint64_t b) {
    // Prevent compiler from removing computation
    volatile uint64_t sink = 0;

    auto start = std::chrono::high_resolution_clock::now();
    uint64_t last = lcg_run(n, m, seed, a, b);
    auto end = std::chrono::high_resolution_clock::now();

    sink ^= last;

    std::chrono::duration<double, std::milli> elapsed = end - start;
    return elapsed.count();
}

int main() {
    // --- LCG parameters (change if your lab specifies others) ---
    const uint64_t m    = 2147483647ULL;     // 2^31 - 1
    const uint64_t a    = 1103515245ULL;
    const uint64_t b    = 12345ULL;
    const uint64_t seed = 42ULL;

    // --- Empirical analysis settings ---
    const int trials = 7;  // more trials => smoother average
    std::vector<uint64_t> ns = {
        1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000
    };

    const std::string outFile = "lcg_summary.csv";
    std::ofstream out(outFile);
    if (!out) {
        std::cerr << "Error: cannot open output file: " << outFile << "\n";
        return 1;
    }

    // This CSV is directly plottable in Excel (one row per n).
    out << "n,avg_ms,min_ms,max_ms,ms_per_op\n";

    for (uint64_t n : ns) {
        double sum = 0.0;
        double mn = std::numeric_limits<double>::infinity();
        double mx = -std::numeric_limits<double>::infinity();

        // Optional warm-up (reduces first-run noise)
        (void)lcg_run(2000, m, seed + 999, a, b);

        for (int t = 0; t < trials; ++t) {
            // vary seed slightly per trial (not required, but fine)
            double ms = time_one_run_ms(n, m, seed + static_cast<uint64_t>(t), a, b);

            sum += ms;
            if (ms < mn) mn = ms;
            if (ms > mx) mx = ms;
        }

        double avg = sum / trials;
        double ms_per_op = avg / static_cast<double>(n); // ~constant if Θ(n)

        out << n << "," << avg << "," << mn << "," << mx << "," << ms_per_op << "\n";
    }

    out.close();
    std::cout << "Wrote plottable CSV: " << outFile << "\n";
    std::cout << "Open it in Excel and plot avg_ms (Y) vs n (X).\n";
    return 0;
}