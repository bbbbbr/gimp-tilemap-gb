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
    uint8_t * p_data;
    uint64_t  hash;
} tile_data;

// Tile Set (composed of individual tiles)
typedef struct {
    uint8_t  tile_bytes_per_pixel;
    uint16_t tile_width;
    uint16_t tile_height;
    uint32_t tile_size;
    uint32_t tile_count;
    tile_data tiles[TILES_MAX_DEFAULT];
} tile_set_data;


// Source Image
typedef struct {
    uint8_t    bytes_per_pixel;
    uint16_t   width;
    uint16_t   height;
    uint32_t   size;
    uint8_t  * p_img_data;
} image_data;


void           tilemap_free_resources();
static int32_t check_dimensions_valid(image_data * p_src_img);
static void    copy_tile_into_buffer(image_data * p_src_img, uint8_t * p_tile, uint32_t img_buf_offset);
int32_t        find_matching_tile(uint64_t hash_sig);
uint8_t *      tile_encode(uint8_t bytes_per_pixel, uint8_t * p_src_tile);
int32_t        register_new_tile(uint64_t hash_sig, uint8_t * p_tile);
unsigned char  process_tiles(image_data * p_src_img);
unsigned char  tilemap_export_process(image_data * p_src_img);
int            tilemap_initialize(image_data * p_src_img);


static void print_tile_buffer(image_data * p_src_img, uint8_t * p_tile);

#endif // LIB_TILEMAP_HEADER

