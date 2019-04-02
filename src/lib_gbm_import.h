//
// lib_gbm_import.h
//

#include "lib_gbm.h"

#include "lib_gbr.h"
#include "lib_gbr_ops.h"


int32_t gbm_object_producer_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_tile_data_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_prop_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_data_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_default_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_settings_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_colors_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_export_decode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_export_prop_decode(gbm_record * p_gbm, gbm_file_object * p_obj);

gbm_tile_record gbm_map_tile_get_xy(gbm_record * p_gbm, uint16_t x, uint16_t y);
int32_t gbm_convert_map_to_image(gbm_record * p_gbm, gbr_record * p_gbr, image_data * p_image);

void gbm_map_tiles_print(gbm_record * p_gbm);
