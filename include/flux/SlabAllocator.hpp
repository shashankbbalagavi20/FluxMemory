#pragma once
#include <vector>
#include <cstddef>

namespace flux {
    class SlabAllocator {
        public:
            struct Block {
                Block* next;
            };

            size_t objectSize;
            size_t pageSize;

            void* currentSlab = nullptr;
            Block* freeListHead = nullptr;
            std::vector<void*> allocatedSlabs;
            
            explicit SlabAllocator(size_t size);

            ~SlabAllocator();
            void* allocate();

            void deallocate(void* ptr);
        
        private:
            void allocateNewSlab();
    };
}