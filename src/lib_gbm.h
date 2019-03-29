//
//   lib_gbm.h
//

// Reference :
// * https://github.com/bashaus/gbtiles
// * http://www.devrs.com/gb/hmgd/gbtd.html


#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "image_info.h"

#ifndef LIB_GBM_FILE_HEADER
#define LIB_GBM_FILE_HEADER

enum gbm_object_types {
    gbm_obj_producer     = 0x01,
    gbm_obj_map          = 0x02,
    gbm_obj_tile_data    = 0x03,
    gbm_obj_map_prop     = 0x04,
    gbm_obj_prop_data    = 0x05,
    gbm_obj_prop_default = 0x06,
    gbm_obj_map_settings = 0x07,
    gbm_obj_prop_colors  = 0x08,
    gbm_obj_map_export   = 0x09,
    gbm_obj_deleted      = 0xFFFF,
};


int32_t gbm_load(const int8_t * filename);
int32_t gbm_load_file(const int8_t * filename);
int32_t gbm_save(const int8_t * filename, image_data * p_src_image, color_data * p_colors);
int32_t gbm_save_file(const int8_t * filename);


#endif // LIB_GBM_FILE_HEADER
