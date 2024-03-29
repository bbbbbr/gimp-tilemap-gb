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
#include <stdbool.h>
#include <string.h>
#include <libgimp/gimp.h>

#include "tilemap_write.h"
#include "lib_tilemap.h"
#include "lib_gbr.h"
#include "lib_gbm.h"

#include "image_info.h"
#include "lib_rom_bin.h"

#include "logging.h"
#include "options.h"

#include "image_remap.h"
#include "palette.h"


static void tilemap_export_parasite_gbr(gint image_id) {

    GimpParasite * img_parasite;
    img_parasite = gimp_image_get_parasite(image_id, "GBR-EXPORT-SETTINGS");

    if (img_parasite) {
        log_verbose("GBR: Found parasite size %d\n", img_parasite->size);

        // Load settings cached in the gimp metadata parasite
        gbr_set_export_from_buffer(img_parasite->size, (unsigned char *)img_parasite->data);

    } else log_verbose("GBR: No parasite found\n");
}


static void tilemap_export_parasite_gbm(gint image_id) {

    GimpParasite * img_parasite_map_export;
    GimpParasite * img_parasite_map_export_prop;

    img_parasite_map_export = gimp_image_get_parasite(image_id, "GBM-EXPORT-SETTINGS");

    if (img_parasite_map_export) {
        log_verbose("GBM: map export: Found parasite size %d\n", img_parasite_map_export->size);

        // Load settings cached in the gimp metadata parasite
        gbm_set_map_export_from_buffer(img_parasite_map_export->size, (unsigned char *)img_parasite_map_export->data);

    } else log_verbose("GBM: map export: No parasite found\n");


    img_parasite_map_export_prop = gimp_image_get_parasite(image_id, "GBM-EXPORT-PROP-SETTINGS");

    if (img_parasite_map_export_prop) {
        log_verbose("GBM: map export: Found parasite size %d\n", img_parasite_map_export_prop->size);

        // Load settings cached in the gimp metadata parasite
        gbm_set_map_export_prop_from_buffer(img_parasite_map_export_prop->size, (unsigned char *)img_parasite_map_export_prop->data);

    } else log_verbose("GBM: map export: No parasite found\n");


}


static int tilemap_export_remap_colors(image_data * p_src_image, color_data * p_src_colors, color_data * p_remap_colors, char * remap_pal_file) {

    // Determine where the palette should come from (file vs copy from source image)
    if (remap_pal_file[0] != '\0') {
        log_standard(" --> load remap pal from FILE\n");
        if (!palette_load_from_file(p_remap_colors, remap_pal_file))
            return false;
    }
    else {
        // Make a duplicate of source image colors, for remapping purposes
        log_standard(" --> load remap pal from SOURCE IMAGE\n");
        memcpy(p_remap_colors->pal, p_src_colors->pal, p_src_colors->color_count * 3);
        p_remap_colors->color_count = p_src_colors->color_count;
    }

    // Subpal size needs to get copied from the source image (even though it's always 4)
    p_remap_colors->subpal_size = p_src_colors->subpal_size;

    if ( !image_remap_to_user_palette(p_src_image, p_src_colors, p_remap_colors) ) {
        log_error("Error: remapping user palette failed!\n");
        return false;
    }

    return true;
}


int write_tilemap(const char * filename, gint image_id, gint drawable_id, const gchar * name)
{
    int32_t status;

    GimpDrawable * drawable;
    GimpPixelRgn rgn;

    image_data app_image;
    color_data app_colors; // initialize all palette colors to zero
    color_data remap_colors; // initialize all palette colors to zero

    guchar * p_cmap_buf;
    gint     cmap_num_colors;

    tile_process_options plugin_options;


    status = true; // Default to success

    // Load options
    tilemap_options_get(&plugin_options);

    tilemap_image_and_colors_init(&app_image, &app_colors);
    tilemap_image_set_palette_tile_size(&app_image, &plugin_options);

    // Get the drawable
    drawable = gimp_drawable_get(drawable_id);

    // Get the Bytes Per Pixel of the incoming app image
    app_image.bytes_per_pixel = (unsigned char)gimp_drawable_bpp(drawable_id);

// TODO: why is this showing "1" even when the source image has an alpha mask and transparency.
  // TODO: --> Maybe because of gimp_export_image?
    // Is it due to the export functionality?

    log_verbose("write-tilemap.c: bytes per pixel= %d\n", app_image.bytes_per_pixel);

    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        drawable->width,
                        drawable->height,
                        false, false);


    // Determine the array size for the app's image then allocate it
    app_image.width      = drawable->width;
    app_image.height     = drawable->height;
    app_image.size       = drawable->width * drawable->height * app_image.bytes_per_pixel;
    app_image.p_img_data = (unsigned char *) malloc(app_image.size);

    // Get the image data
    gimp_pixel_rgn_get_rect(&rgn,
                            app_image.p_img_data,
                            0, 0,
                            drawable->width,
                            drawable->height);


    // Handle Color Maps / Non-indexed color images
    //
    if (gimp_drawable_is_indexed(drawable_id))  {

        // Require palette remapping for indexed when it has an alpha channel
        if  (gimp_drawable_has_alpha(drawable_id) && (plugin_options.remap_pal == false)) {
            gimp_message("Error: Remapping to a Palette MUST be enabled when exporting indexed images with Alpha masks\n");
            status = false;
        }

        // For indexed color images: Load the color map and copy it to a working buffer
        p_cmap_buf = gimp_image_get_colormap(image_id, &cmap_num_colors);

        if (p_cmap_buf) {
            memcpy(app_colors.pal, p_cmap_buf, cmap_num_colors * 3);
            app_colors.color_count = cmap_num_colors;
        }
        else {
            gimp_message("Error: Unable to load palette from image\n");
            status = false;
        }

    }
    else {
        // Require palette remapping WITH a file for non-indexed RGB
        if  ((plugin_options.remap_pal == false) || (plugin_options.remap_pal_file[0] == '\0')) {
            gimp_message("Error: Remapping to a Palette File MUST be enabled when exporting images in Greyscale, RGB (non-indexed) or with Alpha masks\n");
            status = false;
        }
    }

    log_verbose("gimp_image_get_colormap: status= %d, colors=%d\n", status, cmap_num_colors);

    if (status) {
        // Try to repair / remap palette if requested
        if (plugin_options.remap_pal) {
            if (!tilemap_export_remap_colors(&app_image, &app_colors, &remap_colors, plugin_options.remap_pal_file))
                status = false; // Signal failure and exit
        }
    }

    if (status) {
        options_color_defaults_if_unset(app_colors.color_count, &plugin_options);
        tilemap_options_set(&plugin_options);

        // For plugin, set output variable name as filename
        copy_filename_without_path_and_extension(plugin_options.varname, filename);

        // TODO: Check colormap size and throw a warning if it's too large (4bpp vs 2bpp, etc)


        // Update any settings that might need it based on loaded image data (tile size, etc)
        if ( ! tilemap_image_update_settings(&app_image, &app_colors) ) {
            // tilemap_error_set(TILE_ID_INVALID_DIMENSIONS);
            status = false; // Signal failure and exit
        }
    }


    // Enforce 4 color limit if no overrides present
    if ((plugin_options.gb_mode == MODE_DMG_4_COLOR) &&
        (app_colors.color_count > TILE_DMG_COLORS_MAX)) {

        char error_str[1024];
        snprintf(error_str, 1024, "Error: Too many colors (%d) for 4 color export mode.\n\nTry Palette Remapping with a palette file with only 4 colors, or reduce the number of colors in the image.", app_colors.color_count);
        gimp_message(error_str);
        status = false; // Signal failure and exit
    }


    if (status) {

        switch (plugin_options.image_format) {
            case FORMAT_GBDK_C_SOURCE:

                status = tilemap_export_process(&app_image, &app_colors);
                log_verbose("tilemap_export_process: status= %d\n", status);

                if (status)
                    status = tilemap_save(filename, plugin_options.image_format);
                log_verbose("tilemap_save: status= %d\n", status);
                break;

            case FORMAT_GBR:

                // Load cached settings in the image parasite metadata
                tilemap_export_parasite_gbr(image_id);

                status = gbr_save(filename, &app_image, &app_colors, plugin_options); // TODO: CGB MODE: send entire plugin_options struct down?
                log_verbose("gbr_save: status= %d\n", status);
                break;

            case FORMAT_GBM:

                // Load cached settings in the image parasite metadata
                tilemap_export_parasite_gbr(image_id);
                tilemap_export_parasite_gbm(image_id);

                // Set processed Map tile set and map array
                status = gbm_save(filename, &app_image, &app_colors, plugin_options);
                log_verbose("gbm_save: status= %d\n", status);
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


