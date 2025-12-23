# FluxMemory: High-Performance Slab Allocator

![Build Status](https://img.shields.io/badge/build-passing-brightgreen) ![License](https://img.shields.io/badge/license-MIT-blue) ![Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue)

**FluxMemory** is a custom memory allocator implementation in C++ designed for low-latency systems. It aims to replace `glibc malloc` for specific high-frequency object sizes to minimize cache misses and eliminate system call overhead during critical execution paths.

## 🎯 Project Goals

1.  **Deterministic Latency:** Remove the variability of general-purpose allocators.
2.  **Cache Locality:** Enforce contiguous memory layout for high-churn objects (L1/L2 optimization).
3.  **Fragmentation Control:** Use a Slab/Bitmap approach to eliminate external fragmentation.

## 🏗 Architecture

FluxMemory uses a hierarchical approach similar to `jemalloc` but simplified for specific use cases:

* **The Arena:** Large blocks of memory requested from the OS via `mmap`.
* **The Slab:** 4KB pages divided into fixed-size chunks (Size Classes).
* **The Free List:** Intrusive linked list embedded within free blocks to track availability without auxiliary memory.

```text
[ Process Virtual Address Space ]
+-------------------------------------------------------+
|  Arena (Large mmap chunk from OS)                     |
|                                                       |
|  +-------------------+   +-------------------+        |
|  | Slab (4KB Page)   |   | Slab (4KB Page)   |        |
|  | [32B][32B][32B]...|   | [64B][64B][64B]...|        |
|  +-------------------+   +-------------------+        |
|            |                       |                  |
|      (Size Class 1)          (Size Class 2)           |
+-------------------------------------------------------+
```

## 🚀 Performance Strategy

* **O(1) Allocation:** Pointer bump or Free List pop.
* **No Syscalls on Hot Path:** Memory is pre-faulted and pooled.
* **Thread Local Caching (Planned):** Per-thread heaps to avoid mutex contention.

## 🛠 Usage (Planned Interface)

FluxMemory is designed to be a drop-in replacement for STL containers:

```cpp
#include "flux/Allocator.hpp"
#include <vector>

struct Order {
    int id;
    double price;
};

// Use FluxAllocator with std::vector
std::vector<Order, flux::Allocator<Order>> orderBook;
```
## 🙌 Acknowledgements
Inspired from Fast Multi-threaded Memory Allocators (L. Evans), Jemalloc internals. Implemented from scratch in modern C++17 to deepen expertise in Systems Programming, OS Internals, and High-Performance Computing (HPC).