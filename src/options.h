// "options.h"

#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>
#include "tilemap_path_ops.h"

#define OPTION_UNSET 0xFFFF

enum tile_process_modes {
    MODE_DMG_4_COLOR,
    MODE_CGB_32_COLOR,
    MODE_ERROR_TOO_MANY_COLORS = 99
};

enum image_formats {
    FORMAT_GBR,  // For: Game Boy Tile Designer / GBTD
    FORMAT_GBM,  // For: Game Boy Map Builder / GBMB
    FORMAT_GBDK_C_SOURCE,  // For : GBDK/ZGB Game Boy Dev Kit
    FORMAT_PNG_OUT,
    FORMAT_UNKNOWN,
    FORMAT_LAST
};


typedef struct {
    uint16_t gb_mode;              // MODE_DMG_4_COLOR, MODE_CGB_32_COLOR
    uint16_t image_format;         // FORMAT_GBR, FORMAT_GBM, FORMAT_GBDK_C_SOURCE

    uint16_t tile_dedupe_enabled;
    uint16_t tile_dedupe_flips;    // only if (gb_mode == PROC_MODE_CGB_32_COLOR)
    uint16_t tile_dedupe_palettes; // only if (gb_mode == PROC_MODE_CGB_32_COLOR)

    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t palette_tile_width;
    uint16_t palette_tile_height;

    uint16_t ignore_palette_errors; // only if (gb_mode == PROC_MODE_CGB_32_COLOR)

    //       These need to get set *before* image load, 
    //       which then determines number of colors in input image
    //       which may later affect future calls
    uint16_t remap_pal;
    char     remap_pal_file[STR_FILENAME_MAX];
    uint16_t subpal_size; // should be even multiple of 

    uint16_t map_tileid_offset;
    uint16_t bank_num;

    char     varname[STR_FILENAME_MAX]; // output variable name for all formats

} tile_process_options;


void options_log(const char * str_heading, tile_process_options * p_options);
void options_reset(tile_process_options * p_options);
void options_color_defaults_if_unset(int color_count, tile_process_options * p_options);

#endif