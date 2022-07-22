/***************************************************************
                          scheduler.c
                               
Handles scheduler
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "scheduler.h"


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_scheduler(void *arg);


/*********************************
             Globals
*********************************/

static OSThread	s_threadstruct_scheduler;


/*==============================
    scheduler_initialize
    Launches the scheduler thread
==============================*/

void scheduler_initialize()
{
    // Start the scheduler thread
    osCreateThread(&s_threadstruct_scheduler, THREADID_SCHEDULER, threadfunc_scheduler, NULL, STACKREALSTART_SCHEDULER, THREADPRI_SCHEDULER);
    osStartThread(&s_threadstruct_scheduler);
}


/*==============================
    thread_scheduler
    The scheduler thread
    @param Argument passed to the thread
==============================*/

static void threadfunc_scheduler(void *arg)
{
    debug_printf("Created scheduler thread\n");
    
    // Spin this thread forever
    while (1)
    {
        debug_printf("Scheduler thread loop start\n");
        
        debug_printf("Stopping scheduler thread\n");
        osStopThread(&s_threadstruct_scheduler);
    }
}