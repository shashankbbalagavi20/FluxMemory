#pragma once
#include "SlabAllocator.hpp"
#include <limits>
#include <new>

namespace flux {

template <typename T>
class Allocator {
public:
    using value_type = T;

    static SlabAllocator pool;

    Allocator() noexcept {}
    
    template <typename U>
    Allocator(const Allocator<U>&) noexcept {}

    template <typename U>
    struct rebind{
        using other = Allocator<U>;
    };

    T* allocate(std::size_t n) {
        if (n == 1){
            return static_cast<T*>(pool.allocate());
        }

        if (auto p = static_cast<T*>(::operator new(n * sizeof(T)))) {
            return p;
        }
        
        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept {
        if(n == 1){
            pool.deallocate(p);
        } else{
            ::operator delete(p);
        }
    }
};

template <typename T>
SlabAllocator Allocator<T>::pool(sizeof(T));

template <typename T, typename U>
bool operator==(const Allocator<T>&, const Allocator<U>&) { return true; }

template <typename T, typename U>
bool operator!=(const Allocator<T>&, const Allocator<U>&) { return false; }

} // namespace flux