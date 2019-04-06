//
// lib_gbm_import.h
//

#include "lib_gbm.h"


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


