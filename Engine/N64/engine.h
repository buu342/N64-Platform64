#ifndef PLATFORM64_SCENE_H
#define PLATFORM64_SCENE_H

    void engine_level_start();
    void engine_level_update();
    void engine_level_render();
    void engine_level_end();
    
    void engine_level_change(u32 levelid);
    bool engine_level_shouldchange();
    
    inline void scene_set_timescale(f32 timescale);
    inline void engine_set_subtick(f32 subtick);
    inline void engine_set_frametime(f32 frametime);
    inline void engine_increment_gametime();
    
    inline f32 engine_get_deltatime();
    inline f32 engine_get_deltatime_unscaled();
    inline f32 engine_get_frametime();
    inline f32 engine_get_subtick();
    inline f32 engine_get_timescale();
    inline u64 engine_get_gametime();
    
#endif