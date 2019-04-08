//
// tilemap_path_ops.h
//

#define STR_FILENAME_MAX 2048

const char * get_filename_from_path(const char * path);
int32_t get_path_without_filename(const char * path, char * path_only, uint32_t str_max);

