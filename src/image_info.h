//
// image_info.h
//

#include <stdint.h>

#ifndef IMAGE_INFO_HEADER
#define IMAGE_INFO_HEADER

    #define COLOR_DATA_PAL_MAX_COUNT   256 // 255 Colors max for indexed
    #define COLOR_DATA_BYTES_PER_COLOR 3   // RGB 1 byte per color
    #define COLOR_DATA_PAL_SIZE COLOR_DATA_PAL_MAX_COUNT * COLOR_DATA_BYTES_PER_COLOR


    typedef struct {
        uint8_t    bytes_per_pixel;
        uint16_t   width;
        uint16_t   height;
        uint32_t   size;  // size in bytes
        uint8_t  * p_img_data;
    } image_data;


    typedef struct {
        uint16_t color_count;
        uint16_t size;
        uint8_t  pal[COLOR_DATA_PAL_SIZE];
    } color_data;


#endif
