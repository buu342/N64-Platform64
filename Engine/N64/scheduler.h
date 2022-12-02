#ifndef PLATFORM64_SCHEDULER_H
#define PLATFORM64_SCHEDULER_H


    /*********************************
               Definitions
    *********************************/

    // Max number of messages for the scheduler message queue
    #define SIZE_MSGQUEUE_SCHEDULER  8
    
    // Scheduler messages
    #define MSG_SCHEDULER_READYFBUFFER 1
    #define MSG_SCHEDULER_VSYNC        2
    #define MSG_SCHEDULER_PRENMI       3


    /*********************************
                 Structs
    *********************************/

    typedef struct 
    {
        bool         tvblack;    // Whether the TV is blacked or not
        bool         notify;     // Should we be notified when a framebuffer is available?
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