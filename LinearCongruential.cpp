
/*
This program generates random numbers using the method of lineat congruency
Input:
Output:

Name: Byron Cholo Zambonino
Date: 3/1/2026
*/

// Libraries
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <string>

// Linear Congruential Generator loop:
// r0 = seed
// for i = 1..n: r = (a*r + b) mod m
// We only keep the last r (enough for timing).
static inline uint64_t lcg_run(uint64_t n, uint64_t m, uint64_t seed, uint64_t a, uint64_t b) {
    uint64_t r = seed % m;
    for (uint64_t i = 0; i < n; ++i) {
        // Use 128-bit to avoid overflow in a*r when a and r are large.
        __uint128_t prod = static_cast<__uint128_t>(a) * static_cast<__uint128_t>(r);
        __uint128_t sum  = prod + static_cast<__uint128_t>(b);
        r = static_cast<uint64_t>(sum % m);
    }
    return r;
}

int main() {
    // --- Fixed LCG parameters (you can change these) ---
    const uint64_t m    = 2147483647ULL;     // 2^31 - 1 (prime)
    const uint64_t a    = 1103515245ULL;
    const uint64_t b    = 12345ULL;
    const uint64_t seed = 42ULL;

    // --- Empirical analysis settings ---
    const int trials = 5;
    std::vector<uint64_t> ns = {
        1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000
    };

    const std::string outFile = "lcg_empirical.csv";
    std::ofstream out(outFile);
    if (!out) {
        std::cerr << "Error: cannot open output file: " << outFile << "\n";
        return 1;
    }

    // CSV header
    out << "n,trial,time_ms,ops,last_r\n";

    // A volatile sink prevents the compiler from optimizing away the loop.
    volatile uint64_t sink = 0;

    for (uint64_t n : ns) {
        for (int t = 1; t <= trials; ++t) {
            // Optional warm-up (helps stabilize timings)
            sink ^= lcg_run(1000, m, seed + t, a, b);

            auto start = std::chrono::high_resolution_clock::now();
            uint64_t last_r = lcg_run(n, m, seed + t, a, b);
            auto end = std::chrono::high_resolution_clock::now();

            sink ^= last_r;

            std::chrono::duration<double, std::milli> elapsed = end - start;
            double time_ms = elapsed.count();

            // ops = n (one core update per iteration)
            out << n << "," << t << "," << time_ms << "," << n << "," << last_r << "\n";
        }
    }

    out.close();
    std::cout << "Wrote CSV: " << outFile << "\n";
    std::cout << "Ignore this value (prevents optimization): " << sink << "\n";

    return 0;

}
