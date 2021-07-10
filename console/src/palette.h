
#ifndef _PALETTE_H
#define _PALETTE_H

#include "image_info.h"

void palette_copy(color_data *, palette_rgb_LAB *);
void palette_convert_to_lab(palette_rgb_LAB *);
bool palette_load_from_file(palette_rgb_LAB *, char *);

#endif // _PALETTE_H
