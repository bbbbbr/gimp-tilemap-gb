//
// tilemap_tiles.c
//

#include <stdio.h>
#include <string.h>


#include "lib_tilemap.h"
#include "tilemap_tiles.h"

#include "lib_rom_bin.h"
#include "hash.h"

const uint16_t tile_flip_bits[] = {
    TILE_FLIP_BITS_NONE,
    TILE_FLIP_BITS_X,
    TILE_FLIP_BITS_Y,
    TILE_FLIP_BITS_XY };

void tile_free(tile_data * p_tile) {

    if (p_tile->p_img_raw)
        free(p_tile->p_img_raw);

    p_tile->p_img_raw = NULL;
}


void tile_initialize(tile_data * p_tile, tile_map_data * p_tile_map, tile_set_data * p_tile_set) {

    uint32_t    tile_size_bytes;
    uint32_t    tile_size_bytes_hash_padding; // Make sure hashed data is multiple of 32 bits

    // Use pre-initialized values sourced from tilemap_initialize()
    p_tile->raw_bytes_per_pixel = p_tile_set->tile_bytes_per_pixel;
    p_tile->raw_width           = p_tile_map->tile_width;
    p_tile->raw_height          = p_tile_map->tile_height;
    p_tile->raw_size_bytes      = p_tile->raw_height * p_tile->raw_width * p_tile->raw_bytes_per_pixel;

    tile_size_bytes = p_tile->raw_size_bytes;

    // Make sure buffer is an even multiple of 32 bits (for hash function)
    tile_size_bytes_hash_padding = tile_size_bytes % sizeof(uint32_t);

    // Allocate buffer for temporary working tile raw image, 32 bit aligned
    p_tile->p_img_raw = aligned_alloc(sizeof(uint32_t), (tile_size_bytes + tile_size_bytes_hash_padding));

    // Make sure padding bytes are zeroed
    memset(p_tile->p_img_raw, 0x00, tile_size_bytes + tile_size_bytes_hash_padding);
}


tile_map_entry tile_register_new(tile_data * p_src_tile, tile_set_data * tile_set) {

    int             h;
    tile_map_entry  new_map_entry;
    tile_data     * new_tile;

// printf("tile_register_new %d\n",tile_set->tile_count);

    if (tile_set->tile_count < TILES_MAX_DEFAULT) {

        // Set tile id to the current tile
        new_map_entry.id = tile_set->tile_count;

        // Default: no attributes (no flip x/y, no special palette)
        new_map_entry.attribs = 0;

        // Use an easier to read name for the new tile entry
        new_tile = &tile_set->tiles[new_map_entry.id];

        // Store hash and encoded image data into tile
        for (h = TILE_FLIP_MIN; h <= TILE_FLIP_MAX; h++)
            new_tile->hash[h] = p_src_tile->hash[h];

        new_tile->encoded_size_bytes = 0;
        new_tile->raw_bytes_per_pixel = p_src_tile->raw_bytes_per_pixel;
        new_tile->raw_width           = p_src_tile->raw_width;
        new_tile->raw_height          = p_src_tile->raw_height;

        // Copy raw tile data into tile image buffer
        new_tile->raw_size_bytes = p_src_tile->raw_size_bytes;
        new_tile->p_img_raw      = malloc(p_src_tile->raw_size_bytes);

        if (new_tile->p_img_raw) {

            memcpy(new_tile->p_img_raw,
                   p_src_tile->p_img_raw,
                   p_src_tile->raw_size_bytes);


            // Copy encoded tile data into tile buffer
            tile_encode(&(tile_set->tiles[new_map_entry.id]),
                        BIN_MODE_SNESGB_2BPP);

            if (tile_set->tiles[new_map_entry.id].encoded_size_bytes) {
                // Move to new tile
                tile_set->tile_count++;

            } else // encoding failed
                new_map_entry.id = TILE_ID_FAILED_ENCODE;

        } else // malloc failed
            new_map_entry.id = TILE_ID_OUT_OF_SPACE;
    }
    else
        new_map_entry.id TILE_ID_OUT_OF_SPACE;

// printf("tile_register_new tile_id=%d\n",tile_id);

    return (new_map_entry);
}



int32_t tile_encode(tile_data * p_tile, uint32_t image_mode) {

    app_gfx_data   app_gfx;
    app_color_data colorpal; // TODO: rename to app_colorpal?
    rom_gfx_data   rom_gfx;

    rom_bin_init_structs(&rom_gfx, &app_gfx, &colorpal);

    // Set Encoding method and Bytes Per Pixel of the incoming tile image
    app_gfx.image_mode = image_mode;

    // Point app gfx info to the tile info and raw image buffer
    app_gfx.bytes_per_pixel = p_tile->raw_bytes_per_pixel;

    app_gfx.width   = p_tile->raw_width;
    app_gfx.height  = p_tile->raw_height;
    app_gfx.size    = p_tile->raw_width * p_tile->raw_height * p_tile->raw_bytes_per_pixel;
    app_gfx.p_data  = p_tile->p_img_raw;


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



tile_map_entry tile_find_match(uint64_t hash_sig, tile_set_data * tile_set, uint16_t search_mask) {

    int c;
    int h, h_range;
    tile_map_entry tile_match_rec;

    // TODO: for now flip X and flip Y are joined together, so always check each permutation if either is turned on
    if (!search_mask) h_range = TILE_FLIP_MIN;
    else h_range = TILE_FLIP_MAX;

    // Loop through all tiles in the set
    for (c = 0; c < tile_set->tile_count; c++) {
        // Loop through all hashes that are present
        for (h = TILE_FLIP_MIN; h <= h_range; h++) {
            // If a mash matches then return it (along with flip attributes_
            if (hash_sig == tile_set->tiles[c].hash[h]) {

                tile_match_rec.id       = c; // found a matching tile, return it's ID
                tile_match_rec.attribs  = tile_flip_bits[h]; // Set flip x/y bits if present

                if (h == 3)
                    printf("Tilemap: Search: Flip: Found at %d -> %d\n", c, h);
                return(tile_match_rec);
            }
        }
    }

    // No matching tile found
    tile_match_rec.id = TILE_ID_NOT_FOUND;
    return(tile_match_rec);
}


void tile_copy_tile_from_image(image_data * p_src_img,
                              tile_data * p_tile,
                            uint32_t img_buf_offset) {

    int32_t tile_y;
    int32_t tile_img_offset;
    int32_t image_width_bytes;
    int32_t tile_width_bytes;

    tile_img_offset   = 0;
    image_width_bytes = p_src_img->width  * p_src_img->bytes_per_pixel;
    tile_width_bytes  = p_tile->raw_width * p_tile->raw_bytes_per_pixel;

    if (!p_tile->p_img_raw)
        return;

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < p_tile->raw_height; tile_y++) {

        // Copy a row of tile data into the temp tile buffer
        memcpy(p_tile->p_img_raw + tile_img_offset,
               p_src_img->p_img_data + img_buf_offset,
               tile_width_bytes);

        // Move to the next row in image
        img_buf_offset += image_width_bytes;

        // Move to next row in tile buffer
        tile_img_offset += tile_width_bytes;
    }
}



// TODO: DEBUG: REMOVE ME
void tile_print_buffer_raw(tile_data tile) {

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


void tile_print_buffer_encoded(tile_data tile) {

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




void tile_flip_y(tile_data * p_src_tile, tile_data * p_dst_tile) {

    uint16_t  y;
    uint8_t * p_src_top;
    uint8_t * p_dst_bottom;
    uint16_t  row_stride;

    row_stride = (p_src_tile->raw_width * p_src_tile->raw_bytes_per_pixel);

    // Set up pointers to opposite top/bottom rows of image
    // Start of First row / Start of Last row
    p_src_top    = p_src_tile->p_img_raw;
    p_dst_bottom = p_dst_tile->p_img_raw + ((p_src_tile->raw_height - 1) * row_stride);

    // Copy Source rows from top to bottom into Dest from bottom to top
    for (y = 0; y < p_src_tile->raw_height; y++) {
        memcpy(p_dst_bottom, p_src_top, row_stride);
        p_src_top    += row_stride;
        p_dst_bottom -= row_stride;
    }
}


void tile_flip_x(tile_data * p_src_tile, tile_data * p_dst_tile) {

    uint16_t  x, y;
    uint8_t * p_src_left;
    uint8_t * p_dst_right;
    uint8_t   bpp;
    uint16_t  dest_row_increment, dest_pixel_increment;

    // Set up pointers to opposite sides of the first line
    bpp = p_src_tile->raw_bytes_per_pixel;

    p_src_left  = p_src_tile->p_img_raw;
    p_dst_right = p_dst_tile->p_img_raw + ((p_src_tile->raw_width - 1) * bpp);

    // * Source will end up where it needs to
    //   be automatically for the next row
    // * Dest needs to be advanced by two rows
    dest_row_increment   = (p_src_tile->raw_width * 2) * bpp;
    dest_pixel_increment = (bpp * 2) - 1;

    for (y = 0; y < p_src_tile->raw_height; y++) {

        for (x = 0; x < p_src_tile->raw_width; x++) {

            // Copy row contents in reverse order
            // Source increments (right) and Dest decrements_left
            switch (bpp) {
                //case 1: *p_dst_right-- = *p_src_left++;
                //         break;

                // For each higher bit depth, copy one more pixel
                case 4: *p_dst_right++ = *p_src_left++;
                case 3: *p_dst_right++ = *p_src_left++;
                case 2: *p_dst_right++ = *p_src_left++;

                case 1: *p_dst_right   = *p_src_left;
                        // Finished copying pixels
                        // Rewind Dest buffer and advance source
                         p_dst_right -= dest_pixel_increment;
                         p_src_left++;
                        break;
            }
        }

        p_dst_right += dest_row_increment;
    }
}


void tile_calc_alternate_hashes(tile_data * p_tile, tile_data flip_tiles[]) {

    //        if (mask_test & tile_map.search_mask) {

    // TODO: for now flip X and flip Y are joined together, so always check each permutation

    // Check for X flip (new copy of data)
    tile_flip_x(p_tile, &flip_tiles[0]);
    p_tile->hash[1] = MurmurHash2( flip_tiles[0].p_img_raw, flip_tiles[0].raw_size_bytes, 0xF0A5); // len is u8count

    // Check for Y flip (new copy of data)
    tile_flip_y(p_tile, &flip_tiles[0]);
    p_tile->hash[2] = MurmurHash2( flip_tiles[0].p_img_raw, flip_tiles[0].raw_size_bytes, 0xF0A5); // len is u8count

    // Check for X-Y flip (re-use data from previous Y flip -> second flip tile)
    tile_flip_x(&flip_tiles[0], &flip_tiles[1]);
    p_tile->hash[3] = MurmurHash2( flip_tiles[1].p_img_raw, flip_tiles[1].raw_size_bytes, 0xF0A5); // len is u8count

    memcpy(p_tile->p_img_raw, flip_tiles[1].p_img_raw, flip_tiles[1].raw_size_bytes);
}


