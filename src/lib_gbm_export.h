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
int32_t gbm_object_map_export_prop_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_tile_data_encode(gbm_record * p_gbm, gbm_file_object * p_obj);

int32_t gbm_object_map_deleted_1_encode(gbm_record * p_gbm, gbm_file_object * p_obj);
int32_t gbm_object_map_deleted_2_encode(gbm_record * p_gbm, gbm_file_object * p_obj);

int32_t gbm_export_set_defaults(gbm_record * p_gbm);
void gbm_export_update_color_set(gbm_record * p_gbm, uint16_t gb_mode);