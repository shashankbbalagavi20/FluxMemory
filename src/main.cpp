#include "flux/Allocator.hpp"
#include <iostream>
#include <vector>
#include <thread> // <--- NEW

struct Data {
    int id;
    int val;
};

void threadTask(int id) {
    // Each thread creates its own vector, but they share the STATIC SlabAllocator
    std::vector<Data, flux::Allocator<Data>> vec;
    for (int i = 0; i < 1000; ++i) {
        vec.push_back({id, i});
    }
}

int main() {
    std::cout << "--- FluxMemory Thread Safety Test ---\n";
    
    std::vector<std::thread> threads;
    
    // Launch 10 threads running in parallel
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(threadTask, i);
    }

    // Wait for them to finish
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Success! 10 threads allocated 10,000 objects without crashing.\n";
    return 0;
}