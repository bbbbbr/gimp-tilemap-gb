// lib_tilemap.c

#include "lib_tilemap.h"
#include "xtea.h"


// Globals
tile_map_data tile_map;
tile_set_data tile_set;


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

    tile_map.p_data = malloc(tile_map.size * sizeof(uint32_t));


    // Tile Set
    tile_set.tile_bytes_per_pixel = p_src_img->bytes_per_pixel;
    tile_set.tile_width  = TILE_WIDTH_DEFAULT;
    tile_set.tile_height = TILE_HEIGHT_DEFAULT;
    tile_set.tile_size   = tile_set.tile_width * tile_set.tile_height * tile_set.tile_bytes_per_pixel;
    tile_set.tile_count  = 0;
}



unsigned char tilemap_export_process(image_data * p_src_img) {

    if ( check_dimensions_valid(p_src_img) )
        tilemap_initialize(p_src_img); // Success, prep for processing
    else
        return (false); // Signal failure and exit

    if ( ! process_tiles(p_src_img) )
        return (false); // Signal failure and exit
}



unsigned char process_tiles(image_data * p_src_img) {

    int        img_x, img_y;
    uint32_t * p_tile;          // Needs to be 32 bit aligned for hash function
    uint32_t   tile_size_bytes;
    uint32_t   tile_size_bytes_hash_padding; // Make sure hashed data is multiple of 32 bits
    uint64_t   tile_hash;
    uint32_t   img_buf_offset;
    int32_t    tile_id;
    int32_t    map_slot;

    map_slot = 0;
    // Allocate buffer for temporary working tile
    tile_size_bytes = tile_map.tile_width * tile_map.tile_height * p_src_img->bytes_per_pixel;

    // Make sure buffer is an even multiple of 32 bits (for hash function)
    tile_size_bytes_hash_padding = tile_size_bytes % sizeof(uint32_t);

    p_tile = malloc(tile_size_bytes + tile_size_bytes_hash_padding);

    // Make sure padding bytes are zeroed
    memset(p_tile, 0x00, tile_size_bytes_hash_padding);



    if (p_tile) {

        // Iterate over the map, top -> bottom, left -> right
        img_buf_offset = 0;

        for (img_y = 0; img_y < tile_map.map_height; img_y += tile_map.tile_height) {
            for (img_x = 0; img_x < tile_map.map_width; img_x += tile_map.tile_width) {

// printf("====== NEW TILE =======\n");

                // Set buffer offset to upper left of current tile
                img_buf_offset = (img_x + (img_y * tile_map.map_width)) * p_src_img->bytes_per_pixel;

                copy_tile_into_buffer(p_src_img, (uint8_t *)p_tile, img_buf_offset);
// print_tile_buffer(p_src_img, (uint8_t *)p_tile);


                // TODO! Don't hash transparent pixels? Have to overwrite second byte?
                tile_hash = xtea_hash_u32((tile_size_bytes + tile_size_bytes_hash_padding) / sizeof(uint32_t),
                                           p_tile);

                tile_id = find_matching_tile(tile_hash);

                // Tile not found, create a new entry
                if (tile_id == TILE_ID_NOT_FOUND) {

                    tile_id = register_new_tile(tile_hash, (uint8_t *)p_tile);
                    printf(" -> %4d,%4d  - hash:0x%08lx register_new_tile:%d\n", img_x, img_y, tile_hash, tile_id);

                    if (tile_id == TILE_ID_OUT_OF_SPACE) {
                        free(p_tile);
                        return (false); // Ran out of tile space, exit
                    }
                }
                else
                    printf(" -> %4d,%4d  - hash:0x%08lx FOUND matching tile:%d\n", img_x, img_y, tile_hash, tile_id);

                // TODO: insert tile id into tile map for current location
                *(tile_map.p_data + map_slot) = tile_id;
                map_slot++;
            }
        }

    } else { // else if (p_tile) {
        free(p_tile);
        return (false); // Failed to allocate buffer, exit
    }

    free(p_tile);

    printf("Total Tiles=%d\n", tile_set.tile_count);
}


int32_t register_new_tile(uint64_t hash_sig, uint8_t * p_tile) {

    int32_t tile_id;

    if (tile_set.tile_count < TILES_MAX_DEFAULT) {

        // Store hash and encoded image data into tile
        tile_set.tiles[tile_set.tile_count].hash = hash_sig;
        tile_set.tiles[tile_set.tile_count].p_data = tile_encode(tile_set.tile_bytes_per_pixel, p_tile);

        // Set tile id to current tile, or signal if
        // encoding or allocating space for the encoded tile
        if (tile_set.tiles[tile_set.tile_count].p_data)
            tile_id = tile_set.tile_count;
        else
            tile_id = TILE_ID_OUT_OF_SPACE;

        // Move to new tile
        tile_set.tile_count++;
    }
    else
        tile_id TILE_ID_OUT_OF_SPACE;

    return (tile_id);
}



uint8_t * tile_encode(uint8_t bytes_per_pixel, uint8_t * p_src_tile) {

    uint8_t * encoded_tile_data;

    // TODO TODO TODO TODO
    // Calculate size and allocate buffer for encoded tile data
//    encoded_tile_data = malloc(somesize);


    // Encode tile data into tile buffer
    // encode_func(encoded_tile_data);
    //  <<---------------------------------------------------------------- HERE


    // Return pointer to new buffer
    return encoded_tile_data;
}



int32_t find_matching_tile(uint64_t hash_sig) {

    int c;

    for (c = 0; c < tile_set.tile_count; c++) {
        if (hash_sig == tile_set.tiles[c].hash) {
            // found a matching tile, return it's ID
            return(c);
        }
    }

    // No matching tile found
    return(TILE_ID_NOT_FOUND);
}



static void copy_tile_into_buffer(image_data * p_src_img, uint8_t * p_tile, uint32_t img_buf_offset) {

    int32_t tile_y;
    int32_t image_width_bytes;
    int32_t tile_width_bytes;

    image_width_bytes = tile_map.map_width  * p_src_img->bytes_per_pixel;
    tile_width_bytes  = tile_map.tile_width * p_src_img->bytes_per_pixel;

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < tile_map.tile_height; tile_y++) {

        // Copy a row of tile data into the temp tile buffer
        memcpy(p_tile,
               p_src_img->p_img_data + img_buf_offset,
               tile_width_bytes);

        // Move to the next row in image
        img_buf_offset += image_width_bytes;

        // Move to next row in tile buffer
        p_tile += tile_width_bytes;
    }
}



// TODO: DEBUG: REMOVE ME
static void print_tile_buffer(image_data * p_src_img, uint8_t * p_tile) {

    int32_t tile_y;
    int32_t tile_x;

 printf("\n");

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < tile_map.tile_height; tile_y++) {
        for (tile_x = 0; tile_x < tile_map.tile_width; tile_x++)
        {
            printf(" %2x", *(p_tile));

            // Move to the next row
            p_tile += p_src_img->bytes_per_pixel;
        }
        printf(" \n");
    }

printf(" \n");
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
        if (tile_set.tiles[c].p_data)
            free(tile_set.tiles[c].p_data);
    }

    // Free tile map data
    if (tile_map.p_data)
        free(tile_map.p_data);

}