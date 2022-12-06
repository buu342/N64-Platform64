#ifndef PLATFORM64_CONTROLLER_H
#define PLATFORM64_CONTROLLER_H

    /*********************************
               Definitions
    *********************************/

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
    
    s32  controller_rumble_init(u8 player);
    void controller_rumble_start(u8 player);
    void controller_rumble_stop(u8 player);
    
#endif