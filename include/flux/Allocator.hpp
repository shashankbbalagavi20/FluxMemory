#pragma once
#include <cstddef>
#include <limits>
#include <new>

namespace flux {

/**
 * @brief A fixed-size block allocator optimized for cache locality.
 * Implements the C++ Named Requirement: Allocator.
 */
template <typename T>
class Allocator {
public:
    using value_type = T;
    using size_type = std::size_t;

    Allocator() noexcept {}
    
    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    /**
     * @brief Allocates memory for n objects of type T.
     * Currently wraps ::operator new for baseline, transitioning to Slab backend.
     */
    T* allocate(std::size_t n) {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_alloc();
            
        if (auto p = static_cast<T*>(::operator new(n * sizeof(T)))) {
            return p;
        }
        
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t) noexcept {
        ::operator delete(p);
    }
};

template <typename T, typename U>
bool operator==(const Allocator<T>&, const Allocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const Allocator<T>&, const Allocator<U>&) { return false; }

} // namespace flux