#ifndef MINHEAP_HPP
#define MINHEAP_HPP

#include "heap.h"

class MinHeap
{
    private:
        heap_t heap;
    public:
        MinHeap(int32_t (*compare)(const void *key, const void *with),
                void (*datum_delete)(void *));
        ~MinHeap();
        void insert(void *);
        void * extractMin();
};

#endif