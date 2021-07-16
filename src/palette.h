
#ifndef _PALETTE_H
#define _PALETTE_H

#include "image_info.h"

void palette_copy_rgblab_to_colordata_format(color_data *, palette_rgb_LAB *);
void palette_copy_colordata_to_rgblab_format(color_data * p_src_colors, palette_rgb_LAB * p_dst_colors);
void palette_convert_to_lab(palette_rgb_LAB *);
bool palette_load_from_file(color_data *, char *);

#endif // _PALETTE_H
