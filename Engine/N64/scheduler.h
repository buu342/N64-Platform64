#ifndef PLATFORM64_SCHEDULER_H
#define PLATFORM64_SCHEDULER_H


    /*********************************
               Definitions
    *********************************/

    // Max number of messages for the scheduler message queue
    #define SIZE_MSGQUEUE_SCHEDULER  8
    
    // Scheduler messages
    #define MSG_SCHEDULER_VSYNC        1
    #define MSG_SCHEDULER_PRENMI       2


    /*********************************
                 Structs
    *********************************/

    typedef struct 
    {
        bool         tvblack;    // Whether the TV is blacked or not
        OSTask*      task_gfx;   // Current executing graphics task
        OSTask*      task_audio; // Current executing audio task
        OSMesgQueue  queue;
        OSMesg       messages[SIZE_MSGQUEUE_SCHEDULER];
        OSMesgQueue* gfx_notify;
    } Scheduler;


    /*********************************
                Functions
    *********************************/

    Scheduler* scheduler_initialize();

#endif