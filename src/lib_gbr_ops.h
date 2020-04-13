//
// lib_gbr_ops.h
//

#include "lib_gbr.h"

#ifndef LIB_GBR_OPS_FILE_HEADER
#define LIB_GBR_OPS_FILE_HEADER

int32_t gbr_pal_get_buf(uint8_t * dest_buf, gbr_record * p_gbr);
int32_t gbr_tile_get_buf(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index);

void gbr_tile_row_mirror_horizontal( uint8_t tile_row[], int16_t row_width);
void gbr_tile_remap_colors(uint8_t * dest_buf, gbr_record * p_gbr, uint16_t tile_index, int32_t tile_size, uint8_t map_tile_pal_id);

int32_t gbr_tile_palette_identify_and_strip(uint8_t * p_buf, gbr_record * p_gbr, uint16_t tile_index, int32_t tile_size, uint16_t gb_mode);

int32_t gbr_pal_set_buf(uint8_t * src_buf, gbr_record * p_gbr, uint16_t num_colors);
int32_t gbr_tile_set_buf(uint8_t * src_buf, gbr_record * p_gbr, uint16_t tile_index, uint16_t gb_mode);
int32_t gbr_tile_set_buf_padding(gbr_record * p_gbr, uint16_t tile_index);

int32_t gbr_tile_copy_to_image(image_data * p_image, gbr_record * p_gbr, uint16_t tile_index, uint16_t map_x, uint16_t map_y, uint8_t flip_h, uint8_t flip_v, uint8_t map_tile_pal_id);

#endif // LIB_GBR_OPS_FILE_HEADER