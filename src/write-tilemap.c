/*=======================================================================
              ROM bin load / save plugin for the GIMP
                 Copyright 2018 - Others & Nathan Osman (webp plugin base)

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
=======================================================================*/

#include "write-tilemap.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgimp/gimp.h>

#include "lib_tilemap.h"

int write_tilemap(const gchar * filename, gint image_id, gint drawable_id, int image_mode)
{
    int status;

    GimpDrawable * drawable;
    GimpPixelRgn rgn;
    GimpParasite * img_parasite;

    // int              image_mode;
    unsigned int     width;
    unsigned int     height;
    unsigned char  * p_data;
    unsigned char    bytes_per_pixel;
    int              size;


    FILE * file;

    status = 0; // Default to success

/*
    app_gfx_data   app_gfx;
    app_color_data colorpal; // TODO: rename to app_colorpal?
    rom_gfx_data   rom_gfx;

    rom_bin_init_structs(&rom_gfx, &app_gfx, &colorpal);

    app_gfx.image_mode = image_mode;
*/

    // Get the drawable
    drawable = gimp_drawable_get(drawable_id);

    // Get the Bytes Per Pixel of the incoming app image
    bytes_per_pixel = (unsigned char)gimp_drawable_bpp(drawable_id);

    // Abort if it's not 1 or 2 bytes per pixel
    // TODO: handle both 1 (no alpha) and 2 (has alpha) byte-per-pixel mode
    if (bytes_per_pixel >= IMG_BITDEPTH_LAST) {
        return 0;
    }

    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        FALSE, FALSE);


    // Determine the array size for the app's image then allocate it
    width   = drawable->width;
    height  = drawable->height;
    size    = drawable->width * drawable->height * bytes_per_pixel;
    p_data  = malloc(size);

    // Get the image data
    gimp_pixel_rgn_get_rect(&rgn,
                            p_data,
                            0, 0,
                            drawable->width,
                            drawable->height);


// TODO: EXPORT
/*
    status = export_process(&rom_gfx,
                            &app_gfx);
*/
    // TODO: Check colormap size and throw a warning if it's too large (4bpp vs 2bpp, etc)
    if (status != 0) { };


    // Free the image data
    free(p_data);

    // Detach the drawable
    gimp_drawable_detach(drawable);


    // Make sure that the write was successful
    if(size == FALSE) {
        free(p_data);
        return 0;
    }

    // Open the file
    file = fopen(filename, "wb");
    if(!file) {
        free(p_data);
        return 0;
    }

    // Write the data and close it
    fwrite(p_data, size, 1, file);
    free(p_data);
    fclose(file);

    return 1;
}
