// tilemap_console.c

//
// Console wrapper for lib_tilemap
// Loads images and exports to GBR/GBM/C Sources
//

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "logging.h"
#include "image_info.h"
#include "options.h"

#include "image_load.h"
#include "image_remap.h"

#include "tilemap_error.h"
#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"
#include "lib_rom_bin.h"


// Load an image for processing
// Remap the palette if requested by the user

bool tilemap_load_and_prep_image(image_data * p_src_image, color_data * p_src_colors, char * image_filename) {

    tile_process_options options;

    // Load options
    tilemap_options_get(&options);

    // Load source image (from first argument)
    if (!image_load(p_src_image, p_src_colors, image_filename)) {
        log_error("Error: Failed to load image\n\n");
        return false;
    }

    // Remap the image to a user specified palette if requested
    if (options.remap_pal) {
        if ( !image_remap_to_user_palette(p_src_image, p_src_colors, options.remap_pal_file) ) {
            log_error("Error: remapping png to user palette failed!\n");
            return false;
        }            
    } 
    else if (p_src_image->bytes_per_pixel != MODE_8_BIT_INDEXED) { 
        log_error("Error: non-indexed color images are only supported when remapping to a user palette (-pal=)!\n");
        return false;
    }

    return true;
}


// Process a loaded image into a tilemap and/or tileset
// Then write it out to the desired format
bool tilemap_process_and_save_image(image_data * p_src_image, color_data * p_src_colors, char * filename_out) {

    int status = true;
    tile_process_options options;

    log_standard("Writing output to file: %s\n", filename_out);

    // Load options
    tilemap_options_get(&options);

    // Update any settings that might need it based on loaded image data (tile size, etc)
    if ( ! tilemap_image_update_settings(p_src_image, p_src_colors)) {
        tilemap_error_set(TILE_ID_INVALID_DIMENSIONS);
        return (false); // Signal failure and exit
    }

    switch (options.image_format) {

        case FORMAT_GBDK_C_SOURCE:
            log_standard("csource export: %d x %d with mode = %d, dedupe tile pattern = %d, dedupe flip = %d, dedupe pal = %d, ignore pal error = %d\n",
                    p_src_image->width, p_src_image->height,
                    options.gb_mode,
                    options.tile_dedupe_enabled, options.tile_dedupe_flips, options.tile_dedupe_palettes,
                    options.tile_dedupe_palettes);

            status = tilemap_export_process(p_src_image, p_src_colors);
            log_verbose("tilemap_export_process: status= %d\n", status);

            if (status)
                status = tilemap_save(filename_out, options.image_format);
            log_verbose("tilemap_save: status= %d\n", status);
            break;

        case FORMAT_GBR:
            status = gbr_save(filename_out, p_src_image, p_src_colors, options); // TODO: CGB MODE: send entire options struct down?
            log_verbose("gbr_save: status= %d\n", status);
            break;

        case FORMAT_GBM:
            // Set processed Map tile set and map array
            status = gbm_save(filename_out, p_src_image, p_src_colors, options);
            log_verbose("gbm_save: status= %d\n", status);
            break;
    }

    // Free the image data
    if (p_src_image->p_img_data)
        free(p_src_image->p_img_data);

    tilemap_free_resources();

    if (tilemap_error_get() != TILE_ID_OK) {
        log_error(tilemap_error_get_string());
    }


    return status;
}
