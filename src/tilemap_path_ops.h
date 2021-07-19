//
// tilemap_path_ops.h
//

#define STR_FILENAME_MAX 2048

const char * get_filename_from_path(const char * path);
int32_t get_path_without_filename(const char * path, char * path_only, uint32_t str_max);
void copy_filename_without_path_and_extension(char * path_out, const char * path_in);
void copy_filename_without_extension(char * path_out, const char * path_in);
bool matches_extension(char * filename, char * extension);
