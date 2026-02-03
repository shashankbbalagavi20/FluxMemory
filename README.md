# FluxMemory: High-Performance Slab Allocator

![License](https://img.shields.io/badge/license-MIT-blue) ![Standard](https://img.shields.io/badge/C%2B%2B-17%2F20-blue)

**FluxMemory** is a custom memory allocator implementation in C++ designed for low-latency systems. It aims to replace `glibc malloc` for specific high-frequency object sizes to minimize cache misses and eliminate system call overhead during critical execution paths.

> **Status:** Phase 4 Complete (Thread-Local & Benchmarked)

## 🎯 Project Goals

1.  **Deterministic Latency:** Remove the variability of general-purpose allocators.
2.  **Cache Locality:** Enforce contiguous memory layout for high-churn objects (L1/L2 optimization).
3.  **Fragmentation Control:** Use a Slab/Bitmap approach to eliminate external fragmentation.

## 🏗 Architecture

FluxMemory uses a hierarchical approach similar to `jemalloc` but simplified for specific HFT use cases:

* **The Arena:** Large blocks of memory requested from the OS via `mmap`.
* **The Slab:** 4KB/16KB pages divided into fixed-size chunks (Size Classes).
* **The Free List:** Intrusive linked list embedded within free blocks to track availability without auxiliary memory.
* **Thread Local Storage (TLS):** Each thread maintains its own private heap to completely eliminate lock contention.

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

## Performance Benchmarks

Benchmarked on Apple Silicon (M‑Series) with 500,000 allocations of ~32B std::list nodes.

| Allocator Strategy | Execution Time | Relative Speed | Cache Locality |
|---|---:|---|---|
| std::allocator (malloc) | 27.04 ms | 1.0x (Baseline) | Scattered |
| FluxMemory (Thread-Local) | 14.37 ms | 1.88x Faster | Contiguous (40B stride) |

## Why it's faster:
- Zero Lock Contention: Uses thread_local heaps to scale linearly with core count.

- LIFO Hot Cache: Recently freed blocks are immediately reused while still in L1 CPU cache.

- No System Calls: Pre-faulted slabs avoid expensive brk() or mmap() calls on the hot path.

## 🛠 Usage

FluxMemory is designed to be a drop-in replacement for STL containers:

```cpp
#include "flux/Allocator.hpp"
#include <vector>
#include <list>

struct Order {
    int id;
    double price;
};

// 1. Optimized Path (Slab Allocator)
// std::list allocates nodes one-by-one, triggering the O(1) Slab engine.
std::list<Order, flux::Allocator<Order>> orderBook;

// 2. Compatible Path (Standard Allocator)
// std::vector allocates arrays, so FluxMemory safely falls back to standard operators.
std::vector<Order, flux::Allocator<Order>> tradeHistory;
```

## ⚡️ Building & Running Benchmarks

```bash
mkdir build && cd build
cmake ..
make

# Run the correctness test (Thread Safety)
./flux_test

# Run the performance benchmark (Speed)
./flux_bench
```
## 🙌 Acknowledgements
Inspired by Fast Multi-threaded Memory Allocators (L. Evans) and Jemalloc internals. Implemented from scratch in modern C++17 to deepen expertise in Systems Programming, OS Internals, and High-Performance Computing (HPC).