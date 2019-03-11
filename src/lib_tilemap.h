// lib_tilemap.h

#include <stdint.h>

#ifndef LIB_TILEMAP_HEADER
#define LIB_TILEMAP_HEADER

#define TILES_MAX_DEFAULT 255

enum image_depths {
    IMG_BITDEPTH_INDEXED = 1,
    IMG_BITDEPTH_INDEXED_ALPHA = 2,
    IMG_BITDEPTH_LAST
};



// Tile Map
typedef struct {
    uint16_t width_in_tiles;
    uint16_t height_in_tiles;
    uint16_t tile_width;
    uint16_t tile_height;
    uint32_t size;
    uint32_t * p_data;
} tile_map_data;


// Individual Tile from Tile Set
typedef struct {
    uint8_t  bpp;
    uint16_t width;
    uint16_t height;
    uint32_t size;
    uint32_t * p_data;
    uint32_t hash[2]; // uint64_t hash; ??
} tile_data;

// Tile Set (composed of individual tiles)
typedef struct {
    uint32_t tile_count;
    tile_data tiles[TILES_MAX_DEFAULT];
} tile_set_data;



#endif // LIB_TILEMAP_HEADER

