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

/*
    FILE * file;


    app_gfx_data   app_gfx;
    app_color_data colorpal; // TODO: rename to app_colorpal?
    rom_gfx_data   rom_gfx;


    rom_bin_init_structs(&rom_gfx, &app_gfx, &colorpal);

    app_gfx.image_mode      = image_mode;
    app_gfx.bytes_per_pixel = BIN_BITDEPTH_INDEXED_ALPHA;


    // Try to open the file
    file = fopen(filename, "rb");
    if(!file)
        return -1;

    // Get the file size
    fseek(file, 0, SEEK_END);
    rom_gfx.size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Now prepare a buffer of that size
    // and read the data.
    rom_gfx.p_data = malloc(rom_gfx.size);
    fread(rom_gfx.p_data, rom_gfx.size, 1, file);

    // Close the file
    fclose(file);

    // Make sure the alloc succeeded
    if(rom_gfx.p_data == NULL)
        return -1;
*/


    if (image_mode == IMPORT_FORMAT_GBR) {
        status = gbr_load(filename);
    }


    // Check to make sure that the load was successful
    if (status) {
        p_loaded_image = gbr_get_image();
    }
    else {
        printf("Image load failed \n");

        // Free allocated buffers / release resources
        if (image_mode == IMPORT_FORMAT_GBR)
            gbr_free_resources();

        printf("Image load failed: free complete \n");

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
    if (image_mode == IMPORT_FORMAT_GBR) {

        p_loaded_colors = gbr_get_colors();

        gimp_image_set_colormap(new_image_id,
                                &(p_loaded_colors->pal[0]),
                                p_loaded_colors->size);
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


/*
    if ((app_gfx.surplus_bytes_size > 0) &&
        (app_gfx.p_surplus_bytes != NULL)) {

        // Store surplus (non-decodable) bytes from the rom into a gimp metadata parasite
         parasite = gimp_parasite_new("ROM-BIN-SURPLUS-BYTES",
                                       GIMP_PARASITE_PERSISTENT,
                                       app_gfx.surplus_bytes_size,
                                       app_gfx.p_surplus_bytes);
         gimp_image_attach_parasite(new_image_id,
                                    parasite);
         gimp_parasite_free (parasite);

//        // Free the surplus bytes now that they are stored as a parasite
  //      free(app_gfx.p_surplus_bytes);
    }
*/

    // We're done with the drawable
    gimp_drawable_flush(drawable);
    gimp_drawable_detach(drawable);


    // Free allocated buffers / release resources
    if (image_mode == IMPORT_FORMAT_GBR)
        gbr_free_resources();


    // Add the layer to the image
    gimp_image_insert_layer(new_image_id, new_layer_id, -1, 0);

    // Set the filename
    gimp_image_set_filename(new_image_id, filename);

    return new_image_id;
}
