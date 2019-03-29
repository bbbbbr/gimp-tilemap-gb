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
int32_t gbm_load(const int8_t * filename);
int32_t gbm_load_file(const int8_t * filename);
int32_t gbm_save(const int8_t * filename, image_data * p_src_image, color_data * p_colors);
int32_t gbm_save_file(const int8_t * filename);


#endif // LIB_GBM_FILE_HEADER
