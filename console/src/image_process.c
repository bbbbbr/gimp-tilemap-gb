
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "logging.h"

#include "image_process.h"
#include "color_space.h"

#define PAL_MAX_STR_LEN 102

static color_data *  palette_convert_to_lab(palette_rgb_LAB *);
static bool image_remap_to_palette(image_data *, color_data *, palette_rgb_LAB *);
static void image_repair_tile_pals (void);
static bool palette_load_from_file(palette_rgb_LAB *, char *);


// Convert a palette to LAB format
static color_data *  palette_convert_to_lab(palette_rgb_LAB * p_user_pal) {
    // TODO N COL
    for (int c=0; c < p_user_pal->color_count; c++)
        color_rgb2LAB( &(p_user_pal->colors[c]) );
}


static bool pixel_get_rgb(color_rgb_LAB * pixel_col, color_data * p_src_pal, uint8_t * p_src_pixel, uint8_t bytes_per_pixel) {
    
    if (bytes_per_pixel == MODE_8_BIT_INDEXED) {
        if (*p_src_pixel >= p_src_pal->color_count) {
            log_error("Error: Failed to remap image to user palette: referenced index color larger than palette size\n");
            return false;
        }

// printf("  c: pixel_get_rgb: rgb(%02x,%02x,%02x) vs rgb(%02x,%02x,%02x)\n",
//     p_src_pal->pal[c].r, color_list->colors[c].g, color_list->colors[c].b,
//     pixel_col->r, pixel_col->g, pixel_col->b);

        // Look up RGB data from palette
        pixel_col->r = p_src_pal->pal[ (*p_src_pixel * 3)    ]; // R
        pixel_col->g = p_src_pal->pal[ (*p_src_pixel * 3) + 1]; // G
        pixel_col->b = p_src_pal->pal[ (*p_src_pixel * 3) + 2]; // B
    }
    else if ((bytes_per_pixel == MODE_24_BIT_RGB) ||
             (bytes_per_pixel == MODE_32_BIT_RGBA)) {

        pixel_col->r = *(p_src_pixel);
        pixel_col->g = *(p_src_pixel + 1);
        pixel_col->b = *(p_src_pixel + 2);

//printf(" pixel_get_rgb=rgb(%02x,%02x,%02x)\n", pixel_col->r, pixel_col->g, pixel_col->b);
        // Skips alpha channel in 32 bit mode
    } else {
        log_error("Error: Failed to remap image to user palette: unsupported color depth: %d\n", bytes_per_pixel);
        return false;
    }

    return true;
}


// Input image (indexed, 24bit rgb, etc)
// Output remapped image (indexed only)
static bool image_remap_to_palette(image_data * p_src_image, color_data * p_src_pal, palette_rgb_LAB * p_user_pal) {

    uint8_t      * p_src_pixel = p_src_image->p_img_data;
    uint8_t      * p_dst_pixel;
    color_rgb_LAB  pixel_col;

    if (!p_src_image->p_img_data) {
        log_error("Error: Failed to remap image to user palette: source image data empty\n");
        return false;
    }
    else if (p_src_image->size != (p_src_image->width * p_src_image->height * p_src_image->bytes_per_pixel)) {
        log_error("Error: Failed to remap image to user palette: data size error\n");
        return false;
    }        

    // log_verbose("p_src_image->width:%d\n", p_src_image->width);
    // log_verbose("p_src_image->height:%d\n", p_src_image->height);
    // log_verbose("p_src_image->size:%d\n", p_src_image->size);
    // log_verbose("p_src_image->bytes_per_pixel:%d\n", p_src_image->bytes_per_pixel);


// printf("image_remap_to_palette -> after checks\n");

    // Allocate a 1 byte per pixel indexed color image as output
    uint8_t * p_remapped_image = (uint8_t *)malloc(p_src_image->width * p_src_image->height);
    p_dst_pixel = p_remapped_image; // dst pixels are 8 bit indexed pal values
                        
    // Convert all pixels in the image
    for (int x = 0; x < p_src_image->width; x++) {
        for (int y = 0; y < p_src_image->height; y++) {

//log_verbose("image_remap_to_palette -> pixel %3d, %3d *psrc=(%d)", x, y, *p_src_pixel);

            // extract color data for current source pixel
            // if (!pixel_get_rgb(&pixel_col, p_src_pal, p_src_pixel, p_src_image->bytes_per_pixel))
            //     return false;        
pixel_get_rgb(&pixel_col, p_src_pal, p_src_pixel, p_src_image->bytes_per_pixel);
            


//log_verbose(" in.rgb(%02x,%02x,%02x)", pixel_col.r, pixel_col.g, pixel_col.b);

            // try for an exact (and more efficient) RGB match first
            if (!color_find_exact_RGB(p_user_pal, &pixel_col, p_dst_pixel)) {

                // Otherwise find a closest match using LAB color space
                color_rgb2LAB(&pixel_col);
                if (!color_find_closest_LAB(p_user_pal, &pixel_col, p_dst_pixel)) {

                     log_error("Error: Failed to remap image to user palette: failed when searching for closest L A B color\n");
                    return false;                    
                } 
//else log_verbose(" closestLAB= %d", *p_dst_pixel);

            } 
//else log_verbose(" exactrgb= %d", *p_dst_pixel);

// log_verbose("\n");

            // Move to next pixel in source and remapped image
            p_src_pixel += p_src_image->bytes_per_pixel;
            p_dst_pixel++;
        }
    }

    // Now that processing is complete, free the old image and swap in the new one
    free(p_src_image->p_img_data);

    p_src_image->p_img_data      = p_remapped_image;
    p_src_image->bytes_per_pixel = MODE_8_BIT_INDEXED;
    p_src_image->size            = p_src_image->width * p_src_image->height;

    // Copy palette
    p_src_pal->color_count = p_user_pal->color_count;
    p_src_pal->size        = p_user_pal->color_count * COLOR_DATA_BYTES_PER_COLOR;
    for (int c=0; c < p_user_pal->color_count; c++) {
        p_src_pal->pal[(c * 3)    ]  = p_user_pal->colors[c].r;
        p_src_pal->pal[(c * 3) + 1]  = p_user_pal->colors[c].g;
        p_src_pal->pal[(c * 3) + 2]  = p_user_pal->colors[c].b;
    }
    printf("image_remap_to_palette -> end\n");

    return true;
}


// Try to fix CGB one sub-palette (4 colors each) per-tile errors
// Fix DMG too??
// TODO: make the palette size adjustable
static void image_repair_tile_pals (void) {
    // find colors which have duplicates in the palette and might cause palette errors
    // -> ignore those
    //   AKA only evaluate colors which are unique in a palette, avoid dupes which exist in multiple sub-palettes

    // find all sub-palettes used in a tile
    // choose intended sub-palette based on:
    //  * the most used color?
    //  * the sub-palette with the most colors preset <--
    //  try to repair colors that
}


// Palette format: RGB Hex triplets, "#" sign is optional
// Example
//   #e0dfaf
//   #afb680
//   #707a55
//   #414425
static bool palette_load_from_file(palette_rgb_LAB * pal, char * filename) {

    uint32_t r,g,b; // Would prefer this be a uint8_t, but mingw sscanf("%2hhx") has a buffer overflow that corrupts adjacent data
    int      pal_index = 0;;
    char     strline_in[PAL_MAX_STR_LEN] = "";

    log_standard("Loading palette from file: %s\n", filename);

    FILE * pal_file = fopen(filename, "r");
    
    if (pal_file) {
                // Read one line at a time into \0 terminated string
        while ( (fgets(strline_in, sizeof(strline_in), pal_file) != NULL) &&
                (pal_index <= USER_PAL_MAX_COLORS) ) {

// TODO: improve empty line detection
            if (strlen(strline_in) >= 6) {
                // Read a RGB hex triplet in text format
                // If first try fails, try again and ignore unrelated characters
                if (3 != sscanf(strline_in, "%2x%2x%2x", &r, &g, &b)) {
                    if (3 != sscanf(strline_in, "%*[^A-Za-z0-9]%2x%2x%2x", &r, &g, &b)) {
                        log_error("Error: User Palette formatting error.\n"
                                  "Format must be: RGB in hex text, 1 color per line (ex: FF0080)\n"
                                  "Read: %s", strline_in);
                        return false;
                    }
                }

                // Read succeeded
                log_verbose("Pal from file: (%3d) = %02x, %02x, %02x\n", pal_index, r, g, b);
                pal->colors[pal_index].r = (uint8_t)r;
                pal->colors[pal_index].g = (uint8_t)g;
                pal->colors[pal_index].b = (uint8_t)b;
                pal_index++;
            }
        }
        fclose(pal_file);
    } 
    else {
        log_error("Error: User Palette not found, unable to open: %s\n", filename);
        return false;
    }

    pal->color_count = pal_index;
    log_verbose("User Palette: Loaded %d colors\n", pal->color_count);

    return true;
}



bool image_remap_to_user_palette (image_data * p_src_image, color_data * p_src_colors, char * user_colors_filename) {

    palette_rgb_LAB user_palette[USER_PAL_MAX_COLORS];

    if (palette_load_from_file(user_palette, user_colors_filename)) {

        // Pre-convert user palette to LAB for better distance calc performance later
        palette_convert_to_lab(user_palette);

        // rewrites p_src_image and p_src_colors if successful
        if (image_remap_to_palette(p_src_image, p_src_colors, user_palette)) {
            return true; // Success
        }
    }

    return false;
}