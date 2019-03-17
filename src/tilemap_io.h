//
// tilemap_io.h
//

char * get_filename_from_path(char * path);
int32_t tilemap_export(const int8_t * filename, uint32_t export_format, tile_map_data * tile_map, tile_set_data * tile_set);

int32_t tilemap_export_raw_binary(const int8_t * filename, tile_map_data * tile_map, tile_set_data * tile_set);
