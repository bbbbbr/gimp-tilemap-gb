//
// lib_gbm_ops.h
//

#include "lib_gbm.h"

#include "lib_gbr.h"
#include "lib_gbr_ops.h"


gbm_tile_record gbm_map_tile_get_xy(gbm_record * p_gbm, uint16_t x, uint16_t y);
uint32_t gbm_map_tile_set_xy(gbm_record * p_gbm, uint16_t x, uint16_t y, uint16_t tile_index, uint16_t tile_attribs);

int32_t gbm_convert_map_to_image(gbm_record * p_gbm, gbr_record * p_gbr, image_data * p_image);
int32_t gbm_convert_tilemap_buf_to_map(gbm_record * p_gbm, uint8_t * p_map_data, uint16_t * p_map_attribs, uint32_t map_data_count);