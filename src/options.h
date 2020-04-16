// "options.h"

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>

enum tile_process_modes {
    MODE_DMG_4_COLOR,
    MODE_CGB_32_COLOR,
    MODE_ERROR_TOO_MANY_COLORS = 99
};

enum image_formats {
    FORMAT_GBR,  // For: Game Boy Tile Designer / GBTD
    FORMAT_GBM,  // For: Game Boy Map Builder / GBMB
    FORMAT_GBDK_C_SOURCE,  // For : GBDK/ZGB Game Boy Dev Kit
    FORMAT_LAST
};


typedef struct {
    uint16_t gb_mode;              // MODE_DMG_4_COLOR, MODE_CGB_32_COLOR
    uint16_t image_format;         // FORMAT_GBR, FORMAT_GBM, FORMAT_GBDK_C_SOURCE
    uint16_t tile_dedupe_enabled;
    uint16_t tile_dedupe_flips;    // only if (gb_mode == PROC_MODE_CGB_32_COLOR)
    uint16_t tile_dedupe_palettes; // only if (gb_mode == PROC_MODE_CGB_32_COLOR)

    uint16_t dmg_possible;
    uint16_t cgb_possible;

} tile_process_options;

#endif