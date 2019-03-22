//
// lib_gbr_ops.c
//

#include "lib_gbr_ops.h"

int32_t gbr_pal_get_buf(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t pal_index) {

    int32_t offset;

    // Find the starting of the tile in the tile list buffer
    offset = (GBR_PALETTE_SIZE_4_COLORS_XBGR * (pal_index - 1));

    // Make sure the destination buffer is ok
    if (!dest_buf)
        return false;

    // Make sure there is enough data for a complete tile in the source tile buffer
    if ((GBR_PALETTE_SIZE_4_COLORS_XBGR * pal_index) > GBR_PALETTE_COLOR_SETS_SIZE)
      return false;


    memcpy(dest_buf,
           &(p_gbr->palettes.colors[offset]),
           GBR_PALETTE_SIZE_4_COLORS_XBGR);

}



int32_t gbr_tile_get_buf(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index) {

    int32_t offset;
    int32_t tile_size;

    // Find the starting of the tile in the tile list buffer
    tile_size = p_gbr->tile_data.width * p_gbr->tile_data.height;
    offset = (tile_size * (tile_index - 1));

    // Make sure the destination buffer is ok
    if (!dest_buf)
        return false;

    // Make sure there is enough data for a complete tile in the source tile buffer
    if ((tile_size * tile_index) > p_gbr->tile_data.data_size)
      return false;


    memcpy(dest_buf, &(p_gbr->tile_data.tile_list[offset]), tile_size);

}


