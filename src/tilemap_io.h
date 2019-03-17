//
// tilemap_io.h
//

#define STR_FILENAME_MAX 2048

const char * get_filename_from_path(const char * path);
int32_t get_path_without_filename(const char * path, char * path_only, uint32_t str_max);

int32_t tilemap_export(const int8_t * filename, uint32_t export_format, tile_map_data * tile_map, tile_set_data * tile_set);

int32_t tilemap_export_raw_binary(const int8_t * filename, tile_map_data * tile_map, tile_set_data * tile_set);
