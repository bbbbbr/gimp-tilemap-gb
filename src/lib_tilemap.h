// lib_tilemap.h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#ifndef LIB_TILEMAP_HEADER
#define LIB_TILEMAP_HEADER

#define TILES_MAX_DEFAULT 255

#define TILE_WIDTH_DEFAULT  8
#define TILE_HEIGHT_DEFAULT 8

#define TILE_ID_NOT_FOUND     -1
#define TILE_ID_OUT_OF_SPACE  -2
#define TILE_ID_FAILED_ENCODE -3

enum image_modes {
    IMG_BITDEPTH_INDEXED = 1,
    IMG_BITDEPTH_INDEXED_ALPHA = 2,
    IMG_BITDEPTH_RGB = 3,
    IMG_BITDEPTH_RGB_ALPHA = 4,
    IMG_BITDEPTH_LAST
};



// Tile Map
typedef struct {
    uint16_t width_in_tiles;
    uint16_t height_in_tiles;
    uint16_t tile_width;
    uint16_t tile_height;
    uint16_t map_width;
    uint16_t map_height;
    uint32_t size;
    uint32_t * p_data; // TODO: rename tile_id_list
} tile_map_data;


// Individual Tile from Tile Set
typedef struct {
    uint64_t  hash;
    uint8_t   raw_bytes_per_pixel;
    uint16_t  raw_width;
    uint16_t  raw_height;
    uint32_t  raw_size_bytes;     // size in bytes // TODO
    uint32_t  encoded_size_bytes; // size in bytes
    uint8_t * p_img_raw;
    uint8_t * p_img_encoded;
} tile_data;

// Tile Set (composed of individual tiles)
typedef struct {
    uint8_t  tile_bytes_per_pixel; // TODO: convert me to tiles[n].raw_bytes_per_pixel, raw_width, raw_height
    uint16_t tile_width;
    uint16_t tile_height;
    uint32_t tile_size;  // size in bytes
    uint32_t tile_count;
    tile_data tiles[TILES_MAX_DEFAULT];
} tile_set_data;


// Source Image
typedef struct {
    uint8_t    bytes_per_pixel;
    uint16_t   width;
    uint16_t   height;
    uint32_t   size;  // size in bytes
    uint8_t  * p_img_data;
} image_data;


void           tilemap_free_resources();
static int32_t check_dimensions_valid(image_data * p_src_img);
unsigned char  process_tiles(image_data * p_src_img);
unsigned char  tilemap_export_process(image_data * p_src_img);
int            tilemap_initialize(image_data * p_src_img);
int32_t        tilemap_save(const int8_t * filename);

// TODO: move us into a separate file (tile_ops.c/h)
static void    tile_copy_into_buffer(image_data * p_src_img, tile_data tile, uint32_t img_buf_offset);
int32_t        tile_find_matching(uint64_t hash_sig);
int32_t        tile_encode(tile_data * p_tile, uint32_t image_mode);
int32_t        tile_register_new(tile_data tile);

// TODO: delete me
static void tile_print_buffer_raw(tile_data tile);
static void tile_print_buffer_encoded(tile_data tile);

char * get_filename_from_path(char * path);
int32_t        tilemap_format_gbdk_c_source_save(const int8_t * filename);

#endif // LIB_TILEMAP_HEADER

