//
// lib_gbr_import.h
//

#include "lib_gbr.h"

int32_t gbr_object_producer_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_tile_data_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_tile_settings_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_tile_export_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_tile_import_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_palettes_decode(gbr_record * p_gbr, gbr_file_object * p_obj);
int32_t gbr_object_tile_pal_decode(gbr_record * p_gbr, gbr_file_object * p_obj);

int32_t gbr_convert_tileset_to_image(gbr_record * p_gbr, image_data * p_image, color_data * p_colors);
int32_t gbr_load_tileset_palette(color_data * p_colors, gbr_record * p_gbr);
