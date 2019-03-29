/*=======================================================================
              TileMap & TileSet plugin for the GIMP
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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <libgimp/gimp.h>

#include "tilemap_write.h"
#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"

#include "image_info.h"

#include "lib_rom_bin.h"

int write_tilemap(const gchar * filename, gint image_id, gint drawable_id, gint image_mode)
{
    int32_t status;

    GimpDrawable * drawable;
    GimpPixelRgn rgn;
    GimpParasite * img_parasite;

    image_data app_image;
    color_data app_colors;

    guchar * p_cmap_buf;
    gint     cmap_num_colors;

    FILE * file;



    status = true; // Default to success

    // Get the drawable
    drawable = gimp_drawable_get(drawable_id);

    // Get the Bytes Per Pixel of the incoming app image
    app_image.bytes_per_pixel = (unsigned char)gimp_drawable_bpp(drawable_id);

// TODO: why is this showing "1" even when the source image has an alpha mask and transparency.
  // TODO: --> Maybe because of gimp_export_image?
    // Is it due to the export functionality?

    printf("write-tilemap.c: bytes per pixel= %d\n", app_image.bytes_per_pixel);

    // Abort if it's not 1 or 2 bytes per pixel
    // TODO: handle both 1 (no alpha) and 2 (has alpha) byte-per-pixel mode
    if (app_image.bytes_per_pixel > IMG_BITDEPTH_INDEXED_ALPHA) {

        return false;
    }

    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        FALSE, FALSE);


    // Determine the array size for the app's image then allocate it
    app_image.width      = drawable->width;
    app_image.height     = drawable->height;
    app_image.size       = drawable->width * drawable->height * app_image.bytes_per_pixel;
    app_image.p_img_data = malloc(app_image.size);

    // Get the image data
    gimp_pixel_rgn_get_rect(&rgn,
                            app_image.p_img_data,
                            0, 0,
                            drawable->width,
                            drawable->height);


    // Load the color map and copy it to a working buffer
    p_cmap_buf = gimp_image_get_colormap(image_id, &cmap_num_colors);

    if (p_cmap_buf) {
        memcpy(app_colors.pal, p_cmap_buf, cmap_num_colors * 3);
        app_colors.color_count = cmap_num_colors;
    }
    else
        status = false;

    printf("gimp_image_get_colormap: status= %d, colors=%d\n", status, cmap_num_colors);

    // TODO: Check colormap size and throw a warning if it's too large (4bpp vs 2bpp, etc)
//    if (status != 0) { };

    if (status) {
        switch (image_mode) {
            case EXPORT_FORMAT_GBDK_C_SOURCE:
                status = tilemap_export_process(&app_image);
                printf("tilemap_export_process: status= %d\n", status);


                // TODO: Separate out GBDK_C_SOURCE file format handling to a seperate library
                // gbdk_c_source_save
                // gbdk_c_source_format.c
                if (status)
                    status = tilemap_save(filename, image_mode);
                printf("tilemap_save: status= %d\n", status);
                break;

            case EXPORT_FORMAT_GBR:

                status = gbr_save(filename, &app_image, &app_colors);
                printf("gbr_save: status= %d\n", status);
                break;

            case EXPORT_FORMAT_GBM:
                status = tilemap_export_process(&app_image);
                printf("(gbm) tilemap_export_process: status= %d\n", status);

                if (status)
                    // TODO:
                    // Get processed Map tile set and map array
                    // status = gbm_save(filename, image_mode);
                printf("gbm_save: status= %d\n", status);
                break;
        }
    }

    // Free the image data
    if (app_image.p_img_data)
        free(app_image.p_img_data);

    // Detach the drawable
    gimp_drawable_detach(drawable);

    tilemap_free_resources();

    return (status);
}


