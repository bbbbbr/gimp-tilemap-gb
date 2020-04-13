//
// tilemap_read.c
//


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <libgimp/gimp.h>

#include "lib_tilemap.h"
#include "tilemap_read.h"

#include "image_info.h"

#include "lib_gbr.h"
#include "lib_gbm.h"

static void tilemap_read_free_resources(int image_mode);

static void tilemap_read_free_resources(int image_mode) {

    // Free allocated buffers / release resources
    switch (image_mode) {
        case IMPORT_FORMAT_GBR:
            gbr_free_resources();
            break;

        case IMPORT_FORMAT_GBM:
            gbm_free_resources();

            break;
    }
}

void tilemap_import_parasite_gbr(gint image_id) {

    // Store surplus (non-decodable) bytes from the rom into a gimp metadata parasite
    if (gbr_get_export_rec_size()) {
        printf("GBR: Storing parasite %d bytes\n",gbr_get_export_rec_size());
        gimp_image_attach_new_parasite(image_id,
                                      "GBR-EXPORT-SETTINGS",
                                       GIMP_PARASITE_PERSISTENT,
                                       gbr_get_export_rec_size(),
                                       gbr_get_export_rec_buffer());
    } else printf("GBR: Failed to store parasite\n");
}


void tilemap_import_parasite_gbm(gint image_id) {

    // Store surplus (non-decodable) bytes from the rom into a gimp metadata parasite
    if (gbm_get_export_rec_size()) {
        printf("GBM: Storing parasite %d bytes\n", gbm_get_export_rec_size());
        gimp_image_attach_new_parasite(image_id,
                                      "GBM-EXPORT-SETTINGS",
                                       GIMP_PARASITE_PERSISTENT,
                                       gbm_get_export_rec_size(),
                                       gbm_get_export_rec_buffer());
    } else printf("GBR: Failed to store parasite\n");
}


int tilemap_read(const gchar * filename, int image_mode)
{
    int status = 1;

    gint32 new_image_id,
           new_layer_id;
    GimpDrawable * drawable;
    GimpPixelRgn rgn;
    GimpParasite * parasite;

    image_data * p_loaded_image; // TODO: rename?
    color_data * p_loaded_colors;


    switch (image_mode) {
        case IMPORT_FORMAT_GBR:
            status = gbr_load(filename);
            if (status)
                p_loaded_image = gbr_get_image();

            break;

        case IMPORT_FORMAT_GBM:
            // TODO: gbm load
            status = gbm_load(filename);
            if (status)
                p_loaded_image = gbm_get_image();

            break;
    } // switch (image_mode)


    // Check to make sure that the load was successful
    if (!status) {
        printf("Image load failed \n");

        // Free allocated buffers / release resources
        tilemap_read_free_resources(image_mode);

        return -1;
    }



    // Now create the new INDEXED image.
    new_image_id = gimp_image_new(p_loaded_image->width,
                                  p_loaded_image->height,
                                  GIMP_INDEXED);

    // Create the new layer
    new_layer_id = gimp_layer_new(new_image_id,
                                  "Background",
                                  p_loaded_image->width,
                                  p_loaded_image->height,
                                  //GIMP_INDEXEDA_IMAGE,
                                  GIMP_INDEXED_IMAGE,
                                  100,
                                  GIMP_NORMAL_MODE);

    // Get the drawable for the layer
    drawable = gimp_drawable_get(new_layer_id);



    // Set up the indexed color map
    // and cache any export settings
    switch (image_mode) {
        case IMPORT_FORMAT_GBR:
            p_loaded_colors = gbr_get_colors();
            gimp_image_set_colormap(new_image_id,
                                    &(p_loaded_colors->pal[0]),
                                    p_loaded_colors->color_count);

            // Cache export settings in the image parasite metadata
            tilemap_import_parasite_gbr(new_image_id);
            break;

        case IMPORT_FORMAT_GBM:
            p_loaded_colors = gbm_get_colors();
            gimp_image_set_colormap(new_image_id,
                                    &(p_loaded_colors->pal[0]),
                                    p_loaded_colors->color_count);

            // Cache export settings in the image parasite metadata
            tilemap_import_parasite_gbr(new_image_id);
            tilemap_import_parasite_gbm(new_image_id);
            break;
    }


    // Get a pixel region from the layer
    gimp_pixel_rgn_init(&rgn,
                        drawable,
                        0, 0,
                        p_loaded_image->width,
                        p_loaded_image->height,
                        TRUE, FALSE);

    // Now FINALLY set the pixel data
    gimp_pixel_rgn_set_rect(&rgn,
                            p_loaded_image->p_img_data,
                            0, 0,
                            p_loaded_image->width,
                            p_loaded_image->height);


    // We're done with the drawable
    gimp_drawable_flush(drawable);
    gimp_drawable_detach(drawable);


    // Release any resources used during processing
    tilemap_read_free_resources(image_mode);


    // Add the layer to the image
    gimp_image_insert_layer(new_image_id, new_layer_id, -1, 0);

    // Set the filename
    gimp_image_set_filename(new_image_id, filename);

    return new_image_id;
}
