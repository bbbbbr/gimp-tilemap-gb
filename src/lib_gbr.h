//
//   lib_gbr.h
//

// Reference :
// * https://github.com/bashaus/gbtiles
// * http://www.devrs.com/gb/hmgd/gbtd.html


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#ifndef LIB_GBR_FILE_HEADER
#define LIB_GBR_FILE_HEADER


#define PASCAL_OBJECT_MAX_SIZE 65535

#define GBR_PRODUCER_NAME_SIZE    30
#define GBR_PRODUCER_VERSION_SIZE 10
#define GBR_PRODUCER_INFO_SIZE    80
#define GBR_PRODUCER_SIZE  GBR_PRODUCER_NAME_SIZE + GBR_PRODUCER_VERSION_SIZE + GBR_PRODUCER_INFO_SIZE

#define GBR_PRODUCER_NAME_SIZE_STR    GBR_PRODUCER_NAME_SIZE    + 1 // space for trailing \0
#define GBR_PRODUCER_VERSION_SIZE_STR GBR_PRODUCER_VERSION_SIZE + 1
#define GBR_PRODUCER_INFO_SIZE_STR    GBR_PRODUCER_INFO_SIZE    + 1

#define GBR_TILE_DATA_NAME_SIZE       30
#define GBR_TILE_DATA_NAME_SIZE_STR   GBR_TILE_DATA_NAME_SIZE + 1
#define GBR_TILE_DATA_COLOLR_SET_SIZE 4
#define GBR_TILE_DATA_SIZE_MIN        GBR_TILE_DATA_NAME_SIZE + 2 + 2 + 2 + GBR_TILE_DATA_COLOLR_SET_SIZE

#define GBR_TILE_SETTINGS_SIZE        19

#define GBR_TILE_EXPORT_FILE_NAME_SIZE    128
#define GBR_TILE_EXPORT_SECTION_NAME_SIZE 20
#define GBR_TILE_EXPORT_LABEL_NAME_SIZE   20

#define GBR_TILE_EXPORT_FILE_NAME_SIZE_STR    GBR_TILE_EXPORT_FILE_NAME_SIZE + 1
#define GBR_TILE_EXPORT_SECTION_NAME_SIZE_STR GBR_TILE_EXPORT_SECTION_NAME_SIZE + 1
#define GBR_TILE_EXPORT_LABEL_NAME_SIZE_STR   GBR_TILE_EXPORT_LABEL_NAME_SIZE + 1

#define GBR_TILE_EXPORT_SIZE                  27 + GBR_TILE_EXPORT_FILE_NAME_SIZE + GBR_TILE_EXPORT_SECTION_NAME_SIZE + GBR_TILE_EXPORT_LABEL_NAME_SIZE


#define GBR_TILE_IMPORT_FILE_NAME_SIZE     128
#define GBR_TILE_IMPORT_FILE_NAME_SIZE_STR GBR_TILE_IMPORT_FILE_NAME_SIZE + 1
#define GBR_TILE_IMPORT_SIZE               15 + GBR_TILE_IMPORT_FILE_NAME_SIZE

#define GBR_PALETTES_SIZE_MIN 6
#define GBR_TILE_PAL_SIZE_MIN 6


#define GBR_PALETTE_SIZE_4_COLORS_XBGR 4 * sizeof(uint32_t)
#define GBR_PALETTE_COLOR_SET_SIZE     4 * GBR_PALETTE_SIZE_4_COLORS_XBGR
#define GBR_PALETTE_COLOR_SETS         8
#define GBR_PALETTE_COLOR_SETS_SIZE    GBR_PALETTE_COLOR_SETS * GBR_PALETTE_COLOR_SET_SIZE


enum gbr_tilemap_layer {
    gbr_bkg    = 0x00,
    gbr_win    = 0x80,
    gbr_sprite = 0x00
};


enum gbr_tileset_colorset {
    gbr_color_set_pocket   = 0,
    gbr_color_set_game_boy = 1,
    gbr_color_set_gbc      = 2,
    gbr_color_set_sgb      = 3
};

enum gbr_object_types {
    gbr_obj_producer      = 0x01,
    gbr_obj_tile_data     = 0x02,
    gbr_obj_tile_settings = 0x03,
    gbr_obj_tile_export   = 0x04,
    gbr_obj_tile_import   = 0x05,
    gbr_obj_palettes      = 0x0D,
    gbr_obj_tile_pal      = 0x0E,
    gbr_obj_deleted       = 0xFF
};


enum gbr_tileset_sgbpalettes {
    none               = 0,
    constant_per_entry = 1,
    per_entry_2_bits   = 2, // 2_bits_per_entry
    per_entry_4_bits   = 3, // 4_bits_per_entry
    per_entry_1_byte   = 4  // 1_byte_per_entry
};


enum gbr_tileset_splitorder {
    lrtb       = 0,
    horizontal = 0,
    tblr       = 1,
    vertical   = 1
};


typedef struct {
    int8_t name[GBR_PRODUCER_NAME_SIZE_STR];
    int8_t version[GBR_PRODUCER_VERSION_SIZE_STR];
    int8_t info[GBR_PRODUCER_INFO_SIZE_STR];
} gbr_producer;


typedef struct {
    int8_t     name[GBR_TILE_DATA_NAME_SIZE_STR];
    uint16_t   width;
    uint16_t   height;
    uint16_t   count;
               // tile_list: packed arrays width * height * count.
               //            width & height can be up to 32 x 32 (0..31)
    uint8_t    tile_list[PASCAL_OBJECT_MAX_SIZE];
    uint8_t    color_set[GBR_TILE_DATA_COLOLR_SET_SIZE];
    uint32_t   data_size;
} gbr_tile_data;


typedef struct {
    uint16_t  tile_id;
    uint8_t   simple;
    uint8_t   flags;
    uint8_t   left_color;
    uint8_t   right_color;
    uint16_t  split_width;
    uint16_t  split_height;
    uint32_t  split_order;
    uint8_t   color_set;

    uint32_t  bookmarks[3];
    uint8_t   auto_update;
} gbr_tile_settings;



typedef struct {
    uint16_t   tile_id;
    int8_t     file_name[GBR_TILE_EXPORT_FILE_NAME_SIZE_STR];
    uint8_t    file_type;
    int8_t     section_name[GBR_TILE_EXPORT_SECTION_NAME_SIZE_STR];
    int8_t     label_name[GBR_TILE_EXPORT_LABEL_NAME_SIZE_STR];
    uint8_t    bank;
    uint8_t    tile_array;
    uint8_t    format;
    uint8_t    counter;
    uint16_t   from;
    uint16_t   upto;
    uint8_t    compression;
    uint8_t    include_colors;
    uint8_t    sgb_palettes;
    uint8_t    gbc_palettes;
    uint8_t    make_meta_tiles;
    uint32_t   meta_offset;
    uint8_t    meta_counter;
    uint8_t    split;
    uint32_t   block_size;
    uint8_t    sel_tab;
} gbr_tile_export;


typedef struct {
    uint16_t  tile_id;
    int8_t    file_name[GBR_TILE_IMPORT_FILE_NAME_SIZE];
    uint8_t   file_type;
    uint16_t  from_tile;
    uint16_t  to_tile;
    uint16_t  tile_count;
    uint8_t   color_conversion;
    uint32_t  first_byte;
    uint8_t   binary_file_type;

} gbr_tile_import;


typedef struct {
    uint16_t  id;
    uint16_t  count;
    uint8_t   colors[PASCAL_OBJECT_MAX_SIZE];
    uint16_t  sgb_count;
    uint8_t   sgb_colors[PASCAL_OBJECT_MAX_SIZE];
} gbr_palettes;

typedef struct {
    uint16_t  id;
    uint16_t  count;
    uint8_t   color_set[PASCAL_OBJECT_MAX_SIZE];
    uint16_t  sgb_count;
    uint8_t   sgb_color_set[PASCAL_OBJECT_MAX_SIZE];
} gbr_tile_pal;



typedef struct {
    gbr_producer      producer;
    gbr_tile_data     tile_data;
    gbr_tile_settings tile_settings;
    gbr_tile_export   tile_export;
    gbr_tile_import   tile_import;
    gbr_palettes      palettes;
    gbr_tile_pal      tile_pal;
} gbr_record;



typedef struct {
    uint16_t   type;
    uint16_t   id;
    uint32_t   length_bytes;
    uint32_t   offset;
    uint8_t  * p_data;
} pascal_file_object;


#endif // LIB_GBR_FILE_HEADER
