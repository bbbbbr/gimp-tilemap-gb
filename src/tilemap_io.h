//
// tilemap_io.h
//

int32_t tilemap_export(const char * filename, uint32_t export_format, tile_map_data * tile_map, tile_set_data * tile_set);

int32_t tilemap_export_raw_binary(const char * filename, tile_map_data * tile_map, tile_set_data * tile_set);
