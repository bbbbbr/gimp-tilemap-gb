//
// lib_tilemap.c
//

#include "lib_tilemap.h"
#include "xtea.h"
#include "lib_rom_bin.h"


// Globals
tile_map_data tile_map;
tile_set_data tile_set;



// TODO: Fix up all the dubious mixing of global and locals. Simplify code



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
printf("Map Slot %d\n",map_slot);
                tile_copy_into_buffer(p_src_img,
                                      tile,
                                      img_buf_offset);

                // TODO! Don't hash transparent pixels? Have to overwrite second byte?
                tile.hash = xtea_hash_u32((tile.raw_size_bytes + tile_size_bytes_hash_padding) / sizeof(uint32_t),
                                           (uint32_t *)tile.p_img_raw);

                tile_id = tile_find_matching(tile.hash);

// tile_print_buffer_raw(tile);

                // Tile not found, create a new entry
                if (tile_id == TILE_ID_NOT_FOUND) {

                    tile_id = tile_register_new(tile);

printf("///  New Tile\n");
tile_print_buffer_raw(tile_set.tiles[tile_id]);

                    if (tile_id == TILE_ID_OUT_OF_SPACE) {
                        free(tile.p_img_raw);
                        return (false); // Ran out of tile space, exit
                    }
                }
                else

                // TODO: insert tile id into tile map for current location
                *(tile_map.p_data + map_slot) = tile_id;
                map_slot++;
            }
        }

    } else { // else if (tile.p_img_raw) {
        free(tile.p_img_raw);
        return (false); // Failed to allocate buffer, exit
    }

    free(tile.p_img_raw);

    printf("Total Tiles=%d\n", tile_set.tile_count);
}



int32_t tile_register_new(tile_data t_tile) {

    int32_t     tile_id;
    tile_data * new_tile;

// printf("tile_register_new %d\n",tile_set.tile_count);

    if (tile_set.tile_count < TILES_MAX_DEFAULT) {

        // Set tile id to the current tile
        tile_id = tile_set.tile_count;

        // Use an easier to read name for the new tile entry
        new_tile = &tile_set.tiles[tile_id];

        // Store hash and encoded image data into tile
        new_tile->hash = t_tile.hash;
        new_tile->encoded_size_bytes = 0;
        new_tile->raw_bytes_per_pixel = t_tile.raw_bytes_per_pixel;
        new_tile->raw_width           = t_tile.raw_width;
        new_tile->raw_height          = t_tile.raw_height;

        // Copy raw tile data into tile image buffer
        new_tile->raw_size_bytes = t_tile.raw_size_bytes;
        new_tile->p_img_raw = malloc(t_tile.raw_size_bytes);

        if (new_tile->p_img_raw) {

            memcpy(new_tile->p_img_raw,
                   t_tile.p_img_raw,
                   t_tile.raw_size_bytes);


            // Copy encoded tile data into tile buffer
            tile_encode(&tile_set.tiles[tile_id],
                        BIN_MODE_SNESGB_2BPP);

            if (tile_set.tiles[tile_id].encoded_size_bytes) {
                // Move to new tile
                tile_set.tile_count++;

            } else // encoding failed
                tile_id = TILE_ID_FAILED_ENCODE;

        } else // malloc failed
            tile_id = TILE_ID_OUT_OF_SPACE;
    }
    else
        tile_id TILE_ID_OUT_OF_SPACE;

printf("tile_register_new tile_id=%d\n",tile_id);

    return (tile_id);
}



// TODO: consider moving this to a separate file (tile_ops.c/h? with copy/hash/etc)
int32_t tile_encode(tile_data * p_tile, uint32_t image_mode) {

    app_gfx_data   app_gfx;
    app_color_data colorpal; // TODO: rename to app_colorpal?
    rom_gfx_data   rom_gfx;

    rom_bin_init_structs(&rom_gfx, &app_gfx, &colorpal);

    // Set Encoding method and Bytes Per Pixel of the incoming tile image
    app_gfx.image_mode = image_mode;

    // Point app gfx info to the tile info and raw image buffer
    app_gfx.bytes_per_pixel = p_tile->raw_bytes_per_pixel;

    app_gfx.width   = tile_map.tile_width;
    app_gfx.height  = tile_map.tile_height;
    app_gfx.size    = tile_set.tile_width * tile_set.tile_height * tile_set.tile_bytes_per_pixel;
    app_gfx.p_data  = p_tile->p_img_raw;




    int32_t tile_y;
    int32_t tile_x;
    uint8_t * t_tile;
    t_tile = app_gfx.p_data;

 printf("Encoding for \n");

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < tile_map.tile_height; tile_y++) {
        for (tile_x = 0; tile_x < tile_map.tile_width; tile_x++)
        {
            printf(" %2x", *(t_tile));

            // Move to the next row
            t_tile += p_tile->raw_bytes_per_pixel;
        }
        printf(" \n");
    }

printf(" \n");




    // Abort if it's not 1 or 2 bytes per pixel
    // TODO: handle both 1 (no alpha) and 2 (has alpha) byte-per-pixel mode
    if (app_gfx.bytes_per_pixel >= BIN_BITDEPTH_LAST)
        return (FALSE);

    // Encode tile data into tile buffer
    rom_bin_encode(&rom_gfx,
                   &app_gfx);


    // Make sure that the write was successful
    if(rom_gfx.size == FALSE) {

        free(rom_gfx.p_data);
        return (FALSE);

    } else {

        // Copy encoded tile
        p_tile->p_img_encoded = malloc(rom_gfx.size);
        memcpy(p_tile->p_img_encoded,
               rom_gfx.p_data,
               rom_gfx.size);

        p_tile->encoded_size_bytes = rom_gfx.size;

        free(rom_gfx.p_data);
    }

    return (TRUE);
}



int32_t tile_find_matching(uint64_t hash_sig) {

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


// TODO: change this so p_tile is of type tile_data and move bytes_per_pixel into tile_data
static void tile_copy_into_buffer(image_data * p_src_img, tile_data tile, uint32_t img_buf_offset) {

    int32_t tile_y;
    int32_t image_width_bytes;
    int32_t tile_width_bytes;

    image_width_bytes = p_src_img->width  * p_src_img->bytes_per_pixel;
    tile_width_bytes  = tile.raw_width    * tile.raw_bytes_per_pixel;

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < tile.raw_height; tile_y++) {

        // Copy a row of tile data into the temp tile buffer
        memcpy(tile.p_img_raw,
               p_src_img->p_img_data + img_buf_offset,
               tile_width_bytes);

        // Move to the next row in image
        img_buf_offset += image_width_bytes;

        // Move to next row in tile buffer
        tile.p_img_raw += tile_width_bytes;
    }
}



// TODO: DEBUG: REMOVE ME
static void tile_print_buffer_raw(tile_data tile) {

    int32_t tile_y;
    int32_t tile_x;

    printf("\n");

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < tile.raw_height; tile_y++) {
        for (tile_x = 0; tile_x < tile.raw_width; tile_x++) {

            printf(" %2x", *(tile.p_img_raw));

            // Move to the next row
            tile.p_img_raw += tile.raw_bytes_per_pixel;
        }
        printf(" \n");
    }

    printf(" \n");
}


static void tile_print_buffer_encoded(tile_data tile) {

    int32_t c;

    printf("ENCODED:\n");

    // Iterate over each tile, top -> bottom, left -> right
    for (c = 0; c < tile.encoded_size_bytes; c++) {
        printf(" %2x", *(tile.p_img_encoded));

            // Move to the next row
            tile.p_img_encoded++;
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

        if (tile_set.tiles[c].p_img_encoded)
            free(tile_set.tiles[c].p_img_encoded);

        if (tile_set.tiles[c].p_img_raw)
            free(tile_set.tiles[c].p_img_raw);
    }

    // Free tile map data
    if (tile_map.p_data)
        free(tile_map.p_data);

}


// ======================= FORMAT EXPORT SUPPORT (tilemap_io.c/h) ============

int32_t tilemap_save(const int8_t * filename) {

    int c;
    FILE * file;

    // Open the file
    file = fopen(filename, "wb");
    if(!file)
        return (false);

    // Write all the tile set data to a file
    for (c = 0; c < tile_set.tile_count; c++) {

printf("* Writing tile %d of %d : %d bytes\n", c +1, tile_set.tile_count, tile_set.tiles[c].encoded_size_bytes);

        if (tile_set.tiles[c].p_img_encoded) {

tile_print_buffer_encoded(tile_set.tiles[c]);

            fwrite(tile_set.tiles[c].p_img_encoded,
                   tile_set.tiles[c].encoded_size_bytes,
                   1, file);
        }

printf("OUTPUT tile %d\n", c);
tile_print_buffer_raw(tile_set.tiles[c]);

            // TODO: hex output encoding
            //if (tile_set.tiles[c].p_img_raw)
    }

    fclose(file);

    return (true);
}