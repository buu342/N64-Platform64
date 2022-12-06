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
           Definitions
*********************************/

#define SIZE_MSGQUEUE_CONTROLLER  8

// Controller messages
#define MSG_CONTROLLER_QUERY  1
#define MSG_CONTROLLER_READ   2


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_controller(void *arg);


/*********************************
             Globals
*********************************/

// Controller data
static OSContPad	s_contdata[MAXCONTROLLERS];
static OSContStatus	s_contstat[MAXCONTROLLERS];
static OSPfs		s_contrumble[MAXCONTROLLERS];

// Thread structure
static OSThread	s_threadstruct_controller;

// Message queues for thread communication
static OSMesgQueue s_msgqueue_cont;
static OSMesg      s_msgbuffer_cont[SIZE_MSGQUEUE_CONTROLLER];
static OSMesgQueue s_msgqueue_si;
static OSMesg      s_msgbuffer_si;

// Player data
static u8  s_playerindex[MAXCONTROLLERS];


/*==============================
    controller_initialize
    Initializes the controller
    and launches a thread.
==============================*/

void controller_initialize()
{
    // Initialize the serial interface and its message queue
    osCreateMesgQueue(&s_msgqueue_si, &s_msgbuffer_si, 1);
    osSetEventMesg(OS_EVENT_SI, &s_msgqueue_si, (OSMesg)1);
    
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
    int i;
    u8 l_pattern;
    u8 l_foundcount = 0;
    debug_printf("Created Controller thread\n");
    
    // Initialize the thread's message queue
    osCreateMesgQueue(&s_msgqueue_cont, s_msgbuffer_cont, SIZE_MSGQUEUE_CONTROLLER);
    
    // Initialize the controllers
    osContInit(&s_msgqueue_si, &l_pattern, s_contstat);
    
    // Find out what player corresponds to what controller
    memset(s_playerindex, 0, sizeof(u8)*MAXCONTROLLERS);
    debug_printf("Controller Thread: Querying controllers.\n");
    for (i=0; i<MAXCONTROLLERS; i++)
    {
        if ((l_pattern & (0x01 << i)) && s_contstat[i].errno == 0 && (s_contstat[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
        {
            s_playerindex[l_foundcount] = i;
            l_foundcount++;
            debug_printf("Controller Thread: Player %d found in port %d.\n", l_foundcount, i+1);
        }
    }
    debug_printf("Controller Thread: Finished querying controllers.\n");
    
    // Spin this thread forever
    while (1)
    {
        OSMesg l_msg;
        
        // Wait for a controller message to arrive
        #if VERBOSE 
            debug_printf("Controller Thread: Loop start, waiting for message\n");
        #endif
        osRecvMesg(&s_msgqueue_cont, (OSMesg *)&l_msg, OS_MESG_BLOCK);
        
        // At this point, a message has been received from another thread
        #if VERBOSE 
            debug_printf("Controller Thread: Message '%d' received\n", (s32)l_msg);
        #endif
        
        // Pick what to do based on the message value
        switch ((s32)l_msg)
        {
            case MSG_CONTROLLER_QUERY:
                #if VERBOSE 
                    debug_printf("Controller Thread: Starting query\n", (s32)l_msg);
                #endif
                osContStartQuery(&s_msgqueue_si);
                osRecvMesg(&s_msgqueue_si, NULL, OS_MESG_BLOCK);
                osContGetQuery(s_contstat);
                #if VERBOSE 
                    debug_printf("Controller Thread: Query finished\n", (s32)l_msg);
                #endif
            case MSG_CONTROLLER_READ:
                #if VERBOSE 
                    debug_printf("Controller Thread: Starting read\n", (s32)l_msg);
                #endif
                osContStartReadData(&s_msgqueue_si);
                osRecvMesg(&s_msgqueue_si, NULL, OS_MESG_BLOCK);
                osContGetReadData(s_contdata);
                #if VERBOSE 
                    debug_printf("Controller Thread: Read finished\n", (s32)l_msg);
                #endif
            default:
                #if VERBOSE 
                    debug_printf("Controller Thread: Bad message '0x%2x' received\n", (s32)l_msg);
                #endif
                break;
        }
    }
}


/*==============================
    controller_reinitialize_all
    Reinitializes all the controllers
==============================*/

void controller_reinitialize_all()
{
    int i;
    u8 l_foundcount = 0;
    osContReset(&s_msgqueue_si, s_contstat);
    
    // Find out what player corresponds to what controller
    memset(s_playerindex, 0, sizeof(u8)*MAXCONTROLLERS);
    debug_printf("Controller Thread: Querying controllers.\n");
    for (i=0; i<MAXCONTROLLERS; i++)
    {
        if (s_contstat[i].errno == 0 && (s_contstat[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
        {
            s_playerindex[l_foundcount] = i;
            l_foundcount++;
            debug_printf("Controller Thread: Player %d found in port %d.\n", l_foundcount, i+1);
        }
    }
    debug_printf("Controller Thread: Finished querying controllers.\n");
}


/*==============================
    controller_query_all
    Queries all controllers
==============================*/

void controller_query_all()
{
    osSendMesg(&s_msgqueue_cont, (OSMesg)MSG_CONTROLLER_QUERY, OS_MESG_BLOCK);
}


/*==============================
    controller_read_all
    Reads all controllers
==============================*/

void controller_read_all()
{
    osSendMesg(&s_msgqueue_cont, (OSMesg)MSG_CONTROLLER_READ, OS_MESG_BLOCK);
}


/*==============================
    controller_playercount
    Gives you the player count 
    (ie the number of connected controllers)
    @returns The number of players
==============================*/

s32 controller_playercount()
{
    s32 i;
    // Since the last non zero value in s_playerindex is equivalent to our player count
    // (for example, if there's two players, then [0] and [1] will be non zero)
    // we can just iterate backwards and return the first non-zero index (plus 1).
    for (i=MAXCONTROLLERS-1; i>=0; i--)
        if (s_playerindex[i] != 0)
            return i+1;
    return 0;
}


/*==============================
    controller_rumble_init
    Initializes the Rumble Pak for a specific player
    @param The player to initialize the Rumble Pak of.
           Ranges from 1 to 4.
    @returns Any errors that ocurred, or zero
==============================*/

s32 controller_rumble_init(u8 player)
{
    if (s_playerindex[player-1] != 0)
        return osMotorInit(&s_msgqueue_si, s_contrumble, player-1);
    return PFS_ERR_NOPACK;
}


/*==============================
    controller_rumble_start
    Starts the motor on a player's Rumble Pak
    @param The player to start the Rumble Pak of.
           Ranges from 1 to 4.
==============================*/

void controller_rumble_start(u8 player)
{
    osMotorStart(&s_contrumble[player-1]);
}


/*==============================
    controller_rumble_start
    Stops the motor on a player's Rumble Pak
    @param The player to stop the Rumble Pak of.
           Ranges from 1 to 4.
==============================*/

void controller_rumble_stop(u8 player)
{
    osMotorStop(&s_contrumble[player-1]);
}