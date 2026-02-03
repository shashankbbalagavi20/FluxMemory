#include "../include/flux/Allocator.hpp"
#include <iostream>
#include <list>
#include <vector>
#include <chrono>
#include <random>

// Define a rigorous timer
class Timer {
    std::chrono::high_resolution_clock::time_point start;
public:
    Timer() { reset(); }
    void reset() { start = std::chrono::high_resolution_clock::now(); }
    double elapsed_ms() {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000.0;
    }
};

struct Trade {
    int id;
    double price;
    char symbol[8];
    // Size is approx 24-32 bytes
};

int main() {
    const int NUM_OPERATIONS = 500000; // 500k allocations

    std::cout << "--- FluxMemory vs StdAllocator Benchmark ---" << std::endl;
    std::cout << "Operations: " << NUM_OPERATIONS << " push_back / pop_front" << std::endl;

    // -------------------------------------------------
    // ROUND 1: Standard Allocator (The Baseline)
    // -------------------------------------------------
    {
        std::list<Trade> stdList;
        Timer t;
        
        for (int i = 0; i < NUM_OPERATIONS; ++i) {
            stdList.push_back({i, 100.0, "AAPL"});
        }
        // Force deallocation by clearing
        stdList.clear();
        
        std::cout << "[std::allocator] Time: " << t.elapsed_ms() << " ms" << std::endl;
    }

    // -------------------------------------------------
    // ROUND 2: Flux Allocator (Your Code)
    // -------------------------------------------------
    {
        // Notice: We pass our allocator here
        std::list<Trade, flux::Allocator<Trade>> fluxList;
        Timer t;
        
        for (int i = 0; i < NUM_OPERATIONS; ++i) {
            fluxList.push_back({i, 100.0, "AAPL"});
        }
        fluxList.clear();
        
        std::cout << "[flux::Allocator] Time: " << t.elapsed_ms() << " ms" << std::endl;
    }
    
    // -------------------------------------------------
    // ROUND 3: Verify Locality (Qualitative)
    // -------------------------------------------------
    std::cout << "\nChecking Locality..." << std::endl;
    std::list<Trade, flux::Allocator<Trade>> localityList;
    localityList.push_back({1, 1.0, "A"});
    localityList.push_back({2, 2.0, "B"});
    
    // Pointer math to check distance
    long dist = (long)&localityList.back() - (long)&localityList.front();
    std::cout << "Distance between nodes: " << std::abs(dist) << " bytes (Should be small/constant)" << std::endl;

    return 0;
}