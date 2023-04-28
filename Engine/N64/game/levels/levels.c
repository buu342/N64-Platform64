#include <ultra64.h>
#include "../../types.h"
#include "levels.h"

LevelDef g_levels[] = {
    /*LEVEL_BOOT*/      {level_boot_start, level_boot_update, NULL, 1, {{FALSE, level_boot_render}}},
};

inline LevelDef* levels_get_all()
{
    return g_levels;
}