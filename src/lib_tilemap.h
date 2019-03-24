// lib_tilemap.h

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "image_info.h"

#ifndef LIB_TILEMAP_HEADER
#define LIB_TILEMAP_HEADER

    #define TILES_MAX_DEFAULT 255

    #define TILE_WIDTH_DEFAULT  8
    #define TILE_HEIGHT_DEFAULT 8

    #define TILE_ID_NOT_FOUND     -1
    #define TILE_ID_OUT_OF_SPACE  -2
    #define TILE_ID_FAILED_ENCODE -3


    enum import_formats {
        IMPORT_FORMAT_GBR = 1,
        IMPORT_FORMAT_GBTD = 2,  // TODO
        IMPORT_FORMAT_LAST
    };

    // App image bit depths/modes
    enum image_modes {
        IMG_BITDEPTH_INDEXED = 1,
        IMG_BITDEPTH_INDEXED_ALPHA = 2,
        IMG_BITDEPTH_RGB = 3,
        IMG_BITDEPTH_RGB_ALPHA = 4,
        IMG_BITDEPTH_LAST
    };

    // Export formats
    enum export_formats {
        EXPORT_FORMAT_GBDK_C_SOURCE = 1,
        EXPORT_FORMAT_GBTD = 2,  // TODO
        EXPORT_FORMAT_LAST
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
        int32_t * p_data; // TODO: rename tile_id_list
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


    void           tilemap_free_resources();
    static int32_t check_dimensions_valid(image_data * p_src_img);
    unsigned char  process_tiles(image_data * p_src_img);
    unsigned char  tilemap_export_process(image_data * p_src_img);
    int32_t        tilemap_initialize(image_data * p_src_img);
    int32_t        tilemap_save(const int8_t * filename, uint32_t export_format);



#endif // LIB_TILEMAP_HEADER

