//
// tilemap_tiles.h
//

void           tile_free(tile_data * p_tile);
void           tile_copy_tile_from_image(image_data * p_src_img, tile_data * tile, uint32_t img_buf_offset);
tile_map_entry tile_find_match(tile_data * p_tile, tile_set_data * tile_set, tile_map_data * p_tile_map);
int32_t        tile_encode(tile_data * p_tile, uint32_t image_mode);
tile_map_entry tile_register_new(tile_data * src_tile, tile_set_data * tile_set);
void           tile_initialize(tile_data * p_tile, tile_map_data * p_tile_map, tile_set_data * p_tile_set);

void            tile_palette_reapply_offsets(tile_data * p_tile);
int32_t         tile_palette_identify_and_strip(tile_data * p_tile, uint16_t gb_mode);

void           tile_calc_alternate_hashes(tile_data *, tile_data []);
void           tile_flip_x(tile_data * p_src_tile, tile_data * p_dst_tile);
void           tile_flip_y(tile_data * p_src_tile, tile_data * p_dst_tile);

// TODO: delete me
void tile_print_buffer_raw(tile_data tile);
void tile_print_buffer_encoded(tile_data tile);

