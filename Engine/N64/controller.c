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
#define MSG_CONTROLLER_QUERY  1
#define MSG_CONTROLLER_READ   2

// Controller distances
#define CONTROLLER_DEFAULT_MIN  {{0, 5},  {3, 3},   {4, 0},  {3, -3},   {0, -5},  {-3, -3},   {-4, 0},  {-3, 3}}
#define CONTROLLER_DEFAULT_MAX  {{0, 63}, {46, 46}, {61, 0}, {46, -46}, {0, -63}, {-46, -46}, {-61, 0}, {-46, 46}} // Nintendo recommended values
//#define CONTROLLER_DEFAULT_MAX  {{0, 83}, {64, 64}, {74, 0}, {64, -64}, {0, -83}, {-64, -64}, {-74, 0}, {-64, 64}} // My personal controller profile



/*********************************
        Function Prototypes
*********************************/

static void threadfunc_controller(void *arg);
static void controller_calcstick(u8 player);


/*********************************
             Globals
*********************************/

// Controller data
static OSContPad	s_contdata[MAXCONTROLLERS];
static OSContPad	s_contdata_old[MAXCONTROLLERS];
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
static s8      s_playerindex[MAXCONTROLLERS];
static u16     s_playeractions[MAXCONTROLLERS][MAX_ACTIONS];
static Octagon s_playerzone_min[MAXCONTROLLERS] = {CONTROLLER_DEFAULT_MIN, CONTROLLER_DEFAULT_MIN, CONTROLLER_DEFAULT_MIN, CONTROLLER_DEFAULT_MIN};
static Octagon s_playerzone_max[MAXCONTROLLERS] = {CONTROLLER_DEFAULT_MAX, CONTROLLER_DEFAULT_MAX, CONTROLLER_DEFAULT_MAX, CONTROLLER_DEFAULT_MAX};
static Vector2D s_playerstick[MAXCONTROLLERS];


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
    memset(s_playeractions, 0, sizeof(u16)*MAXCONTROLLERS*MAX_ACTIONS);
    memset(s_playerstick, 0, sizeof(Vector2D)*MAXCONTROLLERS);
    
    // Find out what player corresponds to what controller
    memset(s_playerindex, -1, sizeof(s8)*MAXCONTROLLERS);
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
        int i;
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
                memcpy(s_contdata_old, s_contdata, sizeof(OSContPad)*MAXCONTROLLERS);
                osContGetReadData(s_contdata);
                for (i=0; i<MAXCONTROLLERS; i++)
                    if (s_playerindex[i] != -1 && s_contdata[i].stick_x != s_contdata_old[i].stick_x && s_contdata[i].stick_y != s_contdata_old[i].stick_y)
                        controller_calcstick(i);
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
    memset(s_playerindex, -1, sizeof(u8)*MAXCONTROLLERS);
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
    // Since the last non negative value in s_playerindex is equivalent to our player count
    // (for example, if there's two players, then [0] and [1] will be non negative)
    // we can just iterate backwards and return the first non-negative index (plus 1).
    for (i=MAXCONTROLLERS-1; i>=0; i--)
        if (s_playerindex[i] != -1)
            return i+1;
    return 0;
}


/*==============================
    controller_register_action
    Registers an action to a specific set of buttons
    on a player's controller.
    @param The player to register the action. Use the PLAYER_X macros.
    @param The action to register
    @param The buttons to register. This should be a combination of
           Libultra button macros, like A_BUTTON.
==============================*/

void controller_register_action(u8 player, u8 action, u16 buttons)
{
    s_playeractions[player][action] |= buttons;
}


/*==============================
    controller_unregister_action
    Unregisters an action from a specific set of buttons
    on a player's controller.
    @param The player to unregister the action. Use the PLAYER_X macros.
    @param The action to unregister
    @param The buttons to unregister. This should be a combination of
           Libultra button macros, like A_BUTTON.
==============================*/

void controller_unregister_action(u8 player, u8 action, u16 buttons)
{
    s_playeractions[player][action] &= ~buttons;
}


/*==============================
    controller_action_pressed
    Checks if a specific action button was pressed *this frame*
    on a player's controller. 
    @param The player to check. Use the PLAYER_X macros.
    @param The action to check
==============================*/

bool controller_action_pressed(u8 player, u8 action)
{
    u8 l_index = s_playerindex[player];
    u16 l_action = s_playeractions[player][action];
    return (s_contdata[l_index].button & l_action) != 0 && (s_contdata_old[l_index].button & l_action) == 0;
}


/*==============================
    controller_action_down
    Checks if a specific action button is down on a player's
    controller. 
    @param The player to check. Use the PLAYER_X macros.
    @param The action to check
==============================*/

bool controller_action_down(u8 player, u8 action)
{
    return (s_contdata[s_playerindex[player]].button & s_playeractions[player][action]) != 0;
}


/*==============================
    controller_set_stickmin
    Sets a player's stick deadzone profile
    @param The player whos profile we want to modify.
           Use the PLAYER_X macros.
    @param The octagon that represents the deadzone
==============================*/

void controller_set_stickmin(u8 player, Octagon oct)
{
    s_playerzone_min[player] = oct;
}


/*==============================
    controller_set_stickmax
    Sets a player's stick maximum range profile
    @param The player whos profile we want to modify.
           Use the PLAYER_X macros.
    @param The octagon that represents the maximum range.
==============================*/

void controller_set_stickmax(u8 player, Octagon oct)
{
    s_playerzone_max[player] = oct;
}


/*==============================
    controller_set_stickmax
    Normalizes a player's stick coordinates to fit between [-1, 1],
    using their preconfigured stick deadzone and maximum range
    profiles.
==============================*/

static void controller_calcstick(u8 player)
{
    s32 l_x, l_y, l_xp, l_yp;
    u32 l_xa, l_ya;
    s32 l_minx1, l_minx2, l_miny1, l_miny2;
    s32 l_maxx1, l_maxx2, l_maxy1, l_maxy2;
    s32 l_quadrant = 0, l_octant = 0, l_octantnext = 0;
    f32 l_detmin, l_detmax, l_intmin_x, l_intmin_y, l_intmax_x, l_intmax_y;
    f32 l_ang;
    
    // Populate our helper variables
    l_x = s_contdata[player].stick_x;
    l_y = s_contdata[player].stick_y;
    if (l_x == 0 && l_y == 0)
    {
        s_playerstick[player].x = 0;
        s_playerstick[player].y = 0;
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
    l_minx1 = ((OctArray)s_playerzone_min[player]).array[l_octant*2];
    l_miny1 = ((OctArray)s_playerzone_min[player]).array[l_octant*2+1];
    l_maxx1 = ((OctArray)s_playerzone_max[player]).array[l_octant*2];
    l_maxy1 = ((OctArray)s_playerzone_max[player]).array[l_octant*2+1];
    l_octantnext = l_octant+1;
    if (l_octantnext > 7)
        l_octantnext = 0;
    l_minx2 = ((OctArray)s_playerzone_min[player]).array[l_octantnext*2];
    l_miny2 = ((OctArray)s_playerzone_min[player]).array[l_octantnext*2+1];
    l_maxx2 = ((OctArray)s_playerzone_max[player]).array[l_octantnext*2];
    l_maxy2 = ((OctArray)s_playerzone_max[player]).array[l_octantnext*2+1];
    
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
        s_playerstick[player].x = clampf((l_x - l_intmin_x)/(l_intmax_x - l_intmin_x), 0, 1)*cosf(l_ang);
    else
        s_playerstick[player].x = 0;
    if (l_intmax_y - l_intmin_y != 0)
        s_playerstick[player].y = clampf((l_y - l_intmin_y)/(l_intmax_y - l_intmin_y), 0, 1)*sinf(l_ang);
    else
        s_playerstick[player].y = 0;
}


/*==============================
    controller_get_x
    Gets the X value of a player's stick, normalized
    to -1 and 1 based on their stick profile.
    @param The player to get the stick X of.
           Use the PLAYER_X macros.
    @returns The normalized stick X value.
==============================*/

f32 controller_get_x(u8 player)
{
    return s_playerstick[player].x;
}


/*==============================
    controller_get_y
    Gets the Y value of a player's stick, normalized
    to -1 and 1 based on their stick profile.
    @param The player to get the stick Y of.
           Use the PLAYER_X macros.
    @returns The normalized stick Y value.
==============================*/

f32 controller_get_y(u8 player)
{
    return s_playerstick[player].y;
}


/*==============================
    controller_rumble_init
    Initializes the Rumble Pak for a specific player
    @param The player to initialize the Rumble Pak of.
           Use the PLAYER_X macros.
    @returns Any errors that ocurred, or zero
==============================*/

s32 controller_rumble_init(u8 player)
{
    if (s_playerindex[player] != -1)
        return osMotorInit(&s_msgqueue_si, s_contrumble, player);
    return PFS_ERR_NOPACK;
}


/*==============================
    controller_rumble_start
    Starts the motor on a player's Rumble Pak
    @param The player to start the Rumble Pak of.
           Use the PLAYER_X macros.
==============================*/

void controller_rumble_start(u8 player)
{
    osMotorStart(&s_contrumble[player]);
}


/*==============================
    controller_rumble_start
    Stops the motor on a player's Rumble Pak
    @param The player to stop the Rumble Pak of.
           Use the PLAYER_X macros.
==============================*/

void controller_rumble_stop(u8 player)
{
    osMotorStop(&s_contrumble[player]);
}