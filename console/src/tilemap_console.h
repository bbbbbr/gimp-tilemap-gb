
#ifndef TILEMAP_ACTIONS_C
#define TILEMAP_ACTIONS_C

#include "image_info.h"

int tilemap_load_image(image_data * p_src_image, color_data * p_src_colors, const char * filename);
int tilemap_process_and_save_image(image_data * p_src_image, color_data * p_src_colors, const char * filename);


#endif