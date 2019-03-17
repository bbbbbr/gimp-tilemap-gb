//
// lib_tilemap.c
//
#include <stdio.h>
#include <string.h>


#include "lib_tilemap.h"
#include "tilemap_tiles.h"
#include "tilemap_io.h"

#include "xtea.h"

// Globals
tile_map_data tile_map;
tile_set_data tile_set;



// TODO: Fix the dubious mixing of global and locals. Simplify code

// TODO: support configurable tile size
int tilemap_initialize(image_data * p_src_img) {

    // Tile Map
    tile_map.map_width   = p_src_img->width;
    tile_map.map_height  = p_src_img->height;

    tile_map.tile_width  = TILE_WIDTH_DEFAULT;
    tile_map.tile_height = TILE_HEIGHT_DEFAULT;

    tile_map.width_in_tiles  = tile_map.map_width  / tile_map.tile_width;
    tile_map.height_in_tiles = tile_map.map_height / tile_map.tile_height;

    // Max space required to store Tile Map is
    // width x height in tiles (if every map tile is unique)
    tile_map.size = (tile_map.width_in_tiles * tile_map.height_in_tiles);

    tile_map.p_data = malloc(tile_map.size * sizeof(int32_t));

    if (!tile_map.p_data)
            return(false);


    // Tile Set
    tile_set.tile_bytes_per_pixel = p_src_img->bytes_per_pixel;
    tile_set.tile_width  = TILE_WIDTH_DEFAULT;
    tile_set.tile_height = TILE_HEIGHT_DEFAULT;
    tile_set.tile_size   = tile_set.tile_width * tile_set.tile_height * tile_set.tile_bytes_per_pixel;
    tile_set.tile_count  = 0;

    return (true);
}



unsigned char tilemap_export_process(image_data * p_src_img) {

    if ( check_dimensions_valid(p_src_img) ) {
        if (!tilemap_initialize(p_src_img)) // Success, prep for processing
            return (false); // Signal failure and exit
    }
    else
        return (false); // Signal failure and exit

    if ( ! process_tiles(p_src_img) )
        return (false); // Signal failure and exit
}



unsigned char process_tiles(image_data * p_src_img) {

    int         img_x, img_y;
    uint32_t  * tile_buf_intermediary; // Needs to be 32 bit aligned for hash function
    tile_data   tile;
    uint32_t    tile_size_bytes;
    uint32_t    tile_size_bytes_hash_padding; // Make sure hashed data is multiple of 32 bits
    uint64_t    tile_hash;
    uint32_t    img_buf_offset;
    int32_t     tile_id;
    int32_t     map_slot;

    map_slot = 0;

    tile.raw_bytes_per_pixel = p_src_img->bytes_per_pixel;
    tile.raw_width           = tile_map.tile_width;
    tile.raw_height          = tile_map.tile_height;
    tile.raw_size_bytes      = tile.raw_height * tile.raw_width * tile.raw_bytes_per_pixel;

    // Make sure buffer is an even multiple of 32 bits (for hash function)
    tile_size_bytes_hash_padding = tile_size_bytes % sizeof(uint32_t);

    // Allocate buffer for temporary working tile raw image
    // Use a uint32 for initial allocation, then hand it off to the uint8
    // TODO: fix this hack. rumor is that in PC world uint8 buffers always get 32 bit alligned?
    tile_buf_intermediary = malloc(tile_size_bytes + tile_size_bytes_hash_padding);
    tile.p_img_raw        = (uint8_t *)tile_buf_intermediary;

    // Make sure padding bytes are zeroed
    memset(tile.p_img_raw, 0x00, tile_size_bytes_hash_padding);



    if (tile.p_img_raw) {

        // Iterate over the map, top -> bottom, left -> right
        img_buf_offset = 0;

        for (img_y = 0; img_y < tile_map.map_height; img_y += tile_map.tile_height) {
            for (img_x = 0; img_x < tile_map.map_width; img_x += tile_map.tile_width) {

                // Set buffer offset to upper left of current tile
                img_buf_offset = (img_x + (img_y * tile_map.map_width)) * p_src_img->bytes_per_pixel;

                tile_copy_tile_from_image(p_src_img,
                                          &tile,
                                          img_buf_offset);

                // TODO! Don't hash transparent pixels? Have to overwrite second byte?
                tile.hash = xtea_hash_u32((tile.raw_size_bytes + tile_size_bytes_hash_padding) / sizeof(uint32_t),
                                          (uint32_t *)tile.p_img_raw);

                tile_id = tile_find_matching(tile.hash, &tile_set);


                // Tile not found, create a new entry
                if (tile_id == TILE_ID_NOT_FOUND) {

                    tile_id = tile_register_new(&tile, &tile_set);

                    if (tile_id <= TILE_ID_OUT_OF_SPACE) {
                        free(tile.p_img_raw);
                        return (false); // Ran out of tile space, exit
                    }
                }

                int32_t test;
                test = tile_id;

                tile_map.p_data[map_slot] = test; // = tile_id; // TODO: IMPORTANT, SOMETHING IS VERY WRONG

// printf("Map Slot %d: tile_id=%d tilemap[]=%d, %08lx\n",map_slot, tile_id, tile_map.p_data[map_slot], tile.hash);
                map_slot++;
            }
        }

    } else { // else if (tile.p_img_raw) {
        if (tile_map.p_data)
            free(tile_map.p_data);
        return (false); // Failed to allocate buffer, exit
    }

    if (tile.p_img_raw)
        free(tile.p_img_raw);

    printf("Total Tiles=%d\n", tile_set.tile_count);
}


static int32_t check_dimensions_valid(image_data * p_src_img) {

    // Image dimensions must be exact multiples of tile size
    if ( ((p_src_img->width % TILE_WIDTH_DEFAULT) != 0) ||
         ((p_src_img->height % TILE_HEIGHT_DEFAULT ) != 0))
        return false; // Fail
    else
        return true;  // Success
}



void tilemap_free_resources() {

    int c;

    // Free all the tile set data
    for (c = 0; c < tile_set.tile_count; c++) {

        if (tile_set.tiles[c].p_img_encoded)
            free(tile_set.tiles[c].p_img_encoded);

        if (tile_set.tiles[c].p_img_raw)
            free(tile_set.tiles[c].p_img_raw);
    }

    // Free tile map data
    if (tile_map.p_data)
        free(tile_map.p_data);

}




int32_t tilemap_save(const int8_t * filename, uint32_t export_format) {

    return( tilemap_export(filename,
                           export_format,
                           &tile_map,
                           &tile_set) );
}
