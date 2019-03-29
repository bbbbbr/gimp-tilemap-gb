//
// lib_gbm.c
//

#include "lib_gbr.h"
#include "lib_gbm.h"
// #include "lib_gbr_import.h"
// #include "lib_gbr_export.h"
#include "lib_gbm_file_utils.h"

static image_data image;
static color_data colors;

static gbr_record gbr;




int32_t gbm_load(const int8_t * filename) {

    int32_t status;

    printf("gbm_load\n");
    // Load and parse the file
    status = gbm_load_file(filename);
/*
    if (status) {
        // Render the image from the loaded data
        status = gbr_convert_tileset_to_image(&gbr, &image, &colors);
    }

    // TODO: Store imported GBR structure as metadata in GimpParasite? (then write out on export )
*/
    return status;
};



// TODO: Import calette color data?
// TODO: Try to maintain palette color order? Store in Gimp Parasite?
// TODO: ADD gbr_set_image for SOURCE image?
int32_t gbm_save(const int8_t * filename, image_data * p_src_image, color_data * p_colors) {

    int32_t status;
/*
    // Initialize shared GBR structure with defaults
    gbr_export_set_defaults(&gbr);

    // TODO: check
    // TODO: MIN 16 tiles required in file?
    // Convert the image data to tiles
    status = gbr_convert_image_to_tileset(&gbr, p_src_image, p_colors);

    // Load and parse the file
    if (status)
        status = gbr_save_file(filename);
*/
    return status;
};



// Load and parse a .GBR file
//
int32_t gbm_load_file(const int8_t * filename) {

    FILE               * p_file;
    gbm_file_object      obj;
    int32_t              status;

    obj.p_data = malloc(PASCAL_OBJECT_MAX_SIZE);

    status = true;

    printf("gbm_load_file\n");

    // open the file
    p_file = fopen(filename, "rb");
    if ((p_file != NULL) && (obj.p_data)) {

        if (gbm_read_header_key(p_file)) {

            printf("Found Header\n");

            if (gbm_read_version(p_file) && status) {

                // Read objects from the file until it's finished
                while (gbm_read_object_from_file(&obj, p_file)) {

                    //printf("OBJ: type=%d, id=%d, size=%d\n", obj.id, obj.object_id, obj.length_bytes);
                    printf("GBM OBJ: type=%02x, object_id=%02x, master_id=%02x, size=%04x\n",
                           obj.id, obj.object_id, obj.master_id, obj.length_bytes);

                    switch (obj.id) {
                        // Process Object
                        case gbm_obj_producer: printf("gbm_producer\n");
                                          //status = gbr_object_producer_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_map: printf("gbm_obj_map\n");
                                          //status = gbr_object_tile_data_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_tile_data: printf("gbm_obj_tile_data\n");
                                          //status = gbr_object_tile_settings_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_map_prop: printf("gbm_obj_map_prop\n");
                                          //status = gbr_object_tile_export_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_prop_data: printf("gbm_obj_prop_data\n");
                                          //status = gbr_object_tile_import_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_prop_default: printf("gbm_obj_prop_default\n");
                                          //status = gbr_object_palettes_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_map_settings: printf("gbm_obj_map_settings\n");
                                          //status = gbr_object_tile_pal_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_prop_colors: printf("gbm_obj_prop_colors\n");
                                          //status = gbr_object_tile_pal_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_map_export: printf("gbm_obj_map_export\n");
                                          //status = gbr_object_tile_pal_decode(&gbr, &obj);
                                          break;

                        case gbm_obj_deleted: printf("gbm_deleted\n");
                                          break;
                    }
                } // end: while (gbr_read_object_from_file(&obj, p_file))
            } // end: if gbr_write_version
        } // end: if gbr_write_header_key
    } // end: if ((p_file != NULL) && (obj.p_data))

    if (p_file)
        fclose(p_file);

    if (obj.p_data)
        free(obj.p_data);

    return status;
}



// Save to a .GBR file
//
int32_t gbm_save_file(const int8_t * filename) {


    FILE               * p_file;
    pascal_file_object   obj;
    int32_t              status;
/*
    obj.p_data = malloc(PASCAL_OBJECT_MAX_SIZE);

    status = true;

    // open the file
    p_file = fopen(filename, "wb");

    if ((p_file != NULL) && (obj.p_data)) {

        if (gbm_write_header_key(p_file)) {

            printf("Wrote Header\n");

            if (gbm_write_version(p_file)) {

                status = gbr_object_producer_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_data_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_settings_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_export_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_import_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_palettes_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_pal_encode(&gbr, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

            } // end: if gbr_write_version
        } // end: if gbr_write_header_key
    } // end: if ((p_file != NULL) && (obj.p_data))

    if (p_file)
        fclose(p_file);

    if (obj.p_data)
        free(obj.p_data);
*/
    return status;
}



void gbm_free_resources(void) {

    if (image.p_img_data)
        free(image.p_img_data);
};
