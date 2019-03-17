//
// tilemap_tiles.c
//

void           tile_copy_tile_from_image(image_data * p_src_img, tile_data * tile, uint32_t img_buf_offset);
int32_t        tile_find_matching(uint64_t hash_sig, tile_set_data * tile_set);
int32_t        tile_encode(tile_data * p_tile, uint32_t image_mode);
int32_t        tile_register_new(tile_data * src_tile, tile_set_data * tile_set);

// TODO: delete me
static void tile_print_buffer_raw(tile_data tile);
static void tile_print_buffer_encoded(tile_data tile);

