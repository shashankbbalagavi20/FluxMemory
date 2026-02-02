#pragma once
#include <vector>
#include <cstddef>
#include <cstdint>
#include <sys/mman.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>

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
            
            explicit SlabAllocator(size_t size) : objectSize(size){
                pageSize = sysconf(_SC_PAGE_SIZE);
                if(objectSize < sizeof(Block*)){
                    objectSize = sizeof(Block*);
                }
            }

            ~SlabAllocator(){
                for(void* slab : allocatedSlabs){
                    munmap(slab, pageSize);
                }
            }

            void* allocate(){
                if(freeListHead){
                    Block* block = freeListHead;
                    freeListHead = block->next;
                    return static_cast<void*>(block);
                }

                allocateNewSlab();

                Block* block = freeListHead;
                freeListHead = block->next;
                return static_cast<void*>(block);
            }

            void deallocate(void* ptr){
                if(!ptr) return;
                Block* block = static_cast<Block*>(ptr);
                block->next = freeListHead;
                freeListHead = block;
            }
        
        private:
            void allocateNewSlab(){
                void* slab = mmap(nullptr, pageSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
                
                if(slab == MAP_FAILED){
                    throw std::bad_alloc();
                }

                allocatedSlabs.push_back(slab);

                char* start = static_cast<char*>(slab);
                size_t numBlocks = pageSize / objectSize;

                for(size_t i = 0; i < numBlocks - 1; ++i){
                    Block* current = reinterpret_cast<Block*>(start + (i*objectSize));
                    Block* next = reinterpret_cast<Block*>(start + ((i + 1) * objectSize));
                    current->next = next;
                }

                Block* last = reinterpret_cast<Block*>(start + ((numBlocks - 1) * objectSize));
                last->next = nullptr;

                freeListHead = reinterpret_cast<Block*>(start);
            }
    };
}