/***************************************************************
                             main.c
                               
Handles the boot process of the ROM.
***************************************************************/

#include <ultra64.h>
#include "debug.h"


/*==============================
    boot
    Initializes the hardware and prints hello world
==============================*/
void boot()
{
    // Initialize the hardware and software
    osInitialize();

    // Print hello world
    debug_print("Hello, world!\n");
    debug_print("How are you feeling today?\n");
}