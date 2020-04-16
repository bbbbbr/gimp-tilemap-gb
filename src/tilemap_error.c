//
// tilemap_error.c
//
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tilemap_error.h"
#include "lib_tilemap.h"


uint16_t tilemap_error_status;

// Note: must match: enum order of tile_id_status
const char * tile_id_messages[] = {
        "No Errors",
        "No Matching Tile found (non-error)",
        "Error: Memory allocation failed",
        "Error: Tile count exceeded limit",
        "Error: Failed to encode tile",
        "Error: Color count in tile exceeded DMG or CGB Limit",
        "Error: More than one 4-color-adjacent palette used in tile",
        "Error: Invalid image dimensions. Must be multiples of tile size"
    };


void tilemap_error_clear(void) {
    tilemap_error_status = TILE_ID_OK;
}


void tilemap_error_set(uint16_t error_num) {
    tilemap_error_status = error_num;
}


uint16_t tilemap_error_get(void) {
    return tilemap_error_status;
}


const char * tilemap_error_get_string(void) {
    return tile_id_messages[tilemap_error_status];
}