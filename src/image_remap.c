// image_remap.c

//
// Support for remapping an image (8 bit/full color) to
// 8-bit indexed with a user specified palette
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>

#include <math.h>

#include "logging.h"

#include "image_remap.h"
#include "color_space.h"
#include "palette.h"
#include "options.h"

static bool pixel_get_rgb(color_rgb_LAB *, color_data *, uint8_t *, uint8_t);
static bool image_validate_settings(image_data *);
static bool palette_validate_settings(palette_rgb_LAB *);

static void image_replace_with_indexed_buffer(image_data *, uint8_t *, uint8_t);

static bool image_remap_to_palette(image_data *, color_data *, palette_rgb_LAB *);
static bool image_tiles_remap_to_subpalettes(image_data *, color_data *, palette_rgb_LAB *);

static bool tilexy_remap_to_subpal(image_data *, color_data *, palette_rgb_LAB *, uint8_t *, uint32_t, uint32_t, int);
static double tilexy_calc_subpal_distance(image_data *, color_data *, palette_rgb_LAB *, uint32_t, uint32_t, int);


#define INT_RND_UP(A, B) ((A + (B - 1)) / B)
#define INT_RND_CLOSEST(A, B) ((A + (B / 2)) / B)


// TODO: move this error checking farther up the stack
static bool image_validate_settings(image_data * p_image) {

    // For GBM / GBR export, these have to have to match the
    // main tile size even when CGB would allow finer grain
    // sub-palettes on 8x16 tiles for things like 16x16

    // Use 8x8 if tile size is not set
    // Otherwise they come from the main processing tile size
    // via: tilemap_image_set_palette_tile_size
    if (p_image->palette_tile_width == OPTION_UNSET)
        p_image->palette_tile_width = 8;

    if (p_image->palette_tile_height == OPTION_UNSET)
        p_image->palette_tile_height = 8;

    if (!p_image->p_img_data) {
        log_error("Error: Failed to remap image to user palette: source image data empty\n");
        return false;
    }
    else if (p_image->size != (p_image->width * p_image->height * p_image->bytes_per_pixel)) {
        log_error("Error: Failed to remap image to user palette: data size error\n");
        return false;
    }
    else if (((p_image->width % p_image->palette_tile_width) != 0) ||
             ((p_image->height % p_image->palette_tile_height) != 0)) {
        log_error("Error: Tile width and height (%d x %d) must be even multiple of image width and height (%d x %d)\n", p_image->palette_tile_width, p_image->palette_tile_height, p_image->width, p_image->height);
        return false;
    }
    else if (((p_image->palette_tile_width % 8) != 0) ||
             ((p_image->palette_tile_height % 8) != 0)) {
        log_error("Error: Tile width and height (%d x %d) must be even multiple of 8\n", p_image->palette_tile_width, p_image->palette_tile_height);
        return false;
    }

    log_verbose("Remapping image to palette: Tile width and height (%d x %d), Image width and height (%d x %d)\n", p_image->palette_tile_width, p_image->palette_tile_height, p_image->width, p_image->height);

    return true;
}


// TODO: move this error checking farther up the stack
static bool palette_validate_settings(palette_rgb_LAB * p_pal) {

    if (p_pal->subpal_size == 0) {
        log_error("Error: sub-pallet size (%d) cannot be zero\n",p_pal->color_count);
        return false;
    }
    else if ((p_pal->color_count % p_pal->subpal_size) != 0) {
        log_error("Error: sub-pallet size (%d) must be even multiple of whole pallet size (%d)\n", p_pal->subpal_size, p_pal->color_count);
        return false;
    }
    else if (p_pal->subpal_size == 0) {
        log_error("Error: sub-pallet size (%d) cannot be zero. Whole pallet size (%d)\n", p_pal->subpal_size, p_pal->color_count);
        return false;
    }

    return true;
}


static bool pixel_get_rgb(color_rgb_LAB * pixel_col, color_data * p_src_pal, uint8_t * p_src_pixel, uint8_t bytes_per_pixel) {

    if ((bytes_per_pixel == MODE_8_BIT_INDEXED) ||
        (bytes_per_pixel == MODE_8_BIT_INDEXED_ALPHA)) {
        if (*p_src_pixel >= p_src_pal->color_count) {
            log_error("Error: Failed to remap image to user palette: referenced index color larger than palette size\n");
            return false;
        }

        // Look up RGB data from palette
        // Skips alpha channel when present
        pixel_col->r = p_src_pal->pal[ (*p_src_pixel * 3)    ]; // R
        pixel_col->g = p_src_pal->pal[ (*p_src_pixel * 3) + 1]; // G
        pixel_col->b = p_src_pal->pal[ (*p_src_pixel * 3) + 2]; // B
    }
    else if ((bytes_per_pixel == MODE_24_BIT_RGB) ||
             (bytes_per_pixel == MODE_32_BIT_RGBA)) {

        // Straight copy of rgb data with no lookup
        // Skips alpha channel in 32 bit mode
        pixel_col->r = *(p_src_pixel);
        pixel_col->g = *(p_src_pixel + 1);
        pixel_col->b = *(p_src_pixel + 2);
    } else {
        log_error("Error: Failed to remap image to user palette: unsupported color depth: %d\n", bytes_per_pixel);
        return false;
    }

    return true;
}


static void image_replace_with_indexed_buffer(image_data * p_image, uint8_t * p_remapped_image, uint8_t bytes_per_pixel) {

    if (p_image->p_img_data)
        free(p_image->p_img_data);

    p_image->p_img_data      = p_remapped_image;
    p_image->bytes_per_pixel = bytes_per_pixel;
    p_image->size            = p_image->width * p_image->height;
}


// Input image (indexed, 24bit rgb, etc)
// Output remapped image (indexed only)
static bool image_remap_to_palette(image_data * p_src_image, color_data * p_src_pal, palette_rgb_LAB * p_user_pal) {

    uint8_t      * p_src_pixel = p_src_image->p_img_data;
    uint8_t      * p_dst_pixel;
    color_rgb_LAB  pixel_col;
    double         color_distance; // unused, but required for shared call


// TODO: fixme, handle this better
    // Compare full range of palette
    p_user_pal->compare_start = 0;
    p_user_pal->compare_last =  p_user_pal->color_count - 1;

    // Allocate a 1 byte per pixel indexed color image as output
    uint8_t * p_remapped_image = (uint8_t *)malloc(p_src_image->width * p_src_image->height);
    p_dst_pixel = p_remapped_image; // dst pixels are 8 bit indexed pal values

    // Convert all pixels in the image
    for (uint32_t y = 0; y < p_src_image->height; y++) {
        for (uint32_t x = 0; x < p_src_image->width; x++) {

            // extract color data for current source pixel
            if (!pixel_get_rgb(&pixel_col, p_src_pal, p_src_pixel, p_src_image->bytes_per_pixel))
                return false;

            if (!color_find_closest(p_user_pal, &pixel_col, p_dst_pixel, &color_distance))
                return false;

            // Move to next pixel in source and remapped image
            p_src_pixel += p_src_image->bytes_per_pixel;
            p_dst_pixel++;
        }
    }

    // Now that processing is complete, free the old image and swap in the new one
    // And copy user palette into image palette
    image_replace_with_indexed_buffer(p_src_image, p_remapped_image, MODE_8_BIT_INDEXED);
    palette_copy_rgblab_to_colordata_format(p_src_pal, p_user_pal);

    return true;
}



// Given a sub-palette Id, remap a tile to that palette as closely as  possible
// TODO: lot of duplicated code with tilexy_calc_subpal_distance()
static bool tilexy_remap_to_subpal(image_data * p_src_image, color_data * p_src_pal, palette_rgb_LAB * p_user_pal,
                              uint8_t * p_remapped_image,
                              uint32_t tile_id_x, uint32_t tile_id_y, int subpal_id) {

    double         color_distance;
    color_rgb_LAB  pixel_col;

    // Get pixel in first row and column of tile
    uint32_t  tile_start_offset = (tile_id_x * p_src_image->palette_tile_width) + ((tile_id_y * p_src_image->palette_tile_height) * p_src_image->width);
    uint8_t * p_src_pixel = p_src_image->p_img_data + (tile_start_offset * p_src_image->bytes_per_pixel);
    uint8_t * p_dst_pixel = p_remapped_image + tile_start_offset; // Always 8 BPP indexed


    // Set up subpal test range
    p_user_pal->compare_start = subpal_id * p_user_pal->subpal_size;
    p_user_pal->compare_last  = p_user_pal->compare_start + p_user_pal->subpal_size - 1;

    // Loop through all pixels in a given tile
    for (uint32_t tile_y = 0; tile_y < p_src_image->palette_tile_height; tile_y++) {
        for (uint32_t tile_x = 0; tile_x < p_src_image->palette_tile_width; tile_x++) {

            // extract color data for current source pixel
            if (!pixel_get_rgb(&pixel_col, p_src_pal, p_src_pixel, p_src_image->bytes_per_pixel))
                return false;

            if (!color_find_closest(p_user_pal, &pixel_col, p_dst_pixel, &color_distance))
                return false;

            // Move to next pixel in tile (in source and remapped image)
            p_src_pixel += p_src_image->bytes_per_pixel; // Always 8 BPP indexed
            p_dst_pixel++;

        } // End Tile Y loop

        // Move to start of next tile row (in source and remapped image)
        p_src_pixel += (p_src_image->width - p_src_image->palette_tile_width) * p_src_image->bytes_per_pixel;
        p_dst_pixel += (p_src_image->width - p_src_image->palette_tile_width); // Always 8 BPP indexed

    } // End Tile X loop

    return true;
}


// Find a sub-palette that maps as closely as possible for a given tile
//
// Currently uses a tile-global minimization, but doesn't weight small highlight as much as it should
//
// TODO: better ranking and weighting options
// TODO: cache color conversion output so that one can be selected and applied instantly without recalculating
static double tilexy_calc_subpal_distance(image_data * p_src_image, color_data * p_src_pal, palette_rgb_LAB * p_user_pal,
                                          uint32_t tile_id_x, uint32_t tile_id_y, int subpal_id) {
    uint8_t        matched_color_id;
    double         color_distance;
    double         color_distance_total = 0;
    color_rgb_LAB  pixel_col;

    // Get pixel in first row and column of tile
    uint32_t  tile_start_offset = (tile_id_x * p_src_image->palette_tile_width) + ((tile_id_y * p_src_image->palette_tile_height) * p_src_image->width);
    uint8_t * p_src_pixel = p_src_image->p_img_data + (tile_start_offset * p_src_image->bytes_per_pixel);


    // Set up subpal test range
    p_user_pal->compare_start = subpal_id * p_user_pal->subpal_size;
    p_user_pal->compare_last  = p_user_pal->compare_start + p_user_pal->subpal_size - 1;

    // Loop through all pixels in a given tile
    for (uint32_t tile_y = 0; tile_y < p_src_image->palette_tile_height; tile_y++) {
        for (uint32_t tile_x = 0; tile_x < p_src_image->palette_tile_width; tile_x++) {

            // extract color data for current source pixel
            if (!pixel_get_rgb(&pixel_col, p_src_pal, p_src_pixel, p_src_image->bytes_per_pixel))
                return false;

            if (!color_find_closest(p_user_pal, &pixel_col, &matched_color_id, &color_distance))
                return false;

            color_distance_total += color_distance;

            // Move to next pixel in tile
            p_src_pixel += p_src_image->bytes_per_pixel;

        } // End Tile Y loop

        // Move to start of next tile row
        p_src_pixel += (p_src_image->width - p_src_image->palette_tile_width) * p_src_image->bytes_per_pixel;

    } // End Tile X loop

    return color_distance_total;
}


// Remap an input image composed of tiles to a palette composed of multiple sub-palettes
//
// Expects :
// - subpal size to be set
// - tile width, height to be set
//
// Input image (indexed, 24bit rgb, etc)
// Output remapped image (indexed only)
//
// input image , tile sizes
// input palette , sub-palette sizes
static bool image_tiles_remap_to_subpalettes(image_data * p_src_image, color_data * p_src_pal, palette_rgb_LAB * p_user_pal) {

    uint8_t        subpal_best_match;
    double         color_distance_total;
    double         min_distance;
    int            subpal_id;

    int            subpal_count = INT_RND_UP(p_user_pal->color_count, p_user_pal->subpal_size);

    // Allocate a 1 byte per pixel indexed color image as output
    uint8_t * p_remapped_image = (uint8_t *)malloc(p_src_image->width * p_src_image->height);

    // Loop through all tiles of the image
    for (uint32_t tile_id_y = 0; tile_id_y < (p_src_image->height / p_src_image->palette_tile_height); tile_id_y++) {
        for (uint32_t tile_id_x = 0; tile_id_x < (p_src_image->width / p_src_image->palette_tile_width); tile_id_x++) {

            // Loop through all palettes
            // Test each one to calculate total distance for that pixel
            min_distance = DBL_MAX;
            subpal_best_match = 0;
            for (subpal_id = 0; subpal_id < subpal_count; subpal_id++) {

                color_distance_total = tilexy_calc_subpal_distance(p_src_image, p_src_pal, p_user_pal,
                                                                   tile_id_x, tile_id_y, subpal_id);
                if (color_distance_total < min_distance) {
                    min_distance = color_distance_total;
                    subpal_best_match = subpal_id;
                }
            }

            // Remap tile to best matched sub-palette
            if (!tilexy_remap_to_subpal(p_src_image, p_src_pal, p_user_pal, p_remapped_image,
                                        tile_id_x, tile_id_y, subpal_best_match)) {
                return false;
            }

        } // End Image Y loop
    } // End Image X loop

    // Now that processing is complete, free the old image and swap in the new one
    // And copy user palette into image palette
    image_replace_with_indexed_buffer(p_src_image, p_remapped_image, MODE_8_BIT_INDEXED);
    palette_copy_rgblab_to_colordata_format(p_src_pal, p_user_pal);

    return true;
}


bool image_remap_to_user_palette(image_data * p_src_image, color_data * p_src_colors, color_data * p_user_colors) {

    palette_rgb_LAB user_pal_rgblab;

    palette_copy_colordata_to_rgblab_format(p_user_colors, &user_pal_rgblab);

    if (image_validate_settings(p_src_image)) {

        if (palette_validate_settings(&user_pal_rgblab)) {

            // Pre-convert user palette to LAB for better distance calc performance later
            palette_convert_to_lab(&user_pal_rgblab);

            // Non-sub-palette mapping is disabled for now since CGB requires it
            // // rewrites p_src_image and p_src_colors if successful
            // if (image_remap_to_palette(p_src_image, p_src_colors, user_pal_rgblab)) {
            //     return true; // Success
            // }

            // rewrites p_src_image and p_src_colors if successful
            if (image_tiles_remap_to_subpalettes(p_src_image, p_src_colors, &user_pal_rgblab))
                return true; // Success
        }
    }

    return false;
}