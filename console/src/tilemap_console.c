// tilemap_console.c

//
// Console wrapper for lib_tilemap
//
// Allows loading of PNG and export to GBR/GBM/C Sources
//

#include "lodepng.h"

#include "logging.h"
#include "image_info.h"
#include "options.h"

#include "tilemap_error.h"

#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"
#include "lib_rom_bin.h"


int tilemap_unbitpack_image(uint8_t * p_src_image, uint8_t * p_dest_image, uint8_t bitdepth, long src_size_bytes) {

    long c;
    int b;
    int pixels_per_byte;

    pixels_per_byte = (8 / bitdepth);

    for (c=0; c < src_size_bytes; c++) {

        for (b = 0; b < pixels_per_byte; b++) {

            *p_dest_image = *p_src_image >> (8 - bitdepth); // Extract pixel bits
            p_dest_image++; // Move to next pixel in destination image
            *p_src_image <<= bitdepth; // // Mask out the just extracted bits in the source image

        }
        // move to next (byte) group of packed pixels in the source image
        p_src_image++;
    }
}


int tilemap_load_image(image_data * p_src_image, color_data * p_src_colors, char * filename) {

    int status = true;
    unsigned int err, c;
    unsigned char * p_png = NULL;
    unsigned char * p_image = NULL;
    unsigned int width, height;
    long         src_image_size_bytes = 0;
    unsigned int color_count = 0;
    size_t pngsize;
    LodePNGState state;

    log_standard("Loading image from file: %s\n", filename);

    // Initialize png library
    lodepng_state_init(&state);

    // Load the PNG file
    lodepng_load_file(&p_png, &pngsize, filename);

    // Zero out buffer pointer
    p_src_image->p_img_data = NULL;

    // Decode it with conversion disabled
    state.decoder.color_convert = 0;
    err = lodepng_decode(&p_image, &width, &height, &state, p_png, pngsize);
    free(p_png);

    // Fail on errors and Require indexed color
    if (err) {
        status = false;
        log_error("Error: PNG load: %u: %s\n", err, lodepng_error_text(err));
    }
    else if (state.info_png.color.colortype != LCT_PALETTE) {
        status = false;
        log_error("Error: PNG colortype 3 (indexed, 256 colors max) expected!\n");
    }
    else if ((state.info_png.color.bitdepth  > 8) ||
             (state.info_png.color.bitdepth  < 1)) {
        status = false;
        log_error("Error: Decoded image must be between 1 and 8 bits per pixel\n");
    }

    if (status) {

        // Source image could be bit-packed, so reduce byte count accordingly
        src_image_size_bytes = (width * height) / (8 / state.info_png.color.bitdepth);

        // == IMAGE PROPERTIES from decoded PNG ==
        // Determine the array size for the app's image then allocate it
        p_src_image->bytes_per_pixel = 1; // hardwire since state.info_png.color.bitdepth can be between 1 and 8
        p_src_image->width      = width;
        p_src_image->height     = height;
        p_src_image->size       = p_src_image->width * p_src_image->height * p_src_image->bytes_per_pixel;
        p_src_image->p_img_data = (uint8_t *)malloc(p_src_image->size);  // lodepng_decode() handles allocation

        // Can't memcpy to destination image directly since the
        // loaded png image may be bitpacked. So instead, unpack it.
        tilemap_unbitpack_image(p_image,                 // input
                                p_src_image->p_img_data, // output
                                state.info_png.color.bitdepth,
                                src_image_size_bytes);

        log_verbose("p_src_image->width:%d\n", p_src_image->width);
        log_verbose("p_src_image->height:%d\n", p_src_image->height);
        log_verbose("p_src_image->size:%d\n", p_src_image->size);
        log_verbose("p_src_image->bytes_per_pixel:%d\n", p_src_image->bytes_per_pixel);

        log_verbose("colortype:%d\n", state.info_raw.colortype);
        log_verbose("bitdepth:%d\n", state.info_raw.bitdepth);

        // == COLOR DATA ==
        // Load color palette info and data
        p_src_colors->color_count = state.info_png.color.palettesize;

        // Load colors, stripping out the Alpha component of the RGBA palette
        for (c=0; c < p_src_colors->color_count; c++){
            p_src_colors->pal[c * 3    ] = (unsigned char)state.info_png.color.palette[c*4];   /* R */
            p_src_colors->pal[c * 3 + 1] = (unsigned char)state.info_png.color.palette[c*4+1]; /* G */
            p_src_colors->pal[c * 3 + 2] = (unsigned char)state.info_png.color.palette[c*4+2]; /* B */
        }
    }

    // Free resources
    lodepng_state_cleanup(&state);

    return status;
}


int tilemap_process_and_save_image(image_data * p_src_image, color_data * p_src_colors, char * filename) {

    int status = true;
    tile_process_options options;

    log_standard("Writing output to file: %s\n", filename);

    // Load options
    tilemap_options_get(&options);

    switch (options.image_format) {

        case FORMAT_GBDK_C_SOURCE:
            status = tilemap_export_process(p_src_image);
            log_verbose("tilemap_export_process: status= %d\n", status);

            if (status)
                status = tilemap_save(filename, options.image_format);
            log_verbose("tilemap_save: status= %d\n", status);
            break;

        case FORMAT_GBR:
            status = gbr_save(filename, p_src_image, p_src_colors, options); // TODO: CGB MODE: send entire options struct down?
            log_verbose("gbr_save: status= %d\n", status);
            break;

        case FORMAT_GBM:
            // Set processed Map tile set and map array
            status = gbm_save(filename, p_src_image, p_src_colors, options);
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
