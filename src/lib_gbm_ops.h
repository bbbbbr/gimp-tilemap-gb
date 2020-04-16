//
// lib_gbm_ops.h
//

#include "lib_gbm.h"

#include "lib_gbr.h"
#include "lib_gbr_ops.h"


gbm_tile_record gbm_map_tile_get_xy(gbm_record * p_gbm, uint16_t x, uint16_t y);
uint32_t gbm_map_tile_set_xy(gbm_record * p_gbm, uint16_t x, uint16_t y, uint16_t tile_index, uint8_t flip_bits, uint8_t palette_num);

int32_t gbm_convert_map_to_image(gbm_record * p_gbm, gbr_record * p_gbr, image_data * p_image);
int32_t gbm_convert_tilemap_buf_to_map(gbm_record * p_gbm, uint16_t * p_map_tile_ids, uint8_t * p_map_flip_bits, uint8_t * p_map_palette_nums, uint32_t map_data_count);

int32_t gbr_tile_palette_assign_and_strip(uint8_t * p_buf, gbr_record * p_gbr, uint16_t tile_index, int32_t tile_size, uint16_t gb_mode);

void gbm_map_tiles_print(gbm_record * p_gbm);
void gbm_map_tiles_flip_print(gbm_record * p_gbm);
void gbm_map_tiles_pal_print(gbm_record * p_gbm);

