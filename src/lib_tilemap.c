//
// lib_tilemap.c
//
#include <stdio.h>
#include <string.h>

#include "logging.h"

#include "lib_tilemap.h"
#include "tilemap_tiles.h"
#include "tilemap_io.h"
#include "tilemap_error.h"

#include "hash.h"
#include "options.h"


// Globals
tile_map_data tile_map;
tile_set_data tile_set;
color_data  * p_colors = NULL;


// Call after initial user options have been set, but before remapping any colors
void tilemap_image_and_colors_init(image_data * p_img, color_data * p_colors) {

    for (int c; c < COLOR_DATA_PAL_SIZE; c++)
        p_colors->pal[c] = 0; // default is black for all colors/components

    p_colors->subpal_size = 4;

    p_img->p_img_data = NULL;

    p_img->tile_width = OPTION_UNSET;
    p_img->tile_height = OPTION_UNSET;

    p_img->palette_tile_width = OPTION_UNSET;
    p_img->palette_tile_height = OPTION_UNSET;

}


// Call after initial user options have been set, but before trying to remap colors
void tilemap_image_set_palette_tile_size(image_data * p_img, tile_process_options * p_options) {

    if (p_options->tile_width != OPTION_UNSET)
        p_img->palette_tile_width = p_options->tile_width;
    if (p_options->tile_height != OPTION_UNSET)
        p_img->palette_tile_height = p_options->tile_height;
}


// Call after the source image is loaded and user options have been applied via tilemap_options_set()
// Sets image tile width and height, returns error if invalid
bool tilemap_image_update_settings(image_data * p_img, color_data * p_colors) {

    if ((tile_map.options.tile_width != OPTION_UNSET) &&
        (tile_map.options.tile_height != OPTION_UNSET)) {
        // Load in user defaults
        p_img->tile_width = tile_map.options.tile_width;
        p_img->tile_height = tile_map.options.tile_height;
    }
    else {

        // Only GBR formats support tile sizes larger than 8x8...?
        // TODO - what about C source for sprites?
        if  ((tile_map.options.image_format == FORMAT_GBR) ||
             (tile_map.options.image_format == FORMAT_GBDK_C_SOURCE)) {

            if ((p_img->width == 32) && ((p_img->height % 32) != 0)) {
                // 32x32 tiles
                p_img->tile_width = 32;
                p_img->tile_height = 32;
            }
            if ((p_img->width == 16) && ((p_img->height % 16) != 0)) {
                // 16x16 tiles
                p_img->tile_width = 16;
                p_img->tile_height = 16;
            }
            else if ((p_img->width % 8) == 0) {
                // 8x8 tiles (default when possible)
                p_img->tile_width  = 8;
                p_img->tile_height = 8;
            } 
            else {
                // Invalid image size (not a multiple of 8)
                return false;
            }
        }
        // GBM format defaults to 8x8
        else if ((p_img->width % 8) == 0) {
            // 8x8 tiles (default for maps)
            p_img->tile_width  = 8;
            p_img->tile_height = 8;
        }
        else {
            // Invalid image size (not a multiple of 8)
            return false;
        }
    }

    if ( ((p_img->width  % p_img->tile_width ) != 0) ||
         ((p_img->height % p_img->tile_height) != 0) ) {

        // Invalid image size (not a multiple of tile size)
        return false;
    }

    log_verbose("tilemap_image_update_settings() tile size (%d x %d), image size (%d x %d) \n",
        p_img->tile_width, p_img->tile_height,
        p_img->width, p_img->height);

    return true;
}


void tilemap_options_get(tile_process_options * p_options) {

    memcpy(p_options, &tile_map.options, sizeof(tile_process_options));

    options_log("tilemap_options_get() ", p_options);
}


void tilemap_options_set(tile_process_options * p_options) {

    memcpy(&tile_map.options, p_options, sizeof(tile_process_options));
}



// TODO: support configurable tile size
int tilemap_initialize(image_data * p_src_img) {

    // Tile Map
    tile_map.map_width   = p_src_img->width;
    tile_map.map_height  = p_src_img->height;

    tile_map.tile_width  = p_src_img->tile_width;
    tile_map.tile_height = p_src_img->tile_height;

    tile_map.width_in_tiles  = tile_map.map_width  / tile_map.tile_width;
    tile_map.height_in_tiles = tile_map.map_height / tile_map.tile_height;

    // Max space required to store Tile Map is
    // width x height in tiles (if every map tile is unique)
    tile_map.size = (tile_map.width_in_tiles * tile_map.height_in_tiles);

    // See struct: tile_map_entry
    // if TILES_MAX_DEFAULT > 255, tile_id_list must be larger than uint8_t (it is)
    tile_map.tile_id_list = (uint16_t *)malloc(tile_map.size * sizeof(uint16_t));
    if (!tile_map.tile_id_list)
            return(false);

    tile_map.flip_bits_list = (uint8_t *)malloc(tile_map.size * sizeof(uint8_t));
    if (!tile_map.flip_bits_list)
            return(false);

    tile_map.palette_num_list = (uint8_t *)malloc(tile_map.size * sizeof(uint8_t));
    if (!tile_map.palette_num_list)
            return(false);

    tile_map.cgb_attrib_list = (uint8_t *)malloc(tile_map.size * sizeof(uint8_t));
    if (!tile_map.cgb_attrib_list)
            return(false);


    // Tile Set
    tile_set.tile_bytes_per_pixel = p_src_img->bytes_per_pixel;
    tile_set.tile_width  = p_src_img->tile_width;
    tile_set.tile_height = p_src_img->tile_height;
    tile_set.tile_size   = tile_set.tile_width * tile_set.tile_height * tile_set.tile_bytes_per_pixel;
    tile_set.tile_count  = 0;

    // Processing Options
    // tile_map.options = plugin_options; // These get set earlier via the tilemap_options... functions

    return (true);
}



unsigned char tilemap_export_process(image_data * p_src_img, color_data * p_src_colors) {

    if ( check_dimensions_valid(p_src_img) ) {
        if ( ! tilemap_initialize(p_src_img)) { // Success, prep for processing

            tilemap_error_set(TILE_ID_OUT_OF_SPACE);
            return (false); // Signal failure and exit
        }
    }
    else {
        tilemap_error_set(TILE_ID_INVALID_DIMENSIONS);
        return (false); // Signal failure and exit
    }

    if ( ! process_tiles(p_src_img) )
        return (false); // Signal failure and exit

    // Save color palette
    p_colors = p_src_colors;

    return (true);
}



unsigned char process_tiles(image_data * p_src_img) {

    int             img_x, img_y;

    tile_data       tile, flip_tiles[2];
    tile_map_entry  map_entry;
    uint32_t        img_buf_offset;
    int32_t         map_slot;

    map_slot = 0;

    // Use pre-initialized values from tilemap_initialize()
    tile_initialize(&tile, &tile_map, &tile_set);
    tile_initialize(&flip_tiles[0], &tile_map, &tile_set);
    tile_initialize(&flip_tiles[1], &tile_map, &tile_set);

    if (tile.p_img_raw) {

        // Iterate over the map, top -> bottom, left -> right
        img_buf_offset = 0;

        log_verbose("process_tiles(): %d x %d: tiles %d x %d\n", tile_map.map_width, tile_map.map_height, tile_map.tile_width, tile_map.tile_height);

        for (img_y = 0; img_y < tile_map.map_height; img_y += tile_map.tile_height) {
            for (img_x = 0; img_x < tile_map.map_width; img_x += tile_map.tile_width) {

                // log_verbose(" %4d,%4d :", img_x, img_y);

                // Set buffer offset to upper left of current tile
                img_buf_offset = (img_x + (img_y * tile_map.map_width)) * p_src_img->bytes_per_pixel;

                tile_copy_tile_from_image(p_src_img,
                                          &tile,
                                          img_buf_offset);


                // Extract tile color palette and remap indexed colors to be 0..3 based relative to the palette
                //
                // NOTE: This needs to happen *BEFORE* any deduplication hashing
                //       The palette also gets re-applied below
                map_entry.status = tile_palette_identify_and_strip(&tile,
                                                                   tile_map.options.gb_mode, tile_map.options.ignore_palette_errors);
                if (map_entry.status != TILE_ID_OK) {
                    // log_verbose("Tilemap: Process: FAIL -> tile_palette_identify_and_strip = Invalid Palette\n");
                    tilemap_error_set(map_entry.status);
                    return (false); // Exit
                }


                // TODO! Don't hash transparent pixels? Have to overwrite second byte?
                tile.hash[0] = MurmurHash2( tile.p_img_raw, tile.raw_size_bytes, 0xF0A5); // len is u8count, 0xF0A5 is seed

                if (tile_map.options.tile_dedupe_enabled)
                    map_entry = tile_find_match(&tile, &tile_set,
                                                tile_map.options.tile_dedupe_flips, tile_map.options.tile_dedupe_palettes);
                else
                    map_entry.status = TILE_ID_NOT_FOUND;


                // Tile not found, create a new entry
                if (map_entry.status == TILE_ID_NOT_FOUND) {

                    // Calculate remaining hash flip variations
                    // (only for tiles that get registered)
                    if (tile_map.options.tile_dedupe_flips)
                        tile_calc_alternate_hashes(&tile, flip_tiles);

                    // Re-apply the Gameboy palette offset for indexed color
                    // NOTE: This needs to happen *AFTER* tile_calc_alternate_hashes and *BEFORE* tile_register_new
                    tile_palette_reapply_offsets(&tile);

                    map_entry = tile_register_new(&tile, &tile_set);

                    if (map_entry.status != TILE_ID_OK) {
                        tile_free(&tile);
                        tile_free(&flip_tiles[0]);
                        tile_free(&flip_tiles[1]);
                        tilemap_free_resources();

                        // TODO: propegate errors upward
                        log_verbose("Tilemap: Process: FAIL -> Too Many Tiles\n");
                        tilemap_error_set(map_entry.status);
                        return (false); // Ran out of tile space, exit
                    }
                    // log_verbose(" -> NEW tile %d ", map_entry.id);
                }
                // else log_verbose(" -> use tile %d ", map_entry.id);

                tile_map.tile_id_list[map_slot]     = map_entry.id;
                tile_map.flip_bits_list[map_slot]   = map_entry.flip_bits;
                tile_map.palette_num_list[map_slot] = map_entry.palette_num;
                tile_map.cgb_attrib_list[map_slot]  = map_entry.cgb_attrib;

                map_slot++;

                // log_verbose("\n");
            } // for (img_x = 0
        } // for (img_y = 0

    } else { // else if (tile.p_img_raw) {
        tilemap_error_set(TILE_ID_OUT_OF_SPACE);
        tilemap_free_resources();
        return (false); // Failed to allocate buffer, exit
    }

    // Free resources
    tile_free(&tile);
    tile_free(&flip_tiles[0]);
    tile_free(&flip_tiles[1]);

    log_standard("gbm output: total tiles = %d\n", tile_set.tile_count);

    return true;
}


int32_t check_dimensions_valid(image_data * p_src_img) {

    // Image dimensions must be exact multiples of tile size
    if ( ((p_src_img->width % p_src_img->tile_width) != 0) ||
         ((p_src_img->height % p_src_img->tile_height) != 0)) {
        return false; // Fail
    } else {
        return true;  // Success
    }
}




void tilemap_free_tile_set(void) {
        int c;

    // Free all the tile set data
    for (c = 0; c < tile_set.tile_count; c++) {

        if (tile_set.tiles[c].p_img_encoded)
            free(tile_set.tiles[c].p_img_encoded);
        tile_set.tiles[c].p_img_encoded = NULL;

        if (tile_set.tiles[c].p_img_raw)
            free(tile_set.tiles[c].p_img_raw);
        tile_set.tiles[c].p_img_raw = NULL;
    }

    tile_set.tile_count  = 0;
}

void tilemap_free_resources(void) {

    tilemap_free_tile_set();

    // Free tile map data
    if (tile_map.tile_id_list) {
        free(tile_map.tile_id_list);
        tile_map.tile_id_list = NULL;
    }

    if (tile_map.flip_bits_list) {
        free(tile_map.flip_bits_list);
        tile_map.flip_bits_list = NULL;
    }

    if (tile_map.palette_num_list) {
        free(tile_map.palette_num_list);
        tile_map.palette_num_list = NULL;
    }

    if (tile_map.cgb_attrib_list) {
        free(tile_map.cgb_attrib_list);
        tile_map.cgb_attrib_list = NULL;
    }
}



int32_t tilemap_save(const char * filename, uint32_t export_format) {

    return( tilemap_export(filename,
                           export_format,
                           &tile_map,
                           &tile_set,
                           p_colors) );
}



tile_map_data * tilemap_get_map(void) {
    return (&tile_map);
}



tile_set_data * tilemap_get_tile_set(void) {
    return (&tile_set);
}



// TODO: Consider moving this to a different location
//
// Returns an image which is a composite of all the
// tiles in a tile map, in order.
int32_t tilemap_get_image_of_deduped_tile_set(image_data * p_img) {

    uint32_t c;
    uint32_t img_offset;

    // Set up image to store deduplicated tile set
    p_img->width  = tile_map.tile_width;
    p_img->height = tile_map.tile_height * tile_set.tile_count;
    p_img->tile_width  = tile_map.tile_width;
    p_img->tile_height = tile_map.tile_height;
    p_img->size   = tile_set.tile_size   * tile_set.tile_count;
    p_img->bytes_per_pixel = tile_set.tile_bytes_per_pixel;

    log_verbose("== Copy tiles into composite buf %d x %d, total size=%d, Tils size %d x %d\n", p_img->width, p_img->height, p_img->size,  p_img->tile_width,  p_img->tile_height);

    // Allocate a buffer for the image
    p_img->p_img_data = (uint8_t *)malloc(p_img->size);

    if (p_img->p_img_data) {

        img_offset = 0;

        for (c = 0; c < tile_set.tile_count; c++) {

            if (tile_set.tiles[c].p_img_raw) {
                // Copy from the tile's raw image buffer (indexed)
                // into the composite image

                //tile_print_buffer_raw(tile_set.tiles[c]); // TODO: remove

                memcpy(p_img->p_img_data + img_offset,
                       tile_set.tiles[c].p_img_raw,
                       tile_set.tile_size);
            }
            else
                return false;

            img_offset += tile_set.tile_size;
        }
    }
    else
        return false;

    log_verbose("== TILEMAP -> IMG COPIED BUFF\n");

    // // Iterate over each tile, top -> bottom, left -> right
    // for (c = 0; c < p_img->size; c++) {
    //     log_verbose(" %2x", *(p_img->p_img_data + c));
    //     if ((c % 8) ==0) log_verbose("\n");
    // }
    // log_verbose(" \n");

    return true;
}
