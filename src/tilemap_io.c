//
// tilemap_io.c
//

#include <stdio.h>
#include <string.h>

#include "lib_tilemap.h"
#include "tilemap_io.h"

#include "tilemap_format_gbdk_c_source.h"


// TODO: fix up const / int8_t mixing
const char * get_filename_from_path(const char * path)
{
    size_t i;

   for(i = strlen(path) - 1; i; i--) {
        if (path[i] == '/') {
            return &path[i+1];
        }
    }
    return path;
}

int32_t get_path_without_filename(const char * path, char * path_only, uint32_t str_max)
{
    size_t i;

   if (strlen(path) + 1 > str_max)
        return false;

   for(i = strlen(path) - 1; i; i--) {
        if (path[i] == '/') {

            memcpy(path_only, path, i+1 );
            path_only[i+2] = '\0';
            return true;
        }
    }

    memcpy(path_only, path, strlen(path));
    return true;
}



int32_t tilemap_export(const int8_t * filename,
                       uint32_t   export_format,
                       tile_map_data * tile_map,
                       tile_set_data * tile_set) {

    // tilemap_export_raw_binary(filename, tile_map, tile_set);

    if (export_format == EXPORT_FORMAT_GBDK_C_SOURCE)
        tilemap_format_gbdk_c_source_save(filename, tile_map, tile_set);

    return (true);
}


// TODO: should this be uint8_t vs int8_t?
int32_t tilemap_export_raw_binary(const int8_t * filename,
                               tile_map_data * tile_map,
                               tile_set_data * tile_set) {

    int c;
    FILE * file;

    char filename_bin[255];
    snprintf(filename_bin, 255, "%s.bin", filename);

    // Open the file
    file = fopen(filename_bin, "wb");
    if(!file)
        return (false);

    // Write all the tile set data to a file
    for (c = 0; c < tile_set->tile_count; c++) {

printf("* Writing tile %d of %d : %d bytes\n", c +1, tile_set->tile_count, tile_set->tiles[c].encoded_size_bytes);

        if (tile_set->tiles[c].p_img_encoded) {

// tile_print_buffer_encoded(tile_set->tiles[c]);

            fwrite(tile_set->tiles[c].p_img_encoded,
                   tile_set->tiles[c].encoded_size_bytes,
                   1, file);
        }

printf("OUTPUT tile %d\n", c);
// tile_print_buffer_raw(tile_set->tiles[c]);

            // TODO: hex output encoding
            //if (tile_set->tiles[c].p_img_raw)
    }

    fclose(file);

}