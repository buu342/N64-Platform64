#ifndef PLATFORM64_CONTROLLER_H
#define PLATFORM64_CONTROLLER_H

    /*********************************
               Definitions
    *********************************/
    
    // Player helper macros. Don't touch
    #define PLAYER_1  0
    #define PLAYER_2  1
    #define PLAYER_3  2
    #define PLAYER_4  3

    // The total number of actions
    #define MAX_ACTIONS 1
    
    // The list of registerable actions
    #define ACTION_JUMP 0
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void controller_initialize();
    void controller_reinitialize_all();
    void controller_query_all();
    void controller_read_all();
    
    s32 controller_playercount();
    
    void controller_register_action(u8 player, u8 action, u16 buttons);
    void controller_unregister_action(u8 player, u8 action, u16 buttons);
    bool controller_action_pressed(u8 player, u8 action);
    bool controller_action_down(u8 player, u8 action);
    void controller_set_stickmin(u8 player, Octagon oct);
    void controller_set_stickmax(u8 player, Octagon oct);
    f32  controller_get_x(u8 player);
    f32  controller_get_y(u8 player);
    
    s32  controller_rumble_init(u8 player);
    void controller_rumble_start(u8 player);
    void controller_rumble_stop(u8 player);
    
#endif