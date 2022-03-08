/***************************************************************
                             main.c
                               
Handles the boot process of the ROM.
***************************************************************/

#include <ultra64.h>
#include "usb.h"
#include "osconfig.h"


/*==============================
    boot
    Initializes the hardware and
    prints Hello World
==============================*/

void boot()
{
    // Initialize the hardware and software
    osInitialize();
    
    // Initialize the usb system, and print hello world.
    usb_initialize();
    usb_write(DATATYPE_TEXT, "Hello world!\n", 13+1);
}