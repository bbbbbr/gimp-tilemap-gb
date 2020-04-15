//
// lib_gbm_ops.c
//

#include "lib_tilemap.h" // TODO: This is only here for things like : TILE_FLIP_BITS_X, find a better way to handle that
#include "lib_gbm_ops.h"


// TODO: remove once debugging is complete
void gbm_map_tiles_print(gbm_record * p_gbm) {

    uint16_t x, y;
    uint16_t index;

    gbm_tile_record tile;

    index = 0;

    for (y=0; y < p_gbm->map.height; y++) {
        for (x=0; x < p_gbm->map.width; x++) {

            tile = gbm_map_tile_get_xy(p_gbm, x, y);
            index++;

            printf(" %3d", tile.num);
            if ((index % p_gbm->map.width) == 0) printf("\n");
        }
    }

    printf("\n");
}


// TODO: remove once debugging is complete
void gbm_map_tiles_flip_print(gbm_record * p_gbm) {

    uint16_t x, y;
    uint16_t index;

    gbm_tile_record tile;

    index = 0;

    for (y=0; y < p_gbm->map.height; y++) {
        for (x=0; x < p_gbm->map.width; x++) {

            tile = gbm_map_tile_get_xy(p_gbm, x, y);
            index++;

            if (tile.flip_h | tile.flip_v)
                printf(" %3d", tile.flip_h | tile.flip_v); // Flip x/y
            else
                printf("   ."); // Default, no flip
            if ((index % p_gbm->map.width) == 0) printf("\n");
        }
    }

    printf("\n");
}


// TODO: remove once debugging is complete
void gbm_map_tiles_pal_print(gbm_record * p_gbm) {

    uint16_t x, y;
    uint16_t index;

    gbm_tile_record tile;

    index = 0;

    for (y=0; y < p_gbm->map.height; y++) {
        for (x=0; x < p_gbm->map.width; x++) {

            tile = gbm_map_tile_get_xy(p_gbm, x, y);
            index++;

            if (tile.pal_cgb_id)
                printf(" %3d", tile.pal_cgb_id); // Override palette
            else
                printf("   ."); // Default, no palette override
            if ((index % p_gbm->map.width) == 0) printf("\n");
        }
    }

    printf("\n");
}



gbm_tile_record gbm_map_tile_get_xy(gbm_record * p_gbm, uint16_t x, uint16_t y) {

    uint16_t index;

    gbm_tile_record map_tile;

    index = (x + (y * p_gbm->map.width)) * GBM_MAP_TILE_RECORD_SIZE;

    if ((index + 2) > p_gbm->map_tile_data.length_bytes) {
        map_tile.num = 0xFFFF;
        return map_tile;  // TODO: use/signal proper failure return code here
    }

    map_tile.flip_h = p_gbm->map_tile_data.records[index] & GBM_MAP_TILE_FLIP_H_BYTE;
    map_tile.flip_v = p_gbm->map_tile_data.records[index] & GBM_MAP_TILE_FLIP_V_BYTE;

    map_tile.pal_cgb_id    = (p_gbm->map_tile_data.records[index+1] >> GBM_MAP_TILE_PAL_CGB_BITSHIFT) & GBM_MAP_TILE_PAL_CGB_BYTE;
    map_tile.pal_noncgb_id =  p_gbm->map_tile_data.records[index]                                     & GBM_MAP_TILE_PAL_NONCGB_BYTE;

    map_tile.num = ((uint16_t)p_gbm->map_tile_data.records[index+2] |
               ((uint16_t)p_gbm->map_tile_data.records[index+1] >> 8)) & GBM_MAP_TILE_NUM;

        //printf("gbm_map_tile_get_xy() at %d , %d: map_tile.num=%d\n", x,y, map_tile.num);
        // printf(" %3d", map_tile.num);
        // if (((x + 1) % p_gbm->map.width) == 0) printf("\n");

    return map_tile;

}



uint32_t gbm_map_tile_set_xy(gbm_record * p_gbm, uint16_t x, uint16_t y, uint16_t tile_index, uint8_t flip_bits, uint8_t palette_num) {

    uint32_t index;

    index = (x + (y * p_gbm->map.width)) * GBM_MAP_TILE_RECORD_SIZE;

    if ((index + 2) > p_gbm->map_tile_data.length_bytes) {
        return false;  // TODO: use/signal proper failure return code here
        printf("gbm_map_tile_set_xy: FAILED: %d, %d \n", x,y);
    }

    // Set Tile Number
    p_gbm->map_tile_data.records[index + 1] = (uint8_t)((tile_index & GBM_MAP_TILE_NUM) << 8);
    p_gbm->map_tile_data.records[index + 2] = (uint8_t) (tile_index & GBM_MAP_TILE_NUM);

    // Below assumes that proper feature enable/disable has occured for DMG vs CGB
    // such as: if (gb_mode == MODE_CGB_32_COLOR)

    if (flip_bits & TILE_FLIP_BITS_X)
        p_gbm->map_tile_data.records[index] |= GBM_MAP_TILE_FLIP_H_BYTE;

    if (flip_bits & TILE_FLIP_BITS_Y)
        p_gbm->map_tile_data.records[index] |= GBM_MAP_TILE_FLIP_V_BYTE;

    // Set CGB Palette if enabled
    if (palette_num == TILE_PAL_MAP_USE_DEFAULT_FROM_TILE) {
        p_gbm->map_tile_data.records[index + 1] |= ((GBM_MAP_TILE_PAL_CGB_DEFAULT & GBM_MAP_TILE_PAL_CGB_BYTE) << GBM_MAP_TILE_PAL_CGB_BITSHIFT);

    } else {
        // Actual palette number is +1 from value since 0 indicates default
        p_gbm->map_tile_data.records[index + 1] |= (((palette_num + GBM_MAP_TILE_PAL_CGB_OFFSET) & GBM_MAP_TILE_PAL_CGB_BYTE) << GBM_MAP_TILE_PAL_CGB_BITSHIFT);
    }

    // Set Non-CGB Palette
    // TODO: For now, always forced to Palette Zero
    p_gbm->map_tile_data.records[index] |= (GBM_MAP_TILE_PAL_NONCGB_DEFAULT & GBM_MAP_TILE_PAL_NONCGB_BYTE);

    // printf("gbm_map_tile_set_xy() at %4d, %4d: index = %3d, pal = %3d (254=default)\n", x,y, tile_index, palette_num);
    // if ((x % p_gbm->map.width) == 0) printf("\n");

    return true;
}



int32_t gbm_convert_map_to_image(gbm_record * p_gbm, gbr_record * p_gbr, image_data * p_image) {

    uint16_t map_x, map_y;
    gbm_tile_record tile;
    int32_t status;
    uint8_t map_tile_pal_id;

    status = true; // default to success

    // Init image
    p_image->bytes_per_pixel = 1; // TODO: MAKE a #define

    p_image->width  = p_gbm->map.width * p_gbr->tile_data.width;
    p_image->height = p_gbm->map.height * p_gbr->tile_data.height;

    // Calculate total image area based on
    // tile width and height, and number of tiles
    p_image->size = p_image->width * p_image->height * p_image->bytes_per_pixel;

    printf("gbm_convert_map_to_image(): %d x %d @ size: %d\n", p_image->width,
                                                             p_image->height,
                                                             p_image->size);

    // Allocate image buffer, free if needed beforehand
    if (p_image->p_img_data)
        free (p_image->p_img_data);

    p_image->p_img_data = malloc(p_image->size);


    // Create the image from tiles
    if (p_image->p_img_data) {

        // Loop through tile map, get tile number and copy respective tiles into image
        for (map_y=0; map_y < p_gbm->map.height; map_y++) {
            for (map_x=0; map_x < p_gbm->map.width; map_x++) {

                tile = gbm_map_tile_get_xy(p_gbm, map_x, map_y);

                if (p_gbm->map_settings.color_set == gbm_color_set_gbc)
                    map_tile_pal_id = tile.pal_cgb_id;
                else
                    map_tile_pal_id = tile.pal_noncgb_id;

                if (status)
                    status = gbr_tile_copy_to_image(p_image,
                                                    p_gbr,
                                                    tile.num,
                                                    map_x, map_y,
                                                    tile.flip_h, tile.flip_v,
                                                    map_tile_pal_id);
            } // for .. map_x
        } // for.. map_y
    } else {
        // Signal failure if image pointer not allocated
        status = false;
    }

    printf("\n");

    // Return success
    return status;
}


int32_t gbm_convert_tilemap_buf_to_map(gbm_record * p_gbm, uint16_t * p_map_tile_ids, uint8_t * p_map_flip_bits, uint8_t * p_map_palette_nums, uint32_t map_data_count) {

    uint16_t map_x, map_y;

    printf("gbm_convert_image_to_map: %dx%d @ size=%d\n", p_gbm->map.width,
                                                          p_gbm->map.height,
                                                          map_data_count);

    // Bounds check for map data and counters
    if ((p_gbm->map.width * p_gbm->map.height) > map_data_count)
        return false;

    // Copy the tile data into the file_file image from tiles
    if (p_map_tile_ids) {

        // Loop through tile map, get tile number and copy respective tiles into image
        for (map_y=0; map_y < p_gbm->map.height; map_y++) {
            for (map_x=0; map_x < p_gbm->map.width; map_x++) {

                    // Set the map tile
                    if (! gbm_map_tile_set_xy(p_gbm, map_x, map_y,
                                             *(p_map_tile_ids++), // map tile index
                                             *(p_map_flip_bits++), // map tile flip data
                                             *(p_map_palette_nums++)) // map tile palette data
                        )
                        return false;
            }
        }
    } else
        // Signal failure if image pointer not allocated
        return false;

    // Return success
    return true;
}
