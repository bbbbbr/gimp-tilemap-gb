//
// tilemap_io.c
//

#include <stdio.h>
#include <string.h>

#include "lib_tilemap.h"
#include "tilemap_io.h"

#include "tilemap_format_gbdk_c_source.h"


int32_t tilemap_export(const char * filename,
                       uint32_t   export_format,
                       tile_map_data * tile_map,
                       tile_set_data * tile_set) {

    // tilemap_export_raw_binary(filename, tile_map, tile_set);

    if (export_format == FORMAT_GBDK_C_SOURCE)
        tilemap_format_gbdk_c_source_save(filename, tile_map, tile_set);

    return (true);
}



int32_t tilemap_export_raw_binary(const char * filename,
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

        // printf("* Writing tile %d of %d : %d bytes\n", c +1, tile_set->tile_count, tile_set->tiles[c].encoded_size_bytes);

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

    return true;
}