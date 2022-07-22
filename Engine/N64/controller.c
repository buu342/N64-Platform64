/***************************************************************
                          controller.c
                               
Handles controller I/O and provides helper functions for reading
player inputs.
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "controller.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_controller(void *arg);


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_controller;


/*==============================
    controller_initialize
    Initializes the controller
    and launches a thread.
==============================*/

void controller_initialize()
{
    // Start the controller thread
    osCreateThread(&s_threadstruct_controller, THREADID_CONTROLLER, threadfunc_controller, NULL, STACKREALSTART_CONTROLLER, THREADPRI_CONTROLLER);
    osStartThread(&s_threadstruct_controller);
}


/*==============================
    thread_controller
    The controller thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_controller(void *arg)
{
    debug_printf("Created Controller thread\n");
    
    // Spin this thread forever
    while (1)
    {
        debug_printf("Controller thread loop start\n");
        
        debug_printf("Stopping controller thread\n");
        osStopThread(&s_threadstruct_controller);
    }
}