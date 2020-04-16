//
// lib_gbm.c
//

#include "lib_gbm.h"
#include "lib_gbm.h"
#include "lib_gbm_import.h"
#include "lib_gbm_export.h"
#include "lib_gbm_file_utils.h"
#include "lib_gbm_ops.h"

#include "lib_gbr.h"
#include "lib_gbr_export.h"

#include "lib_tilemap.h"
#include "tilemap_path_ops.h"
#include "image_info.h"

static image_data image;
static color_data colors;

static gbm_record gbm;

static gbm_map_export      settings_gbm_map_export;
static uint32_t            settings_gbm_map_export_populated = false;

static gbm_map_export_prop settings_gbm_map_export_prop;
static uint32_t            settings_gbm_map_export_prop_populated = false;



int32_t gbm_load(const char * filename) {

    int32_t status;

    // image_data * p_tile_image;
    color_data * p_tile_colors;
    gbr_record * p_gbr;

    char path_without_filename[STR_FILENAME_MAX];
    char gbr_path[STR_FILENAME_MAX];

    if (!get_path_without_filename(filename, path_without_filename, STR_FILENAME_MAX))
      return (false);

    printf("**gbm_load\n");

    // Load and parse the file
    status = gbm_load_file(filename);

    // If the map file loaded then try to load
    // the tile file (.GBR) that it references
    if(status) {
        snprintf(gbr_path, STR_FILENAME_MAX, "%s%s",  path_without_filename, gbm.map.tile_file);
        printf("calling gbr load:%s:\n", gbr_path);
        status = gbr_load(gbr_path);
    }
    else
        printf("Load GBM failed\n");

    if (status) {
        p_gbr = gbr_get_ptr();
        // p_tile_image = gbr_get_image(); //TODO: maybe don't need?... see below: gbm_convert_map_to_image(&gbm, p_gbr, &image)
        p_tile_colors = gbr_get_colors();

        // copy colors if possible, otherwise signal failure
        if (p_tile_colors)
            memcpy(&colors, p_tile_colors, sizeof(color_data));
        else
            status = false;

        // Render the image from the loaded data
        // TODO: The use of p_gbr here is crossing some boundaries that maybe shouldn't be
        // Consider just passing the tiles converted into an image
        if (status)
            status = gbm_convert_map_to_image(&gbm, p_gbr, &image);
   }

    // TODO: Open the loaded tiles as a separate image?


    return status;
};



// TODO: ADD gbr_set_image for SOURCE image?
int32_t gbm_save(const char * filename, image_data * p_src_image, color_data * p_colors, tile_process_options plugin_options) {

    int32_t status;
    tile_map_data * p_map;
    tile_set_data * p_tile_set;
    image_data      tile_set_deduped_image;

    tile_set_deduped_image.p_img_data = NULL;
    // gbr_record      gbr;

    printf("gbm_save(): %d x %d with mode = %d, dedupe flip = %d, dedupe pal = %d, \n",
            p_src_image->width, p_src_image->height,
            plugin_options.gb_mode, plugin_options.tile_dedupe_flips, plugin_options.tile_dedupe_palettes);

    status = tilemap_export_process(p_src_image);
    printf("(gbm) tilemap_export_process: status= %d\n", status);


    if (status) {

        // == IMAGE -> MAP + TILE SET CONVERSION ==

        // Retrieve the deduplicated map and tile set
        p_map      = tilemap_get_map();
        p_tile_set = tilemap_get_tile_set();
        status     = tilemap_get_image_of_deduped_tile_set(&tile_set_deduped_image);
        printf("(gbm) tilemap_get_image_of_deduped_tile_set: status= %d\n", status);


        if (p_map && p_tile_set && status) {

            // == EXPORT TILE SET AS GBR ==
            char gbr_path[STR_FILENAME_MAX];
            snprintf(gbr_path, STR_FILENAME_MAX, "%s%s",  filename, ".tiles.gbr");

            printf("calling gbr save:%s:\n", gbr_path);
            status = gbr_save(gbr_path, &tile_set_deduped_image, p_colors, plugin_options);
            printf("(gbm) gbr_save_file: status= %d\n", status);

            if (status) {
                // == EXPORT MAP AS GBM ==

                // Initialize shared GBM map structure with defaults
                gbm_export_set_defaults(&gbm);
                gbm_export_update_color_set(&gbm, plugin_options.gb_mode);
                gbm_overlay_cached_settings();

                // Set GBR tile file name for the GBM file to use (exported above)
                snprintf(gbm.map.tile_file, GBM_MAP_TILE_FILE_SIZE, "%s", get_filename_from_path(gbr_path));

                // Set tile map parameters, then convert the image to a map
                gbm.map.width      = p_map->width_in_tiles;
                gbm.map.height     = p_map->height_in_tiles;
                gbm.map.tile_count = p_tile_set->tile_count;
                gbm.map_tile_data.length_bytes = gbm.map.width * gbm.map.height * GBM_MAP_TILE_RECORD_SIZE;

                if (gbm.map_tile_data.length_bytes > GBM_MAP_TILE_DATA_RECORDS_SIZE)
                    status = false;

                status = gbm_convert_tilemap_buf_to_map(&gbm,
                                                        p_map->tile_id_list,
                                                        p_map->flip_bits_list,
                                                        p_map->palette_num_list,
                                                        p_map->size);
                printf("(gbm) gbm_convert_image_to_map: status= %d\n", status);

                printf("gbm_save_file\n");
                if (status) {
                    status = gbm_save_file(filename);

                    printf("(gbm) gbm_save_file: status= %d\n", status);
                }
            }

        } else
            status = false;
    }

    printf("(gbm) gbm_save_file END: status= %d\n", status);

    // Free temp composite image of tiles
    if (tile_set_deduped_image.p_img_data)
        free(tile_set_deduped_image.p_img_data);

    return status;
}



// Load and parse a .GBM file
//
int32_t gbm_load_file(const char * filename) {

    FILE               * p_file;
    gbm_file_object      obj;
    int32_t              status;

    obj.p_data = malloc(GBM_OBJECT_MAX_SIZE);

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
                                          status = gbm_object_producer_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map: printf("gbm_obj_map\n");
                                          status = gbm_object_map_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map_tile_data: printf("gbm_obj_map_tile_data\n");
                                          status = gbm_object_map_tile_data_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map_prop: printf("gbm_obj_map_prop\n");
                                          status = gbm_object_map_prop_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_prop_data: printf("gbm_obj_prop_data\n");
                                          status = gbm_object_prop_data_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_prop_default: printf("gbm_obj_prop_default\n");
                                          status = gbm_object_prop_default_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map_settings: printf("gbm_obj_map_settings\n");
                                          status = gbm_object_map_settings_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_prop_colors: printf("gbm_obj_prop_colors\n");
                                          status = gbm_object_prop_colors_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map_export: printf("gbm_obj_map_export\n");
                                          status = gbm_object_map_export_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_map_export_prop: printf("gbm_obj_map_export_prop\n");
                                          status = gbm_object_map_export_prop_decode(&gbm, &obj);
                                          break;

                        case gbm_obj_deleted: printf("gbm_deleted\n");
                                          break;
                    }
                } // end: while (gbm_read_object_from_file(&obj, p_file))
            } // end: if gbm_write_version
        } // end: if gbm_write_header_key
    } // end: if ((p_file != NULL) && (obj.p_data))

    if (p_file)
        fclose(p_file);

    if (obj.p_data)
        free(obj.p_data);

    return status;
}



// Save to a .gbm file
//
int32_t gbm_save_file(const char * filename) {


    FILE            * p_file;
    gbm_file_object   obj;
    int32_t           status;

    obj.p_data = malloc(GBM_OBJECT_MAX_SIZE);

    status = true;

    // open the file
    p_file = fopen(filename, "wb");

    if ((p_file != NULL) && (obj.p_data)) {

        if (gbm_write_header_key(p_file)) {

            printf("Wrote Header\n");

            if (gbm_write_version(p_file)) {

                status = gbm_object_producer_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_deleted_1_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_prop_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_prop_data_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_prop_default_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_settings_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_prop_colors_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_export_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_deleted_2_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_tile_data_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

                if (status) status = gbm_object_map_export_prop_encode(&gbm, &obj);
                    if (status) status = gbm_write_object_to_file(&obj, p_file);

            } // end: if gbm_write_version
        } // end: if gbm_write_header_key
    } // end: if ((p_file != NULL) && (obj.p_data))

    if (p_file)
        fclose(p_file);

    if (obj.p_data)
        free(obj.p_data);

    return status;
}



image_data * gbm_get_image(void) {

    printf("gbm_get_image\n");
    return &image;
}


color_data * gbm_get_colors(void) {

    printf("gbm_get_colors\n");
    return &colors;
}


// TODO: maybe remove globals for these and just use reference passing to import/export?
// TODO: these are deprecated
void gbm_set_image(image_data * p_src_image) {

 // TODO: implement
    printf("gbm_set_image\n");
//    memcpy(&image, p_src_image, sizeof(image_data));
}


void gbm_set_colors(color_data * p_src_colors) {

 // TODO: implement
    printf("gbm_set_colors\n");
//    memcpy(&colors, p_src_colors, sizeof(color_data));
}



void gbm_free_resources(void) {

    if (image.p_img_data)
        free(image.p_img_data);

    // Free any resources of the tile file used when loading the map
    gbr_free_resources();

    tilemap_free_resources();
};



uint32_t gbm_get_map_export_rec_size(void) {
    return( (uint32_t) sizeof(gbm.map_export) );
}

uint8_t * gbm_get_map_export_rec_buffer(void) {
    return ( (uint8_t *) &(gbm.map_export));
}


void gbm_set_map_export_from_buffer(uint32_t buffer_size, uint8_t * p_src_buf) {

    // Only copy structure if size matches
    if (buffer_size == (uint32_t) sizeof(gbm.map_export)) {

        printf("gbm_set_export_from_buffer(): loading...\n");

        settings_gbm_map_export_populated = true;
        memcpy(&settings_gbm_map_export, p_src_buf, buffer_size);
    } else {

        printf("gbm_set_export_from_buffer(): buffer size mismatch. don't load\n");
    }

}

// TODO: consolidate these into generalized accessor functions

uint32_t gbm_get_map_export_prop_rec_size(void) {
    return( (uint32_t) sizeof(gbm.map_export_prop) );
}

uint8_t * gbm_get_map_export_prop_rec_buffer(void) {
    return ( (uint8_t *) &(gbm.map_export_prop));
}


void gbm_set_map_export_prop_from_buffer(uint32_t buffer_size, uint8_t * p_src_buf) {

    // Only copy structure if size matches
    if (buffer_size == (uint32_t) sizeof(gbm.map_export_prop)) {

        printf("gbm_set_map_export_prop_from_buffer(): loading...\n");

        settings_gbm_map_export_prop_populated = true;
        memcpy(&settings_gbm_map_export_prop, p_src_buf, buffer_size);
    } else {

        printf("gbm_set_map_export_prop_from_buffer(): buffer size mismatch. don't load\n");
    }

}


void gbm_overlay_cached_settings(void) {
    // Overlay any cached export settings (from gimp parasite metadata)
    if (settings_gbm_map_export_populated) {
        memcpy(&(gbm.map_export), &settings_gbm_map_export, sizeof(gbm_map_export));
    }


    // Overlay any cached export settings (from gimp parasite metadata)
    if (settings_gbm_map_export_prop_populated) {
        memcpy(&(gbm.map_export_prop), &settings_gbm_map_export_prop, sizeof(gbm_map_export_prop));
    }
}