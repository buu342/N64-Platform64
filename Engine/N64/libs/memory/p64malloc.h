#ifndef PLATFORM64_MALLOC_H
#define PLATFORM64_MALLOC_H

    /*********************************
                 Typedefs             
    *********************************/

    typedef struct {
        void* baseaddr;
        u32 size;
    } P64Heap;
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void p64_initheap(P64Heap heap);
    void* p64_malloc(P64Heap heap, u32 size);
    void* p64_calloc(P64Heap heap, u32 num, u32 size);
    void* p64_realloc(P64Heap heap, void* data, u32 size);
    void p64_free(P64Heap heap, void* data);

#endif