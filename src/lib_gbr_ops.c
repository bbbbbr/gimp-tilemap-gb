//
// lib_gbr_ops.c
//

#include "lib_gbr_ops.h"


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



int32_t gbr_tile_get_buf(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index) {

    int32_t offset;
    int32_t tile_size;

    // Find the starting of the tile in the tile list buffer
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

    return true;
}


int32_t gbr_tile_set_buf(uint8_t * src_buf, gbr_record * p_gbr, uint16_t tile_index) {

    int32_t offset;
    int32_t tile_size;

    // Find the starting of the tile in the tile list buffer
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

