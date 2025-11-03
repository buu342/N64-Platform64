/***************************************************************
                          p64malloc.c
                               
A custom malloc implementation that is designed to allow
allocation for the many different heaps that have been defined
throughout the engine.
This malloc is implemented via a simple linked list, and will
attmept to use fragmented spaces. Allocations are O(n), while
freeing is O(1).
***************************************************************/

#include <ultra64.h>
#include "../../debug.h"
#include "p64malloc.h"


/*********************************
           Definitions
*********************************/

// Configuration for debugging
#define VALIDATIONS 1
#define PRINTERRORS 1

#define PSZ u32 // The size of the pointer in this system
#define MAGIC 0x12345678 // Magic value to verify the heap's state during debugging


/*********************************
             Structs
*********************************/

// Struct is 16 bytes big (on a 32-bit system)
struct MallocHeader_s;
typedef struct MallocHeader_s {
    u32   magicfree;
    u32   size;
    struct MallocHeader_s* next;
    struct MallocHeader_s* previous;
} MallocHeader;


/*==============================
    p64_initheap
    Initializes this heap.
    Calling this function in an already initialized heap will
    free all the data in it.
    @param The definition of the heap to initialize
==============================*/

void p64_initheap(P64Heap heap)
{
    MallocHeader* head = (MallocHeader*)heap.baseaddr;
    head->magicfree = MAGIC;
    head->size = 0;
    head->next = NULL;
    head->previous = NULL;
}


/*==============================
    doalloc
    Creates the node which will contain the allocated space
    @param  A pointer to the previous node in the heap
    @param  The size of the space we wish to allocate
    @param  The starting location to place the data into
    @return The address of the data's position
==============================*/

static PSZ doalloc(MallocHeader* prev, u32 size, PSZ datastart)
{
    MallocHeader* target = (MallocHeader*)(datastart - sizeof(MallocHeader));
    target->magicfree = MAGIC;
    target->size = (size + (sizeof(PSZ)-1)) & ~(sizeof(PSZ)-1); // Align the size to the pointer size
    target->next = prev->next;
    target->previous = prev;
    
    // Make the next and previous node point to our target
    if (target->next != NULL)
        target->next->previous = target;
    prev->next = target;
    return datastart;
}


/*==============================
    p64_malloc
    Allocate dynamic memory
    @param  The heap we want to allocate into
    @param  The size of the data to allocate
    @return The allocated area, or NULL
==============================*/

void* p64_malloc(P64Heap heap, u32 size)
{
    MallocHeader* head = (MallocHeader*)heap.baseaddr;
    MallocHeader* node;
    
    #if VALIDATIONS
        if (size == 0)
        {
            #if PRINTERRORS
                debug_printf("MALLOC ERROR: Invalid size allocation %d\n", size);
            #endif
            return NULL;
        }
        if (head->magicfree != MAGIC)
        {
            #if PRINTERRORS
                debug_printf("MALLOC ERROR: Heap not initialized\n");
            #endif
            return NULL;
        }
    #endif
       
    // Find the next available space to set up the header struct into
    node = head;
    while (TRUE)
    {
        PSZ nodedatastart = ((PSZ)node) + sizeof(MallocHeader);
        PSZ targetdatastart = nodedatastart + node->size + sizeof(MallocHeader);
            
        // If the next node isn't pointing to anything then we have reached the end of the linked list and we can just allocate at the tail
        if (node->next == NULL)
        {            
            // Check if we still have space in the heap for the data we're asking for
            if ((targetdatastart + size) > (((PSZ)heap.baseaddr) + heap.size))
            {
                #if PRINTERRORS
                    debug_printf("MALLOC ERROR: No space to allocate %d bytes\n", size);
                #endif
                return NULL;
            }
            
            // Create the new node at the tip of this data
            return (void*)doalloc(node, size, targetdatastart);
        }
        
        // If there is space between this node and the next that can fit our data, then let's allocate into this fragmented area
        if (((PSZ)node->next) >= (targetdatastart + size))
            return (void*)doalloc(node, size, targetdatastart);
        
        // No luck here, look at the next node
        node = node->next;
        #if VALIDATIONS
            if (node->magicfree != MAGIC)
            {
                #if PRINTERRORS
                    debug_printf("MALLOC ERROR: Heap corrupted???\n");
                #endif
                return NULL;
            }
        #endif
    }
}


/*==============================
    p64_free
    Frees an allocated chunk of memory
    @param  The heap we want to free the allocated data of
    @param  The allocated area we want to free, or NULL for
            no effect
==============================*/

void p64_free(P64Heap heap, void* data)
{
    MallocHeader* node;
    
    // Ignore freeing NULL pointers
    if (data == NULL)
        return;
    
    // Do some basic validations
    #if VALIDATIONS
        if (((MallocHeader*)heap.baseaddr)->magicfree != MAGIC)
        {
            #if PRINTERRORS
                debug_printf("FREE ERROR: Heap not initialized\n");
            #endif
            return;
        }
        if ((PSZ)data < (((PSZ)heap.baseaddr) + sizeof(MallocHeader)) || (PSZ)data >= (((PSZ)heap.baseaddr) + heap.size))
        {
            #if PRINTERRORS
                debug_printf("FREE ERROR: Attempting to free memory outside the heap\n");
            #endif
            return;
        }
    #endif
    
    // Validate the heap is in a good state
    node = (MallocHeader*)(((PSZ)data) - sizeof(MallocHeader));
    #if VALIDATIONS
        if (node->magicfree != MAGIC)
        {
            #if PRINTERRORS
                debug_printf("FREE ERROR: Heap corrupted???\n");
            #endif
            return;
        }
        node->magicfree = 0;
    #endif
    
    // Change the pointers in the previous and next node
    node->previous->next = node->next;
    if (node->next != NULL)
        node->next->previous = node->previous;
}