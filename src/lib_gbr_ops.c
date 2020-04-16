//
// lib_gbr_ops.c
//

#include "lib_gbr_ops.h"
#include "lib_gbm_ops.h"

#include "options.h"


// TODO: THIS IS DEPRECATED?
// TODO: Bounds checking! max_size
int32_t gbr_pal_get_buf(uint8_t * dest_buf, gbr_record * p_gbr) {

    uint16_t index;
    uint32_t offset;

    // TODO: use a #define here
    if ((p_gbr->palettes.count * COLOR_DATA_BYTES_PER_COLOR) > (COLOR_DATA_PAL_SIZE))
        return false;

    offset = 0;

    for (index = 0; index < p_gbr->palettes.count; index++) {

        // gbr palette format is in u32 XBGR format
        *(dest_buf++) = p_gbr->palettes.colors[offset + 0]; // R.3
        *(dest_buf++) = p_gbr->palettes.colors[offset + 1]; // G.2
        *(dest_buf++) = p_gbr->palettes.colors[offset + 2]; // B.1

        printf(" --COLORS %d: %d) %2x, %2x, %2x, \n", index, offset,
                                                p_gbr->palettes.colors[offset + 0],
                                                p_gbr->palettes.colors[offset + 1],
                                                p_gbr->palettes.colors[offset + 2]);

        offset += GBR_PALETTE_TCOLOR_SIZE;
    }

    return true;
}


// TODO: THIS IS DEPRECATED?
// TODO: Bounds checking! max_size
int32_t gbr_pal_set_buf(uint8_t * src_buf, gbr_record * p_gbr, uint16_t num_colors) {

    uint16_t index;
    uint32_t offset;

    // TODO: use a #define here
    if ((p_gbr->palettes.count * COLOR_DATA_BYTES_PER_COLOR) > COLOR_DATA_PAL_SIZE)
        return false;

    p_gbr->palettes.count = num_colors;

    offset = 0;

    for (index = 0; index < p_gbr->palettes.count; index++) {

        // gbr palette format is in u32 XBGR format
        p_gbr->palettes.colors[offset + 0] = *(src_buf++); // R.3
        p_gbr->palettes.colors[offset + 1] = *(src_buf++); // G.2
        p_gbr->palettes.colors[offset + 2] = *(src_buf++); // B.1

        printf("EXPORT --COLORS %d: %d) %2x, %2x, %2x, \n", index, offset,
                                                p_gbr->palettes.colors[offset + 0],
                                                p_gbr->palettes.colors[offset + 1],
                                                p_gbr->palettes.colors[offset + 2]);

        offset += GBR_PALETTE_TCOLOR_SIZE;
    }

    return true;
}


// WARNING: assumes bytes per pixel = 1 in working buffer
//
// Handle Horizontal row flipping
void gbr_tile_row_mirror_horizontal( uint8_t tile_row[], int16_t row_width) {

    int16_t c;
    uint8_t temp_pixel;
    for (c=0; c < (row_width / 2); c++) {

        // Loop through the line and swap mirrored pixels
        temp_pixel = tile_row[c];
        tile_row[c] = tile_row[ (row_width - c - 1) ];
        tile_row[ (row_width - c - 1) ] = temp_pixel;
    }
}


// WARNING: assumes bytes per pixel = 1 in DEST buffer
//
// TODO: could just pass in pal_index instead of deciding whether to use map tile palette override logic in this function
void gbr_tile_remap_colors(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index, int32_t tile_size, uint8_t map_tile_pal_id) {

    int32_t index;
    uint8_t tile_pal_offset;

    // Choose which color palette to use:
    // * Tile's built-in palette ID, or the Map's per-tile optional override palette ID
    if (map_tile_pal_id == GBR_MAP_TILE_PAL_OVERRIDE_NONE) {

        // CGB color palette loading
        // Remap tile colors if there are multiple palettes
        // Look up palette for tile, then calculate it's 4-bytes-per-palette offset
        tile_pal_offset = (p_gbr->tile_pal.color_set[ (tile_index * GBR_TILE_PAL_COLOR_SET_REC_SIZE) ])
                          * GBR_TILE_DATA_COLOR_SET_SIZE;
    } else {
        // Use pal override from map (with offset for palettes upshifted to accomodate "default" pal setting
        tile_pal_offset = (map_tile_pal_id - GBR_MAP_TILE_PAL_OFFSET)  * GBR_TILE_DATA_COLOR_SET_SIZE;
    }

//    printf("gbr: gbr_tile_remap_colors: tile_id=%d tile_pal=%d map_tile_pal=%d (0=use tile default)\n",tile_index, p_gbr->tile_pal.color_set[ (tile_index * GBR_TILE_PAL_COLOR_SET_REC_SIZE) ], map_tile_pal_id);

    // Now remap the palette
    for (index = 0; index < tile_size; index++) {
        // Remap the tile color to the correct palette index
       (*dest_buf) +=  tile_pal_offset;
       dest_buf++;

    }
}


// WARNING: assumes bytes per pixel = 1 in DEST buffer
int32_t gbr_tile_palette_assign_and_strip(uint8_t * p_buf, gbr_record * p_gbr, uint16_t tile_index, int32_t tile_size, uint16_t gb_mode) {

    int32_t index;

    uint8_t tile_pal_setting;
    uint8_t tile_pal_setting_last;

    // Prime the palette testing in the loop
    tile_pal_setting_last = (*p_buf) / GBR_TILE_DATA_COLOR_SET_SIZE;


    // Unmap the palette from the color
    for (index = 0; index < tile_size; index++) {


        // Identify which color palette is used for the tile based
        // on the color's location in the indexed palette.
        // Palette is zero based
        // Example: Index color 13 = (13 / 4) = Palette 3
        tile_pal_setting = (*p_buf) / GBR_TILE_DATA_COLOR_SET_SIZE;

        // Return error and abort if...
        // * extracted palette num was out of bounds or
        // * the tile tried to use more than one palette
        if ((tile_pal_setting != tile_pal_setting_last)) {

            printf("Error: gbr_tile_palette_assign_and_strip(): Error, multiple palettes in single tile\n");
            return (false);
        }
        else {
            // Update palette for next pass
            tile_pal_setting = tile_pal_setting_last;
        }

        // Remap the palette so it's only colors 0-3, relative to the identified palette
        // Example: Index color 13 = (13 % 4) = Color 1 of Palette 3
        *p_buf = (*p_buf) % GBR_TILE_DATA_COLOR_SET_SIZE;

        // Move to the next pixel
        p_buf++;
    }

    // TODO: FIX ME CGB /DMG Detection handling
    // Check for Palette out of range
    if ((tile_pal_setting > GBR_TILE_DATA_PALETTE_SIZE_DMG) && (gb_mode == MODE_DMG_4_COLOR)) {

        printf("Error: gbr_tile_palette_assign_and_strip(): Palette # too large for DMG 4 color: %d\n", tile_pal_setting);
        return (false);

    } else if ((tile_pal_setting > GBR_TILE_DATA_PALETTE_SIZE_CGB) && (gb_mode == MODE_CGB_32_COLOR)) {

        printf("Error: gbr_tile_palette_assign_and_strip(): Palette # too large for CGB 16 color: %d\n",tile_pal_setting );
        return (false);
    }

    // Set palette number for tile
    // For CGB/DMG
    p_gbr->tile_pal.color_set[ (tile_index * GBR_TILE_PAL_COLOR_SET_REC_SIZE) ] = tile_pal_setting;
    // Set it for SGB too
    p_gbr->tile_pal.sgb_color_set[ (tile_index * GBR_TILE_PAL_COLOR_SET_REC_SIZE) ] = tile_pal_setting;

    return (true);
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














// TODO: FIXME this is making assumptions about bytes per pixel = 1 in both SOURCE and DEST buffers
int32_t gbr_tile_get_buf(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index) {

    int32_t offset;
    int32_t tile_size;

    // Find the start of the tile in the tile list buffer
    tile_size = p_gbr->tile_data.width * p_gbr->tile_data.height;
    //offset = (tile_size * (tile_index - 1));

    // TODO: should it be tile_index - 1? In this code tile_index is 0 based, but maybe not in the original pascal?
    offset = (tile_size * tile_index);

    // Make sure the destination buffer is ok
    if (!dest_buf)
        return false;

    // Make sure there is enough data for a complete tile in the source tile buffer
    if ((tile_size * tile_index) > p_gbr->tile_data.tile_data_size)
        return false;


    memcpy(dest_buf, &(p_gbr->tile_data.tile_list[offset]), tile_size);

    // Remap colors to correct palette (mostly for CGB/SGB mode)
    gbr_tile_remap_colors(dest_buf, p_gbr, tile_index, tile_size, GBR_MAP_TILE_PAL_OVERRIDE_NONE);

    return true;
}


// TODO: CGB MODE: move reprocessing of indexed color vs palette here tile_palette_remap_and_identify()

// Copy pixels from an image into a tile sized slice of the tile list buffer
// TODO: FIXME this is linear for now and assumes an 8 x N image
int32_t gbr_tile_set_buf(uint8_t * src_buf, gbr_record * p_gbr, uint16_t tile_index, uint16_t gb_mode) {

    int32_t offset;
    int32_t tile_size;

    // Find the start of the tile in the tile list buffer
    tile_size = p_gbr->tile_data.width * p_gbr->tile_data.height;
    //offset = (tile_size * (tile_index - 1));

    // TODO: should it be tile_index - 1? In this code tile_index is 0 based, but maybe not in the original pascal?
    offset = (tile_size * tile_index);

    // Make sure the destination buffer is ok
    if (!src_buf)
        return false;

    // Make sure there is enough data for a complete tile in the source tile buffer
    // TODO: better bounds checking here, matched to actual max tile size
    if ((tile_size * tile_index) > PASCAL_OBJECT_MAX_SIZE)
        return false;


    memcpy(&(p_gbr->tile_data.tile_list[offset]), src_buf, tile_size);

    // Remap the colors to 2bpp + palette index (and store palette in list)
    if (!gbr_tile_palette_assign_and_strip(&(p_gbr->tile_data.tile_list[offset]), p_gbr, tile_index, tile_size, gb_mode))
        return false;

/*

    printf("==gbr_tile_set_buf\n");
    int x;
    int y;
    for (y = 0; y < p_gbr->tile_data.height; y++) {
        for (x = 0; x < p_gbr->tile_data.width; x++) {
            printf("%4x", p_gbr->tile_data.tile_list[offset + x + (y * p_gbr->tile_data.width)]);
        }
        printf("\n");
    }
*/

    return true;
}



// Copy pixels from an image into a tile sized slice of the tile list buffer
int32_t gbr_tile_set_buf_padding(gbr_record * p_gbr, uint16_t tile_index) {

    int32_t offset;
    int32_t tile_size;

    // Find the start of the tile in the tile list buffer
    tile_size = p_gbr->tile_data.width * p_gbr->tile_data.height;
    offset = (tile_size * tile_index);

    // Make sure there is enough data for a complete tile in the source tile buffer
    // TODO: better bounds checking here, matched to actual max tile size
    if ((tile_size * tile_index) > PASCAL_OBJECT_MAX_SIZE)
        return false;

    // Fill the padding tile with zeros
    memset(&(p_gbr->tile_data.tile_list[offset]), 0x00, tile_size);
    return true;
}





// Expects
// * An image that is an even multiple of tile_data.width
// * Is sized based on map width in tiles x tile_size / same for height
// * p_image.bytes_per_pixel should be : 1 byte per pixel
//
// map_x, map_y: these are in tile coordinates! not pixel locations
int32_t gbr_tile_copy_to_image(image_data * p_image, gbr_record * p_gbr,
                               uint16_t tile_index,
                               uint16_t map_x, uint16_t map_y,
                               uint8_t flip_h, uint8_t flip_v,
                               uint8_t map_tile_pal_id) {

    int32_t tile_offset;
    int32_t tile_size;
    int32_t image_offset;
    int32_t image_copy_end;
    int16_t row;
    uint8_t tile_row[p_gbr->tile_data.width];
    // int16_t c;
    // uint8_t temp_pixel;


    // Find the start of the tile in the tile list buffer
    tile_size = p_gbr->tile_data.width * p_gbr->tile_data.height;

    tile_offset = (tile_size * tile_index);

    image_offset = (((map_y * p_gbr->tile_data.width) * p_image->width) +
                    (map_x * p_gbr->tile_data.width)) * p_image->bytes_per_pixel;

    // If vertical mirroring is enabled then begin at
    // the start of the last (bottom) row instead of the start of the first (top) row
    if (flip_v) {
        image_offset += (p_gbr->tile_data.height - 1) * (p_image->width * p_image->bytes_per_pixel);
    }

    image_copy_end = image_offset
                     + ((p_gbr->tile_data.height -1) * p_image->width * p_image->bytes_per_pixel)
                     + (p_gbr->tile_data.width * p_image->bytes_per_pixel);


    // Make sure the destination buffer is ok
    if (!p_image->p_img_data)
        return false;

    // Make sure there is enough data for a complete tile in the source tile buffer
    if ((tile_size * tile_index) > p_gbr->tile_data.tile_data_size)
        return false;

    // Make sure there is enough room in the destination image for the tile
    if (image_copy_end > p_image->size)
        return false;

    // TODO: support more than 1 BPP in destination images?
    if (p_image->bytes_per_pixel != 1)
        return false;


    // Copy each row of the tile to the desired row location in the image
    for(row = 0; row < p_gbr->tile_data.height; row++) {

        // Copy a row from source tile into tile working buffer
        memcpy(&tile_row[0],                                // Temp, later copied to -> Destination image
               &(p_gbr->tile_data.tile_list[tile_offset]),   // Source tile buffer
               p_gbr->tile_data.width);                      // Tile row stride

        // Remap colors to correct palette (mostly for CGB/SGB mode)
        gbr_tile_remap_colors(&tile_row[0],
                              p_gbr,
                              tile_index,
                              p_gbr->tile_data.width,
                              map_tile_pal_id);

        // Handle Horizontal flipping if needed
        if (flip_h)
            gbr_tile_row_mirror_horizontal( tile_row, p_gbr->tile_data.width);

        // Copy finished tile row into Destination image
        memcpy(&(p_image->p_img_data[image_offset]),         // Destination image
               &tile_row[0],                                 // From Source tile buffer
               p_gbr->tile_data.width);                      // Tile row stride

        // Advance to next tile and image row locations
        tile_offset += p_gbr->tile_data.width;

        // Handle Vertical mirroring
        // (decrement destination image buffer from end instead of increment from start)
        if (flip_v)
            image_offset -= p_image->width * p_image->bytes_per_pixel;
        else
            image_offset += p_image->width * p_image->bytes_per_pixel;

    }

    return true;
}