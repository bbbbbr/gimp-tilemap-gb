//
// tilemap_tiles.c
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "win_aligned_alloc.h" // Needed for aligned_alloc cross compile

#include "lib_tilemap.h"
#include "tilemap_tiles.h"

#include "lib_rom_bin.h"
#include "hash.h"

#include "options.h"

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
    p_tile->palette_num         = TILE_PAL_DEFAULT;

    tile_size_bytes = p_tile->raw_size_bytes;

    // Make sure buffer is an even multiple of 32 bits (for hash function)
    tile_size_bytes_hash_padding = tile_size_bytes % sizeof(uint32_t);

    // Allocate buffer for temporary working tile raw image, 32 bit aligned
    // Note: requires: #include "win_aligned_alloc.h" // Needed for aligned_alloc cross compile
    p_tile->p_img_raw = aligned_alloc(sizeof(uint32_t), (tile_size_bytes + tile_size_bytes_hash_padding));

    // Make sure padding bytes are zeroed
    memset(p_tile->p_img_raw, 0x00, tile_size_bytes + tile_size_bytes_hash_padding);
}


tile_map_entry tile_register_new(tile_data * p_src_tile, tile_set_data * tile_set) {

    int             h;
    tile_map_entry  new_map_entry;
    tile_data     * new_tile;

    // Default status to found
    new_map_entry.status = TILE_ID_OK;

// printf("tile_register_new %d\n",tile_set->tile_count);

    if (tile_set->tile_count < TILES_MAX_DEFAULT) {

        // == FIRST INITIALIZE THE TILE'S MAP ENTRY ==
        // Set tile id to the current tile
        new_map_entry.id = tile_set->tile_count;

        // Default: no attributes (no flip x/y, no special palette)
        new_map_entry.flip_bits   = TILE_FLIP_BITS_NONE;
        new_map_entry.palette_num = TILE_PAL_MAP_USE_DEFAULT_FROM_TILE;


        // == THEN COPY THE TILE DATA INTO THE NEW TILE ==
        // Use an easier to read name for the new tile entry
        new_tile = &tile_set->tiles[new_map_entry.id];

        // Store hash and encoded image data into tile
        for (h = TILE_FLIP_MIN; h <= TILE_FLIP_MAX; h++)
            new_tile->hash[h] = p_src_tile->hash[h];

        new_tile->encoded_size_bytes = 0;
        new_tile->raw_bytes_per_pixel = p_src_tile->raw_bytes_per_pixel;
        new_tile->raw_width           = p_src_tile->raw_width;
        new_tile->raw_height          = p_src_tile->raw_height;
        new_tile->palette_num         = p_src_tile->palette_num;


        // Copy raw tile data into tile image buffer
        new_tile->raw_size_bytes = p_src_tile->raw_size_bytes;
        new_tile->p_img_raw      = malloc(p_src_tile->raw_size_bytes);

        // Only proceed if the alloc worked
        if (new_tile->p_img_raw) {

            // Copy data from source to new tile
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
                new_map_entry.status = TILE_ID_FAILED_ENCODE;

        } else // malloc failed
            new_map_entry.status = TILE_ID_OUT_OF_SPACE;
    }
    else
        new_map_entry.status = TILE_ID_EXCEEDED_LIMIT;

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



tile_map_entry tile_find_match(tile_data * p_tile, tile_set_data * tile_set, tile_map_data * p_tile_map) {

    int c;
    int h, h_range;
    tile_map_entry tile_match_rec;

    // Default status to found
    tile_match_rec.status = TILE_ID_OK;

    // Earlier checking will enforce CGB mode only for tile_dedupe_flips
    // TODO: for now flip X and flip Y are joined together, so always check each flip permutation
    if (p_tile_map->options.tile_dedupe_flips)
        h_range = TILE_FLIP_MAX;
    else
        h_range = TILE_FLIP_MIN;

    // Loop through all tiles in the set
    for (c = 0; c < tile_set->tile_count; c++) {

        // Loop through all hashes that are present
        for (h = TILE_FLIP_MIN; h <= h_range; h++) {
            // If a hash matches then return it (along with flip attributes)
            if (p_tile->hash[0] == tile_set->tiles[c].hash[h]) {

                // Either palette must match, or dedupe tiles
                // based on palettes must be enabled (CGB Mode only)
                if ((p_tile->palette_num == tile_set->tiles[c].palette_num)
                    || (p_tile_map->options.tile_dedupe_palettes)) {

                    tile_match_rec.id        = c; // found a matching tile, return it's ID
                    tile_match_rec.flip_bits = tile_flip_bits[h]; // Set flip x/y bits if present

                    // If the palettes didn't match
                    // Enable the palette override by setting palette number
                    if (p_tile->palette_num != tile_set->tiles[c].palette_num) {
                        // printf(" -> Dedupe  Pal on tile.pal %d != tile_set(%d).hash(%d).pal %d ", p_tile->palette_num, c, h, tile_set->tiles[c].palette_num);

                        // Use this map location's identified (tile) palette as an override instead of the source tile's palette
                        tile_match_rec.palette_num = p_tile->palette_num;
                    } else {
                        // Otherwise set the flag to use the default palette from the source tile
                        // printf(" -> Default Pal on tile.pal %d != tile_set(%d).hash(%d).pal %d ", p_tile->palette_num, c, h, tile_set->tiles[c].palette_num);
                        tile_match_rec.palette_num = TILE_PAL_MAP_USE_DEFAULT_FROM_TILE;
                    }

                    return(tile_match_rec);

                } // Palette test

            } // Hash test

        } // Hash flip permutations loop

    } // Tile set loop

    // No matching tile found
    tile_match_rec.status = TILE_ID_NOT_FOUND;
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
}

// See tile_palette_identify_and_strip()
// palette_id is 0 (DMG) or 0..7 (CGB)
void tile_palette_reapply_offsets(tile_data * p_tile) {

    int32_t tile_y;
    int32_t tile_x;

    uint8_t * indexed_pixel_data;

    // Set up pointer to indexed pixel data
    indexed_pixel_data = p_tile->p_img_raw;

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < p_tile->raw_height; tile_y++) {
        for (tile_x = 0; tile_x < p_tile->raw_width; tile_x++) {

            // Convert the palette from 0-3 colors back to 0..3 or 0..31
            (*indexed_pixel_data) += (TILE_COLORS_PER_PALETTE * p_tile->palette_num);

            // Move to the next pixel
            indexed_pixel_data += p_tile->raw_bytes_per_pixel;

        } // End: for (tile_x
    } // End: for (tile_y ...
}


// Identifies and strips the color palette off during the first pass.
//
// Required for performing hash based de-duplication of tiles without their palette
//
// TODO: CGB MODE: this could be avoided by having the hash operate on (pixel % 4) instead of just pixel
//
// It then needs to get re-applied later since map processing produces
// a bitmapped image of the tile set (with possibly deduped colors)
// which will then have it's palette stripped one last time on export
int32_t tile_palette_identify_and_strip(tile_data * p_tile, uint16_t gb_mode) {

    int32_t tile_y;
    int32_t tile_x;

    uint8_t palette;
    uint8_t last_palette;

    uint8_t * indexed_pixel_data;

    // Set up pointer to indexed pixel data
    indexed_pixel_data = p_tile->p_img_raw;

    // Prime the palette testing in the loop
    last_palette = (*indexed_pixel_data) / TILE_COLORS_PER_PALETTE;

    // Iterate over each tile, top -> bottom, left -> right
    for (tile_y = 0; tile_y < p_tile->raw_height; tile_y++) {
        for (tile_x = 0; tile_x < p_tile->raw_width; tile_x++) {

            // Identify which color palette is used for the tile based
            // on the color's location in the indexed palette.
            // Palette is zero based
            // Example: Index color 13 = (13 / 4) = Palette 3
            palette = (*indexed_pixel_data) / TILE_COLORS_PER_PALETTE;

            // Return error and abort if...
            // * extracted palette num was out of bounds or
            // * the tile tried to use more than one palette
            if (palette > TILE_PAL_MAX) {

                return (TILE_ID_TOO_MANY_COLORS);

            } else if (palette != last_palette) {

                return (TILE_ID_MULTIPLE_PALETTES_IN_TILE);

            } else {
                // Update palette for next pass
                palette = last_palette;
            }

            // Remap the palette so it's only colors 0-3, relative to the identified palette
            // Example: Index color 13 = (13 % 4) = Color 1 of Palette 3
            *indexed_pixel_data = (*indexed_pixel_data) % TILE_COLORS_PER_PALETTE;

            // Move to the next pixel
            indexed_pixel_data += p_tile->raw_bytes_per_pixel;

        } // End: for (tile_x
    } // End: for (tile_y ...

    // Check for Palette out of range
    if ((palette > TILE_PAL_DMG_MAX) && (gb_mode == MODE_DMG_4_COLOR)) {
        return (TILE_ID_TOO_MANY_COLORS);

    } else if ((palette > TILE_PAL_CGB_MAX) && (gb_mode == MODE_CGB_32_COLOR)) {
        return (TILE_ID_TOO_MANY_COLORS);
    }


// printf("-> pal id as: %d :", palette);
    p_tile->palette_num = palette;

    return TILE_ID_OK;
}
