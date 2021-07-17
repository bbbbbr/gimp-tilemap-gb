//
// options.c
//
#include <stdio.h>
#include <string.h>

#include "logging.h"

#include "lib_tilemap.h"
#include "tilemap_io.h"
#include "tilemap_error.h"

#include "options.h"


void options_log(const char * str_heading, tile_process_options * p_options) {

    log_verbose("==== %s ====\n", str_heading);
    log_verbose("gb_mode:               %d\n", p_options->gb_mode);
    log_verbose("image_format:          %d\n", p_options->image_format);
    log_verbose("tile_dedupe_enabled:   %d\n", p_options->tile_dedupe_enabled);
    log_verbose("tile_dedupe_flips:     %d\n", p_options->tile_dedupe_flips);
    log_verbose("tile_dedupe_palettes:  %d\n", p_options->tile_dedupe_palettes);
    log_verbose("tile_size:             %d x %d\n", p_options->tile_width, p_options->tile_height);
    log_verbose("ignore_palette_errors: %d\n", p_options->ignore_palette_errors);
    log_verbose("remap_pal:             %d\n", p_options->remap_pal);
    log_verbose("remap_pal_file:        %s\n", p_options->remap_pal_file);
    log_verbose("bank_num:              %d\n", p_options->bank_num);
    log_verbose("varname:               %s\n", p_options->varname);
    log_verbose("\n");
}


void options_reset(tile_process_options * p_options) {


    p_options->gb_mode               = OPTION_UNSET;
    p_options->image_format          = OPTION_UNSET;

    p_options->tile_dedupe_enabled   = OPTION_UNSET;
    p_options->tile_dedupe_flips     = OPTION_UNSET;
    p_options->tile_dedupe_palettes  = OPTION_UNSET;

    p_options->tile_width            = OPTION_UNSET;
    p_options->tile_height           = OPTION_UNSET;

    p_options->ignore_palette_errors = OPTION_UNSET;

    p_options->remap_pal             = false;
    p_options->remap_pal_file[0]     = '\0';
    p_options->subpal_size           = OPTION_UNSET;

    p_options->map_tileid_offset     = OPTION_UNSET;
    p_options->bank_num              = OPTION_UNSET;

    p_options->varname[0]            = '\0';
}


void options_color_defaults_if_unset(int color_count, tile_process_options * p_options) {

    if (p_options->gb_mode == OPTION_UNSET) {

        if  (color_count > TILE_DMG_COLORS_MAX)
            p_options->gb_mode = MODE_CGB_32_COLOR;
        else
            p_options->gb_mode = MODE_DMG_4_COLOR;
    }

    if (p_options->tile_dedupe_enabled == OPTION_UNSET)
        p_options->tile_dedupe_enabled  = (p_options->image_format != FORMAT_GBR);

    // Only allow extra dedupe options in CGB and Map mode, regardless of user setting
    if ((p_options->image_format == FORMAT_GBR) || (p_options->gb_mode != MODE_CGB_32_COLOR)) {
        p_options->tile_dedupe_flips = false;
        p_options->tile_dedupe_palettes = false;
    }

    if (p_options->tile_dedupe_flips == OPTION_UNSET)
        p_options->tile_dedupe_flips  = (p_options->gb_mode == MODE_DMG_4_COLOR);

    if (p_options->tile_dedupe_palettes == OPTION_UNSET)
        p_options->tile_dedupe_palettes  = (p_options->gb_mode == MODE_DMG_4_COLOR);

    if (p_options->ignore_palette_errors == OPTION_UNSET)
        p_options->ignore_palette_errors  = false;

    options_log("options_color_defaults_if_unset() ", p_options);
}