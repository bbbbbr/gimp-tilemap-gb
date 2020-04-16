//
// lib_gbr.c
//

#include "lib_gbr.h"
#include "lib_gbr_import.h"
#include "lib_gbr_export.h"
#include "lib_gbr_file_utils.h"

static image_data image;
static color_data colors;

static gbr_record gbr;

static gbr_tile_export settings_gbr_tile_export;
static uint32_t        settings_gbr_tile_export_populated = false;


int32_t gbr_load(const char * filename) {

    int32_t status;

    // Load and parse the file
    status = gbr_load_file(filename);

    if (status) {
        // Render the image from the loaded data
        status = gbr_convert_tileset_to_image(&gbr, &image, &colors);
    }

    // TODO: Store imported GBR structure as metadata in GimpParasite? (then write out on export )

    return status;
};



// TODO: Import palette color data?
// TODO: Try to maintain palette color order? Store in Gimp Parasite?
// TODO: ADD gbr_set_image for SOURCE image?
int32_t gbr_save(const char * filename, image_data * p_src_image, color_data * p_colors, tile_process_options plugin_options) {

    int32_t status;


    // Initialize shared GBR structure with defaults
    gbr_export_set_defaults(&gbr);

    // Overlay any cached export settings
    if (settings_gbr_tile_export_populated) {
        printf("gbr_save(): Overlay any cached export settings - found\n");
        memcpy(&(gbr.tile_export), &settings_gbr_tile_export, sizeof(gbr_tile_export));
    }


    // TODO: check
    // TODO: MIN 16 tiles required in file?
    // Convert the image data to tiles
    status = gbr_convert_image_to_tileset(&gbr, p_src_image, p_colors, plugin_options.gb_mode);

    // Load and parse the file
    if (status)
        status = gbr_save_file(filename);

    return status;
};



gbr_record * gbr_get_ptr(void) {
    return &(gbr);
}



image_data * gbr_get_image(void) {

    printf("gbr_get_image\n");
    return &image;
}


color_data * gbr_get_colors(void) {

    printf("gbr_get_colors\n");
    return &colors;
}


// TODO: maybe remove globals for these and just use reference passing to import/export?
// TODO: these are deprecated
void gbr_set_image(image_data * p_src_image) {

    printf("gbr_set_image\n");
    memcpy(&image, p_src_image, sizeof(image_data));
}


void gbr_set_colors(color_data * p_src_colors) {

    printf("gbr_set_colors\n");
    memcpy(&colors, p_src_colors, sizeof(color_data));
}


uint32_t gbr_get_export_rec_size(void) {
    return( (uint32_t) sizeof(gbr_tile_export) );
}

uint8_t * gbr_get_export_rec_buffer(void) {
    return ( (uint8_t *) &(gbr.tile_export));
}


void gbr_set_export_from_buffer(uint32_t buffer_size, uint8_t * p_src_buf) {

    // Only copy structure if size matches
    if (buffer_size == (uint32_t) sizeof(gbr_tile_export)) {

        printf("gbr_set_export_from_buffer(): loading...\n");

        settings_gbr_tile_export_populated = true;
        memcpy(&settings_gbr_tile_export, p_src_buf, buffer_size);
    } else {

        printf("gbr_set_export_from_buffer(): buffer size mismatch. don't load\n");
    }

}


// Load and parse a .GBR file
//
int32_t gbr_load_file(const char * filename) {

    FILE               * p_file;
    gbr_file_object   obj;
    int32_t              status;

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
int32_t gbr_save_file(const char * filename) {

    FILE               * p_file;
    gbr_file_object   obj;
    int32_t              status;

    obj.p_data = malloc(PASCAL_OBJECT_MAX_SIZE);

    status = true;

    // open the file
    p_file = fopen(filename, "wb");

    if ((p_file != NULL) && (obj.p_data)) {

        if (gbr_write_header_key(p_file)) {

            printf("Wrote Header\n");

            if (gbr_write_version(p_file)) {

                status = gbr_object_producer_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_data_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_settings_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_export_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_import_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_palettes_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

                if (status) status = gbr_object_tile_pal_encode(&gbr, &obj);
                    if (status) status = gbr_write_object_to_file(&obj, p_file);

            } // end: if gbr_write_version
        } // end: if gbr_write_header_key
    } // end: if ((p_file != NULL) && (obj.p_data))

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
