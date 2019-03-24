//
// lib_gbr.h
//

#include "lib_gbr.h"
#include "lib_gbr_import.h"
#include "lib_gbr_ops.h"
#include "lib_gbr_file_utils.h"

static image_data image;
static color_data colors;

static gbr_record gbr;


int32_t gbr_load(const int8_t * filename) {
    int32_t status;

    // Load and parse the file
    status = gbr_load_file(filename);

    //
    if (status) {
        // Render the image from the loaded data
        status = gbr_convert_to_image();
    }

    return status;
};


// Convert loaded .GBR data to an image
int32_t gbr_convert_to_image() {

    int16_t tile_id;
    int32_t offset;

    image.bytes_per_pixel = 1; // TODO: MAKE a #define

    image.width = gbr.tile_data.width;
    image.height = gbr.tile_data.height
                 * gbr.tile_data.count;  // TODO: consider a wider image format?

    // Calculate total image area based on
    // tile width and height, and number of tiles
    image.size = image.width * image.height * image.bytes_per_pixel;

    // Allocate image buffer
    image.p_img_data = malloc(image.size);

    if (image.p_img_data) {

        offset = 0;

        // LOAD IMAGE
        // Copy each tile into the image buffer
        // TODO: FIXME this is linear for now and assumes an 8 x N dest image

        for (tile_id=0; tile_id < gbr.tile_data.count; tile_id++) {
//            printf("Tile:%d, offset=%d\n", tile_id, offset);
            gbr_tile_get_buf(&image.p_img_data[offset],
                             &gbr,
                             tile_id);

            offset+= gbr.tile_data.width * gbr.tile_data.height * image.bytes_per_pixel;
        }

        printf("image:%d x %d (%d x %d # %d) \n", image.width, image.height,
                                                gbr.tile_data.width,
                                                gbr.tile_data.height,
                                                gbr.tile_data.count);

        // LOAD COLOR MAP
        colors.size = gbr.palettes.count * COLOR_DATA_BYTES_PER_COLOR;

        printf("COLOR: size=%d\n", colors.size);
        gbr_pal_get_buf(&(colors.pal[0]),
                        &gbr);
        return true;
    }
    else
        return false;
}



image_data * gbr_get_image() {

    printf("gbr_get_image\n");
    return &image;
}


color_data * gbr_get_colors() {

    printf("gbr_get_colors\n");
    return &colors;
}


// Load and parse a .GBR file
//
int32_t gbr_load_file(const int8_t * filename) {

    FILE               * p_file;
    pascal_file_object   obj;
    int32_t              status;
//    gbr_record           gbr;

    obj.p_data = malloc(PASCAL_OBJECT_MAX_SIZE);

    status = true;

    // open the file
    p_file = fopen(filename, "rb");
    if ((p_file != NULL) && (obj.p_data)) {

        if (gbr_read_header_key(p_file)) {

            printf("Found Header\n");

            if (gbr_read_version(p_file) && status) {

                // Read objects from the file until it's finished
                while (gbr_read_object_from_file(&obj, p_file)) {

                    printf("OBJ: type=%d, id=%d, size=%d\n", obj.type, obj.id, obj.length_bytes);
                    printf("OBJ: type=%02x, id=%02x, size=%04x\n", obj.type, obj.id, obj.length_bytes);

                    switch (obj.type) {
                        // Process Object
                        case gbr_obj_producer: printf("gbr_producer\n");
                                          status = gbr_object_producer_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_tile_data: printf("gbr_tile_data\n");
                                          status = gbr_object_tile_data_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_tile_settings: printf("gbr_tile_settings\n");
                                          status = gbr_object_tile_settings_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_tile_export: printf("gbr_tile_export\n");
                                          status = gbr_object_tile_export_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_tile_import: printf("gbr_tile_import\n");
                                          status = gbr_object_tile_import_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_palettes: printf("gbr_palettes\n");
                                          status = gbr_object_palettes_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_tile_pal: printf("gbr_tile_pal\n");
                                          status = gbr_object_tile_pal_decode(&gbr, &obj);
                                          break;

                        case gbr_obj_deleted: printf("gbr_deleted\n");
                                          break;
                    }
                }
            }
        }
    }

    if (p_file)
        fclose(p_file);

    if (obj.p_data)
        free(obj.p_data);

    return status;
}



void gbr_free_resources(void) {

    if (image.p_img_data)
        free(image.p_img_data);
};
