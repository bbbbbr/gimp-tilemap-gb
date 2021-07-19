// image_load.c

//
// Handles loading PNG images and reformatting them to a usable state
//

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "logging.h"
#include "image_info.h"
#include "options.h"

#include "lodepng.h"


static void image_unbitpack(uint8_t *, uint8_t *, uint8_t, long);
static void image_greyscale_to_rgb(uint8_t *, uint8_t *, long, bool );
static void image_copy_out(uint8_t *, image_data *, unsigned int, uint8_t, long);
static bool image_validate_type(unsigned int, uint8_t, uint8_t *);


// Un-bitpacks a source image into an 8-bit-per-pixel destination buffer
static void image_unbitpack(uint8_t * p_src_image, uint8_t * p_dest_image, uint8_t bitdepth, long src_size_bytes) {

    int pixels_per_byte;
    int c, b;

    pixels_per_byte = (8 / bitdepth);

    for (c = 0; c < src_size_bytes; c++) {

        for (b = 0; b < pixels_per_byte; b++) {

            *p_dest_image = *p_src_image >> (8 - bitdepth); // Extract pixel bits
            p_dest_image++; // Move to next pixel in destination image
            *p_src_image <<= bitdepth; // // Mask out the just extracted bits in the source image

        }
        // move to next (byte) group of packed pixels in the source image
        p_src_image++;
    }
}


// Converts a source greyscale image into a RGB image, strips alpha if present
// loaded png image may be bitpacked. So instead, unpack it.
static void image_greyscale_to_rgb(uint8_t * p_src_image, uint8_t * p_dest_image, long src_size_bytes, bool is_alpha) {

    int c;

    for (c = 0; c < src_size_bytes; c++) {
        *p_dest_image++ = *p_src_image; // -> R
        *p_dest_image++ = *p_src_image; // -> G
        *p_dest_image++ = *p_src_image; // -> B

        p_src_image++; // Next source pixel

        // Skip alpha byte if present
        if (is_alpha)
            p_src_image++;
    }
}


// Converts a source image from various formats into a usable output image
static void image_copy_out(uint8_t * p_src_image, image_data * p_dest_image, unsigned int png_colortype, uint8_t png_bitdepth, long src_size_bytes) {

        // If image is bitpacked (or just indexed) then extract it
        if (png_colortype == LCT_PALETTE) {
            image_unbitpack(p_src_image,
                            p_dest_image->p_img_data,
                            png_bitdepth,
                            src_size_bytes);
        }
        else if ((png_colortype == LCT_GREY) || (png_colortype == LCT_GREY_ALPHA)) {

            image_greyscale_to_rgb(p_src_image, p_dest_image->p_img_data, 
                                   (p_dest_image->width * p_dest_image->height),
                                   (png_colortype == LCT_GREY_ALPHA));
        }
        else {
            // Copy png image data to decoded image as-is
            memcpy(p_dest_image->p_img_data, p_src_image, src_size_bytes);
        }
}


// TODO: move into other file
// Check if image mode is supported
// Lodepng image Types
//    LCT_GREY = 0,        // greyscale: 1,2,4,8,16 bit
//    LCT_RGB = 2,         // RGB: 8,16 bit
//    LCT_PALETTE = 3,     // palette: 1,2,4,8 bit
//    LCT_GREY_ALPHA = 4,  // greyscale with alpha: 8,16 bit
//    LCT_RGBA = 6         // RGB with alpha: 8,16 bit
static bool image_validate_type(unsigned int png_colortype, uint8_t png_bitdepth, uint8_t * p_output_bytes_per_pixel) {

    bool status = true;

    if (png_colortype == LCT_PALETTE) {

        // Indexed color images can only be 1 - 8 bits
        if ((png_bitdepth < 1) || (png_bitdepth > 8)) {
            status = false;
            log_error("Error: Decoded image must be between 1 and 8 bits per pixel (pngtype: %d, bits:%d)\n", png_colortype, png_bitdepth);
        }

        *p_output_bytes_per_pixel = MODE_8_BIT_INDEXED;
    } else {


        if ((png_colortype == LCT_GREY_ALPHA) || (png_colortype == LCT_RGBA)) {
            log_standard("Warning: PNG has alpha mask, discarding. Formerly (semi-)transparent pixels may have unexpected values.\n");
        } 

        if ((png_colortype == LCT_RGBA)) {
            *p_output_bytes_per_pixel = MODE_32_BIT_RGBA;
        } 
        else if ((png_colortype == LCT_GREY) || (png_colortype == LCT_GREY_ALPHA))
            *p_output_bytes_per_pixel = MODE_24_BIT_RGB; // greyscale/alpha will get converted to 24 bit RGB
        else
            *p_output_bytes_per_pixel = MODE_24_BIT_RGB;

        if (png_bitdepth != 8) {
                status = false;
                log_error("Error: Non-indexed images must have 8 bits per color component (pngtype: %d, bits:%d)\n", png_colortype, png_bitdepth);
        }
    }

    return status;
}



// Loads a PNG image
// Performs bit packing or other necessary conversion
// with result buffer assigned to image_data parameter
int image_load(image_data * p_decoded_image, color_data * p_src_colors, char * filename) {

    bool            status = true;
    unsigned int    err, c;
    unsigned char * p_png = NULL;
    unsigned char * p_png_image = NULL;
    unsigned int    width, height;
    long            png_image_size_bytes = 0;
    unsigned int    color_count = 0;
    size_t          png_size;
    LodePNGState    state;

    log_standard("Loading image from file: %s\n", filename);

    // Initialize png library and load the image file
    lodepng_state_init(&state);
    lodepng_load_file(&p_png, &png_size, filename);

    // Decode it with conversion disabled
    state.decoder.color_convert = 0;
    err = lodepng_decode(&p_png_image, &width, &height, &state, p_png, png_size);
    free(p_png);


    // Fail on errors
    if (err) {
        status = false;
        log_error("Error: PNG load: %u: %s\n", err, lodepng_error_text(err));
    }

    if (status) {
        // Check color depth and indexed / rgb mode types
        // Retrieves: bytes per pixel
        status = image_validate_type(state.info_png.color.colortype,
                                     state.info_png.color.bitdepth,
                                     &(p_decoded_image->bytes_per_pixel));
    }

    if (status) {       

        if (state.info_png.color.colortype == LCT_PALETTE)
            png_image_size_bytes = (width * height) / ( 8 / state.info_png.color.bitdepth);  // May be bit-packed
        else
            png_image_size_bytes = (width * height) * p_decoded_image->bytes_per_pixel;  // 24 or 32 bit rgb


        // == IMAGE PROPERTIES from decoded PNG ==
        // Determine the array size for the app's image then allocate it
        // hardwire bpp since tilemap processing only supports 8-bit indexed color
        p_decoded_image->width      = width;
        p_decoded_image->height     = height;
        p_decoded_image->size       = p_decoded_image->width * p_decoded_image->height * p_decoded_image->bytes_per_pixel;
        p_decoded_image->p_img_data = (uint8_t *)malloc(p_decoded_image->size);  // lodepng_decode() handles allocation

        if (!p_decoded_image->p_img_data) {
            status = false;
            log_error("Failed to allocate decoded image buffer\n");
        }

        // Copy image to output buffer
        // Reformat (such as un-bitpack) if needed
        image_copy_out(p_png_image, p_decoded_image,
                       state.info_png.color.colortype, state.info_png.color.bitdepth,
                       png_image_size_bytes);

        log_verbose("p_decoded_image->width:%d\n", p_decoded_image->width);
        log_verbose("p_decoded_image->height:%d\n", p_decoded_image->height);
        log_verbose("p_decoded_image->size:%d\n", p_decoded_image->size);
        log_verbose("p_decoded_image->bytes_per_pixel:%d\n", p_decoded_image->bytes_per_pixel);

        log_verbose("colortype:%d\n", state.info_raw.colortype);
        log_verbose("bitdepth:%d\n", state.info_raw.bitdepth);

        // == COLOR DATA ==
        // Load color palette info and data
        p_src_colors->color_count = state.info_png.color.palettesize;

        // Load colors, stripping out the Alpha component of the RGBA palette
        for (c=0; c < p_src_colors->color_count; c++){
            p_src_colors->pal[c * 3    ] = (unsigned char)state.info_png.color.palette[c*4];   /* R */
            p_src_colors->pal[c * 3 + 1] = (unsigned char)state.info_png.color.palette[c*4+1]; /* G */
            p_src_colors->pal[c * 3 + 2] = (unsigned char)state.info_png.color.palette[c*4+2]; /* B */

            log_verbose("png PAL: (%3d) = %02x, %02x, %02x\n", c, 
                p_src_colors->pal[c * 3    ],
                p_src_colors->pal[c * 3 + 1],
                p_src_colors->pal[c * 3 + 2]);
        }
    }

    // Free resources
    lodepng_state_cleanup(&state);

    return status;
}
