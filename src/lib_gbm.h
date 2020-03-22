//
//   lib_gbm.h
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

#ifndef LIB_GBM_FILE_HEADER
#define LIB_GBM_FILE_HEADER

#define GBM_PRODUCER_NAME_SIZE    128
#define GBM_PRODUCER_VERSION_SIZE 10
#define GBM_PRODUCER_INFO_SIZE    128
#define GBM_PRODUCER_SIZE  GBM_PRODUCER_NAME_SIZE + GBM_PRODUCER_VERSION_SIZE + GBM_PRODUCER_INFO_SIZE

// TODO: inspect strings and determine if all the + 1 _STR stuff can be remove (are strings always null terminated in data file despite being pascal/delphi?)
#define GBM_PRODUCER_NAME_SIZE_STR    GBM_PRODUCER_NAME_SIZE    + 1 // space for trailing \0
#define GBM_PRODUCER_VERSION_SIZE_STR GBM_PRODUCER_VERSION_SIZE + 1
#define GBM_PRODUCER_INFO_SIZE_STR    GBM_PRODUCER_INFO_SIZE    + 1

#define GBM_MAP_NAME_SIZE      128
#define GBM_MAP_TILE_FILE_SIZE 256

#define GBM_MAP_SIZE           20 + GBM_MAP_NAME_SIZE + GBM_MAP_TILE_FILE_SIZE

#define GBM_OBJECT_MAX_SIZE 65535 // This may not be true, it's just for convenience :)

#define GBM_MAP_TILE_DATA_RECORDS_SIZE  GBM_OBJECT_MAX_SIZE
#define GBM_MAP_TILE_DATA_PADDING_UNKNOWN 4800 - 2160 // TODO: what is this, and is it variable?

#define GBM_MAP_PROP_NAME_SIZE 32

#define GBM_MAP_SETTINGS_BOOKMARK_COUNT 3
#define GBM_MAP_SETTINGS_BOOKMARK_SIZE  GBM_MAP_SETTINGS_BOOKMARK_COUNT * sizeof(uint16_t)

#define GBM_MAP_SETTINGS_MIN_SIZE    29 + GBM_MAP_SETTINGS_BOOKMARK_SIZE

#define GBM_MAP_PROP_COLORS_COLORS_COUNT 2
#define GBM_MAP_PROP_COLORS_COLORS_SIZE  GBM_MAP_PROP_COLORS_COLORS_COUNT * 3 * 4 // uint24?

#define GBM_MAP_EXPORT_FILE_NAME_SIZE    255
#define GBM_MAP_EXPORT_SECTION_NAME_SIZE 40
#define GBM_MAP_EXPORT_LABEL_NAME_SIZE   40

#define GBM_MAP_EXPORT_SIZE 19 + GBM_MAP_EXPORT_FILE_NAME_SIZE + GBM_MAP_EXPORT_SECTION_NAME_SIZE + GBM_MAP_EXPORT_LABEL_NAME_SIZE

#define GBM_MAP_EXPORT_PROPS_COUNT 2  // It's possible this should be larger
#define GBM_MAP_EXPORT_PROPS_SIZE  GBM_MAP_PROP_COLORS_COLORS_COUNT * 4 // Maybe uint32? // 3 // uint24


// These are used with: map_tile_data.record
#define GBM_MAP_TILE_RECORD_SIZE  3 // 24 bits per record
// TODO: original pascal says bits 0..8 ~ 0-511, but uses 0x3FF. Which is right?
#define GBM_MAP_TILE_NUM      0x0001FF //.0-8
#define GBM_MAP_TILE_RESERVED 0x3FFE00 //.9-21
#define GBM_MAP_TILE_FLIP_H   0x400000 //.24
#define GBM_MAP_TILE_FLIP_V   0x800000 //.23
#define GBM_MAP_TILE_FLIP_H_BYTE   0x40 //.24
#define GBM_MAP_TILE_FLIP_V_BYTE   0x80 //.24

#define GBM_MAP_EXPORT_DELETED_1_SIZE 0x012C
#define GBM_MAP_EXPORT_DELETED_2_SIZE 0x0


typedef struct {
    uint16_t num;
    uint8_t  flip_h;
    uint8_t  flip_v;
} gbm_tile_record;


enum gbm_object_types {
    gbm_obj_producer        = 0x01,
    gbm_obj_map             = 0x02,
    gbm_obj_map_tile_data   = 0x03,
    gbm_obj_map_prop        = 0x04,
    gbm_obj_prop_data       = 0x05,
    gbm_obj_prop_default    = 0x06,
    gbm_obj_map_settings    = 0x07,
    gbm_obj_prop_colors     = 0x08,
    gbm_obj_map_export      = 0x09,
    gbm_obj_map_export_prop = 0x0A,
    gbm_obj_deleted         = 0xFFFF,
};



typedef struct {
    int8_t name[GBM_PRODUCER_NAME_SIZE_STR];
    int8_t version[GBM_PRODUCER_VERSION_SIZE_STR];
    int8_t info[GBM_PRODUCER_INFO_SIZE_STR];
} gbm_producer;


typedef struct {
    int8_t   name[GBM_MAP_NAME_SIZE];
    uint32_t width;
    uint32_t height;
    uint32_t prop_count;
    int8_t   tile_file[GBM_MAP_TILE_FILE_SIZE];
    uint32_t tile_count;
    uint32_t prop_color_count;
    // End of native structure
} gbm_map;


typedef struct {
    uint8_t   records[GBM_MAP_TILE_DATA_RECORDS_SIZE];  // uint24
    // End of native structure
    uint32_t  length_bytes;
} gbm_map_tile_data;


typedef struct {
    uint32_t p_type;
    uint32_t size;
    int8_t   name[GBM_MAP_PROP_NAME_SIZE];
} gbm_map_prop;


typedef struct {
    uint8_t  data[GBM_OBJECT_MAX_SIZE];
} gbm_map_prop_data;


typedef struct {
    uint8_t  data[GBM_OBJECT_MAX_SIZE];
} gbm_map_prop_default;


typedef struct {
    uint32_t  form_width;
    uint32_t  form_height;
    uint8_t   form_maximized; // bool

    uint8_t   info_panel;     // bool
    uint8_t   grid;           // bool
    uint8_t   double_markers; // bool
    uint8_t   prop_colors;    // bool

    uint16_t  zoom;
    uint16_t  color_set;
    uint16_t  bookmarks[GBM_MAP_SETTINGS_BOOKMARK_COUNT]; // uint32

    uint32_t  block_fill_pattern;
    uint32_t  block_fill_width;
    uint32_t  block_fill_height;

    uint8_t   auto_update; // bool
} gbm_map_settings;


typedef struct {
    uint8_t   colors[GBM_MAP_PROP_COLORS_COLORS_SIZE];  // uint24
} gbm_map_prop_colors;


typedef struct {
    int8_t   file_name[GBM_MAP_EXPORT_FILE_NAME_SIZE];
    uint8_t  file_type;
    int8_t   section_name[GBM_MAP_EXPORT_SECTION_NAME_SIZE];
    int8_t   label_name[GBM_MAP_EXPORT_LABEL_NAME_SIZE];
    uint8_t  bank;

    uint16_t  plane_count;
    uint16_t  plane_order;
    uint16_t  map_layout;

    uint8_t   split;      // bool
    uint32_t  split_size;
    uint8_t   split_bank; // bool

    uint8_t   sel_tab;

    uint16_t  prop_count;

    uint16_t  tile_offset;
} gbm_map_export;


typedef struct {
    uint8_t   props[GBM_MAP_EXPORT_PROPS_SIZE];  // uint24? uint32?
} gbm_map_export_prop;





typedef struct {
    gbm_producer         producer;
    gbm_map              map;
    gbm_map_tile_data    map_tile_data;
    gbm_map_prop         map_prop;         // Doesn't seem to be used
    gbm_map_prop_data    map_prop_data;    // Doesn't seem to be used
    gbm_map_prop_default map_prop_default; // Doesn't seem to be used
    gbm_map_settings     map_settings;
    gbm_map_prop_colors  map_prop_colors;
    gbm_map_export       map_export;
    gbm_map_export_prop  map_export_prop;
} gbm_record;


typedef struct {
    uint8_t   marker[6];    // Should match "HPJMTL"
    uint16_t  id;
    uint16_t  object_id;
    uint16_t  master_id;
    uint32_t  crc;          // Always zero
    uint32_t  length_bytes; // "size"
    uint32_t  offset;
    uint8_t  * p_data;
} gbm_file_object;


image_data * gbm_get_image(void);
color_data * gbm_get_colors(void);
void gbm_set_image(image_data * p_src_image);
void gbm_set_colors(color_data * p_src_colors);


void gbm_free_resources(void);

int32_t gbm_load(const int8_t * filename);
int32_t gbm_load_file(const int8_t * filename);
int32_t gbm_save(const int8_t * filename, image_data * p_src_image, color_data * p_colors);
int32_t gbm_save_file(const int8_t * filename);

void gbm_map_tiles_print(gbm_record * p_gbm);



#endif // LIB_GBM_FILE_HEADER
