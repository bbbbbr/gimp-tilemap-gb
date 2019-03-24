//
// lib_gbr_import.c
//

#include "lib_gbr_import.h"
#include "lib_gbr_file_utils.h"


int32_t gbr_object_producer_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_PRODUCER_SIZE)
        return false;

    gbr_read_str(p_gbr->producer.name,    p_obj, GBR_PRODUCER_NAME_SIZE);
    gbr_read_str(p_gbr->producer.version, p_obj, GBR_PRODUCER_VERSION_SIZE);
    gbr_read_str(p_gbr->producer.info,    p_obj, GBR_PRODUCER_INFO_SIZE);

    printf("PRODUCER:\n%s\n%s\n%s\n", p_gbr->producer.name,
                                      p_gbr->producer.version,
                                      p_gbr->producer.info);

    return true;
}



// Tile: x,y,id = .tile_list[ .height
//                            + (y * .width)
//                            + (id * width * height) ]
// TODO: change to int16_t? read about what is most efficient on 64 bit architectures
int32_t gbr_object_tile_data_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_DATA_SIZE_MIN)
        return false;

    gbr_read_str   ( p_gbr->tile_data.name,      p_obj, GBR_TILE_DATA_NAME_SIZE);
    gbr_read_uint16(&p_gbr->tile_data.width,     p_obj);
    gbr_read_uint16(&p_gbr->tile_data.height,    p_obj);
    gbr_read_uint16(&p_gbr->tile_data.count,     p_obj);

    // TODO : Use colorset against palette?
    gbr_read_buf   ( p_gbr->tile_data.color_set, p_obj, GBR_TILE_DATA_COLOLR_SET_SIZE);

    p_gbr->tile_data.data_size = p_gbr->tile_data.width * p_gbr->tile_data.height
                                                        * p_gbr->tile_data.count;
    // Read N Tiles of x Width x Height array of bytes, one byte per tile pixel
    gbr_read_buf   ( p_gbr->tile_data.tile_list, p_obj, p_gbr->tile_data.data_size);

/*
    int x;
    int y;
    for (y = 0; y < p_gbr->tile_data.height; y++) {
        for (x = 0; x < p_gbr->tile_data.width; x++) {
            printf("%4x", p_gbr->tile_data.tile_list[x + (y * p_gbr->tile_data.width)]);
        }
        printf("\n");
    }
*/




printf("TILE_DATA:\n%s\n %d\n %d\n %d\n %x\n %x\n %x\n %x\n%d\n",
                                 p_gbr->tile_data.name,
                                 p_gbr->tile_data.height,
                                 p_gbr->tile_data.width,
                                 p_gbr->tile_data.count,
                                 p_gbr->tile_data.color_set[0],
                                 p_gbr->tile_data.color_set[1],
                                 p_gbr->tile_data.color_set[2],
                                 p_gbr->tile_data.color_set[3],
                                 p_gbr->tile_data.data_size);

  return true;
}



int32_t gbr_object_tile_settings_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_SETTINGS_SIZE)
        return false;

    gbr_read_uint16(&p_gbr->tile_settings.tile_id,      p_obj);
    gbr_read_bool  (&p_gbr->tile_settings.simple,       p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.flags,        p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.left_color,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_settings.right_color,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_width,  p_obj);
    gbr_read_uint16(&p_gbr->tile_settings.split_height, p_obj);
    gbr_read_uint32(&p_gbr->tile_settings.split_order,  p_obj); // Should this be uint16? = 4294901760
    gbr_read_uint8 (&p_gbr->tile_settings.color_set,    p_obj);


printf("TILE_SETTINGS:\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",
                                 p_gbr->tile_settings.tile_id,
                                 p_gbr->tile_settings.simple,
                                 p_gbr->tile_settings.flags,
                                 p_gbr->tile_settings.left_color,
                                 p_gbr->tile_settings.right_color,
                                 p_gbr->tile_settings.split_width,
                                 p_gbr->tile_settings.split_height,
                                 p_gbr->tile_settings.split_order,
                                 p_gbr->tile_settings.color_set);

  return true;
}


int32_t gbr_object_tile_export_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_EXPORT_SIZE)
        return false;

    gbr_read_uint16(&p_gbr->tile_export.tile_id,        p_obj);
    gbr_read_str    (p_gbr->tile_export.file_name,      p_obj, GBR_TILE_EXPORT_FILE_NAME_SIZE);
    gbr_read_uint8 (&p_gbr->tile_export.file_type,      p_obj);
    gbr_read_str    (p_gbr->tile_export.section_name,   p_obj, GBR_TILE_EXPORT_SECTION_NAME_SIZE);
    gbr_read_str    (p_gbr->tile_export.label_name,     p_obj, GBR_TILE_EXPORT_LABEL_NAME_SIZE);
    gbr_read_uint8 (&p_gbr->tile_export.bank,           p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.tile_array,     p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.format,         p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.counter,        p_obj);
    gbr_read_uint16(&p_gbr->tile_export.from,           p_obj);
    gbr_read_uint16(&p_gbr->tile_export.upto,           p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.compression,    p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.include_colors, p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.sgb_palettes,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.gbc_palettes,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.make_meta_tiles,p_obj);
    gbr_read_uint32(&p_gbr->tile_export.meta_offset,    p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.meta_counter,   p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.split,          p_obj);
    gbr_read_uint32(&p_gbr->tile_export.block_size,     p_obj);
    gbr_read_uint8 (&p_gbr->tile_export.sel_tab,        p_obj);


printf("TILE_EXPORT:\n%d\n%s\n%d\n%s\n%s\n",
                                 p_gbr->tile_export.tile_id,
                                 p_gbr->tile_export.file_name,
                                 p_gbr->tile_export.file_type,
                                 p_gbr->tile_export.section_name,
                                 p_gbr->tile_export.section_name);

  return true;
}



int32_t gbr_object_tile_import_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes != GBR_TILE_IMPORT_SIZE)
        return false;

            gbr_read_uint16(&p_gbr->tile_import.tile_id,          p_obj);
            gbr_read_str    (p_gbr->tile_import.file_name,        p_obj, GBR_TILE_IMPORT_FILE_NAME_SIZE);
            gbr_read_uint8 (&p_gbr->tile_import.file_type,        p_obj);
            gbr_read_uint16(&p_gbr->tile_import.from_tile,        p_obj);
            gbr_read_uint16(&p_gbr->tile_import.to_tile,          p_obj);
            gbr_read_uint16(&p_gbr->tile_import.tile_count,       p_obj);
            gbr_read_uint8 (&p_gbr->tile_import.color_conversion, p_obj);
            gbr_read_uint32(&p_gbr->tile_import.first_byte,       p_obj);
            gbr_read_uint8 (&p_gbr->tile_import.binary_file_type, p_obj);


printf("TILE_import:\n%d\n%s\n%d\n%d\n%d\n",
                                 p_gbr->tile_import.tile_id,
                                 p_gbr->tile_import.file_name,
                                 p_gbr->tile_import.file_type,
                                 p_gbr->tile_import.from_tile,
                                 p_gbr->tile_import.to_tile);

  return true;
}





int32_t gbr_object_palettes_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_PALETTES_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->palettes.id,         p_obj);
            gbr_read_uint16(&p_gbr->palettes.count,      p_obj);
//            gbr_read_buf   ( p_gbr->palettes.colors,     p_obj, p_gbr->palettes.count); // TODO: this probably isn't working right
            gbr_read_buf   ( p_gbr->palettes.colors,     p_obj, GBR_PALETTE_COLOR_SETS_SIZE);
            gbr_read_uint16(&p_gbr->palettes.sgb_count,  p_obj);
//            gbr_read_buf   ( p_gbr->palettes.sgb_colors, p_obj, p_gbr->palettes.sgb_count); // TODO: this probably isn't working right
            gbr_read_buf   ( p_gbr->palettes.sgb_colors, p_obj, GBR_PALETTE_COLOR_SETS_SIZE);


printf("PALEttes:\n%d\n%d\n%d\n",
                                 p_gbr->palettes.id,
                                 p_gbr->palettes.count,
                                 p_gbr->palettes.sgb_count);

  return true;
}


int32_t gbr_object_tile_pal_decode(gbr_record * p_gbr, pascal_file_object * p_obj) {

    if (p_obj->length_bytes < GBR_TILE_PAL_SIZE_MIN)
        return false;


            gbr_read_uint16(&p_gbr->tile_pal.id,            p_obj);
            gbr_read_uint16(&p_gbr->tile_pal.count,         p_obj);
            // It's supposed to be tile_pal_count * array of 16 bit ints
            // but instead it seems to be * array of 32 bit ints...?
            gbr_read_buf   ( p_gbr->tile_pal.color_set,     p_obj, p_gbr->tile_pal.count * (sizeof(uint16_t) * 2));
            gbr_read_uint16(&p_gbr->tile_pal.sgb_count,     p_obj);
            //gbr_read_buf   ( p_gbr->tile_pal.sgb_color_set, p_obj, (p_gbr->tile_pal.sgb_count * (sizeof(uint16_t) * 2) + sizeof(uint16_t)); // Mystery extra 2 color_set reads?
            gbr_read_buf   ( p_gbr->tile_pal.sgb_color_set, p_obj, p_gbr->tile_pal.sgb_count * (sizeof(uint16_t) * 2));

printf("tile pal:\n%d\n%d\n%d\n",
                                 p_gbr->tile_pal.id,
                                 p_gbr->tile_pal.count,
                                 p_gbr->tile_pal.sgb_count);

  return true;
}

