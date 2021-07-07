
#ifndef _COLOR_SPACE_H
#define _COLOR_SPACE_H

#include <math.h>
#include <stdint.h>

#include "image_info.h"

bool color_find_closest_LAB(palette_rgb_LAB *, color_rgb_LAB *, uint8_t *, double *);
bool color_find_exact_RGB(palette_rgb_LAB *, color_rgb_LAB *, uint8_t *);
bool color_find_closest(palette_rgb_LAB *, color_rgb_LAB *, uint8_t *, double *);

void color_rgb2LAB(color_rgb_LAB *);
double color_distance_LAB_CIE76(color_rgb_LAB *, color_rgb_LAB *);


#endif // _COLOR_SPACE_H