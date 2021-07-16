// palette.c

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


#define PAL_MAX_STR_LEN 102


#define INT_RND_UP(A, B) ((A + (B - 1)) / B)
#define INT_RND_CLOSEST(A, B) ((A + (B / 2)) / B)


// TODO: ====== START palette.c ====


void palette_copy_rgblab_to_colordata_format(color_data * p_dst_pal, palette_rgb_LAB * p_src_pal) {

    p_dst_pal->color_count = p_src_pal->color_count;
    p_dst_pal->size        = p_src_pal->color_count * COLOR_DATA_BYTES_PER_COLOR;

    for (int c=0; c < p_src_pal->color_count; c++) {
        p_dst_pal->pal[(c * 3)    ]  = p_src_pal->colors[c].r;
        p_dst_pal->pal[(c * 3) + 1]  = p_src_pal->colors[c].g;
        p_dst_pal->pal[(c * 3) + 2]  = p_src_pal->colors[c].b;
    }
}


// This can get removed once conversion RGB-LAB format gets merged into color_data
void palette_copy_colordata_to_rgblab_format(color_data * p_src_colors, palette_rgb_LAB * p_dst_colors) {

    for(int c = 0; c < p_src_colors->color_count; c++) {

        p_dst_colors->colors[c].r = p_src_colors->pal[(c * 3) + 0];
        p_dst_colors->colors[c].g = p_src_colors->pal[(c * 3) + 1];
        p_dst_colors->colors[c].b = p_src_colors->pal[(c * 3) + 2];
    }

    p_dst_colors->color_count   = p_src_colors->color_count;
    p_dst_colors->compare_start = 0;
    p_dst_colors->compare_last  = p_src_colors->color_count - 1;
    p_dst_colors->subpal_size   = p_src_colors->subpal_size;
}


// Convert a palette to LAB format
void palette_convert_to_lab(palette_rgb_LAB * p_user_pal) {

    for (int c=0; c < p_user_pal->color_count; c++) {
        color_rgb2LAB( &(p_user_pal->colors[c]) );
    }
}


// Palette format: RGB Hex triplets, "#" sign is optional
// Example
//   #e0dfaf
//   #afb680
//   #707a55
//   #414425
bool palette_load_from_file(color_data * p_colors, char * filename) {

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
                p_colors->pal[(pal_index * 3) + 0] = (uint8_t)r;
                p_colors->pal[(pal_index * 3) + 1] = (uint8_t)g;
                p_colors->pal[(pal_index * 3) + 2] = (uint8_t)b;
                pal_index++;
            }
        }
        fclose(pal_file);
    } 
    else {
        log_error("Error: User Palette not found, unable to open: %s\n", filename);
        return false;
    }

    p_colors->color_count = pal_index;
    log_verbose("User Palette: Loaded %d colors\n", p_colors->color_count);

    if (p_colors->color_count > USER_PAL_MAX_COLORS) {
        log_error("Error, maximum number of colors is %d\n", USER_PAL_MAX_COLORS);
        return false;
    }

    return true;
}

