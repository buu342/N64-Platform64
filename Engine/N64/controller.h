#ifndef PLATFORM64_CONTROLLER_H
#define PLATFORM64_CONTROLLER_H

    void controller_initialize();
    void controller_reinitialize_all();
    void controller_query_all();
    void controller_read_all();
    s32 controller_playercount();
    s32  controller_rumble_init(u8 player);
    void controller_rumble_start(u8 player);
    void controller_rumble_stop(u8 player);
    
#endif