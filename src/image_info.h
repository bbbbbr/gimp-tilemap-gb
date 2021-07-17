//
// image_info.h
//

#include <stdint.h>

#ifndef IMAGE_INFO_HEADER
#define IMAGE_INFO_HEADER

    #define MODE_8_BIT_INDEXED        ( 8 / 8)
    #define MODE_8_BIT_INDEXED_ALPHA  (16 / 8)
    #define MODE_24_BIT_RGB           (24 / 8)
    #define MODE_32_BIT_RGBA          (32 / 8)

    #define COLOR_DATA_PAL_MAX_COUNT   256 // 255 Colors max for indexed
    #define COLOR_DATA_BYTES_PER_COLOR 3   // RGB 1 byte per color
    #define COLOR_DATA_PAL_SIZE COLOR_DATA_PAL_MAX_COUNT * COLOR_DATA_BYTES_PER_COLOR

    #define USER_PAL_MAX_COLORS  32


    typedef struct {
        uint8_t    bytes_per_pixel;
        uint16_t   width;
        uint16_t   height;
        uint32_t   size;        // size in bytes
        uint8_t  * p_img_data;
        uint16_t   tile_width;  // should be even multiple of width 
        uint16_t   tile_height; // should be even multiple of height

        uint16_t   palette_tile_width;  // should be even multiple of width 
        uint16_t   palette_tile_height; // should be even multiple of height
    } image_data;


    typedef struct {
        uint16_t color_count;
        uint16_t size;
        uint8_t  pal[COLOR_DATA_PAL_SIZE]; // TODO: make this a union for better access: .r / .g / .b, merge in <-- palette_rgb_LAB
        uint8_t  subpal_size;              // should be even multiple of color_count
    } color_data;

    typedef struct {
        uint8_t r,g,b; // RGB color space
        double  L,A,B; // LAB color space
    } color_rgb_LAB;

    typedef struct {
        int           color_count;
        color_rgb_LAB colors[USER_PAL_MAX_COLORS];
        uint8_t       subpal_size; // should be even multiple of color_count
        uint8_t       compare_start; // zero based
        uint8_t       compare_last;  // zero based
    } palette_rgb_LAB;


#endif
