// "options.h"

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>



enum tile_process_modes {
    MODE_DMG_4_COLOR,
    MODE_CGB_32_COLOR,
    MODE_ERROR_TOO_MANY_COLORS = 99
};

enum export_types {
    EXPORT_GBR,
    EXPORT_GBM
};


typedef struct {
    uint16_t gb_mode;              // MODE_DMG_4_COLOR, MODE_CGB_32_COLOR
    uint16_t export_type;          // EXPORT_GBR, EXPORT_GBM
    uint16_t tile_dedupe_enabled;  // only if (gb_mode == PROC_MODE_CGB_32_COLOR)
    uint16_t tile_dedupe_flips;    // only if (gb_mode == PROC_MODE_CGB_32_COLOR)
    uint16_t tile_dedupe_palettes;

} tile_process_options;

#endif