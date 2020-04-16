//
// lib_gbm_export.c
//

#include "lib_gbm.h"
#include "lib_gbm_export.h"

#include "lib_gbm_file_utils.h"
#include "lib_gbm_ops.h"

#include "options.h"



int32_t gbm_object_producer_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_producer;
    p_obj->object_id = 0; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 0;

    gbm_write_str(p_gbm->producer.name,    p_obj, GBM_PRODUCER_NAME_SIZE);
    gbm_write_str(p_gbm->producer.version, p_obj, GBM_PRODUCER_VERSION_SIZE);
    gbm_write_str(p_gbm->producer.info,    p_obj, GBM_PRODUCER_INFO_SIZE);

    printf("WROTE gbm_object_producer_encode:\n%s\n%s\n%s\n", p_gbm->producer.name,
                                                        p_gbm->producer.version,
                                                        p_gbm->producer.info);

    return true;
}


int32_t gbm_object_map_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map;
    p_obj->object_id = 1; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 0;

    gbm_write_str(    p_gbm->map.name,       p_obj, GBM_MAP_NAME_SIZE);
    gbm_write_uint32(&p_gbm->map.width,      p_obj);
    gbm_write_uint32(&p_gbm->map.height,     p_obj);
    gbm_write_uint32(&p_gbm->map.prop_count, p_obj);
    gbm_write_str(    p_gbm->map.tile_file,  p_obj, GBM_MAP_TILE_FILE_SIZE);
    gbm_write_uint32(&p_gbm->map.tile_count, p_obj);
    gbm_write_uint32(&p_gbm->map.prop_color_count, p_obj);

    printf("WROTE gbm_object_map_encode:\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n",
                                 p_gbm->map.name,
                                 p_gbm->map.width,
                                 p_gbm->map.height,
                                 p_gbm->map.prop_count,
                                 p_gbm->map.tile_file,
                                 p_gbm->map.tile_count,
                                 p_gbm->map.prop_count);

    return true;

}


int32_t gbm_object_map_deleted_1_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_deleted;
    p_obj->object_id = 2 ; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    gbm_write_buf(&(p_gbm->map_export_prop.props[0]), p_obj, GBM_MAP_EXPORT_DELETED_1_SIZE);
    return true;
}


int32_t gbm_object_map_deleted_2_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_deleted;
    p_obj->object_id = 9 ; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 8;

    gbm_write_buf(&(p_gbm->map_export_prop.props[0]), p_obj, GBM_MAP_EXPORT_DELETED_2_SIZE);
    return true;
}


int32_t gbm_object_map_prop_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map_prop;
    p_obj->object_id = 3; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    // TODO: This is a shim, since the map_prop record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_prop_data_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_prop_data;
    p_obj->object_id = 4; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    // TODO: This is a shim, since the map_prop record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_prop_default_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_prop_default;
    p_obj->object_id = 5; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    // TODO: This is a shim, since the map_prop record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_map_settings_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map_settings;
    p_obj->object_id = 6; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    gbm_write_uint32(&p_gbm->map_settings.form_width,  p_obj);
    gbm_write_uint32(&p_gbm->map_settings.form_height, p_obj);
    gbm_write_bool  (&p_gbm->map_settings.form_maximized, p_obj);

    gbm_write_bool (&p_gbm->map_settings.info_panel, p_obj);
    gbm_write_bool (&p_gbm->map_settings.grid, p_obj);
    gbm_write_bool (&p_gbm->map_settings.double_markers, p_obj);
    gbm_write_bool (&p_gbm->map_settings.prop_colors, p_obj);

    gbm_write_uint16(&p_gbm->map_settings.zoom, p_obj);
    gbm_write_uint16(&p_gbm->map_settings.color_set, p_obj);

    gbm_write_uint16(&(p_gbm->map_settings.bookmarks[0]), p_obj);
    gbm_write_uint16(&(p_gbm->map_settings.bookmarks[1]), p_obj);
    gbm_write_uint16(&(p_gbm->map_settings.bookmarks[2]), p_obj);

    gbm_write_uint32(&p_gbm->map_settings.block_fill_pattern, p_obj);
    gbm_write_uint32(&p_gbm->map_settings.block_fill_width, p_obj);
    gbm_write_uint32(&p_gbm->map_settings.block_fill_height, p_obj);

    gbm_write_bool  (&p_gbm->map_settings.auto_update, p_obj);

    printf("WROTE gbm_object_map_settings_encode:\n%d\n%d\n%d \n%d\n%d\n%d\n%d\n %d\n%d\n %d\n%d\n%d\n",
                                 p_gbm->map_settings.form_width,
                                 p_gbm->map_settings.form_height,
                                 p_gbm->map_settings.form_maximized,

                                 p_gbm->map_settings.info_panel,
                                 p_gbm->map_settings.grid,
                                 p_gbm->map_settings.double_markers,
                                 p_gbm->map_settings.prop_colors,

                                 p_gbm->map_settings.zoom,
                                 p_gbm->map_settings.color_set,

                                 p_gbm->map_settings.bookmarks[0],
                                 p_gbm->map_settings.bookmarks[1],
                                 p_gbm->map_settings.bookmarks[2]);
    return true;
}


int32_t gbm_object_prop_colors_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_prop_colors;
    p_obj->object_id = 7; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    // Map Tile Data is an array of uint24, just read them as 3 x uint8
    // The tile records take all of the data in the object
    gbm_write_buf(&(p_gbm->map_prop_colors.colors[0]), p_obj, GBM_MAP_PROP_COLORS_COLORS_SIZE);

    printf("WROTE gbm_object_prop_colors_encode:%d\n",GBM_MAP_PROP_COLORS_COLORS_SIZE);

    return true;
}


int32_t gbm_object_map_export_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map_export;
    p_obj->object_id = 8 ; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    gbm_write_str(    p_gbm->map_export.file_name,   p_obj, GBM_MAP_EXPORT_FILE_NAME_SIZE);
    gbm_write_uint8 (&p_gbm->map_export.file_type,   p_obj);
    gbm_write_str(    p_gbm->map_export.section_name,p_obj, GBM_MAP_EXPORT_SECTION_NAME_SIZE);
    gbm_write_str(    p_gbm->map_export.label_name,  p_obj, GBM_MAP_EXPORT_LABEL_NAME_SIZE);
    gbm_write_uint8 (&p_gbm->map_export.bank,        p_obj);
    gbm_write_uint16(&p_gbm->map_export.plane_count, p_obj);
    gbm_write_uint16(&p_gbm->map_export.plane_order, p_obj);
    gbm_write_uint16(&p_gbm->map_export.map_layout,  p_obj);
    gbm_write_bool  (&p_gbm->map_export.split,       p_obj);
    gbm_write_uint32(&p_gbm->map_export.split_size,  p_obj);
    gbm_write_bool  (&p_gbm->map_export.split_bank,  p_obj);
    gbm_write_uint8 (&p_gbm->map_export.sel_tab,     p_obj);
    gbm_write_uint16(&p_gbm->map_export.prop_count,  p_obj);
    gbm_write_uint16(&p_gbm->map_export.tile_offset, p_obj);

    printf("WROTE gbm_object_map_export_encode:\n%s\n%d\n%s\n%s\n %d\n%d\n%d\n",
                                 p_gbm->map_export.file_name,
                                 p_gbm->map_export.file_type,
                                 p_gbm->map_export.section_name,
                                 p_gbm->map_export.label_name,

                                 p_gbm->map_export.bank,
                                 p_gbm->map_export.plane_count,
                                 p_gbm->map_export.plane_order);
    return true;
}



int32_t gbm_object_map_export_prop_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map_export_prop;
    p_obj->object_id = 9; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 8;


    // Set length of real data bytes in record (might be different than size in file due to junk padding)
    // TODO: there might be a min size here of 160 bytes (20 records x 8 bytes)
    p_gbm->map_export_prop.length_bytes = p_gbm->map_export.prop_count * GBM_MAP_EXPORT_PROPS_REC_SIZE;

    // Length of real data can't be longer than the record itself
    if (p_gbm->map_export_prop.length_bytes > GBM_MAP_EXPORT_PROPS_SIZE_MAX)
        return false;

    // Write the map export prop settings (location format) buffer
    // Always write full size?
    // gbm_write_buf(&(p_gbm->map_export_prop.props[0]), p_obj, GBM_MAP_EXPORT_PROPS_SIZE_MAX);
    gbm_write_buf(&(p_gbm->map_export_prop.props[0]), p_obj, p_gbm->map_export_prop.length_bytes);

    printf("WROTE gbm_object_map_export_prop_encode: length_bytes=%d\n", p_gbm->map_export_prop.length_bytes);
    return true;
}



int32_t gbm_object_tile_data_encode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    p_obj->length_bytes = 0;
    p_obj->offset    = 0;
    p_obj->id        = gbm_obj_map_tile_data;
    p_obj->object_id = 2; // TODO: this should probably increment on write instead of being hardwired
    p_obj->master_id = 1;

    // Map Tile Data is an array of uint24, just read them as 3 x uint8
    // The tile records take all of the data in the object
    gbm_write_buf( p_gbm->map_tile_data.records, p_obj, p_gbm->map_tile_data.length_bytes);

    // TODO: what is the trailing data from 2160 -> 4800 bytes?
    gbm_write_padding(p_obj, GBM_MAP_TILE_DATA_PADDING_UNKNOWN);

    printf("----- ENCODE MAP TILES %d -----\n", p_gbm->map_tile_data.length_bytes + GBM_MAP_TILE_DATA_PADDING_UNKNOWN);
    gbm_map_tiles_print(p_gbm);
    printf("----- ENCODE MAP TILES : FLIP X/Y %d -----\n", p_gbm->map_tile_data.length_bytes + GBM_MAP_TILE_DATA_PADDING_UNKNOWN);
    gbm_map_tiles_flip_print(p_gbm);
    printf("----- ENCODE MAP TILES : MAP PAL OVERRIDES (0=default) %d -----\n", p_gbm->map_tile_data.length_bytes + GBM_MAP_TILE_DATA_PADDING_UNKNOWN);
    gbm_map_tiles_pal_print(p_gbm);

    return true;
}







int32_t gbm_export_set_defaults(gbm_record * p_gbm) {


    // PRODUCER
    // default string values is filled with trailing zeros
    memset(p_gbm->producer.name,    0x00, GBM_PRODUCER_NAME_SIZE);
    memset(p_gbm->producer.version, 0x00, GBM_PRODUCER_VERSION_SIZE);
    memset(p_gbm->producer.info,    0x00, GBM_PRODUCER_INFO_SIZE);

    snprintf(p_gbm->producer.name,     GBM_PRODUCER_NAME_SIZE,    "Gameboy Map Builder");
    snprintf(p_gbm->producer.version,  GBM_PRODUCER_VERSION_SIZE, "1.8");
    snprintf(p_gbm->producer.info,     GBM_PRODUCER_INFO_SIZE,    "Home: http://www.casema.net/~hpmulder");


    // MAP
    memset(p_gbm->map.name,      0x00, GBM_MAP_NAME_SIZE);
    memset(p_gbm->map.tile_file, 0x00, GBM_MAP_TILE_FILE_SIZE);

    //snprintf(p_gbm->producer.name,      GBM_MAP_NAME_SIZE,      "");
    p_gbm->map.width      = 0;
    p_gbm->map.height     = 0;
    p_gbm->map.prop_count = 0;
    snprintf(p_gbm->map.tile_file, GBM_MAP_TILE_FILE_SIZE, "tiles.gbr");
    p_gbm->map.tile_count = 0;
    p_gbm->map.prop_color_count = 2;


    // MAP TILE DATA
    p_gbm->map_tile_data.length_bytes = 0;
    memset(p_gbm->map_tile_data.records, 0x00, GBM_MAP_TILE_DATA_RECORDS_SIZE);


    // MAP PROP
    memset(p_gbm->map_prop.name, 0x00, GBM_MAP_PROP_NAME_SIZE);

    p_gbm->map_prop.p_type = 0;
    p_gbm->map_prop.size   = 0;
    //snprintf(p_gbm->producer.name, GBM_MAP_PROP_NAME_SIZE, "");


    // MAP PROP DATA
    memset(p_gbm->map_prop_data.data, 0x00, GBM_OBJECT_MAX_SIZE);


    // MAP PROP DEFAULT
    memset(p_gbm->map_prop_default.data, 0x00, GBM_OBJECT_MAX_SIZE);


    // MAP SETTINGS
    p_gbm->map_settings.form_width     = 795;
    p_gbm->map_settings.form_height    = 498;
    p_gbm->map_settings.form_maximized = 0; // bool

    p_gbm->map_settings.info_panel     = 1; // bool
    p_gbm->map_settings.grid           = 1; // bool
    p_gbm->map_settings.double_markers = 0; // bool
    p_gbm->map_settings.prop_colors    = 0; // bool

    p_gbm->map_settings.zoom           = 2;
    p_gbm->map_settings.color_set      = 1;
    memset(p_gbm->map_settings.bookmarks, 0xFF, sizeof(p_gbm->map_settings.bookmarks));

    p_gbm->map_settings.block_fill_pattern = 0;
    p_gbm->map_settings.block_fill_width   = 1;
    p_gbm->map_settings.block_fill_height  = 1;

    p_gbm->map_settings.auto_update        = 0;


    // MAP PROP COLORS
    memset(p_gbm->map_prop_colors.colors, 0x00, GBM_MAP_PROP_COLORS_COLORS_SIZE);


    // MAP EXPORT
    memset(p_gbm->map_export.file_name,     0x00, GBM_MAP_EXPORT_FILE_NAME_SIZE);
    memset(p_gbm->map_export.section_name,  0x00, GBM_MAP_EXPORT_SECTION_NAME_SIZE);
    memset(p_gbm->map_export.label_name,    0x00, GBM_MAP_EXPORT_LABEL_NAME_SIZE);

    snprintf(p_gbm->map_export.file_name,      GBM_MAP_EXPORT_FILE_NAME_SIZE, "map.c");
    p_gbm->map_export.file_type = 3; // TODO: make a #define for this?
    // snprintf(p_gbm->map_export.section_name,      GBM_MAP_EXPORT_SECTION_NAME_SIZE, ""); // Section name is blank
    snprintf(p_gbm->map_export.label_name,      GBM_MAP_EXPORT_LABEL_NAME_SIZE, "map%ce1_1", '\0');
    p_gbm->map_export.bank      = 0; // TODO: make a #define for this? Different default bank?

    p_gbm->map_export.plane_count = 1;
    p_gbm->map_export.plane_order = 0;
    p_gbm->map_export.map_layout  = 0;

    p_gbm->map_export.split      = 0; // bool
    p_gbm->map_export.split_size = 0;
    p_gbm->map_export.split_bank = 0; // bool

    p_gbm->map_export.sel_tab    = 0;
    p_gbm->map_export.prop_count = 1;
    p_gbm->map_export.tile_offset = 0;


    // MAP PROP COLORS
    memset(p_gbm->map_export_prop.props, 0x00, GBM_MAP_EXPORT_PROPS_SIZE_MAX);

    return true;
}

void gbm_export_update_color_set(gbm_record * p_gbm, uint16_t gb_mode) {

    // Handle CGB/DMG color mode setting (ignore SGB/GB Pocket for now)
    if (gb_mode == MODE_CGB_32_COLOR)
        p_gbm->map_settings.color_set = gbm_color_set_gbc;
    else
        p_gbm->map_settings.color_set = gbm_color_set_game_boy;
}