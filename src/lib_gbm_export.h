//
// lib_gbm_export.h
//

#include "lib_gbm.h"


int32_t gbm_object_producer_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_prop_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_data_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_default_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_settings_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_prop_colors_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_export_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_tile_data_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
