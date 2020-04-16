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

#include "image_info.h"
#include "options.h"

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
#define GBR_TILE_DATA_COLOR_SET_SIZE  4
#define GBR_TILE_DATA_SIZE_MIN        GBR_TILE_DATA_NAME_SIZE + 2 + 2 + 2 + GBR_TILE_DATA_COLOR_SET_SIZE
#define GBR_TILE_DATA_PALETTE_SIZE_DMG (4 * 1)
#define GBR_TILE_DATA_PALETTE_SIZE_CGB (4 * 8)

#define GBR_TILE_DATA_MIN_TILE_BYTES  1024

#define GBR_TILE_SETTINGS_SIZE        19
#define GBR_TILE_SETTINGS_BOOKMARK_COUNT  3

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

#define GBR_TILE_PAL_COLOR_SET_REC_SIZE sizeof(uint32_t) // supposed to be 16 bits, but appears to be 32 bits
#define GBR_TILE_PAL_UNKNOWN_PADDING 8

#define GBR_COL_MAX 0xF8

// 128 bytes = TGBColorSets -> TGBColorType x 8 -> TColor x 4 -> uint8 x 4 (XRGB)
#define GBR_PALETTE_TCOLOR_SIZE         sizeof(uint32_t) // Pascal:TCOLOR
#define GBR_PALETTE_TGBCOLORTYPE_SIZE  4 * GBR_PALETTE_TCOLOR_SIZE
#define GBR_PALETTE_CGB_SETS_SIZE      8 * GBR_PALETTE_TGBCOLORTYPE_SIZE
#define GBR_PALETTE_SGB_SETS_SIZE      4 * GBR_PALETTE_TGBCOLORTYPE_SIZE
#define GBR_PALETTE_SGB_SETS_ACTUAL_SIZE      8 * GBR_PALETTE_TGBCOLORTYPE_SIZE // GBTD loads this as 4 but saves it with 8 sets (for 128 bytes)

#define GBR_MAP_TILE_PAL_OVERRIDE_NONE 0 // 0 = use default tile pal in cgb mode
#define GBR_MAP_TILE_PAL_OFFSET        1 // Actual palette number is -1 from value since 0 indicates default


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
    char name[GBR_PRODUCER_NAME_SIZE_STR];
    char version[GBR_PRODUCER_VERSION_SIZE_STR];
    char info[GBR_PRODUCER_INFO_SIZE_STR];
} gbr_producer;


typedef struct {
    char       name[GBR_TILE_DATA_NAME_SIZE_STR];
    uint16_t   width;
    uint16_t   height;
    uint16_t   count;
    uint8_t    color_set[GBR_TILE_DATA_COLOR_SET_SIZE];
               // tile_list: packed arrays width * height * count.
               //            width & height can be up to 32 x 32 (0..31)
    uint8_t    tile_list[PASCAL_OBJECT_MAX_SIZE];

    // End of native structure
    uint32_t   pal_data_size;
    uint32_t   tile_data_size;
    uint16_t   padding_tile_count; // Used since GBTD has a 1024 pixel minimum for tile data (8x8x16, 32x32x1, etc) see: GBR_TILE_DATA_MIN_TILE_BYTES
} gbr_tile_data;


typedef struct {
    uint16_t  tile_id;
    uint8_t   simple;
    uint8_t   flags;
    uint8_t   left_color;
    uint8_t   right_color;
    uint16_t  split_width;
    uint16_t  split_height;
    uint8_t   split_order; // spec/source shows int32, but file parsing indicates int8
    uint8_t   color_set;

    uint16_t  bookmarks[GBR_TILE_SETTINGS_BOOKMARK_COUNT];
    uint8_t   auto_update;

    // End of native structure
    uint16_t   pal_data_size;
} gbr_tile_settings;



typedef struct {
    uint16_t   tile_id;
    char       file_name[GBR_TILE_EXPORT_FILE_NAME_SIZE_STR];
    uint8_t    file_type;
    char       section_name[GBR_TILE_EXPORT_SECTION_NAME_SIZE_STR];
    char       label_name[GBR_TILE_EXPORT_LABEL_NAME_SIZE_STR];
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
    char      file_name[GBR_TILE_IMPORT_FILE_NAME_SIZE];
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
    // End of native structure
    uint16_t  color_set_size_bytes;
    uint16_t  sgb_color_set_size_bytes;
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
    uint8_t  * p_data;
    // End of native object
    uint32_t   offset;
} gbr_file_object;




image_data * gbr_get_image(void);
color_data * gbr_get_colors(void);

void gbr_set_image(image_data * p_src_image);
void gbr_set_colors(color_data * p_src_colors);

int32_t gbr_load(const char * filename);
int32_t gbr_save(const char * filename, image_data * p_src_image, color_data * p_colors, tile_process_options plugin_options);

void gbr_free_resources(void);

int32_t gbr_load_file(const char * filename);
int32_t gbr_save_file(const char * filename);

gbr_record * gbr_get_ptr(void);

uint32_t  gbr_get_export_rec_size(void);
uint8_t * gbr_get_export_rec_buffer(void);
void      gbr_set_export_from_buffer(uint32_t buffer_size, uint8_t * p_src_buf);

int32_t gbr_export_tileset_calc_tile_count_padding(gbr_record * p_gbr);
int32_t gbr_export_tileset_calc_dimensions(gbr_record * p_gbr, image_data * p_image);

#endif // LIB_GBR_FILE_HEADER
