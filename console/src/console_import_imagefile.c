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
#include "palette.h"

#include "tilemap_error.h"
#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"
#include "lib_rom_bin.h"


static bool console_import_imagefile(image_data * p_src_image, color_data * p_src_colors, char * image_filename);
static bool save_image_to_tileformat(image_data * p_src_image, color_data * p_src_colors, char * filename_out);


// Load an image for processing
// Remap the palette if requested by the user
bool console_import_imagefile(image_data * p_src_image, color_data * p_src_colors, char * image_filename) {

    tile_process_options options;
    color_data user_palette;
    user_palette.subpal_size = p_src_colors->subpal_size;

    // Load options
    tilemap_options_get(&options);

    // Load source image (from first argument)
    if (!image_load(p_src_image, p_src_colors, image_filename)) {
        log_error("Error: Failed to load image\n\n");
        return false;
    }


    // Remap the image to a user specified palette if requested
    if (options.remap_pal) {
        log_standard(" --> options.remap_pal == true\n");
        if (options.remap_pal_file[0] != '\0') {
            log_standard(" --> load remap pal from FILE\n");
            if (!palette_load_from_file(&user_palette, options.remap_pal_file))
                return false;
        }
        else {
            log_standard(" --> load remap pal from SOURCE IMAGE\n");
            // Make a duplicate of source image colors, for remapping purposes
            memcpy(user_palette.pal, p_src_colors->pal, p_src_colors->color_count * 3);
            user_palette.color_count = p_src_colors->color_count;
        }

        if ( !image_remap_to_user_palette(p_src_image, p_src_colors, &user_palette) ) {
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
bool save_image_to_tileformat(image_data * p_src_image, color_data * p_src_colors, char * filename_out) {

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


bool console_image_to_tileformat_file(tile_process_options * p_user_options, char * filename_in, char * filename_out) {

    color_data src_colors;
    image_data src_image;

    // Need default tile size for setting default palette tile size *before* png image load & remapping,
    // but those defaults are typically set after image load with others based on image properties.
    // So, just set default tile size here as a workaround
    if ((p_user_options->tile_width == OPTION_UNSET) && (p_user_options->tile_height == OPTION_UNSET)) {
        p_user_options->tile_width = 8;
        p_user_options->tile_height = 8;
    }

    // Call these before loading the image and potentially remapping it's palette
    tilemap_image_and_colors_init(&src_image, &src_colors);
    tilemap_image_set_palette_tile_size(&src_image, p_user_options);
    tilemap_options_set(p_user_options);

    if (!console_import_imagefile(&src_image, &src_colors, filename_in ))
        return false;

    // Load default options based on output image format and number of colors in source image
    // Apply the finalized options
    options_color_defaults_if_unset(src_colors.color_count, p_user_options);
    tilemap_options_set(p_user_options);

    // Process and export the image
    if (!save_image_to_tileformat(&src_image, &src_colors, filename_out )) {

        if (tilemap_error_get() != TILE_ID_OK) {
            log_error("%s\n", tilemap_error_get_string() );
        }
        return false;
    }

    return true;
}
