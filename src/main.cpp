#include "flux/Allocator.hpp"
#include <iostream>
#include <vector>
#include <list>
#include <iomanip>

struct Order {
    int id;
    double price;
    char padding[16]; // Make it slightly chonky (~32 bytes)
};

int main() {
    // We use std::list because it allocates nodes ONE BY ONE.
    // This forces it to use our Slab Allocator path (n==1).
    std::list<Order, flux::Allocator<Order>> orderList;

    std::cout << "--- FluxMemory HFT Slab Test ---\n";
    std::cout << "Allocating 5 Orders using Slab...\n";

    for (int i = 0; i < 5; ++i) {
        orderList.push_back({i, 100.0 + i});
        
        // Print the address of the new element
        // HFT GOAL: These addresses should be very close (e.g., 32 or 48 bytes apart)
        // If they are random/far apart, we failed (malloc was used).
        std::cout << "Order " << i << " Address: " << &orderList.back() << "\n";
    }
    
    std::cout << "\nSuccess. If addresses are close, Cache Locality is achieved.\n";

    return 0;
}