//
// lib_gbm_import.c
//

#include "lib_gbm.h"
#include "lib_gbm_import.h"

#include "lib_gbm_file_utils.h"
#include "lib_gbm_ops.h"



// TODO: what is the trailing data from 2160 -> 4800 bytes?
/*

    p_gbm->map_tile_data

// TODO: Palette is encoded?
          k := (TileMap.Data[(i*TileMap.Width)+j].Tile and $03FF);
          k := k + (TileMap.Data[(i*TileMap.Width)+j].Flags shl (22-4));

          k := k + ((TileMap.Data[(i*TileMap.Width)+j].Palette and $0F) shl 10);
          k := k + ((TileMap.Data[(i*TileMap.Width)+j].Palette and $0700) shl (16-8));
*/




int32_t gbm_object_producer_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes != GBM_PRODUCER_SIZE)
        return false;

    gbm_read_str(p_gbm->producer.name,    p_obj, GBM_PRODUCER_NAME_SIZE);
    gbm_read_str(p_gbm->producer.version, p_obj, GBM_PRODUCER_VERSION_SIZE);
    gbm_read_str(p_gbm->producer.info,    p_obj, GBM_PRODUCER_INFO_SIZE);

    printf("gbm_object_producer_decode:\n%s\n%s\n%s\n", p_gbm->producer.name,
                                      p_gbm->producer.version,
                                      p_gbm->producer.info);

    return true;
}



int32_t gbm_object_map_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes != GBM_MAP_SIZE)
        return false;

    gbm_read_str(    p_gbm->map.name,       p_obj, GBM_MAP_NAME_SIZE);
    gbm_read_uint32(&p_gbm->map.width,      p_obj);
    gbm_read_uint32(&p_gbm->map.height,     p_obj);
    gbm_read_uint32(&p_gbm->map.prop_count, p_obj);
    gbm_read_str(    p_gbm->map.tile_file,  p_obj, GBM_MAP_TILE_FILE_SIZE);
    gbm_read_uint32(&p_gbm->map.tile_count, p_obj);
    gbm_read_uint32(&p_gbm->map.prop_color_count, p_obj);


printf("gbm_object_map_decode:\n%s\n%d\n%d\n%d\n%s\n%d\n%d\n",
                                 p_gbm->map.name,
                                 p_gbm->map.width,
                                 p_gbm->map.height,
                                 p_gbm->map.prop_count,
                                 p_gbm->map.tile_file,
                                 p_gbm->map.tile_count,
                                 p_gbm->map.prop_count);

    return true;
}



int32_t gbm_object_map_tile_data_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes > GBM_MAP_TILE_DATA_RECORDS_SIZE)
        return false;

    // Map Tile Data is an array of uint24, just read them as 3 x uint8
    // The tile records take all of the data in the object
    gbm_read_buf( p_gbm->map_tile_data.records, p_obj, p_obj->length_bytes);
    p_gbm->map_tile_data.length_bytes = p_obj->length_bytes;

    printf("----- DECODE MAP TILES -----\n");
    gbm_map_tiles_print(p_gbm);

    return true;
}



int32_t gbm_object_map_prop_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    // TODO: This is a shim, since the map_prop record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_prop_data_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    // TODO: This is a shim, since the map_prop_data record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_prop_default_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    // TODO: This is a shim, since the map_prop_default record doesn't seem to be populated (zero length)
    return true;
}


int32_t gbm_object_map_settings_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes < GBM_MAP_SETTINGS_MIN_SIZE)
        return false;

    gbm_read_uint32(&p_gbm->map_settings.form_width,  p_obj);
    gbm_read_uint32(&p_gbm->map_settings.form_height, p_obj);
    gbm_read_bool  (&p_gbm->map_settings.form_maximized, p_obj);

    gbm_read_bool (&p_gbm->map_settings.info_panel, p_obj);
    gbm_read_bool (&p_gbm->map_settings.grid, p_obj);
    gbm_read_bool (&p_gbm->map_settings.double_markers, p_obj);
    gbm_read_bool (&p_gbm->map_settings.prop_colors, p_obj);

    gbm_read_uint16(&p_gbm->map_settings.zoom, p_obj);
    gbm_read_uint16(&p_gbm->map_settings.color_set, p_obj);

    gbm_read_uint16(&(p_gbm->map_settings.bookmarks[0]), p_obj);
    gbm_read_uint16(&(p_gbm->map_settings.bookmarks[1]), p_obj);
    gbm_read_uint16(&(p_gbm->map_settings.bookmarks[2]), p_obj);

    gbm_read_uint32(&p_gbm->map_settings.block_fill_pattern, p_obj);
    gbm_read_uint32(&p_gbm->map_settings.block_fill_width, p_obj);
    gbm_read_uint32(&p_gbm->map_settings.block_fill_height, p_obj);

    gbm_read_bool  (&p_gbm->map_settings.auto_update, p_obj);

printf("gbm_object_map_settings_decode:\n%d\n%d\n%d \n%d\n%d\n%d\n%d\n %d\n%d\n %d\n%d\n%d\n",
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




int32_t gbm_object_prop_colors_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes != GBM_MAP_PROP_COLORS_COLORS_SIZE)
        return false;

    // Map Tile Data is an array of uint24, just read them as 3 x uint8
    // The tile records take all of the data in the object
    gbm_read_buf(&(p_gbm->map_prop_colors.colors[0]), p_obj, p_obj->length_bytes);
    return true;
}


int32_t gbm_object_map_export_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {

    if (p_obj->length_bytes != GBM_MAP_EXPORT_SIZE)
        return false;

    gbm_read_str(    p_gbm->map_export.file_name,   p_obj, GBM_MAP_EXPORT_FILE_NAME_SIZE);
    gbm_read_uint8 (&p_gbm->map_export.file_type,   p_obj);
    gbm_read_str(    p_gbm->map_export.section_name,p_obj, GBM_MAP_EXPORT_SECTION_NAME_SIZE);
    gbm_read_str(    p_gbm->map_export.label_name,  p_obj, GBM_MAP_EXPORT_LABEL_NAME_SIZE);
    gbm_read_uint8 (&p_gbm->map_export.bank,        p_obj);
    gbm_read_uint16(&p_gbm->map_export.plane_count, p_obj);
    gbm_read_uint16(&p_gbm->map_export.plane_order, p_obj);
    gbm_read_uint16(&p_gbm->map_export.map_layout,  p_obj);
    gbm_read_bool  (&p_gbm->map_export.split,       p_obj);
    gbm_read_uint32(&p_gbm->map_export.split_size,  p_obj);
    gbm_read_bool  (&p_gbm->map_export.split_bank,  p_obj);
    gbm_read_uint8 (&p_gbm->map_export.sel_tab,     p_obj);
    gbm_read_uint16(&p_gbm->map_export.prop_count,  p_obj);
    gbm_read_uint16(&p_gbm->map_export.tile_offset, p_obj);



printf("gbm_object_map_export_decode:\n%s\n%d\n%s\n%s\n %d\n%d\n%d\n",
                                 p_gbm->map_export.file_name,
                                 p_gbm->map_export.file_type,
                                 p_gbm->map_export.section_name,
                                 p_gbm->map_export.label_name,

                                 p_gbm->map_export.bank,
                                 p_gbm->map_export.plane_count,
                                 p_gbm->map_export.plane_order);
    return true;
}


int32_t gbm_object_map_export_prop_decode(gbm_record * p_gbm, gbm_file_object * p_obj) {


    if (p_obj->length_bytes > GBM_MAP_EXPORT_PROPS_SIZE_MAX)
        return false;

    // Set length of real data bytes in record (might be different than size in file due to junk padding)
    // TODO: there might be a min size here of 160 bytes (20 records x 8 bytes)
    p_gbm->map_export_prop.length_bytes = p_gbm->map_export.prop_count * GBM_MAP_EXPORT_PROPS_REC_SIZE;

    // Length of real data can't be longer than the record itself
    if (p_gbm->map_export_prop.length_bytes > p_obj->length_bytes)
        return false;

    // Read the whole buffer, even if the length of real data is shorter
    gbm_read_buf(&(p_gbm->map_export_prop.props[0]), p_obj, p_obj->length_bytes);

    printf("gbm_object_map_export_prop_decode: length_bytes=%d\n",p_gbm->map_export_prop.length_bytes);

    return true;
}


