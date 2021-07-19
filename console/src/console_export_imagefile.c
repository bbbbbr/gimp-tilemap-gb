#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


#include "logging.h"
#include "image_info.h"
#include "options.h"

#include "palette.h"

#include "tilemap_path_ops.h"
#include "tilemap_error.h"
#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"
#include "lib_rom_bin.h"

#include "console_export_imagefile.h"
#include "lodepng.h"

static bool console_import_tilefiles(image_data ** pp_image, color_data ** p_p_colors, char * filename_in, uint16_t * p_file_in_type);
static bool save_image_to_png(image_data * p_image, color_data * p_colors, char * filename_out);


// Load .gbr and .gbm files
// Convert them to an indexed color image with a palette
static bool console_import_tilefiles(image_data ** pp_image, color_data ** pp_colors, char * filename_in, uint16_t * p_file_in_type) {

    int status = false;
    // Load and convert the GBR / GBM files
    if (matches_extension(filename_in, (char *)".gbr")) {

        *p_file_in_type = FORMAT_GBR;
        status = gbr_load(filename_in);

        if (status) {
            log_verbose("Loaded gbr file\n");
            *pp_image = gbr_get_image();
            *pp_colors = gbr_get_colors();
        }
    }
    else if (matches_extension(filename_in, (char *)".gbm")) {

        // Will auto-load the associated .gbr file
        *p_file_in_type = FORMAT_GBM;
        status = gbm_load(filename_in);

        if (status) {
            log_verbose("Loaded gbm file\n");
            *pp_image = gbm_get_image();
            *pp_colors = gbm_get_colors();
        }
    } else {
        status = false;
        log_error("Unknown file type based on extension: %s\n");
    }

    return status;
}


// Save an indexed color image in png format
static bool save_image_to_png(image_data * p_image, color_data * p_colors, char * filename_out) {

    LodePNGState png_state;
    int status = false;
    unsigned int error;
    unsigned char * p_png = NULL;
    unsigned char * p_png_image = NULL;
    long            png_size_bytes = 0;

    lodepng_state_init(&png_state);

    // Loop through colors and add them to the png palette
    for (int c = 0; c < p_colors->color_count; c++) {
        log_verbose("PNG adding color %d : %3d, %3d, %3d\n", c,
                            p_colors->pal[(c * 3) + 0], // r
                            p_colors->pal[(c * 3) + 1], // g
                            p_colors->pal[(c * 3) + 2]);

        lodepng_palette_add(&png_state.info_png.color,
                            p_colors->pal[(c * 3) + 0], // r
                            p_colors->pal[(c * 3) + 1], // g
                            p_colors->pal[(c * 3) + 2], // b
                            255);                                     // alpha (fully opaque)
        lodepng_palette_add(&png_state.info_raw,
                            p_colors->pal[(c * 3) + 0], // r
                            p_colors->pal[(c * 3) + 1], // g
                            p_colors->pal[(c * 3) + 2], // b
                            255);                                     // alpha (fully opaque)
    }


    // lodepng options: going from RAW to indexed PNG
    png_state.info_raw.colortype = LCT_PALETTE;
    png_state.info_raw.bitdepth = 8;


    // Palette must be added both to input and output color mode, because in this
    // Sample both the raw image and the expected PNG image use that palette.
    png_state.info_png.color.colortype = LCT_PALETTE;
    png_state.info_png.color.bitdepth = 8;
    png_state.encoder.auto_convert = 0;  // Specify exactly what output PNG color mode we want

   // Encode and save
    error = lodepng_encode(&p_png_image,
                                        &png_size_bytes,
                                        p_image->p_img_data,
                                        p_image->width, p_image->height,
                                        &png_state);
    if (error) {
        log_verbose("PNG encoder error: %u - %s\n", error, lodepng_error_text(error));
        status = false;
    }
    else {
        log_standard("Writing output image to png file: %d x %d, %d bytes to %s\n", p_image->width, p_image->height, png_size_bytes, filename_out);
        lodepng_save_file(p_png_image, png_size_bytes, filename_out);
        status = true;
    }

    // Free resources
    lodepng_state_cleanup(&png_state);

    return status;
}


bool console_tileformat_to_image_file(char * filename_in, char * filename_out) {

    uint16_t file_in_type = FORMAT_UNKNOWN;
    int status;
    image_data * p_image;
    color_data * p_colors;

    log_standard("Converting file %s to png format\n", filename_in);

    status = console_import_tilefiles(&p_image, &p_colors, filename_in, &file_in_type);

    // Convert the resulting (indexed) image and color data to a png
    if (status) {
        status = save_image_to_png(p_image, p_colors, filename_out);
    }

    // Release any resources used during processing
    switch (file_in_type) {
        case FORMAT_GBR:
            gbr_free_resources();
            break;

        case FORMAT_GBM:
            gbm_free_resources(); // This will also handle freeing gbr resources

            break;
    }

    return status;
}
