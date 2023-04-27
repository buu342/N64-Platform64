#ifndef PLATFORM64_SCENE_H
#define PLATFORM64_SCENE_H
        
    void scene_initialize();
    void scene_update();
    void scene_render();
    
    inline bool scene_shouldchange();
    
    inline void scene_set_timescale(f32 timescale);
    inline f32 scene_get_deltatime();
    inline f32 scene_get_deltatime_unscaled();
    inline f32 scene_get_frametime();
    inline f32 scene_get_subtick();
    inline f32 scene_get_timescale();
    inline u64 scene_get_gametime();
    
#endif