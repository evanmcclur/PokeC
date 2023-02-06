#include "minheap.hpp"

MinHeap::MinHeap(int32_t (*compare)(const void *key, const void *with), void (*datum_delete)(void *))
{
    heap_init(&heap, compare, datum_delete);
}

MinHeap::~MinHeap()
{
    heap_delete(&heap);
}

void MinHeap::insert(void *v)
{
    heap_insert(&heap, v);
}

void * MinHeap::extractMin()
{
    return heap_remove_min(&heap);
}