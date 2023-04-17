/***************************************************************
                          controller.c
                               
Handles controller I/O and provides helper functions for reading
player inputs.
***************************************************************/

#include <ultra64.h>
#include <math.h>
#include "debug.h"
#include "types.h"
#include "helper.h"
#include "osconfig.h"
#include "controller.h"


/*********************************
           Definitions
*********************************/

#define SIZE_MSGQUEUE_CONTROLLER  8

// Controller messages
#define MSG_CONTROLLER_QUERY      1
#define MSG_CONTROLLER_READ       2
#define MSG_CONTROLLER_RUMBLE_P1  3
#define MSG_CONTROLLER_RUMBLE_P2  4
#define MSG_CONTROLLER_RUMBLE_P3  5
#define MSG_CONTROLLER_RUMBLE_P4  6

// Controller distances
#define CONTROLLER_DEFAULT_MIN  {{0, 5},  {3, 3},   {4, 0},  {3, -3},   {0, -5},  {-3, -3},   {-4, 0},  {-3, 3}}
#define CONTROLLER_DEFAULT_MAX  {{0, 83}, {64, 64}, {74, 0}, {64, -64}, {0, -83}, {-64, -64}, {-74, 0}, {-64, 64}} // My personal controller profile
//#define CONTROLLER_DEFAULT_MAX  {{0, 63}, {46, 46}, {61, 0}, {46, -46}, {0, -63}, {-46, -46}, {-61, 0}, {-46, 46}} // Nintendo recommended values

// Rumble flags
#define RUMBLE_ERR 0
#define RUMBLE_ON  1
#define RUMBLE_OFF 2


/*********************************
             Structs
*********************************/

typedef struct 
{
    s8       contindex;
    u8       rumblestat;
    u16      actions[MAX_ACTIONS];
    Vector2D stick;
    Octagon  stickprof_min;
    Octagon  stickprof_max;
    OSPfs    rumble;
    OSTimer  rumbletimer;
    f32      trauma;
} PlayerCont;


/*********************************
        Function Prototypes
*********************************/

static void threadfunc_controller(void *arg);
static void controller_calcstick(plynum player);


/*********************************
             Globals
*********************************/

// Raw controller data
static OSContPad	s_contdata[MAXCONTROLLERS];
static OSContPad	s_contdata_old[MAXCONTROLLERS];
static OSContStatus	s_contstat[MAXCONTROLLERS];

// Thread structure
static OSThread	s_threadstruct_controller;

// Message queues for thread communication
static OSMesgQueue s_msgqueue_cont;
static OSMesg      s_msgbuffer_cont[SIZE_MSGQUEUE_CONTROLLER];
static OSMesgQueue s_msgqueue_si;
static OSMesg      s_msgbuffer_si;

// Player data
static u8         s_playercount = 0;
static PlayerCont s_playercont[MAXCONTROLLERS];


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
    debug_printf("Created Controller thread\n");
    
    // Initialize the thread's message queue
    osCreateMesgQueue(&s_msgqueue_cont, s_msgbuffer_cont, SIZE_MSGQUEUE_CONTROLLER);
    
    // Initialize the controllers
    osContInit(&s_msgqueue_si, &l_pattern, s_contstat);
    memset(s_playercont, 0, sizeof(PlayerCont)*MAXCONTROLLERS);
    s_playercount = 0;
    
    // Find out what player corresponds to what controller
    debug_printf("Controller Thread: Querying controllers.\n");
    for (i=0; i<MAXCONTROLLERS; i++)
    {
        const Octagon l_contstick_min = CONTROLLER_DEFAULT_MIN;
        const Octagon l_contstick_max = CONTROLLER_DEFAULT_MAX;
        
        // Check if we have a controller on port 'i'
        if ((l_pattern & (0x01 << i)) && s_contstat[i].errno == 0 && (s_contstat[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
        {
            s_playercont[s_playercount].contindex = i;
            s_playercount++;
            debug_printf("Controller Thread: Player %d found in port %d.\n", s_playercount, i+1);
        }
        
        // Set the default stick profile
        s_playercont[i].stickprof_min = l_contstick_min;
        s_playercont[i].stickprof_max = l_contstick_max;
    }
    debug_printf("Controller Thread: Finished querying controllers.\n");
    
    // Spin this thread forever
    while (1)
    {
        plynum l_ply;
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
                break;
            case MSG_CONTROLLER_READ:
                #if VERBOSE 
                    debug_printf("Controller Thread: Starting read\n", (s32)l_msg);
                #endif
                osContStartReadData(&s_msgqueue_si);
                osRecvMesg(&s_msgqueue_si, NULL, OS_MESG_BLOCK);
                memcpy(s_contdata_old, s_contdata, sizeof(OSContPad)*MAXCONTROLLERS);
                osContGetReadData(s_contdata);
                for (l_ply=PLAYER_1; l_ply<s_playercount; l_ply++)
                {
                    u32 l_contindex = s_playercont[l_ply].contindex;
                    if (s_contdata[l_contindex].stick_x != s_contdata_old[l_contindex].stick_x || s_contdata[l_contindex].stick_y != s_contdata_old[l_contindex].stick_y)
                    {
                        debug_printf("*****\n");
                        debug_printf("Input -> {%d, %d}\n", s_contdata[l_contindex].stick_x, s_contdata[l_contindex].stick_y);                    
                        controller_calcstick(l_ply);
                        debug_printf("Output -> {%.4f, %.4f}\n", s_playercont[l_ply].stick.x, s_playercont[l_ply].stick.y);
                    }
                }
                #if VERBOSE 
                    debug_printf("Controller Thread: Read finished\n", (s32)l_msg);
                #endif
                break;
            case MSG_CONTROLLER_RUMBLE_P1: // Intentional fallthrough
            case MSG_CONTROLLER_RUMBLE_P2:
            case MSG_CONTROLLER_RUMBLE_P3:
            case MSG_CONTROLLER_RUMBLE_P4:
                l_ply = (plynum)(l_msg-MSG_CONTROLLER_RUMBLE_P1);
                #if VERBOSE 
                    debug_printf("Controller Thread: Trauma at %f %ld\n", s_playercont[l_ply].trauma);
                #endif
                
                // Handle rumble cycle
                // Since we only have on or off functions, we need to use timers to simulate intensity
                if (s_playercont[l_ply].trauma > 0.0f || s_playercont[l_ply].rumblestat == RUMBLE_ON)
                {
                    osStopTimer(&s_playercont[l_ply].rumbletimer);
                    if (s_playercont[l_ply].rumblestat == RUMBLE_ON && s_playercont[l_ply].trauma > 0.0f)
                    {
                        s_playercont[l_ply].trauma -= 0.1f;
                        if (s_playercont[l_ply].trauma > 0.01f)
                            osSetTimer(&s_playercont[l_ply].rumbletimer, OS_USEC_TO_CYCLES((u32)(100000.0f - s_playercont[l_ply].trauma*100000.0f)), 0, &s_msgqueue_cont, (OSMesg)(MSG_CONTROLLER_RUMBLE_P1+l_ply));
                        else
                            s_playercont[l_ply].trauma = 0.0f;
                        s_playercont[l_ply].rumblestat = RUMBLE_OFF;
                        osMotorStop(&s_playercont[l_ply].rumble);
                    }
                    else
                    {
                        osSetTimer(&s_playercont[l_ply].rumbletimer, OS_USEC_TO_CYCLES((u32)(s_playercont[l_ply].trauma*100000.0f)), 0, &s_msgqueue_cont, (OSMesg)(MSG_CONTROLLER_RUMBLE_P1+l_ply));
                        s_playercont[l_ply].rumblestat = RUMBLE_ON;
                        osMotorStart(&s_playercont[l_ply].rumble);
                    }
                }
                
                // Handle rumble motor shutdown
                // The rumble pak must be shut down 3 times or more in order to guarantee it stopped
                if (s_playercont[l_ply].rumblestat != RUMBLE_ERR && s_playercont[l_ply].trauma == 0)
                {
                    if (s_playercont[l_ply].rumblestat >= RUMBLE_OFF && s_playercont[l_ply].rumblestat < RUMBLE_OFF+3)
                    {
                        s_playercont[l_ply].rumblestat++;
                        osMotorStop(&s_playercont[l_ply].rumble);
                        osStopTimer(&s_playercont[l_ply].rumbletimer);
                        osSetTimer(&s_playercont[l_ply].rumbletimer, OS_USEC_TO_CYCLES(100000.0f), 0, &s_msgqueue_cont, (OSMesg)(MSG_CONTROLLER_RUMBLE_P1+l_ply));
                    }
                }
                break;
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
    
    // Reset the controllers
    osContReset(&s_msgqueue_si, s_contstat);
    memset(s_playercont, 0, sizeof(PlayerCont)*MAXCONTROLLERS);
    s_playercount = 0;
    
    // Find out what player corresponds to what controller
    debug_printf("Controller Thread: Querying controllers.\n");
    for (i=0; i<MAXCONTROLLERS; i++)
    {
        if (s_contstat[i].errno == 0 && (s_contstat[i].type & CONT_TYPE_MASK) == CONT_TYPE_NORMAL)
        {
            s_playercont[s_playercount].contindex = i;
            s_playercount++;
            debug_printf("Controller Thread: Player %d found in port %d.\n", s_playercount, i+1);
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

inline s32 controller_playercount()
{
    return s_playercount;
}


/*==============================
    controller_register_action
    Registers an action to a specific set of buttons
    on a player's controller.
    @param The player to register the action.
    @param The action to register
    @param The buttons to register. This should be a combination of
           Libultra button macros, like A_BUTTON.
==============================*/

void controller_register_action(plynum player, u8 action, u16 buttons)
{
    s_playercont[player].actions[action] |= buttons;
}


/*==============================
    controller_unregister_action
    Unregisters an action from a specific set of buttons
    on a player's controller.
    @param The player to unregister the action.
    @param The action to unregister
    @param The buttons to unregister. This should be a combination of
           Libultra button macros, like A_BUTTON.
==============================*/

void controller_unregister_action(plynum player, u8 action, u16 buttons)
{
    s_playercont[player].actions[action] &= ~buttons;
}


/*==============================
    controller_action_pressed
    Checks if a specific action button was pressed *this frame*
    on a player's controller. 
    @param The player to check.
    @param The action to check
==============================*/

bool controller_action_pressed(plynum player, u8 action)
{
    u8 l_index = s_playercont[player].contindex;
    u16 l_action = s_playercont[player].actions[action];
    return (s_contdata[l_index].button & l_action) != 0 && (s_contdata_old[l_index].button & l_action) == 0;
}


/*==============================
    controller_action_down
    Checks if a specific action button is down on a player's
    controller. 
    @param The player to check.
    @param The action to check
==============================*/

bool controller_action_down(plynum player, u8 action)
{
    return (s_contdata[s_playercont[player].contindex].button & s_playercont[player].actions[action]) != 0;
}


/*==============================
    controller_action_released
    Checks if a specific action button was released *this frame*
    on a player's controller. 
    @param The player to check.
    @param The action to check
==============================*/

bool controller_action_released(plynum player, u8 action)
{
    u8 l_index = s_playercont[player].contindex;
    u16 l_action = s_playercont[player].actions[action];
    return (s_contdata[l_index].button & l_action) == 0 && (s_contdata_old[l_index].button & l_action) != 0;
}


/*==============================
    controller_set_stickmin
    Sets a player's stick deadzone profile
    @param The player whos profile we want to modify.
    @param The octagon that represents the deadzone
==============================*/

void controller_set_stickmin(plynum player, Octagon oct)
{
    s_playercont[player].stickprof_min = oct;
}


/*==============================
    controller_set_stickmax
    Sets a player's stick maximum range profile
    @param The player whos profile we want to modify.
    @param The octagon that represents the maximum range.
==============================*/

void controller_set_stickmax(plynum player, Octagon oct)
{
    s_playercont[player].stickprof_max = oct;
}


/*==============================
    controller_set_stickmax
    Normalizes a player's stick coordinates to fit between [-1, 1],
    using their preconfigured stick deadzone and maximum range
    profiles.
    @param The player who's stick we should normalize
==============================*/

static void controller_calcstick(plynum player)
{
    s32 l_x, l_y, l_xp, l_yp;
    u32 l_xa, l_ya;
    s32 l_minx1, l_minx2, l_miny1, l_miny2;
    s32 l_maxx1, l_maxx2, l_maxy1, l_maxy2;
    s32 l_quadrant = 0, l_octant = 0, l_octantnext = 0;
    f32 l_detmin, l_detmax, l_intmin_x, l_intmin_y, l_intmax_x, l_intmax_y;
    f32 l_ang;
    
    // Populate our helper variables
    l_x = s_contdata[s_playercont[player].contindex].stick_x;
    l_y = s_contdata[s_playercont[player].contindex].stick_y;
    if (l_x == 0 && l_y == 0)
    {
        s_playercont[player].stick.x = 0;
        s_playercont[player].stick.y = 0;
        return;
    }
    l_xp = (l_x >> 31) & 0x01;
    l_yp = (l_y >> 31) & 0x01;
    l_xa = (l_x >= 0) ? l_x : -l_x;
    l_ya = (l_y >= 0) ? l_y : -l_y;
    
    // Find the octant on the cartesian plane
    l_quadrant = (!l_xp) ? (l_yp) : (3 - l_yp);
    if (l_quadrant & 0x01 == 1)
        l_octant = (l_xa <= l_ya) ? (l_quadrant*2 + 1) : (l_quadrant*2);
    else
        l_octant = (l_xa <= l_ya) ? (l_quadrant*2) : (l_quadrant*2 + 1);
        
    // Now we know which array values we need from our player zones
    l_minx1 = ((OctArray)s_playercont[player].stickprof_min).array[l_octant*2];
    l_miny1 = ((OctArray)s_playercont[player].stickprof_min).array[l_octant*2+1];
    l_maxx1 = ((OctArray)s_playercont[player].stickprof_max).array[l_octant*2];
    l_maxy1 = ((OctArray)s_playercont[player].stickprof_max).array[l_octant*2+1];
    l_octantnext = l_octant+1;
    if (l_octantnext > 7)
        l_octantnext = 0;
    l_minx2 = ((OctArray)s_playercont[player].stickprof_min).array[l_octantnext*2];
    l_miny2 = ((OctArray)s_playercont[player].stickprof_min).array[l_octantnext*2+1];
    l_maxx2 = ((OctArray)s_playercont[player].stickprof_max).array[l_octantnext*2];
    l_maxy2 = ((OctArray)s_playercont[player].stickprof_max).array[l_octantnext*2+1];
    
    // Ok, so now comes the actual math
    // We project a line from (0,0) to our point, and we find the interceptions with the min and max of our octant
    l_detmin = (l_minx1*l_miny2 - l_miny1*l_minx2)/((float)((l_minx1 - l_minx2)*(0 - l_y) - (l_miny1 - l_miny2)*(0 - l_x)));
    l_detmax = (l_maxx1*l_maxy2 - l_maxy1*l_maxx2)/((float)((l_maxx1 - l_maxx2)*(0 - l_y) - (l_maxy1 - l_maxy2)*(0 - l_x)));
    l_intmin_x = (0 - l_x)*l_detmin;
    l_intmin_y = (0 - l_y)*l_detmin;
    l_intmax_x = (0 - l_x)*l_detmax;
    l_intmax_y = (0 - l_y)*l_detmax;
    
    // We need to find the angle of the line, which we will apply to get the final X Y
    l_ang = atan2(l_y, l_x);
    
    // We know that the distance between the min and max point needs to have a length of 1 when remapped to the unit circle
    // So divide our point by the maximum value (with the minimum subtracted as well)
    // That gives us how far the X and Y values are from the circumference of the circle (as a percentage)
    // Then multiply by the angle to get the final XY value mapped to [-1, 1]
    if (l_intmax_x - l_intmin_x != 0)
        s_playercont[player].stick.x = clampf(((float)l_x - l_intmin_x)/(l_intmax_x - l_intmin_x), 0, 1)*cosf(l_ang);
    else
        s_playercont[player].stick.x = 0;
    if (l_intmax_y - l_intmin_y != 0)
        s_playercont[player].stick.y = clampf(((float)l_y - l_intmin_y)/(l_intmax_y - l_intmin_y), 0, 1)*sinf(l_ang);
    else
        s_playercont[player].stick.y = 0;
}


/*==============================
    controller_get_x
    Gets the X value of a player's stick, normalized
    to -1 and 1 based on their stick profile.
    @param The player to get the stick X of.
    @returns The normalized stick X value.
==============================*/

f32 controller_get_x(plynum player)
{
    return s_playercont[player].stick.x;
}


/*==============================
    controller_get_y
    Gets the Y value of a player's stick, normalized
    to -1 and 1 based on their stick profile.
    @param The player to get the stick Y of.
    @returns The normalized stick Y value.
==============================*/

f32 controller_get_y(plynum player)
{
    return s_playercont[player].stick.y;
}


/*==============================
    controller_rumble_init
    Initializes the Rumble Pak for a specific player
    @param The player to initialize the Rumble Pak of.
    @returns Any errors that ocurred, or zero
==============================*/

s32 controller_rumble_init(plynum player)
{
    u32 ret = osMotorInit(&s_msgqueue_si, &s_playercont[player].rumble, s_playercont[player].contindex);
    if (ret == 0)
        s_playercont[player].rumblestat = RUMBLE_OFF;
    return ret;
}


/*==============================
    controller_rumble_addtrauma
    Adds trauma to a player's controller
    @param The player who's controller we want to add trauma to
    @param The amount of trauma to add (from 0 to 1)
==============================*/

void controller_rumble_addtrauma(plynum player, f32 trauma)
{
    if (s_playercont[player].rumblestat == RUMBLE_ERR)
        return;
    s_playercont[player].trauma = clampf(s_playercont[player].trauma + trauma, 0.0f, 1.0f);
    osSendMesg(&s_msgqueue_cont, (OSMesg)(MSG_CONTROLLER_RUMBLE_P1+player), OS_MESG_BLOCK);
}


/*==============================
    controller_rumble_settrauma
    Sets the trauma on a player's controller
    @param The player who's controller we want to set the trauma of
    @param The trauma value to set (from 0 to 1)
==============================*/

void controller_rumble_settrauma(plynum player, f32 trauma)
{
    if (s_playercont[player].rumblestat == RUMBLE_ERR)
        return;
    s_playercont[player].trauma = clampf(trauma, 0.0f, 1.0f);
    if (s_playercont[player].trauma == 0)
        s_playercont[player].rumblestat = RUMBLE_OFF;
    osSendMesg(&s_msgqueue_cont, (OSMesg)(MSG_CONTROLLER_RUMBLE_P1+player), OS_MESG_BLOCK);
}