#ifndef PLATFORM64_CONTROLLER_H
#define PLATFORM64_CONTROLLER_H

    /*********************************
               Definitions
    *********************************/
    
    // Max number of supported players in this game
    #define MAXPLAYERS  2

    // The total number of actions
    #define MAX_ACTIONS 1
    
    // The list of registerable actions
    #define ACTION_JUMP 0
    

    /*********************************
                 Typedefs             
    *********************************/
    
    // Player numbers
    typedef enum {
        PLAYER_1 = 0,
        PLAYER_2 = 1,
        PLAYER_3 = 2,
        PLAYER_4 = 3
    } plynum;
    
    
    /*********************************
            Function Prototypes
    *********************************/

    void controller_initialize();
    void controller_reinitialize_all();
    void controller_query_all();
    void controller_read_all();
    
    inline s32 controller_playercount();
    
    void controller_register_action(plynum player, u8 action, u16 buttons);
    void controller_unregister_action(plynum player, u8 action, u16 buttons);
    bool controller_action_pressed(plynum player, u8 action);
    bool controller_action_down(plynum player, u8 action);
    bool controller_action_released(plynum player, u8 action);
    void controller_set_stickmin(plynum player, Octagon oct);
    void controller_set_stickmax(plynum player, Octagon oct);
    f32  controller_get_x(plynum player);
    f32  controller_get_y(plynum player);
    
    void controller_rumble_init();
    s32  controller_rumble_check(plynum player);
    void controller_rumble_addtrauma(plynum player, f32 trauma);
    void controller_rumble_settrauma(plynum player, f32 trauma);
    
#endif